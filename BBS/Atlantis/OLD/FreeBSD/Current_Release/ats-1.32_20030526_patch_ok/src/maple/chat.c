/*-------------------------------------------------------*/
/* chat.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : chat client for xchatd                       */
/* create : 95/03/29                                     */
/* update : 02/07/28 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"

#ifdef lint
#include <sys/uio.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXLASTCMD 6
char chatroom[IDLEN];           /* Chat-Room Name */
int stop_line;                  /* next line of bottom of message window area */
static FILE* flog;
/* woju chatid prompt, for call back...needs longer */
int chatid_len = 10;

extern char page_requestor[];
extern char *modestring();
extern char *Cdate();

int chatline;

void get_msg_1(void) {
   DL_func("bin/message.so:get_msg", 1);
}

void printchatline(char *str) {
  if (*str == '*')
     bell();

  move(chatline, 0);
  if (*str == '@') {
     char *id, *msg;
     char buf[100];
     char fpath[100];
     FILE* fp;
     user_info *uentp;
     int tuid;
     extern cmpuids();

     id = str + 1;
     (msg = strchr(id, '@') + 1)[-1] = 0;
     if ((tuid = searchuser(id))  && tuid != usernum &&
         (uentp = (user_info *) search_ulistn(cmpuids, tuid, 1)) &&
         uentp->mode != XMODE && !(is_rejected(uentp) & 2)) {
        sprintf(buf, "¡¹ %s ¤£¦b²á¤Ñ«Ç¡Aµo°e¤ô²y¡¹\n", id);
        outs(buf);
        strcpy(buf + 1, msg);
        *buf = 0;
        my_write(uentp->pid, buf);
        sprintf(buf, "%s¡¹ %s", id, msg);
        strcpy(str, buf);
     }
     else {
        sprintf(buf, "msg fail: %s ¤£¦b¯¸¤W\n", id);
        outs(buf);
     }
  }
  else if (*str == '>' && !PERM_HIDE(currutmp))
     return;
  else if (chatline < stop_line - 1) {
     chatline++;
  }
  else {
     region_scroll_up(2, stop_line - 2);
     move(stop_line - 2, 0);
  }
  outs(str);
  outc('\n');
  outs("¡÷");

  if (flog)
     fprintf(flog, "%s\n", str);
}


static void
chat_clear()
{
  for (chatline = 2; chatline < stop_line; chatline++)
  {
    move(chatline, 0);
    clrtoeol();
  }
  move(b_lines, 0);
  clrtoeol();
  move(chatline = 2, 0);
  outs("¡÷");
}


static void
print_chatid(chatid)
  char *chatid;
{
  move(b_lines - 1, 0);
  clrtoeol();
  outs(chatid);
  outc(':');
}


static int
chat_send(fd, buf)
  int fd;
  char *buf;
{
  int len;
  char genbuf[200];

  sprintf(genbuf, "%s\n", buf);
  len = strlen(genbuf);
  return (send(fd, genbuf, len, 0) == len);
}


static int
chat_recv(fd, chatid)
  int fd;
  char *chatid;
{
  static char buf[512];
  static int bufstart = 0;
  char genbuf[200];
  int c, len;
  char *bptr;

  len = sizeof(buf) - bufstart - 1;
  if ((c = recv(fd, buf + bufstart, len, 0)) <= 0)
    return -1;
  c += bufstart;

  bptr = buf;
  while (c > 0)
  {
    len = strlen(bptr) + 1;
    if (len > c && len < (sizeof buf / 2))
      break;

    if (*bptr == '/')
    {
      switch (bptr[1])
      {
      case 'c':
        chat_clear();
        break;

      case 'n':
        strncpy(chatid, bptr + 2, 8);
        print_chatid(chatid);
        clrtoeol();
        break;

      case 'r':
        strncpy(chatroom, bptr + 2, IDLEN - 1);
        break;

      case 't':
        move(0, 0);
        clrtoeol();
        sprintf(genbuf, "½Í¤Ñ«Ç [%s]", chatroom);
        prints("[1;37;46m %-21s [45m ¸ÜÃD¡G%-48s[0m", genbuf, bptr + 2);
      }
    }
    else
      printchatline(bptr);

    c -= len;
    bptr += len;
  }

  if (c > 0)
  {
    strcpy(genbuf, bptr);
    strcpy(buf, genbuf);
    bufstart = len - 1;
  }
  else
    bufstart = 0;
  return 0;
}


