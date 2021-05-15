/*-------------------------------------------------------*/
/* perm.h       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : permission levels of user & board            */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#ifndef _PERM_H_
#define _PERM_H_

/* ----------------------------------------------------- */
/* These are the 16 basic permission bits.               */
/* ----------------------------------------------------- */

#define PERM_BASIC        000000000001
#define PERM_CHAT         000000000002
#define PERM_PAGE         000000000004
#define PERM_POST         000000000010
#define PERM_LOGINOK      000000000020
#define PERM_MAILLIMIT    000000000040
#define PERM_CLOAK        000000000100
#define PERM_SEECLOAK     000000000200
#define PERM_XEMPT        000000000400
#define PERM_DENYPOST     000000001000
#define PERM_BM           000000002000
#define PERM_ACCOUNTS     000000004000
#define PERM_CHATROOM     000000010000
#define PERM_BOARD        000000020000
#define PERM_POSTMASK     000000040000 
#define PERM_BBSADM       000000040000 
#define PERM_SYSOP        000000100000
#define PERM_NOTOP        000000200000 
#define PERM_XFILE        000000400000 
#define PERM_RESEARCH	  000001000000 
#define PERM_FROM         000002000000 
#define PERM_TEACHER      000004000000 
#define PERM_FORBIDPOST   000010000000 
#define PERM_ANNOUNCE     000020000000 
#define PERM_LAW          000040000000 
#define PERM_LAWADM       000100000000 
#define PERM_SYSTEM       000200000000 
#define PERM_SYSTEMADM    000400000000 
#define PERM_HACKER       001000000000 
#define PERM_GUARD        002000000000 
#define PERM_SECRETARY    004000000000 
#define PERM_SUPER        010000000000 
#define PERM_GOD          020000000000 

/* ----------------------------------------------------- */
/* These permissions are bitwise ORs of the basic bits.  */
/* ----------------------------------------------------- */

 
/* 新使用者擁有的權限 */
/* wildcat :
   必要時 可把PERM_POST拿掉，改為必須email認證後方有 */
#define PERM_DEFAULT    (PERM_BASIC | PERM_FORBIDPOST)

#define PERM_ADMIN      (PERM_ACCOUNTS | PERM_SYSOP)
#define PERM_ALLBOARD   (PERM_SYSOP | PERM_BOARD)
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS)
#define PERM_SEEULEVELS PERM_SYSOP
#define PERM_SEEBLEVELS (PERM_SYSOP | PERM_BM)
#define PERM_NOTIMEOUT  PERM_SYSOP

#define PERM_READMAIL   PERM_BASIC
#define PERM_FORWARD    PERM_BASIC      /* to do the forwarding */

#define PERM_INTERNET   PERM_LOGINOK    /* 身份認證過關的才能寄信到 Internet */

#define HAS_PERM(x)     ((x)?cuser.userlevel&(x):1)
#define HAVE_PERM(x)    (cuser.userlevel&(x))
#define PERM_HIDE(u) ((u)->userlevel & PERM_SYSOP && (u)->userlevel & PERM_DENYPOST)


/* ----------------------------------------------------- */
/* 各種權限的中文意義                                    */
/* ----------------------------------------------------- */

#define NUMPERMS        32

#ifndef _ADMIN_C_
extern char *permstrings[];

#else
char *permstrings[] = {
  "基本權力",                   /* PERM_BASIC */
  "進入聊天室",                 /* PERM_CHAT */
  "找人聊天",                   /* PERM_PAGE */
  "發表文章",                   /* PERM_POST */
  "註冊程序認證",               /* PERM_LOGINOK */
  "信件無上限",                 /* PERM_MAILLIMIT */
  "隱身術",                     /* PERM_CLOAK */
  "看見忍者",                   /* PERM_SEECLOAK */
  "永久保留帳號",               /* PERM_XEMPT */
  "站長隱身術",                 /* PERM_DENYPOST */
  "板主",                       /* PERM_BM */
  "帳號總管",                   /* PERM_ACCOUNTS */
  "聊天室總管",                 /* PERM_CHATCLOAK */
  "看板總管",                   /* PERM_BOARD */
  "站務助理",                   /* PERM_BBSADM & PERM_POSTMASK */
  "站長",                       /* PERM_SYSOP */
  "不列入排行榜",               /* PERM_NOTOP */
  "管理站內 X檔案",             /* PERM_XFILE */
  "系統測試人員",               /* PERM_RESEARCH */
  "修改故鄉",                   /* PERM_FROM */
  "電子系老師",                 /* PERM_TEACHER */
  "禁止發表文章",               /* PERM_FORBIDPOST */
  "精華區總長",                 /* PERM_Announce */
  "紀律管制小組",               /* PERM_LAW */
  "紀律組長",                   /* PERM_LAWADM */
  "系統維修小組",               /* PERM_SYSTEM */
  "系統組長",                   /* PERM_SYSTEMADM */
  "秘密駭客",                   /* PERM_HACKER */
  "護衛長",                     /* PERM_GUARD */
  "秘書長",                     /* PERM_SECRETARY */
  "超級站長",                   /* PERM_SUPER */
  "貓神"                        /* PERM_GOD */  
};

