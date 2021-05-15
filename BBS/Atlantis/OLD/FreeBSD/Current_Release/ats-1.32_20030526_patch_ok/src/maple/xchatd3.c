/*-------------------------------------------------------*/
/* xchatd.c     ( NTHU CS MapleBBS Ver 3.00 )            */
/*-------------------------------------------------------*/
/* target : super KTV daemon for chat server             */
/* create : 95/03/29                                     */
/* update : 96/11/05                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

#undef  MONITOR                 /* �ʷ� chatroom ���ʥH�ѨM�ȯ� */

#undef DEBUG                   /* �{���������� */

#ifdef  DEBUG
#define MONITOR
#endif

#define CHAT_PIDFILE    BBSHOME"/run/chat.pid"
#define CHAT_LOGFILE    BBSHOME"/run/chat.log"
#define SOCK_QLEN       5

/* name of the main room (always exists) */

#define MAIN_NAME       "main"
#define MAIN_TOPIC      "�Ӥ��۳{�ۦ��t"

#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <syslog.h>

#ifdef  AIX
#include <sys/select.h>
#endif

#define ROOM_LOCKED     0x1
#define ROOM_SECRET     0x2
#define ROOM_ALL        (NULL)

#define LOCKED(room)    (room->rflag & ROOM_LOCKED)
#define SECRET(room)    (room->rflag & ROOM_SECRET)

#define RESTRICTED(usr) (usr->uflag == 0)       /* guest */
#define CHATSYSOP(usr)  (usr->uflag & ( PERM_SYSOP | PERM_CHATROOM))
                                    /* Thor: SYSOP �P CHATROOM���O chat�`�� */
#define PERM_ROOMOP     PERM_CHAT   /* Thor: �� PERM_CHAT�� PERM_ROOMOP */
                                    /*       �i�J�ɻݲM��              */
                                    /* Thor: ROOMOP���ж��޲z�� */
#define ROOMOP(usr)  (usr->uflag & ( PERM_ROOMOP | PERM_SYSOP | PERM_CHATROOM))
#define CLOAK(usr)      (usr->uflag & PERM_CLOAK)
                                    /* Thor: ��ѫ������N */

/* ----------------------------------------------------- */
/* ChatRoom data structure                               */
/* ----------------------------------------------------- */

typedef struct ChatRoom ChatRoom;
typedef struct ChatUser ChatUser;
typedef struct UserList UserList;
typedef struct ChatCmd ChatCmd;
typedef struct ChatAction ChatAction;

struct ChatUser
{
  struct ChatUser *next, *prev;
  int sock;                     /* user socket */
  ChatRoom *room;
  UserList *ignore;
  int userno;
  int uflag;
  time_t uptime;                /* Thor: unused */
  char userid[IDLEN + 1];       /* real userid */
  char chatid[9];               /* chat id */
  char ibuf[80];                /* buffer for non-blocking receiving */
  int isize;                    /* current size of ibuf */
};

struct ChatRoom
{
  struct ChatRoom *next, *prev;
  char name[IDLEN];
  char topic[48];               /* Let the room op to define room topic */
  int rflag;                    /* ROOM_LOCKED, ROOM_SECRET */
  int occupants;                /* number of users in room */
  UserList *invite;
};

struct UserList
{
  struct UserList *next;
  int userno;
  char userid[IDLEN + 1];
};

struct ChatCmd
{
  char *cmdstr;
  void (*cmdfunc) ();
  int exact;
};

ChatRoom mainroom;
ChatUser mainuser;
fd_set mainfds;
int maxfds;                     /* number of sockets to select on */
int totaluser;                  /* current number of connections */
struct timeval zerotv;          /* timeval for selecting */
char chatbuf[256];              /* general purpose buffer */
time_t start_time;

char msg_not_op[] = "�� �z���O�o����ѫǪ� Op";
char msg_no_such_id[] = "�� �ثe�S���H�ϥ� [%s] �o�Ӳ�ѥN��";
char msg_not_here[] = "�� [%s] ���b�o����ѫ�";

/* ----------------------------------------------------- */
/* operation log and debug information                   */
/* ----------------------------------------------------- */

int flog;                       /* log file descriptor */

void logit(key, msg)
  char *key;
  char *msg;
{
  time_t now;
  struct tm *p;
  char buf[512];

  time(&now);
  p = localtime(&now);
  sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d %-13s%s\n",
    p->tm_year, p->tm_mon + 1, p->tm_mday,
    p->tm_hour, p->tm_min, p->tm_sec, key, msg);
  write(flog, buf, strlen(buf));
}

void log_init()
{
  flog = open(CHAT_LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0664);
  logit("START", "chat daemon");
}

void log_close()
{
  close(flog);
}

#ifdef  DEBUG
void debug_user()
{
  register ChatUser *user;
  int i;
  char buf[80];

  i = 0;
  for (user = mainuser.next; user; user = user->next)
  {
    sprintf(buf, "%d) %s %s", ++i, user->userid, user->chatid);
    logit("DEBUG_U", buf);
  }
}

void debug_room()
{
  register ChatRoom *room;
  int i;
  char buf[80];

  i = 0;
  room = &mainroom;

  do
  {
    sprintf(buf, "%d) %s %d", ++i, room->name, room->occupants);
    logit("DEBUG_R", buf);
  } while (room = room->next);
}
#endif  DEBUG

/* ----------------------------------------------------- */
/* string routines                                       */
/* ----------------------------------------------------- */

int valid_chatid(id)
  register char *id;
{
  register int ch, len;

  for (len = 0; ch = *id; id++)
  {
    if (ch == '/' || ch == '*' || ch == ':')
      return 0;
    if (++len > 8)
      return 0;
  }
  return len;
}

int Isspace(ch)
  int ch;
{
  return (ch == ' ' || ch == '\t' || ch == 10 || ch == 13);
}

char *nextword(str)
  char **str;
{
  char *head, *tail;
  int ch;

  head = *str;
  for (;;)
  {
    ch = *head;
    if (!ch)
    {
      *str = head;
      return head;
    }
    if (!Isspace(ch))
      break;
    head++;
  }

  tail = head + 1;
  while (ch = *tail)
  {
    if (Isspace(ch))
    {
      *tail++ = '\0';
      break;
    }
    tail++;
  }
  *str = tail;

  return head;
}

/* Case Independent strcmp : 1 ==> euqal */

int str_equal(s1, s2)
  register char *s1, *s2;
{
  register int c1, c2;

  for (;;)
  {
    c1 = *s1;
    if (c1 >= 'A' && c1 <= 'Z')
      c1 |= 32;

    c2 = *s2;
    if (c2 >= 'A' && c2 <= 'Z')
      c2 |= 32;

    if (c1 != c2)
      return 0;

    if (!c1)
      return 1;

    s1++;
    s2++;
  }
}

