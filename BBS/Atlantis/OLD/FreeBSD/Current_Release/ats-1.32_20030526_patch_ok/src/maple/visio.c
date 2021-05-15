/*-------------------------------------------------------*/
/* visio.c      ( NTHU CS MapleBBS Ver 3.00 )            */
/*-------------------------------------------------------*/
/* target : VIrtual Screen Input Output routines         */
/* create : 95/03/29                                     */
/* update : 96/10/10                                     */
/*-------------------------------------------------------*/

#include <varargs.h>
#include <arpa/telnet.h>

#define MBBSD
#include "bbs.h"

#define OBUFSIZE  (3072)
#define IBUFSIZE  (512)

#define INPUT_ACTIVE    0
#define INPUT_IDLE      1

static char inbuf[IBUFSIZE];
static char datemsg[32];
static int ibufsize;
static int icurrchar;
static int i_mode = INPUT_ACTIVE;

int t_lines = 24;
int scr_lns = 24;
int b_lines = 23;
int p_lines = 20;
int t_columns = ANSILINELEN;
uschar scr_cols = ANSILINELEN;
int KEY_ESC_arg;
int dumb_term = NA;

/* ----------------------------------------------------- */
/* ©w®ÉÅã¥Ü°ÊºA¬ÝªO                                      */
/* ----------------------------------------------------- */

#ifdef  HAVE_MOVIE
#define STAY_TIMEOUT    (30*60)
extern void movie();

static void hit_alarm_clock() {
  static int stay_time = 0;
  static int idle_time = 0;

  if(currutmp->pid != currpid)
    setup_utmp(XMODE);          /* ­«·s°t¸m shm */

  if(i_mode == INPUT_IDLE) {
    idle_time += MOVIE_INT;

#ifdef IDLE_TIMEOUT
    if(!(PERM_HIDE(currutmp) || currutmp->mode == MAILALL)
       && idle_time > IDLE_TIMEOUT) {

      clear();
      fprintf(stderr, "¶W¹L¶¢¸m®É¶¡¡IBooting...\n");
      abort_bbs();
    }
  }
  else {
#endif
    currutmp->uptime = time(NULL);
    idle_time = 0;
    i_mode = INPUT_IDLE;
  }

  if(currstat && (currstat < CLASS  || currstat == MAILALL)) movie(0);

  alarm(MOVIE_INT);
  stay_time += MOVIE_INT;
  *currmsg = 0;

#ifdef MOVIE_AND_GOODBABY
  DL_func("bin/goodbaby.so:check_reject_me");
#endif

#ifndef IDLE_TIMEOUT
  if(!chkmail(0)) return;
#endif

  if(chkmail(0) && stay_time > 10 * 60 || stay_time > STAY_TIMEOUT) {
    /* ¦b³o¸Ì´£¥Ü user ¥ð®§¤@¤U */
    char *msg[7] = { "[³ø®É] ¦ù¦ù¸y, ´|´|²´, ³Ü¤f¯ù...",
                     "[³ø®É] ±z¤w¸g¥´¤F¥b­Ó¤p®ÉÅo! °_¨Ó¬¡°Ê¤@¤U§a!",
                     "[³ø®É] ¦³¨S¦³¤H»¡¹L§A«Ü¦³¾y¤O?",
                     "[³ø®É] ¦A§¤¤U¥h­n±o²©½H¤F°Õ~~~",
                     "[³ø®É] ¦³µL­«­nªº¨Æ±¡§Ñ¤F³B²z?",
                     "[³ø®É] ¥Î¡E¥\\¡E°á¡E®Ñ",
                     "[³ø®É] ¬Ý! ­¸ºÐ!!!"};
    /* woju */
    time_t now = time(0);
    int i = rand() % 7;

    sprintf(currmsg, "[1;33;41m<%s> %s[m", Cdate(&now),
            chkmail(0) ? "[´£¥Ü] «H½cùØÁÙ¦³¨S¬Ý¹Lªº«H®@!" : msg[i]);

    kill(currpid, SIGUSR2);
    stay_time = 0;

#ifndef MOVIE_AND_GOODBABY
    DL_func("bin/goodbaby.so:check_reject_me");
#endif
  }

#ifdef IDLE_TIMEOUT
  if(idle_time > IDLE_TIMEOUT - 60) {
    sprintf(currmsg, "[1;5;37;41mÄµ§i¡G±z¤w¶¢¸m¹L¤[¡A­YµL¦^À³¡A¨t²Î§Y±N¤Á"
                     "Â÷¡I¡I[m");
    kill(currpid, SIGUSR2);
  }
#endif
}

void init_alarm() {
  alarm(0);
  signal(SIGALRM, hit_alarm_clock);
  alarm(MOVIE_INT);
}

#else                           /* HAVE_MOVIE */

static void hit_alarm_clock()
{
  if (HAS_PERM(PERM_NOTIMEOUT))
    return;

#ifdef IDLE_TIMEOUT
  if (i_mode == INPUT_IDLE)
  {
    clear();
    fprintf(stderr, "¶W¹L¶¢¸m®É¶¡¡IBooting...\n");
    kill(currpid, SIGHUP);
  }
  i_mode = INPUT_IDLE;
  alarm(IDLE_TIMEOUT);
#endif
}

