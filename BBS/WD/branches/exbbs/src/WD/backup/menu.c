/*-------------------------------------------------------*/
/* menu.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : menu/help/m o v i e routines                     */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#include "bbs.h"

/* ----------------------------------------------------- */
/* Menu Commands struct                                  */
/* ----------------------------------------------------- */

static char movie2[11][256] =  {"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"};

/* ------------------------------------- */
/* help & menu processring               */
/* ------------------------------------- */
int refscreen = NA;
extern char *boardprefix;


int
egetch()
{
  int rval;

  while (1)
  {
    rval = igetkey();
    if (talkrequest)
    {
      talkreply();
      refscreen = YEA;
      return rval;
    }
    if (rval != Ctrl('L'))
      return rval;
    redoscr();
  }
}


void
showtitle(title, mid)
  char *title, *mid;
{
  char buf[40];
  int spc, pad;

  spc = strlen(mid);

  if (title[0] == 0)
    title++;
   else if (chkmail(0))
  {
    mid = "[41;33;5m   信箱裡面有新信唷！  [m[1m"COLOR1;

/*
 * CyberMax:
 *           spc 是匹配 mid 的大小.
 */
    spc = 22;
  }
  else if(check_personal_note(1,NULL))
  {
    mid = "[43;37;5m    答錄機中有留言喔   [m[1m"COLOR1;
    spc = 22;
  }
  else if (dashf(BBSHOME"/register.new") && HAS_PERM(PERM_ACCOUNTS))
  {
    mid = "[45;33;5m  有新的使用者註冊囉!  [m[1m"COLOR1;
    spc = 22;
  }

  spc = 66 - strlen(title) - spc - strlen(currboard);
/*
woju
*/
  if (spc < 0)
     spc = 0;
  pad = 1 - spc & 1;
  memset(buf, ' ', spc >>= 1);
  buf[spc] = '\0';

  move(0,0);
  clrtobot();
//  clear();
  prints(COLOR2"  [1;36m%s  "COLOR1"%s[33m%s%s%s[3%s[1m "COLOR2"  [36m%s  [m\n",
    title, buf, mid, buf, " " + pad,
    currmode & MODE_SELECT ? "1m系列" :
    currmode & MODE_DIGEST ? "5m文摘" : 
    currmode & MODE_DELETED ? "3m回收" : 
//    currmode & MODE_MYMAN ? "7m私人" : 
    "7m看板", currboard);

}

// wildcat : 分格線用的空選單 :p
int
null_menu()
{
  pressanykey("這是一個空選單 :p ");
  return 0;
}


/* ------------------------------------ */
/* 動畫處理                              */
/* ------------------------------------ */


#define FILMROW 11
unsigned char menu_row = 3;
unsigned char menu_column = 4;
char mystatus[255];


/* wildcat 1998.8.7 */

void
movie(key)
  int key;
{
  extern struct FILMCACHE *film;
//  static short history[MAX_HISTORY];
  static char buf[256],pbuf[256];
  static int fi=0;
  int i;
  
  static char *myweek[]= {"日", "一","二","三","四","五","六"};
//  char *msgs[] = {"開", "防", "友", "關","拔"};
  time_t stay_time, now = time(NULL);
  struct tm *ptime = localtime(&now);  
  
  resolve_garbage(); /* get film cache */
  if (currstat > BACKSTREET)
     return;

  i = ptime->tm_wday;
  stay_time = now - cuser.lastlogin;

  sprintf(mystatus,"[1m"
    "[33m 線上:[37m%3d[33m人 "
    "[33m 帳號:[37m%-13s[33m "
    "[33m 停留:[37m%2d時%2d分%2d秒 "
    "[33m 時間:[37m%2d月%2d日星期%s%2d點%2d分 [m",
//    "＄[37m%6d%c,[33m%5d%c"
//    "[32mβ:%-2.2s [35m[%-12.12s]",
    count_ulist(),
    cuser.userid,
    stay_time/60/60,
    stay_time/60%60,
    stay_time%60,
    ptime->tm_mon + 1,
    ptime->tm_mday,
    myweek[i],
    ptime->tm_hour,
    ptime->tm_min);
//    (cuser.silvermoney/1000) <= 0 ? cuser.silvermoney : cuser.silvermoney/1000,
//    (cuser.silvermoney/1000) <= 0 ? ' ' : 'k',
//    (cuser.goldmoney/1000) <= 0 ? cuser.goldmoney : cuser.goldmoney/1000,
//    (cuser.goldmoney/1000) <= 0 ? ' ' : 'k',    
//    msgs[currutmp->pager],
//    currutmp->birth ? "生日請客唷" : film->today_is);
    
//  move(1,0);
//  clrtoeol(); 
//  prints(mystatus);

  outmsgline(mystatus, 1);  

  if (HAVE_HABIT(HABIT_MOVIE))
  {
    if((!film->busystate) && film->max_film && (key>0) )
       fi = (fi+1) % film->max_film;
    setapath(pbuf, "Note");
    sprintf(buf, "%s/%s", pbuf, film->notes[fi]);
    if(film->notes[fi][0])
      show_file(buf,13,FILMROW,ONLY_COLOR);
  }
  
  refresh();  
  return;
}

