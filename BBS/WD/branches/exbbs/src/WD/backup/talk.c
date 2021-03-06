
/* talk.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : talk/quety/friend routines                   */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#define _MODES_C_

#include "bbs.h"
#include "type.h"

#ifdef lint
#include <sys/uio.h>
#endif

#define M_INT 15                /* monitor mode update interval */
#define P_INT 20                /* interval to check for page req. in
                                 * talk/chat */
#define IRH 1
#define HRM 2

char *racename[7] = 
    {RACE_NORACE,RACE_POST,RACE_READ,RACE_IDLE,RACE_CHAT,RACE_MSG,RACE_GAME};

struct talk_win
{
  int curcol, curln;
  int sline, eline;
};

typedef struct
{
  user_info *ui;
  time_t idle;
  usint friend;
}      pickup;

extern int bind( /* int,struct sockaddr *, int */ );
extern char* getuserid();
extern struct UTMPFILE *utmpshm;
extern char watermode,no_oldmsg,oldmsg_count;
extern msgque oldmsg[MAX_REVIEW];
extern int cmpuname();
extern char currdirect[];
extern struct FROMCACHE *fcache;

/* -------------------------- */
/* 記錄 friend 的 user number */
/* -------------------------- */

#define PICKUP_WAYS     6
int pickup_way = 0;
int friendcount;
int friends_number;
int override_number;
int rejected_number;
int rejectedcount;
int bfriends_number;
 static int show_friend = 0;
 static int show_board = 0;
 static int show_uid = 0;
 static int show_tty = 0;
 static int show_pid = 0;
char *fcolor[16] = {"[37m",    // 其他人  
                    "[1;32m",  // my f
                    "[1;33m",  // be f
                    "[1;36m",  // be f & my f
                    "[1;34m",  // board f
                    "[1;32m",
                    "[1;33m",
                    "[1;36m",  // bfwm & my & bf
                    "[1;31m",  // bad guy
                    "[1;35m",  // hide
                    "[1;37m",  // myself
                    "[37m",
                    "[37m",
                    "[37m",
                    "[37m",
                    "[37m"
                   };
char *msg_pickup_way[PICKUP_WAYS] = { "嗨！朋友",
                                      "網友代號",
                                      "網友動態",
                                      "發呆時間",
                                      "來自何方",
                                      "使用看板",
                                    };
char *talk_uent_buf;
char save_page_requestor[40];
char page_requestor[40];
static FILE* flog;

void friend_load();

int is_rejected(user_info *ui);

char *
modestring(uentp, simple)
  user_info *uentp;
  int simple;
{
  static char modestr[40];
  static char *notonline="不在站上";
  register int mode = uentp->mode;
  register char *word;

  word = ModeTypeTable[mode];

  if (!(HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_SEECLOAK)) &&
      ( ((uentp->invisible & 1) && !(currutmp->invisible & 2))
      || (is_rejected(uentp) & 2)))
    return (notonline);
  else if (mode == EDITING) {
     sprintf(modestr, "E:%s",
         ModeTypeTable[uentp->destuid < EDITING ? uentp->destuid : EDITING]);
     word = modestr;
  }
  else if (!mode && *uentp->chatid == 1)
  {
     if (!simple)
        sprintf(modestr, "回應 %s", getuserid(uentp->destuid));
     else
        sprintf(modestr, "回應呼叫");
  }
  else if ((!mode && *uentp->chatid == 3) || mode == DBACK)
  {
     sprintf(modestr, "%s準備中", uentp->cursor);
  }
  else if (!mode)
    return (uentp->destuid == 6) ? uentp->chatid :
      IdleTypeTable[(0 <= uentp->destuid & uentp->destuid < 6) ?
                    uentp->destuid: 0];
  else if (simple)
    return (word);

  else if (uentp->in_chat & mode == CHATING)
    sprintf(modestr, "%s (%s)", word, uentp->chatid);
  else if (mode== TALK)
   {
    if (is_hidden(getuserid(uentp->destuid)))    /* Leeym 對方(紫色)隱形 */
      sprintf(modestr, "%s", "自言自語中"); /* Leeym 大家自己發揮吧！ */
    else
      sprintf(modestr, "%s %s", word, getuserid(uentp->destuid));
   }
  else if (mode==HIT)
  {
    sprintf(modestr, "被%s擊中了", uentp->hit);
  }
  else if (mode != PAGE && mode != QUERY)
    return (word);
  else
    sprintf(modestr, "%s %s", word, getuserid(uentp->destuid));

  return (modestr);
}


int
cmpuids(uid, urec)
  int uid;
  user_info *urec;
{
  return (uid == urec->uid);
}

int     /* Leeym 從 FireBird 移植改寫過來的 */
is_hidden(user)
char *user;
{
    int tuid;
    user_info *uentp;

  if (
      (tuid = getuser(user))
      &&
        (uentp = (user_info *) search_ulist(cmpuids, tuid))         
      && (
            !(uentp->invisible & 1 && !(currutmp->invisible & 2))
            || HAS_PERM(PERM_SYSOP)
            || HAS_PERM(PERM_SEECLOAK)           
         )
     )
     return 1;       /* x */
  else
     return 0;       /* 可以交談 */
}

/*
woju
*/
int
cmppids(pid, urec)
  pid_t pid;
  user_info *urec;
{
  return (pid == urec->pid);
}


/* ------------------------------------- */
/* routines for Talk->Friend             */
/* ------------------------------------- */

/*
static int
can_override(userid, whoasks)
  char *userid;
  char *whoasks;
{
  char buf[STRLEN];
  sethomefile(buf, userid, fn_overrides);
  return belong(buf, whoasks);
}
*/

int
is_friend(ui)
  user_info *ui;
{
  register ushort unum, id,  hit, *myfriends;

  /* 判斷對方是否為我的朋友 ? */

  hit = 0;
  unum = ui->uid;
  myfriends = currutmp->friend;
  while (id = *myfriends++)
  {
    if (unum == id)
    {
      hit |= 1;
      friends_number++;
      break;
    }
  }

  /* 判斷我是否為對方的朋友 ? */

  myfriends = ui->friend;
  while (id = *myfriends++)
  {
    if (usernum == id)
    {
      override_number++;
      hit |= 2;
      break;
    }
  }

  /* 看板好友 */
  
  if(currutmp->brc_id && ui->brc_id == currutmp->brc_id)
  {
     hit |= 4;
     bfriends_number++;
  }
         
  return hit;
}

/*
static int
be_rejected(userid)
  char *userid;
{
  char buf[STRLEN];

  sethomefile(buf, userid, fn_reject);
  return belong(buf, cuser.userid);
}
*/

int
is_rejected(ui)
  user_info *ui;
{
  register ushort unum, id, hit, *myrejects;

  if (PERM_HIDE(ui))
     return 0;
  /* 判斷對方是否為我的仇人 ? */

  hit=0;
  unum = ui->uid;
  myrejects = currutmp->reject;
  while (id = *myrejects++)
  {
    if (unum == id)
    {
      hit |= 1;
      rejected_number++;
      break;
    }
  }

  /* 判斷我是否為對方的仇人 ? */

  myrejects = ui->reject;
  while (id = *myrejects++)
  {
    if (usernum == id)
    {
//      if (hit & IRH)
//         --rejected_number;
      hit |= 2;
      break;
    }
  }
  return hit;
}


/* ------------------------------------- */
/* 真實動作                              */
/* ------------------------------------- */

static void
my_kick(uentp)
  user_info *uentp;
{
  char genbuf[200];

  getdata(1, 0, msg_sure_ny, genbuf, 4, LCECHO,0);
  clrtoeol();
  if (genbuf[0] == 'y')
  {
    sprintf(genbuf, "%s (%s)", uentp->userid, uentp->username);
    log_usies("KICK ", genbuf);
    if ((kill(uentp->pid, SIGHUP) == -1) && (errno == ESRCH))
      memset(uentp, 0, sizeof(user_info));
    /* purge_utmp(uentp); */
    outs("踢出去囉");
  }
  else
    pressanykey(msg_cancel);
}

