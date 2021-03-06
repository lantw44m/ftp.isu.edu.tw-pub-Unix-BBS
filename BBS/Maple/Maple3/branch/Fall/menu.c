/*-------------------------------------------------------*/
/* menu.c	( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* target : menu/help/movie routines		 	 */
/* create : 95/03/29				 	 */
/* update : 97/03/29				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"


extern UCACHE *ushm;


static int
welcome()
{
  film_out(FILM_WELCOME, -1);

  return 0;
}


/* ----------------------------------------------------- */
/* 離開 BBS 站						 */
/* ----------------------------------------------------- */


#define	FN_NOTE_PAD	"run/note.pad"
#define	FN_NOTE_TMP	"run/note.tmp"
#define	NOTE_MAX	100
#define	NOTE_DUE	48


typedef struct
{
  time_t tpad;
  char msg[356];
}      Pad;


int
pad_view()
{
  int fd, count;
  Pad *pad;

  if ((fd = open(FN_NOTE_PAD, O_RDONLY)) < 0)
    return XEASY;

  clear();
  move(0, 23);
  outs("\033[1;32m【 酸 甜 苦 辣 留 言 板 】\033[m\n\n");
  count = 0;

  mgets(-1);

  for (;;)
  {
    pad = mread(fd, sizeof(Pad));
    if (!pad)
    {
      vmsg(NULL);
      break;
    }

    outs(pad->msg);
    if (++count == 5)
    {
      move(b_lines, 0);
      outs("請按 [SPACE] 繼續觀賞，或按其他鍵結束：");
      if (vkey() != ' ')
	break;

      count = 0;
      move(2, 0);
      clrtobot();
    }
  }

  close(fd);
  return 0;
}


static void
pad_draw()
{
  int i, cc, fdr;
  FILE *fpw;
  Pad pad;
  char *str, buf[3][80];

  do
  {
    buf[0][0] = buf[1][0] = buf[2][0] = '\0';
    move(12, 0);
    clrtobot();
    outs("\n請留言 (至多三行)，按[Enter]結束");
    for (i = 0; (i < 3) &&
      vget(16 + i, 0, "：", buf[i], 78, DOECHO); i++);
    cc = vans("(S)存檔觀賞 (E)重新來過 (Q)算了？[S] ");
    if (cc == 'q' || i == 0)
      return;
  } while (cc == 'e');

  time(&(pad.tpad));

  str = pad.msg;
  sprintf(str, "\033[37;44m %s (%s)", cuser.userid, cuser.username);
  for (cc = strlen(str); cc < 60; cc++)
    str[cc] = ' ';
  sprintf(str + cc, "\033[34;47m %s \033[m\n%s\n%s\n%s\n",
    Etime(&(pad.tpad)), buf[0], buf[1], buf[2]);

  f_cat(FN_NOTE_ALL, str);

  if (!(fpw = fopen(FN_NOTE_TMP, "w")))
    return;

  fwrite(&pad, sizeof(pad), 1, fpw);

  if ((fdr = open(FN_NOTE_PAD, O_RDONLY)) >= 0)
  {
    Pad *pp;

    i = 0;
    cc = pad.tpad - NOTE_DUE * 60 * 60;
    mgets(-1);
    while (pp = mread(fdr, sizeof(Pad)))
    {
      fwrite(pp, sizeof(Pad), 1, fpw);
      if ((++i > NOTE_MAX) || (pp->tpad < cc))
	break;
    }
    close(fdr);
  }

  fclose(fpw);

  rename(FN_NOTE_TMP, FN_NOTE_PAD);
  pad_view();
}


static int
goodbye()
{
  switch (vans("G)再別秋亭夜話 M)報告站長 N)留言板 Q)取消？[Q] "))
  {
  case 'g':
  case 'y':
    break;
    
  case 'm':
    mail_sysop();
    break;

  case 'n':
    if (cuser.userlevel)
      pad_draw();
    break;

  case 'q':
  default:
    return XEASY;

  }
  
#ifdef  BMW_SAVE
  bmw_save();                   /* lkchu.981201: 熱訊記錄處理 */
#endif

  clear();
  prints("親愛的 \033[32m%s(%s)\033[m，別忘了再度光臨【 %s 】\n"
    "以下是您在站內的註冊資料:\n",
    cuser.userid, cuser.username, str_site);
  acct_show(&cuser, 0);
  vmsg(NULL);
  u_exit("EXIT ");
  exit(0);
}


