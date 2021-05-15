/*-------------------------------------------------------*/
/* util/bbsmail.c       ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 由 Internet 寄信給 BBS 站內使用者            */
/* create : 95/03/29                                     */
/* update : 03/05/02 (Dopin)                             */
/*-------------------------------------------------------*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <sysexits.h>

#include "bbs.h"
#include "cache.c"
#include "record.c"

#define LOG_FILE        (BBSHOME "/mailog")

/* Dopin: for bad_hosts from stuff.c */
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

/* Dopin: spam mail check */
int is_spam(char *str) {
  char *ptr = str;

  /* Dopin: 壞的 From --> 擋掉 */
  if(belong(BBSHOME "/innd/bad_froms", ptr)) return 1;

  if(ptr = strchr(str, '@')) {
    int ci;

    ptr++; /* 有人問我為啥不用 strcat ? 萬一溢位怎辦 ?_? 所以我堅持這樣 :p */
    for(ci = 0 ; (unsigned char)ptr[ci] > ' ' && ci < 127 ; ci++) ;
    ptr[ci] = 0;

    /* Dopin: 壞的 Host Name --> 擋掉 */
    if(belong(BBSHOME "/innd/bad_hosts", ptr)) return 1;
    /* Dopin: 過長的 From --> 有問題 --> 擋掉 */
    if(ci == 127) return 1;
  }

  return 0;
}

void mailog(char *msg) {
  FILE *fp;

  if(fp = fopen(LOG_FILE, "a+")) {
    time_t now;
    struct tm *p;

    time(&now);
    p = localtime(&now);
    fprintf(fp, "%02d/%02d/%02d %02d:%02d:%02d <bbsmail> %s\n",
            p->tm_year, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min,
            p->tm_sec, msg);
    fclose(fp);
  }
}

void strip_ansi(char *buf, char *str) {
  register int ch, ansi;

  for (ansi = 0; ch = *str; str++)
  {
    if (ch == '\n')
    {
      break;
    }
    else if (ch == 27)
    {
      ansi = 1;
    }
    else if (ansi)
    {
      if (!strchr("[01234567;", ch))
        ansi = 0;
    }
    else
    {
      *buf++ = ch;
    }
  }
  *buf = '\0';
}

int dashd(char *fname) {
  struct stat st;
  return (stat(fname, &st) == 0 && S_ISDIR(st.st_mode));
}

// Dopin: from --> stuff.c
int dashf(char *fname) {
  struct stat st;
  return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}

