/*-----------------------------------------------------*/
/* util/backup.c              (SOB 0.22 ATS Ver. 1.31) */
/*-----------------------------------------------------*/
/* target: 依日期執行備份檔案動作 自動化須搭配 crontab */
/* create: 08/13/02 by Dopin                           */
/* update: 09/06/02                                    */
/* notice: 備份之目標目錄預設為 BBSHOME/backup         */
/*-----------------------------------------------------*/

#include "bbs.h"
#undef DEBUG

void show_syntex(void) {
   puts("Syntex : backup <home|boards|man|src|etc|stfiles|PASS_BOARD>");
   puts("Atlantis BBS V1.31 09/06/02");
}

int main(int argc, char *argv[]) {
   char dateform[30], filename[40], buf[200];
   int i;
   time_t now;
   struct tm *ptime;

   if(argc < 2 || argc > 8) {
      show_syntex();
      return 1;
   }

   for(i = 1 ; i < argc ; i++) {
      time(&now);
      ptime = localtime(&now);

      sprintf(dateform, "%02d%02d%02d-%02d%02d",
              ptime->tm_mon+1, ptime->tm_mday, ptime->tm_year%100,
              ptime->tm_hour, ptime->tm_min);

      if(!strcmp(argv[i], "home") || !strcmp(argv[i], "man") ||
         !strcmp(argv[i], "src") || !strcmp(argv[i], "boards") ||
         !strcmp(argv[i], "etc") || !strcmp(argv[i], "stfiles")) {
         char gvbuf[10];

         strcpy(gvbuf, argv[i]);
STFILES:
         sprintf(filename, "%s_%s.tar", gvbuf, dateform);
         sprintf(buf, "tar cf %s %s", filename, gvbuf);
#ifdef DEBUG
         puts(buf);
#else
         system(buf);
#endif

         sprintf(buf, "gzip %s", filename);
#ifdef DEBUG
         puts(buf);
#else
         system(buf);
#endif

         sprintf(buf, "mv %s.gz backup/", filename);
#ifdef DEBUG
         puts(buf);
#else
         system(buf);
#endif

         if(!strcmp(gvbuf, "stfiles")) {
            strcpy(gvbuf, "run");
            goto STFILES;
         }
      }

      if(!strcmp(argv[i], "PASS_BOARD")) {
         char *files[] = { "PASSWDS", "BOARDS" };
         const int file_number = 2;
         int j;

         for(j = 0 ; j < file_number ; j++) {
            sprintf(buf, "cp .%s backup/%s_%s", files[j], files[j], dateform);
#ifdef DEBUG
            puts(buf);
#else
            system(buf);
#endif

         sprintf(buf, "gzip backup/%s_%s", files[j], dateform);
#ifdef DEBUG
         puts(buf);
#else
         system(buf);
#endif
         }
      }
   }
}
