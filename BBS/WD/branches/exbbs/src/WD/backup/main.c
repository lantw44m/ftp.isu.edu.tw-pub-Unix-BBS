/*-------------------------------------------------------*/
/* main.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* author : opus.bbs@bbs.cs.nthu.edu.tw                  */
/* target : BBS main/login/top-menu routines             */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#define _MAIN_C_

#include "bbs.h"
#define MAXMONEY ((cuser.totaltime*4)+(cuser.numlogins*10)+(cuser.numposts*1000))+10000

jmp_buf byebye;

int talkrequest = NA;

static uschar enter_uflag;

char fromhost[32]="\0";
char tty_name[16]="\0";
char remoteusername[32]="\0";
char fromport[8]="\0";

void check_register();
int mbbsd=0;

/* ----------------------------------- */
/* check system / memory / CPU loading */
/* ----------------------------------- */
/* Modify by Robert NeKo */

int
chkload(limit)
  int limit;
{
    double cpu_load[3];
//    register int i;
    FILE *fp;
        
    fp = fopen("/proc/loadavg", "r");
    if (!fp)
      cpu_load[0] = cpu_load[1] = cpu_load[2] = 0;
    else
    {
      float av[3];
      fscanf(fp, "%g %g %g", av, av + 1, av + 2);
      fclose(fp);
      cpu_load[0] = av[0];
      cpu_load[1] = av[1];
      cpu_load[2] = av[2];
    }
    prints("〈系統負載 = %.2f %.2f %.2f (上限 = %2d)〉",cpu_load[0], cpu_load[1], cpu_load[2], limit);
}                                                    


/* ----------------------------------------------------- */
/* 離開 BBS 程式                                         */
/* ----------------------------------------------------- */

void
log_usies(mode, msg)
  char *mode, *msg;
{
  char buf[512], data[256];
  time_t now;
  
  time(&now);
  if (!msg)
  {
    sprintf(data, "Stay: %d (%s)", 
      (now - login_start_time) / 60, cuser.username);
    msg = data;
  }
  sprintf(buf, "%s %s %-13s%s", Cdate(&now), mode, cuser.userid, msg);
  f_cat(FN_USIES, buf);
}
                          

static void
setflags(mask, value)
  int mask, value;
{
  if (value)
    cuser.uflag |= mask;
  else
    cuser.uflag &= ~mask;
}


void
u_exit(mode)
  char *mode;
{
  extern void auto_backup();    /* 編輯器自動備份 */
  userec xuser;
  int diff = (time(0) - login_start_time) / 60;

  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  auto_backup();
  setflags(PAGER_FLAG, currutmp->pager != 1);
  setflags(CLOAK_FLAG, currutmp->invisible);
  xuser.pager = currutmp->pager;	 /* 記錄pager狀態, add by wisely */
  if (HAS_PERM(PERM_SYSOP) || belong("etc/okid",cuser.userid))
    xuser.invisible = currutmp->invisible; /* 紀錄隱形狀態 by wildcat */
  else
    xuser.invisible = 0;
  xuser.totaltime += ((time(0) - update_time) / count_multi());
  xuser.numposts = cuser.numposts;
  xuser.feeling[4] = '\0';
  xuser.pagernum[6] = '\0';
  
 if(!(HAS_PERM(PERM_DENYPOST)) && (HAS_HABIT(HABIT_SAYALOHA)) 
    && !(currutmp->invisible) )
    do_aloha(MSG_ALOHA_LOGOUT);
     
  purge_utmp(currutmp);
  if(!diff && cuser.numlogins && strcmp(cuser.userid,STR_GUEST))
    xuser.numlogins = --cuser.numlogins; /* Leeym 上站停留時間限制式 */
  substitute_record(fn_passwd, &xuser, sizeof(userec), usernum);
  log_usies(mode, NULL);
}


void
system_abort()
{
  if (currmode)
    u_exit("ABORT");
/*
  clear();
  refresh();
*/
  printf("謝謝光臨, 記得常來喔 !\n");
  sleep(1);
  exit(0);
}