my_query(uid)
  char *uid;
{
  extern char currmaildir[];
  int tuid,i;
  unsigned long int j;
  user_info *uentp;
  userec muser;
  rpgrec rpguser;
  char *money[20] = {"小乞丐","大乞丐","貧窮","清寒","清寒人家",
                     "普通","小康","豪門","富裕","小小富翁",
                     "小富翁","中富翁","大富翁","小財神","中財神",
                     "大財神","招財貓","招財貓神","神","GOD"};

  if (tuid = getuser(uid))
  {
    memcpy(&muser, &xuser, sizeof(userec));
    memcpy(&rpguser, &rpgtmp, sizeof(rpgrec));
    move(1, 0);
    clrtobot();
    move(1, 0);
    setutmpmode(QUERY);
    currutmp->destuid = tuid;
    
    j = muser.silvermoney + (10000 * muser.goldmoney);
    for(i=0;i<20 && j>10;i++) j /= 10;
    
    prints("[ 帳  號 ] %-28.28s[ 暱  稱 ] %s\n",muser.userid,muser.username);
    
    if ((pal_have(muser.userid, cuser.userid)==M_PAL)
         || HAS_PERM(PERM_SYSOP)
         || !strcmp(muser.userid, cuser.userid) )
    {
      char *sex[8] = { MSG_BIG_BOY, MSG_BIG_GIRL,
                       MSG_LITTLE_BOY, MSG_LITTLE_GIRL,
                       MSG_MAN, MSG_WOMAN, MSG_PLANT, MSG_MIME };
      prints("[ 性  別 ] %-28.28s",sex[muser.sex%8]);
    }

    prints("[ 心  情 ] [1;33m%s[m\n",muser.feeling);
    
    uentp = (user_info *) search_ulist(cmpuids, tuid);    
    
    prints("[目前動態] ");
    
    if (uentp)
    {
      if((PERM_HIDE(currutmp)
         || !(is_rejected(uentp) & 2)
         || is_friend(uentp) & 2)
         && (!PERM_HIDE(uentp) || !strcmp(uentp->userid, cuser.userid))
         )
      {
         prints("[1;36m%-28.28s[m", modestring(uentp, 0) );
         prints("[ 武  器 ] [1;32m%s[m\n", uentp->cursor);
      }
      else
         prints("[1;30m不在站上[m\n");
    }
    else
      prints("[1;30m不在站上[m\n");
         
    prints("[新信未讀] ");
    
    sprintf(currmaildir, "home/%s/mailbox/.DIR", muser.userid);
       
    outs(chkmail(1) ? "[1;5;33m有[m\n" : "[1;30m無[m\n");
    
    sprintf(currmaildir, "home/%s/mailbox/.DIR", cuser.userid);

    chkmail(1);
    prints("[1;36m%s[m\n", msg_seperator);
    prints("[上站地點] %-28.28s",muser.lasthost[0] ? muser.lasthost : "(不詳)");
    prints("[上站時間] %s\n",Cdate(&muser.lastlogin));
    prints("[上站次數] %-28d",muser.numlogins);
    prints("[發表文章] %d 篇\n",muser.numposts);
    prints("[人氣指數] %-28d[好奇指數] %d\n",muser.bequery,muser.toquery);
    prints("[水球傳情] 收 %d / 發 %d \n",muser.receivemsg,muser.sendmsg);
    if(HAS_PERM(PERM_SYSOP))
      prints("[前次查詢] %-28.28s[前次被查] %s\n",muser.toqid,muser.beqid);

    prints("[1;36m%s[m\n", msg_seperator);

    prints("[ 職  業 ] %-28.28s",racename[rpguser.race]);
    prints("[ 經  濟 ] %s\n",money[i]);
    if (HAS_PERM(PERM_SYSOP) || !strcmp(muser.userid, cuser.userid))
      prints("[ 金  幣 ] %-28ld[ 銀  幣 ] %-ld\n[ 經  驗 ] %ld\n"
             ,muser.goldmoney,muser.silvermoney,muser.exp);
    
    if(strcmp(muser.beqid,cuser.userid))
    {
      strcpy(muser.beqid,cuser.userid);
      ++muser.bequery;
      substitute_record(fn_passwd, &muser,sizeof(userec), tuid); 
    }
    if(strcmp(muser.userid,cuser.toqid))
    {
      strcpy(cuser.toqid,muser.userid);
      ++cuser.toquery;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
    }

    showplans(uid);
    return RC_FULL;
  }
  update_data(); 
  return RC_NONE;
  /* currutmp->destuid = 0; */
}

int

my_write(pid, hint, msg_mode)
  pid_t pid;
  char *hint;
  int msg_mode;
{
  int len, a;
  char msg[80], genbuf[256];
  uschar pager0;
  FILE *fp;
  struct tm *ptime;
  time_t now;
  user_info *uin ;
  extern msgque oldmsg[MAX_REVIEW];

  uschar mode0 = currutmp->mode;
//  char c0 = currutmp->chatid[0];
  int currstat0 = currstat;
  
  
  if (!pid)   return 0;

  setutmpmode(DBACK);

  time(&now);
  ptime = localtime(&now);

  if(msg_mode==MSG_NORMAL)
  {
    char inputbuf[4];
    if (!(len = getdata(1, 0, hint, msg, 60, DOECHO,0)))
    {
      pressanykey("還沒有準備好..");
//      currutmp->chatid[0] = c0;
      currutmp->mode = mode0;
      currstat = currstat0;
      watermode = 0;
      return 0;
    }
  
    if(watermode > 0)
    {
      a = (no_oldmsg - watermode + MAX_REVIEW )%MAX_REVIEW;
      uin = (user_info*)search_ulist(cmppids, oldmsg[a].last_pid);
    }
    else
      uin = (user_info*)search_ulist(cmppids, pid);
  
    strip_ansi(msg,msg,0);
    if(!uin || !uin->userid)
    {
       pressanykey("找不到對方! ?_?");
      currstat = currstat0;
      currutmp->mode = mode0;
      watermode=0;
      return 0;
    }
    
    sprintf(genbuf, "攻擊 %s :%.40s....?[Y] ",uin->userid, msg);

    getdata(1, 0, genbuf, inputbuf, 3, LCECHO,0);
    if (inputbuf[0] == 'n') {
//      currutmp->chatid[0] = c0;
//      currutmp->mode = mode0;
      currstat = currstat0;
      currutmp->mode = mode0;
      watermode=0;
      return 0;
    }
  }
  else
  { 
     uin = (user_info*)search_ulist(cmppids, pid); 
     strcpy(msg, hint);
     strip_ansi(msg, msg, 0);
  }
  
  if(!uin || !uin->userid)
  {
    if(msg_mode==MSG_NORMAL)
       pressanykey("找不到對方了! p_p");
    currstat = currstat0;
    currutmp->mode = mode0;
    watermode=0;
    return 0;
  }
  
  if(  !HAS_PERM(PERM_SYSOP)
       && ( 
            (uin->msgcount >= MAXMSGS)
            || ( uin->pager==1 )
            || ( uin->pager==2 && (is_rejected(uin) & 2) )
            || ( uin->pager==3 && !(is_friend(uin) & 2) ) 
            || ( uin->pager==4)
          )
    )
    {
       if(msg_mode==MSG_NORMAL)
           pressanykey("對方可能正在忙碌中。");
       currstat = currstat0;
       currutmp->mode = mode0;
       watermode=0;
       return 0;   
    } 



  now = time(0);
  
  if(msg_mode!=MSG_ALOHA)
  {
     sethomefile(genbuf, uin->userid, fn_writelog);
     if (fp = fopen(genbuf, "a"))
     {
        fprintf(fp, COLOR2"[1m★ [37m%s %s %s [0m[%s]\n",
          cuser.userid, (msg_mode==MSG_BROADCAST) ? "[33;41m廣播:" : "", msg, Cdatelite(&now));
        fclose(fp);
     }
     sethomefile(genbuf, cuser.userid, fn_writelog);
     if (fp = fopen(genbuf, "a"))
     {
        fprintf(fp, "To %s: %s [%s]\n", uin->userid, msg,  Cdatelite(&now));
        fclose(fp);
        ++cuser.sendmsg;
        cuser.exp += rpguser.race == 5 ? 15*rpguser.level : 5;
        substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
     }
  }
     
   pager0 = uin->pager;
   uin->pager = 4;
   uin->msgs[uin->msgcount].last_pid = currpid;
   strcpy(uin->msgs[uin->msgcount].last_userid, currutmp->userid);
   strcpy(uin->msgs[uin->msgcount++].last_call_in, msg);
   strcpy(uin->hit, currutmp->cursor);
   uin->pager = pager0;

   len=kill(uin->pid, SIGUSR2);
   if(len==-1)
   {
      if(msg_mode == MSG_NORMAL)
         pressanykey("沒有擊中! o_o");
      currstat = currstat0;
      currutmp->mode = mode0;
      watermode=0;
      return 0;
   }
   
   demoney(1);
   
   currstat = currstat0;
   currutmp->mode = mode0;
   watermode=0;
   return 1;
}

static char t_display_new_flag =0;

void
t_display_new()  // 水球回顧
{
   int i,j;
   char buf[256];

   if(t_display_new_flag) return;

   else t_display_new_flag = 1;
//   clear();
   j=oldmsg_count;
   if (j>=20) j=20;
   if(oldmsg_count && watermode > 0)
     {
         move(2,0);
         clrtoeol();
         outs(
" [1;34m───────[37m水[34m─[37m球[34m─[37m回[34m─[37m顧[34m─────────"COLOR1" [Ctrl-R]往下切換 [34;40m────── [m");
         for(i=0 ; i < j ;i++)
                {
                 int a = (no_oldmsg - i - 1 + MAX_REVIEW )%MAX_REVIEW;
                 move(i+3,0);
                 clrtoeol();
                 if(watermode-1 != i)
                    sprintf(buf,"[1;37m %s %s[m",
                         oldmsg[a].last_userid,oldmsg[a].last_call_in);
                 else
                    sprintf(buf,"[1m[36m>%s %s[m",
                         oldmsg[a].last_userid,oldmsg[a].last_call_in);
                 outs(buf);
                }
          move(i+3,0);
          outs(
" [1;34m───────────────────────"COLOR1" [Ctrl-T]往上切換 [40;34m──────[m ");
     }
  t_display_new_flag =0;
}

/* Thor: for ask last call-in message */