void init_alarm() {
  signal(SIGALRM, hit_alarm_clock);
#ifdef IDLE_TIMEOUT
  alarm(IDLE_TIMEOUT);
#endif
}
#endif                          /* HAVE_MOVIE */

/* ----------------------------------------------------- */
/* output routines                                       */
/* ----------------------------------------------------- */

static char outbuf[OBUFSIZE];
static int obufsize = 0;

oflush() {
  register int size;

  if(size = obufsize) {
    write(0, outbuf, size);
    obufsize = 0;
  }
}

output(s, len)
  char *s;
  int len;
{
  /* Invalid if len >= OBUFSIZE */

  register int size;
  register char *data;

  size = obufsize;
  data = outbuf;
  if (size + len > OBUFSIZE)
  {
    write(0, data, size);
    size = len;
  }
  else
  {
    data += size;
    size += len;
  }
  memcpy(data, s, len);
  obufsize = size;
}

void ochar(c)
  register int c; {
  register char *data;
  register int size;

  data = outbuf;
  size = obufsize;
  if (size > OBUFSIZE - 2)
  {
    write(0, data, size);
    size = 0;
  }
  data[size++] = c;
  if (c == IAC)
    data[size++] = c;
  obufsize = size;
}

/* ----------------------------------------------------- */
/* input routines                                        */
/* ----------------------------------------------------- */

static int i_newfd;
static struct timeval i_to, *i_top;

void add_io(fd, timeout)
  int fd;
  int timeout;
{
  i_newfd = fd;
  if (timeout)
  {
    i_to.tv_sec = timeout;
    i_to.tv_usec = 0;
    i_top = &i_to;
  }
  else
    i_top = NULL;
}

void add_flush(flushfunc)
  int (*flushfunc) ();
{
  flushf = flushfunc;
}

static int iac_count(current)
  char *current;
{
  switch (*(current + 1) & 0xff)
  {
  case DO:
  case DONT:
  case WILL:
  case WONT:
    return 3;
  case SB:                      /* loop forever looking for the SE */
    {
      register char *look = current + 2;

      for (;;)
      {
        if ((*look++ & 0xff) == IAC)
        {
          if ((*look++ & 0xff) == SE)
          {
            return look - current;
          }
        }
      }
    }
  default:
    return 1;
  }
}

int igetch() {
  static int trailing = 0;
  register int ch;
  register char *data;

  data = inbuf;

  for (;;)
  {
    if (ibufsize == icurrchar)
    {
      fd_set readfds;
      struct timeval to;
      register fd_set *rx;
      register int fd, nfds;

      rx = &readfds;
      fd = i_newfd;

  igetnext:

      FD_ZERO(rx);
      FD_SET(0, rx);
      if (fd)
      {
        FD_SET(fd, rx);
        nfds = fd + 1;
      }
      else
        nfds = 1;
      to.tv_sec = to.tv_usec = 0;
      if ((ch = select(nfds, rx, NULL, NULL, &to)) <= 0)
      {
        if (flushf)
          (*flushf) ();

        if (visiop)
          refresh();
        else
          oflush();

        FD_ZERO(rx);
        FD_SET(0, rx);
        if (fd)
          FD_SET(fd, rx);

        while ((ch = select(nfds, rx, NULL, NULL, i_top)) < 0)
        {
          if (errno != EINTR)
            return -1;
        }
        if (ch == 0)
          return I_TIMEOUT;
      }
      if (fd && FD_ISSET(fd, rx))
        return I_OTHERDATA;

      for (;;)
      {
        ch = read(0, data, IBUFSIZE);
        if (ch > 0)
          break;
        if ((ch < 0) && (errno == EINTR))
          continue;
        longjmp(byebye, -1);
      }
      icurrchar = (*data & 0xff) == IAC ? iac_count(data) : 0;
      if (icurrchar >= ch)
        goto igetnext;
      ibufsize = ch;
      i_mode = INPUT_ACTIVE;
    }

    ch = data[icurrchar++];
    if (trailing)
    {
      trailing = 0;
      if (ch == 0 || ch == 0x0a)
/*
rexchen
      if (!ch)
*/
        continue;
    }

    if (ch == Ctrl('L'))
    {
      redoscr();
      continue;
    }
    if (ch == 0x0d)
    {
      trailing = 1;
      ch = '\n';
    }
    return (ch);
  }
}

