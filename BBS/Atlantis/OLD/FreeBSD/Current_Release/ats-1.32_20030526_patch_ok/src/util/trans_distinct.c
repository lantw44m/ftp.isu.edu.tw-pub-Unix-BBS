/*-------------------------------------------------------*/
/* util/trans_distinct         ( Atlantis BBS Ver 1.32 ) */
/*-------------------------------------------------------*/
/* target : 切割非文字 distinct 格式並輸出至檔案         */
/* create : 02/11/27 (Dopin)                             */
/* update :                                              */
/*-------------------------------------------------------*/
/* syntex : trans_distinct <Source File> <Target File>   */
/*-------------------------------------------------------*/

#include "bbs.h"

int trans_distinct(char *fsource, char *ftarget) {
   char buf[512];
   int ch, i;
   FILE *fs, *ft;

   if((fs = fopen(fsource, "rb")) == NULL) return -1;
   if((ft = fopen(ftarget, "w+")) == NULL) {
      fclose(fs);
      return -1;
   }

   i = 0;
   while(!feof(fs) && !ferror(fs)) {
      fscanf(fs, "%c", &ch);
      if(feof(fs) || ferror(fs)) break;
      if((char)ch == 10) {
         fprintf(ft, "%s\n", buf);
         i = 0;
      }
      else buf[i++] = (char)ch;
   }

   fclose(fs);
   fclose(ft);

   return 0;
}

int main(int argc, char *argv[]) {
   if(argc != 3) return 1;
   return trans_distinct(argv[1], argv[2]);
}