int
t_display()
{
  char genbuf[256];
  
  uschar mode0 = currutmp->mode;
  int currstat0 = currstat;
    
  setutmpmode(DBACK);

  setuserfile(genbuf, fn_writelog);
  if (more(genbuf, YEA) != -1)
  {
    switch (getans("清除(C) 移至備忘錄(M) 保留(R) (C/M/R)?[R]"))
    {
      case 'm':
        mail2user(cuser, "熱線[37;41m記錄[m", genbuf);
      case 'c':
        unlink(genbuf);
      default:
        break;
    }
    currstat = currstat0;
    currutmp->mode = mode0;
    return RC_FULL;
  }
  currstat = currstat0;
  currutmp->mode = mode0;
  return RC_NONE;
}


/* ----------------------------------------------------- */

static void
do_talk_nextline(twin)
  struct talk_win *twin;
{
   twin->curcol = 0;
   if (twin->curln < twin->eline)
      ++(twin->curln);
   else
      region_scroll_up(twin->sline, twin->eline);
   move(twin->curln, twin->curcol);
}


static void
do_talk_char(twin, ch)
  struct talk_win *twin;
  int ch;
{
  extern int dumb_term;
  extern screenline* big_picture;
  screenline* line;
  int i;
  char ch0;
  char buf[81];

  if (isprint2(ch))
  {
    ch0 = big_picture[twin->curln].data[twin->curcol];
    if (big_picture[twin->curln].len < 79)
       move(twin->curln, twin->curcol);
    else
       do_talk_nextline(twin);
    outc(ch);
    ++(twin->curcol);
    line =  big_picture + twin->curln;
    if (twin->curcol < line->len) {      /* insert */
       ++(line->len);
       memcpy(buf, line->data + twin->curcol, 80);
       save_cursor();
       do_move(twin->curcol, twin->curln);
       ochar(line->data[twin->curcol] = ch0);
       for (i = twin->curcol + 1; i < line->len; i++)
          ochar(line->data[i] = buf[i - twin->curcol - 1]);
       restore_cursor();
    }
    line->data[line->len] = 0;
    return;
  }

  switch (ch)
  {
  case Ctrl('H'):
  case '\177':
    if (twin->curcol == 0)
    {
      return;
    }
    line =  big_picture + twin->curln;
    --(twin->curcol);
    if (twin->curcol < line->len) {
       --(line->len);
       save_cursor();
       do_move(twin->curcol, twin->curln);
       for (i = twin->curcol; i < line->len; i++)
          ochar(line->data[i] = line->data[i + 1]);
       line->data[i] = 0;
       ochar(' ');
       restore_cursor();
    }
    move(twin->curln, twin->curcol);
    return;

  case Ctrl('D'):
     line =  big_picture + twin->curln;
     if (twin->curcol < line->len) {
        --(line->len);
        save_cursor();
        do_move(twin->curcol, twin->curln);
        for (i = twin->curcol; i < line->len; i++)
           ochar(line->data[i] = line->data[i + 1]);
        line->data[i] = 0;
        ochar(' ');
        restore_cursor();
     }
     return;
  case Ctrl('G'):
    bell();
    return;
  case Ctrl('B'):
     if (twin->curcol > 0) {
        --(twin->curcol);
        move(twin->curln, twin->curcol);
     }
     return;
  case Ctrl('F'):
     if (twin->curcol < 79) {
        ++(twin->curcol);
        move(twin->curln, twin->curcol);
     }
     return;
  case Ctrl('A'):
     twin->curcol = 0;
     move(twin->curln, twin->curcol);
     return;
  case Ctrl('K'):
     clrtoeol();
     return;
  case Ctrl('Y'):
     twin->curcol = 0;
     move(twin->curln, twin->curcol);
     clrtoeol();
     return;
  case Ctrl('E'):
     twin->curcol = big_picture[twin->curln].len;
     move(twin->curln, twin->curcol);
     return;
  case Ctrl('M'):
  case Ctrl('J'):
     line =  big_picture + twin->curln;
     strncpy(buf, line->data, line->len);
     buf[line->len] = 0;
     if (dumb_term)
       outc('\n');
     do_talk_nextline(twin);
     break;
  case Ctrl('P'):
     line =  big_picture + twin->curln;
     strncpy(buf, line->data, line->len);
     buf[line->len] = 0;
     if (twin->curln > twin->sline) {
        --(twin->curln);
        move(twin->curln, twin->curcol);
     }
     break;
  case Ctrl('N'):
     line =  big_picture + twin->curln;
     strncpy(buf, line->data, line->len);
     buf[line->len] = 0;
     if (twin->curln < twin->eline) {
        ++(twin->curln);
        move(twin->curln, twin->curcol);
     }
     break;
  }
  str_trim(buf);
  if (*buf)
     fprintf(flog, "%s%s: %s%s\n",
        (twin->eline == b_lines - 1) ? "[1;33m" : "",
        (twin->eline == b_lines - 1) ?
        getuserid(currutmp->destuid) : cuser.userid, buf,
        (ch == Ctrl('P')) ? "[37;45m(Up)[m" : "[m");
}

/*
char *talk_help =
  "== On-line Help! ==\n"
  " ^O 此線上說明畫面 \n"
  " ^U 列出線上使用者 \n"
  " ^P 切換Pager      \n"
  " ^G 嗶!(睡著了嗎?) \n"
  " ^C/^D 掰掰了...   \n";
*/

static
do_talk(fd)
   int fd;
{
   struct talk_win mywin, itswin;
   time_t talkstart;
   usint myword = 0,itword = 0;
   char mid_line[128], data[200];
   int i, ch, datac, exp;
   int im_leaving = 0;
 /*
   FILE *log;
  */
   struct tm *ptime;
   time_t now;
   char genbuf[200], fpath[100];

   time(&now);
   ptime = localtime(&now);

   sethomepath(fpath, cuser.userid);
   strcpy(fpath, tempnam(fpath, "talk_"));
   flog = fopen(fpath, "w");

#if 0
   setuserfile(genbuf, fn_talklog); /* Kaede */
/*   if (!is_watched(cuser.userid)) */
     log = NULL;
/*   else */
    if (log = fopen(genbuf, "a+"))
     fprintf(log, "[%d/%d %d:%02d] & %s\n",
             ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min,
             save_page_requestor);
#endif

   setutmpmode(TALK);

   ch = 58 - strlen(save_page_requestor);
   sprintf(genbuf, "%s【%s", cuser.userid, cuser.username);
   i = ch - strlen(genbuf);
   if (i >= 0)
   {
      i = (i >> 1) + 1;
   }
   else
   {
     genbuf[ch] = '\0';
     i = 1;
   }
   memset(data, ' ', i);
   data[i] = '\0';

   sprintf(mid_line, COLOR2" <聊天室>  [1m"COLOR1"%s%s】 [37m與  "COLOR1"%s%s[m",
     data, genbuf, save_page_requestor,  data);

   memset(&mywin, 0, sizeof(mywin));
   memset(&itswin, 0, sizeof(itswin));

   i = b_lines >> 1;
   mywin.eline = i - 1;
   itswin.curln = itswin.sline = i + 1;
   itswin.eline = b_lines - 1;

   clear();
   move(i, 0);
   outs(mid_line);
   move(0, 0);

   add_io(fd, 0);
   talkstart = time(0);

   while (1)
   {
     ch = igetkey();

     if (ch == I_OTHERDATA)
     {
       datac = recv(fd, data, sizeof(data), 0);
       if (datac <= 0)
         break;

       if(data[0] == Ctrl('A'))
       {
//         extern int BWboard();
//         if(BWboard(fd,1)==-2)
//         if(DL_func("SO/bwboard.so:vaBWboard",fd,1)==-2)
           break;
         continue;
       }

       itword++;
       for (i = 0; i < datac; i++)
         do_talk_char(&itswin, data[i]);
     }
     else
     {
       if(ch == Ctrl('A'))
       {
//         extern int BWboard();
//         data[0] = (char) ch;
//         if(send(fd, data, 1, 0) != 1)
           break;
//         if (BWboard(fd,0)==-2)
//         if(DL_func("SO/bwboard.so:vaBWboard",fd,0)==-2)
           break;
       }

       if (ch == Ctrl('C'))
       {
         if (im_leaving)
           break; 
         move(b_lines, 0); 
         clrtoeol(); 
         outs("再按一次 Ctrl-C 就正式中止談話囉！"); 
         im_leaving = 1; 
         continue;
       }
       if (im_leaving)
       {
         move(b_lines, 0);
         clrtoeol();
         im_leaving = 0;
       }
       switch(ch)
       {			  
        case KEY_LEFT:            
          ch = Ctrl('B');
	  break;
        case KEY_RIGHT:
          ch = Ctrl('F');
          break;
        case KEY_UP:
          ch = Ctrl('P');
          break;
        case KEY_DOWN:
          ch = Ctrl('N');
          break;
       }
       myword++;
       data[0] = (char) ch;  
       if (send(fd, data, 1, 0) != 1)
         break;

       do_talk_char(&mywin, *data);
     }
   }
   
   add_io(0, 0);
   close(fd);

   if (flog) {
      time(&now);
      fclose(flog);
        exp = rpguser.race != 4 ? myword - itword 
                                    : rpguser.level*(myword - itword);
	if(exp<0) exp=1;
	if(myword/(now - talkstart) > 2) exp = 2;
        inexp(exp);
        pressanykey("你打了 %d 字,對方 %d 字,得到 %d 點經驗值。",
                     myword,itword,exp);
        sprintf(genbuf,"%s與%s聊天, %d 秒, %d 字, %d exp %s",
          cuser.userid,save_page_requestor,now - talkstart,
          myword,exp,Ctime(&now));
          
        f_cat(BBSHOME"/log/talk.log", genbuf);
      more(fpath, NA);
      
      sprintf(genbuf, "對話記錄 [1;36m(%s)[m", 
              getuserid(currutmp->destuid));
              
      if (getans(" 清除(C) 移至備忘錄(M) (C/M)?[C] ") == 'm')
        mail2user(cuser, genbuf, fpath);
      unlink(fpath);
      flog = 0;
   }
   setutmpmode(XINFO);
}

