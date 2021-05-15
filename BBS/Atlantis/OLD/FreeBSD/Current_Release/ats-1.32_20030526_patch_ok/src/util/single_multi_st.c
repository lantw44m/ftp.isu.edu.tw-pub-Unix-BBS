/*-----------------------------------------------------*/
/* util/single_multi_st.c     (SOB 0.22 ATS Ver. 1.32) */
/*-----------------------------------------------------*/
/* target: 秀出 .BOARDS 中各看板權限                   */
/* create: 11/14/02 by Dopin                           */
/* update:                                             */
/*-----------------------------------------------------*/

#include "bbs.h"

#define ORG_SINGLE  /* 原本為單站台則定義 反之反定義 */
#define ORG_LEAST    /* 原先使用最小使用者資料結構則定義 反之反定義 */
#undef  ORG_COLA     /* 原先定義為可樂轉換則定義 反之則否 */

#define SRC_PASSWD BBSHOME "/.PASSWDS"
#define TAR_PASSWD BBSHOME "/PASSWDS.TRANS"

struct oldrec {
  char userid[IDLEN + 1];
  char realname[20];
  char username[24];
  char passwd[PASSLEN];
  uschar uflag;
  usint userlevel;
  unsigned long int numlogins;
  unsigned long int numposts;
  time_t firstlogin;
  time_t lastlogin;
  char lasthost[80];
  char remoteuser[8];
  char email[50];
  char address[50];
  char justify[REGLEN + 1];
  uschar month;
  uschar day;
  uschar year;
  uschar sex;
  uschar state;

  int havemoney;

#ifndef ORG_LEAST
  int havetoolsnumber;
  int havetools[20];
  int addexp;
  usint nowlevel;
  char working[20];

  uschar hp;
  uschar str;
  uschar mgc;
  uschar skl;
  uschar spd;
  uschar luk;
  uschar def;
  uschar mdf;

  uschar spcskl[6];
  uschar wepnlv[2][10];
  uschar curwepnlv[2][1];
  uschar curwepnatt;
  uschar curwepnhit;
  uschar curwepnweg;
  uschar curwepnspc[4];

  char lover[IDLEN+1];
  char commander;
  char belong[21];
  char curwepclass[10];
  char class[7];
#endif

#ifndef ORG_SINGLE
  char station[IDLEN+1];
#endif

#ifndef ORG_LEAST
  char classsex;
  char wephavespc[5];

  char cmdrname[IDLEN+1];
  char class_spc_skll[6];
#endif

  usint welcomeflag;

#ifndef ORG_LEAST
  int win;
  int lost;
  int test_exp;
#endif

  char tty_name[20];
  char extra_mode[10];

#ifndef ORG_LEAST
  char class_spc_test[32];
  char mov;
  char toki_level;

  int will_value;
  int effect_value;
  int belive_value;
  int leader_value;

  char action_value;
#endif

#ifndef ORG_SINGLE
  char station_member[40];
#endif

  uschar now_stno;
  usint good_posts;

#ifdef ORG_COLA
  usint staytime;
  int backup_int[43];
#else
  int backup_int[44];
#endif

#ifndef ORG_LEAST
  int ara_money;
#endif
#ifdef ORG_COLA
  char blood;
  char backup_char[119];
#else
  char backup_char[120];
#endif

#ifndef ORG_LEAST
  int turn;
#endif
};
typedef struct oldrec oldrec;

void trans_ou2nu(oldrec *o, userec *n) {
  int i;

  memset(n, 0, sizeof(userec));

  strcpy(n->userid, o->userid);
  strcpy(n->realname, o->realname);
  strcpy(n->username, o->username);
  strcpy(n->passwd, o->passwd);

  n->uflag = o->uflag;
  n->userlevel = o->userlevel;
  n->numlogins = o->numlogins;
  n->numposts = o->numposts;
  n->firstlogin = o->firstlogin;
  n->lastlogin = o->lastlogin;

  strcpy(n->lasthost, o->lasthost);
  for(i = 0 ; i < 8 ; i++) n->remoteuser[i] = o->remoteuser[i];
  strcpy(n->email, o->email);
  strcpy(n->address, o->address);
  strcpy(n->justify, o->address);

  n->month = o->month;
  n->day = o->day;
  n->year = o->day;
  n->sex = o->sex;
  n->state = o->state;
  n->welcomeflag = o->welcomeflag;

  n->good_posts = o->good_posts;
  n->havemoney = o->havemoney;
}

int check_ou_name(oldrec *u) {
   int i, j;

   i = strlen(u->userid);
   if(i > IDLEN || i < 2) return -1;

   if(!isalpha(u->userid[0])) return 1;

   for(j = 0 ; j < i ; j++)
      if(!isalpha(u->userid[j]) && !isdigit(u->userid[j])) return -1;

   return 0;
}

int main(int argc, int argv) {
   int i, j;
   FILE *fs, *ft;
   oldrec src;
   userec tar;

   fs = fopen(SRC_PASSWD, "rb");
   if(!fs) {
      printf("Open Source File %s Error ...\n", SRC_PASSWD);
      return -1;
   }

   ft = fopen(TAR_PASSWD, "wb+");
   if(!ft) {
      fclose(fs);
      printf("Open Target File %s Error ...\n", TAR_PASSWD);
      return -1;
   }

   i = j = 0;
   while(fread(&src, sizeof(src), 1, fs) == 1) {
      if(!check_ou_name(&src)) {
         trans_ou2nu(&src, &tar);
         fwrite(&tar, sizeof(tar), 1, ft);
         i++;
      }
      j++;
   }

   fclose(fs);
   fclose(ft);

   printf("USER Record Size of Origin %d bytes / Current %d bytes\n",
          sizeof(oldrec), sizeof(userec));
   printf("Trans %d / %d Records\n", i, j);

   return 0;
}
