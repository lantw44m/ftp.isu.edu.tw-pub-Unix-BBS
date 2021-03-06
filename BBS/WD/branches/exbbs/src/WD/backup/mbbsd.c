/*-------------------------------------------------------*/
/* main.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* author : opus.bbs@bbs.cs.nthu.edu.tw                  */
/* target : BBS main/login/top-menu routines             */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#define _MAIN_C_

#define MAXMONEY ((cuser.totaltime * 10) + (cuser.numlogins * 100) + (cuser.numposts * 1000))

#include <varargs.h>
#include "bbs.h"

#include <sys/resource.h> 
#include <sys/wait.h> 
#include <sys/socket.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/telnet.h>
#include <syslog.h>

#define SOCKET_QLEN 4 
#define TH_LOW 100 
#define TH_HIGH 120
#define PID_FILE BBSHOME"/run/mbbsd.pid" 

#ifdef HAVE_CHKLOAD 
#define BANNER "【風與塵埃的對話】◎ 電子布告欄系統 ◎(wd.twbbs.org)\r\n\
 %s IP (140.124.201.23) \r\n"
#else 
#define BANNER "【風與塵埃的對話】◎ 電子布告欄系統 ◎(wd.twbbs.org)\r\n"
#endif

jmp_buf byebye;

int talkrequest = NA;

static uschar enter_uflag;
char genbuf[1024];

#ifdef SHOW_IDLE_TIME
char fromhost[STRLEN - 20] = "\0";
#else
char fromhost[STRLEN] = "\0";
#endif
char remoteusername[40];
int mbbsd = 1; 

void check_register();

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
  xuser.invisible = currutmp->invisible; /* 紀錄隱形狀態 by wildcat */
  xuser.totaltime += time(0) - update_time;
  xuser.numposts = cuser.numposts;
  xuser.feeling[4] = '\0';
  xuser.pagernum[6] = '\0';
  if (!HAS_PERM(PERM_DENYPOST) && !currutmp->invisible)
     do_aloha("<<下站通知>> -- 我走囉！");
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

  clear();
  refresh();
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
   sprintf(buf, COLOR2"[1m★ [37m%s %s [m",
      currutmp->msgs[0].last_userid,
      currutmp->msgs[0].last_call_in);

   move(b_lines, 0);
   clrtoeol();
   refresh();
   outmsg(buf);
}

msgque oldmsg[MAX_REVIEW];   /* 丟過去的水球 */
char   no_oldmsg=0,oldmsg_count=0;            /* pointer */

static void
write_request()
{
  time_t now;
  extern char watermode;
/*  Half-hour remind  */
  if(*currmsg) {
    outmsg(currmsg);
    refresh();
    bell();
    *currmsg = 0;
    return;
  }

  time(&now);

#ifdef  LINUX
  signal(SIGUSR2, write_request);
#endif

  update_data();
  ++cuser.receivemsg;
  cuser.exp+= rpguser.race == 5 ? 15*rpguser.level : 3;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  bell();
  show_last_call_in();
// wildcat patch : 看不到水球??!!
  currutmp->msgcount--;
  memcpy(&oldmsg[no_oldmsg],&currutmp->msgs[0],sizeof(msgque));
  no_oldmsg++;
  no_oldmsg %= MAX_REVIEW;
  if(oldmsg_count < MAX_REVIEW) oldmsg_count++;
  if(watermode)
  {
    if(watermode<oldmsg_count) watermode++;
    t_display_new();
  }
  refresh();
  currutmp->msgcount = 0;
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

    getdata(0, 0, "您想刪除其他重複的 login (Y/N)嗎？[Y] ", genbuf, 3, LCECHO,0);

    if (genbuf[0] != 'n')
    {
      kill(pid, SIGHUP);
      log_usies("KICK ", cuser.username);
    }
    else
    {
      int nums = MULTI_NUMS;
      if(HAS_PERM(PERM_BM))
        nums += 2;
      if (count_multi() >= nums)
        system_abort();
    }
  } 
  else  /* guest的話 */
  {
    if (count_multi() > 512)
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
  char fname[40];
  char genbuf[200];

  sprintf(genbuf, "%c%-12s[%s] %s@%s\n", type, uid,
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
  clear();
  show_file("etc/Welcome_title",0,10,ONLY_COLOR);
  move(10,0);
  counter(BBSHOME"/log/counter/上站人次","光臨本站",1);
  show_file("etc/Welcome_news",12,12,ONLY_COLOR);
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
      exit(1);
    }
    getdata(16, 54, "[您的帳號] ",uid, IDLEN + 1 , DOECHO, NULL);
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
    else if (uid[0] == '\0' || !dosearchuser(uid))
    {
      pressanykey(err_uid);
    }
    else if (strcmp(uid, STR_GUEST))
    {
      getdata(18, 54, "[您的密碼] ", passbuf, PASSLEN, PASS, NULL);
      passbuf[8] = '\0';
      if (!chkpasswd(cuser.passwd, passbuf))
      {
        logattempt(cuser.userid, '-');
        pressanykey(ERR_PASSWD);
      }
      else
      {
        /* SYSOP gets all permission bits */

        if (!ci_strcmp(cuser.userid, str_sysop))
          cuser.userlevel = ~0;
        if (0 && HAS_PERM(PERM_SYSOP) && !strncmp(getenv("RFC931"), "?@", 2)) {
           logattempt(cuser.userid, '*');
           outs("站長請由 trusted host 進入");
           continue;
        }
        else {
           logattempt(cuser.userid, ' ');
           break;
        }
      }
    }
    else
    {                           /* guest */
      cuser.userlevel = 0;
      cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
      break;
    }
  }

