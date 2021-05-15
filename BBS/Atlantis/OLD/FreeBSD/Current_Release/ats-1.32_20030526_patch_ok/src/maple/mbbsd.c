/*-------------------------------------------------------*/
/* mbbsd.c      ( NTHU CS MapleBBS Ver 3.00 )            */
/*-------------------------------------------------------*/
/* target : BBS daemon/main/login/top-menu routines      */
/* create : 95/03/29                                     */
/* update : 02/09/10 (Dopin)                             */
/*-------------------------------------------------------*/

#define _MAIN_C_

#include <varargs.h>
#include "bbs.h"
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/telnet.h>
#include <syslog.h>

#ifdef USE_FREE_PORT
#include "bbsd.h"
#endif

#define SOCKET_QLEN     4
#define TH_LOW          100
#define TH_HIGH         120
#define PID_FILE        BBSHOME "/run/mbbsd.pid"

#ifdef  HAVE_CHKLOAD
#define BANNER  "\r\n¡i¨È¯SÄõ´£´µ¡j[%s]\r\n\n"
#else
#define BANNER  "\r\n¶i¤J¨È¯SÄõ´£´µªº¥@¬É\r\n\n"
#endif

jmp_buf byebye;

char remoteusername[40] = "?";
char fromhost[STRLEN] = "\0";
char tty_name[20] = "\0";
char genbuf[1024];
extern struct UTMPFILE *utmpshm;
static uschar enter_uflag;
static char str_badlogin[] = "logins.bad";

int Argc;
char **Argv;
int mbbsd = 1;

static uschar enter_uflag;
static int port_number;

extern void init_station_info(void);
extern void item_update_passwd(char);
extern void force_lists(void);

extern int station_num;
extern char station_limit[];
extern char station_list[][13];
extern char station_list_cn[][15];

/* ----------------------------------------------------- */
/* operation log                                         */
/* ----------------------------------------------------- */