static int
printuserent(uentp)
  user_info *uentp;
{
  static char uline[80];
  static int cnt;
  char pline[30];

  if (!uentp)
  {
    if (cnt)
      printchatline(uline);
    bzero(uline, 80);
    cnt = 0;
    return 0;
  }
  if (!HAS_PERM(PERM_SYSOP) && !HAS_PERM(PERM_SEECLOAK) && uentp->invisible)
    return 0;

#if 0
  if (kill(uentp->pid, 0) == -1)
    return 0;
#endif

  sprintf(pline, "%-13s%c%-10s ", uentp->userid, uentp->invisible ? '#' : ' ',
    modestring(uentp, 1));
  if (cnt < 2)
    strcat(pline, "¢x");
  strcat(uline, pline);
  if (++cnt == 3)
  {
    printchatline(uline);
    memset(uline, 0, 80);
    cnt = 0;
  }
  return 0;
}


static void
chathelp(cmd, desc)
  char *cmd, *desc;
{
  char buf[STRLEN];

  sprintf(buf, "  %-20s- %s", cmd, desc);
  printchatline(buf);
}


static void
chat_help(arg)
  char *arg;
{
  if (strstr(arg, " op"))
  {
    printchatline("½Í¤Ñ«ÇºÞ²z­û±M¥Î«ü¥O");
    chathelp("[/f]lag [+-][ls]", "³]©wÂê©w¡B¯µ±Kª¬ºA");
    chathelp("[/i]nvite <id>", "ÁÜ½Ð <id> ¥[¤J½Í¤Ñ«Ç");
    chathelp("[/k]ick <id>", "±N <id> ½ð¥X½Í¤Ñ«Ç");
    chathelp("[/o]p <id>", "±N Op ªºÅv¤OÂà²¾µ¹ <id>");
    chathelp("[/t]opic <text>", "´«­Ó¸ÜÃD");
    chathelp("[/w]all", "¼s¼½ (¯¸ªø±M¥Î)");
    printchatline("");
  }
  else
  {
    chathelp("[//]help", "MUD-like ªÀ¥æ°Êµü");
    chathelp("[/h]elp op", "½Í¤Ñ«ÇºÞ²z­û±M¥Î«ü¥O");
    chathelp("[/a]ct <msg>", "°µ¤@­Ó°Ê§@");
    chathelp("[/b]ye [msg]", "¹D§O");
    chathelp("[/c]lear", "²M°£¿Ã¹õ");
    chathelp("[/j]oin <room>", "«Ø¥ß©Î¥[¤J½Í¤Ñ«Ç");
    chathelp("[/l]ist [room]", "¦C¥X½Í¤Ñ«Ç¨Ï¥ÎªÌ");
    chathelp("[/m]sg <id> <msg>", "¸ò <id> »¡®¨®¨¸Ü");
    chathelp("[/n]ick <id>", "±N½Í¤Ñ¥N¸¹´«¦¨ <id>");
    chathelp("[/p]ager", "¤Á´«©I¥s¾¹");
    chathelp("[/q]uery", "¬d¸ßºô¤Í");
    chathelp("[/r]oom", "¦C¥X¤@¯ë½Í¤Ñ«Ç");
    chathelp("[/u]sers", "¦C¥X¯¸¤W¨Ï¥ÎªÌ");
    chathelp("[/w]ho", "¦C¥X¥»½Í¤Ñ«Ç¨Ï¥ÎªÌ");
    chathelp("[/w]hoin <room>", "¦C¥X½Í¤Ñ«Ç<room> ªº¨Ï¥ÎªÌ");
    printchatline("");
  }
}


