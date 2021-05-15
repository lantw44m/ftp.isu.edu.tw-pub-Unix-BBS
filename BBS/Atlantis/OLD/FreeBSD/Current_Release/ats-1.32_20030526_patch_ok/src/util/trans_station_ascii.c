/*--------------------------------------------------------------------------*/
/* util/trans_station_ascii.c                  (SOB 0.22 ATS Version 1.32)  */
/*--------------------------------------------------------------------------*/
/* target : 轉換站台名片檔的標籤程式                                        */
/* create : 02/12/08 by Dopin                                               */
/* update :                                                                 */
/*--------------------------------------------------------------------------*/
/* syntax : trans_station_ascii <source file> <bbsname> <dn> <ip> <station> */
/*--------------------------------------------------------------------------*/

#include "bbs.h"

char check_st_task(char *str, char *mainst, char *domain, char *ip, char *st) {
   char flag = 0, *ptr, *ps[4];
   char *tasks[] = { "[#MAINST#]", "[#DN]", "[#IP#]", "[#ST#]" };
   char cbuf[256];
   int i, j, element;

   ps[0] = mainst;
   ps[1] = domain;
   ps[2] = ip;
   ps[3] = st;

   for(i = 0 ; i < 4 ; i++) {
      strcpy(cbuf, str);
      if(ptr = strstr(cbuf, tasks[i])) {
         strcpy(ptr, ps[i]);
         element = ptr - cbuf;
         strcpy(&cbuf[strlen(cbuf)], &str[element + strlen(tasks[i])]);
         flag = 1;
         strcpy(str, cbuf);
      }
   }

   return flag;
}

int main(int argc, char *argv[]) {
   char fname[80], buf[256];
   int i;
   FILE *fs, *ft;

   if(argc != 6) return 1;

   if((fs = fopen(argv[1], "r")) == NULL) return -1;
   sprintf(fname, BBSHOME "/ascii/%s_trans", argv[5]);
   if((ft = fopen(fname, "w+")) == NULL) {
     fclose(fs);
     return -1;
   }

   while(fgets(buf, 256, fs)) {
      for(i = 0 ; i < 256 && (unsigned char)buf[i] > 13 ; i++) ;
      buf[i] = 0;
      check_st_task(buf, argv[2], argv[3], argv[4], argv[5]);
      fprintf(ft, "%s\n", buf);
   }
   fclose(fs); fclose(ft);

   return 0;
}