/* ----------------------------------------------------- */
/* match strings' similarity case-insensitively          */
/* ----------------------------------------------------- */
/* str_match(keyword, string)                    */
/* ----------------------------------------------------- */
/* 0 : equal            ("foo", "foo")                   */
/* -1 : mismatch        ("abc", "xyz")                   */
/* ow : similar         ("goo", "good")                  */
/* ----------------------------------------------------- */

int str_match(s1, s2)
  register char *s1, *s2;
{
  register int c1, c2;

  for (;;)
  {
    c2 = *s2;
    c1 = *s1;
    if (!c1)
    {
      return c2;
    }

    if (c1 >= 'A' && c1 <= 'Z')
      c1 |= 32;

    if (c2 >= 'A' && c2 <= 'Z')
      c2 |= 32;

    if (c1 != c2)
      return -1;

    s1++;
    s2++;
  }
}

/* ----------------------------------------------------- */
/* search user/room by its ID                            */
/* ----------------------------------------------------- */

ChatUser *cuser_by_userid(userid)
  char *userid;
{
  register ChatUser *cu;

  for (cu = mainuser.next; cu; cu = cu->next)
  {
    if (str_equal(userid, cu->userid))
      break;
  }
  return cu;
}

ChatUser *cuser_by_chatid(chatid)
  char *chatid;
{
  register ChatUser *cu;

  for (cu = mainuser.next; cu; cu = cu->next)
  {
    if (str_equal(chatid, cu->chatid))
      break;
  }
  return cu;
}

ChatUser *fuzzy_cuser_by_chatid(chatid)
  char *chatid;
{
  register ChatUser *cu, *xuser;
  int mode;

  xuser = NULL;

  for (cu = mainuser.next; cu; cu = cu->next)
  {
    mode = str_match(chatid, cu->chatid);
    if (mode == 0)
      return cu;

    if (mode > 0)
    {
      if (xuser == NULL)
        xuser = cu;
      else
        return &mainuser;       /* �ŦX�̤j�� 2 �H */
    }
  }
  return xuser;
}

ChatRoom *croom_by_roomid(roomid)
  char *roomid;
{
  register ChatRoom *room;

  room = &mainroom;
  do
  {
    if (str_equal(roomid, room->name))
      break;
  } while (room = room->next);
  return room;
}

/* ----------------------------------------------------- */
/* UserList routines                                     */
/* ----------------------------------------------------- */

void list_free(list)
  UserList *list;
{
  UserList *tmp;

  while (list)
  {
    tmp = list->next;
    (void) free(list);
    list = tmp;
  }
}

void list_add(list, user)
  UserList **list;
  ChatUser *user;
{
  UserList *node;

  if(node = (UserList *) malloc(sizeof(UserList)))
  {   /* Thor: ����Ŷ����� */
    (void) strcpy(node->userid, user->userid);
    node->userno = user->userno;
    node->next = *list;
    *list = node;
  }
}

int list_delete(list, userid)
  UserList **list;
  char *userid;
{
  UserList *node;

  while (node = *list)
  {
    if (str_equal(node->userid, userid))
    {
      *list = node->next;
      (void) free(node);
      return 1;
    }
    list = &node->next;  /* Thor: list�n��۫e�i */
  }

  return 0;
}

int list_belong(list, userno)
  UserList *list;
  int userno;
{
  while (list)
  {
    if (userno == list->userno)
      return 1;
    list = list->next;
  }
  return 0;
}

/* ------------------------------------------------------ */
/* non-blocking socket routines : send message to players */
/* ------------------------------------------------------ */

void do_send(nfds, wset, msg)
  int nfds;
  fd_set *wset;
  char *msg;
{
  int sr;

  sr = select(nfds + 1, NULL, wset, NULL, &zerotv);
  if (sr > 0)
  {
    register int len;

    len = strlen(msg) + 1;
    while (nfds >= 0)
    {
      if (FD_ISSET(nfds, wset))
      {
        send(nfds, msg, len, 0); /* Thor: �p�Gbuffer���F, ���| block */
        if (--sr <= 0)
          return;
      }
      nfds--;
    }
  }
}

void send_to_room(room, msg, userno)
  ChatRoom *room;
  char *msg;
  int userno;
{
  ChatUser *cu;
  fd_set wset, *wptr;
  int sock, max;

  FD_ZERO(wptr = &wset);
  max = -1;

  for (cu = mainuser.next; cu; cu = cu->next)
  {
    if (room == cu->room || room == ROOM_ALL)
    {
      if (!userno || !list_belong(cu->ignore, userno))
      {
        sock = cu->sock;
        FD_SET(sock, wptr);
        if (max < sock)
          max = sock;
      }
    }
  }
  if (max >= 0)
    do_send(max, wptr, msg);
}

void send_to_user(user, msg, userno)
  ChatUser *user;
  char *msg;
  int userno;
{
  if (!userno || !list_belong(user->ignore, userno))
  {
    fd_set wset, *wptr;
    int sock;

    sock = user->sock;
    FD_ZERO(wptr = &wset);
    FD_SET(sock, wptr);
    do_send(sock, wptr, msg);
  }
}

void send_to_sock(sock, msg)  /* Thor: unused */
  int sock;
  char *msg;
{
  fd_set wset, *wptr;

  FD_ZERO(wptr = &wset);
  FD_SET(sock, wptr);
  do_send(sock, wptr, msg);
}

/* ----------------------------------------------------- */

void exit_room(user, mode, msg)
  ChatUser *user;
  int mode;
  char *msg;
{
  ChatRoom *room;

  if (room = user->room)
  {
    user->room = NULL;
    user->uflag &= ~PERM_ROOMOP;

    if (--room->occupants > 0)
    {
      char *chatid;

      chatid = user->chatid;
      switch (mode)
      {
      case EXIT_LOGOUT:

        sprintf(chatbuf, "�� %s ���}�F ...", chatid);
        if (msg && *msg)
        {
          strcat(chatbuf, ": ");
          msg[79]=0;      /* Thor:����Ӫ� */
          strncat(chatbuf, msg, 80);
        }
        break;

      case EXIT_LOSTCONN:

        sprintf(chatbuf, "�� %s ���F�_�u�������o", chatid);
        break;

      case EXIT_KICK:

        sprintf(chatbuf, "�� �����I%s �Q��X�h�F", chatid);
        break;
      }
      if(!CLOAK(user))    /* Thor: ��ѫ������N */
        send_to_room(room, chatbuf, 0);
      return;
    }

    else if (room != &mainroom)   /* Thor: �H�Ƭ�0��,���Omainroom�~free */
    {
      register ChatRoom *next;

#ifdef  DEBUG
      debug_room();
#endif

      room->prev->next = room->next;
      if (next = room->next)
        next->prev = room->prev;
      list_free(room->invite);
      (void) free(room);

#ifdef  DEBUG
      debug_room();
#endif
    }
  }
}

