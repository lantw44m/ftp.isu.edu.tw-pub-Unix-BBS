/*-------------------------------------------------------*/
/* struct.h     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : all definitions about data structure         */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#ifndef _STRUCT_H_
#define _STRUCT_H_


#define STRLEN   80             /* Length of most string data */
#define BRC_STRLEN 15           /* Length of boardname */
#define BTLEN    48             /* Length of board title */
#define TTLEN    72             /* Length of title */
#define NAMELEN  40             /* Length of username/realname */
#define FNLEN    33             /* Length of filename  */
				/* Ptt 說這裡有bug*/
#define IDLEN    12             /* Length of bid/uid */
#define PASSLEN  14             /* Length of encrypted passwd field */
#define REGLEN   38             /* Length of registration data */



typedef unsigned char uschar;   /* length = 1 */
typedef unsigned int usint;     /* length = 4 */

/* ----------------------------------------------------- */
/* .PASSWDS struct : 512 bytes                           */
/* ----------------------------------------------------- */
struct userec
{
  char userid[IDLEN + 1];         /* 使用者名稱  13 bytes */
  char realname[20];              /* 真實姓名    20 bytes */
  char username[24];              /* 暱稱        24 bytes */
  char passwd[PASSLEN];           /* 密碼        14 bytes */
  uschar uflag;                   /* 使用者選項   1 byte  */
  usint userlevel;                /* 使用者權限   4 bytes */
  ushort numlogins;               /* 上站次數     2 bytes */
  ushort numposts;                /* POST次數     2 bytes */
  time_t firstlogin;              /* 註冊時間     4 bytes */
  time_t lastlogin;               /* 前次上站     4 bytes */
  char lasthost[24];              /* 上站地點    24 bytes */
  char vhost[24];                 /* 虛擬網址    24 bytes */
  char email[50];                 /* E-MAIL      50 bytes */
  char address[50];               /* 地址        50 bytes */
  char justify[REGLEN + 1];       /* 註冊資料    39 bytes */
  uschar month;                   /* 出生月份     1 byte  */
  uschar day;                     /* 出生日期     1 byte  */
  uschar year;                    /* 出生年份     1 byte  */
  uschar sex;                     /* 性別         1 byte  */
  uschar state;                   /* 狀態??       1 byte  */
  usint habit;                    /* 喜好設定     4 bytes */
  uschar pager;                   /* 心情顏色     1 bytes */
  uschar invisible;               /* 隱身模式     1 bytes */
  usint exmailbox;                /* 信箱封數     4 bytes */
  usint exmailboxk;               /* 信箱K數      4 bytes */
  usint toquery;                  /* 好奇度       4 bytes */
  usint bequery;                  /* 人氣度       4 bytes */
  char toqid[IDLEN + 1];	  /* 前次查誰    13 bytes */
  char beqid[IDLEN + 1];	  /* 前次被誰查  13 bytes */
  unsigned long int totaltime;    /* 上線總時數   8 bytes */
  usint sendmsg;                  /* 發訊息次數   4 bytes */
  usint receivemsg;               /* 收訊息次數   4 bytes */
  unsigned long int goldmoney;	  /* 風塵金幣     8 bytes */ 
  unsigned long int silvermoney;  /* 銀幣         8 bytes */
  unsigned long int exp;          /* 經驗值       8 bytes */
  time_t dtime;                   /* 存款時間     4 bytes */
  int limitmoney;		  /* 金錢下限	  4 bytes */
  uschar rtimes;		  /* 填註冊單次數 1 bytes */
  int award;			  /* 獎懲判斷	  4 bytes */ 
  int pagermode; 		  /* 呼叫器門號   4 bytes */
  char pagernum[7];		  /* 呼叫器號碼   7 bytes */
  char feeling[5];		  /* 心情指數     5 bytes */
  char pad[123];                  /* 空著填滿至512用      */
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
/* RPG struct :256 bytes    		                 */
/* ----------------------------------------------------- */
struct rpgrec
{
  char userid[IDLEN+1];		  /* User ID     13 bytes */
  usint age;			  /* 年齡	  4 bytes */
  uschar race;			  /* 種族	  1 bytes */
  uschar subrace;		  /* 副業	  1 bytes */
  ushort level;			  /* 等級	  2 bytes */  
  char family[20];		  /* 家族	 20 bytes */
  char nick[20];		  /* 封號	 20 bytes */
  int hp;			  /* 體力	  4 bytes */
  int mp;			  /* 法力	  4 bytes */
  usint skill;			  /* 技能	  4 bytes */
  ushort str;			  /* 力量	  2 bytes */
  ushort dex;			  /* 敏捷	  2 bytes */
  ushort wis;			  /* 智慧	  2 bytes */
  ushort con;			  /* 體質	  2 bytes */
  ushort kar;			  /* 運氣	  2 bytes */
  uschar weapon;		  /* 武器	  1 bytes */
  uschar armor;			  /* 防具	  1 bytes */
  usint object;			  /* 物件	  4 bytes */
  char pad[164];
};
typedef struct rpgrec rpgrec;


/* ----------------------------------------------------- */
/* DIR of board struct : 128 bytes                       */
/* ----------------------------------------------------- */

struct fileheader
{
  char filename[FNLEN];         /* M.9876543210.A */
  char savemode;                /* file save mode */
  char owner[IDLEN + 2];        /* uid[.] */
  char date[6];                 /* [02/02] or space(5) */
  char title[TTLEN + 1];
//  time_t chrono;                /* timestamp */
//  char dummy;
  uschar filemode;              /* must be last field @ boards.c */
};
typedef struct fileheader fileheader;


struct PAL
{
  char userid[FNLEN];           /* list name/userid */
  char savemode;                
  char owner[IDLEN + 2];        /* /bbcall */
  char date[6];                 /* /birthday */
  char desc[TTLEN + 1];         /* list/user desc */
  uschar ftype;                 /* mode:  PAL, BAD */
};
typedef struct PAL PAL;

#define M_PAL		0x01
#define M_BAD		0x02
#define M_ALOHA		0x04

#define M_VISABLE	0x01
#define M_WATER		0x02
#define M_CANVOTE	0x04 

#define FILE_LOCAL      	0x1     /* local saved */
#define FILE_READ       	0x1     /* already read : mail only */
#define FILE_MARKED    		0x2     /* opus: 0x8 */
#define FILE_DIGEST     	0x4     /* digest */
#define FILE_TAGED      	0x8     /* taged */
#define FILE_PROBLEM    	0x10    /* problem post */
#define FILE_REPLYOK		0x10	/* reply ok : mail only*/
#define FILE_REFUSE		0x20	/* refuse */
#define FILE_DELETED		0x40	/* deleted */
#define FILE_FORFRIENDS         0x40    /* for friends */
#define MAIL_ALREADY_READ	0x1	/* mail already */
#define FILE_MULTI_SEND		0x2	/* mail multi send */
#define FILE_EDIT_AGAIN		0x4	/* file has edit again */

/* ----------------------------------------------------- */
/* Structure used in UTMP file : ??? bytes               */
/* ----------------------------------------------------- */

/* 電子雞 */
typedef struct pipdata
{
  char name[20];
  int hp;         /*體力*/
  int maxhp;      /*體力上限*/
  int mp;         /*法力*/
  int maxmp;      /*法力上限*/
  int attack;     /*攻擊*/
  int resist;     /*防禦*/
  int speed;      /*速度*/
  int mresist;    /*魔法防禦*/
  int resistmore; /*防禦型態*/
  int nodone;     /*完成*/
  int leaving;    /*離開*/
  int pipmode;    /*狀態*/
  int money;      /*金錢*/
  int msgcount;   /*訊息個數*/
  int chatcount;
  char msg[150];  /*訊息內容*/
  char chat[10][150]; /*聊天內容*/
}pipdata;


/*
woju
Message queue
*/
typedef struct {
   pid_t last_pid;
   char last_userid[IDLEN + 1];
   char last_call_in[80];
} msgque;

struct user_info
{
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
  char from[32];                /* machine name the user called in from */
  int from_alias;
  char birth;                   /* 是否是生日 Ptt*/
  char tty[11];                 /* tty port */
  ushort friend[MAX_FRIEND];
  ushort reject[MAX_REJECT];
  uschar msgcount;
  msgque msgs[MAXMSGS];
  time_t uptime;
  time_t lastact;             /* 上次使用者動的時間 */
  usint brc_id;
  uschar lockmode;
  pipdata pip;
  int turn;
  char feeling[5];		/* 心情 */
  char cursor[16];
  char hit[16];
};
typedef struct user_info user_info;


/* ----------------------------------------------------- */
/* BOARDS struct : 256/512 bytes                             */
/* ----------------------------------------------------- */
#define BRD_NOZAP       00001         /* 不可zap  */
#define BRD_NOCOUNT     00002         /* 不列入統計 */
#define BRD_OUTGOING    00004         /* 轉出 */
#define BRD_GROUPBOARD  00010         /* 群組板 */
#define BRD_HIDE        00020         /* 隱藏板 (看板好友才可看) */
#define BRD_POSTMASK    00040         /* 限制發表或閱讀 */
#define BRD_BRDMAIL     00100         /* 接受寄信到看板 */
#define BRD_CLASS	00200         /* 分類看板 */
#define BRD_GOOD	00400         /* 優良看板 */
#define BRD_PERSONAL	01000         /* 個人看板 */
#define BRD_NOFOWARD	02000	      /* 禁止轉錄 */
#define BRD_INGOING     04000         /* 轉入 */
/* #define BRD_ANONYMOUS   10000 */        /* 匿名板? */

struct boardheader
{
  char brdname[IDLEN + 1];      /* 看板英文名稱    13 bytes */
  char title[BTLEN + 1];        /* 看板中文名稱    49 bytes */
  char BM[IDLEN * 3 + 3];       /* 板主ID和"/"     39 bytes */
  usint brdattr;                /* 看板的屬性       4 bytes */
  time_t bupdate;               /* note update time 4 bytes */
  uschar bvote;                 /* Vote flags       1 bytes */
  time_t vtime;                 /* Vote close time  4 bytes */
  usint level;                  /* 可以看此板的權限 4 bytes */
  unsigned long int totalvisit; /* 總拜訪人數       8 bytes */
  unsigned long int totaltime;  /* 總停留時間       8 bytes */
  char lastvisit[IDLEN + 1];    /* 最後看該板的人  13 bytes */
  time_t opentime;              /* 開板時間         4 bytes */
  time_t lastime;               /* 最後拜訪時間     4 bytes */
  char passwd[PASSLEN];         /* 密碼            14 bytes */
  unsigned long int postotal;   /* 總水量 :p        8 bytes */
//  usint maxpost;
//  usint maxtime;
//  char desc[3][80]
//  char pad[87];
  char pad[79];
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
};

struct BCACHE
{
  boardheader bcache[MAXBOARD];
  usint total[MAXBOARD];
  time_t lastposttime[MAXBOARD];
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
/* Ptt */
struct FILMCACHE
{
  char notes[MAX_MOVIE][FNLEN];
  char today_is[20];
  int max_film;
  int max_history;
  time_t uptime;
  time_t touchtime;
  int busystate;
};

struct FROMCACHE
{
  char domain[MAX_FROM][50];
  char replace[MAX_FROM][50];
  int top;
  int max_user;
  time_t max_time;
  time_t uptime;
  time_t touchtime;
  int busystate;
};

struct BACACHE          
{                       
  char author[300][100];
  int top;              
  time_t uptime;        
  time_t touchtime;     
  int busystate;        
};                      

struct hosts
{
 char shortname[24];
 char address[40];
 char desc[24];
};

typedef struct hosts hosts;



/* ----------------------------------------------------- */
/* screen.c 中運用的資料結構                             */
/* ----------------------------------------------------- */

#define ANSILINELEN (250)       /* Maximum Screen width in chars */

/* Line buffer modes */
#define MODIFIED (1)            /* if line has been modifed, screen output */
#define STANDOUT (2)            /* if this line has a standout region */

#define SL_MODIFIED	(1)	/* if line has been modifed, screen output */
#define SL_STANDOUT	(2)	/* if this line contains standout code */
#define SL_ANSICODE	(4)	/* if this line contains ANSI code */

struct screenline
{
  uschar oldlen;                /* previous line length */
  uschar len;                   /* current length of line */
  uschar width;			/* padding length of ANSI codes */
  uschar mode;                  /* status of line, as far as update */
  uschar smod;                  /* start of modified data */
  uschar emod;                  /* end of modified data */
  uschar sso;                   /* start stand out */
  uschar eso;                   /* end stand out */
  uschar data[ANSILINELEN];
};
typedef struct screenline screenline;

typedef struct LinkList
{
  struct LinkList *next;
  char data[0];
}        LinkList;

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

#define WRAPMARGIN (250)

struct textline
{
  struct textline *prev;
  struct textline *next;
  int len;
  char data[WRAPMARGIN + 1];
};
typedef struct textline textline;

#endif                          /* _STRUCT_H_ */

/* ----------------------------------------------------- */
/* announce.c                                            */
/* ----------------------------------------------------- */

#define MAXITEMS        1000     /* 一個目錄最多有幾項 */

typedef struct
{
  fileheader *header;
  char mtitle[STRLEN];
  char *path;
  int num, page, now, level;
} AMENU;

union x_item
{
  struct                        /* bbs_item */
  {
    char fdate[9];              /* [mm/dd/yy] */
    char editor[13];            /* user ID */
    char fname[31];
  }      B;