#endif
#endif                          /* _PERM_H_ */

/*-------------------------------------------------------*/
/* habit.h      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : 使用者喜好設定                               */
/* create : 98/05/21                                     */
/*-------------------------------------------------------*/

#ifndef _HABIT_H_
#define _HABIT_H_

/* ----------------------------------------------------- */
/* these are habits in userec.habit                      */
/* ----------------------------------------------------- */

#define HABIT_MOVIE     000000000001    /* 開/關動態看板 */
#define HABIT_COLOR     000000000002    /* 彩色/黑白切換 */
#define HABIT_NOTE      000000000004    /* 顯示留言板 */
#define HABIT_ALARM	000000000010    /* 半點報時 */
#define HABIT_BELL	000000000020	/* 聲音 */
#define HABIT_BOARDLIST	000000000040    /* 看板列表顯示文章數或是編號 */
#define HABIT_SEELOG	000000000100    /* 上站都去看紀錄排名? */
#define HABIT_CYCLE	000000000200    /* 循環式閱讀 */
#define HABIT_RPG	000000000400
#define HABIT_FEELING	000000001000
#define HABIT_FROM	000000002000
// #define HABIT_NOTEMONE  000000004000
#define HABIT_SAYALOHA		000000004000    /* say aloha to friends */
#define HABIT_ALREADYSET	000000010000    /* 每次上站都設定? */
#define HABIT_LIGHTBAR		000000020000	/* 光棒選單 */

#define HAS_HABIT(x)     ((x)?cuser.habit&(x):1)
#define HAVE_HABIT(x)    (cuser.habit&(x))

#define HABIT_NEWUSER    (HABIT_MOVIE | HABIT_COLOR | HABIT_SAYALOHA | HABIT_BOARDLIST | HABIT_LIGHTBAR)
#define HABIT_GUEST	 (HABIT_MOVIE | HABIT_COLOR | HABIT_SEELOG | HABIT_ALREADYSET | HABIT_LIGHTBAR)
/* ----------------------------------------------------- */
/* 各種喜好設定的中文意義                                */
/* ----------------------------------------------------- */


#ifndef _USER_C_
extern char *habitstrings[];

#else

#define NUMHABITS        14

char *habitstrings[] = {
/* HABIT_MOVIE */    "動態看板              (開/關)",
/* HABIT_COLOR */    "顯示模式切換          (彩色/黑白) ",
/* HABIT_NOTE  */    "上站顯示留言板        (開/關)",
/* HABIT_ALARM */    "半點報時              (開/關)",
/* HABIT_BELL  */    "聲音通知              (開/關)",
/* HABIT_BOARDLIST */"看板列表顯示          (文章數/編號)",
/* HABIT_SEELOG	*/   "上站觀看各項紀錄排名  (開/關)",
/* HABIT_CYCLE */    "循環式閱\讀            (開/關)",
/* HABIT_RPG */      "是否加入RPG遊戲       (是/否)",
/* HABIT_FEELING */  "上站設定心情          (開/關)",
/* HABIT_FROM */     "上站設定故鄉          (開/關)",
/* HABIT_NOTEMONEY   "上站使用記帳本        (開/關)", */
/* HABIT_SAYALOHA */ "上下站通知朋友        (開/關)",
/*HABIT_ALREADYSET*/ "上站不再詢問喜好設定  (開/關)",
/* HABIT_LIGHTBAR */ "使用選單光棒          (開/關)",
};

#endif

#endif                          /* _HABIT_H_ */