/* ----------------------------------------------------- */
/* chat commands                                         */
/* ----------------------------------------------------- */

void chat_topic(cu, msg)
  ChatUser *cu;
  char *msg;
{
  ChatRoom *room;
  char *topic;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0);
    return;
  }

  if (*msg == '\0')
  {
    send_to_user(cu, "�� �Ы��w���D", 0);
    return;
  }

  room = cu->room;
  topic = room->topic;  /* Thor: room ���i�� NULL��?? */
  strncpy(topic, msg, 47);
  topic[47] = '\0';

  sprintf(chatbuf, "/t%s", topic);
  send_to_room(room, chatbuf, 0);

  sprintf(chatbuf, "�� %s �N���D�אּ [1;32m%s[m", cu->chatid, topic);
  if(!CLOAK(cu))           /* Thor: ��ѫ������N */
    send_to_room(room, chatbuf, 0);
}

void chat_nick(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *chatid, *str;
  ChatUser *xuser;

  chatid = nextword(&msg);
  chatid[8] = '\0';
  if (!valid_chatid(chatid))
  {
    send_to_user(cu, "�� �o�Ӳ�ѥN���O�����T��", 0);
    return;
  }

  xuser = cuser_by_chatid(chatid);
  if (xuser != NULL && xuser != cu)
  {
    send_to_user(cu, "�� �w�g���H�������n�o", 0);
    return;
  }

  str = cu->chatid;
  sprintf(chatbuf, "�� %s �N��ѥN���אּ [1;33m%s[m", str, chatid);
  if(!CLOAK(cu))           /* Thor: ��ѫ������N */
    send_to_room(cu->room, chatbuf, cu->userno);
  strcpy(str, chatid);

  sprintf(chatbuf, "/n%s", chatid);
  send_to_user(cu, chatbuf, 0);
}

void chat_list_rooms(cuser, msg)
  ChatUser *cuser;
  char *msg;
{
  ChatRoom *cr, *room;

  if (RESTRICTED(cuser))
  {
    send_to_user(cuser, "�� �z�S���v���C�X�{������ѫ�",0);
    return;
  }

  send_to_user(cuser, "[7m �ͤѫǦW��  �x�H�Ƣx���D        [m",0);
  room = cuser->room;
  cr = &mainroom;
  do
  {
    if(!SECRET(cr) || CHATSYSOP(cuser) || (cr==room&&ROOMOP(cuser)))
    {
      sprintf(chatbuf, " %-12s�x%4d�x%s", cr->name, cr->occupants, cr->topic);
      if (LOCKED(cr))
        strcat(chatbuf, " [���]");
      if (SECRET(cr))
        strcat(chatbuf, " [���K]");
      send_to_user(cuser, chatbuf, 0);
    }
  } while (cr = cr->next);
}

void chat_do_user_list(cu, msg, theroom)
  ChatUser *cu;
  char *msg;
  ChatRoom *theroom;
{
  ChatRoom *myroom, *room;
  ChatUser *user;

  int start, stop, curr;
  start = atoi(nextword(&msg));
  stop = atoi(nextword(&msg));

  myroom = cu->room;

#ifdef DEBUG
  logit(cu->chatid,"do user list");
#endif
  send_to_user(cu, "[7m ��ѥN���x�ϥΪ̥N��  �x��ѫ� [m",0);

  for (user = mainuser.next, curr=0; user; user = user->next)
  {
#ifdef DEBUG
  logit(cu->chatid,"list in for");
#endif
    room = user->room;
    if ((theroom != ROOM_ALL) && (theroom != room))
      continue;

    if (myroom != room)
    {
      if (RESTRICTED(cu)||
         (room&&SECRET(room)&&!CHATSYSOP(cu)))
        continue;
    }

    if (CLOAK(user))   /* Thor: �����N */
      continue;

#ifdef DEBUG
  logit(cu->chatid,"list in for 2");
#endif
    curr++;
    if (start && curr < start)
      continue;
    else if (stop && (curr > stop))
      break;

    sprintf(chatbuf, " %-8s�x%-12s�x%s", user->chatid, user->userid, room ? room->name : "[�b���f�r��]" );
    if (ROOMOP(user))
      strcat(chatbuf, " [Op]");
#ifdef  DEBUG
    logit("list_U", chatbuf);
#endif
    send_to_user(cu, chatbuf,0);
  }
}

void chat_list_by_room(cu, msg)
  ChatUser *cu;
  char *msg;
{
  ChatRoom *whichroom;
  char *roomstr;

  roomstr = nextword(&msg);
  if (*roomstr == '\0')
    whichroom = cu->room;
  else
  {
    if ((whichroom = croom_by_roomid(roomstr)) == NULL)
    {
      sprintf(chatbuf, "�� �S�� [%s] �o�Ӳ�ѫ�", roomstr);
      send_to_user(cu, chatbuf, 0);
      return;
    }

    if (SECRET(whichroom) && !CHATSYSOP(cu))
    {   /* Thor: �n���n���P�@room��SECRET���i�H�C? */
      send_to_user(cu, "�� �L�k�C�X�b���K��ѫǪ��ϥΪ�", 0);
      return;
    }
  }
  chat_do_user_list(cu, msg, whichroom);
}

void chat_list_users(cu, msg)
  ChatUser *cu;
  char *msg;
{
  chat_do_user_list(cu, msg, ROOM_ALL);
}

void chat_map_chatids(cu, whichroom)
  ChatUser *cu;   /* Thor: �٨S���@���P���� */
  ChatRoom *whichroom;
{
  int c;
  ChatRoom *myroom, *room;
  ChatUser *user;

  myroom = cu->room;
  send_to_user(cu,
    "[7m ��ѥN�� �ϥΪ̥N��  �x ��ѥN�� �ϥΪ̥N��  �x ��ѥN�� �ϥΪ̥N�� [m", 0);

  for (user = mainuser.next, c = 0; user; user = user->next)
  {
    room = user->room;
    if (whichroom != ROOM_ALL && whichroom != room)
      continue;
    if (myroom != room)
    {
      if (RESTRICTED(cu)||  /* Thor: �n��check room �O���O�Ū� */
         (room&&SECRET(room)&&!CHATSYSOP(cu)))
        continue;
    }
    if (CLOAK(user)) /* Thor:�����N */
      continue;
    sprintf(chatbuf + (c * 24), " %-8s%c%-12s%s",
      user->chatid, ROOMOP(user) ? '*' : ' ',
      user->userid, (c < 2 ? "�x" : "  "));
    if (++c == 3)
    {
      send_to_user(cu, chatbuf,0);
      c = 0;
    }
  }
  if (c > 0)
    send_to_user(cu, chatbuf,0);
}

void chat_map_chatids_thisroom(cu, msg)
  ChatUser *cu;
  char *msg;
{
  chat_map_chatids(cu, cu->room);
}

