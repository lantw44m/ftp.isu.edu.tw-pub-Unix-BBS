/*-------------------------------------------------------*/
/* xchatd.c     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : super KTV daemon for chat server             */
/* create : 95/03/29                                     */
/* update : 02/08/12 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#ifdef LINUX
#include <unistd.h>
#else
#include <sys/uio.h>
#endif

#ifdef AIX
#include <sys/select.h>
#endif

#if !RELIABLE_SELECT_FOR_WRITE
#include <fcntl.h>
#endif

#if USES_SYS_SELECT_H
#include <sys/select.h>
#endif

#if NO_SETPGID
#define setpgid setpgrp
#endif


#define RESTRICTED(u)   (users[(u)].flags == 0) /* guest */
#define SYSOP(u)        (users[(u)].flags & PERM_SYSOP)
#define HIDE(u)     (SYSOP(u) && (users[(u)].flags & PERM_DENYPOST))
#define ROOMOP(u)       (users[(u)].flags & PERM_CHATROOM)

#define ROOM_LOCKED     0x1
#define ROOM_SECRET     0x2

#define LOCKED(r)       (rooms[(r)].flags & ROOM_LOCKED)
#define SECRET(r)       (rooms[(r)].flags & ROOM_SECRET)

#define ROOM_ALL        (-2)

struct chatuser
{
  int sockfd;                   /* socket to bbs server */
  int utent;                    /* utable entry for this user */
  int room;                     /* room: -1 means none, 0 means main */
  int flags;
  char userid[IDLEN + 1];       /* real userid */
  char chatid[9];               /* chat id */
  char ibuf[80];                /* buffer for sending/receiving */
  int ibufsize;                 /* current size of ibuf */
  char host[30];                /* from host */
}        users[MAXACTIVE];


struct chatroom
{
  char name[IDLEN];             /* name of room; room 0 is "main" */
  short occupants;              /* number of users in room */
  short flags;                  /* ROOM_LOCKED, ROOM_SECRET */
  char invites[MAXACTIVE];      /* Keep track of invites to rooms */
  char topic[48];               /* Let the room op to define room topic */
}        rooms[MAXROOM];


struct chatcmd
{
  char *cmdstr;
  void (*cmdfunc) ();
  int exact;
};


int sock = -1;                  /* the socket for listening */
int nfds;                       /* number of sockets to select on */
int num_conns;                  /* current number of connections */
fd_set allfds;                  /* fd set for selecting */
struct timeval zerotv;          /* timeval for selecting */
char chatbuf[256];              /* general purpose buffer */

/* name of the main room (always exists) */

char mainroom[] = "main";
char maintopic[] = BOARDNAME;

char *msg_not_op = "¡» ±z¤£¬O³o¶¡²á¤Ñ«Çªº Op";
char *msg_no_such_id = "¡» ¥Ø«e¨S¦³¤H¨Ï¥Î [%s] ³o­Ó²á¤Ñ¥N¸¹";
char *msg_not_here = "¡» [%s] ¤£¦b³o¶¡²á¤Ñ«Ç";


#undef HAVE_REPORT

#ifdef  HAVE_REPORT
report(s)
  char *s;
{
  static int disable = NA;
  int fd;

  if (disable)
    return;
  if ((fd = open("trace.chatd", O_WRONLY, 0664)) != -1)
  {
    char buf[160];

    flock(fd, LOCK_EX);
    lseek(fd, 0, L_XTND);
    sprintf(buf, "%s\n", s);
    write(fd, buf, strlen(buf));
    flock(fd, LOCK_UN);
    close(fd);
    return;
  }
  disable = YEA;
  return;
}
#else
#define report(s)       ;
#endif


is_valid_chatid(id)
  char *id;
{
  int i, ch;

/*
woju
*/
  if (*id == '\0' || *id == '/' || *id == '@' || *id == '>')
    return 0;

  for (i = 0; (i < 8) && (ch = id[i]); i++)
    if (ch == '*' || ch == ':')
      return 0;

  return 1;
}


int
Isspace(ch)
  char ch;
{
  return (ch == ' ' || ch == '\t' || ch == 10);
}


char *
nextword(str)
  char **str;
{
  char *p;

  while (Isspace(**str))
    (*str)++;
  p = *str;
  while (**str && !Isspace(**str))
    (*str)++;
  if (**str)
  {
    **str = '\0';
    (*str)++;
  }
  return p;
}


/* Case Independent strcmp : 1 ==> euqal */

int
ci_strcmp(s1, s2)
  register char *s1, *s2;
{
  register char c1, c2;

  while (1)
  {
    c1 = *s1++;
    if (c1 >= 'A' && c1 <= 'Z')
      c1 |= 32;

    c2 = *s2++;
    if (c2 >= 'A' && c2 <= 'Z')
      c2 |= 32;

    if (c1 != c2)
      return 0;
  }
  return 1;
}


int
chatid_to_indx(chatid)
  char *chatid;
{
  register int i;

  for (i = 0; i < MAXACTIVE; i++)
  {
    if (users[i].sockfd == -1)
      continue;
    if (!strcasecmp(chatid, users[i].chatid))
      return i;
  }
  return -1;
}


int
fuzzy_chatid_to_indx(chatid)
  char *chatid;
{
  register int i, indx = -1;
  int len = strlen(chatid);

  for (i = 0; i < MAXACTIVE; i++)
  {
    if (users[i].sockfd == -1)
      continue;
    if (!strncasecmp(chatid, users[i].chatid, len))
    {
      if (len == strlen(users[i].chatid))
        return i;
      if (indx == -1)
        indx = i;
      else
        return -2;
    }
  }
  return indx;
}


int
roomid_to_indx(roomid)
  char *roomid;
{
  register int i;

  for (i = 0; i < MAXROOM; i++)
  {
    if (i && rooms[i].occupants == 0)
      continue;
    report(roomid);
    report(rooms[i].name);
    if (!strcasecmp(roomid, rooms[i].name))
      return i;
  }
  return -1;
}


void
do_send(writefds, str)
  fd_set *writefds;
  char *str;
{
  register int i;
  int len = strlen(str);

  if (select(nfds, NULL, writefds, NULL, &zerotv) > 0)
  {
    for (i = 0; i < nfds; i++)
      if (FD_ISSET(i, writefds))
        send(i, str, len + 1, 0);
  }
}


void
send_to_room(room, str)
  int room;
  char *str;
{
  int i;
  fd_set writefds;

  FD_ZERO(&writefds);
  for (i = 0; i < MAXROOM; i++)
  {
    if (users[i].sockfd == -1)
      continue;
    if (room == ROOM_ALL || room == users[i].room)
      /* write(users[i].sockfd, str, strlen(str) + 1); */
      FD_SET(users[i].sockfd, &writefds);
  }
  do_send(&writefds, str);
}