/*  if(HAS_PERM(PERM_SYSOP))
    sysop_bbcall("站長 %s 上線了!",cuser.userid); */
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
  uinfo.lastact = time(NULL);

  strcpy(uinfo.userid, cuser.userid);
  strcpy(uinfo.realname, cuser.realname);
  strcpy(uinfo.username, cuser.username);
  strcpy(uinfo.feeling, cuser.feeling);
  strncpy(uinfo.from, fromhost, 23);
  uinfo.invisible = cuser.invisible%2;
  uinfo.pager     = cuser.pager%5;
  uinfo.brc_id    = 0;

/* Ptt WHERE*/

#ifdef WHERE
  uinfo.from_alias = where(fromhost);
#else
  uinfo.from_alias = 0;
#endif
  setuserfile(buf, "remoteuser");
  add_distinct(buf, getenv("RFC931"));

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
  
  log_usies("ENTER", getenv("RFC931")/* fromhost */);

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

  if (!(HAS_PERM(PERM_SYSOP) && HAS_PERM(PERM_DENYPOST)))
     do_aloha("<<上站通知>> -- 我來啦！");
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
    currutmp->birth = 0 ;
  }
/* wildcat : 搬家通知用
  if(belong(BBSHOME"/etc/oldip",fromhost))
  {
    more(BBSHOME"/etc/removal");
    abort_bbs();
  }    
*/
  if (cuser.userlevel)          /* not guest */
  {
    move(t_lines - 3, 0);
    prints("      歡迎您第 [1;33m%d[0;37m 度拜訪本站，\
上次您是從 [1;33m%s[0;37m 連往本站，\n\
     我記得那天是 [1;33m%s[0;37m。\n",
      ++cuser.numlogins, cuser.lasthost,
      Cdate(&cuser.lastlogin));
    pressanykey(NULL);


/* Ptt */
  if(currutmp->birth == 1)
   {
    more("etc/Welcome_birth",YEA);
    brc_initial("Greeting");
    set_board();
    do_post();
   }
    setuserfile(genbuf, str_badlogin);
    if (more(genbuf, NA) != -1)
    {
      getdata(b_lines - 1, 0, "您要刪除以上錯誤嘗試的記錄嗎(Y/N)?[Y]",
        ans, 3, LCECHO,"Y");
      if (*ans != 'n')
        unlink(genbuf);
    }
    check_register();
    strncpy(cuser.lasthost, fromhost, 24);
    substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);
    cuser.lasthost[23] = '\0';
    restore_backup();
  }
