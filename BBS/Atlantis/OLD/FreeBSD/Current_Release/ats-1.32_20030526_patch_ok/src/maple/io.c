/*-------------------------------------------------------*/
/* io.c         ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : basic console/screen/keyboard I/O routines   */
/* create : 95/02/28                                     */
/* update : 02/04/22 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"

#ifdef AIX
#include <sys/select.h>
#endif

#ifdef  Linux
#define OBUFSIZE  (2048)
#define IBUFSIZE  (128)
#else
#define OBUFSIZE  (4096)
#define IBUFSIZE  (256)
#endif

#define INPUT_ACTIVE    0
#define INPUT_IDLE      1

static char outbuf[OBUFSIZE];
static int obufsize = 0;

static char inbuf[IBUFSIZE];
static int ibufsize = 0;
static int icurrchar = 0;

static int i_mode = INPUT_ACTIVE;

extern int dumb_term;


/* ----------------------------------------------------- */
/* 定時顯示動態看板                                      */
/* ----------------------------------------------------- */

#ifdef  HAVE_MOVIE
#define STAY_TIMEOUT    (30*60)
extern void movie(int);

static void hit_alarm_clock() {
  static int stay_time = 0;
  static int idle_time = 0;
  time_t now = time(0);

  if(currutmp->pid != currpid) setup_utmp(XMODE); /* 重新配置 shm */

  if(i_mode == INPUT_IDLE) {
    idle_time += MOVIE_INT;
#ifdef IDLE_TIMEOUT
    if(!(PERM_HIDE(currutmp) || currutmp->mode == MAILALL)
        && idle_time > IDLE_TIMEOUT) {
      clear();
      fprintf(stderr, "超過閒置時間！Booting...\n");
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
  currmsg[0] = 0;

#ifdef MOVIE_AND_GOODBABY
  DL_func("bin/goodbaby.so:check_reject_me");
#endif

#ifndef IDLE_TIMEOUT
  if(!chkmail(0)) return;
#endif

  if(chkmail(0) && stay_time > 10 * 60 || stay_time > STAY_TIMEOUT) {
    /* woju */
    int i = rand() % 7;
    time_t now = time(0);
    /* 在這裡提示 user 休息一下 */
    char *msg[7] = {"[報時] 伸伸腰, 揉揉眼, 喝口茶...",
                    "[報時] 您已經打了半個小時囉! 起來活動一下吧!",
                    "[報時] 有沒有人說過你很有魅力?",
                    "[報時] 再坐下去要得痔瘡了啦~~~",
                    "[報時] 有沒有重要的事情忘了處理呢?",
                    "[報時] 用•功\•唸•書",
                    "[報時] 看! 飛碟!!!"};

    /* Dopin */
    sprintf(currmsg, "[1;33;41m<%s> %s[m", Cdate(&now),
            chkmail(0) ? "[提示] 信箱?媮晹釣S看過的信哦!" : msg[i]);

    kill(currpid, SIGUSR2);
    stay_time = 0;

#ifndef MOVIE_AND_GOODBABY
    DL_func("bin/goodbaby.so:check_reject_me");
#endif
  }

  /* Dopin 10/01/01 */
#ifdef IDLE_TIMEOUT
  if(idle_time > IDLE_TIMEOUT - 60) {
    sprintf(currmsg, "[1;5;37;41m警告：您已閒置過久，"
                       "若無回應，系統即將切離！！[m");
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

static void
hit_alarm_clock()
{
  if (HAS_PERM(PERM_NOTIMEOUT) || PERM_HIDE(currutmp) || currstat == MAILALL)
    return;
  if (i_mode == INPUT_IDLE)
  {
    clear();
    fprintf(stderr, "超過閒置時間！Booting...\n");
    kill(currpid, SIGHUP);
  }
  i_mode = INPUT_IDLE;
#ifdef IDLE_TIMEOUT
  alarm(IDLE_TIMEOUT);
#endif
}


void
init_alarm()
{
  signal(SIGALRM, hit_alarm_clock);
#ifdef IDLE_TIMEOUT
  alarm(IDLE_TIMEOUT);
#endif
}
#endif                          /* HAVE_MOVIE */


/* ----------------------------------------------------- */
/* output routines                                       */
/* ----------------------------------------------------- */


oflush()
{
  if (obufsize)
  {
    write(1, outbuf, obufsize);
    obufsize = 0;
  }
}


output(s, len)
  char *s;
{
  /* Invalid if len >= OBUFSIZE */

  if (obufsize + len > OBUFSIZE)
  {
    write(1, outbuf, obufsize);
    obufsize = 0;
  }
  memcpy(outbuf + obufsize, s, len);
  obufsize += len;
}


void
ochar(c)
{
  if (obufsize > OBUFSIZE - 1)
  {
    write(1, outbuf, obufsize);
    obufsize = 0;
  }
  outbuf[obufsize++] = c;
}


/* ----------------------------------------------------- */
/* input routines                                        */
/* ----------------------------------------------------- */


static int i_newfd = 0;
static struct timeval i_to, *i_top = NULL;
static int (*flushf) () = NULL;


void
add_io(fd, timeout)
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


void
add_flush(flushfunc)
  int (*flushfunc) ();
{
  flushf = flushfunc;
}


int
num_in_buf()
{
  return icurrchar - ibufsize;
}


int
igetch()
{
  int ch;

  for (;;)
  {
    if (ibufsize == icurrchar)
    {
      fd_set readfds;
      struct timeval to;

      to.tv_sec = to.tv_usec = 0;
      FD_ZERO(&readfds);
      FD_SET(0, &readfds);
      if (i_newfd)
        FD_SET(i_newfd, &readfds);
      if ((ch = select(FD_SETSIZE, &readfds, NULL, NULL, &to)) <= 0)
      {
        if (flushf)
          (*flushf) ();

        if (dumb_term)
          oflush();
        else
          refresh();

        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        if (i_newfd)
          FD_SET(i_newfd, &readfds);

        while ((ch = select(FD_SETSIZE, &readfds, NULL, NULL, i_top)) < 0)
        {
          if (errno == EINTR)
            continue;
          else
          {
            perror("select");
            return -1;
          }
        }
        if (ch == 0)
          return I_TIMEOUT;
      }
      if (i_newfd && FD_ISSET(i_newfd, &readfds))
        return I_OTHERDATA;

      while ((ibufsize = read(0, inbuf, IBUFSIZE)) <= 0)
      {
        if (ibufsize == 0)
          longjmp(byebye, -1);
        if (ibufsize < 0 && errno != EINTR)
          longjmp(byebye, -1);
      }
      icurrchar = 0;
    }

    i_mode = INPUT_ACTIVE;
    ch = inbuf[icurrchar++];
/*
woju
already processed at hit_alarm_clock()
*/
    if (!dumb_term)
       currutmp->uptime = time(0);
    if (ch != Ctrl('L'))
      return (ch);
    redoscr();
  }
}


char* phone_char(char c)
{
   switch (c) {
          case '1':
             return "ㄅ";
          case 'q':
             return "ㄆ";
          case 'a':
             return "ㄇ";
          case 'z':
             return "ㄈ";
          case '2':
             return "ㄉ";
          case 'w':
             return "ㄊ";
          case 's':
             return "ㄋ";
          case 'x':
             return "ㄌ";
          case 'e':
             return "ㄍ";
          case 'd':
             return "ㄎ";
          case 'c':
             return "ㄏ";
          case 'r':
             return "ㄐ";
          case 'f':
             return "ㄑ";
          case 'v':
             return "ㄒ";
          case '5':
             return "ㄓ";
          case 't':
             return "ㄔ";
          case 'g':
             return "ㄕ";
          case 'b':
             return "ㄖ";
          case 'y':
             return "ㄗ";
          case 'h':
             return "ㄘ";
          case 'n':
             return "ㄙ";
          case 'u':
             return "ㄧ";
          case 'j':
             return "ㄨ";
          case 'm':
             return "ㄩ";
          case '8':
             return "ㄚ";
             break;
          case 'i':
             return "ㄛ";
          case 'k':
             return "ㄜ";
          case ',':
             return "ㄝ";
          case '9':
             return "ㄞ";
          case 'o':
             return "ㄟ";
          case 'l':
             return "ㄠ";
          case '.':
             return "ㄡ";
          case '0':
             return "ㄢ";
          case 'p':
             return "ㄣ";
          case ';':
             return "ㄤ";
          case '/':
             return "ㄥ";
          case '-':
             return "ㄦ";
          case '6':
             return "ˊ";
          case '3':
             return "ˇ";
          case '4':
             return "ˋ";
          case '7':
             return "˙";
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
  extern unsigned char scr_cols;
#define MAXLASTCMD 6
  static char lastcmd[MAXLASTCMD][80];

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
    while ((ch = igetch()) != '\r')
    {
      if(ch == '\n') break;
#ifdef CTRL_R_REVIEW
      /* Dopin: 第 0 個位置 且為回訊模式時 如果按 Ctrl('R') */
      if(cuser.extra_mode[3] == 1 && ch == Ctrl('R') &&
         !(cuser.welcomeflag & 2048)) return 255;
#endif

      if(ch == '\177' || ch == Ctrl('H'))
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
    while (move(y, x + currchar), (ch = igetkey()) != '\r') {
       /* woju */
       keydown = 0;
#ifdef CTRL_R_REVIEW
       /* Dopin: 第 0 個位置 且為回訊模式時 如果按 Ctrl('R') */
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
      if(ch == Ctrl('R') && cuser.extra_mode[3] == 1 && !len) return 255;

      /* woju */
      if(ch == Ctrl('R') && !(currutmp->mode == 0 &&
         (currutmp->chatid[0] == 2 || currutmp->chatid[0] == 3))) {
         DL_func("bin/message.so:get_msg", 0);

         continue;
      }

#ifdef TAKE_IDLE
      if(ch == Ctrl('I') && currstat != IDLE &&
          !(currutmp->mode == 0 &&
            (currutmp->chatid[0] == 2 || currutmp->chatid[0] == 3))) {
         scr_exec(t_idle);
         continue;
      }
#endif

      if(ch == Ctrl('B')) {
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
  return clen;
}


/*
woju
*/
#define TRAP_ESC

#ifdef  TRAP_ESC
int KEY_ESC_arg;

int
igetkey()
{
  int mode;
  int ch, last;

  mode = last = 0;
  while (1)
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
  while (1)
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