char *phone_char(char c)
{
   switch (c) {
          case '1':
             return "£t";
          case 'q':
             return "£u";
          case 'a':
             return "£v";
          case 'z':
             return "£w";
          case '2':
             return "£x";
          case 'w':
             return "£y";
          case 's':
             return "£z";
          case 'x':
             return "£{";
          case 'e':
             return "£|";
          case 'd':
             return "£}";
          case 'c':
             return "£~";
          case 'r':
             return "£¡";
          case 'f':
             return "£¢";
          case 'v':
             return "££";
          case '5':
             return "£¤";
          case 't':
             return "£¥";
          case 'g':
             return "£¦";
          case 'b':
             return "£§";
          case 'y':
             return "£¨";
          case 'h':
             return "£©";
          case 'n':
             return "£ª";
          case 'u':
             return "£¸";

          case 'j':
             return "£¹";
          case 'm':
             return "£º";
          case '8':
             return "£«";
             break;
          case 'i':
             return "£¬";
          case 'k':
             return "£­";
          case ',':
             return "£®";
          case '9':
             return "£¯";
          case 'o':
             return "£°";
          case 'l':
             return "£±";
          case '.':
             return "£²";
          case '0':
             return "£³";
          case 'p':
             return "£´";
          case ';':
             return "£µ";
          case '/':
             return "£¶";
          case '-':
             return "£·";
          case '6':
             return "£½";
          case '3':
             return "£¾";
          case '4':
             return "£¿";
          case '7':
             return "£»";
   }
  return 0;
}

