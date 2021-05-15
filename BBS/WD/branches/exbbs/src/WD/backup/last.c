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
#define MAXMONEY ((cuser.totaltime * 10) + (cuser.numlogins * 100) + (cuser.numposts * 1000))

jmp_buf byebye;

int talkrequest = NA;

static uschar enter_uflag;

#ifdef SHOW_IDLE_TIME
char fromhost[STRLEN - 20] = "\0";
char tty_name[20] = "\0";
#else
char fromhost[STRLEN] = "\0";
#endif
char remoteusername[40];

void check_register();
int mbbsd = 0;
/*
int Argc;
char** Argv;
*/

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
    prints("¡q¨t²Î­t¸ü = %.2f %.2f %.2f (¤W­­ = %2d)¡r",cpu_load[0], cpu_load[1], cpu_load[2], limit);
}                                                    


/* ----------------------------------------------------- */
/* Â÷¶} BBS µ{¦¡                                         */
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
  extern void auto_backup();    /* ½s¿è¾¹¦Û°Ê³Æ¥÷ */
  userec xuser;
  int diff = (time(0) - login_start_time) / 60;

  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  auto_backup();
  setflags(PAGER_FLAG, currutmp->pager != 1);
  setflags(CLOAK_FLAG, currutmp->invisible);
  xuser.pager = currutmp->pager;	 /* °O¿ýpagerª¬ºA, add by wisely */
  xuser.invisible = currutmp->invisible; /* ¬ö¿ýÁô§Îª¬ºA by wildcat */
  xuser.totaltime += time(0) - update_time;
  xuser.numposts = cuser.numposts;
  xuser.feeling[4] = '\0';
  xuser.pagernum[6] = '\0';
  if (!HAS_PERM(PERM_DENYPOST) && !currutmp->invisible)
     do_aloha("<<¤U¯¸³qª¾>> -- §Ú¨«Åo¡I");
  purge_utmp(currutmp);
  if(!diff && cuser.numlogins && strcmp(cuser.userid,STR_GUEST))
    xuser.numlogins = --cuser.numlogins; /* Leeym ¤W¯¸°±¯d®É¶¡­­¨î¦¡ */
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
  printf("ÁÂÁÂ¥úÁ{, °O±o±`¨Ó³á !\n");
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
/* µn¿ý BBS µ{¦¡                                         */
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
   * Linux ¤U³sÄò page ¹ï¤è¨â¦¸´N¥i¥H§â¹ï¤è½ð¥X¥h¡G ³o¬O¥Ñ©ó¬Y¨Ç¨t²Î¤@ nal
   * ¶i¨Ó´N·|±N signal handler ³]©w¬°¤º©wªº handler, ¤£©¯ªº¬O default ¬O±Nµ{
   * erminate. ¸Ñ¨M¤èªk¬O¨C¦¸ signal ¶i¨Ó´N­«³] signal handler
   */

  signal(SIGUSR1, talk_request);
#endif
  bell();
  if (currutmp->msgcount) {
     char buf[200];
     time_t now = time(0);

     sprintf(buf, "[33;41m¡¹%s[34;47m [%s] %s [0m",
        (currutmp->destuip)->userid,  my_ctime(&now),
        (currutmp->sig == 2) ? "­«­n®ø®§¼s¼½¡I(½ÐCtrl-U,l¬d¬Ý¼ö°T°O¿ý)" : "©I¥s¡B©I¥s¡AÅ¥¨ì½Ð¦^µª");
     move(1, 0);
//     clrtoeol();
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
   sprintf(buf, COLOR2"[1m¡¹ [37m%s %s [m",
      currutmp->msgs[0].last_userid,
      currutmp->msgs[0].last_call_in);

//   move(b_lines, 0);
   move(1, 0);
   clrtoeol();
   refresh();
   outmsg(buf);
}

msgque oldmsg[MAX_REVIEW];   /* ¥á¹L¥hªº¤ô²y */
char   no_oldmsg=0,oldmsg_count=0;            /* pointer */