/* 板主提高信箱上限 */
  if(HAS_PERM(PERM_BM) && cuser.exmailbox < 100)
    cuser.exmailbox = 100;
  else if (!strcmp(cuser.userid, STR_GUEST))
  {
    char *nick[10] = {"麵包", "美女圖", "FreeBSD", "DBT目錄", "mp3",
                      "女王頭", "病毒", "童年", "石像", "386 CPU"
                     };
    char *name[10] = {"卡殺米亞", "白鳥澤麗子", "風與塵埃小站", "狂x或保x捷", 
                      "Wu Bai & China Blue","伊力沙白", "C-brain", 
                      "時間", "我", "Intel inside" 
                      };
    char *addr[10] = {"法蘭絲", "www.***.or.jp", "M$", "地下光碟工廠", 
                      "各大唱片行","野柳", "5.25吋磁碟片", "seiko手錶", 
                      "地下迷宮15層", "資源回收桶"};
    int sex[10] = {6, 4, 7, 7, 2, 6, 0, 7, 7, 0};

    int i = rand() % 10;
    sprintf(cuser.username, "被風化的%s", nick[i]);
    sprintf(currutmp->username, cuser.username);
    sprintf(cuser.realname, name[i]);
    sprintf(currutmp->realname, cuser.realname);
    sprintf(cuser.address, addr[i]);
    cuser.sex = sex[i];
    cuser.silvermoney = 300;
    cuser.habit = HABIT_GUEST;	/* guest's habit */
    currutmp->pager = 2;
    pressanykey(NULL);
  }
  if (bad_user(cuser.userid)) {
     sprintf(currutmp->username, "BAD_USER");
     cuser.userlevel = 0;
     cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
  }
  if (!PERM_HIDE(currutmp))
     cuser.lastlogin = login_start_time;
  substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);

   if(cuser.numlogins < 2)
   {
     more("etc/newuser",YEA);
     HELP();
     pressanykey("唷?!新來的唷?自我介紹一下吧!");
     brc_initial("Hi_All");
     set_board();
     do_post();
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
   FILE* fp;
   char userid[80];
   char genbuf[200];

   setuserfile(genbuf, "aloha");
   if (fp = fopen(genbuf, "r")) {
      sprintf(genbuf + 1, hello);
      *genbuf = 1;
      while (fgets(userid, 80, fp)) {
         user_info *uentp;
         extern cmpuids();
         int tuid;

         if ( (tuid = searchuser(userid))  && tuid != usernum &&
             (uentp = (user_info *) search_ulistn(cmpuids, tuid, 1)) &&
             ((uentp->userlevel & PERM_SYSOP) || ((!currutmp->invisible || 
           uentp->userlevel & PERM_SEECLOAK) && !(is_rejected(uentp) & 1))))    
            my_write(uentp->pid, genbuf);
      }
      fclose(fp);
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


void
do_term_init()
{
  char genbuf[10];
  if (!term_init("vt100"))
  {
    do
    {
      getdata(0, 0, "\n終端機型態錯誤！請輸入 [vt100]：", genbuf, 8, DOECHO,0);
    } while (!term_init(genbuf));
  }
  initscr();
}

void
start_client()
{
  extern struct commands cmdlist[];
  extern char currmaildir[32];

  /* ----------- */
  /* system init */
  /* ----------- */
  currmode = 0;
  update_time = login_start_time;

/*
woju
*/
{
   char cmd[80] = "?@";

   if (!getenv("RFC931"))
      setenv("RFC931", strcat(cmd, fromhost), 1);
}

  signal(SIGHUP, abort_bbs);
  signal(SIGBUS, abort_bbs);
  signal(SIGSEGV, abort_bbs);
#ifdef SIGSYS
  signal(SIGSYS, abort_bbs);
#endif
//  signal(SIGINT, SIG_IGN);
//  signal(SIGQUIT, SIG_IGN);
//  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

//  signal(SIGURG, SIG_IGN);
//  signal(SIGTSTP, SIG_IGN);
//  signal(SIGTTIN, SIG_IGN);
//  signal(SIGTTOU, SIG_IGN);

// 收到訊息
  signal(SIGUSR1, talk_request);
// 送出訊息
  signal(SIGUSR2, write_request);

  if (setjmp(byebye))
    abort_bbs();

  dup2(0, 1);
  
  do_term_init();
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
  sethomedir(currmaildir, cuser.userid);
#ifdef DOTIMEOUT
  init_alarm();
#else
  signal(SIGALRM, SIG_IGN);
#endif
  if (HAVE_PERM(PERM_SYSOP | PERM_BM))
  {
    DL_func("SO/vote.so:b_closepolls");
  }
  if (!HAVE_HABIT(HABIT_COLOR))
    showansi = 0;
  while (chkmailbox())
     m_read();
#ifdef HAVE_GAME
  waste_money();
#endif
  m_postnotify(); 
  force_board("Announce");
  {
    char buf[80];
    setbfile(buf, "VIP", FN_LIST);
    if(belong_list(buf,cuser.userid))
      force_board("VIP");
  }
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

  if(HAS_HABIT(HABIT_NOTEMONEY))
    DL_func("SO/mn.so:show_mn");
  if(cuser.exp >= rpguser.level*rpguser.level*10000 && rpguser.race)
  {
    pressanykey("你已經可以升級囉"); 
    DL_func("SO/rpg.so:rpg_guild");
  }
  if (HAS_PERM(PERM_ACCOUNTS) && dashf(fn_register))
    m_register();             
  domenu(MMENU, "主功\能表", (chkmail(0) ? 'M' : 'B'), cmdlist);
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
    outs("請想辦法花掉一些 , 或是把錢轉成金幣吧!\n在商業中心的銀行中有換錢的選項 .");    
    pressanykey("你錢太多囉！想辦法花掉吧！");
    finance();
    game_list();
  }
}
#endif

/* ----------------------------------------------------- */
/* FSA (finite state automata) for telnet protocol       */
/* ----------------------------------------------------- */


static void
telnet_init()
{
  static char svr[] = {
    IAC, DO, TELOPT_TTYPE,
    IAC, SB, TELOPT_TTYPE, TELQUAL_SEND, IAC, SE,
    IAC, WILL, TELOPT_ECHO,
    IAC, WILL, TELOPT_SGA
  };

  register int n, len;
  register char *cmd, *data;
  int rset, oset;
  struct timeval to;
  char buf[256];

  data = buf;


  to.tv_sec = 1;
  rset = to.tv_usec = 0;
  FD_SET(0, (fd_set *) & rset);
  oset = rset;
  for (n = 0, cmd = svr; n < 4; n++)
  {
    len = (n == 1 ? 6 : 3);
    write(0, cmd, len);
    cmd += len;

    if (select(1, (fd_set *) & rset, NULL, NULL, &to) > 0)
      recv(0, buf, sizeof(buf), 0);
//      read(0, data, sizeof(buf));
    rset = oset;
  }
}


/* ----------------------------------------------- */
/* 取得 remote user name 以判定身份                */
/* ----------------------------------------------- */


/*
 * rfc931() speaks a common subset of the RFC 931, AUTH, TAP, IDENT and RFC
 * 1413 protocols. It queries an RFC 931 etc. compatible daemon on a remote
 * host to look up the owner of a connection. The information should not be
 * used for authentication purposes. This routine intercepts alarm signals.
 *
 * Author: Wietse Venema, Eindhoven University of Technology, The Netherlands.
 */

#include <setjmp.h>

#define STRN_CPY(d,s,l) { strncpy((d),(s),(l)); (d)[(l)-1] = 0; }
#define RFC931_TIMEOUT   10
#define RFC931_PORT     113     /* Semi-well-known port */
#define ANY_PORT        0       /* Any old port will do */


/* ------------------------- */
/* timeout - handle timeouts */
/* ------------------------- */


static void
timeout(sig)
  int sig;
{
  (void) longjmp(byebye, sig);
}


static void
getremotename(from, rhost, rname)
  struct sockaddr_in *from;
  char *rhost;
  char *rname;
{
  struct sockaddr_in our_sin;
  struct sockaddr_in rmt_sin;
  unsigned rmt_port, rmt_pt;
  unsigned our_port, our_pt;
  FILE *fp;
  char buffer[512], user[80], *cp;
  int s;
  struct hostent *hp;

  /* get remote host name */

  hp = NULL;
  if (setjmp(byebye) == 0)
  {
    (void) signal(SIGALRM, timeout);
    (void) alarm(3);
    hp = gethostbyaddr((char *) &from->sin_addr, sizeof(struct in_addr),
      from->sin_family);
    (void) alarm(0);
  }
  (void) strcpy(rhost, hp ? hp->h_name : (char *) inet_ntoa(from->sin_addr));

  /*
   * Use one unbuffered stdio stream for writing to and for reading from the
   * RFC931 etc. server. This is done because of a bug in the SunOS 4.1.x
   * stdio library. The bug may live in other stdio implementations, too.
   * When we use a single, buffered, bidirectional stdio stream ("r+" or "w+"
   * mode) we read our own output. Such behaviour would make sense with
   * resources that support random-access operations, but not with sockets.
   */

  s = sizeof our_sin;
  if (getsockname(0, (struct sockaddr*)&our_sin, &s) < 0)
    return;

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return;
  }

  if (!(fp = fdopen(s, "r+")))
  {
    (void) close(s);
    return;
  }

  /*
   * Set up a timer so we won't get stuck while waiting for the server.
   */

  if (setjmp(byebye) == 0)
  {
    (void) signal(SIGALRM, timeout);
    (void) alarm(RFC931_TIMEOUT);

    /*
     * Bind the local and remote ends of the query socket to the same IP
     * addresses as the connection under investigation. We go through all
     * this trouble because the local or remote system might have more than
     * one network address. The RFC931 etc. client sends only port numbers;
     * the server takes the IP addresses from the query socket.
     */

    our_pt = ntohs(our_sin.sin_port);
    our_sin.sin_port = htons(ANY_PORT);

    rmt_sin = *from;
    rmt_pt = ntohs(rmt_sin.sin_port);
    rmt_sin.sin_port = htons(RFC931_PORT);

    (void) setbuf(fp, (char *) 0);
    s = fileno(fp);

    if (bind(s, (struct sockaddr *) & our_sin, sizeof(our_sin)) >= 0 &&
      connect(s, (struct sockaddr *) & rmt_sin, sizeof(rmt_sin)) >= 0)
    {
      /*
       * Send query to server. Neglect the risk that a 13-byte write would
       * have to be fragmented by the local system and cause trouble with
       * buggy System V stdio libraries.
       */

      (void) fprintf(fp, "%u,%u\r\n", rmt_pt, our_pt);
      (void) fflush(fp);

      /*
       * Read response from server. Use fgets()/sscanf() so we can work
       * around System V stdio libraries that incorrectly assume EOF when a
       * read from a socket returns less than requested.
       */

      if (fgets(buffer, sizeof(buffer), fp) && !ferror(fp) && !feof(fp)
        && sscanf(buffer, "%u , %u : USERID :%*[^:]:%79s",
          &rmt_port, &our_port, user) == 3
        && rmt_pt == rmt_port && our_pt == our_port)
      {

        /*
         * Strip trailing carriage return. It is part of the protocol, not
         * part of the data.
         */

        if (cp = (char *) strchr(user, '\r'))
          *cp = 0;
        strcpy(rname, user);
      }
    }
    (void) alarm(0);
  }
  (void) fclose(fp);
}


