#include "bbs.h"

extern void do_hold_mail(char *, char *, char *);

int m_group() {
  int aborted = 0;
  char ans[4];
  static char eans[4];
  int j;
  char genbuf[200];

  setuserfile(genbuf, "mlist.0");
  j = strlen(genbuf) - 1;

  getdata(b_lines - 1, 0, "Email-list (E)編輯 (D)刪除 (M)發信 (Q)取消？[Q] ",
          ans, 4, LCECHO, 0);

  if(ans[0] == 'd') aborted = 1;
  if(ans[0] == 'e') aborted = 2;
  if(ans[0] == 'm') aborted = 3;

  if(aborted) {
    if(!getdata(b_lines - 1, 0, "請選擇 Email-list 檔(0-9)？ ", eans, 4,
      DOECHO, eans)) eans[0] = '0';

    if(eans[0] >= '0' && eans[0] <= '9') {
      genbuf[j] = eans[0];
      if(aborted == 1) {
        unlink(genbuf);
        outs(msg_del_ok);
      }
      else if(aborted == 2) {
        aborted = vedit(genbuf, NA);
        if(!aborted) outs("更新完畢");
      }
      else if (more(genbuf, NA) != -1) {
         char address[STRLEN];
         char xtitle[TTLEN];
         char fpath[STRLEN] = "";
         FILE* fp;

         getdata(b_lines - 1, 0, "主題：", xtitle, TTLEN, DOECHO, 0);
         if (!*xtitle)
            return -2;
         curredit |= EDIT_MAIL;
         setutmpmode(SMAIL);

         if (vedit(fpath, NA) == -1) {
            unlink(fpath);
            clear();
            return -2;
         }
         clear();
         more(genbuf, NA);
         getdata(b_lines - 1, 0, "確定要寄給列表中的人嗎? (Y/N) [Y]", ans, 4,
                 LCECHO, 0);
         switch (*ans)
         {
         case 'n':
            outs("\n信件已取消");
            break;
         default:
            fp = fopen(genbuf, "r");
            while (fgets(address, STRLEN, fp)) {
               strtok(address, " ,\n");
               if (*address) {
                  outmsg(address);
                  refresh();
                  bbs_sendmail(fpath, xtitle, address);
               }
            }
            fclose(fp);
            getdata(b_lines - 1, 0, "已順利寄出，是否自存底稿(Y/N)？[N]",
               ans, 4, LCECHO, 0);
            if (*ans == 'y') {
               sprintf(save_title, "%s (mlist.%c)", xtitle, *eans);
               do_hold_mail(fpath, 0, cuser.userid);
            }
         }
         unlink(fpath);
      }
    }
    pressanykey(NULL);
  }
  return 0;
}