void
send_to_room_hide(room, str1)
  int room;
  char *str1;
{
  int i;
  fd_set writefds;
  char str[200];

  sprintf(str, ">%s", str1);
  FD_ZERO(&writefds);
  for (i = 0; i < MAXROOM; i++)
  {
    if (users[i].sockfd == -1 || !HIDE(i))
       continue;
    if (room == ROOM_ALL || room == users[i].room)
       FD_SET(users[i].sockfd, &writefds);
  }
  do_send(&writefds, str);
}



void
send_to_unum(unum, str)
  int unum;
  char *str;
{
  fd_set writefds;

  FD_ZERO(&writefds);
  FD_SET(users[unum].sockfd, &writefds);
  do_send(&writefds, str);
}

void exit_room(unum, disp, msg)
  int unum;
  int disp;
  char *msg;
{
  int oldrnum = users[unum].room;

  if (oldrnum != -1)
  {
    if (--rooms[oldrnum].occupants)
    {
      sprintf(chatbuf, "%s EXIT", users[unum].userid);
      send_to_room_hide(users[unum].room, chatbuf);
      switch (disp)
      {
      case EXIT_LOGOUT:
        sprintf(chatbuf, "¡» %s Â÷¶}¤F", users[unum].chatid);
        if (msg && *msg)
        {
          strcat(chatbuf, ": ");
          strncat(chatbuf, msg, 80);
        }
        break;

      case EXIT_LOSTCONN:
        sprintf(chatbuf, "¡» %s ¤£¨£¤F... :~(", users[unum].chatid);
        break;

      case EXIT_KICK:
        sprintf(chatbuf, "¡» «¢«¢¡I%s ³Q½ð¥X¥h¤F", users[unum].chatid);
        break;
      }
      if (!HIDE(unum))
         send_to_room(oldrnum, chatbuf);
    }
  }
  users[unum].flags &= ~PERM_CHATROOM;
  users[unum].room = -1;
}


void
chat_topic(unum, msg)
  int unum;
  char *msg;
{
  int rnum;

  rnum = users[unum].room;

  if (!ROOMOP(unum) && !SYSOP(unum))
  {
    send_to_unum(unum, msg_not_op);
    return;
  }
  if (*msg == '\0')
  {
    send_to_unum(unum, "¡° ½Ð«ü©w¸ÜÃD");
    return;
  }


  sprintf(chatbuf, "%s TOPIC", users[unum].userid);
  send_to_room_hide(users[unum].room, chatbuf);
  strncpy(rooms[rnum].topic, msg, 48);
  rooms[rnum].topic[48] = '\0';
  sprintf(chatbuf, "/t%.47s", msg);
  send_to_room(rnum, chatbuf);
  sprintf(chatbuf, "¡» %s ±N¸ÜÃD§ï¬° [1;32m%s[0m", users[unum].chatid, msg);
  send_to_room(rnum, chatbuf);
}

enter_room(int unum, char *room, char *msg) {
  int rnum;
  int op = 0;
  register int i;

  if(!strcmp(users[unum].userid, SUPERVISOR)) {
     op = 1;
  }

  rnum = roomid_to_indx(room);
  if (rnum == -1)
  {
    /* new room */
    for (i = 1; i < MAXROOM; i++)
    {
      if (rooms[i].occupants == 0)
      {
        report("new room");
        rnum = i;
        memset(rooms[rnum].invites, 0, MAXACTIVE);
        strcpy(rooms[rnum].topic, maintopic);
        strncpy(rooms[rnum].name, room, IDLEN - 1);
        rooms[rnum].name[IDLEN - 1] = '\0';
        rooms[rnum].flags = 0;
        op++;
        break;
      }
    }
    if (rnum == -1)
    {
      send_to_unum(unum, "¡° µLªk¦A·sÅP¥]´[¤F");
      return 0;
    }
  }
  if (!SYSOP(unum))
    if (LOCKED(rnum) && rooms[rnum].invites[unum] == 0)
    {
      send_to_unum(unum, "¡° ¤º¦³¤j¥ÕÃT¡A«D½Ð²ö¤J");
      return 0;
    }

  exit_room(unum, EXIT_LOGOUT, msg);
  users[unum].room = rnum;
  if (op || SYSOP(unum))
    users[unum].flags |= PERM_CHATROOM;
  rooms[rnum].occupants++;
  rooms[rnum].invites[unum] = 0;
  sprintf(chatbuf, "%s ENTER", users[unum].userid);
  send_to_room_hide(users[unum].room, chatbuf);
  sprintf(chatbuf, "¡° [32;1m%s[0m ¶i¤J [33;1m[%s][0m ¥]´[",
    users[unum].chatid, rooms[rnum].name);
  if (HIDE(unum))
     send_to_unum(unum, chatbuf);
  else
     send_to_room(rnum, chatbuf);
  sprintf(chatbuf, "/r%s", room);
  send_to_unum(unum, chatbuf);
  sprintf(chatbuf, "/t%s", maintopic);
  send_to_unum(unum, chatbuf);
  return 0;
}


void
logout_user(unum)
{
  int i, sockfd = users[unum].sockfd;

  close(sockfd);
  FD_CLR(sockfd, &allfds);
  memset(&users[unum], 0, sizeof(users[unum]));
  users[unum].sockfd = users[unum].utent = users[unum].room = -1;
  for (i = 0; i < MAXROOM; i++)
  {
    if (rooms[i].invites != NULL)
      rooms[i].invites[unum] = 0;
  }
  num_conns--;
}

print_user_counts(unum)
{
  int i, c, userc = 0, suserc = 0, roomc = 0;

  for (i = 0; i < MAXROOM; i++)
  {
    c = rooms[i].occupants;
    if (i > 0 && c > 0)
    {
      if ((!HIDE(i) || HIDE(unum)) && (!SECRET(i) || SYSOP(unum)))
        roomc++;
    }
    c = users[i].room;
    if (users[i].sockfd != -1 && c != -1)
    {
      if (SECRET(c) && !SYSOP(unum))
        suserc++;
      else if (!HIDE(c) || HIDE(unum))
        userc++;
    }
  }
  sprintf(chatbuf,
    "¡ó Åwªï¥úÁ{¡i«_ÀI¤½·|¡j¡A¥Ø«e¶}¤F [1;31m%d[0m ¶¡¥]´[", roomc + 1);
  send_to_unum(unum, chatbuf);
  sprintf(chatbuf, "¡ó ¦@¦³ [1;36m%d[0m ¤H¦b±µ«Ý©Ò", userc + 1);
  if (suserc)
    sprintf(chatbuf + strlen(chatbuf), " [%d ¤H¦bÄ³«ÇÆU]", suserc);
  send_to_unum(unum, chatbuf);
  return 0;
}


