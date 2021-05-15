#include "bbs.h"

extern int doforward(char *, fileheader *, int);

/* Kaede */
mail_mbox() {
   char cmd[100];
   fileheader fhdr;

   sprintf(cmd, "/tmp/%s.tgz", cuser.userid);
   sprintf(fhdr.title, "%s �p�H���", cuser.userid);
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

    sprintf(genbuf, "�T�w���] [%s] ����ذ϶�(Y/N/Q)�H[Y] ", currboard);
    getdata(b_lines - 1, 0, genbuf, fname, 3, LCECHO, 0);

    if (fname[0] == 'q')
    {
      outmsg("�������]");
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
   sprintf(fhdr.title, "[" BBSNAME "] %s ������ذ�", address);
   doforward(cmd, &fhdr, 'B');
}
