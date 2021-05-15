/*-------------------------------------------------------*/
/* util/clearfhfg.c            ( Atlantis BBS Ver 1.31 ) */
/*-------------------------------------------------------*/
/* target : Trans_From_FB/Cola 版本之 fileheader 顯示    */
/* create : 02/09/11 (Dopin)                             */
/* update : 02/10/25                                     */
/*-------------------------------------------------------*/
/* syntex : clearfhfg                                    */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"       /* 只差在那個 Rename() 那是 kaede 兄寫的小工具 */

#define LISTPATH BBSHOME "/fhfg"
#define ORGSONG BBSHOME "/"

int main(void) {
   char *ptr, *src = ".DIR", *tar = ".DIR.NEW";
   char pbuf[512], obuf[512], nbuf[512];
   FILE *fp, *fs, *ft;
   fileheader fh, sfh;

   printf("%s\n", LISTPATH);
   fp = fopen(LISTPATH, "r");
   if(!fp) {
      printf("List File %s Open Error !\n", LISTPATH);
      return -1;
   }

   while(fgets(pbuf, 512, fp)) {
      if((ptr = strstr(pbuf, src)) == NULL) continue;
      *ptr = 0;

      sprintf(obuf, "%s%s%s", ORGSONG, pbuf, src);
      sprintf(nbuf, "%s%s%s", ORGSONG, pbuf, tar);

      printf("Access %s ", obuf);

      fs = fopen(obuf, "rb");
      if(!fs) {
         printf("Fault !!!\n");
         continue;
      }

      ft = fopen(nbuf, "wb+");
      if(!ft) {
         fclose(fs);
         printf("Fault !!!\n");
         continue;
      }

      while(fread(&sfh, sizeof(sfh), 1, fs)) {
         memset(&fh, 0, sizeof(fh));
         strcpy(fh.filename, sfh.filename);
         fh.report = 0;
         fh.savemode = 0;
         strcpy(fh.owner, sfh.owner);
         strncpy(fh.date, sfh.date, 6);
         strcpy(fh.title, sfh.title);
         fh.goodpost = 0;
         fh.filemode = 0;

         fwrite(&fh, sizeof(fh), 1, ft);
      }

      fclose(fs);
      fclose(ft);

      Rename(nbuf, obuf);
      printf("... done.\n");
   }

   fclose(fp);
   return 0;
}