/* ===== end ===== */

void 
refresh_movie ()
{
  movie(1);
}
          
void
show_movie2(key)
  int key;
{
  int i,j,k,num,num1;
  char buf[128],buf1[128],path[128],fn[128],pbuf[128];
  FILE *fp;
  fileheader item, subitem;
  static id=0, id1=0;
  static char notes2[MAX_MOVIE][FNLEN];

  if( (currstat > CLASS) ) return;
  
#ifdef HAVE_NOTE_2

  if(id==0)
  {
    setapath(pbuf, "Note");
    setadir(buf, pbuf);
    num = rec_num(buf, sizeof item);
    for (j = 0; j <= num; j++)
     if (rec_get(buf, &item, sizeof item, j) != -1)
       if (item.title[3]=='[' && item.title[8]==']')
       {
         setapath(path,"Note");
         sprintf(buf1,"%s/%s",path,item.filename);
         setadir(path, buf1);
         num1 = rec_num(path, sizeof item);
         for (k = 0; k <= num1; k++)
           if (rec_get(path, &subitem, sizeof item, k) != -1)
           {
              sprintf(notes2[id++],"%s/%s",
                 item.filename , subitem.filename);
              if (id > 24) break;
           }
         if (id > 24) break;
       }
  }

  if(key==1 || id1==0)
  {
    id1=rand()%id;
    for(i=0;i<11;i++)  
      strcpy(movie2[i],"\0");
  }
  
  setapath(pbuf, "Note");
  sprintf(fn, "%s/%s", pbuf, notes2[id1]);

  if(fp = fopen(fn,"r"))
  {
     i=0;
     while((i<10) && (fgets(movie2[i],255,fp) != NULL))
     {
       i++;
     }
     fclose(fp);
  }

#endif

}

static int
show_menu(p)
  MENU *p;
{
  register int m,n;
  register char *s;
  char buf[256];
  
  movie(0);
  show_movie2(1);
  
  move(2,0);
  prints(COLOR1"[1m       功\  能          說    明                 按 [[1;33mCtrl-Z[37m] [31m求助               [m");

  move(menu_row, 0);
  m=n=0;
  while ((s = p[n].desc)!=NULL || movie2[m][0]!='\0')
  {
    if(s != NULL )
    {
      if (HAS_PERM(p[n].level))
      {
        sprintf(buf,s+2);
        prints("%*s  [1;37m[[36m%c[37m]", menu_column, "", s[1]);
        if( (currstat > CLASS) || movie2[m][0]=='\0' )
          prints("%s\n",buf);
        else
          prints("%-28s[m%s", buf, movie2[m++]);
      }
      n++;
    }
    else
    {
      if( (currstat > CLASS) )
        break;
      else
        prints("%37s%-s", "", movie2[m++] );
    }
  }
  return n - 1;
}