/* ----------------------------------------------------- */
/* help & menu processring				 */
/* ----------------------------------------------------- */


void
vs_head(title, mid)
  char *title, *mid;
{
  char buf[40], ttl[60];
  int spc;
  usint ufo;
#ifdef	COLOR_HEAD
  int color = (time(0) % 7) + 41;	/* lkchu.981201: random color */
#endif

  if (mid == NULL)
  {
    move(0, 0);
    clrtoeol();
    mid = str_site;
  }
  else
  {
    clear();
  }

  spc = strlen(mid);
  ufo = cutmp->ufo;

  if (!*title)
  {
    title++;

    if (ufo & UFO_BIFF)
      cutmp->ufo = ufo ^ UFO_BIFF;     /* 看過就算 */
  }
  else
  {
    if (ufo & UFO_BIFF)
    {
      mid = "\033[41;37;5m 郵差來按鈴了! \033[0;1;33;44m";
      spc = 15;
    }
    else if (spc > 46)
    {
      spc = 44;
      memcpy(ttl, mid, spc);
      mid = ttl;
      mid[spc] = '\0';
    }
  }

  spc = 67 - strlen(title) - spc - strlen(currboard);
  ufo = 1 - spc & 1;
  memset(buf, ' ', spc >>= 1);
  buf[spc] = '\0';

#ifdef	COLOR_HEAD
  prints("\033[1;%2d;37m【%s】%s\033[33m%s\033[1;%2d;37m%s\033[37m看板《%s》\033[m\n",
    color, title, buf, mid, color, buf + ufo, currboard);
#else
  prints("\033[1;44;37m【%s】%s\033[33m%s%s\033[37m看板《%s》\033[m\n",
    title, buf, mid, buf + ufo, currboard);
#endif

}


/* ------------------------------------- */
/* 動畫處理				 */
/* ------------------------------------- */


static char footer[160];


void
movie()
{
  static int orig_flag = -1;
  static time_t uptime = -1;
  static char flagmsg[16];
  static char datemsg[16];

  int ufo;
  time_t now;

  ufo = cuser.ufo;
  time(&now);

  /* Thor: it is depend on which state */

  if ((bbsmode < M_CLASS) && (ufo & UFO_MOVIE))
  {
    static int tag = FILM_MOVIE;

    tag = film_out(tag, 1);
  }

  /* Thor: 同時 顯示 呼叫器 好友上站 隱身 */

  ufo &= UFO_PAGER | UFO_ALOHA | UFO_CLOAK | UFO_QUIET;
  if (orig_flag != ufo)
  {
    orig_flag = ufo;
    sprintf(flagmsg,
      "%s %s%s%s",
      (ufo & UFO_PAGER) ? "關" : "開",
      (ufo & UFO_ALOHA) ? "友 " : "",
      (ufo & UFO_QUIET) ? "靜 " : "",
      (ufo & UFO_CLOAK) ? "隱 " : "");
  }

  if (now > uptime)
  {
    struct tm *ptime;

    ptime = localtime(&now);
    sprintf(datemsg, "[%d/%d 星期%.2s ",
      ptime->tm_mon + 1, ptime->tm_mday,
      "天一二三四五六" + (ptime->tm_wday << 1));

    uptime = now + 86400 - ptime->tm_hour * 3600 -
      ptime->tm_min * 60 - ptime->tm_sec;
  }
  ufo = (now - (uptime - 86400)) / 60;

  /* Thor.980913: 註解: 最常見呼叫 movie()的時機是每次更新 film, 在 60秒以上,
                        故不需針對 xx:yy 來特別作一字串儲存以加速 */

  sprintf(footer, "\033[1;44;36m%s%d:%02d] \033[1;46;37m [訪客] \033[1;46;33m%d\033[1;46;37m 人 [到此一遊] \033[1;46;33m%-12s\033[1;46;37m[呼叫器]\033[1;46;33m%s\033[m",
    datemsg, ufo / 60, ufo % 60,
    ushm->count, cuser.userid, flagmsg);
  outz(footer);
}


