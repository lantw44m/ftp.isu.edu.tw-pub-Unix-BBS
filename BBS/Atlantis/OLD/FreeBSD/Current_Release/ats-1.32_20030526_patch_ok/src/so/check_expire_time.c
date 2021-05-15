#include "bbs.h"
#include "expire.h"

/* Dopin 11/08/02 */
void check_expire_time(void) {
   char buf[80];
   time_t now = time(NULL), last;
   FILE *fp;

#ifdef Expire_ID
   if(strcmp(cuser.userid, Expire_ID))
#else
   if(!HAS_PERM(PERM_SYSOP))
#endif
      return;

   if((fp = fopen(".expire.time", "rb")) != NULL) {
      fread(&last, sizeof(last), 1, fp);
      fclose(fp);

      if(now - last > Expire_Interval) {
         if((fp = fopen(BBSHOME "/run/exipre.pid", "r")) != NULL) {
            memset(buf, 0, 80);
            fgets(buf, 80, fp);
            fclose(fp);

            buf[22] = 0;
            last = (time_t)atoi(&buf[12]);
            if(now - last > Expire_Timeout)
            pressanykey("expire 程序超過執行上限時間 請檢查系統 !");
            return;
         }
      }
      else return;
   }

   fp = fopen(".expire.time", "wb+");
   fwrite(&now, sizeof(now), 1, fp);
   fclose(fp);

   sprintf(buf, "bin/expire auto &");
   if(system(buf)) pressanykey("expire 程式執行發生錯誤 !");
}
