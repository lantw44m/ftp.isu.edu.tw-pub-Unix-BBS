#include <stdio.h>
#include "cvsup.h"

#undef DEBUG

#define SUM_FILE CVS_WORK "/working_sum"

cksum ori, chk;

int read_sum(char *fpath, cksum *chksum) {
   char ci, buf[256] = {0}, *ptr[3] = { buf, buf, buf };
   int i;
   FILE *fp;

   if((fp = fopen(fpath, "r")) == NULL) return -1;
   fgets(buf, 256, fp);
   fclose(fp);

   for(ci = i = 0 ; i < 256 && ci < 2 ; i++)
     if((uschar)buf[i] <= ' ') {
       buf[i] = 0;
       ptr[++ci] = &buf[i+1];
     }

   chksum->crc1 = (usint)atoi(ptr[0]);
   chksum->crc2 = (usint)atoi(ptr[1]);
   strcpy(chksum->filename, ptr[2]);

   return 0;
}

int check_sum(char *file) {
   char fpath[128], fspath[128], command[256];

   if(file[0] == '~') strcpy(fpath, file);
   else sprintf(fpath, "%s/%s", CVS_WORK, file);
   sprintf(fspath, "%s.sum", fpath);

   sprintf(command, "sum %s > %s", fpath, SUM_FILE);
   if(system(command)) return 1;

   if(read_sum(fspath, &ori)) return -2;
   if(read_sum(SUM_FILE, &chk)) return -2;

#ifdef DEBUG
   printf("%d %d %d %d\n", ori.crc1, chk.crc1, ori.crc2, chk.crc2);
#endif

   return (ori.crc1 - chk.crc1 + ori.crc2 - chk.crc2);
}