void
abort_bbs()
{
  if (currmode)
    u_exit("AXXED");
  exit(0);
}


void
leave_bbs()
{
   reset_tty();
}


/* ----------------------------------------------------- */
/* 登錄 BBS 程式                                         */
/* ----------------------------------------------------- */


int
dosearchuser(userid)
  char *userid;
{
  if (usernum = getuser(userid))
  {
    memcpy(&cuser, &xuser, sizeof(cuser));
    memcpy(&rpguser, &rpgtmp, sizeof(rpguser));
  }
  else
  {
    memset(&cuser, 0, sizeof(cuser));
    memset(&rpguser, 0, sizeof(rpguser));
  }
  return usernum;
}


static void
talk_request()
{
#ifdef  LINUX
  /*
   * Linux 下連續 page 對方兩次就可以把對方踢出去： 這是由於某些系統一 nal
   * 進來就會將 signal handler 設定為內定的 handler, 不幸的是 default 是將程
   * erminate. 解決方法是每次 signal 進來就重設 signal handler
   */

  signal(SIGUSR1, talk_request);
#endif
  bell();
  if (currutmp->msgcount) {
     char buf[200];
     time_t now = time(0);

     sprintf(buf, "[33;41m★%s[34;47m [%s] %s [0m",
        (currutmp->destuip)->userid,  my_ctime(&now),
        (currutmp->sig == 2) ? "重要消息廣播！(請Ctrl-U,l查看熱訊記錄)" : "呼叫、呼叫，聽到請回答");
     move(0, 0);
     clrtoeol();
     outs(buf);
     refresh();
  }
  else {
     uschar mode0 = currutmp->mode;
     char c0 = currutmp->chatid[0];
     screenline* screen = (screenline *)calloc(t_lines, sizeof(screenline));
     currutmp->mode = 0;
     currutmp->chatid[0] = 1;
     vs_save(screen);
     talkreply();
     vs_restore(screen);
     currutmp->mode = mode0;
     currutmp->chatid[0] = c0;
   }
}


show_last_call_in()
{
   char buf[200];  
//   int ch;
//   time_t now;
//   time(&now);

   sprintf(buf, "%-13s: %-s",
      currutmp->msgs[0].last_userid, //my_ctime(now),
      currutmp->msgs[0].last_call_in);
      
   pressanykey(buf);
   refresh();
}

msgque oldmsg[MAX_REVIEW];   /* 丟過去的水球 */
char   no_oldmsg=0,oldmsg_count=0;            /* pointer */

static void
write_request()
{
  time_t now;
  struct tm *tp;
  char ch, buf[256], msg[256];
  int currstat0, mode0;
//  extern char watermode;
  screenline* screen = (screenline *)calloc(t_lines, sizeof(screenline));

  time(&now);
  tp = localtime(&now);
    
/*  Half-hour remind  */

  if(*currmsg)
  {
    outmsg(currmsg);
    refresh();
    bell();
    *currmsg = 0;
    return;
  }

#ifdef  LINUX
  signal(SIGUSR2, write_request);
#endif

  vs_save(screen);
  currstat0 = currstat;
  mode0 = currutmp->mode;

  setutmpmode(HIT);
  
  ++cuser.receivemsg;
  cuser.exp+= rpguser.race == 5 ? 5*rpguser.level : 3;
  bell();
  
  while(currutmp->msgcount>0)
  {
    memcpy(&oldmsg[no_oldmsg],&currutmp->msgs[0],sizeof(msgque));
    ++no_oldmsg;
    no_oldmsg %= MAX_REVIEW;
    if(oldmsg_count < MAX_REVIEW) oldmsg_count++;

    sprintf(msg, "[36m%s : [37m%-s",
            currutmp->msgs[0].last_userid,
            currutmp->msgs[0].last_call_in);
    sprintf(buf, "[1m[44m[33m★ %-80s[31m(%2d:%2d)[m",
            msg, tp->tm_hour, tp->tm_min);
    outmsgline(buf, 0);
    memcpy(&currutmp->msgs[0], &currutmp->msgs[1], 
           sizeof(msgque) * currutmp->msgcount);
    --currutmp->msgcount;
  
    do
    {
//       ch = dogetch();
       ch = igetch();
    }while ((ch != ' ') && (ch != '\r') && (ch != '\n') && (ch != Ctrl('R')));
  }
  
  currstat = currstat0;
  currutmp->mode = mode0;
  vs_restore(screen);
  
  if(watermode)
  {
    if(watermode<oldmsg_count) watermode++;
    t_display_new();
  }  
  refresh();
}

