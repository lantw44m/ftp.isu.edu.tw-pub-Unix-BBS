#include "bbs.h"

struct help_struct {
  char key[8];
  char title[64];
  char path[128];
  char perm;
  char backup[55];
};
typedef struct help_struct hpst;

hpst acshpst[128] = {0};

void show_syntex(void) {
   puts("trans_help <filename>");
   puts("Atlantis BBS 1.32 by Dopin");
}

int main(int argc, char *argv[]) {
   char buf[128], tobuf[96], key[10], title[64];
   int i, j;
   FILE *fs, *ft;
   hpst temp;

   if(argc != 2) {
      show_syntex();
      return 1;
   }

   printf("Size of Help Structure is %d bytes\n", sizeof(temp));

   sprintf(buf, BBSHELP "/%s", argv[1]);
   sprintf(tobuf, BBSHELP "/%s.help", argv[1]);

   if((fs = fopen(buf, "r")) == NULL) {
      printf("Source File %s Open Error\n", buf);
      return -1;
   }

   if((ft = fopen(tobuf, "wb+")) == NULL) {
      fclose(fs);
      printf("Target File %s Open Error\n", tobuf);
      return -1;
   }

   if(!fgets(title, 22, fs)) return -1;
   else {
      for(i = 0 ; i < 22 ; i++) if((uschar)title[i] < ' ') break;
      title[i] = 0;
   }

   for(i = 0 ; i < 128 ; i++) {
      memset(&temp, 0, sizeof(temp));

      if(ferror(fs) || feof(fs)) {
         puts("Source File may Broken, Interrupt !\n");
         break;
      }

      if(fgets(buf, 80, fs)) buf[1] = buf[12] = buf[76] = 0;
      for(j = 2 ; j < 12 ; j++) if((uschar)buf[j] <= ' ') buf[j] = 0;
      for(j = 14 ; j < 76 ; j++) if((uschar)buf[j] < ' ') buf[j] = 0;
      temp.perm = atoi(buf);
      strcpy(temp.key, &buf[2]);
      strcpy(temp.title, &buf[13]);
      if(!i) strcpy(temp.backup, title);

      if(fgets(buf, 128, fs)) {
         for(j = 0 ; j < 127 ; j++) if((unsigned char)buf[j] <= ' ') break;
         buf[j] = 0;

         if(!strstr(buf, "[NULL]")) strcpy(temp.path, buf);
      }
      else {
         puts("Source File Reading Error , Interrupt !\n");
         break;
      }

      fwrite(&temp, sizeof(temp), 1, ft);
   }

   printf("TRANS %d RECORDS\n", i);

   fclose(fs);
   fclose(ft);

   return 0;
}