int mail2bbs(char *userid) {
  fileheader mymail;
  char genbuf[512], title[256], sender[256], *ip, *ptr;
  char fname[200], fnamepgp[200], pgppath[200];
  char firstline[100];
  struct stat st;
  time_t tmp_time;
  FILE *fout, *fpgp;
  int pgp = 0;

  /* check if the userid is in our bbs now */
  if(!searchuser(userid)) {
    sprintf(genbuf, "BBS user <%s> not existed", userid);
    puts(genbuf);
    mailog(genbuf);
    return EX_NOUSER;
  }

  sprintf(genbuf, "home/%s", userid);

  if(stat(genbuf, &st) == -1) {
    if(mkdir(genbuf, 0755) == -1) {
      sprintf(genbuf, "BBS user <%s> no mail box", userid);
      puts(genbuf);
      mailog(genbuf);
      return -1;
    }
  }
  else if(!(st.st_mode & S_IFDIR)) {
    sprintf(genbuf, "BBS user <%s> mail box error", userid);
    puts(genbuf);
    mailog(genbuf);
    return -1;
  }

#ifdef  DEBUG
  printf("dir: %s\n", genbuf);
#endif

  /* allocate a file for the new mail */
  stampfile(genbuf, &mymail);

#ifdef  DEBUG
  printf("file: %s\n", genbuf);
#endif

  /* copy the stdin to the specified file */
  if((fout = fopen(genbuf, "w")) == NULL) {
    printf("Cannot open file <%s>\n", genbuf);
    return -1;
  }

  sprintf(fnamepgp, "%s.pgp", strcpy(fname, genbuf));
  sprintf(pgppath, "home/%s/pgp", userid);

  /* parse header */
  title[0] = sender[0] = '\0';

  while(fgets(genbuf, sizeof(genbuf), stdin)) {
    if(!strncmp(genbuf, "From", 4)) {
      if((ip = strchr(genbuf, '<')) && (ptr = strrchr(ip, '>'))) {
        if(ip[-1] == ' ') ip[-1] = '\0';

        if(strchr(++ip, '@')) *ptr = '\0';
        else strcpy(ptr, "@" MYHOSTNAME); /* 由 local host 寄信 */

        ptr = (char *) strchr(genbuf, ' ');
        while (*++ptr == ' ');
        sprintf(sender, "%s (%s)", ip, ptr);
      }
      else {
        strtok(genbuf, " \t\n\r");
        strcpy(sender, (char *) strtok(NULL, " \t\n\r"));

        /* 由 local host 寄信 */
        if(strchr(sender, '@') == NULL) strcat(sender, "@" MYHOSTNAME);
      }
      continue;
    }

    if(!strncmp(genbuf, "Subject: ", 9)) {
      strip_ansi(title, genbuf + 9);
      continue;
    }

    if(genbuf[0] == '\n') break;
  }

  time(&tmp_time);

  if(fgets(genbuf, sizeof(genbuf), stdin)) {
    strncpy(firstline, genbuf, 99);
    firstline[99] = 0;
  }

  {
    char s_buf[128];

    /* Dopin: 一不做二不休 使用者拒收境外信也一起來吧 QQ;;; */
    sprintf(s_buf, "home/%s/reject_imail", userid);
    if(dashf(s_buf)) goto IS_SPAM;

    strncpy(s_buf, sender, 127);
    s_buf[127] = 0;
    if(is_spam(s_buf)) {
IS_SPAM:
       fclose(fout);
       return EX_NOUSER;
    }
  }

  if(!title[0]) sprintf(title, "來自 %.64s", sender);

  fprintf(fout, "作者: %s\n標題: %s\n時間: %s\n", sender, title,
          ctime(&tmp_time));

  if(strstr(genbuf, "BEGIN PGP MESSAGE")
     && dashd(pgppath)
     && (fpgp = fopen(fnamepgp, "w"))) {
    fputs(genbuf, fpgp);
    while(fgets(genbuf, sizeof(genbuf), stdin)) fputs(genbuf, fpgp);
    fclose(fpgp);
    fclose(fout);

    sprintf(genbuf, "PGPPATH=%s;PGPPASSFD=0;export PGPPATH PGPPASSFD;\
echo `cat home/%s/pgp.phrase` | /usr/local/bin/pgp +batchmode -o- %s >> %s",
            pgppath, userid, fnamepgp, fname);
    system(genbuf);
     unlink(fnamepgp);
     pgp = 1;
     mymail.filemode |= FILE_MARKED;
  }
  else {
     fputs(genbuf, fout);
     while(fgets(genbuf, sizeof(genbuf), stdin)) fputs(genbuf, fout);
     fclose(fout);
  }

  sprintf(genbuf, "%s => %s%s", sender, userid, pgp ? " [PGP]" : "");
  mailog(genbuf);

  /* append the record to the MAIL control file */
  strncpy(mymail.title, title, 72);

  if(strtok(sender, " .@\t\n\r")) strcat(sender, ".");
  sender[IDLEN + 1] = '\0';
  strcpy(mymail.owner, sender);

  sprintf(genbuf, "home/%s/.DIR", userid);
  return append_record(genbuf, &mymail, sizeof(mymail));
}

int main(int argc, char *argv[]) {
  char receiver[256];

  /* argv[1] is userid in bbs   */
  if(argc < 2) {
    printf("Usage:\t%s <bbs_uid>\n", argv[0]);
    exit(-1);
  }

  strncpy(receiver, argv[1], 255);
  receiver[255] = 0;

#if 0
  setreuid( BBSUID, BBSUID );
  setregid( BBSGID, BBSGID );
#endif

  setgid(BBSGID);
  setuid(BBSUID);
  chdir(BBSHOME);

  if(mail2bbs(receiver)) {
    /* eat mail queue */
    while (fgets(receiver, sizeof(receiver), stdin));
    /* exit(-1); */
  }
  exit(0);

  return 0;
}
