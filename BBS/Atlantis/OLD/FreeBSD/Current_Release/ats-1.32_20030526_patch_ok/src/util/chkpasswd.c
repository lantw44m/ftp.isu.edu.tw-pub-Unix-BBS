/*-----------------------------------------------------*/
/* util/chkpasswd.c           (SOB 0.22 ATS Ver. 1.31) */
/*-----------------------------------------------------*/
/* target : .PASSWDS 檢查重複帳號名 轉換備份帳號       */
/* create : 02/08/28 by Dopin                          */
/* update : 02/08/29                                   */
/* notice : 工作目錄預設為 BBSHOME/backup              */
/*-----------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"

#define  SRC    ".PASSWDS"
#define  TAR    "PASSWDS.CHECK"
#define  BAK    "PASSWDS.backup"
#define  APD    "append_ids"
#define  WKD    "/backup/"

#define  MAX_ADD  4

int main(int argc, char *argv[]) {
   char buf[200], genbuf[200];
   int i, j, k, idc;
   FILE *fps, *fpt, *fpb;
   userec u;

   if(argc != 2 || strcmp(argv[1], "check_only") &&
      strcmp(argv[1], "do_append") && strcmp(argv[1], "setzero")) {
      puts("syntex : chkpasswd <check_only / do_append / setzero>");
      return -1;
   }

   strcpy(buf, BBSHOME WKD SRC);

   printf("opening %s ... ", buf);

   fps = fopen(buf, "rb");
   if(!fps) {
      printf("\nSource File %s open Error or not Exist\n", SRC);
      return -1;
   }
   else puts("done.");

   strcpy(buf, BBSHOME WKD TAR);
   fpt = fopen(buf, "wb+");
   if(!fpt) {
      fclose(fps);
      printf("\nTarget File %s open Error\n", TAR);
      return -1;
   }

   for(j = 0, idc = 0 ; idc < MAXUSERS ; idc++) {
      if(fread(&u, sizeof(u), 1, fps) == 1) {
         if(!check_u_name(&u)) {
            if(!check_repeat_id(0, &u)) {
               if(!strcmp(argv[1], "setzero")) {
                  /* 這個大小是看你想要設定幾個為零 請注意不要產生溢位 */
                  memset(&u.backup_int[0], 0, sizeof(int) * 40);
                  /* 這裡還可以加一些你想要設定初值的變數 但請注意不要動到 */
                  /* backup_char 這個陣列 因為許多已經在使用了 (汗) */
               }
               fwrite(&u, sizeof(u), 1, fpt);
               j++;
            }
         }
      }
      else break;
   }
   fclose(fps);
   fclose(fpt);

   for(i = 0 ; i < j ; i++)
      printf("%-14s %d\n", users[i].userid, users[i].count+1);
   printf("Process %d / %d Records\n", i, idc-1);

   if(strcmp(argv[1], "do_append")) goto SKIP_APD;

   strcpy(buf, BBSHOME WKD TAR);
   fps = fopen(buf, "ab+");
   if(!fps) {
      printf("Append Source File %s open Error ...\n", buf);
      return -1;
   }

   strcpy(buf, BBSHOME WKD APD);
   fpt = fopen(buf, "rb");
   if(!fpt) {
      fclose(fps);
      printf("Append Index File %s open Error ...\n", buf);
      return -1;
   }

   strcpy(buf, BBSHOME WKD BAK);
   fpb = fopen(buf, "rb");
   if(!fpb) {
      fclose(fps); fclose(fpt);
      printf("Backup File %s open Error ...\n", buf);
      return -1;
   }

   for(i = 0 ; i < MAX_ADD ; i++) {
      while(fgets(buf, 80, fpt) && !ferror(fpt)) {
         for(k = 0 ; k < 80 && (unsigned char)buf[k] > ' ' ; k++) ;
         buf[k] = 0;

         fseek(fpb, SEEK_SET, 0);
         while(fread(&u, sizeof(u), 1, fpb) == 1)
            if(!check_u_name(&u))
               if(!check_repeat_id(1, &u))
                  if(!strcmp(buf, u.userid)) {
                     fwrite(&u, sizeof(u), 1, fps);
                     printf("%-14s Append OK!\n", u.userid);
                     continue;
                  }
      }
   }
   fclose(fpb);
   fclose(fps);
   fclose(fpt);

SKIP_APD:
   return 0;
}