/* ----------------------------------------------------- */
/* stand-alone daemon                                    */
/* ----------------------------------------------------- */


static int mainset;             /* read file descriptor set */
static struct sockaddr_in xsin;


static void
reapchild()
{
  int state, pid;

  while ((pid = waitpid(-1, &state, WNOHANG | WUNTRACED)) > 0);
}


static void
start_daemon()
{
  int n;
  char buf[80];

  /*
   * More idiot speed-hacking --- the first time conversion makes the C
   * library open the files containing the locale definition and time zone.
   * If this hasn't happened in the parent process, it happens in the
   * children, once per connection --- and it does add up.
   */

  time_t dummy = time(NULL);
  struct tm *dummy_time = localtime(&dummy);
  (void) strftime(buf, 80, "%d/%b/%Y:%H:%M:%S", dummy_time);
/*
  (void) gethostname(myhostname, sizeof(myhostname));
*/

  if (n=fork())
   {
    printf("pid[%d]\n",n);
    exit(0);
   }
  n = getdtablesize();
  sprintf(genbuf, "%d\t%s", getpid(), buf);

  while (n)
    (void) close(--n);

  n = open("/dev/tty", O_RDWR);
  if (n > 0)
  {
    (void) ioctl(n, TIOCNOTTY, (char *) 0);
    (void) close(n);
  }

  for (n = 1; n < NSIG; n++)
    (void) signal(n, SIG_IGN);
}


