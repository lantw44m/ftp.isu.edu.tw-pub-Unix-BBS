/*-------------------------------------------------------*/
/* config.h     ( WD-BBS Ver 2.3 )			 */
/*-------------------------------------------------------*/
/* target : site-configurable settings                   */
/* create : 95/03/29                                     */
/* update : 98/12/09                                     */
/*-------------------------------------------------------*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

/* ----------------------------------------------------- */
/* 定義 BBS 站名位址                                     */
/* ------------------------------------------------------*/

#define BOARDNAME       "紅樓頂尖"			/* 中文站名 */
#define BBSNAME         "NIIT EXBBS"			/* 英文站名 */
#define MYHOSTNAME      "exbbs.ilantech.edu.tw"		/* 網址 */
#define MYIP		"140.111.79.2"			/* IP */
#define MYVERSION       "WD-BBS 2.3"			/* 程式版本 */
#define MYBBSID         "EXBBS"				/* 轉信代碼 */
#define BBSHOME         "/bbs"				/* BBS 的家 */
#define BBSUSER         "bbs"
#define BBSUID          999
#define BBSGID          900
#define BBSSHELL        "/bbs/bin/bbsrf"		/* 前端程式 */
#define BBSPROG         "/bbs/bin/bbs"			/* 主程式 */
#define TAG_VALID	"[EX BBS]To "

#define CURSOR_BAR
#ifdef CURSOR_BAR
 #define BAR_COLOR "[1;33;46m"
#endif


/* ----------------------------------------------------- */
/* 組態規劃                                              */
/* ------------------------------------------------------*/

#define HAVE_CHKLOAD		/* check cpu load */
#ifdef HAVE_CHKLOAD
  #define MAX_CPULOAD     (32)            /* CPU 最高load */
  #define MAX_SWAPUSED    (32)            /* SWAP最高使用率 */
#endif

#define WITHOUT_CHROOT		/* 不需要 root set-uid */
#define HAVE_MOVIE		/* 顯示動態告示板 */
#define INTERNET_PRIVATE_EMAIL	/* 可以寄私人信件到 Internet */
#define BIT8			/* 支援中文系統 */
#define DEFAULTBOARD	"SYSOP"	/* 預設看板 */
#define LOGINATTEMPTS	(3)	/* 最大進站失誤次數 */
#define LOGINASNEW		/* 採用上站申請帳號制度 */
#define VOTE		(1)	/* 提供投票功能 */
#define INTERNET_EMAIL		/* 支援 InterNet Email 功能(含 Forward) */
#define EMAIL_JUSTIFY		/* 發出 InterNet Email 身份認證信函 */
#undef  NEWUSER_LIMIT		/* 新手上路的三天限制 */
#define POSTNOTIFY		/* 新文章通知功能 */
#undef  INVISIBLE		/* 隱藏 BBS user 來自何處 */
#define MULTI_NUMS	(2)	/* 最多重複上站人次 (guest除外) */
#define MULTI_GUEST	(16)	/* 最多重複上站的guest */
#define INITIAL_SETUP		/* 剛開站，還沒建立預設看板[SYSOP] */
#define HAVE_MMAP		/* 採用 mmap(): memory mapped I/O */
#define HAVE_ANONYMOUS		/* 提供 Anonymous 板 */
#define HAVE_ORIGIN		/* 顯示 author 來自何處 */
#define HAVE_MAILCLEAN		/* 清理所有使用者個人信箱 */
#define HAVE_SUICIDE		/* 提供使用者自殺功能 */
#define WHERE			/* 是否有故鄉功能 */
#define HAVE_NOTE_2		/* wildcat:小看板功能 */

/* ----------------------------------------------------- */
/* 隨 BBS 站規模成長而擴增                               */
/* ----------------------------------------------------- */

#define MAXUSERS        (65536)         /* 最高註冊人數 */
#define MAXBOARD        (512)           /* 最大開版個數 */
#define MAXACTIVE       (512)           /* 最多同時上站人數 */
#define MAX_FRIEND      (128)           /* 載入 cache 之最多朋友數目 */
#define MAX_REJECT      (128)            /* 載入 cache 之最多壞人數目 */
#define MAX_LOGIN_INFO  (128)           /* 最多上線通知人數 */
#define MAX_POST_INFO   (128)            /* 最多新文章通知人數 */
#define MAX_NAMELIST    (128)           /* 最多其他特別名單人數 */
#define MAX_MOVIE       (128)		/* 最多動態看版數 */
#define MAX_FROM        (128)		/* 最多故鄉數 */
#define MAX_REVIEW      (32)            /* 最多水球回顧 */

/* ----------------------------------------------------- */
/* 其他系統上限參數                                      */
/* ----------------------------------------------------- */