int getdata(line, col, prompt, buf, len, echo, ans)
  int line, col;
  char *prompt, *buf, *ans;
  int len, echo;
{
  register int ch;
  int clen;
  int x, y;
#define MAXLASTCMD 6
  static char lastcmd[MAXLASTCMD][80];
  /* Thor: ¬°¤F¨¾¤î¦b¼ö°T¦^À³ªº·í±¼, ¦b getdata®É, ¤£ºÞ i_newfd */

  int my_newfd = i_newfd;
  i_newfd = 0;

  if(prompt) {
    move(line, col);
    clrtoeol();
    if(strchr(prompt, KEY_ESC)) edit_outs(prompt);
    else outs(prompt);
  }
  else clrtoeol();

  if(dumb_term || !echo) {
    len--;
    clen = 0;

    while ((ch = igetch()) != '\r') {
      if(ch == '\n') break;
#ifdef CTRL_R_REVIEW
      /* Dopin: ²Ä 0 ­Ó¦ì¸m ¥B¬°¦^°T¼Ò¦¡®É ¦pªG«ö Ctrl('R') */
      if(cuser.extra_mode[3] == 1 && ch == Ctrl('R') &&
         !(cuser.welcomeflag & 2048)) return 255;
#endif

      if (ch == '\177' || ch == Ctrl('H'))
      {
        if (!clen)
        {
          bell();
          continue;
        }
        clen--;
        if (echo)
        {
          ochar(Ctrl('H'));
          ochar(' ');
          ochar(Ctrl('H'));
        }
        continue;
      }

#ifdef BIT8
      if (!isprint2(ch))
#else
      if (!isprint(ch))
#endif

      {
        if (echo)
          bell();
        continue;
      }
      if (clen >= len)
      {
        if (echo)
          bell();
        continue;
      }
      buf[clen++] = ch;
      if (echo)
        ochar(ch);
    }
    buf[clen] = '\0';
    outc('\n');
    oflush();
  }
  else
  {
   int cmdpos = MAXLASTCMD -1;
   int currchar = 0;
   int phone_mode = 0;
   int keydown;
   int dirty;
   char* pstr;

    getyx(&y, &x);
    standout();
    for (clen = len--; clen; clen--)
      outc(' ');
    standend();

    if (ans) {
       int i;

       strncpy(buf, ans, len);
       buf[len] = 0;
       for (i = strlen(buf) + 1; i < len; i++)
          buf[i] = 0;
       move(y, x);
       edit_outs(buf);
       clen = currchar = strlen(buf);

    }
    else
       memset(buf, 0, len);

    dirty = 0;
    while (move(y, x + currchar), (ch = igetkey()) != '\r')
    {
       /* woju */
       keydown = 0;
#ifdef CTRL_R_REVIEW
       /* Dopin: ²Ä 0 ­Ó¦ì¸m ¥B¬°¦^°T¼Ò¦¡®É ¦pªG«ö Ctrl('R') */
       if(cuser.extra_mode[3] == 1 && ch == Ctrl('R') &&
          !(cuser.welcomeflag & 2048)) return 255;
#endif

       switch (ch) {
       case KEY_DOWN:
       case Ctrl('N'):
          keydown = 1;
       case Ctrl('P'):
       case KEY_UP: {
          int i;

          if (clen && dirty) {
             for (i = MAXLASTCMD - 1; i; i--)
                strcpy(lastcmd[i], lastcmd[i - 1]);
             strncpy(lastcmd[0], buf, len);
          }

          i = cmdpos;
          do {
             if (keydown)
                --cmdpos;
             else
                ++cmdpos;
             if (cmdpos < 0)
                cmdpos = MAXLASTCMD - 1;
             else if (cmdpos == MAXLASTCMD)
                cmdpos = 0;
          } while (cmdpos != i && (!*lastcmd[cmdpos]
                   || !strncmp(buf, lastcmd[cmdpos], len)));
          if (cmdpos == i)
             continue;

          strncpy(buf, lastcmd[cmdpos], len);
          buf[len] = 0;

          move(y, x);                   /* clrtoeof */
          for (i = 0; i <= clen; i++)
             outc(' ');
          move(y, x);

          edit_outs(buf);
          clen = currchar = strlen(buf);
          dirty = 0;
          continue;
       }
       case KEY_LEFT:
          if (currchar)
             --currchar;
          continue;
       case KEY_RIGHT:
          if (buf[currchar])
             ++currchar;
          continue;
       case KEY_ESC:
           if (KEY_ESC_arg == 'p')
              phone_mode ^= 1;
           else if (KEY_ESC_arg == 'c')
              capture_screen();
           else if (KEY_ESC_arg == 'n')
              scr_exec(edit_note);
           else if (KEY_ESC == 'U' && currstat != IDLE  &&
               !(currutmp->mode == 0 &&
                 (currutmp->chatid[0] == 2 || currutmp->chatid[0] == 3)))
              scr_exec(t_users);
           continue;
       }

      if(ch == '\n' || ch == '\r') break;

      /* woju */
      if(ch == Ctrl('R') && !(currutmp->mode == 0 &&
         (currutmp->chatid[0] == 2 || currutmp->chatid[0] == 3))) {
         /* Dopin */
         DL_func("bin/message.so:get_msg", 0);

         continue;
      }

#ifdef TAKE_IDLE
      if (ch == Ctrl('I') && currstat != IDLE &&
          !(currutmp->mode == 0 &&
            (currutmp->chatid[0] == 2 || currutmp->chatid[0] == 3))) {
         scr_exec(t_idle);
         continue;
      }
#endif

      if (ch == Ctrl('B')) {
         scr_exec(m_read);
         continue;
      }
      if (ch == '\177' || ch == Ctrl('H'))
      {
        if (currchar) {
           int i;

           currchar--;
           clen--;
           for (i = currchar; i <= clen; i++)
              buf[i] = buf[i + 1];
           move(y, x + clen);
           outc(' ');
           move(y, x);
           edit_outs(buf);
           dirty = 1;
        }
        continue;
      }
      if (ch == Ctrl('Y')) {
         int i;

         buf[0] = 0;
         currchar = 0;
         move(y, x);                    /* clrtoeof */
         for (i = 0; i < clen; i++)
            outc(' ');
         clen = 0;
         continue;
      }
      if (ch == Ctrl('D')) {
        if (buf[currchar]) {
           int i;

           clen--;
           for (i = currchar; i <= clen; i++)
              buf[i] = buf[i + 1];
           move(y, x + clen);
           outc(' ');
           move(y, x);
           edit_outs(buf);
           dirty = 1;
        }
        continue;
      }
      if (ch == Ctrl('K')) {
         int i;

         buf[currchar] = 0;
         move(y, x + currchar);
         for (i = currchar; i < clen; i++)
            outc(' ');
         clen = currchar;
         dirty = 1;
         continue;
      }
      if (ch == Ctrl('A') || ch == KEY_HOME) {
         currchar = 0;
         continue;
      }

      if (ch == Ctrl('E') || ch == KEY_END) {
         currchar = clen;
         continue;
      }

      if (!(phone_mode && (pstr = phone_char(ch)) || isprint2(ch) || ch == Ctrl('U') && HAS_PERM(PERM_SYSOP)))
      {
        continue;
      }
      if (clen + (phone_mode && pstr) >= len || x + clen >= scr_cols)
      {
        continue;
      }
/*
woju
*/
      if (buf[currchar]) {               /* insert */
         int i;

         for (i = currchar; buf[i] && i + (phone_mode && pstr) < len && i + (phone_mode && pstr) < 80; i++)
            ;
         buf[i + 1 + (phone_mode && pstr)] = 0;
         for (; i > currchar; i--)
            buf[i + (phone_mode && pstr)] = buf[i - 1];
      }
      else                              /* append */
         buf[currchar + 1 + (phone_mode && pstr)] = '\0';
      if (ch == Ctrl('U'))
         ch = KEY_ESC;
      if (phone_mode && pstr) {
          buf[currchar] = pstr[0];
          buf[currchar + 1] = pstr[1];
      }
      else
         buf[currchar] = ch;
      move(y, x + currchar);
      edit_outs(buf + currchar);
      currchar++;
      currchar += phone_mode && pstr;
      clen++;
      clen += phone_mode && pstr;
      dirty = 1;
    }
    buf[clen] = '\0';
    if (clen > 1) {
       for (cmdpos = MAXLASTCMD - 1; cmdpos; cmdpos--)
          strcpy(lastcmd[cmdpos], lastcmd[cmdpos - 1]);
       strncpy(lastcmd[0], buf, len);
    }
    if (echo) {
      move(y, x + clen);
      outc('\n');
    }
    refresh();
  }
  if ((echo == LCECHO) && ((ch = buf[0]) >= 'A') && (ch <= 'Z'))
    buf[0] = ch | 32;
  /* Thor: ¨¾¤î¼ö°T¦^À³·í¾÷, ¦s¦^¥h i_newfd */

  i_newfd = my_newfd;
  return clen;
}



