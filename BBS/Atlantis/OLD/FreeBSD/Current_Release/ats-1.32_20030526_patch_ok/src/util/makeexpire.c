/*-----------------------------------------------------*/
/* util/makeexpire.c          (SOB 0.22 ATS Ver. 1.31) */
/*-----------------------------------------------------*/
/* target: 自動抓取 .BOARDS 資料建立相關 expire 資訊   */
/* create: 08/09/02 by Dopin                           */
/* update:                                             */
/* notice: mv ~/etc/expire.auto ~/etc/exipre.con       */
/*         另需搭配 bin/exipre 之執行方可限制文章數量  */
/*-----------------------------------------------------*/

#include "bbs.h"

#define Day     365   /* 設定看板文章期限 */
#define Max    2000   /* 設定看板文章最大數量 */
#define Min     800   /* 設定看板文章刪除後數量 */

int main(void) {
   char buf[80];
   int i;
   FILE *fs, *ft;
   boardheader bh;

   fs = fopen(BBSHOME "/.BOARDS", "rb");
   if(!fs) return -1;
   ft = fopen(BBSHOME "/etc/expire.auto", "w+");
   if(!ft) return -1;

   i = 0;
   while(fread(&bh, sizeof(boardheader), 1, fs) == 1) {
      printf("%s\n", bh.brdname);
      fprintf(ft, "%-24s%d     %d     %d\n", bh.brdname, Day, Max, Min);
      i++;
      i++;
   }

   fclose(fs);
   fclose(ft);

   printf(" . %d .\n", i);
   return 0;
}
