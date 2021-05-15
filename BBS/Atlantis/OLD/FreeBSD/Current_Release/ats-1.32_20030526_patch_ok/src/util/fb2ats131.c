/*-------------------------------------------------------*/
/* util/fb2ats131.c           (SOB 0.22 ATS Verion 1.31) */
/*-------------------------------------------------------*/
/* target : 轉換原先 火鳥 BBS 3.0 各項資料               */
/* create : 02/07/20 (Dopin)                             */
/* update : 02/09/17                                     */
/*-------------------------------------------------------*/
/* syntex : fb2ats131 (as root in usr/home)              */
/*-------------------------------------------------------*/

#include "bbs.h"

#define  OLDBBSHOME     "/home/bbs"
#define  NEWBBSHOME     "/home/bbsrs"
#define  NEWBBSUSER     "bbsrs"

#define  BM_LEN         50
#define  OLD_PERM_POSTMASK     32768      /* 0100000 */

struct oldrec {
   char            userid[IDLEN+2];   /* PASSFILE */
   time_t          firstlogin;
   char            lasthost[16];
   unsigned int    numlogins;
   unsigned int    numposts;
   char            flags[2];
   char            passwd[PASSLEN];
   char            username[NAMELEN];
   char            ident[NAMELEN];
   char            termtype[16];
   char            reginfo[STRLEN-16];
   unsigned int    userlevel;
   time_t          lastlogin;
   time_t          stay;
   char            realname[NAMELEN];
   char            address[STRLEN];
   char            email[STRLEN-12];
   unsigned int    nummails;
   time_t          lastjustify;
   char            gender;
   unsigned char   birthyear;
   unsigned char   birthmonth;
   unsigned char   birthday;
   int             signature;
   unsigned int    userdefine;
   time_t          notedate;
   int             noteline;
};
typedef struct oldrec oldrec;

void copy_ou2nu(oldrec *ou, userec *nu) {
  int i;

  memset(nu, 0, sizeof(userec));

  ou->realname[19] = 0;
  ou->username[23] = 0;

  strcpy(nu->userid, ou->userid);
  strcpy(nu->realname, ou->realname);
  strcpy(nu->username, ou->username);

  for(i = 0 ; i < PASSLEN ; i++)
     nu->passwd[i] = ou->passwd[i];

  for(i = 0 ; i < 49 && ou->email[i] ; i++)
      nu->email[i] = ou->email[i];
  nu->email[i] = 0;
  for(i = 0 ; i < 49 && ou->address[i] ; i++)
      nu->address[i] = ou->address[i];
  nu->address[i] = 0;
  for(i = 0 ; i < REGLEN && ou->ident[i] ; i ++)
      nu->justify[i] = ou->ident[i];
  nu->justify[i] = 0;
  strcpy(nu->lasthost, ou->lasthost);

  nu->userlevel = ou->userlevel;
  nu->numlogins = (unsigned long int)ou->numlogins;
  nu->numposts = (unsigned long int)ou->numposts;

  nu->uflag |= MOVIE_FLAG + COLOR_FLAG;
  nu->firstlogin = ou->firstlogin;
  nu->lastlogin = ou->lastlogin;

  nu->month = ou->birthmonth;
  nu->day = ou->birthday;
  nu->year = ou->birthyear;
}

struct oldbd {
   char filename[STRLEN];   /* the BOARDS files */
   char owner[STRLEN - BM_LEN];
   char BM[ BM_LEN - 1];
   char flag;
   char title[STRLEN ];
   unsigned level;
   unsigned char accessed[ 12 ];
};
typedef struct oldbd oldbd;

int check_name(char mode, char *str) {
  int i, j, flag;

  i = strlen(str);
  if(mode == 'U') if(i < 2 || i > IDLEN) return -1;

  if(mode == 'B') if(i < 2 || i > IDLEN * 2) return -1;

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

void copy_ob2nb(oldbd *ob, boardheader *nb) {  /* use newbd once */
  int i;

  memset(nb, 0, sizeof(boardheader));

  for(i = 0 ; i < (IDLEN+1) * 2 - 1 && ob->filename[i] ; i++)
     nb->brdname[i] = ob->filename[i];
  nb->brdname[i] = 0;
  for(i = 0 ; i < BTLEN -1 && ob->title[i+1] ; i++)
     nb->title[i] = ob->title[i+1];
  nb->title[i] = 0;

  for(i = 0 ; i < IDLEN*3+2 ; i++)
     nb->BM[i] = ob->BM[i];
  nb->BM[i] = 0;

  nb->level = ob->level;
  if(nb->level & OLD_PERM_POSTMASK) {
     nb->level &= 0xFFFFFFFF & OLD_PERM_POSTMASK;
     nb->level |= PERM_POSTMASK;
  }
  strcpy(nb->station, DEFAULTST);
}

int main(void) {
   char buf[200];
   int i, j;
   FILE *fs, *ft;
   oldrec ou;
   userec nu;
   oldbd ob;
   boardheader nb;  /* user newbd once */

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

         sprintf(buf,
                 "cp -r " OLDBBSHOME "/home/%c/%s " NEWBBSHOME "/home/",
                 toupper(ou.userid[0]), ou.userid);
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

   i = j = 0;
   while(fread(&ob, sizeof(ob), 1, fs)) {
      if(ferror(fs) || feof(fs))
         break;
      if(!check_name('B', ob.filename)) {
         copy_ob2nb(&ob, &nb);
         fwrite(&nb, sizeof(nb), 1, ft);
         sprintf(buf, "cp -r " OLDBBSHOME "/boards/%s " NEWBBSHOME "/boards/",
                 nb.brdname);
         system(buf);
         sprintf(buf, "cp -r "
                      OLDBBSHOME "/0Announce/%s " NEWBBSHOME "man/boards/",
                      nb.brdname);
         system(buf);
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

   sprintf(buf, "cp " NEWBBSHOME "/stfiles/atlantis.h.fb3 " NEWBBSHOME
                "/stfiles/atlantis.h");
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


   printf("\nSetting Owner/Mode for NEWBBSHOME ... ");
   sprintf(buf, "chown -R " NEWBBSUSER " " NEWBBSHOME);
   system(buf);
   sprintf(buf, "chgrp -R " NEWBBSUSER " " NEWBBSHOME);
   system(buf);
   sprintf(buf, "chmod -R 770 " NEWBBSHOME);
   system(buf);
   puts("\n\nAll done...\n");

   puts("please su " NEWBBSUSER "adm, go maple Directory, make clean update");
   return 0;
}