void
domenu(cmdmode, cmdtitle, cmd, cmdtable)
  char *cmdtitle;
  int cmdmode, cmd;
  MENU *cmdtable;
{

  int lastcmdptr;
  int n, pos, total, i;
  int err;
  int chkmailbox();
  static char cmd0[LOGIN];

  if (cmd0[cmdmode])
     cmd = cmd0[cmdmode];

  setutmpmode(cmdmode);

  showtitle(cmdtitle, BoardName);
  total = show_menu(cmdtable);
  move(1,0);
  outs(mystatus);

  lastcmdptr = pos = 0;

  do
  {
    i = -1;

    switch (cmd)
    {
    case KEY_ESC:
       if (KEY_ESC_arg == 'c')
          capture_screen();
       else if (KEY_ESC_arg == 'n') {
          edit_note();
          refscreen = YEA;
       }
       i = lastcmdptr;
       break;
    case Ctrl('N'):
       New();
       refscreen = YEA;
       i = lastcmdptr;
       break;
    case Ctrl('A'):
    {
      int stat0 = currstat;
      currstat = RMAIL;
      if (man() == RC_FULL)
        refscreen = YEA;
      i = lastcmdptr;
      currstat = stat0;
      break;
    }
    case KEY_DOWN:
      i = lastcmdptr;

    case KEY_HOME:
    case KEY_PGUP:
      do
      {
        if (++i > total)
          i = 0;
      } while (!HAS_PERM(cmdtable[i].level));
      break;

    case KEY_END:
    case KEY_PGDN:
      i = total;
      break;

    case KEY_UP:
      i = lastcmdptr;
      do
      {
        if (--i < 0)
          i = total;
      } while (!HAS_PERM(cmdtable[i].level));
      break;

    case KEY_LEFT:
    case 'e':
    case 'E':
      if (cmdmode == MMENU)
        cmd = 'G';
      else if ((cmdmode == MAIL) && chkmailbox())
        cmd = 'R';
      else return;
    default:
       if ((cmd == Ctrl('G') || cmd == Ctrl('S')) && (currstat == MMENU || currstat == TMENU || currstat == XMENU))  {
          if (cmd == Ctrl('S'))
             ReadSelect();
          else if (cmd == Ctrl('G'))
            Read();
          refscreen = YEA;
          i = lastcmdptr;
          break;
        }
      if (cmd == '\n' || cmd == '\r' || cmd == KEY_RIGHT)
      {

        boardprefix = cmdtable[lastcmdptr].desc;

        if(cmdtable[lastcmdptr].mode && DL_get(cmdtable[lastcmdptr].cmdfunc))
        {
          void *p = (void *)DL_get(cmdtable[lastcmdptr].cmdfunc);
          if(p) cmdtable[lastcmdptr].cmdfunc = p;
          else break;
        }

        currstat = XMODE;

        {
          int (*func)() = 0;

          func = cmdtable[lastcmdptr].cmdfunc;
          if(!func) return;
          if ((err = (*func)()) == QUIT)
            return;
        }

        currutmp->mode = currstat = cmdmode;

        if (err == XEASY)
        {
          refresh();
          sleep(1);
        }
        else if (err != XEASY + 1 || err == RC_FULL)
          refscreen = YEA;

        if (err != -1)
          cmd = cmdtable[lastcmdptr].desc[0];
        else
          cmd = cmdtable[lastcmdptr].desc[1];
        cmd0[cmdmode] = cmdtable[lastcmdptr].desc[0];
      }

      if (cmd >= 'a' && cmd <= 'z')
        cmd &= ~0x20;
      while (++i <= total)
      {
        if (cmdtable[i].desc[1] == cmd)
          break;
      }
    }

    if (i > total || !HAS_PERM(cmdtable[i].level))
    {
      continue;
    }

    if (refscreen)
    {
      showtitle(cmdtitle, BoardName);
      show_menu(cmdtable);
      move(1,0);
      outs(mystatus);
      refscreen = NA;
    }

  if(HAS_HABIT(HABIT_LIGHTBAR))
  {
    if(!HAS_PERM(cmdtable[lastcmdptr].level)) lastcmdptr++;
      cursor_bar_clear(menu_row + pos, menu_column,cmdtable[lastcmdptr].desc,movie2[pos]);
  }
  else
    cursor_clear(menu_row + pos, menu_column);

    n = pos = -1;
    while (++n <= (lastcmdptr = i))
    {
      if (HAS_PERM(cmdtable[n].level))
        pos++;
    }

  if(HAS_HABIT(HABIT_LIGHTBAR))
     cursor_bar_show(menu_row + pos, menu_column, cmdtable[lastcmdptr].desc,movie2[pos]);
  else
     cursor_show(menu_row + pos, menu_column);


  } while (((cmd = egetch()) != EOF) || refscreen);

  abort_bbs();
}
/* INDENT OFF */


/* ----------------------------------------------------- */
/* administrator's maintain menu                         */
/* ----------------------------------------------------- */

int m_user(), m_newbrd(), m_board(), m_register(),x_reg(),XFile(),
    search_key_user(),search_bad_id,reload_cache(),adm_givegold(),
    u_list();
/*    ,search_bad_id();*/