void chat_setroom(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *modestr;
  ChatRoom *room;
  char *chatid;
  int sign;
  int flag;
  char *fstr;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0);
    return;
  }

  modestr = nextword(&msg);
  sign = 1;
  if (*modestr == '+')
    modestr++;
  else if (*modestr == '-')
  {
    modestr++;
    sign = 0;
  }
  if (*modestr == '\0')
  {
    send_to_user(cu,
      "�� �Ы��w���A: {[+(�]�w)][-(����)]}{[l(���)][s(���K)]}", 0);
    return;
  }

  room = cu->room;
  chatid = cu->chatid;

  while (*modestr)
  {
    flag = 0;
    switch (*modestr)
    {
    case 'l':
    case 'L':
      flag = ROOM_LOCKED;
      fstr = "���";
      break;

    case 's':
    case 'S':
      flag = ROOM_SECRET;
      fstr = "���K";
      break;

    default:
      sprintf(chatbuf, "�� ���A���~�G[%c]", *modestr);
      send_to_user(cu, chatbuf, 0);
    }

    /* Thor: check room �O���O�Ū�, ���Ӥ��O�Ū� */
    if (flag && (room->rflag & flag) != sign * flag)
    {
      room->rflag ^= flag;
      sprintf(chatbuf, "�� ����ѫǳQ %s %s [%s] ���A",
        chatid, sign ? "�]�w��" : "����", fstr);
      if(!CLOAK(cu))           /* Thor: ��ѫ������N */
        send_to_room(room, chatbuf, 0);
    }
    modestr++;
  }
}

void chat_private(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *recipient;
  ChatUser *xuser;
  int userno;

  userno = 0;
  recipient = nextword(&msg);
  xuser = (ChatUser *) fuzzy_cuser_by_chatid(recipient);
  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, recipient);
  }
  else if (xuser == &mainuser)  /* ambiguous */
  {
    strcpy(chatbuf, "�� �Ы�����ѥN��");
  }
  else if (*msg)
  {
    userno = cu->userno;
    sprintf(chatbuf, "[1m*%s*[m ", cu->chatid);
    msg[79]=0;      /* Thor:����Ӫ� */
    strncat(chatbuf, msg, 80);
    send_to_user(xuser, chatbuf, userno);
    sprintf(chatbuf, "%s> ", xuser->chatid);
    strncat(chatbuf, msg, 80);
  }
  else
  {
    sprintf(chatbuf, "�� �z�Q�� %s ������ܩO�H", xuser->chatid);
  }
  send_to_user(cu, chatbuf, userno);  /* Thor: userno �n�令 0 ��? */
}

void chat_cloak(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if(CHATSYSOP(cu))
  {
    cu->uflag ^= PERM_CLOAK;
    sprintf(chatbuf, "�� %s", CLOAK(cu)? MSG_CLOAKED:MSG_UNCLOAK);
    send_to_user(cu, chatbuf, 0);
  }
}
/* ----------------------------------------------------- */

void arrive_room(cuser, room)
  ChatUser *cuser;
  ChatRoom *room;
{
  char *rname;

  cuser->room = room;
  room->occupants++;
  rname = room->name;

  sprintf(chatbuf, "/r%s", rname);
  send_to_user(cuser, chatbuf, 0);

  sprintf(chatbuf, "/t%s", room->topic);
  send_to_user(cuser, chatbuf, 0);

  sprintf(chatbuf, "�� [32;1m%s[m �i�J [33;1m[%s][m �]�[",
    cuser->chatid, rname);
  if(!CLOAK(cuser))           /* Thor: ��ѫ������N */
    send_to_room(room, chatbuf, cuser->userno);
}

int enter_room(cuser, rname, msg)
  ChatUser *cuser;
  char *rname;
  char *msg;
{
  ChatRoom *room;
  int create;

  create = 0;
  room = croom_by_roomid(rname);
  if (room == NULL)
  {
    /* new room */

#ifdef  MONITOR
    logit(cuser->userid, "create new room");
#endif

    room = (ChatRoom *) malloc(sizeof(ChatRoom));
    if (room == NULL)
    {
      send_to_user(cuser, "�� �L�k�A�s�P�]�[�F", 0);
      return 0;
    }

    (void) memset(room, 0, sizeof(ChatRoom));
    (void) memcpy(room->name, rname, IDLEN - 1);
    (void) strcpy(room->topic, "�o�O�@�ӷs�Ѧa");

    if (mainroom.next != NULL)
      mainroom.next->prev = room;
    room->next = mainroom.next;
    mainroom.next = room;
    room->prev = &mainroom;

    create = 1;
  }
  else
  {
    if (cuser->room == room)
    {
      sprintf(chatbuf, "�� �z���ӴN�b [%s] ��ѫ��o :)", rname);
      send_to_user(cuser, chatbuf, 0);
      return 0;
    }

    if (!CHATSYSOP(cuser) && LOCKED(room) && !list_belong(room->invite, cuser->userno))
    {
      send_to_user(cuser, "�� �����c���A�D�в��J", 0);
      return 0;
    }
  }

  exit_room(cuser, EXIT_LOGOUT, msg);
  arrive_room(cuser, room);

  if (create)
    cuser->uflag |= PERM_ROOMOP;

  return 0;
}

void logout_user(cuser)
  ChatUser *cuser;
{
  int sock;
  ChatUser *next, *prev;

#ifdef  DEBUG
  logit("before", "logout");
  debug_user();
#endif

  sock = cuser->sock;
  close(sock);
  FD_CLR(sock, &mainfds);
/*
  if (sock >= maxfds)
    maxfds = sock - 1;
*/  /* Thor: �]�\���t�o�@�� */

  list_free(cuser->ignore);

  next = cuser->next;
  prev = cuser->prev;
  prev->next = next;
  if (next)
    next->prev = prev;

  (void) free(cuser);

#ifdef  DEBUG
  logit("after", "logout");
  debug_user();
#endif

  totaluser--;
}

void print_user_counts(cuser)
  ChatUser *cuser;
{
  ChatRoom *room;
  int num, userc, suserc, roomc;

  userc = suserc = roomc = 0;

  room = &mainroom;
  do
  {
    num = room->occupants;
    if (SECRET(room))
    {
      suserc += num;
      if (CHATSYSOP(cuser))
        roomc++;
    }
    else
    {
      userc += num;
      roomc++;
    }
  } while (room = room->next);

  sprintf(chatbuf,
    "�� �w����{�i���������]�j�A�ثe�}�F [1;31m%d[m ���]�[", roomc);
  send_to_user(cuser, chatbuf, 0);
  sprintf(chatbuf, "�� �@�� [1;36m%d[m �H���\\�s���}", userc);
  if (suserc)
    sprintf(chatbuf + strlen(chatbuf), " [%d �H�b���K��ѫ�]", suserc);
  send_to_user(cuser, chatbuf, 0);
}