static void
chat_date()
{
  time_t thetime;
  char genbuf[200];

  time(&thetime);
  sprintf(genbuf, "¡» %s¼Ð·Ç®É¶¡: %s", BoardName, Cdate(&thetime));
  printchatline(genbuf);
}


static void
chat_pager()
{
  char genbuf[200];

  char *msgs[] = {"Ãö³¬", "¥´¶}", "©Þ±¼", "¨¾¤ô"};
  sprintf(genbuf, "¡» ±zªº©I¥s¾¹¤w¸g%s¤F!", msgs[currutmp->pager=(currutmp->pager+1)%4]);
  printchatline(genbuf);
}


static void
chat_query(arg)
  char *arg;
{
  char *uid;
  int tuid;

  printchatline("");
  strtok(arg, str_space);
  if ((uid = strtok(NULL, str_space)) && (tuid = getuser(uid)))
  {
    char buf[128], *ptr;
    FILE *fp;

    sprintf(buf, "%s(%s) ¦@¤W¯¸ %d ¦¸¡Aµoªí¹L %d ½g¤å³¹",
      xuser.userid, xuser.username, xuser.numlogins, xuser.numposts);
    printchatline(buf);

    sprintf(buf, "³Ìªñ(%s)±q[%s]¤W¯¸", Cdate(&xuser.lastlogin),
      (xuser.lasthost[0] ? xuser.lasthost : "(¤£¸Ô)"));
    printchatline(buf);

    sethomefile(buf, xuser.userid, fn_plans);
    if (fp = fopen(buf, "r"))
    {
      tuid = 0;
      while (tuid++ < MAXQUERYLINES && fgets(buf, 128, fp))
      {
        if (ptr = strchr(buf, '\n'))
          ptr[0] = '\0';
        printchatline(buf);
      }
      fclose(fp);
    }
  }
  else
    printchatline(err_uid);
}


static void
chat_users()
{
  printchatline("");
  printchatline("¡i " BOARDNAME "ªº¹C«È¦Cªí ¡j");
  printchatline(msg_shortulist);

  if (apply_ulist(printuserent) == -1)
  {
    printchatline("ªÅµL¤@¤H");
  }
  printuserent(NULL);
}


struct chat_command
{
  char *cmdname;                /* Char-room command length */
  void (*cmdfunc) ();           /* Pointer to function */
};


struct chat_command chat_cmdtbl[] = {
  {"help", chat_help},
  {"clear", chat_clear},
  {"date", chat_date},
  {"pager", chat_pager},
  {"query", chat_query},
  {"users", chat_users},
  {NULL, NULL}
};


static int
chat_cmd_match(buf, str)
  char *buf;
  char *str;
{
  while (*str && *buf && !isspace(*buf))
  {
    if (tolower(*buf++) != *str++)
      return 0;
  }
  return 1;
}


static int
chat_cmd(buf, fd)
  char *buf;
  int fd;
{
  int i;

  if (*buf++ != '/')
    return 0;

  for (i = 0; chat_cmdtbl[i].cmdname; i++)
  {
    if (chat_cmd_match(buf, chat_cmdtbl[i].cmdname))
    {
      chat_cmdtbl[i].cmdfunc(buf);
      return 1;
    }
  }
  return 0;
}


