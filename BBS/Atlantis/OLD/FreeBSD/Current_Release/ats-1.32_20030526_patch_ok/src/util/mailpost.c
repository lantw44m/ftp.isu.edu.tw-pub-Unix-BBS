/*-------------------------------------------------------*/
/* util/mailpost.c      ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : (1) general user E-mail post 到看板          */
/*          (2) BM E-mail post 到精華區                  */
/*          (3) 自動審核身份認證信函之回信               */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include        <stdio.h>
#include        <ctype.h>
#include        <sys/stat.h>
#include        <sys/file.h>
#include        <fcntl.h>
#include        <time.h>

#include "record.c"
#include "cache.c"

userec record;
char myfrom[128], mysub[128], myname[128], mypasswd[128], myboard[128],
     mytitle[128], mydigest[128];

#define JUNK            0
#define NET_SAVE        1
#define LOCAL_SAVE      2

int mymode = JUNK;

#define LOG_FILE        "mailog"

/* ----------------------------------------------------- */
/* buffered I/O for stdin                                */
/* ----------------------------------------------------- */

#define POOL_SIZE       8192
#define LINE_SIZE       256
#define MAXHEADER       50

char pool[POOL_SIZE];
char mybuf[LINE_SIZE];
char header[MAXHEADER + 1][LINE_SIZE];
int pool_size = POOL_SIZE;
int pool_ptr = POOL_SIZE;
int headerline = 0;

int readline(char *buf) {
  register char ch;
  register int len, bytes;

  len = bytes = 0;

  do {
    if(pool_ptr >= pool_size) {
      if(pool_size = fread(pool, 1, POOL_SIZE, stdin)) pool_ptr = 0;
      else break;
    }

    ch = pool[pool_ptr++];
    bytes++;

    if(ch == '\r') continue;
    buf[len++] = ch;
  } while (ch != '\n' && len < (LINE_SIZE - 1));

  buf[len] = '\0';
  if(headerline >= 0 && headerline < MAXHEADER)
     strcpy(header[headerline++], buf);

  return bytes;
}

/* ----------------------------------------------------- */
/* record mailog for management                      */
/* ----------------------------------------------------- */

void mailog(char *mode, char *msg) {
  FILE *fp;

  if(fp = fopen(LOG_FILE, "a+")) {
    time_t now;
    struct tm *p;

    time(&now);
    p = localtime(&now);
    fprintf(fp, "%02d/%02d/%02d %02d:%02d:%02d <%s> %s\n",
            p->tm_year+1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min,
            p->tm_sec, mode, msg);
    fclose(fp);
  }
}

void str_lower(char *t, char *s) {
  register uschar ch;

  do {
    ch = *s++;
    *t++ = (ch >= 'A' && ch <= 'Z') ? ch | 32 : ch;
  } while (ch);
}

/* Dopin: for black/while mailsrv from stuff.c */
int belong(char *filelist, char *key) {
  FILE *fp;
  char *str_space         = " \t\n\r";
  int rc = 0;

  if(fp = fopen(filelist, "r")) {
    char buf[STRLEN], *ptr;

    while(fgets(buf, STRLEN, fp)) {
      if((ptr = strtok(buf, str_space)) && !strcasecmp(ptr, key)) {
        rc = 1;
        break;
      }
    }

    fclose(fp);
  }

  return rc;
}

int valid_ident(char *ident) {
  static char *invalid[] = { "unknown@", "root@", "gopher@", "bbs@",
    "@bbs", "guest@", "@ppp", "@slip", NULL };
  char buf[128], buf2[128], *ptr;
  int i;

  str_lower(buf, ident);
  strcpy(buf2, buf);
  for(i = 0; invalid[i]; i++) if(strstr(buf, invalid[i])) return 0;

  if((ptr = strchr(buf2, '@')) != NULL) {
    if(belong(BBSHOME "/etc/black_mailsrv", ptr)) return 0;
    if(belong(BBSHOME "/etc/white_mailsrv", ptr)) return 1;
  }

  return 1;
}

int get_userec(int usrnum, userec *record) {
  int fh;

  if((fh = open(fn_passwd, O_RDWR)) == -1)
    printf(":Err: unable to open %s file.\n", fn_passwd);
  else {
    lseek(fh, (usrnum - 1) * sizeof(userec), SEEK_SET);
    read(fh, record, sizeof(userec));
  }

  return fh;
}

/* ------------------------------------------------------- */
/* 記錄驗證資料：user 有可能正在線上，所以寫入檔案以保周全 */
/* ------------------------------------------------------- */

void justify_user() {
  FILE *fp;
  char buf[80];

  sprintf(buf, "home/%s/email", record.userid);

  /* Dopin: by sagit.bbsrs@blue.ccsh.kh.edu.tw USE "w+" instead */
  if(fp = fopen(buf, "w+")) {
    fprintf(fp, "%s\n", myfrom);
    fclose(fp);
  }
  strncpy(record.justify, myfrom, REGLEN);

  /* Let userlevel opend by register.c: check_register() */
  /* record.userlevel |= PERM_LOGINOK; */
}

