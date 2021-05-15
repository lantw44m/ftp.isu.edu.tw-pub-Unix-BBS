#include "cvsup.h"
#include "dopin.h"

#undef DEBUG

void show_syntex() {
   puts("cvsup <visit / update / force-update>");
   puts("Atlantis BBS(C) 1.32-Release by Dopin");
}

int visit_temp(char mode) {
   char buf[128];

   if(dashd(CVS_WORK)) {
     if(mode == 'V') return -1;
     else {
       sprintf(buf, "rm -rf %s", CVS_WORK);
       system(buf);
     }
   }

   if(mode != 'D') {
     sprintf(buf, "mkdir %s", CVS_WORK);
     system(buf);
   }

   return 0;
}

int main(int argc, char *argv[]) {
   char mon, day, hour, min, curcvs[64], buf[256];
   int year, i;
   time_t now = time(NULL);
   FILE *fs, *ft;
   cvsindex c;

   if(argc != 2 || strcmp(argv[1], "visit") && strcmp(argv[1], "update") &&
      strcmp(argv[1], "force-update")) {
      show_syntex();
      return 0;
   }

   if(visit_temp('V')) {
     if(strcmp(argv[1], "force-update")) {
       write_log("[ERROR]   Working Directory Exist !");
       return -1;
     }
     else visit_temp('F');
   }

   get_tm_time(&year, &mon, &day, &hour, &min);
   chdir(CVS_WORK);
   for(i = 0 ; i < ERROR_COUNT ; i++) {
     sprintf(buf, "wget -c %s/%d-%d/cvs_index > /dev/null 2>&1",
             OFFICAL_URL, year, mon);
     if(system(buf)) continue;
     else break;
   }

   if(i == ERROR_COUNT) {
     write_log("[ERROR]   Receving file 'cvs_index' Fault ...");
     return -2;
   }

   /* Dopin: 隨時會變更 故取消之 */ /*
   for(i = 0 ; i < ERROR_COUNT ; i++) {
     sprintf(buf, "wget -c %s/%d-%d/cvs_index.sum > /dev/null 2>&1",
             OFFICAL_URL, year, mon);
     if(system(buf)) continue;
     else break;
   }

   if(i == ERROR_COUNT) {
     write_log("[ERROR]   Receving file 'cvs_index.sum' Fault ...");
     return -3;
   }
   */

   i = patch_update(year, mon, argv[1]);
   if(i == 0 || i == 999) {
      visit_temp('D');

      if(i == 0) write_log("[SUCCESS] BBS cvsup finished ...");
      else       write_log("[MESSAGE] Visit Mode finished ...");

#ifdef DEBUG
     printf("SYSTEM Code : %d\n", i);
#endif

      return 0;
   }
   else {
      write_log("[ERROR]   CVS Server may cause Error(s) ...");
#ifdef DEBUG
      printf("ERROR Code : %d\n", i);
#endif
      return -i;
   }
}
