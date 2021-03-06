#include "bbs.h"

extern void m_sysop();
extern void note();
extern char station_list_cn[][15];

int Goodbye() {
  extern void movie(int);
  char genbuf[200];

  /* woju */
  char ans[4];

  setuserfile(genbuf, fn_writelog);
  if (more(genbuf, NA) != -1) {
     getdata(b_lines, 0, "清除(C) 移至備忘錄(M) 保留(R) (C/M/R)?[R]",
        ans, 3, LCECHO, 0);
     if (*ans == 'm') {
        fileheader mymail;
        char title[128], buf[80];

        sethomepath(buf, cuser.userid);
        stampfile(buf, &mymail);

        mymail.savemode = 'H';        /* hold-mail flag */
        mymail.filemode = FILE_READ;
        strcpy(mymail.owner, "[備.忘.錄]");
        strcpy(mymail.title, "熱線[37;41m記錄[m");
        sethomedir(title, cuser.userid);
        append_record(title, &mymail, sizeof(mymail));
        Rename(genbuf, buf);
      }
     else if (*ans == 'c') {
        char buf[80];

        setuserfile(buf, fn_writelog);
        unlink(buf);
     }
  }

  clear();
  movie(0);
  sprintf(genbuf, "您確定要離開【 %s 】嗎(Y/N)？[N] ",
          station_list_cn[cuser.now_stno]);
  getdata(b_lines, 0, genbuf, genbuf, 3, LCECHO, 0);

  if(*genbuf != 'y') return 0;

  movie(999);
  if(cuser.userlevel) {
    getdata(b_lines, 0, "(G)停止冒險 (M)與站長聯絡 (N)冒險的英雄事蹟 [G] ",
      genbuf, 3, LCECHO, 0);
    if (genbuf[0] == 'm')
      mail_sysop();
    else if (genbuf[0] == 'n')
      note();
  }
  if(HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_SYSOP)) t_aloha('O');

  clear();
  prints("[1;36m親愛的 [33m%s(%s)[36m，別忘了再度光臨[45;33m"
    " %s [40;36m！\n以下是您在站內的註冊資料:[0m\n",
    cuser.userid, cuser.username, station_list_cn[cuser.now_stno]);

  user_display(&cuser, 0);
  if(currmode) u_exit("EXIT ");

  pressanykey("準備離線");
  sleep(1);
  reset_tty();
  exit(0);
}
