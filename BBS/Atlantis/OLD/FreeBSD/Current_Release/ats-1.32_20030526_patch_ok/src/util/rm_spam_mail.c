/*-------------------------------------------------------*/
/* util/rm_spam_mail.c         ( Atlantis BBS Ver 1.32 ) */
/*-------------------------------------------------------*/
/* target : �̷өU���H�����ݪO�R���ϥΪ̫H�c���U���H     */
/* create : 03/05/10 (Dopin)                             */
/* update :   /  /                                       */
/*-------------------------------------------------------*/
/* syntex : rm_spam_mail (as bbsrsadm)                   */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"

#define PASSFILE BBSHOME "/.PASSWDS"
#define SPAMBOARD "SPAM_Mail"        /* �U���H�����ݪO�W �Ш̶Q���ݭn�ק� */
#define SPAMDIR   BBSHOME "/boards/" SPAMBOARD "/.DIR"
#define MAXRECORD 1024               /* Dopin: �̦hŪ�J 1024 �ʩU���H��� */

#define SAFE_DEL
/* Dopin: �Ш̶Q�����p�ק� �p���Ů� ���i�ٲ� �J�L���ܴN�令�Ŧr�� "" */
#define TRANS "[���]"

typedef struct {
  char id[IDLEN+1];
  char title[TTLEN];
} condition;

union fc {
  struct {
    fileheader fh;
  } F;

  struct {
    char buf[128];
  } C;
};
typedef union fc fc;

condition cdn[MAXRECORD];
char us[MAXUSERS][IDLEN+1];

int get_condition_by_board(void) {
  int i = 0;
  fileheader fh;
  FILE *fp;

  if((fp = fopen(SPAMDIR, "rb")) == NULL) return -1;
  while(fread(&fh, sizeof(fh), 1, fp) == 1) {
    if(fh.filemode & FILE_MARKED) {  /* Dopin: ���Q mark �_�Ӥ~��@�w�B�z */
      char *ptr;

      strncpy(cdn[i].id, fh.owner, IDLEN);
      cdn[i].id[IDLEN] = 0;
      strncpy(cdn[i].title, fh.title, TTLEN-(strlen(TRANS)+1));
      cdn[i].title[TTLEN-strlen(TRANS)] = 0;

      i++;
      if(i == MAXRECORD) break;
    }
  }
  fclose(fp);

  return i;
}

int get_user_by_passwd(void) {
  int i = 0;
  userec u;
  FILE *fp;

  if((fp = fopen(PASSFILE, "rb")) == NULL) return -1;
  while(fread(&u, sizeof(u), 1, fp) == 1) {
    strcpy(us[i], u.userid);
    i++;
    if(i == MAXUSERS) break;
  }
  fclose(fp);

  return i;
}

int check_bad_id(char *id) {
  int i, j = 0;

  /* Dopin: �h�{�ҫH�� ���� (��M�A�n�Ӭ�]�i�H QQ Remark �o��Y�i) */
  if(strcmp(id, "����")) return 0; /* �����G�r�� ATS ���� �Ш̶Q�����p��� */

  for(i = 0 ; i < IDLEN ; i++) {
    if((uschar)id[i] <= ' ') break;
    if(!(isalpha(id[i]) || isdigit(id[i]) || id[i] == '.')) {
      j = 1;
      break;
    }
  }

  return j;
}

int check_bad_title(char *title) {
  int i;
  char buf[TTLEN];

  strncpy(buf, title, TTLEN-1);
  buf[TTLEN-1] = 0;

  for(i = 0 ; i < TTLEN ; i++) {
     if((uschar)buf[i] < ' ') break;
     buf[i] = tolower(buf[i]);
  }

  /* Dopin: �o��i�H�u���ܧ� */
  if(strstr(buf, "g5?"))      return 1;
  if(strstr(buf, "fw:"))      return 1;
  if(strstr(buf, "fw :"))     return 1;
  if(strstr(buf, "big5"))     return 1;
  if(strstr(buf, "so-8859"))  return 1;
  if(strstr(buf, "�Ӧ�"))     return 1;
  if(strstr(buf, "�L�X"))     return 1;
  if(strstr(buf, "�ۼx"))     return 1;
  if(strstr(buf, "�Q����"))   return 1;
  if(strstr(buf, "�u�@��"))   return 1;
  if(strstr(buf, "�W���ȿ�")) return 1;
  if(strstr(buf, "�n�d�i�D")) return 1;
  if(strstr(buf, "������"))   return 1;
  if(strstr(buf, "������"))   return 1;

  return 0;
}