int login_user(cu, msg)
  ChatUser *cu;
  char *msg;
{
  int utent;
  char *utentstr;
  char *level;
  char *userid;
  char *chatid;
  ChatUser *xuser;

#ifdef  DEBUG
  logit("ENTER", msg);
#endif

  utentstr = nextword(&msg);
  utent = atoi(utentstr);

  for (xuser = mainuser.next; xuser; xuser = xuser->next)
  {
    if (xuser->userno == utent)
    {

#ifdef  DEBUG
      logit("enter", "bogus");
#endif

      send_to_user(cu, CHAT_LOGIN_BOGUS, 0);
      return -1;  /* Thor: �άO0�����ۤv�F�_? */
    }
  }

  /* �ǰѼơGuserlevel, userid, chatid */
  /* client/server �����̾� userid �� .PASSWDS �P�_ userlevel */

  level = nextword(&msg);
  userid = nextword(&msg);
  chatid = nextword(&msg);

  if (!valid_chatid(chatid))
  {

#ifdef  DEBUG
    logit("enter", chatid);
#endif

    send_to_user(cu, CHAT_LOGIN_INVALID, 0);
    return 0;
  }

#ifdef  DEBUG
  debug_user();
#endif

  if (cuser_by_chatid(chatid) != NULL)
  {
    /* chatid in use */

#ifdef  DEBUG
    logit("enter", "duplicate");
#endif

    send_to_user(cu, CHAT_LOGIN_EXISTS, 0);
    return 0;
  }

  cu->userno = utent;
  cu->uflag = atoi(level) & ~(PERM_ROOMOP | PERM_CLOAK);
               /* Thor: �i�ӥ��M��ROOMOP, CLOAK(�PPERM_CHAT) */
  strcpy(cu->userid, userid);
  memcpy(cu->chatid, chatid, 8);
  cu->chatid[8] = '\0';
  send_to_user(cu, CHAT_LOGIN_OK, 0);
  arrive_room(cu, &mainroom);
  print_user_counts(cu);

#ifdef  DEBUG
  logit("enter", "OK");
#endif

  return 0;
}

void chat_act(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (*msg)
  {
    sprintf(chatbuf, "%s [36m%s[m", cu->chatid, msg);
    send_to_room(cu->room, chatbuf, cu->userno);
  }
}

void chat_ignore(cu, msg)
  ChatUser *cu;
  char *msg;
{

  if (RESTRICTED(cu))
  {
    strcpy(chatbuf, "�� �z�S�� ignore �O�H���v�Q");
  }
  else
  {
    char *ignoree;

    ignoree = nextword(&msg);
    if (*ignoree)
    {
      ChatUser *xuser;

      xuser = cuser_by_userid(ignoree);

      if (xuser == NULL)
      {
        sprintf(chatbuf, "�� �ͤѫǲ{�b�S�� [%s] �o���H��", ignoree);
      }
      else if (xuser == cu || CHATSYSOP(xuser) ||
               (ROOMOP(xuser) && (xuser->room == cu->room)))
      {
        sprintf(chatbuf, "�� ���i�H ignore [%s]", ignoree);
      }
      else
      {

        if (list_belong(cu->ignore, xuser->userno))
        {
          sprintf(chatbuf, "�� %s �w�g�Q�ᵲ�F", xuser->chatid);
        }
        else
        {
          list_add(&(cu->ignore), xuser);
          sprintf(chatbuf, "�� �N [%s] ���J�N�c�F :p", xuser->chatid);
        }
      }
    }
    else
    {
      UserList *list;

      if (list = cu->ignore)
      {
        int len;
        char buf[16];

        send_to_user(cu, "�� �o�ǤH�Q���J�N�c�F�G", 0);
        len = 0;
        do
        {
          sprintf(buf, "%-13s", list->userid);
          strcpy(chatbuf + len, buf);
          len += 13;
          if (len >= 78)
          {
            send_to_user(cu, chatbuf, 0);
            len = 0;
          }
        } while (list = list->next);

        if (len == 0)
          return;
      }
      else
      {
        strcpy(chatbuf, "�� �z�ثe�èS�� ignore ����H");
      }
    }
  }

  send_to_user(cu, chatbuf, 0);
}

void chat_unignore(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *ignoree;

  ignoree = nextword(&msg);

  if (*ignoree)
  {
    sprintf(chatbuf, (list_delete(&(cu->ignore), ignoree)) ?
      "�� [%s] ���A�Q�A�N���F" :
      "�� �z�å� ignore [%s] �o���H��", ignoree);
  }
  else
  {
    strcpy(chatbuf, "�� �Ы��� user ID");
  }
  send_to_user(cu, chatbuf, 0);
}

void chat_join(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (RESTRICTED(cu))
  {
    send_to_user(cu, "�� �z�S���[�J��L��ѫǪ��v��", 0);
  }
  else
  {
    char *roomid = nextword(&msg);

    if (*roomid)
      enter_room(cu, roomid, msg);
    else
      send_to_user(cu, "�� �Ы��w��ѫǪ��W�r", 0);
  }
}

void chat_kick(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *twit;
  ChatUser *xuser;
  ChatRoom *room;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0);
    return;
  }

  twit = nextword(&msg);
  xuser = cuser_by_chatid(twit);
  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, twit);
    send_to_user(cu, chatbuf, 0);
    return;
  }

  room = cu->room;
  if (room != xuser->room || CLOAK(xuser))  /* Thor: ��ѫ������N */
  {
    sprintf(chatbuf, msg_not_here, twit);
    send_to_user(cu, chatbuf, 0);
    return;
  }

  if (CHATSYSOP(xuser))  /* Thor: �𤣨� CHATSYSOP */
  {
    sprintf(chatbuf, "�� ���i�H kick [%s]", twit);
    send_to_user(cu, chatbuf, 0);
    return;
  }

  exit_room(xuser, EXIT_KICK, (char *) NULL);

  if (room == &mainroom)
    logout_user(xuser);
  else
    enter_room(xuser, MAIN_NAME, (char *) NULL);
}

void chat_makeop(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *newop;
  ChatUser *xuser;
  ChatRoom *room;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0);
    return;
  }

  newop = nextword(&msg);
  xuser = cuser_by_chatid(newop);
  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, newop);
    send_to_user(cu, chatbuf, 0);
    return;
  }

  if (cu == xuser)
  {
    sprintf(chatbuf, "�� �z���N�w�g�O Op �F��");
    send_to_user(cu, chatbuf, 0);
    return;
  }

  room = cu->room;

  if (room != xuser->room || CLOAK(xuser)) /* Thor: ��ѫ������N */
  {
    sprintf(chatbuf, msg_not_here, xuser->chatid);
    send_to_user(cu, chatbuf, 0);
    return;
  }
  cu->uflag &= ~PERM_ROOMOP;
  xuser->uflag |= PERM_ROOMOP;
  sprintf(chatbuf, "�� %s �N Op �v�O�ಾ�� %s",
    cu->chatid, xuser->chatid);
  if(!CLOAK(cu))           /* Thor: ��ѫ������N */
    send_to_room(room, chatbuf, 0);
}

