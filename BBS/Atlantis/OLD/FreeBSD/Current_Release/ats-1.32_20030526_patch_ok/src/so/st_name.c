#include "bbs.h"

extern char station_list_cn[][15];
extern int station_num;

int c_st_name(void) {
   FILE *fp;
   int i;
   char buf[20];

   if(!is_station_sysop())
      return DONOTHING;

   getdata(b_lines, 0, "請輸入新的中文站名 : ", buf, 15, DOECHO, 0);
   if(!buf[0]) return FULLUPDATE;

   strcpy(station_list_cn[cuser.now_stno], buf);
   fp = fopen(BBSHOME "/stfiles/station_list_cn", "w+");
   for(i = 0 ; i < station_num ; i++)
      fprintf(fp, "%s\n", station_list_cn[i]);

   fclose(fp);
   return FULLUPDATE;
}