static void
close_daemon()
{
  exit(0);
}


static int
bind_port(port)
  int port;
{
  int sock, on;

  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  on = 1;
  (void) setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
  (void) setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &on, sizeof(on));

  on = 0;
  (void) setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &on, sizeof(on));

  xsin.sin_port = htons(port);
  if (bind(sock, (struct sockaddr *)&xsin, sizeof xsin) < 0) {
    syslog(LOG_INFO, "bbsd bind_port can't bind to %d",port);
    exit(1);
  }

  if (listen(sock, SOCKET_QLEN) < 0) {
    syslog(LOG_INFO, "bbsd bind_port can't listen to %d",port);
    exit(1);
  }

  /*(void)*/ FD_SET(sock, (fd_set *) & mainset);
  return sock;
}

bad_host(char* name)
{
   FILE* list;
   char buf[40];

  if (list = fopen("etc/bad_host", "r")) {
     while (fgets(buf, 40, list)) {
        buf[strlen(buf) - 1] = '\0';
        if (!strcmp(buf, name))
           return 1;
        if (buf[strlen(buf) - 1] == '.' && !strncmp(buf, name, strlen(buf)))
           return 1;
        if (*buf == '.' && strlen(buf) < strlen(name) &&
            !strcmp(buf, name + strlen(name) - strlen(buf)))
           return 1;
     }
     fclose(list);
  }
  return 0;
}


