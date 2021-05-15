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
     getdata(b_lines, 0, "²M°£(C) ²¾¦Ü³Æ§Ñ¿ý(M) «O¯d(R) (C/M/R)?[R]",
        ans, 3, LCECHO, 0);
     if (*ans == 'm') {
        fileheader mymail;
        char title[128], buf[80];

        sethomepath(buf, cuser.userid);
        stampfile(buf, &mymail);

        mymail.savemode = 'H';        /* hold-mail flag */
        mymail.filemode = FILE_READ;
        strcpy(mymail.owner, "[³Æ.§Ñ.¿ý]");
        strcpy(mymail.title, "¼ö½u[37;41m°O¿ý[m");
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
  sprintf(genbuf, "±z½T©w­nÂ÷¶}¡i %s ¡j¶Ü(Y/N)¡H[N] ",
          station_list_cn[cuser.now_stno]);
  getdata(b_lines, 0, genbuf, genbuf, 3, LCECHO, 0);

  if(*genbuf != 'y') return 0;

  movie(999);
  if(cuser.userlevel) {
    getdata(b_lines, 0, "(G)°±¤î«_ÀI (M)»P¯¸ªøÁpµ¸ (N)«_ÀIªº­^¶¯¨ÆÂÝ [G] ",
      genbuf, 3, LCECHO, 0);
    if (genbuf[0] == 'm')
      mail_sysop();
    else if (genbuf[0] == 'n')
      note();
  }
  if(HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_SYSOP)) t_aloha('O');

  clear();
  prints("[1;36m¿Ë·Rªº [33m%s(%s)[36m¡A§O§Ñ¤F¦A«×¥úÁ{[45;33m"
    " %s [40;36m¡I\n¥H¤U¬O±z¦b¯¸¤ºªºµù¥U¸ê®Æ:[0m\n",
    cuser.userid, cuser.username, station_list_cn[cuser.now_stno]);

  user_display(&cuser, 0);
  if(currmode) u_exit("EXIT ");

  pressanykey("·Ç³ÆÂ÷½u");
  sleep(1);
  reset_tty();
  exit(0);
}