int
login_user(unum, msg)
  int unum;
  char *msg;
{
  int i, utent, fd = users[unum].sockfd;
  int userlevel;
  char *utentstr;
  char *level;
  char *userid;
  char *chatid;

  utentstr = nextword(&msg);
  level = nextword(&msg);
  userid = nextword(&msg);
  chatid = nextword(&msg);
/*
  if (*chatid == KEY_ESC) {
     strcpy(chatid, chatid + 1);
     if (!strcmp(users[unum].host, MYHOSTNAME))
        userlevel = atoi(level) | PERM_DENYPOST;
     else
        userlevel = 0;
  }
  else
     userlevel = atoi(level) & ~PERM_DENYPOST;
*/
  if (!strcmp(users[unum].host, MYHOSTNAME))
     userlevel = atoi(level) & 1;
  utent = atoi(utentstr);
  for (i = 0; i < MAXACTIVE; i++)
  {
    if (users[i].sockfd != -1 && users[i].utent == utent)
    {
      send_to_unum(unum, CHAT_LOGIN_BOGUS);
      return -1;
    }
  }
  if (!is_valid_chatid(chatid))
  {
    send_to_unum(unum, CHAT_LOGIN_INVALID);
    return 0;
  }
  if (chatid_to_indx(chatid) != -1)
  {
    /* userid in use */
    send_to_unum(unum, CHAT_LOGIN_EXISTS);
    return 0;
  }
  report(level);
  users[unum].utent = utent;
  users[unum].flags = userlevel;
  strcpy(users[unum].userid, userid);
  strncpy(users[unum].chatid, chatid, 8);
  users[unum].chatid[8] = '\0';
  send_to_unum(unum, CHAT_LOGIN_OK);
  print_user_counts(unum);
  enter_room(unum, mainroom, (char *) NULL);
  return 0;
}


void
chat_list_rooms(unum, msg)
  int unum;
  char *msg;
{
  int i, occupants;

  if (RESTRICTED(unum))
  {
    send_to_unum(unum, "¡° ±z¨S¦³Åv­­¦C¥X²{¦³ªº²á¤Ñ«Ç");
    return;
  }
  send_to_unum(unum, "[7m ½Í¤Ñ«Ç¦WºÙ  ¢x¤H¼Æ¢x¸ÜÃD        [0m");
  for (i = 0; i < MAXROOM; i++)
  {
    occupants = rooms[i].occupants;
    if (occupants > 0)
    {
      if (!SYSOP(unum))
        if ((rooms[i].flags & ROOM_SECRET) && (users[unum].room != i))
          continue;
      sprintf(chatbuf, " %-12s¢x%4d¢x%s", rooms[i].name, occupants, rooms[i].topic);
      if (rooms[i].flags & ROOM_LOCKED)
        strcat(chatbuf, " [Âê¦í]");
      if (rooms[i].flags & ROOM_SECRET)
        strcat(chatbuf, " [¯µ±K]");
      send_to_unum(unum, chatbuf);
    }
  }
}


int
chat_do_user_list(unum, msg, whichroom)
  int unum;
  char *msg;
  int whichroom;
{
  int start, stop, curr = 0;
  int i, rnum, myroom = users[unum].room;

  while (*msg && Isspace(*msg))
    msg++;
  start = atoi(msg);
  while (*msg && isdigit(*msg))
    msg++;
  while (*msg && !isdigit(*msg))
    msg++;
  stop = atoi(msg);
  send_to_unum(unum, "[7m ²á¤Ñ¥N¸¹¢x¨Ï¥ÎªÌ¥N¸¹  ¢x²á¤Ñ«Ç [0m");
  for (i = 0; i < MAXROOM; i++)
  {
    rnum = users[i].room;
    if (users[i].sockfd != -1 && rnum != -1)
    {
      if (whichroom != -1 && whichroom != rnum)
        continue;
      if (myroom != rnum)
      {
        if (RESTRICTED(unum))
          continue;
        if ((rooms[rnum].flags & ROOM_SECRET) && !SYSOP(unum))
          continue;
      }
      curr++;
      if (curr < start)
        continue;
      else if (stop && (curr > stop))
        break;
      sprintf(chatbuf, " %-8s¢x%-12s¢x%-15s %c%c%c%c %30s", users[i].chatid,
        users[i].userid, rooms[rnum].name,
        HIDE(i) ? 'H' : ' ',
        SYSOP(i) ? 'S' : ' ',
        ROOMOP(i) ? 'O' : ' ',
        RESTRICTED(i) ? 'R' : ' ',
        users[i].host);
      if (!HIDE(i) || HIDE(unum))
         send_to_unum(unum, chatbuf);
    }
  }
  return 0;
}


void
chat_list_by_room(unum, msg)
  int unum;
  char *msg;
{
  int whichroom;
  char *roomstr;

  roomstr = nextword(&msg);
  if (*roomstr == '\0')
    whichroom = users[unum].room;
  else
  {
    if ((whichroom = roomid_to_indx(roomstr)) == -1)
    {
      sprintf(chatbuf, "¡° ¨S¦³ [%s] ³o­Ó²á¤Ñ«Ç", roomstr);
      send_to_unum(unum, chatbuf);
      return;
    }
    if ((rooms[whichroom].flags & ROOM_SECRET) && !SYSOP(unum))
    {
      send_to_unum(unum, "¡° µLªk¦C¥X¦b¯µ±K²á¤Ñ«Çªº¨Ï¥ÎªÌ");
      return;
    }
  }
  chat_do_user_list(unum, msg, whichroom);
}


void
chat_list_users(unum, msg)
  int unum;
  char *msg;
{
  chat_do_user_list(unum, msg, -1);
}


void
chat_map_chatids(unum, whichroom)
  int unum;
  int whichroom;
{
  int i, c, myroom, rnum;

  myroom = users[unum].room;
  send_to_unum(unum,
    "[7m ²á¤Ñ¥N¸¹ ¨Ï¥ÎªÌ¥N¸¹  ¢x ²á¤Ñ¥N¸¹ ¨Ï¥ÎªÌ¥N¸¹  ¢x ²á¤Ñ¥N¸¹ ¨Ï¥ÎªÌ¥N¸¹ [0m");
  for (i = 0, c = 0; i < MAXROOM; i++)
  {
    rnum = users[i].room;
    if (users[i].sockfd != -1 && rnum != -1)
    {
      if (whichroom != -1 && whichroom != rnum)
        continue;
      if (myroom != rnum)
      {
        if (RESTRICTED(unum))
          continue;
        if ((rooms[rnum].flags & ROOM_SECRET) && !SYSOP(unum))
          continue;
      }
      if (!HIDE(unum) && HIDE(i))
         continue;
      sprintf(chatbuf + (c * 24), " %-8s%c%-12s%s",
        users[i].chatid, (ROOMOP(i)) ? '*' : ' ', users[i].userid,
        (c < 2 ? "¢x" : "  "));
      if (++c == 3)
      {
        send_to_unum(unum, chatbuf);
        c = 0;
      }
    }
  }
  if (c > 0)
    send_to_unum(unum, chatbuf);
}


