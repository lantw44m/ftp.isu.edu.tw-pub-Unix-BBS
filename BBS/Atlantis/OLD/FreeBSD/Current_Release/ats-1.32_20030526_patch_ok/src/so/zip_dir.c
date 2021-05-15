#include "bbs.h"

extern int doforward(char *, fileheader *, int);

/* Kaede */
mail_mbox() {
   char cmd[100];
   fileheader fhdr;

   sprintf(cmd, "/tmp/%s.tgz", cuser.userid);
   sprintf(fhdr.title, "%s 私人資料", cuser.userid);
   doforward(cmd, &fhdr, 'Z');
}

/* Gene */
mail_board() {
   char cmd[100];
   fileheader fhdr;
   char genbuf[300];
   char fname[300];
   char address[300];

   if(strcmp(cuser.userid, SUPERVISOR))
      return;

   strcpy(address, currboard);

    sprintf(genbuf, "確定打包 [%s] 的精華區嗎(Y/N/Q)？[Y] ", currboard);
    getdata(b_lines - 1, 0, genbuf, fname, 3, LCECHO, 0);

    if (fname[0] == 'q')
    {
      outmsg("取消打包");
      return 1;
    }
    if (fname[0] == 'n')
      address[0] = '\0';

  if(!address[0]) {
    Select();
    strcpy(address, currboard);
  }
  else strcpy(currboard, address);

   sprintf(cmd, "/tmp/%s.tgz", address);
   sprintf(fhdr.title, "[" BBSNAME "] %s 版的精華區", address);
   doforward(cmd, &fhdr, 'B');
}
