#include "bbs.h"

int main(void) {
   userec user;
   FILE *fs;
   char buf[128];

   strcpy(buf, BBSHOME "/.PASSWDS");
   printf("%s\n", buf);
   fs = fopen(buf, "rb");
   if(!fs) return -1;

   while (fread(&user, sizeof(user), 1, fs) == 1) {
       sprintf(buf, BBSHOME "/home/%s/put", user.userid);
       printf("%s\n", buf);
       remove(buf);
   }
   fclose(fs);

   return 0;
}