void
chat_map_chatids_thisroom(unum, msg)
  int unum;
  char *msg;
{
  chat_map_chatids(unum, users[unum].room);
}


void
chat_setroom(unum, msg)
  int unum;
  char *msg;
{
  char *modestr;
  int rnum = users[unum].room;
  int sign = 1;
  int flag;
  char *fstr;

  modestr = nextword(&msg);
  if (!ROOMOP(unum))
  {
    send_to_unum(unum, msg_not_op);
    return;
  }
  if (*modestr == '+')
    modestr++;
  else if (*modestr == '-')
  {
    modestr++;
    sign = 0;
  }
  if (*modestr == '\0')
  {
    send_to_unum(unum,
      "¡° ½Ð«ü©wª¬ºA«ü¼Ð: {[+(³]©w)][-(¨ú®ø)]}{[l(Âê¦í)][s(¯µ±K)]}");
    return;
  }

  while (*modestr)
  {
    flag = 0;
    switch (*modestr)
    {
    case 'l':
    case 'L':
      flag = ROOM_LOCKED;
      fstr = "Âê¦í";
      break;

    case 's':
    case 'S':
      flag = ROOM_SECRET;
      fstr = "¯µ±K";
      break;

    default:
      sprintf(chatbuf, "¡° ª¬ºA«ü¼Ð¿ù»~¡G[%c]", *modestr);
      send_to_unum(unum, chatbuf);
    }

    if (flag && ((rooms[rnum].flags & flag) != sign * flag))
    {
      rooms[rnum].flags ^= flag;
      sprintf(chatbuf, "%s SETROOM", users[unum].userid);
      send_to_room_hide(users[unum].room, chatbuf);
      sprintf(chatbuf, "¡° ¥»²á¤Ñ«Ç³Q %s %s%sª¬ºA",
        users[unum].chatid, sign ? "³]©w¬°" : "¨ú®ø", fstr);
      send_to_room(rnum, chatbuf);
    }
    modestr++;
  }
}


void
chat_nick(unum, msg)
  int unum;
  char *msg;
{
  char *chatid;
  int othernum;

  chatid = nextword(&msg);
  if (!is_valid_chatid(chatid))
  {
    send_to_unum(unum, "¡° ³o­Ó²á¤Ñ¥N¸¹¬O¤£¥¿½Tªº");
    return;
  }
  othernum = chatid_to_indx(chatid);
  if (othernum != -1 && othernum != unum)
  {
    send_to_unum(unum, "¡° ¤w¸g¦³¤H±¶¨¬¥ýµnÅo");
    return;
  }
  chatid[8] = '\0';

  sprintf(chatbuf, "%s NICK", users[unum].userid);
  send_to_room_hide(users[unum].room, chatbuf);
  sprintf(chatbuf, "¡° %s ±N²á¤Ñ¥N¸¹§ï¬° [1;33m%s[0m",
    users[unum].chatid, chatid);
  if (!HIDE(unum))
     send_to_room(users[unum].room, chatbuf);
  strcpy(users[unum].chatid, chatid);
  sprintf(chatbuf, "/n%s", users[unum].chatid);
  send_to_unum(unum, chatbuf);
}


void
chat_private(unum, msg)
  int unum;
  char *msg;
{
  char *recipient;
  int recunum;

  recipient = nextword(&msg);
  recunum = fuzzy_chatid_to_indx(recipient);
  if (recunum < 0)
  {
    /* no such user, or ambiguous */
    if (recunum == -1)
/*
woju
      sprintf(chatbuf, msg_no_such_id, recipient);
*/
      sprintf(chatbuf, "@%s@%s", recipient, msg);
    else
      sprintf(chatbuf, "¡° ½Ð«ü©ú²á¤Ñ¥N¸¹", recipient);
    send_to_unum(unum, chatbuf);
    return;
  }
  if (*msg)
  {
    sprintf(chatbuf, "*%s* ", users[unum].chatid);
    strncat(chatbuf, msg, 80);
    send_to_unum(recunum, chatbuf);
    sprintf(chatbuf, "%s> ", users[recunum].chatid);
    strncat(chatbuf, msg, 80);
    send_to_unum(unum, chatbuf);
  }
}


put_chatid(unum, str)
  int unum;
  char *str;
{
  int i;
  char *chatid = users[unum].chatid;

  memset(str, ' ', 10);
  for (i = 0; chatid[i]; i++)
    str[i] = chatid[i];
  str[i] = ':';
  str[10] = '\0';
}


void
chat_allmsg(unum, msg)
  int unum;
  char *msg;
{
  if (*msg)
  {
    sprintf(chatbuf, "%s ALLMSG", users[unum].userid);
    send_to_room_hide(users[unum].room, chatbuf);
    put_chatid(unum, chatbuf);
    strcat(chatbuf, msg);
    send_to_room(users[unum].room, chatbuf);
  }
}


void
chat_act(unum, msg)
  int unum;
  char *msg;
{
  if (*msg)
  {
    sprintf(chatbuf, "%s ACT", users[unum].userid);
    send_to_room_hide(users[unum].room, chatbuf);
    sprintf(chatbuf, "%s [36m%s[m", users[unum].chatid, msg);
    send_to_room(users[unum].room, chatbuf);
  }
}


void
chat_join(unum, msg)
  int unum;
  char *msg;
{
  if (RESTRICTED(unum))
  {
    send_to_unum(unum, "¡° ±z¨S¦³¥[¤J¨ä¥L²á¤Ñ«ÇªºÅv­­");
  }
  else
  {
    char *roomid = nextword(&msg);

    if (*roomid)
      enter_room(unum, roomid, msg);
    else
      send_to_unum(unum, "¡° ½Ð«ü©w²á¤Ñ«Çªº¦W¦r");
  }
}


void
chat_kick(unum, msg)
  int unum;
  char *msg;
{
  char *twit;
  int rnum = users[unum].room;
  int recunum;

  twit = nextword(&msg);
  if (!ROOMOP(unum) && !SYSOP(unum))
  {
    send_to_unum(unum, msg_not_op);
    return;
  }
  if ((recunum = chatid_to_indx(twit)) == -1 || HIDE(recunum))
  {
    sprintf(chatbuf, msg_no_such_id, twit);
    send_to_unum(unum, chatbuf);
    return;
  }
  if (rnum != users[recunum].room)
  {
    sprintf(chatbuf, msg_not_here, users[recunum].chatid);
    send_to_unum(unum, chatbuf);
    return;
  }
  sprintf(chatbuf, "%s KICK %s", users[unum].userid, users[recunum].userid);
  send_to_room_hide(users[unum].room, chatbuf);
  exit_room(recunum, EXIT_KICK, (char *) NULL);

  if (rnum == 0)
    logout_user(recunum);
  else
    enter_room(recunum, mainroom, (char *) NULL);
}