static void
my_talk(uin)
  user_info *uin;
{
  int sock, msgsock, length, ch,pkmode = 0;	//星空雞對戰
  struct sockaddr_in sin;
  pid_t pid;
  char c;
  char genbuf[4];
  uschar mode0 = currutmp->mode;

  ch = uin->mode;
  strcpy(currutmp->chatid,uin->userid);
  strcpy(currauthor, uin->userid);

  if (ch == EDITING || ch == TALK || ch == CHATING
      || ch == PAGE || ch == MAILALL || ch == FIVE  
      || !ch && (uin->chatid[0] == 1 || uin->chatid[0] == 3))
    pressanykey("人家在忙啦");
    
  else if (!HAS_PERM(PERM_SYSOP) &&
           !(is_friend(uin) & 2) && uin->pager == 2)
    pressanykey("對方只接受好友的呼叫");
    
  else if (!HAS_PERM(PERM_SYSOP) &&
           ((uin->pager == 3) || (is_rejected(uin) & 2)))
    pressanykey("對方關掉呼叫器了");
    
  else if (!HAS_PERM(PERM_SYSOP) && uin->pager == 4)
    pressanykey("對方拔掉呼叫器了");
           
  else if (!(pid = uin->pid) || (kill(pid, 0) == -1))
  {
    resetutmpent();
    pressanykey(msg_usr_left);
  }
  else
  {
    if(currstat == CHICKEN && ch == CHICKEN)  // 星空雞對戰
    {
      pkmode = 1;
      getdata(2, 0, "確定要和他/她 PK 嗎(Y/N)?[N] ", genbuf, 2, LCECHO,0);
    }
    else
    {  
//      showplans(uin->userid);
      getdata(2, 0, "找他 [y]聊天 [f]下五子棋? [p]RPG PK [N]放棄 ",
            genbuf, 2, LCECHO,0); 
//        getdata(2, 0, "找他 (Y)聊天 (F)下五子棋? [N]放棄 ", genbuf, 2, LCECHO, 0);
    }

    if (*genbuf == 'y' || *genbuf == 'Y') 
    {
      uin->turn = 0;
      log_usies("TALK ", uin->userid);
    }
    else if (*genbuf == 'f' || *genbuf == 'p')
    {
      currutmp->turn = 0;
      uin->turn = 1;
    }
/*    
    else if (*genbuf == 'p')
    {
      currutmp->turn = 0;
      uin->turn = 1;
    }
 */
    else
      return;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
      return;

#if     defined(__OpenBSD__)                    /* lkchu */

    if (!(h = gethostbyname(MYHOSTNAME)))
      return -1;  
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = 0;
    memcpy(&sin.sin_addr, h->h_addr, h->h_length);                

#else

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = 0;
    memset(sin.sin_zero, 0, sizeof(sin.sin_zero));

#endif

    length = sizeof(sin);
    if (bind(sock, (struct sockaddr *) &sin, length) < 0 || getsockname(sock, (struct sockaddr *) &sin, &length) < 0)
    {
      close(sock);
      return;
    } 

    currutmp->sockactive = YEA;
    currutmp->sockaddr = sin.sin_port;
    currutmp->destuid = uin->uid;
    if(currstat == CHICKEN && ch == CHICKEN)  // 星空雞對戰
    {
      DL_func("SO/pip.so:pip_set_currutmp");
      setutmpmode(CHICKENPAGE);
    }
    else
      setutmpmode(PAGE);
    uin->destuip = currutmp;
    kill(pid, SIGUSR1);
    clear();
    prints("正呼叫 %s.....\n鍵入 Ctrl-D 中止....", uin->userid);

    listen(sock, 1);
    add_io(sock, 20);
    while (1) 
    {
      ch = igetch();
      if (ch == I_TIMEOUT) 
      {
        ch = uin->mode;
        if (!ch && uin->chatid[0] == 1 && uin->destuip == currutmp) 
        {
          bell();
          outmsg("對方回應中...");
          refresh();
        }
        else if (ch == EDITING || ch == TALK || ch == CHATING
             || ch == PAGE || ch == MAILALL || ch == FIVE
             || !ch && (uin->chatid[0] == 1 || uin->chatid[0] == 3)) 
        {
          add_io(0, 0);
          close(sock);
          currutmp->sockactive = currutmp->destuid = 0;
          pressanykey("人家在忙啦");
          return;
        }
        else 
        {
#ifdef LINUX
          add_io(sock, 20);       /* added 4 linux... achen */
#endif
          move(0, 0);
          outs("再");
          bell();
          uin->destuip = currutmp;

          if (kill(pid, SIGUSR1) == -1)
          {
#ifdef LINUX
            add_io(sock, 20);       /* added 4 linux... achen */
#endif
            pressanykey(msg_usr_left);
            refresh(); 
       	    return;
          }
          continue;
       	}
      }

      if (ch == I_OTHERDATA)
        break;

      if (ch == '\004')
      {
        add_io(0, 0);
        close(sock);
        currutmp->sockactive = currutmp->destuid = 0;
        return;
      }
    }

    msgsock = accept(sock, (struct sockaddr *) 0, (int *) 0);

    if (msgsock == -1)
    {
      perror("accept");
      return;
    }
    add_io(0, 0);
    close(sock);
    currutmp->sockactive = NA;
    /* currutmp->destuid = 0 ; */
    read(msgsock, &c, sizeof c);

    if (c == 'y')
    {
      sprintf(save_page_requestor, "%s (%s)", uin->userid, uin->username);
      if (uin->turn && *genbuf == 'f' )
        DL_func("SO/five.so:va_gomoku",msgsock);
      if (uin->turn && *genbuf == 'p' )
      {
        setutmpmode(RPK);
        pressanykey("單挑功\能維修中");
        return;
//        rpg_pk(msgsock,uin->userid);
      }
      else if(pkmode)
      {
        pressanykey("單挑功\能維修中");
//        DL_func("SO/pip.so:va_pip_vf_fight",msgsock,1);
        return;
      }
      else if(*genbuf == 'y' )
        do_talk(msgsock);
    }
    else
    {
      move(9, 9);
      outs("【回音】 ");
      switch (c)
      {
      case '1':
        outs("我現在很忙耶..");
        break;
      case '2':
        outs("對不起，我有事情耶....");
        break;
      case 'c':
        outs("請勿打擾ok..");
        break;
      case '3':
        outs("找我有事嗎？請先來信..");
        break;
      case 'e':
      {
        char msgbuf[60];
        read(msgsock, msgbuf, 60);
        outs("對不起，因為\n");
        move(10,18);
        outs(msgbuf);
      }
      break;
      default:
        outs("對方選擇 No.....:)");
      }
    }
    pressanykey("Press Any Key...");
    close(msgsock);
  }
  currutmp->mode = mode0;
  currutmp->destuid = 0;
}


/* ------------------------------------- */
/* 選單式聊天介面                        */
/* ------------------------------------- */


#define US_PICKUP       1234
#define US_RESORT       1233
#define US_ACTION       1232
#define US_REDRAW       1231

static int
search_pickup(num, actor, pklist)
  int num;
  int actor;
  pickup pklist[];
{
  char genbuf[IDLEN + 2];

  getdata(b_lines - 1, 0, "請輸入使用者姓名：", genbuf, IDLEN + 1, DOECHO,0);
  move(b_lines - 1, 0);
  clrtoeol();

  if (genbuf[0])
  {
    int n = (num + 1) % actor;
    str_lower(genbuf, genbuf);
    while (n != num)
    {
      if (strstr_lower(pklist[n].ui->userid, genbuf))
        return n;
      if (++n >= actor)
        n = 0;
    }
  }
  return -1;
}


static int
pickup_cmp(i, j)
  pickup *i, *j;
{
  switch (pickup_way)
  {
  case 0:
    {
      register int friend;

      if (friend = j->friend - i->friend)
        return friend;
    }
  case 1:
    return strcasecmp(i->ui->userid, j->ui->userid);
  case 2:
    return (i->ui->mode - j->ui->mode);
  case 3:
    return (i->idle - j->idle);
  case 4:
    return strcasecmp(i->ui->from, j->ui->from);
  case 5:
    return (j->ui->brc_id - i->ui->brc_id); 
  }
}

int 
pal_init(PAL *pal)
{
  time_t now = time(NULL);
  struct tm *ptime = localtime(&now);
  pal->ftype = 0;
  pal->savemode = 0;
  sprintf(pal->date, "%02d/%02d",  ptime->tm_mon + 1, ptime->tm_mday);
  sprintf(pal->desc, "?");
  sprintf(pal->owner, "?");
  sprintf(pal->userid, "?");  
  return 1;                        
}


