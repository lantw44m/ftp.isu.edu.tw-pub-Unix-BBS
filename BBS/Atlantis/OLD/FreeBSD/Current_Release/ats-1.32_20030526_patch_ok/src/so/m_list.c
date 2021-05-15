#include "bbs.h"

static char listfile[] = "list.0";
static char msg_cc[] =   "[�s�զW��]\n";

extern void multi_list(int *);

int m_list() {
  int aborted;
  char genbuf[4];

  getdata(b_lines - 1, 0, "�W���� (E)�s�� (D)�R�� (Q)�����H[Q] ",
    genbuf, 4, LCECHO, 0);

  if(genbuf[0] == 'd') aborted = 1;
  else if (genbuf[0] == 'e') aborted = 2;
  else aborted = 0;

  if(aborted) {
    char listpath[64], ans[4];

    getdata(b_lines - 1, 0, "�п�ܦW�� (0-9 �H[0] ", ans, 3, DOECHO, 0);

    if(ans[0] == '\0') ans[0] = '0';
    if(ans[0] >= '0' && ans[0] <= '9') {
      listfile[5] = ans[0];
      setuserfile(listpath, listfile);

      if(aborted == 1) {
        if(more(listpath, NA) != -1) getdata(b_lines - 1, 0, "�T�w�R����"
                                             "(Y/N)?[N]",  ans, 3, LCECHO, 0);
        if(*ans == 'y') {
           unlink(listpath);
           outmsg(msg_del_ok);
        }
      }
      else {
        int reciper;

        stand_title("�s��W��");
        reciper = 0;
        CreateNameList();
        LoadNameList(&reciper, listpath, msg_cc);
        multi_list(&reciper);
        SaveNameList(listpath);
      }
    }
  }
  return FULLUPDATE;
}