static void
write_request()
{
  time_t now;
  extern char watermode;
/*  Half-hour remind  */

  if(*currmsg)
  {
//    outmsg(currmsg);
    
    move(1, 0);
//    clrtoeol();
    outs(currmsg);
    
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
  memcpy(&oldmsg[no_oldmsg],&currutmp->msgs[0],sizeof(msgque));
  ++no_oldmsg;
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
    return;         /* wildcat:¯¸ªø,¸òokid¸Ìªº¤H¤£­­¨î */

  if (cuser.userlevel)
  {
    if (!(ui = (user_info *) search_ulist(cmpuids, usernum)))
      return;                   /* user isn't logged in */

    pid = ui->pid;
    if (!pid || (kill(pid, 0) == -1))
      return;                   /* stale entry in utmp file */

    getdata(0, 0, "±z·Q§R°£¨ä¥L­«½Æªº login (Y/N)¶Ü¡H[Y] ", genbuf, 3, LCECHO,0);

    if (genbuf[0] != 'n')
    {
      kill(pid, SIGHUP);
      log_usies("KICK ", cuser.username);
    }
    else
    {
      int nums = MULTI_NUMS;
      if (HAS_PERM(PERM_BM))
         nums += 1;
      if (count_multi() >= nums)
      {
         // system_abort();
         pressanykey("©êºp¡A¤w¶W¹L¥»¯¸¤º©w¤§¦P®É­«ÂÐ¤W¯¸¤H¼ÆºÞ¨î¡C");
         oflush();
         exit(1);               
      }
    }
  } 
  else  /* guestªº¸Ü */
  {
    if (count_multi() > MULTI_GUEST)
    {
      pressanykey("©êºp¡A¥Ø«e¤w¦³¤Ó¦h guest, ½Ðµy«á¦A¸Õ¡C");
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
    sysop_bbcall("½Ðª`·N!¨t²Î­t²ü°¾°ª : %f %f %f",
      cpu_load[0], cpu_load[1],  cpu_load[2]);

  if(cpu_load[0] > MAX_CPULOAD)
  {
    pressanykey("¥Ø«e¨t²Î­t²ü %f ¹L°ª,½Ðµy«á¦A¨Ó",cpu_load[0]);
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
  
// Modify by ROBERT_LIU 00.09.09 -START-

  show_file("etc/Welcome_title",0,19,ONLY_COLOR);
//  move(12,0);
//  show_file("etc/Welcome_news",13,19,ONLY_COLOR);
//  move(20,0);
//  counter(BBSHOME"/log/counter/¤W¯¸¤H¦¸","¥úÁ{¥»¯¸",1);
  
// -END-

  if (attempts >= MAXACTIVE)
  {
    pressanykey("¥Ø«e¯¸¤W¤H¼Æ¤w¹F¤W­­¡A½Ð±zµy«á¦A¨Ó¡C");
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
      pressanykey("¿ù»~¤Ó¦h¦¸,ÙTÙT~~~~~");
      oflush();
      exit(1);
    }
    getdata(20, 12, "[±zªº±b¸¹] ",uid, IDLEN + 1 , DOECHO,0);
    if (ci_strcmp(uid, str_new) == 0)
    {
#ifdef LOGINASNEW
      new_register();
      break;
#else
      outs("¥»¨t²Î¥Ø«eµLªk¥H new µù¥U, ½Ð¥Î guest ¶i¤J\n");
      continue;
#endif
    }
    else if (ci_strcmp(uid, STR_GUEST) == 0)
    {
      cuser.userlevel = 0;
      cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
      break;
    }
    else if (uid[0] == '\0' || !dosearchuser(uid))
    {
      pressanykey(err_uid);
    }
    else if (strcmp(uid, STR_GUEST))
    {
      getdata(20, 35, "[±zªº±K½X] ", passbuf, PASSLEN, PASS,0);
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
           outs("¯¸ªø½Ð¥Ñ trusted host ¶i¤J");
           continue;
        }
        else {
           logattempt(cuser.userid, ' ');
           break;
        }
      }
    }
    
/* guest    
    else
    {                  
      cuser.userlevel = 0;
      cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
      break;
    }
*/

  }

/*  if(HAS_PERM(PERM_SYSOP))
    sysop_bbcall("¯¸ªø %s ¤W½u¤F!",cuser.userid); */
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
check_BM()      /* Ptt ¦Û°Ê¨ú¤UÂ÷Â¾ªO¥DÅv¤O */
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
  if(cuser.userlevel & PERM_BM) check_BM(); /* Ptt ¦Û°Ê¨ú¤UÂ÷Â¾ªO¥DÅv¤O */
  uinfo.userlevel = cuser.userlevel;

  strcpy(uinfo.userid, cuser.userid);
  strcpy(uinfo.realname, cuser.realname);
  strcpy(uinfo.username, cuser.username);
  strcpy(uinfo.feeling, cuser.feeling);
  strncpy(uinfo.from, fromhost, 23);
  uinfo.invisible = cuser.invisible;
  uinfo.pager     = cuser.pager;

/* Ptt WHERE*/

#ifdef WHERE
  uinfo.from_alias = where(fromhost);
#else
  uinfo.from_alias = 0;
#endif
  setuserfile(buf, "remoteuser");
  add_distinct(buf, getenv("RFC931"));

#ifdef SHOW_IDLE_TIME
  strcpy(uinfo.tty, tty_name);
#endif

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
  /* ªì©l¤Æ uinfo¡Bflag¡Bmode */
  /* ------------------------ */

  setup_utmp(LOGIN);
  currmode = MODE_STARTED;
  enter_uflag = cuser.uflag;

/* get local time */
  tmp = localtime(&cuser.lastlogin);

  update_data(); //wildcat: update user data
/*Ptt check ¦P®É¤W½u¤H¼Æ */
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
    strcpy(currboard, "©|¥¼¿ï©w");
  }
  else
#endif

  {
    brc_initial(DEFAULT_BOARD);
    set_board();
  }

  /* ------------ */
  /* µe­±³B²z¶}©l */
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
    chkload(32);
    currutmp->birth = 0 ;
  }
  pressanykey(NULL);
  
