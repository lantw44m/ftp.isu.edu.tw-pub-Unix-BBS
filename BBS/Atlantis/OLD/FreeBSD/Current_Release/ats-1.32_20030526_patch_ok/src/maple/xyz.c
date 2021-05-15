/*-------------------------------------------------------*/
/* xyz.c        ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : system toolbox routines                      */
/* create : 95/03/29                                     */
/* update : 02/09/02 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"

extern char station_list_cn[][15];

/* ----------------------------------------------------- */
/* Â÷¶} BBS ¯¸                                           */
/* ----------------------------------------------------- */

void note() {
  static char *fn_note_tmp = "note.tmp";
  static char *fn_note_dat = "note.dat";
  int total, i, collect, len;
  struct stat st;
  char buf[256], buf2[80], *p;
  int fd, fx;
  FILE *fp, *foo;

  struct notedata {
    time_t date;
    char userid[IDLEN + 1];
    char username[19];
    char buf[3][80];
  };
  struct notedata myitem;

  do {
    myitem.buf[0][0] = myitem.buf[1][0] = myitem.buf[2][0] = '\0';
    move(12, 0);
    clrtobot();
    outs("\n½Ð¯d¨¥ (¦Ü¦h¤T¦æ)¡A«ö[Enter]µ²§ô");
    for (i = 0; (i < 3) &&
      getdata(16 + i, 0, "¡G", myitem.buf[i], 78, DOECHO, 0); i++);
    getdata(b_lines - 1, 0, "(S)Àx¦s (E)­«·s¨Ó¹L (Q)¨ú®ø¡H[S] ", buf, 3,
            LCECHO, 0);
    /* woju */
    if (buf[0] == 'q' || i == 0 && *buf != 'e')
      return;
  } while (buf[0] == 'e');

  strcpy(myitem.userid, cuser.userid);
  strncpy(myitem.username, cuser.username, 18);
  myitem.username[18] = '\0';
  time(&(myitem.date));

  /* begin load file */
  if ((foo = fopen(".note", "a")) == NULL) return;
  if ((fp = fopen(fn_note_ans, "w")) == NULL) return;
  if ((fx = open(fn_note_tmp, O_WRONLY | O_CREAT, 0664)) <= 0) return;

  if ((fd = open(fn_note_dat, O_RDONLY)) == -1) {
    total = 1;
  }
  else if (fstat(fd, &st) != -1) {
    total = st.st_size / sizeof(struct notedata) + 1;
    if (total > MAX_NOTE)
      total = MAX_NOTE;
  }

  fputs("\t\t\t[37;45m ¡´ «_ ÀI ªº ­^ ¶¯ ¨Æ ¸ñ ¡´ \n\n", fp);
  collect = 1;

  while (total) {
    sprintf(buf, "[1;37;46m£W[47;34m %s [33m(%s)",
      myitem.userid, myitem.username);
    len = strlen(buf);
    strcat(buf, " [30;46m" + (len & 1));

    for (i = len >> 1; i < 41; i++)
      strcat(buf, "¢e");
    sprintf(buf2, "[47;34m %.14s [37;46m£W[0m\n",
      Cdate(&(myitem.date)));
    strcat(buf, buf2);
    fputs(buf, fp);

    if (collect)
      fputs(buf, foo);

    sprintf(buf, "%s\n%s\n%s\n", myitem.buf[0], myitem.buf[1], myitem.buf[2]);
    fputs(buf, fp);

    if (collect) {
      fputs(buf, foo);
      fclose(foo);
      collect = 0;
    }

    write(fx, &myitem, sizeof(myitem));

    if (--total) read(fd, (char *) &myitem, sizeof(myitem));
  }
  fclose(fp);
  close(fd);
  close(fx);
  Rename(fn_note_tmp, fn_note_dat);
  strcpy(vetitle, "[«_ÀIªº­^¶¯¨Æ¸ñ]");
  more(fn_note_ans, YEA);
}