static void
multi_user_check()
{
  register user_info *ui;
  register pid_t pid;
  int cmpuids();
  char genbuf[3];

  if (HAS_PERM(PERM_SYSOP) || belong("etc/okid",cuser.userid))
    return;         /* wildcat:站長,跟okid裡的人不限制 */

  if (cuser.userlevel)
  {
    if (!(ui = (user_info *) search_ulist(cmpuids, usernum)))
      return;                   /* user isn't logged in */

    pid = ui->pid;
    if (!pid || (kill(pid, 0) == -1))
      return;                   /* stale entry in utmp file */

    getdata(b_lines, 0, "您想刪除其他重複的 login 嗎？[Y] ", genbuf, 2, LCECHO,0);

    if (genbuf[0] != 'n')
    {
      kill(pid, SIGHUP);
      log_usies("KICK ", cuser.username);
    }
    else
    {
      int nums = MULTI_NUMS;
      if (HAS_PERM(PERM_BM))
         nums += 0;
      if (count_multi() >= nums)
      {
         // system_abort();
         pressanykey("抱歉，已超過本站內定之同時重覆上站人數管制。");
         oflush();
         exit(1);               
      }
    }
  } 
  else  /* guest的話 */
  {
    if (count_multi() > MULTI_GUEST)
    {
      pressanykey("抱歉，目前已有太多 guest, 請稍後再試。");
      oflush();
      exit(1);
    }
  }
}

/* --------- */
/* bad login */
/* --------- */

static char str_badlogin[] = "logins.bad";


static void
logattempt(uid, type)
  char *uid;
  char type;                    /* '-' login failure   ' ' success */
{
  char fname[64];
  char genbuf[256]="\0";

  sprintf(genbuf, "%c%-12s[%s] %s@%s", type, uid,
    Cdate(&login_start_time), remoteusername, fromhost);
  f_cat(str_badlogin, genbuf);
  
  if (type == '-')
  {
    sprintf(genbuf, "[%s] %s\n", Cdate(&login_start_time), fromhost);
    sethomefile(fname, uid, str_badlogin);

    f_cat(fname, genbuf);
  }
}

#ifdef BSD44
static int
over_load()
{
  double cpu_load[3];

  getloadavg(cpu_load, 3);

  if(cpu_load[0] > 3 && cpu_load[1] > 3 && cpu_load[2] > 3)
    sysop_bbcall("請注意!系統負荷偏高 : %f %f %f",
      cpu_load[0], cpu_load[1],  cpu_load[2]);

  if(cpu_load[0] > MAX_CPULOAD)
  {
    pressanykey("目前系統負荷 %f 過高,請稍後再來",cpu_load[0]);
    return 1;
  }  
  return 0;
}
#endif

