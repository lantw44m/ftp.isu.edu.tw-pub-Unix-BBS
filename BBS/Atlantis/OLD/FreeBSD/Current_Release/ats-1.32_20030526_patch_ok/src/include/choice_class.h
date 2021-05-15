#include "all_stations.h"

int domenu(int cmdmode, char *cmdtitle, int cmd, struct commands cmdtable[], int mode);

static int Class() {
   char ci, flag=0;
   FILE *fp;

   if(cuser.extra_mode[0] == 1) {
      cuser.extra_mode[0] = 0;
      strcpy(currboard, "Announce");
      load_boards("Announce");
   }

   point_to_classlist();

#ifndef NO_USE_MULTI_STATION
   for(ci = 0 ; ci < station_num ; ci++)
      if(!strcmp(station_list[ci], cuser.station)) {
         flag = 1;
         domenu(CLASS, "分類看板", '0', ptr_st[ci], 0);
         break;
      }
   if(!flag)
#endif
      domenu(CLASS, "分類看板", '0', ptr_st[0], 0);

   return 0;
}
