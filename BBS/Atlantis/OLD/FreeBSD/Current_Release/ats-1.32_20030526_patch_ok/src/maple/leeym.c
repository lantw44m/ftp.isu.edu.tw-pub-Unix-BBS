/*-------------------------------------------------------*/
/* sob/leeym.c                (SOB 0.22 ATS Verion 1.31) */
/*-------------------------------------------------------*/
/* target : ATSVersion 由 Leeym 兄公告用於 SOB 的函式集  */
/* create : 02/09/04 by Dopin                            */
/* update : 02/09/11                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

#ifdef COUNT_MAXLOGINS
/* Leeym 98/03/26 */
check_max_online() {
  FILE *fp, *fp1;
  int maxonline = 0;
  time_t now = time(NULL);
  struct tm *ptime;

  ptime = localtime(&now);

  if(fp = fopen(".maxonline", "r")) {
    fscanf(fp, "%d", &maxonline);
    if(count_ulist() > maxonline) {
      /* Dopin: 檔案不存在 則開立新檔 a -> a+ */
      if (fp1 = fopen("etc/.history", "a+")) {
        /* Dopin: 加上公元紀元 %-20s 不必強制格式 */
        fprintf(fp1,"【%02d/%02d/%04d %02d:%02d】"
        "[1;32;40m%s[m 首次達到 [1;33;40m%d[m 人次\n",
        ptime->tm_mon+1, ptime->tm_mday, ptime->tm_year+1900, ptime->tm_hour,
        ptime->tm_min, "本站同時上線人數", count_ulist());
        fclose(fp1);
      }
    }
    fclose(fp);
  }

  /* Dopin: 人次超過就記錄 */     /* Dopin: 檔案存在 則破壞其內容 w -> w+ */
  if((count_ulist() - maxonline > 0) && (fp = fopen(".maxonline", "w+"))) {
    fprintf(fp, "%d", count_ulist());
    fclose(fp);
  }
}
#endif

/* Leeym 限制發表文章 */
int deny_me() {
  char buf[STRLEN];

  setbfile(buf, currboard, fn_water);
  return belong(buf, cuser.userid);
}