static int
x_sysload()
{
  double *load;
  char buf[80];

  load = ushm->sysload;
  sprintf(buf, "系統負載 %.2f %.2f %.2f", load[0], load[1], load[2]);
  vmsg(buf);
  return XEASY;
}


typedef struct
{
  void *func;
  /* int (*func) (); */
  usint level;
  int umode;
  char *desc;
}      MENU;


#define	MENU_XPOS	18
#define	MENU_YPOS	13
#define	MENU_LOAD	1
#define	MENU_DRAW	2
#define	MENU_FILM	4


#define	PERM_MENU	PERM_PURGE


extern MENU menu_main[];


/* ----------------------------------------------------- */
/* administrator's maintain menu			 */
/* ----------------------------------------------------- */


static MENU menu_admin[] =
{
  m_user, PERM_ACCOUNTS, M_SYSTEM,
  "User       使用者資料",

  m_newbrd, PERM_SYSOP, M_SYSTEM,
  "Board      開闢新看板",

  x_sysload, PERM_SYSOP, M_SYSTEM,
  "Load       系統負載",

#ifdef	HAVE_REGISTER_FORM
  m_register, PERM_ACCOUNTS, M_SYSTEM,
  "Register   審核註冊表單",
#endif

  m_xfile, PERM_SYSOP, M_XFILES,
  "Xfile      編輯系統檔案",

#ifdef	HAVE_ADM_SHELL
  x_csh, PERM_SYSOP, M_SYSTEM,
  "Shell      執行系統 Shell",
#endif

#ifdef	HAVE_REPORT
  m_trace, PERM_SYSOP, M_SYSTEM,
  "Trace      設定是否記錄除錯資訊",
#endif

  menu_main, PERM_MENU + 'U', M_ADMIN,
  "系統維護"
};


/* ----------------------------------------------------- */
/* mail menu						 */
/* ----------------------------------------------------- */


static int
XoMbox()
{
  xover(XZ_MBOX);
  return 0;
}


static MENU menu_mail[] =
{
  XoMbox, PERM_READMAIL, M_RMAIL,
  "Read       閱\讀信件",

  m_send, PERM_BASIC, M_SMAIL,
  "Send       [1;31m站內寄信[0m",

#ifdef MULTI_MAIL  /* Thor.981009: 防止愛情幸運信 */
  mail_list, PERM_BASIC, M_SMAIL,
  "List       群組寄信",
#endif

  m_internet, PERM_INTERNET, M_SMAIL,
  "Internet   寄信到 Internet",

  mail_sysop, PERM_BASIC, M_SMAIL,
  "Yes Sir!   寄信給站長",

  menu_main, PERM_MENU + 'R', M_MAIL,
  "電子郵件"
};


/* ----------------------------------------------------- */
/* Talk menu						 */
/* ----------------------------------------------------- */


static int
XoUlist()
{
  xover(XZ_ULIST);
  return 0;
}


static MENU menu_talk[] =
{
  XoUlist, 0, M_LUSERS,
  "Users      [1;33m使用者列表[0m",

  t_pal, PERM_BASIC, M_PAL,
  "Friend     好友通訊錄",

  t_pager, PERM_BASIC, M_XMODE,
  "Pager      切換呼叫器",

  t_query, 0, M_QUERY,
  "Query      查詢網友",

#if 0
  t_talk, PERM_PAGE, M_PAGE,
  "Talk       找人聊天",
#endif

  t_chat, PERM_CHAT, M_CHAT,
  "Chat       秋亭聊天室",

  t_recall, PERM_BASIC, M_XMODE,
  "Write      回顧前幾次熱訊",

#ifdef LOGIN_NOTIFY
  t_loginNotify, PERM_PAGE, M_XMODE,
  "Notify     設定系統網友協尋",
#endif

  menu_main, PERM_MENU + 'U', M_TMENU,
  "休閒聊天"
};


/* ----------------------------------------------------- */
/* User menu						 */
/* ----------------------------------------------------- */