void chat_invite(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *invitee;
  ChatUser *xuser;
  ChatRoom *room;
  UserList **list;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0);
    return;
  }

  invitee = nextword(&msg);
  xuser = cuser_by_chatid(invitee);
  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, invitee);
    send_to_user(cu, chatbuf, 0);
    return;
  }

  room = cu->room;  /* Thor: �O�_�n check room �O�_ NULL ? */
  list = &(room->invite);

  if (list_belong(*list, xuser->userno))
  {
    sprintf(chatbuf, "�� %s �w�g�����L�ܽФF", xuser->chatid);
    send_to_user(cu, chatbuf, 0);
    return;
  }
  list_add(list, xuser);

  sprintf(chatbuf, "�� %s �ܽбz�� [%s] ��ѫ�",
    cu->chatid, room->name);
  send_to_user(xuser, chatbuf, 0);  /* Thor: �n���n�i�H ignore? */
  sprintf(chatbuf, "�� %s ����z���ܽФF", xuser->chatid);
  send_to_user(cu, chatbuf, 0);
}

void chat_broadcast(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (!CHATSYSOP(cu))
  {
    send_to_user(cu, "�� �z�S���b��ѫǼs�����v�O!", 0);
    return;
  }
  if (*msg == '\0')
  {
    send_to_user(cu, "�� �Ы��w�s�����e", 0);
    return;
  }
  sprintf(chatbuf, "[1m�� " BOARDNAME "�ͤѫǼs���� [%s].....[m",
    cu->chatid);
  send_to_room(ROOM_ALL, chatbuf, 0);
  sprintf(chatbuf, "�� %s", msg);
  send_to_room(ROOM_ALL, chatbuf, 0);
}

void chat_goodbye(cu, msg)
  ChatUser *cu;
  char *msg;
{
  exit_room(cu, EXIT_LOGOUT, msg);
  /* Thor: �n���n�[ logout_user(cu) ? */
}

/* --------------------------------------------- */
/* MUD-like social commands : action             */
/* --------------------------------------------- */

struct ChatAction
{
  char *verb;                   /* �ʵ� */
  char *part1_msg;              /* ���� */
  char *part2_msg;              /* �ʧ@ */
};

ChatAction party_data[] =
{
  {"bearhug", "�������֩�", ""},
  {"bless", "����", "�߷Q�Ʀ�"},
  {"bow", "���`���q���V", "���`"},
  {"caress", "���N", ""},
  {"cringe", "�V", "���`�}���A�n���^��"},
  {"cry", "�V", "�z�ޤj��"},
  {"comfort", "�Ũ��w��", ""},
  {"clap", "�V", "���P���x"},
  {"dance", "�ԤF", "���⽡���_�R"},
  {"dogleg", "��", "���L"},
  {"drivel", "���", "�y�f��"},
  {"giggle", "���", "�̶̪��b��"},
  {"grin", "��", "�S�X���c�����e"},
  {"growl", "��", "�H�����w"},
  {"hand", "��", "����"},
  {"hug", "�����a�֩�", ""},
  {"kick", "��", "�𪺦��h����"},
  {"kiss", "���k", "���y�U"},
  {"laugh", "�j�n�J��", ""},
  {"nod", "�V", "�I�Y�٬O"},
  {"nudge", "�Τ�y��", "���Ψ{�l"},
  {"pad", "����", "���ӻH"},
  {"pinch", "�ΤO����", "�����«C"},
  {"punch", "�����~�F", "�@�y"},
  {"shrug", "�L�`�a�V", "�q�F�q�ӻH"},
  {"sigh", "��", "�ۤF�@�f��"},
  {"slap", "�԰Ԫ��ڤF", "�@�y�ե�"},
  {"smooch", "�֧k��", ""},
  {"snicker", "�K�K�K..����", "�ѯ�"},
  {"sniff", "��", "�ᤧ�H��"},
  {"spank", "�Τڴx��", "���v��"},
  {"squeeze", "���a�֩��", ""},
  {"thank", "�V", "�D��"},
  {"tickle", "�B�T!�B�T!�k", "���o"},
  {"wave", "���", "���R���n��"},
  {"wink", "��", "�������w�w����"},
  {"zap", "��", "�ƨg������"},
  {NULL, NULL, NULL}
};

int party_action(cu, cmd, party)
  ChatUser *cu;
  char *cmd;
  char *party;
{
  ChatAction *cap;
  char *verb;

  for (cap = party_data; verb = cap->verb; cap++)
  {
    if (str_equal(cmd, verb))
    {
      if (*party == '\0')
      {
        party = "�j�a";
      }
      else
      {
        ChatUser *xuser;

        xuser = fuzzy_cuser_by_chatid(party);

        if (xuser == NULL)
        {
          sprintf(chatbuf, msg_no_such_id, party);
          send_to_user(cu, chatbuf, 0);
          return 0;
        }
        else if (xuser == &mainuser)
        {
          sprintf(chatbuf, "�� �Ы�����ѥN��");
          send_to_user(cu, chatbuf, 0);
          return 0;
        }
        else if (cu->room != xuser->room || CLOAK(xuser))
        {
          sprintf(chatbuf, msg_not_here, party);
          send_to_user(cu, chatbuf, 0);
          return 0;
        }
        else
        {
          party = xuser->chatid;
        }
      }
      sprintf(chatbuf, "[1;32m%s [31m%s[33m %s [31m%s[m",
        cu->chatid, cap->part1_msg, party, cap->part2_msg);
      send_to_room(cu->room, chatbuf, cu->userno);
      return 0;  /* Thor: cu->room �O�_�� NULL? */
    }
  }
  return 1;
}

/* --------------------------------------------- */
/* MUD-like social commands : speak              */
/* --------------------------------------------- */

ChatAction speak_data[] =
{
  {"ask", "�߰�", NULL},
  {"chant", "�q�|", NULL},
  {"cheer", "�ܪ�", NULL},
  {"chuckle", "����", NULL},
  {"curse", "�G�|", NULL},
  {"demand", "�n�D", NULL},
  {"frown", "�٬�", NULL},
  {"groan", "�D�u", NULL},
  {"grumble", "�o�c��", NULL},
  {"hum", "���ۻy", NULL},
  {"moan", "�d��", NULL},
  {"notice", "�`�N", NULL},
  {"order", "�R�O", NULL},
  {"ponder", "�H��", NULL},
  {"pout", "���ۼL��", NULL},
  {"pray", "��ë", NULL},
  {"request", "���D", NULL},
  {"shout", "�j�s", NULL},
  {"sing", "�ۺq", NULL},
  {"smile", "�L��", NULL},
  {"smirk", "����", NULL},
  {"swear", "�o�}", NULL},
  {"tease", "�J��", NULL},
  {"whimper", "��|����", NULL},
  {"yawn", "����s��", NULL},
  {"yell", "�j��", NULL},
  {NULL, NULL, NULL}
};

