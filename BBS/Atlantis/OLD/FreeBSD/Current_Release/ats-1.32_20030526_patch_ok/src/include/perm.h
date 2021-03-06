/*-------------------------------------------------------*/
/* perm.h       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : permission levels of user & board            */
/* create : 95/03/29                                     */
/* update : 02/07/12                                     */
/*-------------------------------------------------------*/

#include "struct.h"
#ifndef _PERM_H_
#define _PERM_H_
extern userec cuser;

/* ----------------------------------------------------- */
/* Dopin: These are the 32 Total permission bits.        */
/* ----------------------------------------------------- */

#define PERM_BASIC          000001  /* A    1 */
#define PERM_CHAT           000002  /* B    2 */
#define PERM_PAGE           000004  /* C    4 */
#define PERM_POST           000010  /* D    8 */
#define PERM_LOGINOK        000020  /* E   16 */
#define PERM_MAILLIMIT      000040  /* F   32 */
#define PERM_CLOAK          000100  /* G   64 */
#define PERM_SEECLOAK       000200  /* H  128 */
#define PERM_XEMPT          000400  /* I  256 */
#define PERM_DENYPOST       001000  /* J  512 */
#define PERM_BM             002000  /* K 1024 */
#define PERM_ACCOUNTS       004000  /* M 2048 */
#define PERM_CHATROOM       010000  /* N 4096 */
#define PERM_BOARD          020000  /* O 8192 */
#define PERM_SYSOP          040000  /* P 16384 */
#define PERM_EXTRA1        0100000  /* Q 32768 */ /* SOB 之 PERM_POSTMASK */
#define PERM_SPCMEMBER     0200000  /* 以下略  */
#define PERM_ENTERSC       0400000
#define PERM_SECRET       01000000
#define PERM_ENTERSCRQ    02000000
#define PERM_RMK1         04000000
#define PERM_RMK2        010000000
#define PERM_RMK3        020000000
#define PERM_RMK4        040000000
#define PERM_RMK5       0100000000
#define PERM_RMK6       0200000000
#define PERM_RMK7       0400000000
#define PERM_RMK8      01000000000
#define PERM_RMK9      02000000000
#define PERM_RMK10     04000000000
#define PERM_RMK11    010000000000
#define PERM_POSTMASK 020000000000 /* means the rest is a post mask */

/* ----------------------------------------------------- */
/* These permissions are bitwise ORs of the basic bits.  */
/* ----------------------------------------------------- */

#undef LEAST_PERM_DEFAULT    /* (構想者 : ErOs (hcu.twbbs.org) */

/* This is the default permission granted to all new accounts. */
#ifndef LEAST_PERM_DEFAULT
  #define PERM_DEFAULT    (PERM_BASIC | PERM_CHAT | PERM_PAGE | PERM_POST)
#else
  #define PERM_DEFAULT    (PERM_BASIC)
  #define PERM_OKADD      (PERM_CHAT | PERM_PAGE | PERM_POST)
#endif

#define PERM_ADMIN      (PERM_ACCOUNTS | PERM_SYSOP)
#define PERM_ALLBOARD   (PERM_SYSOP | PERM_BOARD)
/*
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS)
*/
#define PERM_LOGINCLOAK (PERM_CLOAK)
#define PERM_SEEULEVELS PERM_SYSOP
#define PERM_SEEBLEVELS (PERM_SYSOP | PERM_BM)
#define PERM_NOTIMEOUT  PERM_SYSOP

#define PERM_READMAIL   PERM_BASIC
#define PERM_FORWARD    PERM_BASIC      /* to do the forwarding */

#define PERM_INTERNET   PERM_LOGINOK    /* 身份認證過關的才能寄信到 Internet */

// #define HAS_PERM(x)     ((x)?cuser.userlevel&(x):1)
/* Dopin: 改成多重 PERM */

#define HAVE_PERM(x)    (cuser.userlevel&(x))
#define PERM_HIDE(u) ((u)->userlevel & PERM_SYSOP && (u)->userlevel & PERM_DENYPOST)


/* ----------------------------------------------------- */
/* 各種權限的中文意義                                    */
/* ----------------------------------------------------- */


#ifndef _ADMIN_C_
extern char *permstrings[];
#else
#define NUMPERMS        32

char *permstrings[] = {
  "基本權力",                    /* PERM_BASIC */
  "進入聊天室",                  /* PERM_CHAT */
  "找人聊天",                    /* PERM_PAGE */
  "發表文章",                    /* PERM_POST */
  "註冊程序認證",                /* PERM_LOGINOK */
  "信件無上限",                  /* PERM_MAILLIMIT */
  "隱身權限",                    /* PERM_CLOAK */
  "看見隱身者",                  /* PERM_SEECLOAK */
  "永久保留帳號",                /* PERM_XEMPT */  /* 基本看板回應確定到此 */
  "禁止發文   [站長則上站紫隱]", /* PERM_DENYPOST */
  "板主權限   [分站板主不需要]", /* PERM_BM */
#ifndef USE_ACCOUNTS_PERM
  "帳號總管   [保留]",           /* PERM_ACCOUNTS */
#else
  "帳號總管   [使用]",           /* PERM_ACCOUNTS */
#endif
  "分站長旗標 [系統使用]",       /* PERM_CHATCLOAK */
  "分站長權限 [設定分站長]",     /* PERM_BOARD */
  "站長       [請審慎賦予]",     /* PERM_SYSOP */
#ifndef USE_BOARDS_MANAGER
  "備用成員",                    /* 黑箱名單 審視權利請審慎分配 需更改   */
#else
  "看板總管權限",                /* 各部份程式對應 */
#endif
  "特殊成員",                    /* 黑箱名單 board.c Ben_Perm() 函式對應 */
  "進板秘密   [列表可見進板審名單]",   /* [1;36;40m分站長可動用權限由此開始[m */
  "秘密看板",        /* Dopin: 要設成秘密板 必需搭配看板限制為閱讀與此權限 */

  "進板秘密申請", /* 可詢問加入名單 此 12 項為最新擴充定義 所以 level 項目 */
  "擴充備用 1",                  /* 已可使用到最大值 32 項 */
  "擴充備用 2",
  "擴充備用 3",
  "擴充備用 4",
  "擴充備用 5",
  "擴充備用 6",
  "擴充備用 7",
  "擴充備用 8",
  "擴充備用 9",
  "擴充備用10",
  "擴充備用11",                 /* 到此 */

  "Post/Read 限制"              /* PERM_POSTMASK 閱讀/發文限制 */ /* [1;36;40m到這裡[m */
};

#endif

#endif                          /* _PERM_H_ */