#define MAX_HISTORY     64		/* 動態看板保持 12 筆歷史記錄 */
#define MAXKEEPMAIL     (100)		/* 最多保留幾封 MAIL？ */
#define MAXEXKEEPMAIL   (500)		/* 最多信箱加大多少封 */
#define MAX_NOTE        (64)            /* 最多保留幾篇留言？ */
#define MAXSIGLINES     (8)             /* 簽名檔引入最大行數 */
#define MAXQUERYLINES   (16)            /* 顯示 Query/Plan 訊息最大行數 */
#define MAXPAGES        (900)           /* more.c 中文章頁數上限 (lines/22) */
#define MAXMSGS         (16)		/* 水球(熱訊)忍耐上限 */
#define MOVIE_INT       (12)		/* 切換動畫的週期時間 12 秒 */

/* ----------------------------------------------------- */
/* 發呆過久自動簽退    */
/* ------------------------------------------------------*/

#define DOTIMEOUT

#ifdef  DOTIMEOUT
#define IDLE_TIMEOUT    (29*60)		/* 一般情況之 timeout */
// #define MONITOR_TIMEOUT (29*60)         /* monitor 時之 timeout */
#define SHOW_IDLE_TIME                  /* 顯示閒置時間 */
#endif

/* ----------------------------------------------------- */
/* chat.c & xchatd.c 中採用的 port 及 protocol           */
/* ------------------------------------------------------*/

#define CHATPORT	3838

#define MAXROOM         32             /* 最多有幾間包廂？ */

#define EXIT_LOGOUT     0
#define EXIT_LOSTCONN   -1
#define EXIT_CLIERROR   -2
#define EXIT_TIMEDOUT   -3
#define EXIT_KICK       -4

#define CHAT_LOGIN_OK       "OK"
#define CHAT_LOGIN_EXISTS   "EX"
#define CHAT_LOGIN_INVALID  "IN"
#define CHAT_LOGIN_BOGUS    "BG"

#define BADCIDCHARS " *:/\,;.?"        /* Chat Room 中禁用於 nick 的字元 */


/* ----------------------------------------------------- */
/* 系統參數      cache                                   */
/* ----------------------------------------------------- */
#define MAGIC_KEY       1999   /* 身分認證信函編碼 */

#define SEM_ENTER      -1      /* enter semaphore */
#define SEM_LEAVE      1       /* leave semaphore */
#define SEM_RESET      0       /* reset semaphore */

#define BRDSHM_KEY      1215
#define UIDSHM_KEY      1217
#define UTMPSHM_KEY     1219
#define FILMSHM_KEY     1221    /* 動態看版 , 節日 */
#define FROMSHM_KEY     1223    /* whereis, 最多使用者 */

#define BRDSEM_KEY      2005
#define FILMSEM_KEY     2000    /* semaphore key */
#define FROMSEM_KEY     2003    /* semaphore key */

/* ----------------------------------------------------- */
/* 申請帳號時要求申請者真實資料                          */
/* ----------------------------------------------------- */

#define SHOWUID                 /* 看見使用者 UID */
#define SHOWTTY                 /* 看見使用者 TTY */
#define SHOWPID                 /* 看見使用者 PID */

#define REALINFO                /* 真實姓名 */

#ifdef  REALINFO
#undef  ACTS_REALNAMES          /* 主目錄的 (U)ser 顯示真實姓名 */
#undef  POST_REALNAMES          /* 貼文件時附上真實姓名 */
#undef  MAIL_REALNAMES          /* 寄站內信件時附上真實姓名 */
#undef  QUERY_REALNAMES         /* 被 Query 的 User 告知真實姓名 */
#endif

/* ----------------------------------------------------- */
/* http                                                  */
/* ----------------------------------------------------- */

#define USE_LYNX   	      /* 使用外部lynx dump ? */
#undef USE_PROXY
#ifdef  USE_PROXY
#define PROXYSERVER "140.111.72.8"
#define PROXYPORT   3128
#endif

#define LOCAL_PROXY           /* 是否開啟local 的proxy */
#ifdef  LOCAL_PROXY
#define HPROXYDAY   3         /* local的proxy refresh天數 */
#endif

/* ----------------------------------------------------- */
/* 提供外掛程式                                          */
/* ----------------------------------------------------- */

#define HAVE_EXTERNAL

  #ifdef HAVE_EXTERNAL
    #undef HAVE_TIN        /* 提供 news reader */
    #undef HAVE_GOPHER     /* 提供 gopher */
    #undef HAVE_WWW        /* 提供 www browser */
    #define HAVE_BIG2       /* 提供 big2 game */
    #define HAVE_MJ         /* 提供 mj game */
    #define HAVE_BRIDGE     /* 提供 Ok Bridge */
    #undef HAVE_MUD        /* 提供 MUD */
    #define HAVE_CAL        /* 提功計算機 */
    #undef HAVE_ARCHIE     /* have arche */
    #define BBSDOORS        /* 提供 bbs doors */
    #define HAVE_GAME       /* 提供網路連線遊戲 */
    #define HAVE_ADM_SHELL  /* 提供 csh 以便管理 */
  #endif

#endif

/* ----------------------------------------------------- */
/* 定義基本顏色                                          */
/* ----------------------------------------------------- */

#define ANNOUNCE_BRD	"Announce"
#define VOTEBOARD	"VoteBoard"

#define COLOR1	"[46;37m"
#define COLOR2	"[1m[44;33m"