int
t_chat()
{
  char inbuf[80], chatid[20], lastcmd[MAXLASTCMD][80], *ptr, inbuf0[80];
  char chathost[80];
  struct sockaddr_in sin;
  struct hostent *h;
  int cfd, cmdpos, ch;
  int currchar, currchar0;
  int newmail;
  extern int dumb_term;
  int page_pending = NA;
  int chatting = YEA;
  char fpath[80];
  FILE* fp;
  char genbuf[200], ver[3];
  int phone_mode = 0;
  char* pstr;
  extern char* phone_char();
  int keydown, dirty = 0, i;


  outmsg("¥i¥´¤J: freebsd(¹w³])¡Bsob¡Bmiou¡Bgod¡Bcivil¡Bimbbs");
/*  if (!getdata(b_lines - 1, 0, "½Ð¿é¤J²á¤Ñ«Ç¦a§}: ", inbuf, 30, DOECHO, 0)) */
     strcpy(inbuf, MYHOSTNAME);

  move(b_lines, 0);
  clrtoeol();
  if (!(h = gethostbyname(inbuf)))
  {
    perror("gethostbyname");
    return -1;
  }
  memset(&sin, 0, sizeof sin);
  sin.sin_family = PF_INET;
  /* sin.sin_family = h->h_addrtype; */
  memcpy(&sin.sin_addr, h->h_addr, h->h_length);
  /*
  getdata(b_lines - 1, 0, "ÂÂ²á¤Ñ«Ç(O) / ·s²á¤Ñ«Ç(N) [O]", ver, 3, LCECHO, 0);
  */
  ver[0] == 'o';
  sin.sin_port = (*ver == 'n') ? htons(CHATPORT) : CHATPORT;
  cfd = socket(sin.sin_family, SOCK_STREAM, 0);

  if (connect(cfd, (struct sockaddr *) & sin, sizeof sin))
  {
    close(cfd);

    switch (ch = fork())
    {
    case -1:
      break;

    case 0:
      execl("bin/xchatd", "xchatd", NULL);
      exit(1);

    default:
      /* The chat daemon forks so we can wait on it here. */
      waitpid(ch, NULL, 0);
      gethostname(inbuf, STRLEN);
      if (!(h = gethostbyname(inbuf)))
      {
        perror("gethostbyname");
        return -1;
      }
      memcpy(&sin.sin_addr, h->h_addr, h->h_length);
    }

    cfd = socket(sin.sin_family, SOCK_STREAM, 0);
    if ((connect(cfd, (struct sockaddr *) & sin, sizeof sin)))
    {
      perror("connect failed");
      return -1;
    }
  }
  outmsg(strcpy(chathost, inbuf));

  while (1)
  {
    if (!getdata(b_lines - 1, 0, "½Ð¿é¤J²á¤Ñ¥N¸¹¡G", chatid, 9, DOECHO, cuser.userid))
       return 0;
    chatid[8] = '\0';

    sprintf(inbuf, "/! %d %d %s %s", usernum,
      currutmp->userlevel, cuser.userid, chatid);
    chat_send(cfd, inbuf);
    if (recv(cfd, inbuf, 3, 0) != 3)
    {
      return 0;
    }
    if (!strcmp(inbuf, CHAT_LOGIN_OK))
      break;
    else if (!strcmp(inbuf, CHAT_LOGIN_EXISTS))
      ptr = "³o­Ó¥N¸¹¤w¸g¦³¤H¥Î¤F";
    else if (!strcmp(inbuf, CHAT_LOGIN_INVALID))
      ptr = "³o­Ó¥N¸¹¬O¿ù»~ªº";

    move(b_lines - 2, 0);
    outs(ptr);
    clrtoeol();
    bell();
  }

  add_io(cfd, 0);

  newmail = currchar = 0;
  cmdpos = MAXLASTCMD -1;
  memset(lastcmd, 0, MAXLASTCMD * 80);

  setutmpmode(CHATING);
  currutmp->in_chat = YEA;

  if (*chatid == KEY_ESC)
     strcpy(chatid, chatid + 1);
  strcpy(currutmp->chatid, chatid);

  clear();
  chatline = 2;
  strcpy(inbuf, chatid);
  stop_line = t_lines - 3;


  move(stop_line, 0);
  outs(msg_seperator);
  move(1, 0);
  outs(msg_seperator);
  print_chatid(chatid);
  memset(inbuf, 0, 80);

/*
woju
*/
  sethomepath(fpath, cuser.userid);
  strcpy(fpath, tempnam(fpath, "chat_"));
  flog = fopen(fpath, "w");

  outmsg(chathost);


  while (chatting)
  {
    move(b_lines - 1, currchar + chatid_len);
    ch = igetkey();
    keydown = 0;

    switch (ch)
    {
    case KEY_DOWN:
      keydown = 1;

    case KEY_UP:
      if (*inbuf && dirty) {
         for (i = MAXLASTCMD - 1; i; i--)
            strcpy(lastcmd[i], lastcmd[i - 1]);
         strcpy(lastcmd[0], inbuf);
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
               || !strcmp(inbuf, lastcmd[cmdpos])));
      if (cmdpos == i)
         continue;

      strcpy(inbuf, lastcmd[cmdpos]);
      move(b_lines - 1, chatid_len);
      clrtoeol();
      outs(inbuf);
      currchar = strlen(inbuf);
      dirty = 0;
      continue;

    case KEY_LEFT:
      if (currchar)
        --currchar;
      continue;

    case KEY_RIGHT:
      if (inbuf[currchar])
        ++currchar;
      continue;
    }

    if (!newmail && chkmail(0))
    {
      newmail = 1;
      printchatline("¡» ¾´¡I¶l®t¤S¨Ó¤F...");
    }

    if (ch == I_OTHERDATA)      /* incoming */
    {
      if (chat_recv(cfd, chatid) == -1) {
        chatting = chat_send(cfd, "/b");
        break;
      }
      continue;
    }

    if (phone_mode && (pstr = phone_char(ch)) || isprint2(ch))
    {
      if (currchar + (phone_char && pstr) < 68)
      {
        if (inbuf[currchar])
        {                       /* insert */
          int i;

          for (i = currchar; inbuf[i] && i + (phone_mode && pstr) < 68; i++)
             ;
          inbuf[i + 1 + (phone_mode && pstr)] = '\0';
          for (; i > currchar; i--)
            inbuf[i + (phone_mode && pstr)] = inbuf[i - 1];
        }
        else
        {                       /* append */
          inbuf[currchar + 1 + (phone_mode && pstr)] = '\0';
        }
        if (phone_mode && pstr) {
           inbuf[currchar] = pstr[0];
           inbuf[currchar + 1] = pstr[1];
        }
        else
           inbuf[currchar] = ch;
        move(b_lines - 1, currchar + chatid_len);
        outs(&inbuf[currchar++]);
        currchar += phone_mode && pstr;
        dirty = 1;
      }
      continue;
    }

    if (ch == '\n' || ch == '\r')
    {
      if (*inbuf)
      {
        chatting = chat_cmd(inbuf, cfd);
        if (chatting == 0)
          chatting = chat_send(cfd, inbuf);
        if (!strncmp(inbuf, "/b", 2))
          break;

        for (cmdpos = MAXLASTCMD - 1; cmdpos; cmdpos--)
          strcpy(lastcmd[cmdpos], lastcmd[cmdpos - 1]);
        strcpy(lastcmd[0], inbuf);

        inbuf[0] = '\0';
        currchar = 0;
        cmdpos = -1;
        dirty = 0;
      }
      print_chatid(chatid);
      move(b_lines - 1, chatid_len);
      continue;
    }


    if (ch == KEY_ESC)
       switch (KEY_ESC_arg) {
       case 'p':
          phone_mode ^= 1;
          continue;
       case 'n':
          add_io(0, 0);
          scr_exec(edit_note);
          add_io(cfd, 0);
          continue;
       case 'c':
          capture_screen();
          continue;
       case '0':
       case '5':
       case '6':
       case '7':
       case '8':
       case '9': {
          FILE *fp;
          char fp_tmpbuf[200];
          char tmpfname[] = "buf.0";
          int i;

          tmpfname[4] = KEY_ESC_arg;
          setuserfile(fp_tmpbuf, tmpfname);
          if (fp = fopen(fp_tmpbuf, "r")) {
             for (i = 0; i < 15 && fgets(fp_tmpbuf, 200, fp) && chatting; i++) {
                fp_tmpbuf[strlen(fp_tmpbuf) - 1] = 0;
                chatting = chat_send(cfd, fp_tmpbuf);
                if (ch == I_OTHERDATA)      /* incoming */
                {
                  if (chat_recv(cfd, chatid) == -1) {
                    chatting = chat_send(cfd, "/b");
                    break;
                  }
                  continue;
                }
             }
             fclose(fp);
          }
          continue;
          }
       }


    if (ch == Ctrl('H') || ch == '\177')
    {
      if (currchar)
      {
        currchar--;
        inbuf[69] = '\0';
        memcpy(&inbuf[currchar], &inbuf[currchar + 1], 69 - currchar);
        move(b_lines - 1, currchar + chatid_len);
        clrtoeol();
        outs(&inbuf[currchar]);
        dirty = 1;
      }
      continue;
    }
    if (ch == Ctrl('Z') || ch == Ctrl('Y'))
    {
      inbuf[0] = '\0';
      currchar = 0;
      print_chatid(chatid);
      move(b_lines - 1, chatid_len);
      continue;
    }

    if (ch == Ctrl('C'))
    {
      chat_send(cfd, "/b");
      break;
    }
/*
woju
*/
    if (ch == Ctrl('D'))
    {
      if (currchar < strlen(inbuf))
      {
        inbuf[69] = '\0';
        memcpy(&inbuf[currchar], &inbuf[currchar + 1], 69 - currchar);
        move(b_lines - 1, currchar + chatid_len);
        clrtoeol();
        outs(&inbuf[currchar]);
        dirty = 1;
      }
      continue;
    }
    if (ch == Ctrl('K')) {
       inbuf[currchar] = 0;
       move(b_lines - 1, currchar + chatid_len);
       clrtoeol();
       dirty = 1;
       continue;
    }
    if (ch == Ctrl('A')) {
       currchar = 0;
       continue;
    }
    if (ch == Ctrl('E')) {
       currchar = strlen(inbuf);
       continue;
    }
    if (ch == Ctrl('U'))
     {
       add_io(0, 0);
       scr_exec(t_users);
       add_io(cfd, 0);
       continue;
    }

#ifdef CTRL_G_REVIEW
    if(ch == Ctrl('G')) {
       add_io(0, 0);
       scr_exec(get_msg_1);
       add_io(cfd, 0);
       continue;
    }
#endif

    if (ch == Ctrl('B')) {
       add_io(0, 0);
       scr_exec(m_read);
       add_io(cfd, 0);
       continue;
    }

#ifdef TAKE_IDLE
    if (ch == Ctrl('I')) {
       add_io(0, 0);
       scr_exec(t_idle);
       add_io(cfd, 0);
       continue;
    }
#endif

    if (ch == Ctrl('R')) {
       extern screenline* big_picture;
       screenline* screen0 = calloc(t_lines, sizeof(screenline));

       memcpy(screen0, big_picture, t_lines * sizeof(screenline));
       add_io(0, 0);
       DL_func("bin/message.so:get_msg", 0);
       add_io(cfd, 0);
       continue;
    }

    if (ch == Ctrl('Q')) {
      print_chatid(chatid);
      move(b_lines - 1, chatid_len);
      outs(inbuf);
      continue;
    }
  }

  close(cfd);
  add_io(0, 0);
  currutmp->in_chat = currutmp->chatid[0] = 0;


  if (flog) {
     char ans[4];

     fclose(flog);
     more(fpath, NA);
     getdata(b_lines - 1, 0, "²M°£(C) ²¾¦Ü³Æ§Ñ¿ý(M) (C/M)?[C] ",
        ans, 4, LCECHO, 0);
     if (*ans == 'm') {
        fileheader mymail;
        char title[128];

        sethomepath(genbuf, cuser.userid);
        stampfile(genbuf, &mymail);
        mymail.savemode = 'H';        /* hold-mail flag */
        mymail.filemode = FILE_READ;
        strcpy(mymail.owner, "[³Æ.§Ñ.¿ý]");
        strcpy(mymail.title, "·|Ä³[1;33m°O¿ý[m");
        sethomedir(title, cuser.userid);
        append_record(title, &mymail, sizeof(mymail));
        Rename(fpath, genbuf);
     }
     else
        unlink(fpath);
  }

  return 0;
}