int
pal_have(userid, whoask)
/* return value :
 * 0  : no such user
 * 1  : have such user
 */
  char *userid;
  char *whoask;
{
  char buf[STRLEN];
  int fd, have = -1;
  PAL pal;

  sethomefile(buf, userid, FN_PAL);
  if ((fd = open(buf, O_RDONLY)) >= 0)
  {
    while (read(fd, &pal, sizeof(pal)) == sizeof(pal))
    {
      if (!strcmp(pal.userid, whoask))
      {
         have=pal.ftype;
         break;
      }
    }
    close(fd);
  }
  return have;
}


void
friend_add(uid)
  char *uid;
{
  PAL pal, aloha;
  pal_init(&pal);
  pal_init(&aloha);
  
  if (uid[0] > ' ')
  {
    char fpath[80];
    char buf[22];
    strcpy(pal.userid, uid);
    
    sprintf(fpath, "對於 %s 的描述：", uid);
    getdata(2, 0, fpath, buf, 22, DOECHO, 0);
    if(strlen(buf)<2)
       sprintf(buf, "?");
       
    strncpy(pal.desc, buf, 21);
      
    getdata(2, 0, "壞人嗎 (Y/N) ? [N]", buf, 3, DOECHO, 0);
    if (*buf != 'y')
    {
      pal.ftype |= M_PAL;
      
      if (strcmp(uid, cuser.userid))
      {
        getdata(2, 0, "加入上下站通知嗎 (Y/N) ? [N]", buf, 3, DOECHO, 0);
        if (*buf == 'y')
        {
          pal.ftype |= M_ALOHA;
          {
            strcpy(aloha.userid, cuser.userid);
            sethomefile(fpath, uid, FN_ALOHA);
            rec_add(fpath, &aloha, sizeof(aloha));
          }
        }
      }
    }
    else
      pal.ftype |= M_BAD;

    if (currstat == LUSERS)
      sethomefile(currdirect, cuser.userid, FN_PAL);
    rec_add(currdirect, &pal, sizeof(pal));
  }     
}


int
cmpuname(userid, pal)
  char *userid;
  PAL *pal;
{
  return (!str_ncmp(userid, pal->userid, sizeof(pal->userid)));
}

static void
friend_delete(uid)
  char *uid;
{
  char fpath[64], ans[4];
  PAL pal, aloha;
  int pos;
  
  sethomefile(fpath, cuser.userid, FN_PAL);
  pos = rec_search(fpath, &pal, sizeof(pal), cmpuname, (int) uid);

  if (pos)
  {
    getdata(1, 0, msg_sure_ny, ans, 3, DOECHO, 0);
    if (*ans ==  'y')
      rec_del(fpath, sizeof(pal), pos, NULL, NULL);

      sethomefile(fpath, uid, FN_ALOHA);
      pos = rec_search(fpath, &aloha, sizeof(aloha), cmpuname, (int) cuser.userid);
      if (pos)
        rec_del(fpath, sizeof(PAL), pos , NULL, NULL);

  }
}

    
void
friend_load()
{
  ushort myfriends[MAX_FRIEND];
  ushort myrejects[MAX_REJECT];
  char genbuf[200];
  PAL pal;
  int fd;

  memset(myfriends, 0, sizeof(myfriends));
  memset(myrejects, 0, sizeof(myrejects));
  friendcount = rejectedcount = 0;

  setuserfile(genbuf, FN_PAL);
  if ((fd = open(genbuf, O_RDONLY)) > 0)
  {
    ushort unum;

    while (read(fd, &pal, sizeof(pal)))  
    {
      if (unum = searchuser(pal.userid))
      {
        if (pal.ftype & M_PAL)
          myfriends[friendcount++] = (ushort) unum;
        if (pal.ftype & M_BAD)
          myrejects[rejectedcount++] = (ushort) unum;
      }
    }
    close(fd);
  }
  memcpy(currutmp->friend, myfriends, sizeof(myfriends));
  memcpy(currutmp->reject, myrejects, sizeof(myrejects));
}

static char *       /* Kaede show friend description */
friend_descript(uident)
  char *uident;
{
  static char *space_buf="                    ";
  static char desc_buf[80];
  char fpath[80];
  int pos;
  PAL pal;

  setuserfile(fpath, FN_PAL);
  pos = rec_search(fpath, &pal, sizeof(pal), cmpuname, (int) uident);

  if (pos)
  {
    strcpy (desc_buf, pal.desc);
    return desc_buf;
  }
  else
    return space_buf;          
}

int
pklist_doent(pklist, num, row, bar)
  pickup pklist;
  int num, row, bar;
{
  register user_info *uentp;
  time_t diff;
  int color_index;
  char pagerchar[10] = " WF*- wfo_";	        
  char sightchar[4] = " )(#";
  char outmsg[256], buf[128];
  
  uentp = pklist.ui;
    if (!uentp->pid)
        return US_PICKUP;

  if(bar==1)        
    sprintf(outmsg,"●[1;33;44m");
  else
    sprintf(outmsg,"  [1;37;40m");
      
  color_index = (currutmp == uentp) ? 10 : pklist.friend;
  
  if (PERM_HIDE(uentp)) color_index = 9; 
  
  sprintf(buf,"%3d %c%c%s%-13s%-16.16s [37m",
         show_uid ? uentp->uid : (num+1),
         pagerchar[uentp->pager + ((pklist.friend & 2)>0) * 5],
         sightchar[uentp->invisible],
         fcolor[color_index],
         uentp->userid,
         uentp->username);

  strcat(outmsg,buf);
 
  if(show_friend)
    sprintf(buf,"-15.15s ", friend_descript(uentp->userid) );
  else if(show_board)
    sprintf(buf,"-15.15s ", (char *) getbname(uentp->brc_id) );
  else
    sprintf(buf,"%-15.15s ",
        ((  uentp->pager==0
          || ( uentp->pager<4 && !(pklist.friend & 8))
          || HAS_PERM(PERM_SYSOP) ) ? uentp->from_alias ?
        fcache->replace[uentp->from_alias] : uentp->from : "*" ) );
        
  strcat(outmsg,buf);
  
  sprintf(buf,"%-14.14s %s%-4.4s",
      show_tty ? uentp->tty : modestring(uentp, 0),
      uentp->birth ? fcolor[8] : fcolor[0],
      uentp->birth ? "獸星" : uentp->feeling[0] ? uentp->feeling : "不明");

  strcat(outmsg,buf);
  
  diff = pklist.idle;
  
  if (diff > 0)
     sprintf(buf, "%3d'%02d[m", diff / 60, diff % 60);
  else
     sprintf(buf, " 00'00[m");
        
  if (show_pid)
     sprintf(buf, "%6d[m", uentp->pid);

  strcat(outmsg, buf);

//  prints("%s\n",outmsg);
  outmsgline(outmsg, row);
//  move(b_lines, 0);
  
  return color_index;
}

