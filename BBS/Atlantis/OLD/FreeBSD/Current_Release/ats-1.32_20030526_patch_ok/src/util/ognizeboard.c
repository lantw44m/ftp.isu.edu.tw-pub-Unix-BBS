/*-----------------------------------------------------*/
/* util/ognizboard.c          (SOB 0.22 ATS Ver. 1.31) */
/*-----------------------------------------------------*/
/* target: 重整 .BOARDS 資料                           */
/* create: 07/19/02 by Dopin                           */
/* update:                                             */
/* notice: mv ~/.BOARDS.OGNIZED ~/.BOARDS              */
/*-----------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"

int main() {
   int i = 0;
   FILE *fs, *ft;
   boardheader bh;

   fs = fopen(BBSHOME"/.BOARDS", "rb");
   ft = fopen(BBSHOME"/BOARDS.OGNIZED", "wb+");

   while(!feof(fs) && !ferror(fs) && !ferror(ft)) {
      fread(&bh, sizeof(bh), 1, fs);
      if(feof(fs) || ferror(fs) || ferror(ft)) break;
      else {
         if(!check_b_name(&bh)) {
            printf("%s\n", bh.brdname);
            fwrite(&bh, sizeof(bh), 1, ft);
            i++;
         }
         else
            continue;
      }
   }

ERROR:
   if(fclose(fs) == EOF) {
      printf("Error closing source file.\n");
      exit(1);
   }

   if(fclose(ft) == EOF) {
      printf("Error closing destination file.\n");
      exit(1);
   }

   printf("Records : %d\n", i);
   return 0;
}
