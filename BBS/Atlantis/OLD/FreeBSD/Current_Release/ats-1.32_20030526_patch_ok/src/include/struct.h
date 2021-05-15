/*-------------------------------------------------------*/
/* struct.h                   (SOB 0.22 ATS Verion 1.31) */
/*-------------------------------------------------------*/
/* target : all definitions about data structure         */
/* create : 95/03/29                                     */
/* update : 02/08/26 (Dopin)                             */
/*-------------------------------------------------------*/

#ifndef _STRUCT_H_
#define _STRUCT_H_

#define STRLEN   80             /* Length of most string data */
#define BTLEN    48             /* Length of board title */
#define TTLEN    72             /* Length of title */
#define NAMELEN  40             /* Length of username/realname */
#define FNLEN    33             /* Length of filename  */
#define IDLEN    12             /* Length of bid/uid */

#ifdef ENCPASSLEN
  #define PASSLEN  35           /* Length of encrypted passwd field for FB3 */
#else
  #define PASSLEN  14           /* Length of encrypted passwd field for ATS */
#endif

#define REGLEN   38             /* Length of registration data */

typedef unsigned char uschar;   /* length = 1 */
typedef unsigned int usint;     /* length = 4 */

/* ----------------------------------------------------- */
/* .PASSWDS struct : 1024 bytes                          */
/* ----------------------------------------------------- */

struct userec {
  char userid[IDLEN + 1];
  char realname[20];
  char username[24];
  char passwd[PASSLEN];
  uschar uflag;

  usint userlevel;
  unsigned long int numlogins;
  unsigned long int numposts;
  time_t firstlogin;
  time_t lastlogin;
  char lasthost[80];
  char remoteuser[8];
  char email[50];
  char address[50];
  char justify[REGLEN + 1];
  uschar month;
  uschar day;
  uschar year;
  uschar sex;
  uschar state;

  int havemoney;

#ifndef MIN_USEREC_STRUCT
  int havetoolsnumber;
  int havetools[20];
  int addexp;
  usint nowlevel;
  char working[20];

  uschar hp;
  uschar str;
  uschar mgc;
  uschar skl;
  uschar spd;
  uschar luk;
  uschar def;
  uschar mdf;

  uschar spcskl[6];
  uschar wepnlv[2][10];
  uschar curwepnlv[2][1];
  uschar curwepnatt;
  uschar curwepnhit;
  uschar curwepnweg;
  uschar curwepnspc[4];

  char lover[IDLEN+1];
  char commander;
  char belong[21];
  char curwepclass[10];
  char class[7];
#endif

#ifndef NO_USE_MULTI_STATION
  char station[IDLEN+1];
#endif

#ifndef MIN_USEREC_STRUCT
  char classsex;
  char wephavespc[5];

  char cmdrname[IDLEN+1];
  char class_spc_skll[6];
#endif

  usint welcomeflag;

#ifndef MIN_USEREC_STRUCT
  int win;
  int lost;
  int test_exp;
#endif

  char tty_name[20];
  char extra_mode[10];

#ifndef MIN_USEREC_STRUCT
  char class_spc_test[32];
  char mov;
  char toki_level;

  int will_value;
  int effect_value;
  int belive_value;
  int leader_value;

  char action_value;
#endif

#ifndef NO_USE_MULTI_STATION
  char station_member[40];
#endif

  uschar now_stno;
  usint good_posts;

#ifdef CHANGE_USER_MAIL_LIMIT
  int max_mail_number;
  int max_mail_kbytes;
#endif

#ifdef TRANS_FROM_COLA
  usint staytime;
  #ifdef CHANGE_USER_MAIL_LIMIT
    time_t last_post_time;
    time_t count_time;
    int post_number;
    unsigned int reject_me_time;
    int backup_int[37];
  #else
    time_t last_post_time;
    time_t count_time;
    int post_number;
    unsigned int reject_me_time;
    int backup_int[39];
  #endif
#else
  #ifdef CHANGE_USER_MAIL_LIMIT
    time_t last_post_time;
    time_t count_time;
    int post_number;
    unsigned int reject_me_time;
    int backup_int[38];
  #else
    time_t last_post_time;
    time_t count_time;
    int post_number;
    unsigned int reject_me_time;
    int backup_int[40];
  #endif
#endif

#ifndef MIN_USEREC_STRUCT
  int ara_money;
#endif

#ifdef TRANS_FROM_COLA
  char blood;
  char normal_post;
  char backup_char[118];
#else
  char normal_post;
  char backup_char[119];
#endif

#ifndef MIN_USEREC_STRUCT
  int turn;
#endif
};
typedef struct userec userec;