static void
pickup_user()
{
  static int num = 0;
  char genbuf[256];
  register user_info *uentp;
  register pid_t pid0=0, id0;
  register int actor, head, foot, friend, badman;
  int state = US_PICKUP, ch, smode=currstat;
  time_t diff, freshtime;
  pickup pklist[USHM_SIZE];             

  resolve_fcache();
  while (1)
  {
    if (state == US_PICKUP)
      freshtime = 0;

    if (utmpshm->uptime > freshtime)
    {
      time(&freshtime);
      bfriends_number =  friends_number = override_number = 
      rejected_number = actor = ch = 0;

      while (ch < USHM_SIZE)
      {
        uentp = &(utmpshm->uinfo[ch++]);
        if (uentp->pid)
        {
           friend = is_friend(uentp);
           badman = is_rejected(uentp);
           
          if (
              (
               ((uentp->invisible & 1) && !(currutmp->invisible & 2))
               && !( HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_SEECLOAK))
              )
              ||
                 (
                  (badman & 2) 
                  && !HAS_PERM(PERM_SYSOP)
                 )
             )    
            continue;           /* Thor: can't see anyone who rejects you. */
          if (uentp->userid[0] == 0) continue;  /* Ptt's bug */

          if (!PERM_HIDE(currutmp) && PERM_HIDE(uentp))
             continue;

          if ( (cuser.uflag & FRIEND_FLAG)
                && (!friend || badman ) )
            continue;

          {
            if(!uentp->lastact) uentp->lastact = time(0);
            diff = freshtime - uentp->lastact;

            /* prevent fault /dev mount from kicking out users */

            if ( (diff > IDLE_TIMEOUT) && (diff < 60 * 60 * 24 * 5)
                  && !HAS_PERM(PERM_NOTIMEOUT) )
            {
              if ((kill(uentp->pid, SIGHUP) == -1) && (errno == ESRCH))
                memset(uentp, 0, sizeof(user_info));
              continue;
            }
          }
          pklist[actor].idle = diff;

          pklist[actor].friend = friend;
          if(badman & 1) pklist[actor].friend = 8;
          pklist[actor].ui = uentp;
          actor++;
        }
      }

      state = US_PICKUP;
      if (!actor)
      {
        getdata(b_lines - 1, 0, "你的朋友還沒上站，要看看一般網友嗎(Y/N)？[Y]", genbuf, 4, LCECHO,"Y");
        if (genbuf[0] != 'n')
        {
          cuser.uflag &= ~FRIEND_FLAG;
          continue;
        }
        return;
      }
    }

    if (state >= US_RESORT)
      qsort(pklist, actor, sizeof(pickup), pickup_cmp);

    if (state >= US_ACTION)
    {
      showtitle((cuser.uflag & FRIEND_FLAG)? "好友列表": "休閒聊天", BoardName);
      prints("  [%s] 上站人數：%-4d[1;32m我的朋友：%-3d"
        "[33m與我為友：%-3d[31m壞人：%-2d[37m                [m\n"
        COLOR1"[1m  %s P%c代號         %-17s%-17s%-13s%-10s[m\n",
        msg_pickup_way[pickup_way], actor,
        friends_number, override_number,rejected_number,
        show_uid ? "UID" :
        "No.",
        (HAS_PERM(PERM_SEECLOAK) || HAS_PERM(PERM_SYSOP)) ? 'C' : ' ',
        "暱稱", show_friend ? "好友描述" : show_board ? "使用看板" : "故鄉",
        show_tty ? "TTY " :
        "動態",
        show_pid ? "       PID" :
        " 心情  發呆"
        );
    }
    else
    {
      move(3, 0);
      clrtobot();
    }

    if(pid0)
       for (ch = 0; ch < actor; ch++)
        {
          if(pid0 == (pklist[ch].ui)->pid &&
           id0  == 256 * pklist[ch].ui->userid[0] + pklist[ch].ui->userid[1])
            {
               num = ch;
            }
        }

    if (num < 0)
      num = 0;
    else if (num >= actor)
      num = actor - 1;

    head = (num / p_lines) * p_lines;
    foot = head + p_lines;
    if (foot > actor)
      foot = actor;

    for (ch = head; ch < foot; ch++)
    {
      uentp = pklist[ch].ui;
      if (!uentp->pid)
      {
        state = US_PICKUP;
        break;
      }
      state = pklist_doent(pklist[ch], ch, ch + 3 - head, 0);
/*
      diff = pklist[ch].idle;
      if (diff > 0)
        sprintf(buf, "%3d'%02d", diff / 60, diff % 60);
      else
        buf[0] = '\0';
      if (show_pid)
        sprintf(buf, "%6d", uentp->pid);
      color_index = (currutmp == uentp) ? 10 : pklist[ch].friend;
      if (PERM_HIDE(uentp)) color_index = 9; 
      state=color_index;
      prints("%5d %c%c%s%-13s%-16.16s [m%-15.15s %-14.14s %s%-4.4s%s[m\n",
      show_uid ? uentp->uid :
      (ch + 1),
      pagerchar[uentp->pager + ((pklist[ch].friend & 2)>0) * 5],
      sightchar[uentp->invisible],
      fcolor[color_index],
      uentp->userid,
      uentp->username, 
      show_friend ? friend_descript(uentp->userid) :
      show_board ? (char *)getbname(uentp->brc_id) :
      ((uentp->pager==0 || (uentp->pager<4 && !(pklist[ch].friend & 8))
       || HAS_PERM(PERM_SYSOP)) ?
      uentp->from_alias ? fcache->replace[uentp->from_alias] : uentp->from
      : "*" ),
      show_tty ? uentp->tty :
      modestring(uentp, 0),
      uentp->birth ? fcolor[8] : fcolor[0],
      uentp->birth ? "獸星" : uentp->feeling[0] ? uentp->feeling : "不明" ,
      buf);
*/
    }

    if (state == US_PICKUP)
      continue;

    move(b_lines, 0);
    prints(COLOR1"[1;33m (TAB/f)[37m排序/好友 [33m(t)[37m聊天 "
      "[33m(a/d/o)[37m交友 [33m(q)[37m查詢 [33m(w)[37m扣應 "
      "[33m(m)[37m寄信   [37m[44m %13s [m",cuser.userid);
    state = 0;
    while (!state)
    {
      if(HAS_HABIT(HABIT_LIGHTBAR))
      {
        pklist_doent(pklist[num], num, num + 3 - head, 1);
        move(b_lines,0);
        ch = egetch();
        pklist_doent(pklist[num], num, num + 3 - head, 0);
        move(b_lines,0);
      }
      else
        ch = cursor_key(num + 3 - head, 0);
      
      if (ch == KEY_RIGHT || ch == '\n' || ch == '\r')
        ch = 't';

      switch (ch)
      {
      case KEY_LEFT:
      case 'e':
      case 'E':
        return;

        case KEY_TAB:
        {
          char ans[2];
          getdata(b_lines-1, 0,"排序方式 1.好友 2.代號 3.動態 4.發呆 5.故鄉 6.看板"
            ,ans, 2, LCECHO,"1");
            if(!ans[0])
            {
              state = US_REDRAW;
              break;
            }
          
          pickup_way = atoi(ans) - 1;
          if(pickup_way > 5 || pickup_way < 0) pickup_way = 0;
//          pickup_way = (pickup_way +1) %PICKUP_WAYS;
          state = US_PICKUP;
          num = 0;
          break;
        }

      case KEY_DOWN:
      case 'n':
        if (++num < actor)
        {
          if (num >= foot)
            state = US_REDRAW;
          break;
        }

      case '0':
      case KEY_HOME:
        num = 0;
        if (head)
          state = US_REDRAW;
        break;

      case 'N':
         if (HAS_PERM(PERM_BASIC)) {
            char buf[100];
            sprintf(buf, "暱稱 [%s]：", currutmp->username);
            if (!getdata(1, 0, buf, currutmp->username, 17, DOECHO,0))
               strcpy(currutmp->username, cuser.username);

            state = US_PICKUP;
         }
         break;

      case 'M':
         if (HAS_PERM(PERM_BASIC)) {
            char buf[64];
            sprintf(buf, "心情 [%s]：", currutmp->feeling);
            if (!getdata(1, 0, buf, currutmp->feeling, 5, DOECHO, currutmp->feeling))
              strcpy(currutmp->feeling, cuser.feeling);
            state = US_PICKUP;
         }
         break;

      case 'H':
         if (HAS_PERM(PERM_SYSOP)) {
            currutmp->userlevel ^= PERM_DENYPOST;
            state = US_PICKUP;
         }
         break;

      case 'D':
         if (HAS_PERM(PERM_SYSOP)) {
            char buf[100];

            sprintf(buf, "代號 [%s]：", currutmp->userid);
            if (!getdata(1, 0, buf, currutmp->userid, IDLEN + 1, DOECHO,0))
               strcpy(currutmp->userid, cuser.userid);

            state = US_PICKUP;
         }
         break;
      case 'F':
        {
            char buf[100];
            if(HAS_PERM(PERM_FROM))
            {
//              if(check_money(50,GOLD)) break;
//              degold(50);
//              pressanykey("修改故鄉花去金幣 50元");
//            }
              sprintf(buf, "故鄉 [%s]：", currutmp->from);
              if (getdata(1, 0, buf, currutmp->from, 17, DOECHO,currutmp->from))
              currutmp->from_alias=0;
              state = US_PICKUP;
            }
          }
         break;
      case ' ':
      case KEY_PGDN:
      case Ctrl('F'):
        if (foot < actor)
        {
          num += p_lines;
          state = US_REDRAW;
          break;
        }
        if (head)
          num = 0;
        state = US_PICKUP;
        break;

      case KEY_UP:
        if (--num < head)
        {
          if (num < 0)
          {
            num = actor - 1;
            if (actor == foot)
              break;
          }
          state = US_REDRAW;
        }
        break;

      case KEY_PGUP:
      case Ctrl('B'):
      case 'P':
        if (head)
        {
          num -= p_lines;
          state = US_REDRAW;
          break;
        }

      case KEY_END:
      case '$':
        num = actor - 1;
        if (foot < actor)
          state = US_REDRAW;
        break;

      case '/':
        {
          int tmp;
          if ((tmp = search_pickup(num, actor, pklist)) >= 0)
            num = tmp;
          state = US_REDRAW;
        }
        break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        {                       /* Thor: 可以打數字跳到該人 */
          int tmp;
          if ((tmp = search_num(ch, actor - 1)) >= 0)
            num = tmp;
          state = US_REDRAW;
        }
        break;

      case 'U':
        if (HAS_PERM(PERM_SYSOP))
          show_uid ^= 1;
        state = US_PICKUP;
        break;
      case 'Y':
        if (HAS_PERM(PERM_SYSOP))
          show_tty ^= 1;
        state = US_PICKUP;
        break;
      case 'I':
        if (HAS_PERM(PERM_SYSOP))
          show_pid ^= 1;
        state = US_PICKUP;
        break;

      case 'b':         /* broadcast */
        if(HAS_PERM(PERM_SYSOP) || cuser.uflag & FRIEND_FLAG)
        {
          int actor_pos = actor;
          char ans[4];
          state = US_PICKUP;
          if (!getdata(0, 0, "廣播訊息:", genbuf, 60, DOECHO,0))
            break;
          if (getdata(0, 0, "確定廣播? [Y]", ans, 4, LCECHO,0) && *ans == 'n')
            break;
          while (actor_pos)
          {
            uentp = pklist[--actor_pos].ui;
//            if (uentp->pid &&
//                currpid != uentp->pid &&
//                kill(uentp->pid, 0) != -1 &&
//                (HAS_PERM(PERM_SYSOP) || (uentp->pager != 3 &&
//                 (uentp->pager != 4 || is_friend(uentp) & 4))))
                my_write(uentp->pid, genbuf,MSG_BROADCAST);
           }
         }
         break;

      case 'S':         /* 顯示好友描述 */
        show_friend ^= 1;
        state = US_PICKUP;
        break;

      case 'B':         /* 顯示正在看的板 */
//        if (!HAS_PERM(PERM_SYSOP))
//          continue;
        show_board ^= 1;
        state = US_PICKUP;
        break;

      case 'u':         /* 線上修改資料 */
        if (!HAS_PERM(PERM_ACCOUNTS) || !HAS_PERM(PERM_SYSOP))
          continue;
      case 'W':
      case 'K':         /* 把壞蛋踢出去 */
      case 'g':         /* 修改RPG資料 */
        if (!HAS_PERM(PERM_SYSOP))
          continue;
        state = US_ACTION;
        break;

      case 't':
      case 's':
      case 'w':
        if (!HAS_PERM(PERM_PAGE))
          continue;
        state = US_ACTION;
        break;

      case 'C':
      case 'i':
        if (!HAS_PERM(PERM_CLOAK))  /* wildcat:隱形 */
          break;
        state = US_ACTION;
        break;

      case 'a':
      case 'd':
      case 'o':
      case 'f':
        if (!HAS_PERM(PERM_LOGINOK))  /* 註冊才有 Friend */
          break;
        if (ch == 'f')
        {
          cuser.uflag ^= FRIEND_FLAG;
          state = US_PICKUP;
          break;
        }
        state = US_ACTION;
        break;

      case 'q':
      case 'c':
      case 'm':
      case 'r':
      case 'l':
        if (!cuser.userlevel && ch != 'q' && ch != 'l') /* guest 只能 query */
          break;
      case 'h':
        state = US_ACTION;
        break;
      case 'p':
         if (HAS_PERM(PERM_BASIC)) {
            t_pager();
            state = US_PICKUP;
         }
         break;
      case KEY_ESC:
         if (KEY_ESC_arg == 'c')
            capture_screen();
         else if (KEY_ESC_arg == 'n') {
            edit_note();
            state = US_PICKUP;
         }
         break;
      default:          /* refresh user state */
        state = US_PICKUP;
      }
    }

    if (state != US_ACTION)
        {
         pid0 = 0;
         continue;
        }
    uentp = pklist[num].ui;
    pid0 = uentp->pid;
    id0  = 256 * uentp->userid[0] + uentp->userid[1];

    if (ch == 'w' || ch == 's')
    {
      if (
           (uentp->pid != currpid)    // 禁止自己傳給自己
            && ( HAS_PERM(PERM_SYSOP)
                 || (uentp->pager==0) 
                 || (uentp->pager==2)
                 || ((uentp->pager==3) && (is_friend(uentp) & 2))
               )
         )
      {
        cursor_show(num + 3 - head, 0);
        sprintf(genbuf,"★%s攻擊：", currutmp->cursor);
        my_write(uentp->pid, genbuf,MSG_NORMAL);
      }
      else
        state = 0;
    }
    else if (ch == 'l')
    {                           /* Thor: 看 Last call in */
      t_display();
      state = US_PICKUP;
    }
    else
    {
      switch (ch)
      {
      case 'W':
        if(HAS_PERM(PERM_SYSOP))
        {
          strcpy(uentp->cursor, "水球");
        }
        break;

      case 'r':
        m_read();
        break;

      case 'a':
        if (pal_have(cuser.userid, uentp->userid)==-1)
        {
          friend_add(uentp->userid, FRIEND_OVERRIDE);
          friend_load();
          state = US_PICKUP;
        }
        break;

      case 'd':
        if(pal_have(cuser.userid, uentp->userid)!=-1)
        {
          friend_delete(uentp->userid);
          friend_load();
          state = US_PICKUP;
        }
        break;
      case 'o':
        {
          char buf[80];
          
          setuserfile(buf, FN_PAL);
          ListEdit(buf);
          state = US_PICKUP;
        }
        break;

      case 'K':

        if (uentp->pid && (kill(uentp->pid, 0) != -1))
        {
          move(1, 0);
          clrtobot();
          move(2, 0);
          my_kick(uentp);
          state = US_PICKUP;
        }
        break;

      case 'm':
        if(!cuser.userlevel) break;
        stand_title("寄  信");
        prints("收信人：%s", uentp->userid);
        my_send(uentp->userid);

      case 'q':
        //strcpy(currauthor, uentp->userid);
        my_query(uentp->userid);
        break;

      case 'c':
        DL_func("SO/pip.so:pip_data_list_va", uentp->userid);
        /* shakalaca.990704: 小雞修正 */
        break;

      case 'i':
        u_cloak();
        break;

      case 'u':         /* Thor: 可線上查看及修改使用者 */
        {
          int id;
          userec muser;
           strcpy(currauthor, uentp->userid);
          stand_title("使用者設定");
          move(1, 0);
          if (id = getuser(uentp->userid))
          {
            memcpy(&muser, &xuser, sizeof(muser));
            user_display(&muser, 1);
            uinfo_query(&muser, 1, id);
          }
         }
        break;

//      case 'g':         /* wildcat:rpg query & edit */
//        rpg_udisplay(uentp->userid);
//        rpg_uquery(uentp->userid);
//        break;

      case 't':
        if (uentp->pid != currpid &&
            strcmp(uentp->from, currutmp->from) )
        {
          move(1, 0);
          clrtobot();
          move(3, 0);
          my_talk(uentp);
          state = US_PICKUP;
        }
      }
    }
    setutmpmode(smode);
  }
}