int speak_action(cu, cmd, msg)
  ChatUser *cu;
  char *cmd;
  char *msg;
{
  ChatAction *cap;
  char *verb;

  for (cap = speak_data; verb = cap->verb; cap++)
  {
    if (str_equal(cmd, verb))
    {
      sprintf(chatbuf, "[1;32m%s [31m%s�G[33m %s[m",
        cu->chatid, cap->part1_msg, msg);
      send_to_room(cu->room, chatbuf, cu->userno);
      return 0;  /* Thor: cu->room �O�_�� NULL? */
    }
  }
  return 1;
}

/* -------------------------------------------- */
/* MUD-like social commands : condition          */
/* -------------------------------------------- */

ChatAction condition_data[] =
{
  {"applaud", "�԰԰԰԰԰԰�....", NULL},
  {"blush", "�y�����F", NULL},
  {"cough", "�y�F�X�n", NULL},
  {"happy", "r-o-O-m....ť�F�u�n�I", NULL},
  {"luck", "�z�I�֮�աI", NULL},
  {"puke", "�u���ߡA��ť�F���Q�R", NULL},
  {"shake", "�n�F�n�Y", NULL},
  {"sleep", "Zzzzzzzzzz�A�u�L��A���ֺεۤF", NULL},
  {"so", "�N��l!!", NULL},
  {"strut", "�j�n�j�\\�a��", NULL},
  {"tongue", "�R�F�R���Y", NULL},
  {"think", "�n���Y�Q�F�@�U", NULL},
  {NULL, NULL, NULL}
};

int condition_action(cu, cmd)
  ChatUser *cu;
  char *cmd;
{
  ChatAction *cap;
  char *verb;

  for (cap = condition_data; verb = cap->verb; cap++)
  {
    if (str_equal(cmd, verb))
    {
      sprintf(chatbuf, "[1;32m%s [31m%s[m",
        cu->chatid, cap->part1_msg);
      send_to_room(cu->room, chatbuf, cu->userno);
      return 1;  /* Thor: cu->room �O�_�� NULL? */
    }
  }
  return 0;
}

/* --------------------------------------------- */
/* MUD-like social commands : help               */
/* --------------------------------------------- */

char *dscrb[] = {
  "[1m�i Verb + Nick�G   �ʵ� + ���W�r �j[36m   �ҡG//kick piggy[m",
  "[1m�i Verb + Message�G�ʵ� + �n������ �j[36m   �ҡG//sing �ѤѤ���[m",
"[1m�i Verb�G�ʵ� �j    �����G�¸ܭ���[m", NULL};
ChatAction *catbl[] = {party_data, speak_data, condition_data, NULL};

#define SCREEN_WIDTH    80
#define MAX_VERB_LEN    10
#define VERB_NO         8

void view_action_verb(cu)
  register ChatUser *cu;
{
  register int i, j, sock;
  register char *p, *data;
  register ChatAction *cap;

  send_to_user(cu, "/c", 0);

  data = chatbuf;

  for (i = 0; p = dscrb[i]; i++)
  {
    send_to_user(cu, p, 0);
    *data = '\0';
    j = 0;
    cap = catbl[i];
    while (p = cap[j++].verb)
    {
      strcat(data, p);
      if ((j % VERB_NO) == 0)
      {
        send_to_user(cu, data, 0);
        *data = '\0';
      }
      else
      {
        strncat(data, "        ", MAX_VERB_LEN - strlen(p));
      }
    }
    if (j % VERB_NO)
      send_to_user(cu, data,0);
    send_to_user(cu, " ",0);
  }
}

/* ----------------------------------------------------- */
/* chat user service routines                            */
/* ----------------------------------------------------- */

ChatCmd chatcmdlist[] =
{
  "act", chat_act, 0,
  "bye", chat_goodbye, 0,
  "cloak", chat_cloak, 2,
  "flags", chat_setroom, 0,
  "ignore", chat_ignore, 1,
  "invite", chat_invite, 0,
  "join", chat_join, 0,
  "kick", chat_kick, 1,
  "msg", chat_private, 0,
  "nick", chat_nick, 0,
  "operator", chat_makeop, 0,
  "room", chat_list_rooms, 0,
  "unignore", chat_unignore, 1,
  "whoin", chat_list_by_room, 1,
  "wall", chat_broadcast, 2,

  "who", chat_map_chatids_thisroom, 0,
  "list", chat_list_users, 0,
  "topic", chat_topic, 1,

  NULL, NULL, 0
};

/* Thor: 0 ���� exact, 1 �n exactly equal, 2 ���K���O */

int command_execute(cu)
  ChatUser *cu;
{
  char *msg, *cmd;
  ChatCmd *cmdrec;
  int match;

  msg = cu->ibuf;
  match = *msg;

#ifdef  DEBUG
  logit("COMMAND", msg);
#endif

  /* Validation routine */

  if (cu->room == NULL)
  {
    /* MUST give special /! command if not in the room yet */

    if (match != '/' || msg[1] != '!')
      return -1;
    else
      return (login_user(cu, msg + 2));
  }

  /* If not a /-command, it goes to the room. */

  if (match != '/')
  {
    if (match)
    {
      char buf[16];

      sprintf(buf, "%s:", cu->chatid);
      sprintf(chatbuf, "%-10s%s", buf, msg);
      if(!CLOAK(cu))           /* Thor: ��ѫ������N */
        send_to_room(cu->room, chatbuf, cu->userno);
                      /* Thor: �n check cu->room NULL��? */
#ifdef  MONITOR
      logit(cu->userid, msg);
#endif
    }
    return 0;
  }

  msg++;
  cmd = nextword(&msg);
  match = 0;

  if (*cmd == '/')
  {
    cmd++;
    if (!*cmd || str_equal(cmd, "help"))
    {
      view_action_verb(cu);
      match = 1;
    }
    else if (party_action(cu, cmd, msg) == 0)
      match = 1;
    else if (speak_action(cu, cmd, msg) == 0)
      match = 1;
    else
      match = condition_action(cu, cmd);
  }
  else
  {
    char *str;

    for (cmdrec = chatcmdlist; str = cmdrec->cmdstr; cmdrec++)
    {
      switch (cmdrec->exact)
      {
        case 1:  /* exactly equal */
          match = str_equal(cmd, str);
          break;
        case 2:  /* Thor: secret command */
          if(CHATSYSOP(cu))
            match = str_equal(cmd,str);
          break;
        default: /* not necessary equal */
          match = str_match(cmd, str) >= 0;
          break;
      }

      if (match)
      {
        cmdrec->cmdfunc(cu, msg);
        break;
      }
    }
  }

  if (!match)
  {
    sprintf(chatbuf, "�� ���O���~�G/%s", cmd);
    send_to_user(cu, chatbuf, 0);
  }
  return 0;
}

