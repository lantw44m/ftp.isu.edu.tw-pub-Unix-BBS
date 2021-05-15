/* --------------------------------------------- */
/* ¦C¥X©Ò¦³µù¥U¨Ï¥ÎªÌ     update: Dopin 11/04/02 */
/* --------------------------------------------- */

#include "bbs.h"

extern struct UCACHE *uidshm;
int usercounter, totalusers, showrealname;
usint u_list_special;

static int u_list_CB(userec *uentp) {
  static int i;
  char permstr[8], *ptr;
  register int level;

  if(uentp == NULL) {
    clear();
    prints("[1;37;44m  ¨Ï¥ÎªÌ¥N¸¹   %-26.26s¤W¯¸  ¤å³¹  %s  ³Ìªñ¥úÁ{¤é´Á  "
           "      [0m", showrealname ? "¯u¹ê©m¦W" : "ºï¸¹¼ÊºÙ",
           HAS_PERM(PERM_SEEULEVELS) ? "µ¥¯Å" : "");
    i = 1;
    return 0;
  }

  if(uentp->userid[0] == '\0') return 0;
  if(uentp->userlevel < u_list_special) return 0;

  if(i == b_lines) {
    i = pressanykey("¤wÅã¥Ü %6d/%6d ¤H [%d%%]    (¡ö)Â÷¶}",
                     usercounter, totalusers, usercounter * 100 / totalusers);
    if(i == KEY_LEFT) return QUIT;

    i = 1;
  }

  if (i == 1) {
    move(1, 0);
    clrtobot();
  }

  level = uentp->userlevel;
  strcpy(permstr, "----");

  if(level & PERM_SYSOP) permstr[0] = 'S';
#ifdef USE_ACCOUNTS_PERM
  else if(level & PERM_ACCOUNTS) permstr[0] = 'A';
#endif
  else if(level & PERM_DENYPOST) permstr[0] = 'p';

#ifdef USE_BOARDS_MANAGER
  if(level & (PERM_EXTRA1)) permstr[1] = 'B';
#endif
  else if(level & (PERM_BM)) permstr[1] = 'b';

  if(level & (PERM_XEMPT)) permstr[2] = 'X';
  else if(level & (PERM_LOGINOK)) permstr[2] = 'R';

  if(level & (PERM_CLOAK | PERM_SEECLOAK)) permstr[3] = 'C';

  ptr = (char *) Cdate(&uentp->lastlogin);
  ptr[18] = '\0';
  prints("  %-12s %-26.26s%.5d  %.5d  %s  %s\n",
         uentp->userid, showrealname ? uentp->realname : uentp->username,
         uentp->numlogins, uentp->numposts,
         HAS_PERM(PERM_SEEULEVELS) ? permstr : "", ptr);

  usercounter++;
  i++;
  return 0;
}

int u_list() {
  char genbuf[3];

  setutmpmode(LAUSERS);
  showrealname = u_list_special = usercounter = 0;
  totalusers = uidshm->number;

  if(HAS_PERM(PERM_SEEULEVELS)) {
    getdata(b_lines - 1, 0, "Æ[¬Ý [1]¯S®íµ¥¯Å (2)¥þ³¡¡H[1] ",
            genbuf, 3, DOECHO, 0);
    if(genbuf[0] != '2') u_list_special = 32;
  }

  if(HAS_PERM(PERM_SYSOP)) {
    getdata(b_lines - 1, 0, "Åã¥Ü [1]¯u¹ê©m¦W (2)¼ÊºÙ¡H[1] ", genbuf, 3,
            DOECHO, 0);
    if(genbuf[0] != '2') showrealname = 1;
  }

  u_list_CB(NULL);

  if(apply_record(fn_passwd, u_list_CB, sizeof(userec)) == -1) {
    pressanykey(msg_nobody);
    return XEASY;
  }

  pressanykey("Áä¤J¥kÃä©Ò¥Ü«öÁäÂ÷¶}¦Cªí");
  return 0;
}