#define TRAP_ESC
#ifdef  TRAP_ESC
int
igetkey()
{
  int mode;
  int ch, last;

  mode = last = 0;
  for (;;)
  {
    ch = igetch();
    if (mode == 0)
    {
      if (ch == KEY_ESC)
        mode = 1;
      else
        return ch;              /* Normal Key */
    }
    else if (mode == 1)
    {                           /* Escape sequence */
      if (ch == '[' || ch == 'O')
        mode = 2;
      else if (ch == '1' || ch == '4')
        mode = 3;
      else
      {
        KEY_ESC_arg = ch;
        return KEY_ESC;
      }
    }
    else if (mode == 2)
    {                           /* Cursor key */
      if (ch >= 'A' && ch <= 'D')
        return KEY_UP + (ch - 'A');
      else if (ch >= '1' && ch <= '6')
        mode = 3;
      else
        return ch;
    }
    else if (mode == 3)
    {                           /* Ins Del Home End PgUp PgDn */
      if (ch == '~')
        return KEY_HOME + (last - '1');
      else
        return ch;
    }
    last = ch;
  }
}

#else                           /* TRAP_ESC */

int
igetkey(void)
{
  int mode;
  int ch, last;

  mode = last = 0;
  for (;;)
  {
    ch = igetch();
    if (ch == KEY_ESC)
      mode = 1;
    else if (mode == 0)         /* Normal Key */
      return ch;
    else if (mode == 1)
    {                           /* Escape sequence */
      if (ch == '[' || ch == 'O')
        mode = 2;
      else if (ch == '1' || ch == '4')
        mode = 3;
      else
        return ch;
    }
    else if (mode == 2)
    {                           /* Cursor key */
      if (ch >= 'A' && ch <= 'D')
        return KEY_UP + (ch - 'A');
      else if (ch >= '1' && ch <= '6')
        mode = 3;
      else
        return ch;
    }
    else if (mode == 3)
    {                           /* Ins Del Home End PgUp PgDn */
      if (ch == '~')
        return KEY_HOME + (last - '1');
      else
        return ch;
    }
    last = ch;
  }
}
#endif                          /* TRAP_ESC */


/* ----------------------------------------------------- */
/* virtual screen                                        */
/* ----------------------------------------------------- */


#define O_CLEAR         "\033[;H\033[2J"
#define O_CLROL         "\033[K"
#define O_SCRLV         "\033[L"
#define O_STUP          "\033[7m"
#define O_STDOWN        "\033[m"

#define o_clear()     output(O_CLEAR,sizeof(O_CLEAR)-1)
#define o_cleol()     output(O_CLROL,sizeof(O_CLROL)-1)
#define o_scrollrev() output(O_SCRLV,sizeof(O_SCRLV)-1)
#define o_standup()   output(O_STUP,sizeof(O_STUP)-1)
#define o_standdown() output(O_STDOWN,sizeof(O_STDOWN)-1)


usint cur_ln = 0, cur_col = 0;
usint docls;
usint standing = NA;
int roll = 0;
int scrollcnt, tc_col, tc_line;


screenline *big_picture, *visiop = NULL;
screenline *cur_slp;            /* current screen line pointer */
int cur_pos = 0;                /* current position with ANSI codes */

void initscr()
{
  if (!visiop) {
//    extern char *calloc();

    big_picture = visiop = cur_slp =
      (screenline *) calloc(t_lines, sizeof(screenline));
    docls = YEA;
  }
}

move(y, x) {
  if (visiop && y < t_lines && x < t_columns)
  {
    register screenline *cslp;

    cur_ln = y;
    if ((y += roll) >= t_lines)
      y -= t_lines;
    cur_slp = cslp = &visiop[y];
    cur_col = cur_pos = x;

    /* ------------------------------------- */
    /* ¹LÂo ANSI codes¡A­pºâ´å¼Ð¯u¥¿©Ò¦b¦ì¸m */
    /* ------------------------------------- */

    if (x && (cslp->mode & SL_ANSICODE))
    {
      register char *str = cslp->data;
      register int ch, cpos;
      register int ansi = NA;
      register int len = 0;

      cpos = x;
      while (x && (ch = *str))
      {
        str++;
        if (ch == KEY_ESC)
        {
          ansi = YEA;
          cpos++;
          continue;
        }
        if (ansi)
        {
          cpos++;
          if (!strchr(str_ansicode, ch))
          {
            ansi = NA;
          }
          continue;
        }
        x--;
      }
      cur_pos = cpos + x;
    }
  }
}


getyx(y, x)
  int *y, *x;
{
  *y = cur_ln;
  *x = cur_col;
}


/*-------------------------------------------------------*/
/* ­pºâ slp ¤¤ len ¤§³Bªº´å¼Ð column ©Ò¦b                */
/*-------------------------------------------------------*/

static int
ansicol(slp, len)
  screenline *slp;
  int len;
{
  if (len && (slp->mode & SL_ANSICODE))
  {
    register char ch, *str = slp->data;
    register int ansi, col;

    ansi = col = 0;

    while (len-- && (ch = *str++))
    {
      if (ch == KEY_ESC && *str == '[')
      {
        ansi = YEA;
        continue;
      }
      if (ansi)
      {
        if (ch == 'm')
          ansi = NA;
        continue;
      }
      col++;
    }
    len = col;
  }
  return len;
}