void mail_sysop() {
  FILE *fp;
  char genbuf[200];

  sprintf(genbuf, "etc/sysop.%s",
#ifndef NO_USE_MULTI_STATION
  cuser.station
#else
  DEFAULTST
#endif
  );

  if (fp = fopen(genbuf, "r"))
  {
    int i, j;
    char *ptr;

    struct SYSOPLIST
    {
      char userid[IDLEN + 1];
      char duty[40];
    }         sysoplist[9];

    j = 0;
    while (fgets(genbuf, 128, fp))
    {
      if (ptr = strchr(genbuf, '\n'))
      {
        *ptr = '\0';
        ptr = genbuf;
        while (isalnum(*ptr))
           ptr++;
        if (*ptr)
        {
          *ptr = '\0';
          do
          {
            i = *++ptr;
          } while (i == ' ' || i == '\t');
          if (i)
          {
            strcpy(sysoplist[j].userid, genbuf);
            strcpy(sysoplist[j++].duty, ptr);
          }
        }
      }
    }

    move(12, 0);
    clrtobot();
    prints("%16s   %-18sÅv³d¹º¤À\n\n", "½s¸¹", "¯¸ªø ID"/*, msg_seperator*/);

    for (i = 0; i < j; i++)
    {
      prints("%15d.   [1;%dm%-16s%s[0m\n",
        i + 1, 31 + i % 7, sysoplist[i].userid, sysoplist[i].duty);
    }
    prints("%-14s0.   [1;%dmÂ÷¶}[0m", "", 31 + j % 7);
    getdata(b_lines - 1, 0, "                   ½Ð¿é¤J¥N½X[0]¡G", genbuf, 4, DOECHO, 0);
    i = genbuf[0] - '0' - 1;
    if (i >= 0 && i < j)
    {
      clear();
      do_send(sysoplist[i].userid, NULL);
    }
  }
}

int m_sysop() {
  setutmpmode(MSYSOP);
  mail_sysop();
  return 0;
}

/* ----------------------------------------------------- */
/* ¤ä´©¥~±¾µ{¦¡ : tin¡Bgopher¡Bwww¡Bbbsnet¡Bgame¡Bcsh    */
/* ----------------------------------------------------- */

#ifdef HAVE_EXTERNAL
#define LOOKFIRST       (0)
#define LOOKLAST        (1)
#define QUOTEMODE       (2)
#define MAXCOMSZ        (1024)
#define MAXARGS         (40)
#define MAXENVS         (20)
#define BINDIR          "/bin/"

char *bbsenv[MAXENVS];
int numbbsenvs = 0;

int bbssetenv(char *env, char *val) {
  register int i, len;
  // extern char *malloc();

  if (numbbsenvs == 0)
    bbsenv[0] = NULL;
  len = strlen(env);
  for (i = 0; bbsenv[i]; i++)
    if (!ci_strncmp(env, bbsenv[i], len))
      break;
  if (i >= MAXENVS)
    return -1;
  if (bbsenv[i])
    free(bbsenv[i]);
  else
    bbsenv[++numbbsenvs] = NULL;
  bbsenv[i] = malloc(strlen(env) + strlen(val) + 2);
  strcpy(bbsenv[i], env);
  strcat(bbsenv[i], "=");
  strcat(bbsenv[i], val);
}

pid_t xyzpid;

void xyz_idle()
{
  struct stat ttystat;

  if (currutmp->pid != currpid)
  {
    setup_utmp(XMODE);          /* ­«·s°t¸m shm */
  }

  if (stat(currutmp->tty, &ttystat))
     currutmp->uptime = time(0);
  else
     currutmp->uptime = ttystat.st_atime;

#ifdef IDLE_TIMEOUT
  if (!(PERM_HIDE(currutmp) || currutmp->mode == MAILALL)
      && time(0) - currutmp->uptime > IDLE_TIMEOUT)
  {
    restore_tty();
    bell();
    prints("[1;37;41m¶W¹L¶¢¸m®É¶¡¡IBooting...[m\r");
    refresh();
    abort_bbs();
  }
#endif
  alarm(MOVIE_INT);
}

