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
    mid = "[41;33;5m   �H�c�̭����s�H��I  [m[1m"COLOR1;

/*
 * CyberMax:
 *           spc �O�ǰt mid ���j�p.
 */
    spc = 22;
  }
  else if(check_personal_note(1,NULL))
  {
    mid = "[43;37;5m    �����������d����   [m[1m"COLOR1;
    spc = 22;
  }
  else if (dashf(BBSHOME"/register.new") && HAS_PERM(PERM_ACCOUNTS))
  {
    mid = "[45;33;5m  ���s���ϥΪ̵��U�o!  [m[1m"COLOR1;
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
    currmode & MODE_SELECT ? "1m�t�C" :
    currmode & MODE_DIGEST ? "5m��K" : 
    currmode & MODE_DELETED ? "3m�^��" : 
//    currmode & MODE_MYMAN ? "7m�p�H" : 
    "7m�ݪO", currboard);

}

// wildcat : ����u�Ϊ��ſ�� :p
int
null_menu()
{
  pressanykey("�o�O�@�Ӫſ�� :p ");
  return 0;
}


/* ------------------------------------ */
/* �ʵe�B�z                              */
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
  
  static char *myweek[]= {"��", "�@","�G","�T","�|","��","��"};
//  char *msgs[] = {"�}", "��", "��", "��","��"};
  time_t stay_time, now = time(NULL);
  struct tm *ptime = localtime(&now);  
  
  resolve_garbage(); /* get film cache */
  if (currstat > BACKSTREET)
     return;

  i = ptime->tm_wday;
  stay_time = now - cuser.lastlogin;

  sprintf(mystatus,"[1m"
    "[33m �u�W:[37m%3d[33m�H "
    "[33m �b��:[37m%-13s[33m "
    "[33m ���d:[37m%2d��%2d��%2d�� "
    "[33m �ɶ�:[37m%2d��%2d��P��%s%2d�I%2d�� [m",
//    "�C[37m%6d%c,[33m%5d%c"
//    "[32m�]:%-2.2s [35m[%-12.12s]",
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
//    currutmp->birth ? "�ͤ�Ыȭ�" : film->today_is);
    
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
  prints(COLOR1"[1m       �\\  ��          ��    ��                 �� [[1;33mCtrl-Z[37m] [31m�D�U               [m");

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
  m_user,       PERM_ACCOUNTS,  "UUser          [�ϥΪ̸��]",0,
  search_key_user,PERM_ACCOUNTS,"FFind User     [�j�M�ϥΪ�]",0,
  m_newbrd,     PERM_BOARD,     "NNew Board     [ �}�s�ݪO ]",0,
  m_board,      PERM_BOARD,     "SSet Board     [ �]�w�ݪO ]",0,
  m_register,   PERM_BBSADM,    "RRegister      [ �f���U�� ]",0,
  XFile,        PERM_XFILE,     "XXfile         [ �t���ɮ� ]",0,
  reload_cache, PERM_SYSOP,     "CCache Reload  [ ��s���A ]",0,
  adm_givegold, PERM_SECRETARY, "GGive $$       [ �o����� ]",0,
  
//"SO/xyz.so:x_bbsnet",PERM_SYSOP,"BBBSNet      [ �s�u�u�� ]",1,
//m_mclean, PERM_BBSADM, "MMail Clean    �M�z�ϥΪ̭ӤH�H�c",0,

  x_reg,        PERM_ACCOUNTS,  "MMerge         [�f�֭ײz��]",0,
  u_list,       PERM_ACCOUNTS,  "LList users    [ ���U�W�� ]",0,

  NULL, 0, NULL,0};


/* ----------------------------------------------------- */
/* class menu                                            */
/* ----------------------------------------------------- */

int board(), local_board(), Boards(), ReadSelect() ,
    New(),Favor(),favor_edit(),good_board(),voteboard();