static void
login_query()
{
  char uid[IDLEN + 1], passbuf[PASSLEN];
  int attempts;
  char genbuf[200];
  extern struct UTMPFILE *utmpshm;

  resolve_utmp();
  attempts = utmpshm->number;
  term_init("vt100");
  initscr();
//  clear();
  
// Modify by Robert NeKo 00.09.09 -START-

  show_welcomes();
//  show_file("etc/Welcome_title",0,19,ONLY_COLOR);
//  move(12,0);
//  show_file("etc/Welcome_news",13,19,ONLY_COLOR);
//  move(20,0);
//  counter(BBSHOME"/log/counter/上站人次","光臨本站",1);
  
// -END-

  move(22,4);
  prints("[線上人數] %d", attempts);
  if (attempts >= MAXACTIVE)
  {
    pressanykey("目前站上人數已達上限，請您稍後再來。");
    oflush();
    sleep(1);
    exit(1);
  }
#ifdef BSD44
  if(over_load()) 
  {
    oflush();
    sleep(1);
    exit(1); 
  }
#endif
  attempts = 0;
  while (1)
  {
    if (attempts++ >= LOGINATTEMPTS)
    {
      pressanykey("錯誤太多次,掰掰~~~~~");
      oflush();
      exit(1);
    }
    getdata(22, 19, "[您的帳號] ",uid, IDLEN + 1 , DOECHO,0);
    if (ci_strcmp(uid, str_new) == 0)
    {
#ifdef LOGINASNEW
      new_register();
      break;
#else
      outs("本系統目前無法以 new 註冊, 請用 guest 進入\n");
      continue;
#endif
    }
/*
    else if (ci_strcmp(uid, STR_GUEST) == 0)
    {
      cuser.userlevel = 0;
      cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
      break;
    }
*/
    else if (uid[0] == '\0' || !dosearchuser(uid))
    {
      pressanykey(err_uid);
    }
    else if (strcmp(uid, STR_GUEST))
    {
      getdata(22, 45, "[您的密碼] ", passbuf, PASSLEN, PASS,0);
//      passbuf[PASSLEN] = '\0';
      if (!chkpasswd(cuser.passwd, passbuf))
      {
        logattempt(cuser.userid, '-');
        pressanykey(ERR_PASSWD);
      }
      else
      {
        /* SYSOP gets all permission bits */

//        if (!ci_strcmp(cuser.userid, str_sysop))
//          cuser.userlevel = ~0;

        if ( (HAS_PERM(PERM_SYSOP)
              || HAS_PERM(PERM_ACCOUNTS)
              || HAS_PERM(PERM_BOARD)
             )
             && (!belong("etc/trusted_port", fromport))
           )
        {
           logattempt(cuser.userid, '*');
           pressanykey("[系統警戒] 特殊帳號 請由 Trusted Port 進入 ");
           continue;
//           pressanykey(getenv("RFC931"));
        }   
//        if (0 && HAS_PERM(PERM_SYSOP) && !strncmp(getenv("RFC931"), "?@", 2)) {
//           logattempt(cuser.userid, '*');
//           outs("站長請由 trusted host 進入");
//           continue;
//        }
        else {
           logattempt(cuser.userid, ' ');
           break;
        }
      }
    }
    
// guest    
    else
    {                  
      cuser.userlevel = 0;
      cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
      break;
    }

  }

//  if(HAS_PERM(PERM_SYSOP))
//    sysop_bbcall("站長 %s 上線了!",cuser.userid);

    multi_user_check();
    sethomepath(genbuf, cuser.userid);
    mkdir(genbuf, 0755);
    srand(time(0) ^ getpid() ^ (getpid() << 10));
    srandom(time(0) ^ getpid() ^ (getpid() << 10));
}



/*
woju
*/
add_distinct(char* fname, char* line)
{
   FILE *fp;
   int n = 0;

   if (fp = fopen(fname, "a+")) {
      char buffer[80];
      char tmpname[100];
      FILE *fptmp;

      strcpy(tmpname, fname);
      strcat(tmpname, "_tmp");
      if (!(fptmp = fopen(tmpname, "w"))) {
         fclose(fp);
         return;
      }

      rewind(fp);
      while (fgets(buffer, 80, fp)) {
         char* p = buffer + strlen(buffer) - 1;

         if (p[-1] == '\n' || p[-1] == '\r')
            p[-1] = 0;
         if (!strcmp(buffer, line))
            break;
         sscanf(buffer + strlen(buffer) + 2, "%d", &n);
         fprintf(fptmp, "%s%c#%d\n", buffer, 0, n);
      }

      if (feof(fp))
         fprintf(fptmp, "%s%c#1\n", line, 0);
      else {
         sscanf(buffer + strlen(buffer) + 2, "%d", &n);
         fprintf(fptmp, "%s%c#%d\n", buffer, 0, n + 1);
         while (fgets(buffer, 80, fp)) {
            sscanf(buffer + strlen(buffer) + 2, "%d", &n);
            fprintf(fptmp, "%s%c#%d\n", buffer, 0, n);
         }
      }
      fclose(fp);
      fclose(fptmp);
      unlink(fname);
      rename(tmpname, fname);
   }
}