main(argc, argv,envp)
  int argc;
  char *argv[];
  char *envp[];
{
  extern int errno;
  register int msock, csock;    /* socket for Master and Child */
  register int nfds;            /* number of sockets */
  register pid_t pid;
  int readset;
  int value;
  struct timeval tv;

  /* --------------------------------------------------- */
  /* setup standalone                                    */
  /* --------------------------------------------------- */
  start_daemon();

  (void) signal(SIGCHLD, reapchild);
  (void) signal(SIGTERM, close_daemon);


  /* --------------------------------------------------- */
  /* port binding                                        */
  /* --------------------------------------------------- */

  xsin.sin_family = AF_INET;

  sprintf(margs,"%s ",argv[0]);
  if (argc > 1)
  {
    msock = -1;
    for (nfds = 1; nfds < argc; nfds++)
    {
      csock = atoi(argv[nfds]);
      if (csock > 0)
        {
         msock = bind_port(csock);
         strcat(margs,argv[nfds]);
	 strcat(margs," ");
	}
      else
         break;
    }
    if (msock < 0)
      exit(1);
  }
  else
  {
    static int ports[] = {23,3333,3006};

    for (nfds = 0; nfds < sizeof(ports) / sizeof(int); nfds++)
    {
      csock = ports[nfds];
      msock = bind_port(csock);

      sprintf(genbuf + 512, "\t%d", csock);
      strcat(genbuf, genbuf + 512);
    }
  }
  nfds = msock + 1;

  /* --------------------------------------------------- */
  /* Give up root privileges: no way back from here      */
  /* --------------------------------------------------- */

  (void) setgid(BBSGID);
  (void) setuid(BBSUID);
  (void) chdir(BBSHOME);

  f_cat(PID_FILE, genbuf); 


  initsetproctitle(argc, argv, envp);
  printpt("%s: listening ",margs);

  /* --------------------------------------------------- */
  /* main loop                                           */
  /* --------------------------------------------------- */

/*
  th_low = (argc > 1) ? atoi(argv[1]) : TH_LOW;
  th_high = (argc > 2) ? atoi(argv[2]) : TH_HIGH;
*/

  tv.tv_sec = 60 * 30;
  tv.tv_usec = 0;


  for (;;)
  {
again:

    readset = mainset;
    msock = select(nfds, (fd_set *) & readset, NULL, NULL, &tv);

    if (msock < 0)
    {
      goto again;
    }
    else if (msock == 0)        /* No network traffic */
      continue;

    msock = 0;
    csock = 1;
    for (;;)
    {
      if (csock & readset)
        break;
      if (++msock >= nfds)
        goto again;
      csock <<= 1;
    }

    value = sizeof xsin;
    do
    {
      csock = accept(msock, (struct sockaddr *)&xsin, &value);
    } while (csock < 0 && errno == EINTR);

    if (csock < 0)
    {
      goto again;
    }

/*
    pid = *totaluser;
    if (pid >= MAXACTIVE)
    {
      char buf[128];

      (void) sprintf(buf, "目前線上人數 [%d] 人，客滿了，請稍後再來", pid);
      (void) write(csock, buf, strlen(buf));
      (void) close(csock);
      goto again;
    }
*/
    pid = fork();

    if (!pid)
    {
     char info[256];
     time_t now;

     initsetproctitle(argc, argv, envp);
     nice(2);      /*  Ptt lower priority */
     sprintf(info, BANNER, genbuf);   
     info[255]=0;
     write(csock,info , strlen(info));
      while (--nfds >= 0)
        (void) close(nfds);
      (void) dup2(csock, 0);
      (void) close(csock);
      time(&now);
      login_start_time = now;

      getremotename(&xsin, fromhost, remoteusername);   /* FC931 */

      /* ban 掉 bad host / bad user 
      if (bad_host(fromhost) && !strcmp(remoteusername, "?"))
         exit(1);
      */
      setenv("REMOTEHOST", fromhost, 1);
      setenv("REMOTEUSERNAME", remoteusername, 1);
      {
        char RFC931[80];
        sprintf(RFC931, "%s@%s", remoteusername, fromhost);
        setenv("RFC931", RFC931, 1);
      }
      telnet_init();
      printpt("mbbsd : %s",fromhost);
      start_client();
    }
    
    (void) close(csock);
  }
}
