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
#define PERM_EXTRA1        0100000  /* Q 32768 */ /* SOB ¤§ PERM_POSTMASK */
#define PERM_SPCMEMBER     0200000  /* ¥H¤U²¤  */
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

#undef LEAST_PERM_DEFAULT    /* (ºc·QªÌ : ErOs (hcu.twbbs.org) */

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

#define PERM_INTERNET   PERM_LOGINOK    /* ¨­¥÷»{ÃÒ¹LÃöªº¤~¯à±H«H¨ì Internet */

// #define HAS_PERM(x)     ((x)?cuser.userlevel&(x):1)
/* Dopin: §ï¦¨¦h­« PERM */

#define HAVE_PERM(x)    (cuser.userlevel&(x))
#define PERM_HIDE(u) ((u)->userlevel & PERM_SYSOP && (u)->userlevel & PERM_DENYPOST)


/* ----------------------------------------------------- */
/* ¦UºØÅv­­ªº¤¤¤å·N¸q                                    */
/* ----------------------------------------------------- */


#ifndef _ADMIN_C_
extern char *permstrings[];
#else
#define NUMPERMS        32

char *permstrings[] = {
  "°ò¥»Åv¤O",                    /* PERM_BASIC */
  "¶i¤J²á¤Ñ«Ç",                  /* PERM_CHAT */
  "§ä¤H²á¤Ñ",                    /* PERM_PAGE */
  "µoªí¤å³¹",                    /* PERM_POST */
  "µù¥Uµ{§Ç»{ÃÒ",                /* PERM_LOGINOK */
  "«H¥óµL¤W­­",                  /* PERM_MAILLIMIT */
  "Áô¨­Åv­­",                    /* PERM_CLOAK */
  "¬Ý¨£Áô¨­ªÌ",                  /* PERM_SEECLOAK */
  "¥Ã¤[«O¯d±b¸¹",                /* PERM_XEMPT */  /* °ò¥»¬ÝªO¦^À³½T©w¨ì¦¹ */
  "¸T¤îµo¤å   [¯¸ªø«h¤W¯¸µµÁô]", /* PERM_DENYPOST */
  "ªO¥DÅv­­   [¤À¯¸ªO¥D¤£»Ý­n]", /* PERM_BM */
#ifndef USE_ACCOUNTS_PERM
  "±b¸¹Á`ºÞ   [«O¯d]",           /* PERM_ACCOUNTS */
#else
  "±b¸¹Á`ºÞ   [¨Ï¥Î]",           /* PERM_ACCOUNTS */
#endif
  "¤À¯¸ªøºX¼Ð [¨t²Î¨Ï¥Î]",       /* PERM_CHATCLOAK */
  "¤À¯¸ªøÅv­­ [³]©w¤À¯¸ªø]",     /* PERM_BOARD */
  "¯¸ªø       [½Ð¼f·V½á¤©]",     /* PERM_SYSOP */
#ifndef USE_BOARDS_MANAGER
  "³Æ¥Î¦¨­û",                    /* ¶Â½c¦W³æ ¼fµøÅv§Q½Ð¼f·V¤À°t »Ý§ó§ï   */
#else
  "¬ÝªOÁ`ºÞÅv­­",                /* ¦U³¡¥÷µ{¦¡¹ïÀ³ */
#endif
  "¯S®í¦¨­û",                    /* ¶Â½c¦W³æ board.c Ben_Perm() ¨ç¦¡¹ïÀ³ */
  "¶iªO¯µ±K   [¦Cªí¥i¨£¶iªO¼f¦W³æ]",   /* [1;36;40m¤À¯¸ªø¥i°Ê¥ÎÅv­­¥Ñ¦¹¶}©l[m */
  "¯µ±K¬ÝªO",        /* Dopin: ­n³]¦¨¯µ±KªO ¥²»Ý·f°t¬ÝªO­­¨î¬°¾\Åª»P¦¹Åv­­ */

  "¶iªO¯µ±K¥Ó½Ð", /* ¥i¸ß°Ý¥[¤J¦W³æ ¦¹ 12 ¶µ¬°³Ì·sÂX¥R©w¸q ©Ò¥H level ¶µ¥Ø */
  "ÂX¥R³Æ¥Î 1",                  /* ¤w¥i¨Ï¥Î¨ì³Ì¤j­È 32 ¶µ */
  "ÂX¥R³Æ¥Î 2",
  "ÂX¥R³Æ¥Î 3",
  "ÂX¥R³Æ¥Î 4",
  "ÂX¥R³Æ¥Î 5",
  "ÂX¥R³Æ¥Î 6",
  "ÂX¥R³Æ¥Î 7",
  "ÂX¥R³Æ¥Î 8",
  "ÂX¥R³Æ¥Î 9",
  "ÂX¥R³Æ¥Î10",
  "ÂX¥R³Æ¥Î11",                 /* ¨ì¦¹ */

  "Post/Read ­­¨î"              /* PERM_POSTMASK ¾\Åª/µo¤å­­¨î */ /* [1;36;40m¨ì³o¸Ì[m */
};

#endif

#endif                          /* _PERM_H_ */