void
chat_makeop(unum, msg)
  int unum;
  char *msg;
{
  char *newop = nextword(&msg);
  int rnum = users[unum].room;
  int recunum;

  if (!ROOMOP(unum))
  {
    send_to_unum(unum, msg_not_op);
    return;
  }
  if ((recunum = chatid_to_indx(newop)) == -1)
  {
    /* no such user */
    sprintf(chatbuf, msg_no_such_id, newop);
    send_to_unum(unum, chatbuf);
    return;
  }
  if (unum == recunum)
  {
    sprintf(chatbuf, "¡° ±z¦­´N¤w¸g¬O Op ¤F°Ú");
    send_to_unum(unum, chatbuf);
    return;
  }
  if (rnum != users[recunum].room)
  {
    sprintf(chatbuf, msg_not_here, users[recunum].chatid);
    send_to_unum(unum, chatbuf);
    return;
  }
  users[unum].flags &= ~PERM_CHATROOM;
  users[recunum].flags |= PERM_CHATROOM;
  sprintf(chatbuf, "%s MAKEOP %s", users[unum].userid, users[recunum].userid);
  send_to_room_hide(users[unum].room, chatbuf);
  sprintf(chatbuf, "¡° %s ±N Op Åv¤OÂà²¾µ¹ %s", users[unum].chatid,
    users[recunum].chatid);
  send_to_room(rnum, chatbuf);
}


void
chat_invite(unum, msg)
  int unum;
  char *msg;
{
  char *invitee = nextword(&msg);
  int rnum = users[unum].room;
  int recunum;

  if (!ROOMOP(unum))
  {
    send_to_unum(unum, msg_not_op);
    return;
  }
  if ((recunum = chatid_to_indx(invitee)) == -1)
  {
    sprintf(chatbuf, msg_not_here, invitee);
    send_to_unum(unum, chatbuf);
    return;
  }
  if (rooms[rnum].invites[recunum] == 1)
  {
    sprintf(chatbuf, "¡° %s ¤w¸g±µ¨ü¹LÁÜ½Ð¤F", users[recunum].chatid);
    send_to_unum(unum, chatbuf);
    return;
  }
  rooms[rnum].invites[recunum] = 1;
  sprintf(chatbuf, "¡° %s ÁÜ½Ð±z¨ì [%s] ²á¤Ñ«Ç",
    users[unum].chatid, rooms[rnum].name);
  send_to_unum(recunum, chatbuf);
  sprintf(chatbuf, "¡° %s ¦¬¨ì±zªºÁÜ½Ð¤F", users[recunum].chatid);
  send_to_unum(unum, chatbuf);
}


void
chat_broadcast(unum, msg)
  int unum;
  char *msg;
{
  if (!SYSOP(unum))
  {
    send_to_unum(unum, "¡° ±z¨S¦³¦b²á¤Ñ«Ç¼s¼½ªºÅv¤O!");
    return;
  }
  if (*msg == '\0')
  {
    send_to_unum(unum, "¡° ½Ð«ü©w¼s¼½¤º®e");
    return;
  }
  sprintf(chatbuf, "%s BROADCAST", users[unum].userid);
  send_to_room_hide(ROOM_ALL, chatbuf);
  sprintf(chatbuf, "*¡° " BOARDNAME "½Í¤Ñ«Ç¼s¼½¤¤ [%s].....",
    users[unum].chatid);
  send_to_room(ROOM_ALL, chatbuf);
  sprintf(chatbuf, "¡» %s", msg);
  send_to_room(ROOM_ALL, chatbuf);
}


void
chat_goodbye(unum, msg)
  int unum;
  char *msg;
{
  exit_room(unum, EXIT_LOGOUT, msg);
}


/* -------------------------------------------- */
/* MUD-like social commands : action             */
/* -------------------------------------------- */


struct action
{
  char *verb;                   /* °Êµü */
  char *part1_msg;              /* ¤¶µü */
  char *part2_msg;              /* °Ê§@ */
  char *keyword ;
};


