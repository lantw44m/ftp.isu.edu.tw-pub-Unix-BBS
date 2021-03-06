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

 
/* ?s?????????????v?? */
/* wildcat :
   ???n?? ?i??PERM_POST?????A????????email?{???????? */
#define PERM_DEFAULT    (PERM_BASIC | PERM_FORBIDPOST)

#define PERM_ADMIN      (PERM_ACCOUNTS | PERM_SYSOP)
#define PERM_ALLBOARD   (PERM_SYSOP | PERM_BOARD)
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS)
#define PERM_SEEULEVELS PERM_SYSOP
#define PERM_SEEBLEVELS (PERM_SYSOP | PERM_BM)
#define PERM_NOTIMEOUT  PERM_SYSOP

#define PERM_READMAIL   PERM_BASIC
#define PERM_FORWARD    PERM_BASIC      /* to do the forwarding */

#define PERM_INTERNET   PERM_LOGINOK    /* ?????{???L?????~???H?H?? Internet */

#define HAS_PERM(x)     ((x)?cuser.userlevel&(x):1)
#define HAVE_PERM(x)    (cuser.userlevel&(x))
#define PERM_HIDE(u) ((u)->userlevel & PERM_SYSOP && (u)->userlevel & PERM_DENYPOST)


/* ----------------------------------------------------- */
/* ?U???v?????????N?q                                    */
/* ----------------------------------------------------- */

#define NUMPERMS        32

#ifndef _ADMIN_C_
extern char *permstrings[];

#else
char *permstrings[] = {
  "?????v?O",                   /* PERM_BASIC */
  "?i?J??????",                 /* PERM_CHAT */
  "???H????",                   /* PERM_PAGE */
  "?o??????",                   /* PERM_POST */
  "???U?{???{??",               /* PERM_LOGINOK */
  "?H???L?W??",                 /* PERM_MAILLIMIT */
  "?????N",                     /* PERM_CLOAK */
  "????????",                   /* PERM_SEECLOAK */
  "???[?O?d?b??",               /* PERM_XEMPT */
  "?????????N",                 /* PERM_DENYPOST */
  "?O?D",                       /* PERM_BM */
  "?b???`??",                   /* PERM_ACCOUNTS */
  "???????`??",                 /* PERM_CHATCLOAK */
  "???O?`??",                   /* PERM_BOARD */
  "?????U?z",                   /* PERM_BBSADM & PERM_POSTMASK */
  "????",                       /* PERM_SYSOP */
  "???C?J?????]",               /* PERM_NOTOP */
  "???z???? X????",             /* PERM_XFILE */
  "?t???????H??",               /* PERM_RESEARCH */
  "?????G?m",                   /* PERM_FROM */
  "?q?l?t???v",                 /* PERM_TEACHER */
  "?T???o??????",               /* PERM_FORBIDPOST */
  "???????`??",                 /* PERM_Announce */
  "?????????p??",               /* PERM_LAW */
  "????????",                   /* PERM_LAWADM */
  "?t???????p??",               /* PERM_SYSTEM */
  "?t??????",                   /* PERM_SYSTEMADM */
  "???K?b??",                   /* PERM_HACKER */
  "?@????",                     /* PERM_GUARD */
  "??????",                     /* PERM_SECRETARY */
  "?W??????",                   /* PERM_SUPER */
  "????"                        /* PERM_GOD */  
};

#endif
#endif                          /* _PERM_H_ */

/*-------------------------------------------------------*/
/* habit.h      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : ?????????n?]?w                               */
/* create : 98/05/21                                     */
/*-------------------------------------------------------*/

#ifndef _HABIT_H_
#define _HABIT_H_

/* ----------------------------------------------------- */
/* these are habits in userec.habit                      */
/* ----------------------------------------------------- */

#define HABIT_MOVIE     000000000001    /* ?}/?????A???O */
#define HABIT_COLOR     000000000002    /* ?m??/???????? */
#define HABIT_NOTE      000000000004    /* ?????d???O */
#define HABIT_ALARM	000000000010    /* ?b?I???? */
#define HABIT_BELL	000000000020	/* ?n?? */
#define HABIT_BOARDLIST	000000000040    /* ???O?C???????????????O?s?? */
#define HABIT_SEELOG	000000000100    /* ?W?????h?????????W? */
#define HABIT_CYCLE	000000000200    /* ?`?????\?? */
#define HABIT_RPG	000000000400
#define HABIT_FEELING	000000001000
#define HABIT_FROM	000000002000
// #define HABIT_NOTEMONE  000000004000
#define HABIT_SAYALOHA		000000004000    /* say aloha to friends */
#define HABIT_ALREADYSET	000000010000    /* ?C???W?????]?w? */
#define HABIT_LIGHTBAR		000000020000	/* ???????? */

#define HAS_HABIT(x)     ((x)?cuser.habit&(x):1)
#define HAVE_HABIT(x)    (cuser.habit&(x))

#define HABIT_NEWUSER    (HABIT_MOVIE | HABIT_COLOR | HABIT_SAYALOHA | HABIT_BOARDLIST | HABIT_LIGHTBAR)
#define HABIT_GUEST	 (HABIT_MOVIE | HABIT_COLOR | HABIT_SEELOG | HABIT_ALREADYSET | HABIT_LIGHTBAR)
/* ----------------------------------------------------- */
/* ?U?????n?]?w???????N?q                                */
/* ----------------------------------------------------- */


#ifndef _USER_C_
extern char *habitstrings[];

#else

#define NUMHABITS        14

char *habitstrings[] = {
/* HABIT_MOVIE */    "???A???O              (?}/??)",
/* HABIT_COLOR */    "????????????          (?m??/????) ",
/* HABIT_NOTE  */    "?W???????d???O        (?}/??)",
/* HABIT_ALARM */    "?b?I????              (?}/??)",
/* HABIT_BELL  */    "?n???q??              (?}/??)",
/* HABIT_BOARDLIST */"???O?C??????          (??????/?s??)",
/* HABIT_SEELOG	*/   "?W???[???U?????????W  (?}/??)",
/* HABIT_CYCLE */    "?`?????\\??            (?}/??)",
/* HABIT_RPG */      "?O?_?[?JRPG?C??       (?O/?_)",
/* HABIT_FEELING */  "?W???]?w????          (?}/??)",
/* HABIT_FROM */     "?W???]?w?G?m          (?}/??)",
/* HABIT_NOTEMONEY   "?W???????O?b??        (?}/??)", */
/* HABIT_SAYALOHA */ "?W?U???q???B??        (?}/??)",
/*HABIT_ALREADYSET*/ "?W?????A???????n?]?w  (?}/??)",
/* HABIT_LIGHTBAR */ "????????????          (?}/??)",
};

#endif

#endif                          /* _HABIT_H_ */
