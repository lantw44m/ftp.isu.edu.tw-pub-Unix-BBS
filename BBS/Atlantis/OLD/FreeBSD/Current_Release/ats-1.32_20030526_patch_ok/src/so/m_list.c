#include "bbs.h"

static char listfile[] = "list.0";
static char msg_cc[] =   "[群組名單]\n";

extern void multi_list(int *);

int m_list() {
  int aborted;
  char genbuf[4];

  getdata(b_lines - 1, 0, "名單檔 (E)編輯 (D)刪除 (Q)取消？[Q] ",
    genbuf, 4, LCECHO, 0);

  if(genbuf[0] == 'd') aborted = 1;
  else if (genbuf[0] == 'e') aborted = 2;
  else aborted = 0;

  if(aborted) {
    char listpath[64], ans[4];

    getdata(b_lines - 1, 0, "請選擇名單 (0-9 ？[0] ", ans, 3, DOECHO, 0);

    if(ans[0] == '\0') ans[0] = '0';
    if(ans[0] >= '0' && ans[0] <= '9') {
      listfile[5] = ans[0];
      setuserfile(listpath, listfile);

      if(aborted == 1) {
        if(more(listpath, NA) != -1) getdata(b_lines - 1, 0, "確定刪除嗎"
                                             "(Y/N)?[N]",  ans, 3, LCECHO, 0);
        if(*ans == 'y') {
           unlink(listpath);
           outmsg(msg_del_ok);
        }
      }
      else {
        int reciper;

        stand_title("編輯名單");
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