/* wildcat : ·h®a³qª¾¥Î
  if(belong(BBSHOME"/etc/oldip",fromhost))
  {
    more(BBSHOME"/etc/removal");
    abort_bbs();
  }    
*/

  more("log/day",NA);
  
  if (cuser.userlevel)          /* not guest */
  {
  
    if (!(HAS_PERM(PERM_SYSOP) && HAS_PERM(PERM_DENYPOST)))
      do_aloha("<<¤W¯¸³qª¾>> -- §Ú¨Ó°Õ¡I");  
  
    move(t_lines - 2, 0);
    prints("¡qÅwªï±z²Ä [1;33m%d[0;37m «×«ô³X¥»¯¸¡r", ++cuser.numlogins );
    
//    prints("¡qÅwªï±z²Ä [1;33m%d[0;37m «×«ô³X¥»¯¸¡A\¤W¦¸±z¬O±q [1;33m%s[0;37m ³s©¹¥»¯¸¡A\n\§Ú°O±o¨º¤Ñ¬O [1;33m%s[0;37m¡C\n",
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
      getdata(b_lines - 1, 0, "±z­n§R°£¥H¤W¿ù»~¹Á¸Õªº°O¿ý¶Ü(Y/N)?[Y]",
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
/* ªO¥D´£°ª«H½c¤W­­ */
  if(HAS_PERM(PERM_BM) && cuser.exmailbox < 100)
    cuser.exmailbox = 100;
  else if (!strcmp(cuser.userid, STR_GUEST))
  {
    char *nick[10] = {"ÄÑ¥]", "¬ü¤k¹Ï", "FreeBSD", "DBT¥Ø¿ý", "mp3",
                      "¤k¤ýÀY", "¯f¬r", "µ£¦~", "¥Û¹³", "386 CPU"
                     };
    char *name[10] = {"¥d±þ¦Ì¨È", "¥Õ³¾¿AÄR¤l", "­·»P¹Ð®J¤p¯¸", "¨gx©Î«Ox±¶", 
                      "Wu Bai & China Blue","¥ì¤O¨F¥Õ", "C-brain", 
                      "®É¶¡", "§Ú", "Intel inside" 
                      };
    char *addr[10] = {"ªkÄõµ·", "www.***.or.jp", "M$", "¦a¤U¥úºÐ¤u¼t", 
                      "¦U¤j°Û¤ù¦æ","³¥¬h", "5.25¦TºÏºÐ¤ù", "seiko¤â¿ö", 
                      "¦a¤U°g®c15¼h", "¸ê·½¦^¦¬±í"};
    int sex[10] = {6, 4, 7, 7, 2, 6, 0, 7, 7, 0};

    int i = rand() % 10;
    sprintf(cuser.username, "³Q­·¤Æªº%s", nick[i]);
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
//     HELP();
     pressanykey("­ò?! ·s¨Óªº­ò? ¥[ªo!");
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
   char genbuf[200];

   setuserfile(genbuf, FN_ALOHA);
   if ((fd = open(genbuf, O_RDONLY)) > 0)
   {
      sprintf(genbuf + 1, hello);
      *genbuf = 1;
      while (read(fd, &pal, sizeof(pal)) == sizeof(pal)) {
         user_info *uentp;
         extern cmpuids();
         int tuid;

         if ( (tuid = searchuser(pal.userid))  && tuid != usernum &&
             (uentp = (user_info *) search_ulistn(cmpuids, tuid, 1)) &&
             ((uentp->userlevel & PERM_SYSOP) || ((!currutmp->invisible || 
           uentp->userlevel & PERM_SEECLOAK) && !(is_rejected(uentp) & 1))))    
            my_write(uentp->pid, genbuf);
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
    sysop_bbcall("¹F¨ì¥»¯¸³Ì¤j¤W½u¤H¼Æ %d ¤H",count_ulist()); 
    fprintf(fp, "%d", count_ulist());
    fclose(fp);
  }
  if(fp = fopen(".maxtoday", "r"))
  {
    fscanf(fp, "%d", &maxonline);
    if (count_ulist() > maxonline){
      fclose(fp);
      sysop_bbcall("¹F¨ì¥»¤é³Ì¤j¤W½u¤H¼Æ %d ¤H",count_ulist()); 
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

/*
  Argc = argc;
  Argv = argv;
*/

  if (argc > 1)
  {
    strcpy(fromhost, argv[1]);

#ifdef SHOW_IDLE_TIME
    if (argc > 2)
      strcpy(tty_name, argv[2]);

#endif

    if (argc > 3)
       strcpy(remoteusername, argv[3]);
  }
/*
woju
*/
{
   char cmd[80] = "?@";

   if (!getenv("RFC931"))
      setenv("RFC931", strcat(cmd, fromhost), 1);
}

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
  
/* wildcat : ­×´_¤¤
  if(!HAVE_PERM(PERM_SYSOP))
  {
    pressanykey("¾ã­×¤¤ , ¸T¤î¯¸ªø¥H¥~ user µn¤J");
    abort_bbs();
  }
*/
  check_max_online();
  
  sethomedir(currmaildir, cuser.userid);
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

  m_postnotify(); 
  
  force_board("Announce");

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
    sprintf(fbuf, "¬G¶m [%s]¡G", currutmp->from);
    if(getdata(b_lines, 0, fbuf, currutmp->from, 17, DOECHO,0))
      currutmp->from_alias=0;
  }

  if(HAS_HABIT(HABIT_FEELING))
  {
    getdata(b_lines ,0,"¤µ¤Ñªº¤ß±¡¦p¦ó©O¡H", cuser.feeling, 5 ,DOECHO,cuser.feeling);
    cuser.feeling[4] = '\0';
    strcpy(currutmp->feeling, cuser.feeling);
    substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  }

  if(HAS_HABIT(HABIT_NOTEMONEY))
    DL_func("SO/mn.so:show_mn");
    
  if(cuser.exp >= rpguser.level*rpguser.level*10000 && rpguser.race)
  {
    pressanykey("§A¤w¸g¥i¥H¤É¯ÅÅo"); 
    DL_func("SO/rpg.so:rpg_guild");
  }
  if (HAS_PERM(PERM_ACCOUNTS) && dashf(fn_register))
    m_register();             
    
  domenu(MMENU, "¥D¥\\¯àªí", (chkmail(0) ? 'M' : 'B'), cmdlist);
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
    prints("§Aªº»È¹ô¤W­­¬° %ld¡I\n\n\n\n",MAXMONEY);
    outs("½Ð·Q¿ìªkªá±¼¤@¨Ç , ©Î¬O§â¿úÂà¦¨ª÷¹ô§a!\n¦b°Ó·~¤¤¤ßªº»È¦æ¤¤¦³´«¿úªº¿ï¶µ .");    
    pressanykey("§A¿ú¤Ó¦hÅo¡I·Q¿ìªkªá±¼§a¡I");
    finance();
    game_list();
  }
}
#endif