junk() {
  FILE* fp;
  int i;
  char msgbuf[256];


  sprintf(msgbuf, "[%s]%s => %s", record.userid, myfrom, myboard);
  mailog("mailpost", msgbuf);
  /* Dopin: by sagit.bbsrs@blue.ccsh.kh.edu.tw USE "w+" instead */
  if(fp = popen("bin/bbspost junk > /dev/null 2>&1", "w+")) {
     for(i = 0; i < headerline; i++) fputs(header[i], fp);
     headerline = -1;
     while(readline(mybuf)) fputs(mybuf, fp);
     pclose(fp);
  }
  return 0;
}

verify_user(char *magic) {
  char *ptr, *next, ch, buf[80];
  ushort checksum;
  int usrnum, fh, sd = 0;
  FILE *fp;

  if(ptr = (char *) strchr(magic, '(')) {
    *ptr++ = '\0';

    sprintf(buf, "%s/home/%s/MAGIC_IDENT", BBSHOME, magic);
    if((fp = fopen(buf, "r")) != NULL) {
      fgets(buf, 79, fp);
      for(sd = 0 ; sd < 79 && (uschar)buf[sd] > ' ' ; sd++) ;
      buf[sd] = 0;

      sd = atoi(buf);
      fclose(fp);
    }

    if(next = (char *) strchr(ptr, ':')) {
      *next++ = '\0';
      usrnum = atoi(ptr) - MAGIC_KEY;
      if(ptr = (char *) strchr(next, ')')) {
        *ptr++ = '\0';
        checksum = atoi(next);

        if((fh = get_userec(usrnum-sd, &record)) == -1) return;

        if(!ci_strcmp(magic, record.userid)) {
          ptr = myfrom;         /* record.email; */
          while (ch = *ptr++) {
            if(ch <= ' ') break;
            if(ch >= 'A' && ch <= 'Z') ch |= 0x20;
            usrnum = (usrnum << 1) ^ ch;
          }

          if(((ushort)usrnum == checksum) && valid_ident(myfrom)) {
            justify_user();
            lseek(fh, -(off_t)sizeof(record), SEEK_CUR);
            write(fh, &record, sizeof record);
            sprintf(mybuf, "[%s]%s", record.userid, myfrom);
            mailog("verify", mybuf);
            close(fh);
            return;
          }
        }
        close(fh);
      }
    }
  }
}

int mailpost() {
  int uid, fh, dirty;
  char *ip, *ptr;
  char *token, *key;

  /* parse header */

  readline(mybuf);
  if(strncasecmp(mybuf, "From", 4)) return junk();

  dirty = *myfrom = *mysub = *myname = *mypasswd = *myboard = *mytitle =
  *mydigest = 0;

  while(!*myname || !*mypasswd || !*myboard || !*mytitle) {
    if(mybuf[0] == '#') {
      key = mybuf + 1;

      /* skip trailing space */
      if(ptr = strchr(key, '\n')) {
        do {
          *ptr = '\0';
          fh = *(--ptr);
        } while (fh == ' ' || fh == '\t');
      }

      /* split token & skip leading space */
      if(token = strchr(key, ':')) {
        *token = '\0';

        do {
          fh = *(++token);
        } while (fh == ' ' || fh == '\t');
      }

      if(!ci_strcmp(key, "name")) strcpy(myname, token);
      else if(!ci_strcmp(key, "passwd") || !ci_strcmp(key, "password") ||
              !ci_strcmp(key, "passward")) strcpy(mypasswd, token);
      else if(!ci_strcmp(key, "board")) strcpy(myboard, token);
      else if(!ci_strcmp(key, "title") || !ci_strcmp(key, "subject"))
        strcpy(mytitle, token);
      else if(!ci_strcmp(key, "digest")) strcpy(mydigest, token);
      else if(!ci_strcmp(key, "local")) mymode = LOCAL_SAVE;
    }
    else if(!strncasecmp(mybuf, "From", 4)) {
      str_lower(myfrom, mybuf + 4);

      if(strstr(myfrom, "mailer-daemon"))
        /* woju */ /* return junk(); */
        return;

      if((ip = strchr(mybuf, '<')) && (ptr = strrchr(ip, '>'))) {
        *ptr = '\0';
        if(ip[-1] == ' ') ip[-1] = '\0';

        ptr = (char *) strchr(mybuf, ' ');
        while (*++ptr == ' ');
        sprintf(myfrom, "%s (%s)", ip + 1, ptr);
      }
      else {
        strtok(mybuf, " ");
        strcpy(myfrom, (char *) strtok(NULL, " "));
      }
    }
    else if(!strncmp(mybuf, "Subject: ", 9))
    {
      /* audit justify mail */
      if(ptr = strstr(mybuf, "[SOB BBS]To ")) {
        verify_user(ptr + 12);
        return -1;
      }
      if(ptr = strchr(token = mybuf + 9, '\n')) *ptr = '\0';
      strcpy(mysub, token);
    }

    if((++dirty > MAXHEADER) || !readline(mybuf)) break;
  }
  return junk();
}

int main(void) {
  chdir(BBSHOME);
  setgid(BBSGID);
  setuid(BBSUID);

  /* eat mail queue */
  if(mailpost()) while (fread(pool, 1, POOL_SIZE, stdin)) ;

  return 0;
}