do_move(col, row)
  int col;
  int row;
{
  char buf[40];

  sprintf(buf, "\033[%d;%dH", row + 1, col + 1);
  output(buf, strlen(buf));
}


/*
woju
by `tput cs > aa`
*/
change_scroll_range(int top, int bottom)
{
  char buf[40];

  sprintf(buf, "\033[%d;%dr", top + 1, bottom + 1);
  output(buf, strlen(buf));
}

scroll_forward()
{
  output("\033D", 2);
}

scroll_reverse()
{
  output("\033M", 2);
}

save_cursor()
{
  output("\0337", 2);
}

restore_cursor()
{
  output("\0338", 2);
}

static void
rel_move(was_col, was_ln, new_col, new_ln)
  int was_col, was_ln, new_col, new_ln;
{
  if (new_ln >= t_lines || new_col >= t_columns)
    return;

  tc_col = new_col;
  tc_line = new_ln;

  if (new_col == 0)
  {
    if (new_ln == was_ln)
    {
      if (was_col)
        ochar('\r');
      return;
    }
    else if (new_ln == was_ln + 1)
    {
      ochar('\n');
      if (was_col)
        ochar('\r');
      return;
    }
  }
  if (new_ln == was_ln)
  {
    if (was_col == new_col)
    {
      return;
    }
    else if (new_col == was_col - 1)
    {
      ochar(Ctrl('H'));
      return;
    }
  }
  do_move(new_col, new_ln);
}

region_scroll_up(int top, int bottom)
{
   int i;

   if (top > bottom) {
      i = top;
      top = bottom;
      bottom = i;
   }

   if (top < 0 || bottom >= t_lines)
     return;

   for (i = top; i < bottom; i++)
      visiop[i] = visiop[i + 1];
   memset(visiop + i, 0, sizeof(*visiop));
   memset(visiop[i].data, ' ', t_columns);
   save_cursor();
   change_scroll_range(top, bottom);
   do_move(0, bottom);
   scroll_forward();
   change_scroll_range(0, t_lines - 1);
   restore_cursor();
   refresh();
}

static void standoutput(slp, ds, de)
  screenline *slp;
  int ds, de;
{
  register char *str = slp->data;
  register int sso = slp->sso, eso = slp->eso;

  if (eso <= ds || sso >= de)
  {
    output(str + ds, de - ds);
  }
  else
  {
    if (sso > ds)
    {
      output(str + ds, sso - ds);
    }
    else
      sso = ds;

    o_standup();
    output(str + sso, MIN(eso, de) - sso);
    o_standdown();

    if (de > eso)
      output(str + eso, de - eso);
  }
}


standout()
{
  if (!standing && visiop)
  {
    standing = YEA;
    cur_slp->sso = cur_slp->eso = cur_pos;
    cur_slp->mode |= SL_STANDOUT;
  }
}


static
standoff()
{
  if (standing)
  {
    standing = NA;
    if (cur_slp->eso < cur_pos);
    cur_slp->eso = cur_pos;
  }
}


standend()
{
  if (visiop)
    standoff();
}


redoscr()
{
  register screenline *bp;
  register int i, j, len;

  if (!visiop)
    return;

  o_clear();
  for (tc_col = tc_line = i = 0, bp = &visiop[j = roll];
    i < t_lines; i++, j++, bp++)
  {
    if (j >= t_lines)
    {
      j = 0;
      bp = visiop;
    }
    if (len = bp->len)
    {
      rel_move(tc_col, tc_line, 0, i);

      if (bp->mode & SL_STANDOUT)
        standoutput(bp, 0, len);
      else
        output(bp->data, len);

      bp->mode &= ~(SL_MODIFIED);

      bp->oldlen = tc_col = bp->width;
    }
  }
  rel_move(tc_col, tc_line, cur_col, cur_ln);
  docls = scrollcnt = 0;
  oflush();
}


refresh()
{
  register screenline *bp;
  register int i, j, len, smod, emod;

  if (!visiop && (icurrchar != ibufsize))
    return;

  i = scrollcnt;

  if ((docls) || (abs(i) >= p_lines))
  {
    redoscr();
    return;
  }

  if (i)
  {
    if (i < 0)
    {
      rel_move(tc_col, tc_line, 0, 0);

      do
      {
        o_scrollrev();
      } while (++i);
    }
    else
    {
      rel_move(tc_col, tc_line, 0, b_lines);
      do
      {
        ochar('\n');
      } while (--i);
    }
    scrollcnt = 0;
  }

  for (i = 0, bp = &visiop[j = roll]; i < t_lines; i++, j++, bp++)
  {
    if (j >= t_lines)
    {
      j = 0;
      bp = visiop;
    }
    len = bp->len;
    if ((bp->mode & SL_MODIFIED) && (smod = bp->smod) < len)
    {
      bp->mode &= ~(SL_MODIFIED);

      if (bp->emod >= len)
        bp->emod = len - 1;
      emod = bp->emod + 1;

      rel_move(tc_col, tc_line, ansicol(bp, smod), i);

      if (bp->mode & SL_STANDOUT)
        standoutput(bp, smod, emod);
      else
        output(&bp->data[smod], emod - smod);

      tc_col = ansicol(bp, emod);
    }
    len = bp->width /* = ansicol(bp, len) */ ;
    if (bp->oldlen > len)
    {
      rel_move(tc_col, tc_line, len, i);
      o_cleol();
    }
    bp->oldlen = len;
  }
  rel_move(tc_col, tc_line, cur_col, cur_ln);
  oflush();
}