del_distinct(char* fname, char* line)
{
   FILE *fp;
   int n = 0;

   if (fp = fopen(fname, "r")) {
      char buffer[80];
      char tmpname[100];
      FILE *fptmp;

      strcpy(tmpname, fname);
      strcat(tmpname, "_tmp");
      if (!(fptmp = fopen(tmpname, "w"))) {
         fclose(fp);
         return;
      }

      rewind(fp);
      while (fgets(buffer, 80, fp)) {
         char* p = buffer + strlen(buffer) - 1;

         if (p[-1] == '\n' || p[-1] == '\r')
            p[-1] = 0;
         if (!strcmp(buffer, line))
            break;
         sscanf(buffer + strlen(buffer) + 2, "%d", &n);
         fprintf(fptmp, "%s%c#%d\n", buffer, 0, n);
      }

      if (!feof(fp))
         while (fgets(buffer, 80, fp)) {
            sscanf(buffer + strlen(buffer) + 2, "%d", &n);
            fprintf(fptmp, "%s%c#%d\n", buffer, 0, n);
         }
      fclose(fp);
      fclose(fptmp);
      unlink(fname);
      rename(tmpname, fname);
   }
}

#ifdef WHERE
/* Jaky and Ptt*/
int where (char *from)
{
   extern struct FROMCACHE *fcache;
   register int i,count,j;

   resolve_fcache();
   for (j=0;j<fcache->top;j++)
   {
      char *token=strtok(fcache->domain[j],"&");
      i=0;count=0;
      while(token)
      {
         if (strstr(from,token)) count++;
         token=strtok(NULL, "&");
         i++;
      }
      if (i==count) break;
   }
   if (i!=count) return 0;
   return j;
}
#endif

void
check_BM()      /* Ptt 自動取下離職板主權力 */
{
  int i;
  boardheader *bhdr;
  extern boardheader *bcache;
  extern int numboards;

  resolve_boards();
  cuser.userlevel &= ~PERM_BM;
  for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
      is_BM(bhdr->BM);
}

void
setup_utmp(mode)
  int mode;
{
  user_info uinfo;
  char buf[80];

  memset(&uinfo, 0, sizeof(uinfo));
  uinfo.pid = currpid = getpid();
  uinfo.uid = usernum;
  uinfo.mode = currstat = mode;
  uinfo.msgcount = 0;
  if(cuser.userlevel & PERM_BM) check_BM(); /* Ptt 自動取下離職板主權力 */
  uinfo.userlevel = cuser.userlevel;

  strcpy(uinfo.userid, cuser.userid);
  strcpy(uinfo.realname, cuser.realname);
  strcpy(uinfo.username, cuser.username);
  strcpy(uinfo.feeling, cuser.feeling);
  strncpy(uinfo.from, fromhost, 32);
  uinfo.from[31]='\0';
  uinfo.invisible = cuser.invisible;
  uinfo.pager     = cuser.pager;

  uinfo.from_alias = where(fromhost);
  setuserfile(buf, "remoteuser");
  add_distinct(buf, getenv("RFC931"));
  strcpy(uinfo.tty, tty_name);

  if (enter_uflag & CLOAK_FLAG)
      uinfo.invisible = YEA;

  getnewutmpent(&uinfo);
  friend_load();
}