static struct MENU classlist[] = {
  voteboard,PERM_LOGINOK,"VVoteBoard    [�ݪO�s�p�t��]",0,
  board, 0,              "CClass        [���������ݪO]",0,
  New, 0,                "NNew          [�Ҧ��ݪO�C��]",0,
  local_board,0,         "LLocal        [�����ݪO�C��]",0,
  good_board,0,          "GGoodBoard    [  �u�}�ݪO  ]",0,
  Favor,PERM_LOGINOK,    "BBoardFavor   [�ڪ��̷R�ݪO]",0,
favor_edit,PERM_LOGINOK, "FFavorEdit�   [�s��ڪ��̷R]",0,
  ReadSelect, 0,         "SSelect       [  ��ܬݪO  ]",0,
  refresh_movie, 0,      "MMovie        [  �ʺA�ݪO  ]",0,
  NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* RPG menu                                             */
/* ----------------------------------------------------- */
int t_pk(),rpg_help();

struct MENU rpglist[] = {
  rpg_help,0,	"HHelp       ���C������/�W�h����",0,
  "SO/rpg.so:rpg_uinfo",
    0,		"UUserInfo   �ۤv�����A",1,
  "SO/rpg.so:rpg_race_c",
    PERM_BASIC,"JJoin       �[�J¾�~�u�|(�ݤ��T����)",1,
  "SO/rpg.so:rpg_guild",
    0,		"GGuild      ¾�~�u�|",1,
  "SO/rpg.so:rpg_train",
    0,		"TTrain      �V�m��(�I�u��)",1,
  "SO/rpg.so:rpg_top",
    0,		"LListTop    �ϥΪ̱Ʀ�]",1,
  "SO/rpg.so:rpg_edit",
    PERM_SYSOP,	"QQuery      �d�߭ק�USER���",1,
//  "SO:rpg.so:rpg_shop",0,           "SShop       �˳ưө�(�I�u��)",1,
//  "SO:rpg.so:t_pk",0,                 "PPK         ���� PK",1,
NULL, 0, NULL,0};

int
rpg_menu()
{
  domenu(RMENU, "�����t�C��", 'U', rpglist);
  return 0;
}
/* ----------------------------------------------------- */
/* Ptt money menu                                        */
/* ----------------------------------------------------- */
static struct MENU finlist[] = {
  "SO/buy.so:bank",     PERM_BASIC,  "11Bank           [1;36m�q�l�Ȧ�[m       ECE BANK",1,
  "SO/pip.so:pip_money",PERM_BASIC,  "22ChickenMoney   [1;32m�����I���B[m     �������q�l����",1,
 "SO/song.so:ordersong",PERM_LOGINOK,"33OrderSong      [1;33m�q�l�I�q��[m     $",1,
  "SO/buy.so:p_cloak",  PERM_BASIC,  "44Cloak          �{������/�{��",1,
  "SO/buy.so:p_from",   PERM_BASIC,  "55From           �{�ɭק�G�m",1,
  "SO/buy.so:p_exmail", PERM_LOGINOK,"66Mailbox        �ʶR�H�c�W��",1,
//  "SO/buy.so:p_ulmail", PERM_LOGINOK,"00NoLimit        �H�c�L�W��",1,
NULL, 0, NULL,0};

int
finance()
{
  domenu(FINANCE, "�ӫ~�j��", '1', finlist);
  return 0;
}

/* ----------------------------------------------------- */
/* money menu 2 */
/* ----------------------------------------------------- */
int u_kill();
static struct MENU backstreetlist[] = {
  "SO/buy.so:p_ffrom",  PERM_LOGINOK,"11PlaceBook      �G�m�ק��_��",1,
  "SO/buy.so:p_fcloak", PERM_LOGINOK,"22UltimaCloak    �׷������j�k",1,
  "SO/buy.so:p_scloak", PERM_LOGINOK,"33SeeCloak       �·t�z����",1,
  "SO/buy.so:time_machine", PERM_LOGINOK, "44TimeMachine    �·t�ɥ���",1,
  "SO/buy.so:darkwish", PERM_BASIC,  "55DarkWish       �·t�\\�@��",1,
  u_kill,               PERM_LOGINOK,"66Suicide        �·t�ܩR��",0,
  
NULL, 0, NULL,0};

int
backstreet()
{
  domenu(BACKSTREET, "�a�U��D", '1', backstreetlist);
  return 0;
}
      

#ifdef HAVE_GAME

/* ----------------------------------------------------- */
/* NetGame menu                                          */
/* ----------------------------------------------------- */

struct MENU netgame_list[] = {
  "SO/xyz.so:x_mj",0,
    "QQkmj      �� �����±N��",1,
  "SO/xyz.so:x_big2",0,
    "BBig2      �� �����j�ѤG",1,
  "SO/xyz.so:x_chess",PERM_LOGINOK,
    "CChess     �� �����U�H��",1,
NULL, 0, NULL,0};

int
netgame_menu()
{
  domenu(NETGAME, "�����s�u�C��", 'Q', netgame_list);
  return 0;
}
/* ----------------------------------------------------- */
/* Game menu                                             */
/* ----------------------------------------------------- */

static struct MENU gamelist[] = {
  rpg_menu,PERM_BASIC,
    "RRPG        �� �����t�C��           ",0,
  netgame_menu,PERM_LOGINOK,
    "NNetGame    �� �����s�u�C��           $100s/��",0,
  "SO/gamble.so:ticket_main",PERM_BASIC,
    "GGamble     �� ���ֽ�L             $100s/�i",1,
  "SO/marie.so:mary_m",0,
    "MMarie      �� �p���R�ֶ�             �̧C���O $1s",1,
  "SO/race.so:p_race",PERM_LOGINOK,
    "RRace       �� �}�G���ɽ޳�           �̧C���O $1s",1,
  "SO/bingo.so:bingo",PERM_BASIC,
    "BBingo      �� �դ뻫�G��             �̧C���O $1s",1,
  "SO/gagb.so:gagb",0,
    "??A?B       �� �q�q�q�Ʀr             �̧C���O $1s",1,
  "SO/guessnum.so:fightNum",0,
    "FFightNum   �� ��Բq�Ʀr             �̧C���O $1s",1,
  "SO/bj.so:BlackJack",PERM_LOGINOK,
    "JJack       �� �դ�³ǧJ             �̧C���O $1s",1,
  "SO/nine.so:p_nine",PERM_LOGINOK,
    "999         �� �Ѧa�[�E�E�            �̧C���O $1s",1,
  "SO/dice.so:x_dice",PERM_BASIC,
    "DDice       �� ��K�Խ��             ",1,
  "SO/gp.so:p_gp",PERM_LOGINOK,
    "PPoke       �� �����J����             �̧C���O $1s",1,
  "SO/pip.so:p_pipple",PERM_LOGINOK,
    "CChicken    �� ���йq�l��             �K�O���A��!!",1,
//  "SO/xyz.so:x_tetris",0,
//    "TTetris     �� �Xù�����             �K�O���A��",1,
  "SO/mine.so:Mine",0,
    "LLandMine   �� �l�z��a�p             �K�O���A��",1,
  "SO/poker.so:p_dragon",0,
    "11���s      �� ���դ������s           �K�O���A��",1,
  "SO/chessmj.so:p_chessmj",PERM_LOGINOK,
    "22ChessMJ   �� �H�ѳ±N               �̧C���O $1s",1,
  "SO/seven.so:p_seven",PERM_LOGINOK,
    "33Seven     �� �䫰�C�i               �̧C���O $1s",1,
  "SO/bet.so:p_bet",PERM_LOGINOK,
    "44Bet       �� �ƨg��L               �̧C���O $1s",1,
//  "SO/stock.so:p_stock",PERM_BASIC,
//    "SStock      �� ���Ъѥ�",1,

/*  x_bridgem,PERM_LOGINOK,"OOkBridge    �i ���P�v�� �j",0,*/
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

