/*-----------------------------------------------------*/
/* util/makeexpire.c          (SOB 0.22 ATS Ver. 1.31) */
/*-----------------------------------------------------*/
/* target: �۰ʧ�� .BOARDS ��ƫإ߬��� expire ��T   */
/* create: 08/09/02 by Dopin                           */
/* update:                                             */
/* notice: mv ~/etc/expire.auto ~/etc/exipre.con       */
/*         �t�ݷf�t bin/exipre �������i����峹�ƶq  */
/*-----------------------------------------------------*/

#include "bbs.h"

#define Day     365   /* �]�w�ݪO�峹���� */
#define Max    2000   /* �]�w�ݪO�峹�̤j�ƶq */
#define Min     800   /* �]�w�ݪO�峹�R����ƶq */

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