static void
user_login()
{
  char ans[4];
  char genbuf[200];
  struct tm *ptime,*tmp;
  time_t now = time(0);
  int a;

  extern struct FROMCACHE *fcache;
  extern struct UTMPFILE *utmpshm;
  extern int fcache_semid;
  
  log_usies("ENTER", getenv("RFC931"));

  /* ------------------------ */
  /* 初始化 uinfo、flag、mode */
  /* ------------------------ */

  setup_utmp(LOGIN);
  currmode = MODE_STARTED;
  enter_uflag = cuser.uflag;

/* get local time */
  tmp = localtime(&cuser.lastlogin);

  update_data(); //wildcat: update user data
/*Ptt check 同時上線人數 */
  resolve_fcache();
  resolve_utmp();

  if((a=utmpshm->number)>fcache->max_user)
    {
      sem_init(FROMSEM_KEY,&fcache_semid);
      sem_lock(SEM_ENTER,fcache_semid);
      fcache->max_user = a;
      fcache->max_time = now;
      sem_lock(SEM_LEAVE,fcache_semid);
    }

#ifdef  INITIAL_SETUP
  if (!getbnum(DEFAULT_BOARD))
  {
    strcpy(currboard, "尚未選定");
  }
  else
#endif

  {
    brc_initial(DEFAULT_BOARD);
    set_board();
  }

  /* ------------ */
  /* 畫面處理開始 */
  /* ------------ */

/*  initscr(); */
//  reload_ucache();
//  reload_bcache();
//  reload_filmcache(); 
//  reload_fcache();
//  resetutmpent(); 

/*
Ptt
*/

  time(&now);
  ptime = localtime(&now);

  if((cuser.day == ptime->tm_mday) && (cuser.month == (ptime->tm_mon + 1)))
  {
    currutmp->birth  = 1;
  }
  else
  {
    more("etc/Welcome_login", NA);
    
/* Modify by Robert NeKo */

    move(t_lines - 2, 0);
    chkload(10);
    currutmp->birth = 0 ;
  }
  pressanykey(NULL);
  
/* wildcat : 搬家通知用
  if(belong(BBSHOME"/etc/oldip",fromhost))
  {
    more(BBSHOME"/etc/removal");
    abort_bbs();
  }    
*/

  more("log/day",NA);
  
  if (cuser.userlevel)          /* not guest */
  {
    if(!(HAS_PERM(PERM_DENYPOST)) && (HAS_HABIT(HABIT_SAYALOHA)) )
      do_aloha(MSG_ALOHA_LOGIN);  
  
    move(t_lines - 2, 0);
    prints("〈歡迎您第 [1;33m%d[0;37m 度拜訪本站〉", ++cuser.numlogins );
    
//    prints("〈歡迎您第 [1;33m%d[0;37m 度拜訪本站，\上次您是從 [1;33m%s[0;37m 連往本站，\n\我記得那天是 [1;33m%s[0;37m。\n",
//      ++cuser.numlogins, cuser.lasthost, Cdate(&cuser.lastlogin));

    pressanykey(NULL);

/* Ptt */
  if(currutmp->birth == 1)
   {
    more("etc/Welcome_birth",YEA);
//    brc_initial("Greeting");
//    set_board();
//    do_post();
   }
   
    setuserfile(genbuf, str_badlogin);
    if (more(genbuf, NA) != -1)
    {
      getdata(b_lines - 1, 0, "您要刪除以上錯誤嘗試的記錄嗎(Y/N)?[Y]",
        ans, 3, LCECHO,"Y");
      if (*ans != 'n')
        unlink(genbuf);
    }
    if(strcmp(cuser.userid, STR_GUEST))
       check_register();

    strncpy(cuser.lasthost, fromhost, 24);
    cuser.lasthost[23]='\0';
    substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);
    restore_backup();
    cursor_load("cursor");
  }