void file_append(char *fpath, char *msg) {
  int fd;

  if ((fd = open(fpath, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
  {
    write(fd, msg, strlen(msg));
    close(fd);
  }
}

void log_usies(char *mode, char *msg) {
  char buf[512], data[256];
  time_t now = time(0);

  if(!msg) {
    msg = data;
    sprintf(msg, "Stay: %d (%s@%s)",
      (now - login_start_time) / 60, remoteusername, fromhost);
  }

  sprintf(buf, "%s %s %-13s%s\n", Cdate(&now), mode, cuser.userid, msg);
  file_append(FN_USIES, buf);
}

int bad_user(char* name) {
  FILE* list;
  char buf[40];

  if(list = fopen("etc/bad_user", "r")) {
    while (fgets(buf, 40, list)) {
      buf[strlen(buf) - 1] = '\0';
      if(!strcmp(buf, name))
         return 1;
    }
    fclose(list);
  }
  return 0;
}

void setup_utmp(int mode) {
  user_info uinfo;
  char buf[80];
  FILE *fp;

  memset(&uinfo, 0, sizeof(uinfo));
  uinfo.pid = currpid = getpid();
  uinfo.uid = usernum;
  uinfo.mode = currstat = mode;
//  uinfo.msgcount = 0;
  uinfo.userlevel = cuser.userlevel;
  uinfo.uptime = time(0);

#ifndef NO_USE_MULTI_STATION
  uinfo.now_stno = cuser.now_stno;
#endif

  strcpy(uinfo.userid, cuser.userid);
  strcpy(uinfo.realname, cuser.realname);
  strcpy(uinfo.username, cuser.username);
  strncpy(uinfo.from, fromhost, 28);
  uinfo.welcomeflag = cuser.welcomeflag;

  setuserfile(buf, "remoteuser");
  add_distinct(buf, getenv("RFC931"));

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


void u_exit(char *mode) {
  extern void auto_backup();    /* ½s¿è¾¹¦Û°Ê³Æ¥÷ */
  userec xuser;

  get_record(fn_passwd, &xuser, sizeof(xuser), usernum);
  auto_backup();

  setflags(PAGER_FLAG, currutmp->pager != 1);
  if (HAS_PERM(PERM_LOGINCLOAK))
    setflags(CLOAK_FLAG, currutmp->invisible);
  purge_utmp(currutmp);

  if ((cuser.uflag != enter_uflag) || (currmode & MODE_DIRTY)) {
    xuser.uflag = cuser.uflag;
    xuser.numposts = cuser.numposts;
    substitute_record(fn_passwd, &xuser, sizeof(userec), usernum);
  }
  log_usies(mode, NULL);
}

setflags(int mask, int value) {
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

void login_query(unsigned local_port) {
  char uid[IDLEN + 1], passbuf[PASSLEN];
  int attempts, pts;
  char genbuf[200], ports[8];
  extern struct UTMPFILE *utmpshm;
  FILE *fpd;
  char ch = ' ';
  char ci;
  extern dumb_term;

  dumb_term = YEA;

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

  attempts = utmpshm->number;
  init_station_info();

  port_number = (int)local_port;

#ifndef USE_FREE_PORT
   if(port_number - MBBSDP >= 0 && port_number - MBBSDP < station_num)
      port_number -= MBBSDP;
   else
      port_number = 0;
#else
   pts = port_2_station('M', port_number);
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

  if((fpd = fopen(genbuf, "r")) == NULL)
     goto Skip_show_ats;

  while(ch != EOF)
     prints("%c", (ch = fgetc(fpd)));
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

  /* Dopin skip hint */
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
       outs("¨È¯¸°±¥Î SYSOP ±b¸¹, ½Ð¥Î¨ä¥L¥N¸¹¶i¤J");
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
    else if(uid[0] == '\0' || !dosearchuser(uid)) {
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

  sethomepath(genbuf, cuser.userid);
  mkdir(genbuf, 0755);
  dumb_term = NA;
}

show_hint_message() {
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

multi_user_check()
{
  register user_info *ui;
  register pid_t pid;
  int cmpuids();
  char genbuf[3];

  if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_BM))
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

int dosearchuser(char *userid) {
  if(usernum = getuser(userid))
    memcpy(&cuser, &xuser, sizeof(cuser));
  else
    memset(&cuser, 0, sizeof(cuser));
  return usernum;
}

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
  if ((fd = open(str_badlogin, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
  {
    write(fd, genbuf, len);
    close(fd);
  }

  if (type == '-')
  {
    sprintf(genbuf, "[%s] %s\n", Cdate(&login_start_time), fromhost);
    len = strlen(genbuf);
    sethomefile(fname, uid, str_badlogin);
    if ((fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
    {
      write(fd, genbuf, len);
      close(fd);
    }
  }
}

#ifndef NO_USE_MULTI_STATION
extern uschar enter_list[MAX_STATIONS];
extern uschar enter_number;
#endif

user_login() {
  char ans[4];
  char genbuf[200];
  int i, k;

  log_usies("ENTER", getenv("RFC931")/* fromhost */);

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
  else
     if(!cuser.station_member[port_number])
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
      ++cuser.numlogins, cuser.lasthost, Cdate(&cuser.lastlogin));

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

  if (bad_user(cuser.userid)) {
     sprintf(currutmp->username, "BAD_USER");
     cuser.userlevel = 0;
     cuser.uflag = COLOR_FLAG | PAGER_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
  }

  m_init();
  while(chkmailbox()) m_read();

  if(!PERM_HIDE(currutmp)) cuser.lastlogin = login_start_time;
  substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);

  force_lists();

  /* Dopin */
  if(xuser.welcomeflag & 2)
     more("etc/birth.today", YEA);

  if(xuser.welcomeflag & 4)
     more(fn_note_ans, 2);

  if(birthday()) {
     clear();
     more("etc/birthday", YEA);
  }
  /* Dopin */
}

void abort_bbs()
{
  if(currmode) u_exit("AXXED");
  exit(0);
}

static void talk_request() {
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
/*
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

    if(currutmp->pager && cuser.userlevel /* && currutmp->msgcount */
          && currutmp->mode != TALK && currutmp->mode != CHATING
          && currutmp->mode != PAGE && currutmp->mode != IDLE
          && currutmp->mode != MAILALL && currutmp->mode != MONITOR) {
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

/* ----------------------------------------------------- */
/* BBS child process ¥Dµ{¦¡                              */
/* ----------------------------------------------------- */


/* start_client() ´N¬O main.c ªº main()¡A³¡¥÷¬Ù²¤ */

static void start_client(unsigned local_port) {
  char buf[200];
  FILE *fp;
  extern struct commands cmdlist[];

  currmode = 0;
  login_start_time = time(0);

  (void) signal(SIGHUP, abort_bbs);
  (void) signal(SIGBUS, abort_bbs);
  (void) signal(SIGSEGV, abort_bbs);
  (void) signal(SIGSYS, abort_bbs);
  (void) signal(SIGTERM, abort_bbs);

  (void) signal(SIGUSR1, talk_request);
  (void) signal(SIGUSR2, write_request);

  if (setjmp(byebye))
    abort_bbs();

  dup2(0, 1);
  login_query(local_port);
  user_login();

  if (HAVE_PERM(PERM_SYSOP | PERM_BM))
    b_closepolls();

  if (!(cuser.uflag & COLOR_FLAG))
    showansi = 0;

#ifdef DOTIMEOUT
  (void) init_alarm();
#else
  (void) signal(SIGALRM, SIG_IGN);
#endif

  if (!(cuser.uflag & COLOR_FLAG))
    showansi = 0;

  if((HAS_PERM(PERM_SYSOP)
#ifdef USE_ACCOUNTS_PERM
     || HAS_PERM(PERM_ACCOUNTS)
#endif
  ) && belong("etc/sysop", cuser.userid) && dashf(fn_register))
    DL_func("bin/register.so:m_register");

#ifdef Check_Expire
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
  if(dashd(&buf[100])) force_board(buf);  /* Dopin: Á×§K¨S¶}ªO¨t²Î¥X°ÝÃD */
#endif

#ifdef USE_NEWCOMER
  if(dashd("boards/" COMERDIR))
    if(cuser.userlevel && !(cuser.backup_char[2] & 1))
       DL_func("bin/newcomers.so:newcomers");
#endif

  (void) domenu(MMENU, "¥D¥\\¯àªí", (chkmail(0) ? 'M' : 'C'), cmdlist);
}

/* ----------------------------------------------------- */
/* FSA (finite state automata) for telnet protocol       */
/* ----------------------------------------------------- */

static void telnet_init() {
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
  sprintf(data, BANNER, genbuf);
  write(0, data, strlen(data));

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
    {
      read(0, data, sizeof(buf));
    }
    rset = oset;
  }
}

/* ----------------------------------------------- */
/* ¨ú±o remote user name ¥H§P©w¨­¥÷                */
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

static void timeout(int sig) {
  (void) longjmp(byebye, sig);
}

static unsigned getremotename(struct sockaddr_in *from, char *rhost, char *rname) {
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

/*
woju
  *rname = '\0';
*/
  s = sizeof our_sin;
  if(getsockname(0, (struct sockaddr*)&our_sin, &s) < 0)
    return 0;

  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return 0;
  }

  if(!(fp = fdopen(s, "r+"))) {
    (void) close(s);
    return 0;
  }

  /*
   * Set up a timer so we won't get stuck while waiting for the server.
   */

  if(setjmp(byebye) == 0)
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

  return our_pt;
}

/* ----------------------------------- */
/* check system / memory / CPU loading */
/* ----------------------------------- */

#ifdef  HAVE_CHKLOAD
int fkmem;

static void
chkload_init()
{
#include <nlist.h>
#define VMUNIX  "/vmunix"
#define KMEM    "/dev/kmem"

  static struct nlist nlst[] = {
    {"_avenrun"},
    {0}
  };
  register int kmem;
  long offset;

  (void) nlist(VMUNIX, nlst);
  if (nlst[0].n_type == 0)
    exit(1);
  offset = (long) nlst[0].n_value;

  if ((kmem = open(KMEM, O_RDONLY)) == -1)
    exit(1);

  if (lseek(kmem, offset, L_SET) == -1)
    exit(1);

  fkmem = kmem;
}

static int chkload(int limit) {
  double cpu_load[3];
  long avenrun[3];
  register int i;

  i = fkmem;

  if (read(i, (char *) avenrun, sizeof(avenrun)) == -1)
    exit(1);

  lseek(i, -(off_t) sizeof(avenrun), SEEK_CUR);

#define loaddouble(la) ((double)(la) / (1 << 8))

  for (i = 0; i < 3; i++)
    cpu_load[i] = loaddouble(avenrun[i]);

  memcpy(utmpshm->sysload, cpu_load, sizeof(cpu_load));

  i = cpu_load[0] + cpu_load[1] * 4;
  if (i < limit)
    i = 0;
  sprintf(genbuf, "¨t²Î­t¸ü %.2f %.2f %.2f%s",
    cpu_load[0], cpu_load[1], cpu_load[2],
    (i ? "¡A½Ðµy«á¦A¨Ó\n" : ""));

  return i >> 3;
}
#endif

/* ----------------------------------------------------- */
/* stand-alone daemon                                    */
/* ----------------------------------------------------- */

static int mainset;             /* read file descriptor set */
static struct sockaddr_in xsin;

static void reapchild() {
  int state, pid;

  while ((pid = waitpid(-1, &state, WNOHANG | WUNTRACED)) > 0);
}

static void start_daemon() {
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
  struct tm *other_dummy_time = gmtime(&dummy);
  (void) strftime(buf, 80, "%d/%b/%Y:%H:%M:%S", dummy_time);

  n = getdtablesize();
  if (fork())
    exit(0);

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

static void close_daemon() {
  exit(0);
}

static int bind_port(int port) {
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

  (void) FD_SET(sock, (fd_set *) & mainset);
  return sock;
}

/*
woju
*/
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

del_distinct(char* fname, char* line) {
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

bad_host(char* name) {
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

int main(int argc, char *argv[]) {
  extern int errno;

  register int msock, csock;    /* socket for Master and Child */
  register int nfds;            /* number of sockets */
  register int th_low, th_high, overload;
  register int *totaluser;
  register pid_t pid;
  register time_t uptime;
  int readset;
  int value;
  unsigned local_port;
  struct timeval tv;

  Argc = argc;
  Argv = argv;

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

  if (argc > 1)
  {
    msock = -1;
    for (nfds = 1; nfds < argc; nfds++)
    {
      csock = atoi(argv[nfds]);
      if (csock > 0)
         msock = bind_port(csock);
      else
         break;
    }
    if (msock < 0)
      exit(1);
  }
  else
  {
#ifndef USE_FREE_PORT
    static int ports[] = {            /* ½Ð¨Ì¶Q¯¸¹ê»Ú»Ý­n§ó§ï */
      MBBSDP, MBBSDP+1, MBBSDP+2, MBBSDP+3, MBBSDP+4
    };
#else
    static int ports[MAX_ACS_MP];
    for(nfds = 0 ; nfds < MAX_ACS_MP ; nfds++)
       ports[nfds] = mbbsd_port[nfds][1];
#endif

    for (nfds = 0; nfds < sizeof(ports) / sizeof(int); nfds++) {
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

  strcat(genbuf, "\n");
  file_append(PID_FILE, genbuf);

  /* --------------------------------------------------- */
  /* main loop                                           */
  /* --------------------------------------------------- */

  resolve_utmp();
  totaluser = &utmpshm->number;

#ifdef  HAVE_CHKLOAD
  chkload_init();
#endif

  tv.tv_sec = 60 * 30;
  tv.tv_usec = 0;

  overload = uptime = 0;

  for (;;)
  {

#ifdef  HAVE_CHKLOAD
    pid = time(0);
    if (pid > uptime)
    {
      overload = chkload(overload ? th_low : th_high);
      uptime = pid + overload + 45;     /* µu®É¶¡¤º¤£¦AÀË¬d system load */
    }
#endif

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

#ifdef  HAVE_CHKLOAD
    if (overload)
    {
      (void) write(csock, genbuf, strlen(genbuf));
      (void) close(csock);
      continue;
    }
#endif

    pid = *totaluser;
    if (pid >= MAXACTIVE)
    {
      char buf[128];

      (void) sprintf(buf, "¥Ø«e½u¤W¤H¼Æ [%d] ¤H¡A«Èº¡¤F¡A½Ðµy«á¦A¨Ó", pid);
      (void) write(csock, buf, strlen(buf));
      (void) close(csock);
      goto again;
    }

    pid = fork();

    if (!pid)
    {

#ifdef  HAVE_CHKLOAD
      (void) close(fkmem);
#endif

      while (--nfds >= 0)
        (void) close(nfds);
      (void) dup2(csock, 0);
      (void) close(csock);

      local_port =
                 getremotename(&xsin, fromhost, remoteusername);   /* FC931 */
      if(!local_port) exit(1);

      /* ban ±¼ bad host / bad user */
      if (bad_host(fromhost) && !strcmp(remoteusername, "?"))
         exit(1);

      setenv("REMOTEHOST", fromhost, 1);
      setenv("REMOTEUSERNAME", remoteusername, 1);
      {
        char RFC931[80];
        sprintf(RFC931, "%s@%s", remoteusername, fromhost);
        setenv("RFC931", RFC931, 1);
      }

      telnet_init();
      start_client(local_port);
    }

    (void) close(csock);
  }
}
