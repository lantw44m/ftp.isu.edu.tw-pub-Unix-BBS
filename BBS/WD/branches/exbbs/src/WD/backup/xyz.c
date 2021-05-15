/*-------------------------------------------------------*/
/* xyz.c        ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : system toolbox routines                      */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#include "bbs.h"


/* ----------------------------------------------------- */
/* ¦UºØ²Î­p¤Î¬ÛÃö¸ê°T¦Cªí                                */
/* ----------------------------------------------------- */

int
show_hint_message()
{
        struct timeval  timep;
        struct timezone timezp;
        int     i, j, msgNum;
        FILE    *hintp;
        char    msg[136];

        if (!(hintp = fopen(BBSHOME"/etc/hint", "r")))
          return 0;
        fgets(msg, 135, hintp);
        msgNum = atoi(msg);
        gettimeofday(&timep, &timezp);
        i = (int) timep.tv_sec%(msgNum + 1); /* ³Ì·sªº¤@½g¾÷·|¥[­¿ */
        if (i == msgNum)
          i--;
        j = 0;

        while (j < i)
        {
          fgets(msg, 135, hintp);
          msg[1] = '\0';
          if (!strcmp(msg,"#"))
            j++;
        }
        move(12, 0);
        clrtobot();
        fgets(msg, 135, hintp);
        prints("[1;36m­·§j¨Óªº®ø®§¡G [1;31m±zª¾¹D¶Ü¡H[40;0m\n");
        prints("                   %s[0m", msg);
        fgets(msg, 135, hintp);
        prints("                   %s[0m", msg);
        pressanykey(NULL);
        fclose(hintp);
}

/* ----------------------------------------------------- */
/* Â÷¶} BBS ¯¸                                           */
/* ----------------------------------------------------- */


void
note()
{
  static char *fn_note_tmp = "note.tmp";
  static char *fn_note_dat = "note.dat";
  int total, i, collect, len;
  struct stat st;
  char buf[256], buf2[80];
  int fd, fx;
  FILE *fp, *foo;
  struct notedata
  {
    time_t date;
    char userid[IDLEN + 1];
    char username[19];
    char buf[3][80];
  };
  struct notedata myitem;
  if(check_money(100)) return;
  setutmpmode(EDNOTE);
  do
  {
    myitem.buf[0][0] = myitem.buf[1][0] = myitem.buf[2][0] = '\0';
    move(12, 0);
    clrtobot();
    outs("\n½Ð¯d¨¥ (¦Ü¦h¤T¦æ)¡A«ö[Enter]µ²§ô");
    for (i = 0; (i < 3) &&
      getdata(16 + i, 0, "¡G", myitem.buf[i], 78, DOECHO,0); i++);
    getdata(b_lines - 1, 0, "(S)Àx¦s (E)­«·s¨Ó¹L (Q)¨ú®ø¡H[S] ", buf, 3, LCECHO,"S");
/*
woju
*/
    if (buf[0] == 'q' || i == 0 && *buf != 'e')
      return;
  } while (buf[0] == 'e');
  demoney(100);
  strcpy(myitem.userid, cuser.userid);
  strncpy(myitem.username, cuser.username, 18);
  myitem.username[18] = '\0';
  time(&(myitem.date));

  /* begin load file */

  if ((foo = fopen(BBSHOME"/.note", "a")) == NULL)
    return;

  if ((fp = fopen(fn_note_ans, "w")) == NULL)
    return;

  if ((fx = open(fn_note_tmp, O_WRONLY | O_CREAT, 0644)) <= 0)
    return;

  if ((fd = open(fn_note_dat, O_RDONLY)) == -1)
  {
    total = 1;
  }
  else if (fstat(fd, &st) != -1)
  {
    total = st.st_size / sizeof(struct notedata) + 1;
    if (total > MAX_NOTE)
      total = MAX_NOTE;
  }

  fputs("[1m                             "COLOR1" ¡» [37m¤ß ±¡ ¯d ¨¥ ªO [33m¡»[0m \n\n",fp);
  collect = 1;

  while (total)
  {
    sprintf(buf, "[1;31m¡¼ùù [33m%s[37m(%s)",
      myitem.userid, myitem.username);
    len = strlen(buf);
    strcat(buf," [31m" + (len&1));

    for (i = len >> 1; i < 36; i++)
      strcat(buf, "ùù");
    sprintf(buf2, "ùù[33m %.14s [31mùù¡¼[0m\n",
      Cdate(&(myitem.date)));
    strcat(buf, buf2);
    fputs(buf, fp);

    if (collect)
      fputs(buf, foo);

    sprintf(buf, "%s\n%s\n%s\n", myitem.buf[0], myitem.buf[1], myitem.buf[2]);
    fputs(buf, fp);

    if (collect)
    {
      fputs(buf, foo);
      fclose(foo);
      collect = 0;
    }

    write(fx, &myitem, sizeof(myitem));

    if (--total)
      read(fd, (char *) &myitem, sizeof(myitem));
  }
  fclose(fp);
  close(fd);
  close(fx);
  Rename(fn_note_tmp, fn_note_dat);
  more(fn_note_ans, YEA);
}