static int
listcuent(uentp)
  user_info *uentp;
{
  if ((uentp->uid != usernum) 
       && ( !(uentp->invisible & 1)
            || (currutmp->invisible & 2)
            || HAS_PERM(PERM_SYSOP) 
            || HAS_PERM(PERM_SEECLOAK)))
    AddNameList(uentp->userid);
  return 0;
}


static void
creat_list()
{
  CreateNameList();
  apply_ulist(listcuent);
}


int
t_users()
{
  int destuid0 = currutmp->destuid;
  if (chkmailbox())
     return;
  setutmpmode(LUSERS);
  pickup_user();
  currutmp->destuid = destuid0;
  return 0;
}


int
t_pager()
{
  //char *msgs[] = {"關", "開", "拔", "防","友"};
  char *msgs[] = {"開", "防", "友", "關", "拔"};
  currutmp->pager = (currutmp->pager + 1) % 5;
  pressanykey("您的 呼叫器狀態 將切換成：%s",msgs[currutmp->pager]);
  return 0;
}


int
t_idle()
{
  int destuid0 = currutmp->destuid;
  int mode0 = currutmp->mode;
  int stat0 = currstat;
  char genbuf[20];

  setutmpmode(IDLE);
  getdata(b_lines - 1, 0, "理由：[0]發呆 (1)接電話 (2)覓食 (3)打瞌睡 (4)裝死 (5)羅丹 (6)其他 (Q)沒事？", genbuf, 3, DOECHO,0);
  if (genbuf[0] == 'q' || genbuf[0] == 'Q') {
     currutmp->mode = mode0;
     currstat = stat0;
     return 0;
   }
  else if (genbuf[0] >= '1' && genbuf[0] <= '6')
    currutmp->destuid = genbuf[0] - '0';
  else
    currutmp->destuid = 0;

  if (currutmp->destuid == 6)
    if (!cuser.userlevel || !getdata(b_lines - 1, 0, "發呆的理由：", currutmp->chatid, 11, DOECHO,0))
      currutmp->destuid = 0;
 {
   char buf[80],passbuf[PASSLEN];
   do
   {
     move(b_lines - 2, 0);
     clrtoeol();
     sprintf(buf, "(鎖定螢幕)發呆原因: %s", (currutmp->destuid != 6) ?
         IdleTypeTable[currutmp->destuid] : currutmp->chatid);
     outs(buf);
     refresh();
     getdata(b_lines - 1, 0, MSG_PASSWD, passbuf, PASSLEN, PASS,0);
    passbuf[8]='\0';
  }while(!chkpasswd(cuser.passwd, passbuf) && strcmp(STR_GUEST,cuser.userid));
}
  currutmp->mode = mode0;
  currutmp->destuid = destuid0;
  currstat = stat0;

  return 0;
}


int
t_query()
{
  char uident[STRLEN];

  stand_title("查詢網友");
  usercomplete(msg_uid, uident);
  if (uident[0])
    my_query(uident);
  return 0;
}

int
t_talk()
{
  char uident[16];
  int tuid, unum, ucount;
  user_info *uentp;
  char genbuf[4];

  if (count_ulist() <= 1)
  {
    outs("目前線上只有您一人，快邀請朋友來光臨【" BOARDNAME "】吧！");
    return XEASY;
  }
  stand_title("打開話匣子");
  creat_list();
  namecomplete(msg_uid, uident);
  if (uident[0] == '\0')
    return 0;

  move(3, 0);
  if (!(tuid = searchuser(uident)) || tuid == usernum)
  {
    pressanykey(err_uid);
    return 0;
  }

  /* ----------------- */
  /* multi-login check */
  /* ----------------- */

  unum = 1;
  while ((ucount = count_logins(cmpuids, tuid, 0)) > 1)
  {
    outs("(0) 不想 talk 了...\n");
    count_logins(cmpuids, tuid, 1);
    getdata(1, 33, "請選擇一個聊天對象 [0]：", genbuf, 4, DOECHO,0);
    unum = atoi(genbuf);
    if (unum == 0)
      return 0;
    move(3, 0);
    clrtobot();
    if (unum > 0 && unum <= ucount)
      break;
  }

  if (uentp = (user_info *) search_ulistn(cmpuids, tuid, unum))
    my_talk(uentp);

  return 0;
}