struct action party_data[] =
{
  {"1", "®³¥X¹q¿÷¡A´Â", "ªº¨º¸Ì¿÷¤F¤U¥h...", "¢°¸¹"},
  {"2", "¶}©l¹ï", "²r¥´¤âºj¡I", "¢±¸¹"},
  {"3", "¸I¤F¤@Án¡A", "°Ú~~¤F¤@Án¡I", "¢²¸¹"},
  {"4", "®¼°_¾÷ºj¡A§â", "¥´¦¨¸ÁºÛ", "¢³¸¹"},
  {"5", "¾rµÛ¥d¨®¡AµM«á¬ðµM¹ï", "¥kÂà...", "¢´¸¹"},
  {"6", "bon quey ¤F¡I®³°_¤»¸¹½Ä¦V", "", "¢µ¸¹"},
  {"7", "¹ï", "©ß¤F¤@­Ó´A²´¡A§â¥L¬Ý¦º¤F...", "¢¶¸¹"},
  {"8", "«ö¤F pause¡AµM«á¶}©l¹ï", "¯}¤f¤j½|", "¢·¸¹"},
  {"bearhug", "¼ö±¡ªº¾Ö©ê", "", "¼ö±¡¾Ö©ê"},
  {"bless", "¯¬ºÖ", "¤ß·Q¨Æ¦¨", "¯¬ºÖ"},
  {"bow", "²¦°`²¦·qªº¦V", "Áù°`", "Áù°`"},
  {"caress", "»´»´ªº¼¾ºNµÛ", "", "»´¼¾"},
  {"cry", "¦V", "Àz°Þ¤j­ú", "¤j­ú"},
  {"comfort", "·Å¨¥¦w¼¢", "", "¦w¼¢"},
  {"clap", "¦V", "¼ö¯P¹ª´x", "¹ª´x"},
  {"dance", "©Ô¤F", "ªº¤â½¡½¡°_»R", "¸õ»R"},
  {"dogleg", "¹ï", "ª¯»L", "ª¯»L"},
  {"drivel", "¹ïµÛ", "¬y¤f¤ô", "¬y³è²G"},
  {"french", "µ¹", "¤@­Óªk°ê¦¡ªº§k", "ªk°êªº§k"},
  {"giggle", "¹ïµÛ", "¶Ì¶Ìªº§b¯º", "¶Ì¯º"},
  {"grin", "¹ï", "ÅS¥X¨¸´cªº¯º®e", "¦l¯º"},
  {"growl", "¹ï", "©H­ý¤£¤w", "©H­ý"},
  {"hand", "¸ò", "´¤¤â", "´¤¤â"},
  {"hug", "»´»´¦a¾Ö©ê", "", "¾Ö©ê"},
  {"jab", "·Å¬XªºÂWµÛ", "", "ÂW¤H"},
  {"kick", "§â", "½ðªº¦º¥h¬¡¨Ó", "½ð¤H"},
  {"kiss", "µ¹", "¤@­Ó·Å¬Xªº§k", "·Å¬Xªº§k"},
  {"laugh", "¤jÁn¼J¯º", "", "¼J¯º"},
  {"marry", "±·µÛ¤E¦Ê¤E¤Q¤E¦·ª´ºÀ¦V", "¨D±B", "¨D±B"},
  {"nod", "¦V", "ÂIÀYºÙ¬O", "¦P·N"},
  {"nudge", "¥Î¤â¨y³»", "ªºªÎ¨{¤l", "§ðÀ»"},
  {"pad", "»´©ç", "ªºªÓ»H", "©çªÓ"},
  {"pinch", "¥Î¤Oªº§â", "À¾ªº¶Â«C", "¥Î¤OÀ¾¤H"},
  {"punch", "¬½¬½´~¤F", "¤@¹y", "´~¤H"},
  {"shrug", "µL©`¦a¦V", "Áq¤FÁqªÓ»H", "ÁqªÓ"},
  {"sigh", "¹ï", "¼Û¤F¤@¤f®ð", "¼Û®ð"},
  {"slap", "°Ô°Ôªº¤Ú¤F", "¤@¹y¦Õ¥ú", "¥´¦Õ¥ú"},
  {"smooch", "¾Ö§kµÛ", "", "¾Ö§k"},
  {"snicker", "¼K¼K¼K..ªº¹ï", "ÅÑ¯º", "ÅÑ¯º"},
  {"sniff", "¹ï", "¶á¤§¥H»ó", "¤£®h"},
  {"sob", "¹ï", "¼«¼«ªº»¡ You S...un Of B...each¡I", "¶§¥ú¨FÅy"},
  {"spank", "¥Î¤Ú´x¥´", "ªºÁv³¡", "¥´§¾§¾"},
  {"squeeze", "ºòºò¦a¾Ö©êµÛ", "", "ºòºò¾Ö©ê"},
  {"thank", "¦V", "¹DÁÂ", "¹DÁÂ"},
  {"tickle", "©B¼T!©B¼T!·k", "ªºÄo", "·kÄo"},
  {"wave", "¹ïµÛ", "«÷©Rªº´§¤â", "´§¤â"},
  {"wink", "¹ï", "¯«¯µªº¯w¯w²´·ú", "¯w²´"},
  {"zap", "¹ï", "ºÆ¨gªº§ðÀ»", "²r§ð"},
  {NULL, NULL, NULL, NULL}
};


int
party_action(unum, cmd, party)
  int unum;
  char *cmd;
  char *party;
{
  int i;

  for (i = 0; party_data[i].verb; i++)
  {
    if (!strcmp(cmd, party_data[i].verb))
    {
      if (*party == '\0')
      {
        party = "¤j®a";
      }
      else
      {
        int recunum = fuzzy_chatid_to_indx(party);

        if (recunum < 0)
        {
          /* no such user, or ambiguous */
          if (recunum == -1)
            sprintf(chatbuf, msg_no_such_id, party);
          else
            sprintf(chatbuf, "¡° ½Ð«ü©ú²á¤Ñ¥N¸¹", party);
          send_to_unum(unum, chatbuf);
          return 0;
        }
        party = users[recunum].chatid;
      }
      sprintf(chatbuf, "%s PARTY_ACTION", users[unum].userid);
      send_to_room_hide(users[unum].room, chatbuf);
      sprintf(chatbuf, "[1;32m%s [31m%s[33m %s [31m%s[37;0m",
        users[unum].chatid,
        party_data[i].part1_msg, party, party_data[i].part2_msg);
      send_to_room(users[unum].room, chatbuf);
      return 0;
    }
  }
  return 1;
}


/* -------------------------------------------- */
/* MUD-like social commands : speak              */
/* -------------------------------------------- */


struct action speak_data[] =
{
  {"ask", "¸ß°Ý", NULL, "¸ß°Ý"},
  {"chant", "ºq¹|", NULL, "ºq¹|"},
  {"cheer", "³Üªö", NULL, "³Üªö"},
  {"curse", "©G½|", NULL, "©G½|"},
  {"demand", "­n¨D", NULL, "­n¨D"},
  {"frown", "ÂÙ¬Ü", NULL, "ÂÙ¬Ü"},
  {"groan", "©D§u", NULL, "©D§u"},
  {"grumble", "µo¨cÄÌ", NULL, "µo¨cÄÌ"},
  {"helpme", "¤jÁn©I±Ï", NULL, "¤jÁn©I±Ï"},
  {"hum", "³ä³ä¦Û»y", NULL, "³ä³ä¦Û»y"},
  {"moan", "´d¹Ä", NULL, "´d¹Ä"},
  {"notice", "ª`·N", NULL, "ª`·N"},
  {"order", "©R¥O", NULL, "©R¥O"},
  {"ponder", "¨H«ä", NULL, "¨H«ä"},
  {"pout", "äþµÛ¼L»¡", NULL, "äþµÛ¼L»¡"},
  {"pray", "¬èÃ«", NULL, "¬èÃ«"},
  {"request", "Àµ¨D", NULL, "Àµ¨D"},
  {"shout", "¤j¥s", NULL, "¤j¥s"},
  {"sing", "°Ûºq", NULL, "°Ûºq"},
  {"smile", "·L¯º", NULL, "·L¯º"},
  {"smirk", "°²¯º", NULL, "°²¯º"},
  {"swear", "µo»}", NULL, "µo»}"},
  {"tease", "¼J¯º", NULL, "¼J¯º"},
  {"whimper", "¶ã«|ªº»¡", NULL, "»ï«|ªº»¡"},
  {"yell", "¤j³Û", NULL, "¤j³Û"},
  {NULL, NULL, NULL, NULL}
};



int
speak_action(unum, cmd, msg)
  int unum;
  char *cmd;
  char *msg;
{
  int i;

  for (i = 0; speak_data[i].verb; i++)
  {
    if (!strcmp(cmd, speak_data[i].verb))
    {
      sprintf(chatbuf, "%s SPEAK_ACTION", users[unum].userid);
      send_to_room_hide(users[unum].room, chatbuf);
      if (msg[0])
        sprintf(chatbuf, "[1;32m%s [31m%s¡G[33m %s[37;0m",
                users[unum].chatid, speak_data[i].part1_msg, msg);
      else
        sprintf(chatbuf, "[1;32m%s [31m%s[33m[37;0m",
                users[unum].chatid, speak_data[i].part1_msg);
      send_to_room(users[unum].room, chatbuf);
      return 0;
    }
  }
  return 1;
}