/* these are flags in userec.uflag */
#define SIG_FLAG        0x3     /* signature number, 2 bits */
#define PAGER_FLAG      0x4     /* true if pager was OFF last session */
#define CLOAK_FLAG      0x8     /* true if cloak was ON last session */
#define FRIEND_FLAG     0x10    /* true if show friends only */
#define BRDSORT_FLAG    0x20    /* true if the boards sorted alphabetical */
#define MOVIE_FLAG      0x40    /* true if show movie */
#define COLOR_FLAG      0x80    /* true if the color mode open */

/* ----------------------------------------------------- */
/* DIR of board struct                                   */
/* ----------------------------------------------------- */

#ifndef TRANS_FROM_FB3          /* struct size = 256 bytes */
  #ifndef TRANS_FROM_COLA
struct fileheader {
  char filename[FNLEN-1];       /* M.109876543210.A */
  char report;                  /* Dopin : 新制提報 */
  char savemode;                /* file save mode */
  char owner[IDLEN + 2];        /* uid[.] */
  char date[6];                 /* [02/02] or space(5) */
  char title[TTLEN];
  uschar goodpost;              /* 推薦文章 */
  uschar filemode;              /* must be last field @ boards.c */
};
  #else
struct fileheader {             /* For Cola BBS */
  char filename[STRLEN];
  char owner[STRLEN];
  char title[STRLEN];
  char date[6];                 /* 補上 for ATS/SOB */
  char savemode;
  uschar filemode;
  char report;
  uschar goodpost;
  char backup_char[6];          /* 到這裡 */
};
  #endif
#else
struct fileheader {             /* This structure is used to hold data in */
  char filename[STRLEN-2];      /* the DIR files */
  char report;                  /* 亞站提報 */
  char savemode;                /* file save mode */
  char owner[STRLEN-6];
  char date[6];
  char title[STRLEN];
  unsigned level;
  unsigned char accessed[10];
  uschar goodpost;              /* 推薦文章 */
  uschar filemode;              /* must be last field @ boards.c */
};
#endif

typedef struct fileheader fileheader;

#define FILE_LOCAL      0x1     /* local saved */
#define FILE_READ       0x1     /* already read : mail only */
#define FILE_MARKED     0x2     /* opus: 0x8 */
#define FILE_DIGEST     0x4     /* digest */
#define FILE_TAGED      0x8     /* taged */

/* ----------------------------------------------------- */
/* Menu Commands struct                                  */
/* ----------------------------------------------------- */

struct commands
{
  int (*cmdfunc) ();
  int level;
  char *desc;                   /* next/key/description */
  int mode;                     /* dso mode */
};
typedef struct commands commands;

/* ----------------------------------------------------- */
/* Structure used in UTMP file : 128 bytes               */
/* ----------------------------------------------------- */

/* woju Message queue */
typedef struct {
   pid_t last_pid;

#ifdef RECORD_NEW_MSG
   char newmsg;
#endif

   char last_userid[IDLEN + 1];
   char last_call_in[80];
} msgque;

struct user_info {
  int uid;                      /* Used to find user name in passwd file */
  pid_t pid;                    /* kill() to notify user of talk request */
  int sockaddr;                 /* ... */
  int destuid;                  /* talk uses this to identify who called */
  struct user_info* destuip;
  uschar active;                /* When allocated this field is true */
  uschar invisible;             /* Used by cloaking function in Xyz menu */
  uschar sockactive;            /* Used to coordinate talk requests */
  usint userlevel;
  uschar mode;                  /* UL/DL, Talk Mode, Chat Mode, ... */
  uschar pager;                 /* pager toggle, YEA, or NA */
  uschar in_chat;               /* for in_chat commands   */
  uschar sig;                   /* signal type */
  char userid[IDLEN + 1];
  char chatid[11];              /* chat id, if in chat mode */
  char realname[20];
  char username[24];
  char from[29];                /* machine name the user called in from */
  char tty[11];                 /* tty port */
  ushort friend[MAX_FRIEND];
  ushort aloha[MAX_ALOHA];      /* 新制上線通知 */
  ushort reject[MAX_REJECT];
  time_t uptime;
  msgque msgs;

#ifndef NO_USE_MULTI_STATION
  uschar now_stno;
#else
  char kimochi[IDLEN+1];
#endif
  uschar welcomeflag;

#ifdef MIN_USEREC_STRUCT
  int turn;
#endif
};
typedef struct user_info user_info;

/* ----------------------------------------------------- */
/* BOARDS struct : Standard 656 bytes                    */
/* ----------------------------------------------------- */

struct boardheader {
  char brdname[(IDLEN+1)*2];      /* bid */
  char title[BTLEN + 1];
  char BM[IDLEN * 3 + 3];       /* BMs' uid, token '/' */
  char pad[11];
  time_t bupdate;               /* note update time */
  char pad2[3];
  uschar bvote;                 /* Vote flags */
  time_t vtime;                 /* Vote close time */
  usint level;
  char document[128 * 3];       /* add extra document */
  char station[16];
  char sysop[16];
  char pastbrdname[16];
  char yankflags[16];
  char backup[64];
};
typedef struct boardheader boardheader;