#ifdef  HAVE_MAILCLEAN
int m_mclean();
#endif

static struct MENU adminlist[] = {
  m_user,       PERM_ACCOUNTS,  "UUser          [使用者資料]",0,
  search_key_user,PERM_ACCOUNTS,"FFind User     [搜尋使用者]",0,
  m_newbrd,     PERM_BOARD,     "NNew Board     [ 開新看板 ]",0,
  m_board,      PERM_BOARD,     "SSet Board     [ 設定看板 ]",0,
  m_register,   PERM_BBSADM,    "RRegister      [ 審註冊單 ]",0,
  XFile,        PERM_XFILE,     "XXfile         [ 系統檔案 ]",0,
  reload_cache, PERM_SYSOP,     "CCache Reload  [ 更新狀態 ]",0,
  adm_givegold, PERM_SECRETARY, "GGive $$       [ 發放獎金 ]",0,
  
//"SO/xyz.so:x_bbsnet",PERM_SYSOP,"BBBSNet      [ 連線工具 ]",1,
//m_mclean, PERM_BBSADM, "MMail Clean    清理使用者個人信箱",0,

  x_reg,        PERM_ACCOUNTS,  "MMerge         [審核修理機]",0,
  u_list,       PERM_ACCOUNTS,  "LList users    [ 註冊名單 ]",0,

  NULL, 0, NULL,0};


/* ----------------------------------------------------- */
/* class menu                                            */
/* ----------------------------------------------------- */

int board(), local_board(), Boards(), ReadSelect() ,
    New(),Favor(),favor_edit(),good_board(),voteboard();

