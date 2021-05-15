/*-------------------------------------------------------*/
/* util/sob2ats131.c           ( Atlantis BBS Ver 1.31 ) */
/*-------------------------------------------------------*/
/* target : 轉換原先 陽光沙灘 BBS 各項資料               */
/* create : 02/07/20 (Dopin)                             */
/* update : 02/09/17                                     */
/*-------------------------------------------------------*/
/* syntex : sob2ats131 (as root in usr/home)             */
/*-------------------------------------------------------*/

#include "bbs.h"

#define  RMHOLES

#define  OLDBBSHOME     "/home/bbs"
#define  NEWBBSHOME     "/home/bbsrs"
#define  NEWBBSUSER     "bbsrs"

#define  OLD_PERM_POSTMASK     0200000

struct oldrec
{
  char userid[IDLEN + 1];
  char realname[20];
  char username[24];
  char passwd[PASSLEN];
  uschar uflag;
  usint userlevel;
  ushort numlogins;
  ushort numposts;
  time_t firstlogin;
  time_t lastlogin;
  char lasthost[16];
  char remoteuser[8];
  char email[50];
  char address[50];
  char justify[REGLEN + 1];
  uschar month;
  uschar day;
  uschar year;
  uschar sex;
  uschar state;
};
typedef struct oldrec oldrec;

void copy_ou2nu(oldrec *ou, userec *nu) {
  int i;

  memset(nu, 0, sizeof(userec));

  strcpy(nu->userid, ou->userid);
  strcpy(nu->realname, ou->realname);
  strcpy(nu->username, ou->username);

  for(i = 0 ; i < PASSLEN ; i++)
     nu->passwd[i] = ou->passwd[i];
  nu->uflag = ou->uflag;
  nu->userlevel = ou->userlevel;
  nu->numlogins = (unsigned long int)ou->numlogins;
  nu->numposts = (unsigned long int)ou->numposts;
  nu->firstlogin = ou->firstlogin;
  nu->lastlogin = ou->lastlogin;
  strcpy(nu->lasthost, ou->lasthost);
  for(i = 0 ; i < 8 ; i++)
     nu->remoteuser[i] = ou->remoteuser[i];
  strcpy(nu->email, ou->email);
  strcpy(nu->address, ou->address);
  strcpy(nu->justify, ou->justify);
  nu->month = ou->month;
  nu->day = ou->day;
  nu->year = ou->year;
  nu->sex = ou->sex;
  nu->state = ou->state;

#ifndef NO_USE_MULTI_STATION
  for(i = 0; i < 40 ; i++) nu->station_member[i] = 0;
#endif
}

struct oldbd
{
  char brdname[IDLEN + 1];
  char title[BTLEN + 1];
  char BM[IDLEN * 3 + 3];
  char pad[11];
  time_t bupdate;
  char pad2[3];
  uschar bvote;
  time_t vtime;
  usint level;
};
typedef struct oldbd oldbd;

int check_name(char mode, char *str) {
  int i, j, flag;

  i = strlen(str);
  if(i < 2 || i > IDLEN) return -1;

  for(j = 0 ; j < i ; j++) {
     flag = 0;
     if(str[j] <= ' ' || (unsigned char)str[j] > 126)
        return -2;
      if(isdigit(str[j]))
         flag = 'C';
      else if(isalpha(str[j]))
         flag = 'D';
      else if(mode == 'B' &&
              (str[j] == '.' || str[j] == '-' || str[j] == '_'))
      flag = 'R';

      if(!flag) return -3;
  }

  return 0;
}

void copy_ob2nb(oldbd *ob, boardheader *nb) {
  int i;

  memset(nb, 0, sizeof(boardheader));

  strcpy(nb->brdname, ob->brdname);
  strcpy(nb->title, ob->title);
  strcpy(nb->BM, ob->BM);

  for(i = 0 ; i < 11 ; i++)
     nb->pad[i] = ob->pad[i];
   nb->bupdate = ob->bupdate;
   for(i = 0 ; i < 3 ; i++)
      nb->pad2[i] = ob->pad2[i];

   nb->bvote = ob->bvote;
   nb->vtime = ob->vtime;
   nb->level = ob->level;

   if(nb->level & OLD_PERM_POSTMASK) {
      nb->level &= 0xFFFFFFFF & OLD_PERM_POSTMASK;
      nb->level |= PERM_POSTMASK;
   }
   strcpy(nb->station, DEFAULTST);
}