clear()
{
  if (visiop)
  {
    register int i;
    register screenline *slp;

    docls = YEA;
    cur_pos = cur_col = cur_ln = roll = i = 0;
    cur_slp = slp = visiop;
    while (i++ < t_lines)
    {
      memset(slp++, 0, 9);
    }
  }
}

clrtoeol()
{
  if (visiop)
  {
    register screenline *cslp = cur_slp;
    register int cpos = cur_pos;

    standing = NA;
    if (cpos)
    {
      if (cpos <= cslp->sso)
        cslp->mode &= ~SL_STANDOUT;

      cslp->len = cpos;
      cslp->width = cur_col;
    }
    else
    {
      memset((char *) cslp + 1, 0, 8);
    }
  }
}


clrtobot()
{
  if (visiop)
  {
    register screenline *slp;
    register int i, j, oldlen;

    i = cur_ln;
    j = i + roll;
    slp = cur_slp;
    while (i < t_lines)
    {
      if (j >= t_lines)
      {
        j = 0;
        slp = visiop;
      }
      oldlen = slp->oldlen;
      memset((char *) slp + 1, 0, 8);
      if (slp->oldlen)
        slp->oldlen = 255;
      i++;
      j++;
      slp++;
    }
  }
}


static
addch(str, ch)
  register char *str, ch;
{
  register char i;

  i = *str;
  *str = ch;
  cur_pos++;
  cur_col++;
  if (!i)
  {
    *++str = '\0';
    cur_slp->len = cur_pos;
    cur_slp->width = cur_col;
  }
}


outc(c)
  register int c;
{
  register screenline *cslp;
  register char *txt, *str;
  register int i, j;

  static char ansibuf[16] = "\033";
  static int ansipos = 0;

#ifndef BIT8
  c &= 0x7f;
#endif

  if (!visiop)
  {

#ifdef BIT8
    if (c != KEY_ESC && !isprint2(c))
#else
    if (c != KEY_ESC && !isprint(c))
#endif

    {
      if (c == '\n')
        ochar('\r');
      else
        c = '*';
    }
    ochar(c);
    return;
  }

  cslp = cur_slp;
  txt = &(cslp->data[cur_pos]); /* «ü¦V¥Ø«e¿é¥X¦ì¸m */

  /* -------------------- */
  /* ¸É¨¬©Ò»Ý­nªºªÅ¥Õ¦r¤¸ */
  /* -------------------- */

  i = cslp->len;
  j = cur_pos - i;
  if (j >= 0)
  {
    memset(&cslp->data[i], ' ', j);
    *txt = '\0';
    cslp->len = cur_pos + 1;
  }

#ifdef BIT8
  if (c != KEY_ESC && !isprint2(c))
#else
  if (c != KEY_ESC && !isprint(c))
#endif

  {
    if (c == '\n')
    {
      if (cur_pos)
      {
        standoff();

        *txt = '\0';
        cslp->len = cur_pos;
        cslp->width = cur_col;
      }
      else
        memset((char *) cslp + 1, 0, 8);

      move(cur_ln + 1, 0);
      return;
    }
    c = '*';                    /* substitute a '*' for non-printable */
  }

  /* ---------------------------- */
  /* §P©w­þ¨Ç¤å¦r»Ý­n­«·s°e¥X¿Ã¹õ */
  /* ---------------------------- */

  if (c != *txt || c == KEY_ESC)
  {
    if (cslp->mode & SL_MODIFIED)
    {
      if (cslp->smod > cur_pos)
        cslp->smod = cur_pos;
      if (cslp->emod < cur_pos)
        cslp->emod = cur_pos;
    }
    else
    {
      cslp->mode |= SL_MODIFIED;
      cslp->smod = cslp->emod = cur_pos;
    }
  }

  /* ---------------------------- */
  /* ANSI control code ¤§¯S§O³B²z */
  /* ---------------------------- */

  if (c == KEY_ESC)
  {
    ansipos = 1;
    return;
  }
  if (ansipos == 1 && c != '[')
  {                             /* ¥i¯à¬O¨ä¥LºØÃþªº±±¨î½X */
    ansipos = 0;
    addch(txt++, KEY_ESC);
  }
  else if (ansipos)
  {
    if (ansipos >= 15)
    {
      ansipos = 0;              /* ANSI code ¤Óªø¤F¡Aªø±o¤£¦X²z¡A¤©¥H©¿²¤ */
    }
    else if (c == 'm' || strchr(str_ansicode, c))
    {
      ansibuf[ansipos++] = c;

      if (c == 'm')
      {                         /* ¥u±µ¨üÃC¦â«ü¥O¡A±Æ°£¦ì²¾«ü¥O */
        if (showansi || ansipos <= 4)
        {
          i = cur_pos + ansipos;
          if (i < ANSILINELEN - 1)
          {
            memcpy(txt, ansibuf, ansipos);
            txt[ansipos] = '\0';
            cslp->len = cslp->emod = cur_pos = i;
            cslp->width = cur_col;
            cslp->mode |= SL_ANSICODE;
          }
        }
        ansipos = 0;
      }
    }
    return;
  }
  addch(txt, c);
  if (cur_col > t_columns)
  {
    standoff();
    move(cur_ln + 1, 0);
  }
}

