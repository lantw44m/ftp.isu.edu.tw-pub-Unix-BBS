/*-------------------------------------------------------*/
/* main.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* author : opus.bbs@bbs.cs.nthu.edu.tw                  */
/* target : BBS main/login/top-menu routines             */
/* create : 95/03/29                                     */
/* update : 03/05/06 (Dopin)                             */
/*-------------------------------------------------------*/

#define _MAIN_C_

#include "bbs.h"

#ifdef USE_FREE_PORT
#include "bbsd.h"
#endif

jmp_buf byebye;
static uschar enter_uflag;
static int port_number;

extern void init_station_info(void);
extern void item_update_passwd(char);
extern void force_lists(void);

extern int station_num;
extern char station_limit[];
extern char station_list[][13];
extern char station_list_cn[][15];

#ifdef SHOW_IDLE_TIME
char fromhost[STRLEN - 20] = "\0";
char tty_name[20] = "\0";
#else
char fromhost[STRLEN] = "\0";
#endif

char remoteusername[40];
int mbbsd = 0;
extern int now_num;

void check_register();
int Argc;
char** Argv;

/* ----------------------------------------------------- */
/* Â÷¶} BBS µ{¦¡                                         */
/* ----------------------------------------------------- */

void
log_usies(mode, mesg)
  char *mode, *mesg;
{
  FILE *fp;
  char genbuf[200];

  if (fp = fopen(FN_USIES, "a+"))
  {
    time_t now = time(0);

    if (!mesg)
    {
      sprintf(genbuf, "Stay: %d (%s)",
        (now - login_start_time) / 60, cuser.username);
      mesg = genbuf;
    }

    fprintf(fp, cuser.userid[0] ? "%s %s %-12s %s\n" : "%s %s %s%s\n",
      Cdate(&now), mode, cuser.userid, mesg);
    fclose(fp);
  }
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

int birthday(void) {
   struct tm *ptime;
   time_t now;
   time(&now);

   ptime = localtime(&now);

   if(ptime->tm_mon + 1 == cuser.month && ptime->tm_mday == cuser.day)
      return 1;
   else
      return 0;
}

void
u_exit(mode)
  char *mode;
{
  extern void auto_backup();    /* ½s¿è¾¹¦Û°Ê³Æ¥÷ */
  userec xuser;
  int diff = (time(0) - login_start_time) / 60; /* Leeym ¤W¯¸°±¯d®É¶¡­­¨î¦¡ */
  char genbuf[80], fpath[80];

  if (currmode & MODE_SELECT){
     sprintf(fpath, "SR.%s", cuser.userid);
     if(currstat==RMAIL)
     {
     setuserfile(genbuf, fpath);
     unlink(genbuf);
     }
     else{
     setbfile(genbuf, currboard, fpath);
     unlink(genbuf);
     }
  }

  get_record(fn_passwd, &xuser, sizeof(xuser), usernum);

  auto_backup();

  setflags(PAGER_FLAG, currutmp->pager != 1);
  if (HAS_PERM(PERM_LOGINCLOAK))
    setflags(CLOAK_FLAG, currutmp->invisible);
  purge_utmp(currutmp);

  if ((cuser.uflag != enter_uflag) || (currmode & MODE_DIRTY) || !diff) {
    xuser.uflag = cuser.uflag;
    xuser.numposts = cuser.numposts;
    if(!diff && cuser.numlogins)
       xuser.numlogins = --cuser.numlogins; /* Leeym ¤W¯¸°±¯d®É¶¡­­¨î¦¡ */
    substitute_record(fn_passwd, &xuser, sizeof(userec), usernum);
  }
  log_usies(mode, NULL);
}


void
system_abort()
{
  if (currmode)
    u_exit("ABORT");

  clear();
  refresh();
  printf("ÁÂÁÂ¥úÁ{, °O±o±`¨Ó³á !\n");
  sleep(1);
  exit(0);
}


void
abort_bbs()
{
/*
  kill(xyzpid, SIGHUP);
*/
  kill(0, SIGHUP);
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
    memcpy(&cuser, &xuser, sizeof(cuser));
  else
    memset(&cuser, 0, sizeof(cuser));
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
  bell();

  /* DOPIN REMARK */ /*
  if (currutmp->msgcount) {
     char buf[200];
     time_t now = time(0);

     sprintf(buf, "[33;41m¡¹%s[34;47m [%s] %s [0m",
        (currutmp->destuip)->userid,  my_ctime(&now),
        (currutmp->sig == 2) ? "­«­n®ø®§¼s¼½¡I(½ÐCtrl-U,l¬d¬Ý¼ö°T°O¿ý)" : "©I¥s¡B©I¥s¡AÅ¥¨ì½Ð¦^µª");
     move(0, 0);
     clrtoeol();
     outs(buf);
     refresh();
  }
  else */
  scr_exec(talkreply);
}

show_last_call_in() {
   char buf[200];

   sprintf(buf, "[1;33;46m¡¹%s[37;45m %s [0m",
      currutmp->msgs.last_userid,
      currutmp->msgs.last_call_in);
   my_outmsg(buf);
}

static void write_request() {
  FILE *fp;
  char fpath[80];
  struct tm *ptime;
  time_t now;


#ifdef DOTIMEOUT
/* Half-hour remind */
  if(*currmsg) {
    my_outmsg(currmsg);
    if(*currmsg != 0x0a) bell();
    *currmsg = 0;
    return;
  }
#endif

  time(&now);
  ptime = localtime(&now);

#ifdef  LINUX
  signal(SIGUSR2, write_request);
#endif

    if(currutmp->pager && cuser.userlevel && currutmp->mode != TALK &&
      currutmp->mode != CHATING && currutmp->mode != PAGE &&
      currutmp->mode != IDLE && currutmp->mode != MAILALL &&
      currutmp->mode != MONITOR) {
      char c0 = currutmp->chatid[0];
      int currstat0 = currstat;
      uschar mode0 = currutmp->mode;

      currutmp->mode = 0;
      currutmp->chatid[0] = 2;
      currstat = XMODE;

      bell();
      show_last_call_in();
      if(igetch() == Ctrl('R')) DL_func("bin/message.so:get_msg", 0);

      currutmp->chatid[0] = c0;
      currutmp->mode = mode0;
      currstat = currstat0;
    }
    else {
       bell();
       show_last_call_in();
    }
}

static void multi_user_check() {
  register user_info *ui;
  register pid_t pid;
  int cmpuids();
  char genbuf[3];

  if (HAS_PERM(PERM_SYSOP))
    return;                     /* don't check sysops */

  if (cuser.userlevel)
  {
    if (!(ui = (user_info *) search_ulist(cmpuids, usernum)))
      return;                   /* user isn't logged in */

    pid = ui->pid;
    if (!pid || (kill(pid, 0) == -1))
      return;                   /* stale entry in utmp file */

    getdata(0, 0, "±z·Q§R°£¨ä¥L­«½Æªº login (Y/N)¶Ü¡H[Y] ", genbuf, 3, LCECHO, 0);

    if (genbuf[0] != 'n')
    {
      kill(pid, SIGHUP);
      log_usies("KICK ", cuser.username);
    }
    else
    {
      if (count_multi() >= 2)
        system_abort();         /* Goodbye(); */
    }
  }
  else
  {
    /* allow multiple guest user */
    if (count_multi() > 32)
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
  int fd, len;
  char genbuf[200];

  sprintf(genbuf, "%c%-12s[%s] %s\n", type, uid,
    Cdate(&login_start_time), getenv("RFC931"));
  len = strlen(genbuf);
  if ((fd = open(str_badlogin, O_WRONLY | O_CREAT | O_APPEND, 0664)) > 0)
  {
    write(fd, genbuf, len);
    close(fd);
  }

  if (type == '-')
  {
    sprintf(genbuf, "[%s] %s\n", Cdate(&login_start_time), fromhost);
    len = strlen(genbuf);
    sethomefile(fname, uid, str_badlogin);
    if ((fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0664)) > 0)
    {
      write(fd, genbuf, len);
      close(fd);
    }
  }
}

show_hint_message()
{
        struct timeval  timep;
        struct timezone timezp;
        int     i, j, msgNum;
        FILE    *hintp;
        char    msg[200];

        if (!(hintp = fopen("etc/hint", "r")))
          return 0;
        fgets(msg, 200, hintp);
        msgNum = atoi(msg);
        gettimeofday(&timep, &timezp);
        i = (int) timep.tv_sec%(msgNum + 1); /* ³Ì·sªº¤@½g¾÷·|¥[­¿ */
        if (i == msgNum)
          i--;
        j = 0;

        while (j < i)
        {
          fgets(msg, 200, hintp);
          msg[1] = '\0';
          if (!strcmp(msg,"#"))
            j++;
        }

        fgets(msg, 200, hintp);
        prints("[1;34m®ü­·§j¨Ó¬Kªº«H®§¡G [1;37m±z¥i´¿ª¾¹D¡H[40;0m\n");
        prints("                   %s[0m", msg);
        fgets(msg, 200, hintp);
        prints("                   %s[0m", msg);
        fclose(hintp);
}

bad_user(char* name)
{
   FILE* list;
   char buf[40];

  if (list = fopen("etc/bad_user", "r")) {
     while (fgets(buf, 40, list)) {
        buf[strlen(buf) - 1] = '\0';
        if (!strcmp(buf, name))
           return 1;
     }
     fclose(list);
  }
  return 0;
}

static void login_query(unsigned local_port) {
  char uid[IDLEN + 1], passbuf[PASSLEN];
  int attempts, pts;
  char genbuf[200], ports[8];
  extern struct UTMPFILE *utmpshm;
  FILE *fpd;
  char ch = ' ';
  char ci;

  fpd = fopen(BBSHOME"/stfiles/station_number", "r");
  if(!fpd) station_num = 1;
  else {
     int k;

     fgets(genbuf, 10, fpd);
     k = atoi(genbuf);
     if(k < 1 || k > 19) k = 1;
     station_num = k;

     fclose(fpd);
  }

  fpd = fopen(BBSHOME"/stfiles/station_attrib", "rb");
  if(fpd) {
     char atb[19];

     fread(&atb, sizeof(char), 19, fpd);
     for(ci = 0 ; ci < station_num ; ci++)
         station_limit[ci] = atb[ci];

         fclose(fpd);
  }

  resolve_utmp();

  if(local_port) port_number = local_port;
  else port_number = 23;

  attempts = utmpshm->number;
  init_station_info();

#ifndef USE_FREE_PORT
  if(port_number - ZBBSDP >= 0 && port_number - ZBBSDP < station_num)
     port_number -= ZBBSDP;
  else
     port_number = 0;
#else
  pts = port_2_station('Z', port_number);
  if(pts != -1) port_number = pts;
  else port_number = 0;

  if(port_number < 0 || port_number >= station_num)
     port_number = 0;
#endif

#ifdef MultiIssue
  if(check_multi_index('I', port_number, genbuf)) goto Skip_show_ats;
#else
  sprintf(genbuf, BBSHOME"/etc/issue.%s", station_list[port_number]);
#endif

  if((fpd = fopen(genbuf, "r")) == NULL) goto Skip_show_ats;
  else while(ch != EOF) prints("%c", (ch = fgetc(fpd)));
  fclose(fpd);

Skip_show_ats:
  prints("\nÅwªï¥úÁ{¡i[1;37;45m %s [0m¡j(¥Ø«eÁ`¦@¦³ %d ¤H¤W½u)",
  station_list_cn[port_number], attempts);

  if (attempts >= MAXACTIVE)
  {
    outs("¥Ñ©ó¤H¼Æ¤Ó¦h¡A½Ð±zµy«á¦A¨Ó¡C\n");
    oflush();
    sleep(1);
    exit(1);
  }

  /* hint */
  /* show_hint_message(); */

  attempts = 0;
  while (1)
  {
    if (attempts++ >= LOGINATTEMPTS)
    {
      more("etc/goodbye", NA);
      oflush();
      sleep(1);
      exit(1);
    }

    getdata(0, 0, "\n½Ð¿é¤J¥N¸¹¡A©Î¥H[guest]°ÑÆ[¡A¥H[new]µù¥U¡G",
      uid, IDLEN + 1, DOECHO, 0);

    strcpy(genbuf, uid);
    for(ci = 0 ; ci < IDLEN+1 ; ci++)
       if(genbuf[ci] >= 'A' && genbuf[ci] <= 'Z')
          genbuf[ci] += 'a' - 'A';

    if(!strcmp(genbuf, KEEPWD1) || !strcmp(genbuf, KEEPWD2)) {
       outs("¦¹±b¸¹¬°¨È¯¸ªº«O¯d¦r, ½Ð¥Î¨ä¥L¥N¸¹¶i¤J\n");
       continue;
    }

#ifndef SYSOP_CAN_LOGIN
    if(!strcmp(genbuf, "sysop")) {
       outs("¨È¯¸°±¥Î SYSOP ±b¸¹, ½Ð¥Î¨ä¥L¥N¸¹¶i¤J\n");
       continue;
    }
#endif

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
    else if (uid[0] == '\0' || !dosearchuser(uid))
    {
      outs(err_uid);
    }
    else if(strcmp(uid, "guest") && !belong("etc/anonymous", uid)) {
      getdata(0, 0, MSG_PASSWD, passbuf, PASSLEN, NOECHO, 0);
      passbuf[8] = '\0';

      if(!checkpasswd(cuser.passwd, passbuf)) {
        logattempt(cuser.userid, '-');
        outs(ERR_PASSWD);
      }
      else {
        /* SYSOP gets all permission bits */
        if(!ci_strcmp(cuser.userid, str_sysop)) cuser.userlevel = ~0;
        if(0 && HAS_PERM(PERM_SYSOP) && !strncmp(getenv("RFC931"), "?@", 2)) {
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
    else {
      /* guest */
      cuser.userlevel = 0;
      cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
      break;
    }
  }

  multi_user_check();

  if(!term_init("vt100")) {
    do {
      getdata(0, 0, "\n²×ºÝ¾÷«¬ºA¿ù»~¡I½Ð¿é¤J¡G", genbuf, 8, DOECHO, "vt100");
    } while (!term_init(genbuf));
  }

  sethomepath(genbuf, cuser.userid);
  mkdir(genbuf, 0755);
}

/* woju */
add_distinct(char* fname, char* line) {
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

void setup_utmp(int mode) {
  user_info uinfo;
  char buf[80];
  FILE *fp;

  memset(&uinfo, 0, sizeof(uinfo));
  uinfo.pid = currpid = getpid();
  uinfo.uid = usernum;
  uinfo.mode = currstat = mode;
  /* DOPIN REMARK */ /*
  uinfo.msgcount = 0; */
  uinfo.userlevel = cuser.userlevel;
  uinfo.uptime = time(0);

#ifndef NO_USE_MULTI_STATION
  uinfo.now_stno = cuser.now_stno;
#endif

  strcpy(uinfo.userid, cuser.userid);
  strcpy(uinfo.realname, cuser.realname);
  strcpy(uinfo.username, cuser.username);
  strncpy(uinfo.from, fromhost, 28);

  setuserfile(buf, "remoteuser");

  add_distinct(buf, getenv("RFC931"));

#ifdef SHOW_IDLE_TIME
  strcpy(uinfo.tty, tty_name);
#endif

  uinfo.welcomeflag = cuser.welcomeflag;

  if (enter_uflag & CLOAK_FLAG)
  {
      uinfo.invisible = YEA;
    if (HAS_PERM(PERM_LOGINCLOAK))
      uinfo.invisible = YEA;
    else
      cuser.uflag ^= CLOAK_FLAG;
  }
  uinfo.pager = !(enter_uflag & PAGER_FLAG);

  getnewutmpent(&uinfo);
  friend_load();
}

#ifndef NO_USE_MULTI_STATION
extern uschar enter_list[MAX_STATIONS];
extern uschar enter_number;
#endif

static void user_login() {
  char ans[4];
  char genbuf[200];
  int i, k;

  log_usies("ENTER", getenv("RFC931"));

  /* ------------------------ */
  /* ªì©l¤Æ uinfo¡Bflag¡Bmode */
  /* ------------------------ */

  enter_uflag = cuser.uflag;

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

#ifndef NO_USE_MULTI_STATION
  cuser.now_stno = port_number;
  strcpy(cuser.station, station_list[port_number]);
#endif
  setup_utmp(LOGIN);
  currmode = MODE_STARTED;

#ifdef LEAST_PERM_DEFAULT
  if(HAS_PERM(PERM_LOGINOK)) cuser.userlevel |= PERM_OKADD;
#endif

  /* ------------ */
  /* µe­±³B²z¶}©l */
  /* ------------ */

  initscr();

#ifndef NO_USE_MULTI_STATION
  if(is_station_sysop()) goto PASS_NONM;

  if(station_limit[port_number]) {
    if(cuser.station_member[port_number])
       goto PASS_NONM;
  }
  else if(!cuser.station_member[port_number])
          goto PASS_NONM;

  clear();
  move(5, 20);
  prints("[1;37;41m±z¨Ã¤£¬O¥»¯¸¦¨­û,½Ð¥Ñ¨ä¥L¯¸¥x¶i¤J...[m");
  pressanykey(NULL);
  reset_tty();
  exit(1);
#endif

PASS_NONM:
#ifndef NO_USE_MULTI_STATION
  for(k = 0, i = 0 ; k < station_num && k < MAX_STATIONS ; k++) {
#ifdef SEE_ALL_STATION
     enter_list[i] = k;
     i++;
#else
     sprintf(genbuf, BBSHOME "/run/%s.sysop_name", station_list[k]);

     if(!station_limit[k])
       if(cuser.station_member[k] && !(HAS_PERM(PERM_SYSOP) ||
          belong(genbuf, cuser.userid) && HAS_PERM(PERM_BOARD)))
          continue;

     if(!station_limit[k] || HAS_PERM(PERM_SYSOP) || cuser.station_member[k]
        || belong(genbuf, cuser.userid)) {
        enter_list[i] = k;
        i++;
     }
#endif
  }
  enter_number = i;
#endif

#ifdef COUNT_MAXLOGINS
  check_max_online();  /* Dopin: °O¿ý³Ì°ª¤W½u¤H¦¸ by Leeym */
#endif

  if(HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_SYSOP)) t_aloha('I');

  /* Dopin 01/10/02 */
#ifndef NO_USE_MULTI_STATION
  sprintf(genbuf, "etc/welcome.%s", cuser.station);
#else
  sprintf(genbuf, "etc/welcome.%s", DEFAULTST);
#endif
  more(genbuf, NA);
  /* Dopin */
  cuser.extra_mode[1] = 0;

  if(cuser.userlevel) {         /* not guest */
    char buf[80], *ptr;

#ifdef GOOD_BABY
   DL_func("bin/goodbaby.so:check_reject_me");
#endif

#ifdef CHECK_LIST_ID_SELF
    check_listname();
#endif

    move(b_lines - 2, 0);
    prints("  [0;37mÅwªï±z²Ä [1;33m%d[0;37m «×«ô³X¥»¯¸¡A\
¤W¦¸±z±q [1;33m%-.40s[0;37m ³s©¹¥»¯¸\n\
  ·í®Éªº®É¶¡¬° [1;33m%s[0;37m¡C\n",
      ++cuser.numlogins, cuser.lasthost,
      Cdate(&cuser.lastlogin));

#ifdef OSONG_NUM_LIMIT
    /* Dopin: ¦pªG¹j¤é¤W¯¸ ²MªÅÂIºq¼Æ (idea by itoc) */
    if(login_start_time / 86400 != cuser.lastlogin / 86400)
       cuser.backup_char[1] = 0;
#endif

#ifdef NO_USE_MULTI_STATION
    out2line(1, b_lines, "");
    getdata(b_lines, 2, "¤µ¤é¤ß±¡ : ", currutmp->kimochi, IDLEN+1, DOECHO, 0);
#else
    pressanykey(NULL);
#endif

    setuserfile(genbuf, str_badlogin);
    if (more(genbuf, NA) != -1)
    {
      getdata(b_lines - 1, 0, "±z­n§R°£¥H¤W¿ù»~¹Á¸Õªº°O¿ý¶Ü(Y/N)?[Y]",
        ans, 3, LCECHO, 0);
      if (*ans != 'n')
        unlink(genbuf);
    }
    check_register();
#ifndef REC_ALL_FROM
    strncpy(cuser.lasthost, fromhost, 80);
    cuser.lasthost[79] = '\0';
#endif

    restore_backup();
  }
  else if (!strcmp(cuser.userid, STR_GUEST))
  {
    char *nick[13] = {"·¦¤l", "¨©´ß", "¤º¦ç", "Ä_¯S²~", "Â½¨®³½",
                      "¾ð¸­", "¯BµÓ", "¾c¤l", "¼ç¤ô¸¥", "Å]¤ý",
                      "ÅKÅø", "¦Ò¨÷", "¤j¬ü¤k"};
    char *name[13] = {"¤j¤ý·¦¤l", "ÆxÄMÁ³", "¤ñ°ò¥§", "¥i¤f¥i¼Ö", "¥õªaªº³½",
                      "¾Ð", "°ª©£«Î", "AIR Jordon", "¬õ¦â¤Q¤ë¸¹", "¸¨µµ²G",
                      "SASAYA·¦¥¤", "Àn³J", "¥¬¾|§J÷«³½­»µ·"};
    char *addr[13] = {"¶§¥ú¨FÅy", "¤j®ü", "¶§¥ú¤ÑÅéÀç", "¬ü°ê", "ºñ¦â¬À·äÁG",
                      "»·¤è", "­ì¥»®ü", "NIKE", "Ä¬Áp", "¨k¤C420«Ç",
                      "·R¤§¨ý", "¹q¾÷B Àd¤l¶h", "ÂÅ¦â¬À·äÁG"};
    int sex[13] = {6, 7, 7, 7, 2, 6, 0, 7, 7, 0, 7, 6, 1};

    int i = rand() % 13;
    sprintf(cuser.username, "®üÃäº}¨Óªº%s", nick[i]);
    sprintf(currutmp->username, cuser.username);
    sprintf(cuser.realname, name[i]);
    sprintf(currutmp->realname, cuser.realname);
    sprintf(cuser.address, addr[i]);
    cuser.sex = sex[i];
    pressanykey(NULL);
  }
  else
     pressanykey(NULL);

#ifdef REC_ALL_FROM
  strncpy(cuser.lasthost, fromhost, 80);
  cuser.lasthost[79] = '\0';
#endif

  if(bad_user(cuser.userid)) {
     sprintf(currutmp->username, "BAD_USER");
     cuser.userlevel = 0;
     cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
  }

  m_init();
  while(chkmailbox()) m_read();

  if (!PERM_HIDE(currutmp))
     cuser.lastlogin = login_start_time;
  substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);

  force_lists();

  /* Dopin  §ï¦¨¤W½u§P©w ¤£¥Î */
  /*
  if(xuser.welcomeflag & 1)
     more("etc/welcome.2", YEA);
  */

  if(xuser.welcomeflag & 2)
     more("etc/birth.today", YEA);

  if(xuser.welcomeflag & 4)
     more(fn_note_ans, 2);

  if(birthday()) {
     clear();
     more("etc/birthday", YEA);
  }
}

int main(int argc, char *argv[]) {
  int getin;
  char buf[200];
  char* rfc931;

  extern struct commands cmdlist[];

  /* ----------- */
  /* system init */
  /* ----------- */

  setgid(BBSGID);
  setuid(BBSUID);
  chdir(BBSHOME);
  currmode = 0;
  srand(login_start_time = time(0));
  Argc = argc;
  Argv = argv;

  setenv("REMOTEHOST", strcpy(fromhost, (argc > 1) ? argv[1] : "localhost"), 1);
#ifdef IDLE_TIMEOUT
  strcpy(tty_name, (argc > 2) ? argv[2] : ttyname(0));
#endif
  setenv("REMOTEUSERNAME", strcpy(remoteusername, (argc > 3) ? argv[3] : "?"), 1);

  if (!(rfc931 = getenv("RFC931"))) {
     sprintf(buf, "%s@%s", remoteusername, fromhost);
     setenv("RFC931", buf, 1);
  }
  else {
     strcpy(buf, rfc931);
     setenv("REMOTEUSERNAME", strcpy(remoteusername, strtok(buf, "@")), 1);
     setenv("REMOTEHOST", strcpy(fromhost, strtok(0, " \n\r")), 1);
  }
#ifdef IDLE_TIMEOUT
  sprintf(buf, "if test -x %s/bin/banner; then %s/bin/banner %s; fi", BBSHOME, BBSHOME, (*tty_name) ? tty_name + 5 : 0);
#endif
  system(buf);

  atexit(leave_bbs);

  signal(SIGHUP, abort_bbs);
  signal(SIGBUS, abort_bbs);
  signal(SIGSEGV, abort_bbs);
  signal(SIGSYS, abort_bbs);

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

  if(setjmp(byebye)) abort_bbs();
  init_tty();
  login_query(argc >= 5 ? (unsigned)atoi(argv[4]) : 0);
  user_login();

  if(HAVE_PERM(PERM_SYSOP | PERM_BM)) b_closepolls();
  if(!(cuser.uflag & COLOR_FLAG)) showansi = 0;

#ifdef DOTIMEOUT
  init_alarm();
#else
  signal(SIGALRM, SIG_IGN);
#endif

  if((HAS_PERM(PERM_SYSOP)
#ifdef USE_ACCOUNTS_PERM
     || HAS_PERM(PERM_ACCOUNTS)
#endif
  ) && belong("etc/sysop", cuser.userid) && dashf(fn_register))
    DL_func("bin/register.so:m_register");

#ifdef Expire_ID
  DL_func("bin/check_expire_time.so:check_expire_time");
#endif

  if(!is_station_sysop()) cuser.userlevel &= ~PERM_CHATROOM;
  else cuser.userlevel |= PERM_CHATROOM;

  cuser.extra_mode[5] = cuser.extra_mode[4] = cuser.extra_mode[3] =
  cuser.extra_mode[2] = 0;

#ifdef CHECK_POST_TIME
  cuser.last_post_time = cuser.post_number = 0;
  cuser.count_time = cuser.lastlogin;
#endif

#ifdef IDLE_TIMEOUT
  strcpy(cuser.tty_name, &tty_name[5]);
#endif

#ifndef NO_USE_MULTI_STATION
  access_mask('l', &cuser.userlevel, cuser.userid);
#endif

#ifdef NOC_DFB
  if(!(cuser.welcomeflag & 8192))
#endif
    force_board(DEFAULTBOARD);

#ifndef NO_USE_MULTI_STATION
  sprintf(buf, "Announce.%s", cuser.station);
  sprintf(&buf[100], "boards/%s", buf);
  if(dashd(&buf[100])) force_board(buf); /* Dopin: Á×§K¨S¶}ªO¨t²Î¥X°ÝÃD */
#endif

#ifdef USE_NEWCOMER
  if(dashd("boards/" COMERDIR))
    if(cuser.userlevel && !(cuser.backup_char[2] & 1))
       DL_func("bin/newcomers.so:newcomers");
#endif

  domenu(MMENU, "¥D¥\\¯àªí", (chkmail(0) ? 'M' : 'C'), cmdlist, 1);

  return 0;
}
