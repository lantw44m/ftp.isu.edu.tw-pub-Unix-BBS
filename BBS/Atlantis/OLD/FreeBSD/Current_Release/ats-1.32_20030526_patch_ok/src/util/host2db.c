/*-------------------------------------------------------*/
/* util/host2db.c              ( Atlantis BBS Ver 1.32 ) */
/*-------------------------------------------------------*/
/* target : 轉換文字故鄉參照檔為二位元結構               */
/* create : 02/07/20 (Dopin)                             */
/* update : 02/09/17                                     */
/*-------------------------------------------------------*/
/* syntex : host2db (as bbsrs)                           */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"

#define SRC BBSHOME "/etc/domain_name_query"
#define TAR BBSHOME "/etc/transhost.db"

int main(void) {
   char buf[80];
   int i = 0, j;
   fh2st db;
   FILE *fs, *ft;

   if((fs = fopen(SRC, "r")) == NULL) {
      printf("Open Source File %s Error !\n", SRC);
      return 1;
   }

   if((ft = fopen(TAR, "wb+")) == NULL) {
      fclose(fs);
      printf("Open Target File %s Error !\n", TAR);
      return 1;
   }

   while(fgets(buf, 80, fs)) {
      if(strlen(buf) < 42) continue;
      for(j = 0 ; j < 80 ; j++) if((uschar)buf[j] <= ' ') buf[j] = 0;

      db.host_type = (char)atoi(buf);
      strcpy(db.fromhost, &buf[2]);
      strcpy(db.transto, &buf[40]);

      printf("%d %-40s %-20s\n", db.host_type, db.fromhost, db.transto);
      fwrite(&db, sizeof(db), 1, ft);
      i++;
   }

   fclose(fs);
   fclose(ft);

   printf("Compelte %d Records\n", i);
}