/* ------------------------------------- */
/* 有人來串門子了，回應呼叫器            */
/* ------------------------------------- */

user_info *uip;

void
talkreply()
{
  int a;
  struct hostent *h;
  char hostname[STRLEN],buf[80];
  struct sockaddr_in sin;
  char genbuf[200];
  int pkmode = 0;  // 星空雞對戰

  talkrequest = NA;
  uip = currutmp->destuip;
  sprintf(page_requestor, "%s (%s)", uip->userid, uip->username);
  currutmp->destuid = uip->uid;
  currstat = XMODE;             /* 避免出現動畫 */

  if(uip->mode == CHICKENPAGE || uip->mode == RPK)
    pkmode = 1; //chicken pk

  clear();
  outs("\n
       (Y) 答應                 (1) 我現在很忙耶
       (N) 拒絕                 (2) 對不起，我有事情
       (C) 勿打擾               (3) 有事嗎？請先來信
       (E) [1;33m我自己輸入理由好了...[m\n\n");

  getuser(uip->userid);
  currutmp->msgs[0].last_pid = uip->pid;
  strcpy(currutmp->msgs[0].last_userid, uip->userid);
  strcpy(currutmp->msgs[0].last_call_in, "呼叫、呼叫，聽到請回答");
  prints("對方來自 [%s]，共上站 %d 次，文章 %d 篇\n",
    uip->from, xuser.numlogins, xuser.numposts);
//  showplans(uip->userid);
//  show_last_call_in();
  sprintf(genbuf, "你想跟 %s %s嗎？請選擇(Y/N/C/1/2/3/E) [N] ",
  page_requestor,pkmode?"PK":currutmp->turn?"下棋":"聊天");
  getdata(0, 0, genbuf, buf, 4, LCECHO,0);

  if (uip->mode != PAGE && uip->mode != CHICKENPAGE && uip->mode != RPK) {
     sprintf(genbuf, "%s已停止呼叫，按Enter繼續...", page_requestor);
     getdata(0, 0, genbuf, buf, 4, LCECHO,0);
     return;
  }

  currutmp->msgcount = 0;
  strcpy(save_page_requestor, page_requestor);
  memset(page_requestor, 0, sizeof(page_requestor));
  gethostname(hostname, STRLEN);
  if (!(h = gethostbyname(hostname)))
  {
    perror("gethostbyname");
    return;
  }
  memset(&sin, 0, sizeof sin);
  sin.sin_family = h->h_addrtype;
  memcpy(&sin.sin_addr, h->h_addr, h->h_length);
  sin.sin_port = uip->sockaddr;
  a = socket(sin.sin_family, SOCK_STREAM, 0);
  if ((connect(a, (struct sockaddr *) & sin, sizeof sin)))
  {
    perror("connect err");
    return;
  }
  if (!buf[0] || !strchr("ync123e", buf[0]))
  {
    if(pkmode)
      uip->destuip = currutmp;
//    buf[0] = 'y';
  }
  write(a, buf, 1);
  if (buf[0] == 'e' || buf[0] == 'E')
   {
     if (!getdata(b_lines, 0, "不能 talk 的原因：", genbuf, 60, DOECHO,0))
       strcpy(genbuf, "不告訴你咧 !! ^o^");
     write(a, genbuf, 60);
   }

  if (buf[0] == 'y' )
  {
    strcpy(currutmp->chatid,uip->userid);
    if(uip->mode == RPK)
    {
      pressanykey("單挑功\能維修中");
      close(a);
//      rpg_pk(a,uip->userid);
    }
    else if (currutmp->turn) 
      DL_func("SO/five.so:va_gomoku",a);
    else if(pkmode)
    {
      pressanykey("單挑功\能維修中");
//      DL_func("SO/pip.so:va_pip_vf_fight",a,2);
//      close(a);
    }
    else do_talk(a);
  }
  else
    close(a);
  clear();
}


/* ------------------------------------- */
/* 網友動態簡表                          */
/* ------------------------------------- */

int
shortulist(uentp)
  user_info *uentp;
{
  static int lineno, fullactive, linecnt;
  static int moreactive, page, num;
  char uentry[50];
  int state;

  if (!lineno)
  {
    lineno = 3;
    page = moreactive ? (page + p_lines * 3) : 0;
    linecnt = num = moreactive = 0;
    move(1, 70);
    prints("Page: %d", page / (p_lines) / 3 + 1);
    move(lineno, 0);
  }
  if (uentp == NULL)
  {
    int finaltally;

    clrtoeol();
    move(++lineno, 0);
    clrtobot();
    finaltally = fullactive;
    lineno = fullactive = 0;
    return finaltally;
  }
  if (( !HAS_PERM(PERM_SYSOP) && !HAS_PERM(PERM_SEECLOAK) 
       && (uentp->invisible & 1 && !(currutmp->invisible & 2))
      )
      ||
      ((is_rejected(uentp) & HRM) && !HAS_PERM(PERM_SYSOP)))
  {
    if (lineno >= b_lines)
      return 0;
    if (num++ < page)
      return 0;
    memset(uentry, ' ', 25);
    uentry[25] = '\0';
  }
  else
  {
    fullactive++;
    if (lineno >= b_lines)
    {
      moreactive = 1;
      return 0;
    }
    if (num++ < page)
      return 0;

    state = (currutmp == uentp) ? 10 : is_friend(uentp);

    if (PERM_HIDE(uentp))
       state = 9;  

    sprintf(uentry, "%s%-13s%c%-10s%s ", fcolor[state],
      uentp->userid, (uentp->invisible & 1) ? '#' : ' ',
      modestring(uentp, 1), state ? "[m" : "");
  }
  if (++linecnt < 3)
  {
    strcat(uentry, "│");
    outs(uentry);
  }
  else
  {
    outs(uentry);
    linecnt = 0;
    clrtoeol();
    move(++lineno, 0);
  }
  return 0;
}


static void
do_list(modestr)
  char *modestr;
{
  int count;

  showtitle(modestr, BoardName);

  outc('\n');
  outs(msg_shortulist);

  friends_number = override_number = 0;
  if (apply_ulist(shortulist) == -1)
    outs(msg_nobody);
  else
  {
    time_t thetime = time(NULL);

    count = shortulist(NULL);
    move(b_lines, 0);
    prints(COLOR1"[1m  上站總人數：%-7d[32m我的朋友：%-6d"
      "[33m與我為友：%-8d[30m%-23s[37;40;0m",
      count, friends_number, override_number, Cdate(&thetime));
    refresh();
  }
}


int
t_list()
{
  setutmpmode(LUSERS);
  do_list("使用者狀態");
  igetch();
  return 0;
}



void
t_aloha()
{
   int i;
   user_info *uentp;
   pid_t pid;
   char buf[100];

   sprintf(buf + 1, "[1;37;41m☆ %s(%s) 上站了! [0m",
      cuser.userid, cuser.username);
   *buf = 0;

    /* Thor: 特別注意, 自己上站不會通知自己... */

   for (i = 0; i < USHM_SIZE; i++) {
      uentp = &utmpshm->uinfo[i];
      if ((pid = uentp->pid) && (kill(pid, 0) != -1) &&
          uentp->pager && (is_friend(uentp) & 2) &&
          strcmp(uentp->userid, cuser.userid))
         my_write(uentp->pid, buf, MSG_ALOHA);
   }
}

int
lockutmpmode(int unmode)
{
  if (count_multiplay(unmode))
  {
   char buf[80];
   sprintf(buf,"抱歉! 您已有其他線相同的ID正在%s",ModeTypeTable[unmode]);
   pressanykey(buf);
   return 1;
  }
  setutmpmode(unmode);
  currutmp->lockmode = unmode;
  return 0;
}

int
unlockutmpmode()
{
  currutmp->lockmode = 0;
}

int
t_pk()
{
  char uident[16];
  int tuid, unum, ucount;
  char genbuf[4];

  if (count_ulist() <= 1)
  {
    outs("目前線上只有您一人，快邀請朋友來光臨【" BOARDNAME "】吧！");
    return XEASY;
  }
  stand_title("找人單挑");
  creat_list();
  namecomplete(msg_uid, uident);
  if (uident[0] == '\0')
    return 0;

  move(3, 0);
  if (!(tuid = searchuser(uident)) || tuid == usernum || !strcmp(uident,"guest"))
  {
    pressanykey(err_uid);
    return 0;
  }

  /* ----------------- */
  /* multi-login check */
  /* ----------------- */

  unum = 1;
  while ((ucount = count_logins(cmpuids, tuid, 0)) > 1)
  {
    outs("(0) 不想 pk 了...\n");
    count_logins(cmpuids, tuid, 1);
    getdata(1, 33, "請選擇一個 PK 對象 [0]：", genbuf, 4, DOECHO,0);
    unum = atoi(genbuf);
    if (unum == 0)
      return 0;
    move(3, 0);
    clrtobot();
    if (unum > 0 && unum <= ucount)
      break;
  }

//  if (uentp = (user_info *) search_ulistn(cmpuids, tuid, unum))
//    rpg_pk(0,uident);

  return 0;
}