int check_spam(fileheader *pfh, int spam_record, char *userid) {
  int i, j = 0, trans = strlen(TRANS);

#ifdef SAFE_DEL
  /* BBS �ƥ����� */
  if(!strcmp(pfh->owner, "[��.��.��]") || !strcmp(pfh->owner, "[SYSTEM]"))
    return 0;
#endif

#ifndef SAFE_DEL
  if(!strcmp(pfh->owner, "RECOVER") return 1;
#endif

  for(i = 0 ; i < spam_record ; i++)
    if(trans ?
#ifdef SAFE_DEL
       !strncmp(&cdn[i].title[strlen(TRANS)], pfh->title, TTLEN-strlen(TRANS))
     : !strncmp(pfh->owner, cdn[i].id, IDLEN) &&
       strncmp(pfh->title, cdn[i].title, TTLEN-1)
#else
       strstr(&cdn[i].title[strlen(TRANS)], pfh->title)
     : !strncmp(pfh->owner, cdn[i].id, IDLEN) ||
        strncmp(pfh->title, cdn[i].title, TTLEN-1)
#endif
     || check_bad_id(pfh->owner) || check_bad_title(pfh->title)) {
      /* Dopin: ��ܥX [�ϥΪ̫H�c] �U���H�ӷ� ID �U���H�ӷ����D */
      printf("[%s] %s %s\n", userid, pfh->owner, pfh->title);
      j = 1;
      break;
    }

  return j;
}

int main(int argc, char *argv[]) {
  char src[MAXPATHLEN], tar[MAXPATHLEN], tmp[MAXPATHLEN];
  int user_num, spam_num, i, j, k = 0;
  fc fh;       /* Dopin: ���I�h���@�|(�����ŧi fileheader ��i) ���ڰ��� :p */
  int fd;      /* Dopin: ��@ file index �ҥH�t�~�ŧi */
  FILE *ft;

  spam_num = get_condition_by_board();
  user_num = get_user_by_passwd();

  printf("User Records is = %d / Spam Mail Records is %d\n", user_num,
         spam_num);
  if(!user_num || !spam_num) return -1; /* Dopin: �S������ ���ΰ��� */

  for(i = 0 ; i < user_num ; i++) {
    sprintf(src, "%s/home/%s/.DIR", BBSHOME, us[i]);
    /* ������ �p�G�]�ɰ��b�@�ӨϥΪ̥ؿ��ܤ[�ܤ[ ���ˬd�ӥؿ� .DIR �O�_�l�a */
    puts(src);

    sprintf(tar, "%s/home/%s/.DIR.check", BBSHOME, us[i]);
    /* �S���n���� �ڹ�b�����w(�]���i :p) �Q�� open/close �o���F�F�Ӻ��@�ɮ� */
    if((fd = open(src, O_RDONLY, 0)) == -1) continue;
    if((ft = fopen(tar, "wb+")) == NULL) {
      close(fd);
      continue;
    }

    flock(fd, LOCK_EX);  /* Dopin: ����ɮ� �H���w�� */
    while(read(fd, &fh.C.buf, sizeof(fh)) == sizeof(fh)) {
      if(!check_spam(&fh.F.fh, spam_num, us[i]))
        fwrite(&fh.F.fh, sizeof(fh), 1, ft);
      else {
        /* �R���U���H�� */
        printf("%s\n", fh.F.fh.title);
        sprintf(tmp, "%s/home/%s/%s", BBSHOME, us[i], fh.F.fh.filename);
        unlink(tmp);
      }
    }
    flock(fd, LOCK_UN);

    close(fd);
    fclose(ft);
    Rename(tar, src);
  }

  return 0;
}