#ifdef HAVE_EXTERNAL
int do_exec(char *com, char *wd) {
  char genbuf[200];
  char path[MAXPATHLEN];
  char pcom[MAXCOMSZ];
  char *arglist[MAXARGS], *arglist1[MAXARGS + 1];
  char *tz;
  register int i, len;
  register int argptr;
  register char *lparse;
  int status, w;
  pid_t pid;
  int pmode;
  void (*isig) (), (*qsig) (), (*hsig)(), (*u1sig)(), (*u2sig)(), (*asig)();
  int pager0 = currutmp->pager;

  if(mbbsd) return;

  currutmp->pager = 3;
  strncpy(path, BINDIR, MAXPATHLEN);
  strncpy(pcom, com, MAXCOMSZ);
  len = MIN(strlen(com) + 1, MAXCOMSZ);
  pmode = LOOKFIRST;
  for(i = 0, argptr = 0; i < len; i++) {
    if(pcom[i] == '\0') break;

    if(pmode == QUOTEMODE) {
      if(pcom[i] == '\001') {
        pmode = LOOKFIRST;
        pcom[i] = '\0';
        continue;
      }
      continue;
    }

    if(pcom[i] == '\001') {
      pmode = QUOTEMODE;
      arglist[argptr++] = &pcom[i + 1];
      if(argptr + 1 == MAXARGS) break;
      continue;
    }

    if(pmode == LOOKFIRST)
      if(pcom[i] != ' ') {
        arglist[argptr++] = &pcom[i];
        if (argptr + 1 == MAXARGS)
          break;
        pmode = LOOKLAST;
      }
      else continue;

    if(pcom[i] == ' ') {
      pmode = LOOKFIRST;
      pcom[i] = '\0';
    }
  }

  arglist[argptr] = NULL;
  if(argptr == 0) return -1;

  if(*arglist[0] == '/') strncpy(path, arglist[0], MAXPATHLEN);
  else strncat(path, arglist[0], MAXPATHLEN);

  reset_tty();
  /* woju alarm(0); */

  if((pid = vfork()) == 0) {
    if(wd)
      if(chdir(wd)) {
        fprintf(stderr, "Unable to chdir to '%s'\n", wd);
        exit(-1);
      }

    bbssetenv("PATH", "/bin:.");
    bbssetenv("TERM", "vt100"/* cuser.termtype */);
    bbssetenv("USER", cuser.userid);
    bbssetenv("USERNAME", cuser.username);
    /* added for tin's HOME and EDITOR */

    /* woju */
    sprintf(genbuf, BBSHOME "/home/%s", cuser.userid);
    bbssetenv("HOME", genbuf);
    bbssetenv("EDITOR", "bin/ve");
    /* end */

    /* added for tin's reply to */
    bbssetenv("REPLYTO", cuser.email);
    bbssetenv("FROMHOST", fromhost);
    /* end of insertion */

    if((tz = getenv("TZ")) != NULL) bbssetenv("TZ", tz);
    if(numbbsenvs == 0) bbsenv[0] = NULL;

    memcpy(arglist1 + 1, arglist, sizeof(arglist));
    arglist1[0] = "bin/bbsroot";
    execve("bin/bbsroot", arglist1, bbsenv);

    fprintf(stderr, "EXECV FAILED... path = '%s'\n", path);
    exit(-1);
  }

  xyzpid = pid;
  asig = signal(SIGALRM, xyz_idle);
  isig = signal(SIGINT, SIG_IGN);
  qsig = signal(SIGQUIT, SIG_IGN);
  u1sig = signal(SIGUSR1, SIG_IGN);
  u2sig = signal(SIGUSR2, SIG_IGN);
  while ((w = wait(&status)) != pid && w != 1)

     /* NULL STATEMENT */ ;
  signal(SIGALRM, asig);
  signal(SIGINT,  isig);
  signal(SIGQUIT, qsig);
  signal(SIGUSR1, u1sig);
  signal(SIGUSR2, u2sig);
  currutmp->pager = pager0;

  restore_tty();

#ifdef DOTIMEOUT
/* woju alarm(IDLE_TIMEOUT); */
#endif

  return ((w == -1) ? w : status);
}

int exec_cmd(int umode, int pager, char *cmdfile, char *mesg) {
  char buf[64];
  int save_pager;

  if(!dashf(cmdfile)) {
    move(2, 0);
    prints("«Ü©êºp, ¥»¯¸¤£´£¨Ñ %s (%s) ¥\\¯à.", mesg, cmdfile);
    return 0;
  }

  save_pager = currutmp->pager;
  if(pager == NA) currutmp->pager = pager;

  setutmpmode(umode);
  sprintf(buf, "/bin/sh %s", cmdfile);
  reset_tty();
  do_exec(buf, NULL);
  restore_tty();
  currutmp->pager = save_pager;
  clear();
  return 0;
}
#endif

