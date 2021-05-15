/*-------------------------------------------------------*/
/* util/safh2fh.c              ( Atlantis BBS Ver 1.31 ) */
/*-------------------------------------------------------*/
/* target : 轉換 ATS fileheader 至 Trans_From_FB/Cola 版 */
/* create : 02/09/11 (Dopin)                             */
/* update : 02/09/25                                     */
/*-------------------------------------------------------*/
/* syntex : safh2fh                                      */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"       /* 只差在那個 Rename() 那是 kaede 兄寫的小工具 */

#define LISTPATH BBSHOME "/list_songs"
#define ORGSONG BBSHOME "/"

struct safh {  /* SongBook's File Header (Standard Ats FH) */
  char filename[FNLEN-1];       /* M.109876543210.A */
  char report;                  /* Dopin : 新制推薦 */
  char savemode;                /* file save mode */
  char owner[IDLEN + 2];        /* uid[.] */
  char date[6];                 /* [02/02] or space(5) */
  char title[TTLEN];
  uschar goodpost;
  uschar filemode;              /* must be last field @ boards.c */
};
typedef struct safh safh;

int main(void) {
   char *ptr, *src = ".DIR", *tar = ".DIR.NEW";
   char pbuf[512], obuf[512], nbuf[512];
   FILE *fp, *fs, *ft;
   safh sfh;
   fileheader fh;

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
         fh.report = sfh.report;
         fh.savemode = sfh.savemode;
         strcpy(fh.owner, sfh.owner);
         strncpy(fh.date, sfh.date, 6);
         strcpy(fh.title, sfh.title);
         fh.goodpost = sfh.goodpost;
         fh.filemode = sfh.filemode;

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

