#include "bbs.h"
#include "dopin.h"
#include "agree.h"
#include "dl_lib.c"

#define Error_log "./check_agree.log"

/* Dopin 11/24/98 */
usint power(int i, int j) {
   usint res=1, cnt;

   for(cnt = 0 ; cnt < j ; cnt ++) res *= i;
   return res;
}

int HAS_PERM(usint perm) {
   int i;
   usint tp;

   if(!perm) return 1;

   for(i = 0 ; i < NUMPERMS ; i++) {
     tp = power(2, i);
     if(tp & perm) if(cuser.userlevel & tp) return 1;
   }

   return 0;
}

userec cuser;

int main(void) {
   char act, fg, ans[6], genbuf[200];
   int i;
   FILE *fp;

   sprintf(genbuf, "touch %s", LOCKFILE);
   system(genbuf);

   fp = fopen(TARGET, "rb");
   if(fp == NULL) {
      fp = fopen(Error_log, "a+");
      fprintf(fp, "Error Open Target File %s\n", TARGET);
      fclose(fp);
      return 1;
   }

   if(fread(&work, sizeof(work), 1, fp) == 1) fclose(fp);
   else {
      fclose(fp);
      fp = fopen(Error_log, "a+");
      fprintf(fp, "Error Reading Target File (Size Error)\n");
      fclose(fp);
      return 1;
   }

   sprintf(genbuf, "cp " TARGET " " BACKUP);
   system(genbuf);

   for(i = 0 ; i < MAXAGREES ; i++) {
      FILE *fps;
      char fbuf[80], m, d;
      time_t now;

      if(!work[i].title[0] || !work[i].count) continue;

      get_tm_time(NULL, &m, &d, NULL, NULL);
      if(work[i].count < work[i].minlimit &&
         (!strcmp(work[i].end, "--/--") ||
         (check_mmdd(work[i].end, m, d) == 1))
      )
         continue;

      fp = fopen(TEMPFILE, "w+");
      time(&now);
      fprintf(fp, "作者: [系統公告] 看板: %s\n"
                  "標題: <連署> %s\n"
                  "時間: %s\n\n",
                  POSTBOARD, work[i].title, (char *)Cdate(&now));

      fprintf(fp, "系統狀態 : 站務執行公告結果    連署限制 : %s\n"
                  "發起人   : %-13s       具名連署 : %s\n"
                  "起始日期 : %-5s               結束日期 : %s\n"
                  "連署門檻 : %-5d 人            連署人數 : %-5d 人\n"
                  "連署結果 : %s\n",
                  limit[perm_acs(1, &work[i].perm, 0)-1],
                  work[i].author, work[i].anonymous ? "否" : "是",
                  work[i].start, work[i].end,
                  work[i].minlimit, work[i].count,
                  (work[i].minlimit <= work[i].count) ?
                  "成功\" : "失敗"
             );

      fprintf(fp, "\n簽署人員記錄清單 :\n");
      set_agree_file(genbuf, i);
      if((fps = fopen(genbuf, "r")) != NULL) {
         while(fgets(fbuf, 80, fps)) {
            if(!work[i].anonymous)
               fprintf(fp, "%s", fbuf);
            else {
               fbuf[13] = 0;
               fprintf(fp, "%-13s %s", "[Anonymous]", &fbuf[14]);
            }
         }
         remove(genbuf);
         fclose(fps);
      }

      addorg(0, fp);
      fclose(fp);
      void_log('B', TEMPFILE, POSTBOARD, work[i].title);

      memset(&work[i], 0, sizeof(ags));
   }

   fp = fopen(TARGET, "wb+");
   fwrite(&work, sizeof(work), 1, fp);
   fclose(fp);

   sprintf(genbuf, "%s", LOCKFILE);
   remove(genbuf);

   return 0;
}