static MENU menu_user[] =
{
  u_info, PERM_BASIC, M_XMODE,
  "Info       設定個人資料與密碼",

  u_addr, PERM_BASIC, M_XMODE,
  "Address    設定 E-mail Address",

  u_setup, 0, M_UFILES,
  "Setup      [1;32m設定操作模式[0m",

  pad_view, 0, M_READ,
  "Note       觀看心情留言板",
  
  welcome, 0, M_READ,
  "Welcome    觀賞歡迎畫面",

  u_lock, PERM_BASIC, M_XMODE,
  "Lock       鎖定螢幕",

#ifdef	HAVE_REGISTER_FORM
  u_register, PERM_BASIC, M_UFILES,
  "Register   填寫《註冊申請單》",
#endif

  u_xfile, PERM_BASIC, M_UFILES,
  "Xfile      編輯個人檔案",

  menu_main, PERM_MENU + 'S', M_UMENU,
  "個人設定"
};


/* ----------------------------------------------------- */
/* call menu						 */
/* ----------------------------------------------------- */

void alpha0943(), alpha0946(), fitel();

static MENU menu_call[] =
{
  alpha0943, PERM_VALID, M_XMODE,
  "3Alpha0943   [1;37;41mAlphacall 0943[0m",
      
  alpha0946, PERM_VALID, M_XMODE,
  "6Alpha0946   [1;37;45mAlphacall 0946[0m",

  fitel, PERM_VALID, M_XMODE,
  "8Fitel       [1;37;44m大眾電信  0948[0m",
  
  menu_main, PERM_MENU + '3', M_XMENU,
  "網路傳呼"
};

int gagb(), BlackJack(),x_dice(),p_gp(),p_nine(),p_race(),bingo();

static MENU menu_game[] =
{
  gagb, PERM_BASIC, M_XAXB,
  "GAGB         [1;31m※猜數字※[0m",   

  BlackJack, PERM_BASIC, M_BLACKJACK,
  "BlackJack    [1;32m★黑傑克★[0m",

  x_dice, PERM_BASIC, M_DICE,
  "Dice         [1;33m◆西八拉◆[0m",
  
  p_gp, PERM_BASIC, M_GP,
  "GP           [1;34m¥金撲克梭哈¥[0m",
  
  p_nine, PERM_BASIC, M_NINE,
  "Nine         [1;35m〒天地九九〒[0m",

  bingo, PERM_BASIC , M_BINGO,
  "Bingo        [1;36m∮賓果賓果∮[0m",
  
  p_race, PERM_SYSOP, M_RACE,
  "Race         [1;37;41m● 賽馬 ●[0m",   
  
  menu_main, PERM_MENU + 'G', M_GMENU,
  "遊戲專區" 
};
 

/* ----------------------------------------------------- */
/* main menu						 */
/* ----------------------------------------------------- */


static int
Gem()
{
  XoGem("gem/.DIR", "", (HAS_PERM(PERM_SYSOP) ? GEM_SYSOP : GEM_USER));
  return 0;
}


static MENU menu_main[] =
{
  menu_admin, PERM_ADMIN, M_ADMIN,
  "0[37mAdmin    【 [1;31m系統維護區[0;37m 】[0m",

  Gem, 0, M_GEM,
  "A[37mnnounce  【 [1;32m精華公佈欄[0;37m 】[0m",

  Boards, 0, M_BOARD,
  "B[37moards    【 [1;33m佈告討論區[0;37m 】[0m",

  Class, 0, M_CLASS,
  "C[37mlass     【 [1;34m分組討論區[0;37m 】[0m",

  menu_mail, PERM_BASIC, M_MAIL,
  "M[37mail      【 [1;35m私人信件區[0;37m 】[0m",

  menu_talk, 0, M_TMENU,
  "T[37malk      【 [1;36m休閒聊天區[0;37m 】[0m",

  menu_user, 0, M_UMENU,
  "U[37mser      【 [1;31m個人設定區[0;37m 】[0m",

  menu_game, PERM_VALID , M_GMENU,
  "P[37mlay      【  [1;32m遊戲專區[0;37m  】[0m",
  
  menu_call, PERM_VALID, M_XMENU,
  "N[37met       【 [1;33m網路傳呼區[0;37m 】[0m",
  
#if 0
  Select, 0, M_BOARD,
  "Select     選擇看板",
#endif

  goodbye, 0, M_XMODE,
  "G[1;36moodbye    再別秋亭夜話，輕輕的我走了[0m",

  NULL, PERM_MENU + 'B', M_MMENU,
  "主功\能表"
};