/* -------------------------------------------- */
/* MUD-like social commands : condition          */
/* -------------------------------------------- */


struct action condition_data[] =
{
  {"applaud", "°Ô°Ô°Ô°Ô°Ô°Ô°Ô....", NULL, "¹ª´x"},
  {"back", "¦^¨Ó§¤¥¿Ä~Äò¾Ä¾Ô", NULL, "¦^¨Ó"},
  {"blood", "­Ë¦b¦åªy¤§¤¤", NULL, "¥¢¦å"},
  {"blush", "Áy³£¬õ¤F", NULL, "Áy¬õ"},
  {"bokan", "Bo Kan! Bo Kan!", NULL, "®ð¥\\"},
  {"cough", "«y¤F´XÁn", NULL, "«y¹Â"},
  {"die", "¼ÉÀÅ", NULL, "¦º¤`"},
  {"faint", "·í³õ©ü­Ë", NULL, "©ü­Ë"},
  {"haha", "«z«¢«¢«¢«¢«¢«¢«¢«¢~~~~!!!!!", NULL, "«¢«¢«¢"},
  {"happy", "¢ç¢Ï¡I *^_^*", NULL, "¤f­C"},
  {"hurricane", "¢Ö¢÷---¢à£B¢ý--¢Ù¢é¢ö¡I¡I¡I", NULL, "ª@Às®±"},
  {"idle", "§b¦í¤F", NULL, "§b¦í"},
  {"lag", "lllllllaaaaaaaaaaaagggggggggggggg.................", NULL, "¸}"},
  {"puke", "¹Ã¦R¤¤", NULL, "¹Ã¦R"},
  {"room", "r-o-O-m-r-O-¢Ý-Mmm-rR¢à........", NULL, "©Ð¶¡"},
  {"shake", "·n¤F·nÀY", NULL, "·nÀY"},
  {"sleep", "­w¦bÁä½L¤WºÎµÛ¤F¡A¤f¤ô¬y¶iÁä½L¡A³y¦¨·í¾÷¡I", NULL, "ºÎµÛ"},
  {"so", "´NÂæ¤l!!", NULL, "¦p¦¹"},
/*{"strut", "¤j·n¤jÂ\\¦a¨«", NULL},*/
  {"tongue", "¦R¤F¦R¦ÞÀY", NULL, "¦R¦Þ"},
  {"think", "¬nµÛÀY·Q¤F¤@¤U", NULL, "«ä¦Ò"},
  {"wawa", "«z«z«z~~~~~!!!!!  ~~~>_<~~~", NULL, "«z«z«z"},
  {"www", "¨L¨L¨L!!!", NULL, "¥þ²yºô­¶"},
  {NULL, NULL, NULL, NULL}
};



int
condition_action(unum, cmd)
  int unum;
  char *cmd;
{
  int i;

  for (i = 0; condition_data[i].verb; i++)
  {
    if (!strcmp(cmd, condition_data[i].verb))
    {
      sprintf(chatbuf, "%s CONDITION_ACTION", users[unum].userid);
      send_to_room_hide(users[unum].room, chatbuf);
      sprintf(chatbuf, "[1;32m%s [31m%s[0m",
        users[unum].chatid, condition_data[i].part1_msg);
      send_to_room(users[unum].room, chatbuf);
      return 1;
    }
  }
  return 0;
}


/* -------------------------------------------- */
/* MUD-like social commands : help               */
/* -------------------------------------------- */

char *dscrb[] = {
  "[1m¡i Verb + Nick¡G   °Êµü + ¹ï¤è¦W¦r ¡j[36m   ¨Ò¡G//french Kaede[0m",
  "[1m¡i Verb + Message¡G°Êµü + ­n»¡ªº¸Ü ¡j[36m   ¨Ò¡G//sing I swear...[0m",
"[1m¡i Verb¡G°Êµü ¡j    ¡ô¡õ¡GÂÂ¸Ü­«´£[0m", NULL};
struct action *verbs[] = {party_data, speak_data, condition_data, NULL};


#define SCREEN_WIDTH    80
#define MAX_VERB_LEN    10
#define VERB_NO         8


void
view_action_verb(unum, c)
  int unum,
      c;        /* view verbs of class c */
{
  int i, j;
  char *p;
  char *q;

  if(c==0) {
    send_to_unum(unum, "  [//]help 1 - ¦C¥X²Ä¤@Ãþ°Êµü");
    send_to_unum(unum, "  [//]help 2 - ¦C¥X²Ä¤GÃþ°Êµü");
    send_to_unum(unum, "  [//]help 3 - ¦C¥X²Ä¤TÃþ°Êµü");
    send_to_unum(unum, "");
    return ;
  }

  c -- ;
/*  send_to_unum(unum, "/c"); */
  send_to_unum(unum, dscrb[c]);
  chatbuf[0] = '\0';
  i = j = 0;

  p = verbs[0][0].verb ;

  while(p) {
    chatbuf[0] = '\0' ;
    while (p = verbs[c][j].verb)
    {
      j++;
      strcat(chatbuf, p);
      if ((j % VERB_NO) == 0)
      {
        send_to_unum(unum, chatbuf);
        chatbuf[0] = '\0';
        break ;
      }
      else
      {
        strncat(chatbuf, "         ", MAX_VERB_LEN - strlen(p));
      }
    }

    if (j % VERB_NO)
      send_to_unum(unum, chatbuf);

    chatbuf[0] = '\0' ;
    while (p = verbs[c][i].keyword) {
      i ++ ;
      strcat(chatbuf, p);
      if((i % VERB_NO) == 0)
      {
        send_to_unum(unum, chatbuf) ;
        chatbuf[0] = '\0' ;
        break ;
      }
      else
        strncat(chatbuf, "         ", MAX_VERB_LEN - strlen(p));
    }

    if (i % VERB_NO)
      send_to_unum(unum, chatbuf);
  }

  send_to_unum(unum, " ");
}




struct chatcmd chatcmdlist[] =
{
  "act", chat_act, 0,
  "bye", chat_goodbye, 0,
  "flags", chat_setroom, 0,
  "invite", chat_invite, 0,
  "join", chat_join, 0,
  "kick", chat_kick, 0,
  "msg", chat_private, 0,
  "nick", chat_nick, 0,
  "operator", chat_makeop, 0,
  "rooms", chat_list_rooms, 0,
  "whoin", chat_list_by_room, 1,
  "wall", chat_broadcast, 1,

  "who", chat_map_chatids_thisroom, 0,
  "list", chat_list_users, 0,
  "topic", chat_topic, 0,

  NULL, NULL, 0
};