  struct                        /* gopher_item */
  {
    char path[81];
    char server[48];
    int port;
  }      G;
};

typedef struct
{
  char title[63];
  union x_item X;
}      ITEM;

typedef struct
{
  ITEM *item[MAXITEMS];
  char mtitle[STRLEN];
  char *path;
  int num, page, now, level;
}      GMENU;


/* 答錄機的 struct */

struct notedata {
  time_t date;
  char userid[IDLEN + 1];
  char username[19];
  char buf[3][80];
};
typedef struct notedata notedata;

// bwboard 用到的

typedef struct
{
  int key;
  int (*func) ();
}      KeyFunc;

/* ----------------------------------------------------- */
/* mn.c 中運用的資料結構                                 */
/* ----------------------------------------------------- */
struct  money_note
{
  usint  year;      // 年         4 b
  uschar month;     // 月         1 b
  uschar day;       // 日         1 b
  uschar flag;      // 收入/支出  1 b
  usint  money;     // 金額       4 b
  uschar use_way;   // 類別(食衣住行育樂?)    1b
  char   desc[50];  // 說明       50 b
  char   pad[62];   // null pad   62 b
};
typedef struct money_note MN;

#define MODE_OUT	0x1	// 支出
#define MODE_IN		0x2	// 收入

#define	WAY_EAT		0	// 食
#define WAY_WEAR	1	// 衣
#define	WAY_LIVE	2	// 住
#define WAY_MOVE	3	// 行
#define	WAY_EDU		4	// 育
#define	WAY_PLAY	5	// 樂
#define WAY_OTHER	6	// 其他

struct MENU
{
  void *cmdfunc;
  usint level;
  char *desc;/* next/key/description */
  int mode;
};
typedef struct MENU MENU;