  t_users,      0,              "LList          [�u�W�W��]",0,
  t_pager,      PERM_BASIC,     "PPager         [�������A]",0,
  t_idle,       PERM_LOGINOK,   "IIdle          [��w�ù�]",0,
  t_query,      PERM_LOGINOK,   "QQueryUser     [�d��User]",0,
  t_talk,       PERM_PAGE,      "TTalk          [��H���]",0,
  "SO/chat.so:t_chat",PERM_CHAT,"CChatRoom      [�s�u���]",1,
  t_display,    PERM_BASIC,     "DDisplay       [���y�^�U]",0,
NULL, 0, NULL,0};

/*-------------------------------------------------------*/
/* powerbook menu                                        */
/* ----------------------------------------------------- */

int null_menu(),my_gem(),my_allpost();

static struct MENU powerlist[] = {

  "SO/bbcall.so:bbcall_menu",PERM_BASIC,"MMessager      [ �q�T�� ]",1,
  "SO/mn.so:show_mn",    PERM_BASIC,    "NNoteMoney     [ �O�b�� ]",1,
  my_gem,              PERM_LOGINOK,    "GGem           [�ڪ����]",0,
  my_allpost,          PERM_LOGINOK,    "AAllPost       [�ڪ��峹]",0,
  null_menu,           PERM_BASIC,      "------ ������ �\\�� ------",0,
  "SO/pnote.so:p_read",PERM_BASIC,      "PPhone Answer  [ť���d��]",1,
  "SO/pnote.so:p_call",PERM_LOGINOK,    "CCall phone    [�e�X�d��]",1,
  "SO/pnote.so:p_edithint",PERM_LOGINOK,"RRecord        [���w���]",1,

NULL, 0, NULL,0};

int
PowerBook()
{
  domenu(POWERBOOK, "�U�Τ�U", 'N', powerlist);
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
  PowerBook,	PERM_BASIC,	"PPowerBook     [�U�Τ�U]",0,
  u_info,       PERM_BASIC,     "IInfo          [�ק���]",0,
  u_habit,      PERM_BASIC,     "HHabit         [�ߦn�]�w]",0,
  ListMain,     PERM_LOGINOK,   "LList          [�]�w�W��]",0, 
//  re_m_postnotify,PERM_LOGINOK, "PPostNotify    [�峹�q��]",0,
  u_editfile,   PERM_LOGINOK,   "FFileEdit      [�ӤH�ɮ�]",0,
  u_cloak,      PERM_CLOAK,     "CCloak         [���αK�k]",0,
  u_justify,    PERM_BASIC,     "JJustify       [�H�{�ҫH]",0,
  u_register,   PERM_BASIC,     "RRegister      [����U��]",0,
//  u_list,       PERM_LOGINOK,   "UUsers         [���U�W��]",0,
NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* XYZ menu                                              */
/* ----------------------------------------------------- */
#ifdef HAVE_GAME
int
game_list()
{
  domenu(GAME, "�����C�ֳ�", 'R', gamelist);
  return 0;
}
#endif

/* Ptt */
int note();

static struct MENU servicelist[] = {
#ifdef HAVE_GAME
  game_list,    0,              "PPlay          [�|�֤���]",0,
#endif
  finance,      0,              "FFinance       [�ӫ~�j��]",0,
  backstreet,   0,              "BBackStreet    [�a�U��D]",0,
  "SO/vote.so:all_vote",
                PERM_LOGINOK,   "VVote          [�벼����]",1,
  note,         PERM_LOGINOK,   "NNote          [�g�d���O]",0,
  "SO/xyz.so:show_hint_message",
                0,              "HHint          [�оǺ��F]",1,
//  "SO/indict.so:x_dict",
//                0,              "DDictionary    [�ʬ����]",1,
//  "SO/xyz.so:x_cdict",
//                PERM_BASIC,     "CCD-67         [�q�l�r��]",1,
//  "SO/bbcall.so:bbcall_main",
//                PERM_LOGINOK,   "BB.B.Call      [�����ǩI]",1,
//"SO/tv.so:catv",PERM_BASIC,     "TTV-Program    [�q���`�جd��]",1,
//
//"SO/railway.so:railway2",
//                PERM_BASIC,     "RRailWay       [�����ɨ�d��]",1,
//"SO/fortune.so:main_fortune",
//                PERM_BASIC,     "QQueryFortune  [�ӤH�B�չw��]",1,

#ifdef HAVE_GOPHER
  "SO/xyz.so:x_gopher",PERM_LOGINOK,"GGopher       �� �@�s�p�a�����A�� ��",1,
#endif
#ifdef BBSDOORS
//  "SO/xyz.so:x_bbsnet", PERM_LOGINOK, "DDoor          [�ɪŹh�D]",1,
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
  m_new,        PERM_READMAIL,  "RNew           [�\\Ū�s�H]",0,
  m_read,       PERM_READMAIL,  "RRead          [�H��C��]",0,
  m_send,       PERM_LOGINOK,   "SSend          [�����H�H]",0,
  mail_list,    PERM_LOGINOK,   "MMailist       [�s�ձH�H]",0,
  RejectMail,   PERM_LOGINOK,   "BBadMail       [�׫H�W��]",0,
  m_internet,   PERM_INTERNET,  "IInternet      [�����l��]",0,
  setforward,   PERM_LOGINOK,   "FForward       [���H��H]",0,
  m_sysop,      PERM_BASIC,     "OOp Mail       [���گ���]",0,
  mail_mbox,    PERM_INTERNET,  "ZZip           [���]���]",0,
  mail_all,     PERM_SYSOP,     "AAll           [�t�γq�i]",0,
NULL, 0, NULL,0};


/* ----------------------------------------------------- */
/* main menu                                             */
/* ----------------------------------------------------- */

static int
admin()
{
  domenu(ADMIN, "�����Ѥj", 'R', adminlist);
  return 0;
}

static int
BOARD()
{
  domenu(CLASS, "�ݪO���", 'G', classlist);
  return 0;
}

static int
Mail()
{
  domenu(MAIL, "�l����", 'R', maillist);
  return 0;
}

int
static Talk()
{
  domenu(TMENU, "��ѿ��", 'L', talklist);
  return 0;
}

static int
User()
{
  domenu(UMENU, "�ӤH�]�w", 'H', userlist);
  return 0;
}


static int
Service()
{
  domenu(PMENU, "�U�تA��", 'F', servicelist);
  return 0;
}


int Announce(), Boards(), Goodbye(),Log(),board();


struct MENU cmdlist[] = {
  admin,        PERM_BBSADM,    "00Admin        [�t�κ޲z]",0,
  Announce,     0,              "AAnnounce      [�Ѧa���]",0,
  BOARD,        0,              "BBoard         [�ݪO�\\��]",0,
  board,        0,              "CClass         [�����ݪO]",0,
  Mail,         PERM_BASIC,     "MMail          [�q�l�l��]",0,
  Talk,         0,              "TTalk          [�ͤѻ��a]",0,
  User,         PERM_BASIC,     "UUser          [�ӤH�u��]",0,
  Log,          0,              "HHistory       [���v�y��]",0,
  Service,      0,              "SService       [�U�تA��]",0,
  Goodbye,      0,              "GGoodbye       [���t�d��]",0,
NULL, 0, NULL,0};
/* INDENT ON */