struct one_key
{                               /* Used to pass commands to the readmenu */
  int key;
  int (*fptr) ();
};

/* ----------------------------------------------------- */
/* cache.c 中運用的資料結構                              */
/* ----------------------------------------------------- */

#define USHM_SIZE       (MAXACTIVE + 4)
struct UTMPFILE
{
  user_info uinfo[USHM_SIZE];
  time_t uptime;
  int number;
  int busystate;
  double sysload[3];
};

struct BCACHE
{
  boardheader bcache[MAXBOARD];
  time_t uptime;
  time_t touchtime;
  int number;
  int busystate;
};

struct UCACHE
{
  char userid[MAXUSERS][IDLEN + 1];
  time_t uptime;
  time_t touchtime;
  int number;
  int busystate;
};

/* ----------------------------------------------------- */
/* screen.c 中運用的資料結構                             */
/* ----------------------------------------------------- */

#ifdef MBBSD
int (*flushf) ();

#define ANSILINELEN (255)       /* Maximum Screen width in chars */
                                /* 假設 160 個就夠用了，最多可以到 255 */

/* Screen Line buffer modes */

#define SL_MODIFIED     (1)     /* if line has been modifed, screen output */
#define SL_STANDOUT     (2)     /* if this line has a standout region */
#define SL_ANSICODE     (4)     /* if this line contains ANSI code */

struct screenline
{
  uschar oldlen;                /* previous line length */
  uschar len;                   /* current length of line */
  uschar mode;                  /* status of line, as far as update */
  uschar smod;                  /* start of modified data */
  uschar emod;                  /* end of modified data */
  uschar sso;                   /* start stand out */
  uschar eso;                   /* end stand out */
  uschar width;                 /* line width except ANSI codes */
  uschar data[ANSILINELEN];
};
typedef struct screenline screenline;
extern screenline *visiop;

#else

#define ANSILINELEN (255)       /* Maximum Screen width in chars */

/* Line buffer modes */
#define MODIFIED (1)            /* if line has been modifed, screen output */
#define STANDOUT (2)            /* if this line has a standout region */

struct screenline
{
  uschar oldlen;                /* previous line length */
  uschar len;                   /* current length of line */
  uschar mode;                  /* status of line, as far as update */
  uschar smod;                  /* start of modified data */
  uschar emod;                  /* end of modified data */
  uschar sso;                   /* start stand out */
  uschar eso;                   /* end stand out */
  unsigned char data[ANSILINELEN + 1];
};
typedef struct screenline screenline;

#endif

/* ----------------------------------------------------- */
/* name.c 中運用的資料結構                               */
/* ----------------------------------------------------- */

struct word
{
  char *word;
  struct word *next;
};

/* ----------------------------------------------------- */
/* edit.c 中運用的資料結構                               */
/* ----------------------------------------------------- */

#define WRAPMARGIN (255)

struct textline
{
  struct textline *prev;
  struct textline *next;
  int len;
  char data[WRAPMARGIN + 1];
};
typedef struct textline textline;

/* ----------------------------------------------------- */
/* 故鄉轉換資料結構                 Atlantis BBS 1.32-A  */
/* ----------------------------------------------------- */

#ifdef FROMHOST_BY_FILE
struct fh2st {
  char host_type;     /* 0: strcmp 1:strstr */
  char fromhost[42];  /* 這邊浪費三字元 為的只是 對齊 64 bytes 比較快 */
  char transto[21];
};
typedef struct fh2st fh2st;
#endif

/* ----------------------------------------------------- */
/* 點歌暫存檔資料結構               Atlantis BBS 1.32-B  */
/* ----------------------------------------------------- */

#ifdef OSONG_ANN
struct song {
   char type;
   char to[IDLEN+1];
   char f_path[256];
   char title[TTLEN];
   char msg[26];
};
typedef struct song song;
#endif

/* ----------------------------------------------------- */
/* 目錄看板資料結構 (含移至這裡的)  Atlantis BBS 1.32-R  */
/* ----------------------------------------------------- */

typedef struct {
  char bm[IDLEN+1];
  char name[(IDLEN+1)*2];
  char spath[16];
  char title[TTLEN + 1];
}      TRS;

union IF {
  struct {
    fileheader *header;
  }    F;

  struct {
    TRS *header;
  }    I;
};

typedef struct {
  union IF p;
  char mtitle[STRLEN];
  char *path;
  int num, page, now, level;
}      MENU;

/* ----------------------------------------------------- */
/* 看板狀態結構                   原位於 SOB --> board.c */
/* ----------------------------------------------------- */

typedef struct {
  int pos, total;
  char *name, *title, *BM;
  uschar unread, zap, bvote;
} boardstat;

/* End of _STRUCT_H_ */
#endif