void
menu()
{
  MENU *menu, *mptr, *table[12];
  usint level, mode;
  int cc, cx;			/* current / previous cursor position */
  int max, mmx;			/* current / previous menu max */
  int cmd, depth;
  char *str;

  mode = MENU_LOAD | MENU_DRAW | MENU_FILM;
  menu = menu_main;
  level = cuser.userlevel;
  depth = mmx = 0;

  for (;;)
  {
    if (mode & MENU_LOAD)
    {
      for (max = -1;; menu++)
      {
	cc = menu->level;
	if (cc & PERM_MENU)
	{

#ifdef	MENU_VERBOSE
	  if (max < 0)		/* 找不到適合權限之功能，回上一層功能表 */
	  {
	    menu = (MENU *) menu->func;
	    continue;
	  }
#endif

	  break;
	}
	if (cc && !(cc & level))
	  continue;

	table[++max] = menu;
      }

      if (mmx < max)
	mmx = max;

      if ((depth == 0) && (cutmp->ufo & UFO_BIFF))
	cmd = 'M';
      else
	cmd = cc ^ PERM_MENU;	/* default command */
      utmp_mode(menu->umode);
    }

    if (mode & MENU_DRAW)
    {
      if (mode & MENU_FILM)
      {
	clear();
	movie();
	cx = -1;
      }

      vs_head(menu->desc, NULL);

      mode = 0;
      do
      {
	move(MENU_YPOS + mode, MENU_XPOS + 2);
	if (mode <= max)
	{
	  mptr = table[mode];
	  str = mptr->desc;
	  outs("(\033[1;36m");
	  outc(*str++);
	  outs("\033[m)");
	  outs(str);
	}
	clrtoeol();
      } while (++mode <= mmx);

      mmx = max;
      mode = 0;
    }

    switch (cmd)
    {
    case KEY_DOWN:
      if (++cc <= max)
	break;

    case KEY_HOME:
      cc = 0;
      break;

    case KEY_UP:
      if (--cc >= 0)
	break;

    case KEY_END:
      cc = max;
      break;

    case '\n':
    case KEY_RIGHT:
      mptr = table[cc];
      utmp_mode(cmd = mptr->umode);

      if (cmd <= M_XMENU)
      {
	menu->level = PERM_MENU + mptr->desc[0];
	menu = (MENU *) mptr->func;
	mode = MENU_LOAD | MENU_DRAW;
	depth++;
	continue;
      }

      {
	int (*func) ();

	func = mptr->func;
	mode = (*func) ();
      }

      utmp_mode(menu->umode);

      if (mode == XEASY)
      {
#if 0
	move(b_lines, 0);

        /* Thor.980728: 再查看看 visio.c */
        /* clrtoeol(); */ /* lkchu: 新版會出現亂碼, 3.02 則不會 */

	outs(footer);
#endif
#if 1
        /* Thor.980826: 用 outz 就不用 move + clrtoeol了 */
	outz(footer);
#endif

	mode = 0;
      }
      else
      {
	mode = MENU_DRAW | MENU_FILM;
      }

      cmd = mptr->desc[0];
      continue;

#ifdef EVERY_Z
    case Ctrl('Z'):
      every_Z();		/* Thor: ctrl-Z everywhere */
      goto menu_key;
#endif

    case KEY_LEFT:
    case 'e':
      if (depth > 0)
      {
	menu->level = PERM_MENU + table[cc]->desc[0];
	menu = (MENU *) menu->func;
	mode = MENU_LOAD | MENU_DRAW;
	depth--;
	continue;
      }

      cmd = 'G';

    default:

      if (cmd >= 'a' && cmd <= 'z')
	cmd -= 0x20;

      cc = 0;
      for (;;)
      {
	if (table[cc]->desc[0] == cmd)
	  break;
	if (++cc > max)
	{
	  cc = cx;
	  goto menu_key;
	}
      }
    }

    if (cc != cx)
    {
      if (cx >= 0)
      {
	move(MENU_YPOS + cx, MENU_XPOS);
	outc(' ');
      }
      move(MENU_YPOS + cc, MENU_XPOS);
      outc('>');
      cx = cc;
    }
    else
    {
      move(MENU_YPOS + cc, MENU_XPOS + 1);
    }

menu_key:

    cmd = vkey();
  }
}