int
command_execute(unum)
  int unum;
{
  char *msg = users[unum].ibuf;
  char *cmd;
  struct chatcmd *cmdrec;
  int match = 0;

  /* Validation routine */
  if (users[unum].room == -1)
  {
    /* MUST give special /! command if not in the room yet */
    if (msg[0] != '/' || msg[1] != '!')
      return -1;
    else
      return (login_user(unum, msg + 2));
  }

  /* If not a /-command, it goes to the room. */
  if (msg[0] != '/')
  {
    chat_allmsg(unum, msg);
    return 0;
  }

  msg++;
  cmd = nextword(&msg);

  if (cmd[0] == '/')
  {

    if ( !strcmp(cmd + 1, "help") || cmd[1]=='\0')
    {
      int c ;

      c = atoi(msg) ;

      switch(c) {
      case 1:
        view_action_verb(unum, 1) ;
        match = 1 ;
        break ;
      case 2:
        view_action_verb(unum, 2) ;
        match = 1;
        break ;
      case 3:
        view_action_verb(unum, 3) ;
        match = 1;
        break ;
      default:
        view_action_verb(unum, 0) ;
        match = 1 ;
      }
    }
    else if (party_action(unum, cmd + 1, msg) == 0)
      match = 1;
    else if (speak_action(unum, cmd + 1, msg) == 0)
      match = 1;
    else
      match = condition_action(unum, cmd + 1);
  }
  else
  {
    for (cmdrec = chatcmdlist; !match && cmdrec->cmdstr; cmdrec++)
    {
      if (cmdrec->exact)
        match = !strcasecmp(cmd, cmdrec->cmdstr);
      else
        match = !strncasecmp(cmd, cmdrec->cmdstr, strlen(cmd));
      if (match)
        cmdrec->cmdfunc(unum, msg);
    }
  }

  if (!match)
  {
    sprintf(chatbuf, "¡» «ü¥O¿ù»~¡G/%s", cmd);
    send_to_unum(unum, chatbuf);
  }
  memset(users[unum].ibuf, 0, sizeof users[unum].ibuf);
  return 0;
}


int
process_chat_command(unum)
  int unum;
{
  register int i;
  int rc, ibufsize;

  if ((rc = recv(users[unum].sockfd, chatbuf, sizeof chatbuf, 0)) <= 0)
  {
    /* disconnected */
    exit_room(unum, EXIT_LOSTCONN, (char *) NULL);
    return -1;
  }
  ibufsize = users[unum].ibufsize;
  for (i = 0; i < rc; i++)
  {
    /* if newline is two characters, throw out the first */
    if (chatbuf[i] == '\r')
      continue;

    /* carriage return signals end of line */
    else if (chatbuf[i] == '\n')
    {
      users[unum].ibuf[ibufsize] = chatbuf[i] = '\0';
      if (command_execute(unum) == -1)
        return -1;
      ibufsize = 0;
    }

    /* add other chars to input buffer unless size limit exceeded */
    else
    {
      if (ibufsize < 79)
        users[unum].ibuf[ibufsize++] = chatbuf[i];
    }
  }
  users[unum].ibufsize = ibufsize;
  return 0;
}


int
main()
{
  struct sockaddr_in sin;
  register int i;
  int pid, sr, newsock, sinsize;
  fd_set readfds;
  struct timeval tv;

  setgid(BBSGID);
  setuid(BBSUID);

  /* ----------------------------- */
  /* init variable : rooms & users */
  /* ----------------------------- */

  strcpy(rooms[0].name, mainroom);
  strcpy(rooms[0].topic, maintopic);

  for (i = 0; i < MAXACTIVE; i++)
  {
    users[i].chatid[0] = '\0';
    users[i].sockfd = users[i].utent = -1;
  }

  /* ------------------------------ */
  /* bind chat server to port       */
  /* ------------------------------ */

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return -1;
  }

  sin.sin_family = AF_INET;
  sin.sin_port = CHATPORT;
  sin.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *) & sin, sizeof(sin)) < 0)
  {
    return -1;
  }

  sinsize = sizeof sin;
  if (getsockname(sock, (struct sockaddr *) & sin, &sinsize) == -1)
  {
    perror("getsockname");
    exit(1);
  }

  if (listen(sock, 5) == -1)
  {
    perror("listen");
    exit(1);
  }

  if (fork())
  {
    exit(0);
  }
  setpgid(0, 0);

  /* ------------------------------ */
  /* trap signals                   */
  /* ------------------------------ */

  signal(SIGHUP, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGALRM, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGURG, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);

  FD_ZERO(&allfds);
  FD_SET(sock, &allfds);
  nfds = sock + 1;

  while(1) {
    memcpy(&readfds, &allfds, sizeof readfds);

    tv.tv_sec = 60 * 30;
    tv.tv_usec = 0;
    if ((sr = select(nfds, &readfds, NULL, NULL, &tv)) < 0)
    {
      if (errno != EINTR)
        sleep(5);
      continue;
    }
    else if (!sr)
      continue;

#if 0
    if(sr == 0) {
      exit(0);                  /* normal chat server shutdown */
    }
    if(tvptr) tvptr = NULL;
#endif

    if (FD_ISSET(sock, &readfds))
    {
      sinsize = sizeof sin;
      newsock = accept(sock, (struct sockaddr *) & sin, &sinsize);
      if (newsock == -1)
      {
        continue;
      }
      for (i = 0; i < MAXACTIVE; i++)
      {
        if (users[i].sockfd == -1)
        {
          struct hostent *hp;
          char *s = users[i].host;

          hp = gethostbyaddr((char *) &sin.sin_addr, sizeof(struct in_addr),
            sin.sin_family);
          strncpy(s, hp ? hp->h_name : (char *) inet_ntoa(sin.sin_addr), 30);
          s[29] = 0;
          users[i].sockfd = newsock;
          users[i].room = -1;
          break;
        }
      }

      if (i >= MAXACTIVE)
      {
        /* full -- no more chat users */
        close(newsock);
      }
      else
      {

#if !RELIABLE_SELECT_FOR_WRITE
        int flags = fcntl(newsock, F_GETFL, 0);

        flags |= O_NDELAY;
        fcntl(newsock, F_SETFL, flags);
#endif

        FD_SET(newsock, &allfds);
        if (newsock >= nfds)
          nfds = newsock + 1;
        num_conns++;
      }
    }

    for (i = 0; i < MAXACTIVE; i++)
    {
      /* we are done with newsock, so re-use the variable */
      newsock = users[i].sockfd;
      if (newsock != -1 && FD_ISSET(newsock, &readfds))
      {
        if (process_chat_command(i) == -1)
        {
          logout_user(i);
        }
      }
    }

#if 0
    if (num_conns <= 0)
    {
      /* one more pass at select, then we go bye-bye */
      tvptr = &zerotv;
    }
#endif
  }
  /* NOTREACHED */
}