/* 板主提高信箱上限 */
  if(HAS_PERM(PERM_BM) && cuser.exmailbox < 100)
    cuser.exmailbox = 100;
  else if (!strcmp(cuser.userid, STR_GUEST))
  {
    char *nick[10] = {"路過的神","路過的貓","路過的狗","路過的豬","路過的鳥",
                  "路過的電子雞","何須問我名","神秘背影","遊魂","失憶的訪客"
                     };
    char *name[10] = {"艾維克", "菲尼絲", "凱爾", "摩里蘭", "艾芙琳",
                      "魯坦達", "泰拉罕", "伊凡", "達卡"  , "蕾娜"
                     };
    char *addr[10] = {"風系", "土系", "火系", "雷系", "水系",
                      "風系", "土系", "火系", "雷系", "天系"
                     };
    int sex[10] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

    int i = rand() % 10;
    sprintf(cuser.username, "%s", nick[i]);
    sprintf(currutmp->username, cuser.username);
    sprintf(cuser.realname, name[i]);
    sprintf(currutmp->realname, cuser.realname);
    sprintf(cuser.address, addr[i]);
    cuser.sex = sex[i];
    cuser.silvermoney = 10;
    cuser.habit = HABIT_GUEST;	/* guest's habit */
    currutmp->pager = 4;
    pressanykey(NULL);
  }
  if (bad_user(cuser.userid)) {
     sprintf(currutmp->username, "BAD_USER");
     cuser.userlevel = 0;
     cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
  }
  
//  if (!PERM_HIDE(currutmp))

     cuser.lastlogin = login_start_time;
  substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);

   if(cuser.numlogins < 2)
   {
     more("etc/newuser",YEA);
//     HELP();
     pressanykey("唷?! 新來的唷? 加油!");
     //brc_initial("Hi_All");
     //set_board();
     //do_post();
   }
   
  if(HAS_HABIT(HABIT_NOTE))
    more("note.ans",YEA);
  if(HAS_HABIT(HABIT_SEELOG))
    Log();
  if(!HAS_HABIT(HABIT_ALREADYSET) && cuser.userlevel)
    u_habit();
}

do_aloha(char *hello)
{
   int fd;
   PAL pal;
   char genbuf[256];

   setuserfile(genbuf, FN_ALOHA);
   if ((fd = open(genbuf, O_RDONLY)) > 0)
   {
      sprintf(genbuf, hello);
      while (read(fd, &pal, sizeof(pal)) == sizeof(pal)) {
         user_info *uentp;
         extern cmpuids();
         int tuid;

         if ( 
              (tuid = searchuser(pal.userid)) 
              && (tuid != usernum)
              && (uentp = (user_info *) search_ulistn(cmpuids, tuid, 1))
              && (
                  (uentp->userlevel & PERM_SYSOP)
                  || (
                      ((currutmp->invisible==0) 
                       || uentp->userlevel & PERM_SEECLOAK) 
                  && !(is_rejected(uentp) & 1))
                 )
              )    
            {
              my_write(uentp->pid, genbuf, MSG_ALOHA);
            }
      }
      close(fd);
   }
}

void
check_max_online()
{
  FILE *fp;
  int maxonline=0;
  time_t now = time(NULL);
  struct tm *ptime;

  ptime = localtime(&now);

  if(fp = fopen(".maxonline", "r"))
  {
    fscanf(fp, "%d", &maxonline);
    fclose(fp);
  }

  if ((count_ulist() > maxonline) && (fp = fopen(".maxonline", "w")))
  {
    sysop_bbcall("達到本站最大上線人數 %d 人",count_ulist()); 
    fprintf(fp, "%d", count_ulist());
    fclose(fp);
  }
  if(fp = fopen(".maxtoday", "r"))
  {
    fscanf(fp, "%d", &maxonline);
    if (count_ulist() > maxonline){
      fclose(fp);
      sysop_bbcall("達到本日最大上線人數 %d 人",count_ulist()); 
      fp = fopen(".maxtoday", "w");
      fprintf(fp, "%d", count_ulist());
    }
    fclose(fp);
  }
}