int
m_sysop()
{
  FILE *fp;
  char genbuf[200];

  setutmpmode(MSYSOP);
  if (fp = fopen(BBSHOME"/etc/sysop", "r"))
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
    getdata(b_lines - 1, 0, "                   ½Ð¿é¤J¥N½X[0]¡G", genbuf, 4, DOECHO,"1");
    i = genbuf[0] - '0' - 1;
    if (i >= 0 && i < j)
    {
      clear();
      do_send(sysoplist[i].userid, NULL);
    }
  }
  return 0;
}


int
Goodbye()
{
  extern void movie();
  char genbuf[100];

  getdata(b_lines - 1, 0, "±z½T©w­nÂ÷¶}¡i " BOARDNAME " ¡j¶Ü(Y/N)¡H[N] ",
    genbuf, 3, LCECHO,0);

  if (*genbuf != 'y')
    return 0;
  movie(999);
  if (cuser.userlevel)
  {
    getdata(b_lines - 1, 0, "(G)ÀH­·¦Ó³u (M)¦«¹Ú¯¸ªø (N)§Ú­n§o³Û [G] ",
      genbuf, 3, LCECHO,0);
    if (genbuf[0] == 'm')
      m_sysop();
    else if (genbuf[0] == 'n')
      note();
  }

  t_display();
  clear();
  prints("[1;31m¿Ë·Rªº [31m%s([37m%s)[31m¡A§O§Ñ¤F¦A«×¥úÁ{"COLOR1
    " %s [40;33m¡I\n\n¥H¤U¬O±z¦b¯¸¤ºªºµù¥U¸ê®Æ:[m\n",
    cuser.userid, cuser.username, BoardName);
  user_display(&cuser, 0);
  pressanykey(NULL);

  more(BBSHOME"/etc/Logout",NA);
  pressanykey(NULL);
  if (currmode)
    u_exit("EXIT ");
  reset_tty();
  exit(0);
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
#define BINDIR          BBSHOME"/bin/"

char *bbsenv[MAXENVS];
int numbbsenvs = 0;


int
bbssetenv(env, val)
  char *env, *val;
{
  register int i, len;
  extern char *malloc();

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

#define MAXPATHLEN 256

int
do_exec(com, wd)
  char *com, *wd;
{
  char genbuf[200];
  char path[MAXPATHLEN];
  char pcom[MAXCOMSZ];
  char *arglist[MAXARGS];
  char *tz;
  register int i, len;
  register int argptr;
  int status, w;
  pid_t pid;
  int pmode;
  void (*isig) (), (*qsig) ();

  strncpy(path, BINDIR, MAXPATHLEN);
  strncpy(pcom, com, MAXCOMSZ);
  len = MIN(strlen(com) + 1, MAXCOMSZ);
  pmode = LOOKFIRST;
  for (i = 0, argptr = 0; i < len; i++)
  {
    if (pcom[i] == '\0')
      break;
    if (pmode == QUOTEMODE)
    {
      if (pcom[i] == '\001')
      {
        pmode = LOOKFIRST;
        pcom[i] = '\0';
        continue;
      }
      continue;
    }
    if (pcom[i] == '\001')
    {
      pmode = QUOTEMODE;
      arglist[argptr++] = &pcom[i + 1];
      if (argptr + 1 == MAXARGS)
        break;
      continue;
    }
    if (pmode == LOOKFIRST)
      if (pcom[i] != ' ')
      {
        arglist[argptr++] = &pcom[i];
        if (argptr + 1 == MAXARGS)
          break;
        pmode = LOOKLAST;
      }
      else
        continue;
    if (pcom[i] == ' ')
    {
      pmode = LOOKFIRST;
      pcom[i] = '\0';
    }
  }
  arglist[argptr] = NULL;
  if (argptr == 0)
    return -1;
  if (*arglist[0] == '/')
    strncpy(path, arglist[0], MAXPATHLEN);
  else
    strncat(path, arglist[0], MAXPATHLEN);
  reset_tty();
  alarm(0);
  if ((pid = vfork()) == 0)
  {
    if (wd)
      if (chdir(wd))
      {
        fprintf(stderr, "Unable to chdir to '%s'\n", wd);
        exit(-1);
      }
    bbssetenv("PATH", "/bin:.");
    bbssetenv("TERM", "vt100"/* cuser.termtype */);
    bbssetenv("USER", cuser.userid);
    bbssetenv("USERNAME", cuser.username);
    /* added for tin's HOME and EDITOR */
/*
woju
*/
    sprintf(genbuf, BBSHOME"/home/%s", cuser.userid);
    bbssetenv("HOME", genbuf);
    bbssetenv("EDITOR", "/bin/ve");
    /* end */
    /* added for tin's reply to */
    bbssetenv("REPLYTO", cuser.email);
    bbssetenv("FROMHOST", fromhost);
    /* end of insertion */
    if ((tz = getenv("TZ")) != NULL)
      bbssetenv("TZ", tz);
    if (numbbsenvs == 0)
      bbsenv[0] = NULL;
    execve(path, arglist, bbsenv);
    fprintf(stderr, "EXECV FAILED... path = '%s'\n", path);
    exit(-1);
  }
  isig = signal(SIGINT, SIG_IGN);
  qsig = signal(SIGQUIT, SIG_IGN);
  while ((w = wait(&status)) != pid && w != 1)
     /* NULL STATEMENT */ ;
  signal(SIGINT, isig);
  signal(SIGQUIT, qsig);
  restore_tty();

#ifdef DOTIMEOUT
  alarm(IDLE_TIMEOUT);
#endif

  return ((w == -1) ? w : status);
}


int
exec_cmd(umode, pager, cmdfile, mesg)
  char *cmdfile, *mesg;
{
  char buf[64];
  int save_pager;

  if (!dashf(cmdfile))
  {
    move(2, 0);
    prints("«Ü©êºp, ¥»¯¸¤£´£¨Ñ %s (%s) ¥\\¯à.", mesg, cmdfile);
    return 0;
  }
  save_pager = currutmp->pager;
  if (pager == NA)
  {
    currutmp->pager = pager;
  }
  setutmpmode(umode);
  sprintf(buf, "/bin/sh %s", cmdfile);
  reset_tty();
  do_exec(buf, NULL);
  restore_tty();
  currutmp->pager = save_pager;
  clear();
  return 0;
}

#ifdef HAVE_MJ
x_mj()
{
  char buf[64];
  int save_pager;
  if(check_money(100)) return 0;
  clear();
  counter(BBSHOME"/counter/³Â±N","¥´³Â±N");
  pressanykey(NULL);
  save_pager = currutmp->pager;
  currutmp->pager = 2;
  setutmpmode(MJ);
  demoney(100);
  reset_tty();
  sprintf(buf, BBSHOME"/bin/qkmj95p4-freebsd 140.112.28.167 7001");
  do_exec(buf, NULL);
  restore_tty();
  currutmp->pager = save_pager;
  clear();
  prints("          ÁÂÁÂ [1;33m%s[0m ªº¥úÁ{!!\n", cuser.userid);
  pressanykey("Åwªï¤U¦¸¦A¨ÓºN¨â°é°Ú!! ^o^");
  clear();
  return 0;
}
#endif

#ifdef HAVE_BIG2
x_big2()
{
  char buf[64];
  int save_pager;
  if(check_money(100)) return 0;
  clear();
  counter(BBSHOME"/counter/¤j¦Ñ¤G","¥´¤j¦Ñ¤G");
  pressanykey(NULL);
  save_pager = currutmp->pager;
  currutmp->pager = 2;
  setutmpmode(BIG2);
  demoney(100);
  reset_tty();
  sprintf(buf, BBSHOME"/bin/big2.sh");
  do_exec(buf, NULL);

  restore_tty();
  currutmp->pager = save_pager;

  clear();
  prints("          ÁÂÁÂ [1;33m%s[0m ªº¥úÁ{!!\n", cuser.userid);
    outs("          Åwªï¤U¦¸¦A¨Ó´ê¥|¸}°Ú!! ^o^\n");
  pressanykey(NULL);

  clear();
  return 0;
}
#endif

#ifdef  HAVE_ADM_SHELL
x_csh()
{
  int save_pager;

  clear();
  refresh();
  reset_tty();
  save_pager = currutmp->pager;
  currutmp->pager = 2;
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
#endif                          /* HAVE_EXTERNAL */

int
x_showload()
{
  char genbuf[80];
  double cpu_load[3];
  int i;
  getloadavg(cpu_load, 3);
  i = cpu_load[0];
  sprintf(genbuf, "[1;33m¥Ø«e¨t²Î­t²ü:[37m %.2f  %.2f  %.2f , %s",
  cpu_load[0], cpu_load[1],  cpu_load[2],i ? 
  ( i-1 ? "[31m­t²ü¦MÀI" : "[33m­t²ü°¾°ª") : "[32m­t²ü¥¿±`");
  if (i) 
    system(BBSHOME"/bin/shutdownbbs &");
  move(b_lines - 1, 0);
  pressanykey(genbuf);
  return 0;
}

#ifdef LINUX
int getloadavg( double load[], int nelem ) {
  FILE *fp;
  int i, j=0;

  fp = fopen("/proc/loadavg", "r");
  if( fp == NULL ) {
    for( i=0; i<nelem; i++ )
      load[i] = 0.0;
    return -1;
  }

  for( i=0; i<nelem; i++ ) {
    j += fscanf(fp, "%lf", &load[i]);
  }

  return j;
}
#endif