/* ----------------------------------------------------- */
/* serve chat_user's connection                          */
/* ----------------------------------------------------- */

int cuser_serve(cu)
  ChatUser *cu;
{
  register int ch, len;
  register char *str, *cmd;

  str = chatbuf;
  len = recv(cu->sock, str, sizeof(chatbuf), 0);
  if (len <= 0)
  {
    /* disconnected */

    exit_room(cu, EXIT_LOSTCONN, (char *) NULL);
    return -1;
  }

  str[len] = '\0';   /* Thor: ���T�w����ǰe protocol */

#ifdef  DEBUG
  logit(cu->userid, str);
#endif

  len = cu->isize;
  cmd = cu->ibuf + len;
  for (; ch = *str; str++)
  {
    if (ch == '\r')
      continue;
    if (ch == '\n')
    {
      *cmd = '\0';
      if (command_execute(cu) < 0)
        return -1;
      len = 0;
      break;
    }
    if (len < 79)
    {
      *cmd++ = ch;
      len++;
    }
  }
  cu->isize = len;
  return 0;
}

/* ---------------------------------------------------- */
/* chatroom server core routines                         */
/* ---------------------------------------------------- */

int start_daemon()
{
  int fd, value;
  char buf[80];
  struct sockaddr_in fsin;

  /*
   * More idiot speed-hacking --- the first time conversion makes the C
   * library open the files containing the locale definition and time zone.
   * If this hasn't happened in the parent process, it happens in the
   * children, once per connection --- and it does add up.
   */

  time_t dummy = time(NULL);
  struct tm *dummy_time = localtime(&dummy);
  struct tm *other_dummy_time = gmtime(&dummy);
  strftime(buf, 80, "%d/%b/%Y:%H:%M:%S", dummy_time);

  fd = getdtablesize();

  do
  {
    (void) close(--fd);
  } while (fd);

  if (fork())
    exit(0);

  fd = open(CHAT_PIDFILE, O_WRONLY | O_CREAT, 0664);
  if (fd >= 0)
  {
    sprintf(buf, "%5d\n", getpid());
    write(fd, buf, 6);
    close(fd);
  }

  if ((fd = open("/dev/tty", O_RDWR)) > 0)
  {
    ioctl(fd, TIOCNOTTY, 0);  /* Thor : �������٭n��  tty? */
    close(fd);
  }

  /* ------------------------------ */
  /* trap signals                   */
  /* ------------------------------ */

  for (fd = 1; fd < NSIG; fd++)
  {
    (void) signal(fd, SIG_IGN);
  }

  fd = socket(AF_INET, SOCK_STREAM, 0);

  value = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &value, sizeof(value));
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &value, sizeof(value));

  memset((char *) &fsin, 0, sizeof(fsin));
  fsin.sin_family = AF_INET;
  fsin.sin_addr.s_addr = htonl(INADDR_ANY);
  fsin.sin_port = htons(CHATPORT);

  if (bind(fd, (struct sockaddr *) & fsin, sizeof(fsin)) < 0)
    exit(1);

  openlog("xchatd3", LOG_PID, LOG_AUTH);
  syslog(LOG_NOTICE, "start\n");
  return fd;
}

void abort_server()
{
  if (time(0) - start_time < 60) {
     syslog(LOG_NOTICE, "Got SIGHUP at first 60 secs, ignore");
     return;
  }
  syslog(LOG_NOTICE, "abort");
  log_close();
  exit(1);
}

int main()
{
  register int msock, csock, nfds;
  register ChatUser *cu;
  register fd_set *rptr;
  fd_set rset;

  start_time = time(0);
  msock = start_daemon();

  (void) setgid(BBSGID);
  (void) setuid(BBSUID);
  (void) setpriority(PRIO_PROCESS, 0, 0);

  log_init();

  signal(SIGHUP, abort_server);

  /* ----------------------------- */
  /* init variable : rooms & users */
  /* ----------------------------- */

  (void) memset(&mainuser, 0, sizeof(mainuser));
  (void) memset(&mainroom, 0, sizeof(mainroom));
  (void) strcpy(mainroom.name, MAIN_NAME);
  (void) strcpy(mainroom.topic, MAIN_TOPIC);

  /* ----------------------------------- */
  /* main loop                           */
  /* ----------------------------------- */

  listen(msock, SOCK_QLEN);

  FD_ZERO(&mainfds);
  FD_SET(msock, &mainfds);
  rptr = &rset;
  maxfds = msock + 1;

  for (;;)
  {
    (void) memcpy(rptr, &mainfds, sizeof(fd_set));

    nfds = select(maxfds, rptr, NULL, NULL, NULL);

    /* check error condition */

    if (nfds < 0)
    {
      csock = errno;
      if (csock != EINTR)
      {
#ifndef BSD44
        extern char *sys_errlist[];
#endif

        logit("select", sys_errlist[csock]);
      }
      continue;
    }

    /* free idle user & chatroom's resource when no traffic */

    if (nfds == 0)
    {
      /* client/server �����Q�� ping-pong ��k�P�_ user �O���O�٬��� */
      /* �p�G client �w�g�����F�A�N����� resource */

      /* free_resource(); */
      continue;
    }

    /* accept new connection */

    if (FD_ISSET(msock, rptr))
    {
      for (;;)
      {
        csock = accept(msock, NULL, NULL);
        if (csock >= 0)
        {
          if (cu = (ChatUser *) malloc(sizeof(ChatUser)))
          {
            (void) memset(cu, 0, sizeof(ChatUser));
            cu->sock = csock;

            if (mainuser.next != NULL)
              mainuser.next->prev = cu;
            cu->next = mainuser.next;
            mainuser.next = cu;
            cu->prev = &mainuser;

            totaluser++;
            FD_SET(csock, &mainfds);
            if (csock >= maxfds)
              maxfds = csock + 1;

#ifdef  DEBUG
            logit("accept", "OK");
#endif
          }
          else
          {
            close(csock);
          }
          break;
        }
        csock = errno;
        if (csock != EINTR)
        {
#ifndef BSD44
          extern char *sys_errlist[];
#endif

          logit("accept", sys_errlist[csock]);
          break;
        }
      }

      FD_CLR(msock, rptr);

      if (--nfds <= 0)
        continue;
    }

    for (cu = mainuser.next; cu; cu = cu->next)
    {
      csock = cu->sock;
      if (FD_ISSET(csock, rptr))
      {
        if (cuser_serve(cu) < 0)
          logout_user(cu);

        FD_CLR(csock, rptr);
        if (--nfds <= 0)
          break;
      }
    }

    /* end of main loop */
  }
}