static struct MENU classlist[] = {
  voteboard,PERM_LOGINOK,"VVoteBoard    [看板連署系統]",0,
  board, 0,              "CClass        [本站分類看板]",0,
  New, 0,                "NNew          [所有看板列表]",0,
  local_board,0,         "LLocal        [站內看板列表]",0,
  good_board,0,          "GGoodBoard    [  優良看板  ]",0,
  Favor,PERM_LOGINOK,    "BBoardFavor   [我的最愛看板]",0,
favor_edit,PERM_LOGINOK, "FFavorEdit?   [編輯我的最愛]",0,
  ReadSelect, 0,         "SSelect       [  選擇看板  ]",0,
  refresh_movie, 0,      "MMovie        [  動態看板  ]",0,
  NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* RPG menu                                             */
/* ----------------------------------------------------- */
int t_pk(),rpg_help();

struct MENU rpglist[] = {
  rpg_help,0,	"HHelp       本遊戲介紹/規則說明",0,
  "SO/rpg.so:rpg_uinfo",
    0,		"UUserInfo   自己的狀態",1,
  "SO/rpg.so:rpg_race_c",
    PERM_BASIC,"JJoin       加入職業工會(需五枚金幣)",1,
  "SO/rpg.so:rpg_guild",
    0,		"GGuild      職業工會",1,
  "SO/rpg.so:rpg_train",
    0,		"TTrain      訓練場(施工中)",1,
  "SO/rpg.so:rpg_top",
    0,		"LListTop    使用者排行榜",1,
  "SO/rpg.so:rpg_edit",
    PERM_SYSOP,	"QQuery      查詢修改USER資料",1,
//  "SO:rpg.so:rpg_shop",0,           "SShop       裝備商店(施工中)",1,
//  "SO:rpg.so:t_pk",0,                 "PPK         不算 PK",1,
NULL, 0, NULL,0};

int
rpg_menu()
{
  domenu(RMENU, "角色扮演遊戲", 'U', rpglist);
  return 0;
}
/* ----------------------------------------------------- */
/* Ptt money menu                                        */
/* ----------------------------------------------------- */
static struct MENU finlist[] = {
  "SO/buy.so:bank",     PERM_BASIC,  "11Bank           [1;36m電子銀行[m       ECE BANK",1,
  "SO/pip.so:pip_money",PERM_BASIC,  "22ChickenMoney   [1;32m雞金兌換處[m     換錢給電子雞用",1,
 "SO/song.so:ordersong",PERM_LOGINOK,"33OrderSong      [1;33m電子點歌機[m     $",1,
  "SO/buy.so:p_cloak",  PERM_BASIC,  "44Cloak          臨時隱身/現身",1,
  "SO/buy.so:p_from",   PERM_BASIC,  "55From           臨時修改故鄉",1,
  "SO/buy.so:p_exmail", PERM_LOGINOK,"66Mailbox        購買信箱上限",1,
//  "SO/buy.so:p_ulmail", PERM_LOGINOK,"00NoLimit        信箱無上限",1,
NULL, 0, NULL,0};

int
finance()
{
  domenu(FINANCE, "商品大街", '1', finlist);
  return 0;
}

/* ----------------------------------------------------- */
/* money menu 2 */
/* ----------------------------------------------------- */
int u_kill();
static struct MENU backstreetlist[] = {
  "SO/buy.so:p_ffrom",  PERM_LOGINOK,"11PlaceBook      故鄉修改寶典",1,
  "SO/buy.so:p_fcloak", PERM_LOGINOK,"22UltimaCloak    終極隱身大法",1,
  "SO/buy.so:p_scloak", PERM_LOGINOK,"33SeeCloak       黑暗透視鏡",1,
  "SO/buy.so:time_machine", PERM_LOGINOK, "44TimeMachine    黑暗時光機",1,
  "SO/buy.so:darkwish", PERM_BASIC,  "55DarkWish       黑暗許\願池",1,
  u_kill,               PERM_LOGINOK,"66Suicide        黑暗奪命丹",0,
  
NULL, 0, NULL,0};

int
backstreet()
{
  domenu(BACKSTREET, "地下街道", '1', backstreetlist);
  return 0;
}
      

#ifdef HAVE_GAME

/* ----------------------------------------------------- */
/* NetGame menu                                          */
/* ----------------------------------------------------- */

struct MENU netgame_list[] = {
  "SO/xyz.so:x_mj",0,
    "QQkmj      ★ 網路麻將場",1,
  "SO/xyz.so:x_big2",0,
    "BBig2      ★ 網路大老二",1,
  "SO/xyz.so:x_chess",PERM_LOGINOK,
    "CChess     ★ 網路下象棋",1,
NULL, 0, NULL,0};

int
netgame_menu()
{
  domenu(NETGAME, "網路連線遊戲", 'Q', netgame_list);
  return 0;
}
/* ----------------------------------------------------- */
/* Game menu                                             */
/* ----------------------------------------------------- */

static struct MENU gamelist[] = {
  rpg_menu,PERM_BASIC,
    "RRPG        ■ 角色扮演遊戲           ",0,
  netgame_menu,PERM_LOGINOK,
    "NNetGame    ■ 網路連線遊戲           $100s/次",0,
  "SO/gamble.so:ticket_main",PERM_BASIC,
    "GGamble     ★ 對對樂賭盤             $100s/張",1,
  "SO/marie.so:mary_m",0,
    "MMarie      ☆ 小瑪麗樂園             最低消費 $1s",1,
  "SO/race.so:p_race",PERM_LOGINOK,
    "RRace       ☆ 糖果屋賽豬場           最低消費 $1s",1,
  "SO/bingo.so:bingo",PERM_BASIC,
    "BBingo      ☆ 盈月賓果園             最低消費 $1s",1,
  "SO/gagb.so:gagb",0,
    "??A?B       ☆ 猜猜猜數字             最低消費 $1s",1,
  "SO/guessnum.so:fightNum",0,
    "FFightNum   ☆ 對戰猜數字             最低消費 $1s",1,
  "SO/bj.so:BlackJack",PERM_LOGINOK,
    "JJack       ☆ 盈月黑傑克             最低消費 $1s",1,
  "SO/nine.so:p_nine",PERM_LOGINOK,
    "999         ☆ 天地久九九?            最低消費 $1s",1,
  "SO/dice.so:x_dice",PERM_BASIC,
    "DDice       ☆ 西八拉賭場             ",1,
  "SO/gp.so:p_gp",PERM_LOGINOK,
    "PPoke       ☆ 金撲克梭哈             最低消費 $1s",1,
  "SO/pip.so:p_pipple",PERM_LOGINOK,
    "CChicken    ◆ 風塵電子雞             免費給你玩!!",1,
//  "SO/xyz.so:x_tetris",0,
//    "TTetris     ◆ 俄羅斯方塊             免費給你玩",1,
  "SO/mine.so:Mine",0,
    "LLandMine   ◆ 勁爆踩地雷             免費給你玩",1,
  "SO/poker.so:p_dragon",0,
    "11接龍      ◆ 測試中的接龍           免費給你玩",1,
  "SO/chessmj.so:p_chessmj",PERM_LOGINOK,
    "22ChessMJ   ☆ 象棋麻將               最低消費 $1s",1,
  "SO/seven.so:p_seven",PERM_LOGINOK,
    "33Seven     ☆ 賭城七張               最低消費 $1s",1,
  "SO/bet.so:p_bet",PERM_LOGINOK,
    "44Bet       ☆ 瘋狂賭盤               最低消費 $1s",1,
//  "SO/stock.so:p_stock",PERM_BASIC,
//    "SStock      ◇ 風塵股市",1,

/*  x_bridgem,PERM_LOGINOK,"OOkBridge    【 橋牌競技 】",0,*/
NULL, 0, NULL,0};
#endif

/* ----------------------------------------------------- */
/* Talk menu                                             */
/* ----------------------------------------------------- */

int t_users(), t_list(), t_idle(), t_query();
int t_pager(), t_talk();
// t_chat();
/* Thor: for ask last call-in message */
int t_display();

static struct MENU talklist[] = {