#ifdef HAVE_TIN
x_tin()
{
  return exec_cmd(XMODE, YEA, "bin/tin.sh", "TIN");
}
#endif


#ifdef HAVE_GOPHER
x_gopher()
{
  return exec_cmd(XMODE, YEA, "bin/gopher.sh", "GOPHER");
}
#endif

#ifdef HAVE_BRIDGE
x_bridge()
{
  char buf[64];
  int save_pager;

  clear();
  outs("Åwªï¶i¤J ¡´[31m¶§¥ú[1;33m¨FÅy[0m¡´ ¨Óª±[1;37mºô¸ô¾ôµP[0m\n");
  outs("[1;35mKaede[37m:[0m ª±¾ôµP¤£¥Î½ä¿ú,\n");
  outs("       ÁÙ¥i¥H©M¤j®a³s½Ë,¦hª±¦³¯q°·±d³á!! *^_^*\n\n");
  outs("[1;34mRaw[37m:[0m ¶â¶â..ÁÙ¥i¥H¹w¨¾¦Ñ¤HÃ¨§b¯g!!\n");
  outs("     (§O¤£¬Û«H,³o¦³®Ú¾Úªº!!)\n\n");
  outs("p.s: ²Ä¤@¦ì¤W½uªÌ±N¦Û°Ê¦¨¬° Server!\n");
  pressanykey(NULL);

  save_pager = currutmp->pager;
  currutmp->pager = NA;
  setutmpmode(BRIDGE);
  reset_tty();

  sprintf(buf, BBSHOME"/bin/okbridge -n %s -s freebsd", cuser.userid);
  if (!mbbsd)
     do_exec(buf, NULL);

  restore_tty();
  currutmp->pager = save_pager;

  clear();
  prints("ÁÂÁÂ [1;33m%s[0m ªº¥úÁ{!!\n", cuser.userid);
  outs("Åwªï¤U¦¸¦A¨Ó´ê¥|¸}°Ú!! ^o^\n");
  pressanykey(NULL);

  clear();
  return 0;
}
#endif

#ifdef HAVE_WWW
x_www()
{
  return exec_cmd(WWW, NA, "bin/www.sh", "WWW");
}
#endif                          /* HAVE_WWW */

#ifdef HAVE_IRC
x_irc()
{
  return exec_cmd(XMODE, NA, "bin/irc.sh", "IRC");
}
#endif                          /* HAVE_IRC */

#ifdef  HAVE_ADM_SHELL
x_csh()
{
  int save_pager;

  clear();
  refresh();
  reset_tty();
  save_pager = currutmp->pager;
  currutmp->pager = NA;

#ifdef  HAVE_REPORT
  report("shell out");
#endif

#ifdef SYSV
  do_exec("sh", NULL);
#else
  do_exec("tcsh", NULL);
#endif

  restore_tty();
  currutmp->pager = save_pager;
  clear();
  return 0;
}
#endif                          /* NO_ADM_SHELL */

#ifdef BBSDOORS
x_bbsnet()                      /* Bill Schwartz */
{
  int save_pager = currutmp->pager;

  currutmp->pager = NA;

  setutmpmode(BBSNET);
  /* bbsnet.sh is a shell script that can be customized without */
  /* having to recompile anything.  If you edit it while someone */
  /* is in bbsnet they will be sent back to the xyz menu when they */
  /* leave the system they are currently in. */

  reset_tty();
  do_exec("bbsnet.sh", NULL);
  restore_tty();
  currutmp->pager = save_pager;
  clear();
  return 0;
}
#endif

#ifdef HAVE_GAME
x_game()
{
  int save_pager = currutmp->pager;

  currutmp->pager = NA;

  setutmpmode(BBSNET);
  /* bbsnet.sh is a shell script that can be customized without */
  /* having to recompile anything.  If you edit it while someone */
  /* is in bbsnet they will be sent back to the xyz menu when they */
  /* leave the system they are currently in. */

  reset_tty();
  do_exec("okbridge.sh", NULL);
  restore_tty();
  currutmp->pager = save_pager;
  clear();
  return 0;
}
#endif

#endif   /* HAVE_EXTERNAL */
