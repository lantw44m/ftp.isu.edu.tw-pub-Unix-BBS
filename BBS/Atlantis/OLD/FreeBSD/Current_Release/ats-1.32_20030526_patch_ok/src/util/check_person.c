/*-------------------------------------------------------*/
/* util/check_person.c        (SOB 0.22 ATS Verion 0.84) */
/*-------------------------------------------------------*/
/* target : 顯示特殊權限 請依實際需要修改                */
/* create : 00/12/17 by Dopin                            */
/* update :   /  /                                       */
/*-------------------------------------------------------*/
/* syntax : check_person                                 */
/*-------------------------------------------------------*/

#include "bbs.h"

int main(void) {
   userec user;
   char ch;
   int i, j, k;
   FILE *fs;
   char buf[64];

   sprintf(buf, BBSHOME "/.PASSWDS");
   fs = fopen(buf, "rb");

   k = 0;
   while (fread(&user, sizeof(user), 1, fs) == 1) {
      if(user.userlevel & PERM_SYSOP ||
         user.userlevel & PERM_SPCMEMBER ||
         user.userlevel & PERM_CHATROOM ||
         user.userlevel & PERM_BOARD ||
         user.userlevel & PERM_ENTERSC ||
         user.userlevel & PERM_SECRET ||
         user.userlevel & PERM_ACCOUNTS) {
         printf("%-13.13s", user.userid);

         if(user.userlevel & PERM_SYSOP) printf(" %-s", "sysop");
         if(user.userlevel & PERM_CHATROOM) printf(" %-s", "chat");
         if(user.userlevel & PERM_BOARD) printf(" %-s", "board");
         if(user.userlevel & PERM_ACCOUNTS) printf(" %-s", "account");
         if(user.userlevel & PERM_SPCMEMBER) printf(" %-s", "spc");
         if(user.userlevel & PERM_BM) printf(" %-s", "bm");
         if(user.userlevel & PERM_ENTERSC) printf(" %-s", "etsc");
         if(user.userlevel & PERM_SECRET) printf(" %-s", "sec");
         printf("\n");
      }
   }

   fclose(fs);
   return 0;
}