int main(void) {
   char buf[100];
   int i, j;
   FILE *fs, *ft;
   oldrec ou;
   userec nu;
   oldbd ob;
   boardheader nb;

   strcpy(buf, NEWBBSHOME "/src/maple/shutdownbbs > /dev/null 2>&1");
   system(buf);

   fs = fopen(OLDBBSHOME "/.PASSWDS", "rb");
   if(!fs) return -1;
   printf("Opening Source PASSWDS File\n");
   ft = fopen(NEWBBSHOME "/.PASSWDS", "wb+");
   if(!ft) {
      fclose(fs);
      return -1;
   }
   printf("Opening Target PASSWDS File\n");

   i = j = 0;
   while(fread(&ou, sizeof(ou), 1, fs)) {
      if(ferror(fs) || feof(fs))
         break;
      if(!check_name('U', ou.userid)) {
         copy_ou2nu(&ou, &nu);
         fwrite(&nu, sizeof(nu), 1, ft);
         i++;

         sprintf(buf, "cp -r " OLDBBSHOME "/home/%s " NEWBBSHOME "/home/",
                 ou.userid);
         system(buf);
      }
      j++;
   }
   fclose(fs);
   fclose(ft);
   printf("Trans %d User Records (Origin %d)\n\n", i, j);

   fs = fopen(OLDBBSHOME "/.BOARDS", "rb");
   if(!fs) return -1;
   printf("Opening Source BOARDS File\n");
   ft = fopen(NEWBBSHOME "/.BOARDS", "wb+");
   if(!ft) return -1;
   printf("Opening Target BOARDS File\n");

   strcpy(buf, "rm -rf " NEWBBSHOME "/boards");
   system(buf);
   strcpy(buf, "mkdir " NEWBBSHOME "/boards");
   system(buf);
   strcpy(buf, "rm -rf " NEWBBSHOME "/man");
   system(buf);
   strcpy(buf, "mkdir " NEWBBSHOME "/man");
   system(buf);
   strcpy(buf, "mkdir " NEWBBSHOME "/man/boards");
   system(buf);

   printf("Copying Boards ... ");
   strcpy(buf, "cp -r " OLDBBSHOME "/boards/* " NEWBBSHOME "/boards/");
   system(buf);
   printf("\nCopying Mans ... ");
   strcpy(buf, "cp -r " OLDBBSHOME "/man/* " NEWBBSHOME "/man/");
   system(buf);

   sprintf(buf, "rm -f " NEWBBSHOME "/etc/issue.%s ", DEFAULTST);
   system(buf);
   sprintf(buf, "ln -s " NEWBBSHOME "/etc/issue " NEWBBSHOME
                 "/etc/issue.%s", DEFAULTST);
   system(buf);
   sprintf(buf, "rm -f " NEWBBSHOME "/etc/welcome.%s", DEFAULTST);
   system(buf);
   sprintf(buf, "ln -s " NEWBBSHOME "/etc/welcome " NEWBBSHOME
                "/etc/welcome.%s", DEFAULTST);
   system(buf);

   i = j = 0;
   while(fread(&ob, sizeof(ob), 1, fs)) {
      if(ferror(fs) || feof(fs))
         break;
      if(!check_name('B', ob.brdname)) {
         copy_ob2nb(&ob, &nb);
         fwrite(&nb, sizeof(nb), 1, ft);
         i++;
      }
      j++;
   }
   fclose(fs);
   fclose(ft);
   printf("\nTrans %d Board Records (Origin %d)\n\n", i, j);

   strcpy(buf, "rm -rf " NEWBBSHOME "/etc");
   system(buf);
   strcpy(buf, "mkdir " NEWBBSHOME "/etc");
   system(buf);

   printf("\nCopying etc Files ...");
   strcpy(buf, "cp -r " OLDBBSHOME "/etc/* " NEWBBSHOME "/etc/");
   system(buf);
   strcpy(buf, "cp " NEWBBSHOME "/src/maple/version " NEWBBSHOME "/etc/");
   system(buf);

   printf("\nCopying BBSHOME Files ...");
   {
      char *rot[] =
      { "sendmail.log", "logins.bad", "usies", "register.log", "out.going",
        "mail*", "mbox_sent", ".post*", ".vot*", ".ve*" };

      for(i = 0 ; i < 10 ; i++) {
         sprintf(buf,
            "cp -r " OLDBBSHOME "/%s " NEWBBSHOME "/ > /dev/null 2>&1",
                 rot[i]);
         system(buf);
      }
   }

   printf("\nCopying innd Files ...");
   {
      char *innd[] =
      {  "*active*", "*.bbs", "history*", "*.link", "*.OLD", "*.bntp",
         "bbslog" };

      for(i = 0 ; i < 7 ; i++) {
         sprintf(buf,
            "cp " OLDBBSHOME "/innd/%s " NEWBBSHOME "/innd/ > /dev/null 2>&1",
                 innd[i]);
         system(buf);
      }
   }

#ifdef RMVHOLES
   strcpy(buf,
      "find bbsrs/home -name 'alohaed' -print -delete");
   system(buf);
   strpcy(buf,
      "find bbsrs/home -name 'postlist' -print -delete");
   system(buf);
   strcpy(buf,
      "find bbsrs/home -name 'postnotify' -print -delete");
#endif

   printf("\nSetting Owner/Mode for NEWBBSHOME ... ");
   sprintf(buf, "chown -R " NEWBBSUSER " " NEWBBSHOME);
   system(buf);
   sprintf(buf, "chgrp -R " NEWBBSUSER " " NEWBBSHOME);
   system(buf);

   sprintf(buf, "chmod -R 770 " NEWBBSHOME);
   system(buf);
   puts("\n\nAll done...\n");
   return 0;
}
