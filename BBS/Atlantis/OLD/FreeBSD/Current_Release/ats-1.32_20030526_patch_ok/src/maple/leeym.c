/*-------------------------------------------------------*/
/* sob/leeym.c                (SOB 0.22 ATS Verion 1.31) */
/*-------------------------------------------------------*/
/* target : ATSVersion ¥Ñ Leeym ¥S¤½§i¥Î©ó SOB ªº¨ç¦¡¶°  */
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
      /* Dopin: ÀÉ®×¤£¦s¦b «h¶}¥ß·sÀÉ a -> a+ */
      if (fp1 = fopen("etc/.history", "a+")) {
        /* Dopin: ¥[¤W¤½¤¸¬ö¤¸ %-20s ¤£¥²±j¨î®æ¦¡ */
        fprintf(fp1,"¡i%02d/%02d/%04d %02d:%02d¡j"
        "[1;32;40m%s[m ­º¦¸¹F¨ì [1;33;40m%d[m ¤H¦¸\n",
        ptime->tm_mon+1, ptime->tm_mday, ptime->tm_year+1900, ptime->tm_hour,
        ptime->tm_min, "¥»¯¸¦P®É¤W½u¤H¼Æ", count_ulist());
        fclose(fp1);
      }
    }
    fclose(fp);
  }

  /* Dopin: ¤H¦¸¶W¹L´N°O¿ý */     /* Dopin: ÀÉ®×¦s¦b «h¯}Ãa¨ä¤º®e w -> w+ */
  if((count_ulist() - maxonline > 0) && (fp = fopen(".maxonline", "w+"))) {
    fprintf(fp, "%d", count_ulist());
    fclose(fp);
  }
}
#endif

/* Leeym ­­¨îµoªí¤å³¹ */
int deny_me() {
  char buf[STRLEN];

  setbfile(buf, currboard, fn_water);
  return belong(buf, cuser.userid);
}
