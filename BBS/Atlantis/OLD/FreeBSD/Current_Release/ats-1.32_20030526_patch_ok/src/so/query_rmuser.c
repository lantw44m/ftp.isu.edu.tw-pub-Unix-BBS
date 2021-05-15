#include "bbs.h"

void settmpfile(char *fpath, char *fname) {
   sprintf(fpath, "tmp/%s.%s", fname, cuser.userid);
}

#ifdef QUERY_REMOTEUSER
/* Dopin 02/11/27 */
void query_rmuser(void) {
   char bufs[40], buft[40], command[100];

   setuserfile(bufs, "remoteuser");
   settmpfile(buft, "remoteuser");
   sprintf(command, "bin/trans_distinct %s %s", bufs, buft);
   system(command);

   settmpfile(bufs, "remoteuser_20");
   sprintf(command, "tail -n 20 %s > %s", buft, bufs);
   system(command);

   more(bufs, YEA);
   unlink(buft);
   unlink(bufs);
}
#endif