main(argc, argv)
  int argc;
  char **argv;
{
  extern struct commands cmdlist[];
  extern char currmaildir[32];

  /* ----------- */
  /* system init */
  /* ----------- */

  setgid(BBSGID);
  setuid(BBSUID);
  chdir(BBSHOME);  
  currmode = 0;
  srand(login_start_time = time(0));
  update_time = login_start_time;

  if (argc > 1)
     strcpy(fromhost, argv[1]);
  if (argc > 2)
     strcpy(tty_name, argv[2]);
  if (argc > 3)
     strcpy(remoteusername, argv[3]);
  if (argc > 4)
     strcpy(fromport,argv[4]);
  else
     strcpy(fromport,"0");

  fromhost[31]='\0';      
  if (!getenv("RFC931"))
     setenv("RFC931", fromhost, 1);

  atexit(leave_bbs);

  signal(SIGHUP, abort_bbs);
  signal(SIGBUS, abort_bbs);
  signal(SIGSEGV, abort_bbs);
#ifdef SIGSYS
  signal(SIGSYS, abort_bbs);
#endif
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  signal(SIGURG, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);

  signal(SIGUSR1, talk_request);
  signal(SIGUSR2, write_request);

  if (setjmp(byebye))
    abort_bbs();
  nice(3);  /* lower priority */

  init_tty();
  login_query();
  user_login();
  
/* wildcat : 修復中
  if(!HAVE_PERM(PERM_SYSOP))
  {
    pressanykey("整修中 , 禁止站長以外 user 登入");
    abort_bbs();
  }
*/
  check_max_online();
//  pressanykey("system testing");

  sprintf(currmaildir, "home/%s/mailbox/.DIR", cuser.userid);
//  if(dashs(currmaildir)==-1)       
//     sethomedir(currmaildir, cuser.userid);
     
  if (HAVE_PERM(PERM_SYSOP | PERM_BM))
  {
    DL_func("SO/vote.so:b_closepolls");
  }
  if (!HAVE_HABIT(HABIT_COLOR))
    showansi = 0;
    
#ifdef DOTIMEOUT
  init_alarm();
#else
  signal(SIGALRM, SIG_IGN);
#endif

  while (chkmailbox())
     m_read();
     
#ifdef HAVE_GAME
  waste_money();
#endif

//  m_postnotify(); 
  
//  force_board("Announce");

/*  
  {
    char buf[80];
    setbfile(buf, "VIP", FN_LIST);
    if(belong_list(buf,cuser.userid))
      force_board("VIP");
  }
*/
  
//  force_board("Boards");

  if(HAS_HABIT(HABIT_RPG) && !rpguser.race && cuser.userlevel)
  {
    rpg_help();
    DL_func("SO/rpg.so:rpg_race_c");
  }
  else
    cuser.habit ^= HABIT_RPG;

  if(HAS_HABIT(HABIT_FROM) && HAS_PERM(PERM_FROM))
  {
    char fbuf[50];
    sprintf(fbuf, "故鄉 [%s]：", currutmp->from);
    if(getdata(b_lines, 0, fbuf, currutmp->from, 17, DOECHO,0))
      currutmp->from_alias=0;
  }

  if(HAS_HABIT(HABIT_FEELING))
  {
    getdata(b_lines ,0,"今天的心情如何呢？", cuser.feeling, 5 ,DOECHO,cuser.feeling);
    cuser.feeling[4] = '\0';
    strcpy(currutmp->feeling, cuser.feeling);
    substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  }

//  if(HAS_HABIT(HABIT_NOTEMONEY))
//    DL_func("SO/mn.so:show_mn");
    
  if(cuser.exp >= rpguser.level*rpguser.level*10000 && rpguser.race)
  {
    pressanykey("你已經可以升級囉"); 
    DL_func("SO/rpg.so:rpg_guild");
  }
  if (HAS_PERM(PERM_ACCOUNTS) && dashf(fn_register))
    m_register();             
  
  domenu(MMENU, "主功\能表", (chkmail(0) ? 'M' : 'B'), cmdlist);
  return;
}

#ifdef HAVE_GAME
waste_money()
{
  while(cuser.silvermoney >= MAXMONEY
    && cuser.numlogins > 2)
  {
    clear();
    move(10,0);
    prints("你的銀幣上限為 %ld！\n\n\n\n",MAXMONEY);
    outs("請想辦法花掉一些 , 或是把錢轉成金幣吧!\n在商品大街的電子銀行中有換錢的選項 .");    
    pressanykey("你銀幣太多囉！想辦法花掉吧！");
    finance();
    game_list();
  }
}
#endif