  t_users,      0,              "LList          [線上名單]",0,
  t_pager,      PERM_BASIC,     "PPager         [切換狀態]",0,
  t_idle,       PERM_LOGINOK,   "IIdle          [鎖定螢幕]",0,
  t_query,      PERM_LOGINOK,   "QQueryUser     [查詢User]",0,
  t_talk,       PERM_PAGE,      "TTalk          [找人聊天]",0,
  "SO/chat.so:t_chat",PERM_CHAT,"CChatRoom      [連線聊天]",1,
  t_display,    PERM_BASIC,     "DDisplay       [水球回顧]",0,
NULL, 0, NULL,0};

/*-------------------------------------------------------*/
/* powerbook menu                                        */
/* ----------------------------------------------------- */

int null_menu(),my_gem(),my_allpost();

static struct MENU powerlist[] = {

  "SO/bbcall.so:bbcall_menu",PERM_BASIC,"MMessager      [ 通訊錄 ]",1,
  "SO/mn.so:show_mn",    PERM_BASIC,    "NNoteMoney     [ 記帳本 ]",1,
  my_gem,              PERM_LOGINOK,    "GGem           [我的精華]",0,
  my_allpost,          PERM_LOGINOK,    "AAllPost       [我的文章]",0,
  null_menu,           PERM_BASIC,      "------ 答錄機 功\能 ------",0,
  "SO/pnote.so:p_read",PERM_BASIC,      "PPhone Answer  [聽取留言]",1,
  "SO/pnote.so:p_call",PERM_LOGINOK,    "CCall phone    [送出留言]",1,
  "SO/pnote.so:p_edithint",PERM_LOGINOK,"RRecord        [錄歡迎詞]",1,

NULL, 0, NULL,0};

int
PowerBook()
{
  domenu(POWERBOOK, "萬用手冊", 'N', powerlist);
  return 0;
}


/* ----------------------------------------------------- */
/* User menu                                             */
/* ----------------------------------------------------- */

extern int u_editfile();
int u_info(), u_register(),u_cloak() , u_habit(),u_justify(),
    PowerBook();
int re_m_postnotify(), ListMain();

static struct MENU userlist[] = {
  PowerBook,	PERM_BASIC,	"PPowerBook     [萬用手冊]",0,
  u_info,       PERM_BASIC,     "IInfo          [修改資料]",0,
  u_habit,      PERM_BASIC,     "HHabit         [喜好設定]",0,
  ListMain,     PERM_LOGINOK,   "LList          [設定名單]",0, 
//  re_m_postnotify,PERM_LOGINOK, "PPostNotify    [文章通知]",0,
  u_editfile,   PERM_LOGINOK,   "FFileEdit      [個人檔案]",0,
  u_cloak,      PERM_CLOAK,     "CCloak         [隱形密法]",0,
  u_justify,    PERM_BASIC,     "JJustify       [寄認證信]",0,
  u_register,   PERM_BASIC,     "RRegister      [填註冊單]",0,
//  u_list,       PERM_LOGINOK,   "UUsers         [註冊名單]",0,
NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* XYZ menu                                              */
/* ----------------------------------------------------- */
#ifdef HAVE_GAME
int
game_list()
{
  domenu(GAME, "網路遊樂場", 'R', gamelist);
  return 0;
}
#endif

/* Ptt */
int note();

static struct MENU servicelist[] = {
#ifdef HAVE_GAME
  game_list,    0,              "PPlay          [育樂中心]",0,
#endif
  finance,      0,              "FFinance       [商品大街]",0,
  backstreet,   0,              "BBackStreet    [地下街道]",0,
  "SO/vote.so:all_vote",
                PERM_LOGINOK,   "VVote          [投票中心]",1,
  note,         PERM_LOGINOK,   "NNote          [寫留言板]",0,
  "SO/xyz.so:show_hint_message",
                0,              "HHint          [教學精靈]",1,
//  "SO/indict.so:x_dict",
//                0,              "DDictionary    [百科全書]",1,
//  "SO/xyz.so:x_cdict",
//                PERM_BASIC,     "CCD-67         [電子字典]",1,
//  "SO/bbcall.so:bbcall_main",
//                PERM_LOGINOK,   "BB.B.Call      [網路傳呼]",1,
//"SO/tv.so:catv",PERM_BASIC,     "TTV-Program    [電視節目查詢]",1,
//
//"SO/railway.so:railway2",
//                PERM_BASIC,     "RRailWay       [火車時刻查詢]",1,
//"SO/fortune.so:main_fortune",
//                PERM_BASIC,     "QQueryFortune  [個人運勢預測]",1,

#ifdef HAVE_GOPHER
  "SO/xyz.so:x_gopher",PERM_LOGINOK,"GGopher       ■ 世新小地鼠伺服器 ←",1,
#endif
#ifdef BBSDOORS
//  "SO/xyz.so:x_bbsnet", PERM_LOGINOK, "DDoor          [時空閘道]",1,
#endif
  NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* mail menu                                             */
/* ----------------------------------------------------- */
int m_new(), m_read(), m_send(),m_sysop(),mail_mbox(),mail_all(),
    setforward(),mail_list(),RejectMail();

#ifdef INTERNET_PRIVATE_EMAIL
int m_internet();
#endif

static struct MENU maillist[] = {
  m_new,        PERM_READMAIL,  "RNew           [閱\讀新信]",0,
  m_read,       PERM_READMAIL,  "RRead          [信件列表]",0,
  m_send,       PERM_LOGINOK,   "SSend          [站內寄信]",0,
  mail_list,    PERM_LOGINOK,   "MMailist       [群組寄信]",0,
  RejectMail,   PERM_LOGINOK,   "BBadMail       [擋信名單]",0,
  m_internet,   PERM_INTERNET,  "IInternet      [網路郵件]",0,
  setforward,   PERM_LOGINOK,   "FForward       [收信轉寄]",0,
  m_sysop,      PERM_BASIC,     "OOp Mail       [托夢站長]",0,
  mail_mbox,    PERM_INTERNET,  "ZZip           [打包資料]",0,
  mail_all,     PERM_SYSOP,     "AAll           [系統通告]",0,
NULL, 0, NULL,0};


/* ----------------------------------------------------- */
/* main menu                                             */
/* ----------------------------------------------------- */

static int
admin()
{
  domenu(ADMIN, "站長老大", 'R', adminlist);
  return 0;
}

static int
BOARD()
{
  domenu(CLASS, "看板選單", 'G', classlist);
  return 0;
}

static int
Mail()
{
  domenu(MAIL, "郵件選單", 'R', maillist);
  return 0;
}

int
static Talk()
{
  domenu(TMENU, "聊天選單", 'L', talklist);
  return 0;
}

static int
User()
{
  domenu(UMENU, "個人設定", 'H', userlist);
  return 0;
}


static int
Service()
{
  domenu(PMENU, "各種服務", 'F', servicelist);
  return 0;
}


int Announce(), Boards(), Goodbye(),Log(),board();


struct MENU cmdlist[] = {
  admin,        PERM_BBSADM,    "00Admin        [系統管理]",0,
  Announce,     0,              "AAnnounce      [天地精華]",0,
  BOARD,        0,              "BBoard         [看板功\能]",0,
  board,        0,              "CClass         [分類看板]",0,
  Mail,         PERM_BASIC,     "MMail          [電子郵件]",0,
  Talk,         0,              "TTalk          [談天說地]",0,
  User,         PERM_BASIC,     "UUser          [個人工具]",0,
  Log,          0,              "HHistory       [歷史軌跡]",0,
  Service,      0,              "SService       [各種服務]",0,
  Goodbye,      0,              "GGoodbye       [有緣千里]",0,
NULL, 0, NULL,0};
/* INDENT ON */