outch(char c)
{
   outc(c);
}

outs(str)
  register char *str;
{
  register int ch;

  while (ch = *str)
  {
    outc(ch);
    str++;
  }
}

void out2line(char ref, char row, register char *str) {
   move(row, 0);
   clrtoeol();
   outs(str);
   if(ref) refresh();
}

void eolrange(char start, char end) {
   int i;

   if(start < 0) start = 0;
   if(end < 0) end = 0;
   if(start > b_lines) start = b_lines;
   if(end > b_lines) end = b_lines;
   if(end < start) return;

   for(i = start ; i <= end ; i++) out2line(1, i, "");

   return;
}

#ifdef SHOW_USER_IN_TEXT

/* Thor: ¥i¥H¨q¥X user ªº name ©M nick */

xouts(str)
  register char *str;
{
  register char *t_name = cuser.userid;
  register char *t_nick = cuser.username;
  register int ch;

  while (ch = *str)
  {
    switch (ch)
    {
    case 1:
      if (ch = *t_name)
        t_name++;
      else
        ch = ' ';
      break;

    case 2:
      if (ch = *t_nick)
        t_nick++;
      else
        ch = ' ';
    }
    outc(ch);
    str++;
  }
}
#endif

outmsg(msg)
  register char *msg;
{
  move(b_lines, 0);
  clrtoeol();
  outs(msg);
}

int pressanykey(va_alist)
va_dcl
{
  va_list ap;
  char msg[128], buf[128], *fmt;
  int ch, i;

  msg[0] = 0;

  va_start(ap);
  fmt = va_arg(ap, char *);
  if(fmt) vsprintf(msg, fmt, ap);
  va_end(ap);

  ch = 41 + rand() % 7;
  do {
   i  = 41 + rand() % 7;
  } while (i == ch);

  if(!msg[0])
     sprintf(buf,
"[37;%2d;1m                        ¡´ ½Ð«ö [33m(Space/Return)[37m Ä~Äò ¡´                        [0m",
     ch);
  else {
      sprintf(buf,
          "[37;%2d;1m  %-60.60s  [37;%2d;1m  Space/Enter  [0m", i, msg, ch);
  }
  outmsg(buf);

  do {
    ch = igetkey();
    if(ch == KEY_ESC && KEY_ESC_arg == 'c')
       capture_screen();
  } while ((ch != ' ') && (ch != KEY_LEFT) && (ch != '\r') && (ch != '\n'));

  out2line(1, b_lines, "");
  return ch;
}

prints(va_alist)
va_dcl
{
  va_list args;
  char buff[512], *fmt;

  va_start(args);
  fmt = va_arg(args, char *);
  vsprintf(buff, fmt, args);
  va_end(args);
  outs(buff);
}

scroll()
{
  if (!visiop)
  {
    outc('\n');
  }
  else
  {
    scrollcnt++;
    if (++roll >= t_lines)
      roll = 0;
    move(b_lines, 0);
    clrtoeol();
  }
}


rscroll()
{
  if (!visiop)
  {
    outs("\n\n");
  }
  else
  {
    scrollcnt--;
    if (--roll < 0)
      roll = b_lines;
    move(0, 0);
    clrtoeol();
  }
}


static int old_col, old_ln, old_roll;


vsave_cursor()
{
  old_col = cur_col;
  old_ln = cur_ln;
}


vrestore_cursor()
{
  move(old_ln, old_col);
}


save_foot(slp)
  screenline *slp;
{
  vsave_cursor();

  move(b_lines - 1, 0);
  memcpy(slp, cur_slp, sizeof(screenline) * 2);
  slp[0].smod = 0;
  slp[0].mode |= SL_MODIFIED;
  slp[1].smod = 0;
  slp[1].mode |= SL_MODIFIED;
}


restore_foot(slp)
  screenline *slp;
{
  move(b_lines - 1, 0);
  memcpy(cur_slp, slp, sizeof(screenline) * 2);
  vrestore_cursor();
  refresh();
}


save_screen(slp)
  screenline *slp;
{
  vsave_cursor();
  old_roll = roll;
  memcpy(slp, visiop, sizeof(screenline) * t_lines);
}

restore_screen(slp)
  screenline *slp;
{
  memcpy(visiop, slp, sizeof(screenline) * t_lines);
  vrestore_cursor();
  roll = old_roll;
  redoscr();
}

int vans(char *msg) {
   move(b_lines-1, 2);
   outs(msg);
   return igetkey();
}

char * Etime(time_t *clock) {
  strftime(datemsg, 22, "%D %T %a", localtime(clock));
  return (datemsg);
}

reset_tty()
{
}

restore_tty()
{
}
