/*-------------------------------------------------------*/
/* modes.h      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : user operating mode & status                 */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#ifndef _MODES_H_
#define _MODES_H_

/* ----------------------------------------------------- */
/* strip ansi mode   Ptt                                 */
/* ----------------------------------------------------- */

enum
{STRIP_ALL, ONLY_COLOR, NO_RELOAD};

/* ----------------------------------------------------- */
/* money mode   wildcat                                  */
/* ----------------------------------------------------- */

#define SILVER 0
#define GOLD   1
#define BUY_C  20     /* �ʶR���� */
#define BUY_F  20     /* �ʶR�G�m */
#define BUY_M  100    /* �ʶR�l�c */
#define BUY_K  2000   /* �ʶR�۱� */
#define BUY_S  50     /* �ʶR�z�� */
#define BUY_SC 2000   /* �׷����� */
#define BUY_SF 2000   /* �׷��G�m */

/* ----------------------------------------------------- */
/* �s�զW��Ҧ�   Ptt                                    */
/* ----------------------------------------------------- */

#define FRIEND_OVERRIDE 0
#define FRIEND_REJECT   1
#define FRIEND_ALOHA    2
#define FRIEND_POST     3         
#define FRIEND_SPECIAL  4
#define FRIEND_CANVOTE  5
#define BOARD_WATER     6
#define BOARD_VISABLE   7 
#define FRIEND_MAILLIST	8
#define FRIEND_MAILREPLYLIST	9



/* ----------------------------------------------------- */
/* user �ާ@���A�P�Ҧ�                                   */
/* ----------------------------------------------------- */

#define IDLE            0
#define MMENU           1       /* menu mode */
#define ADMIN           2
#define MAIL            3
#define TMENU           4
#define UMENU           5
#define XMENU           6
#define PMENU           7
#define CLASS           8
#define FINANCE         9
#define BACKSTREET      10
#define READBRD         11
#define NMENU           12
#define POSTING         13      /* boards & class */
#define READING         14
#define READNEW         15
#define SELECT          16
#define RMAIL           17      /* mail menu */
#define SMAIL           18
#define CHATING         19      /* talk menu */
#define XMODE           20
#define FRIEND          21
#define LAUSERS         22
#define LUSERS          23
#define MONITOR         24
#define PAGE            25
#define QUERY           26
#define TALK            27
#define EDITPLAN        28      /* user menu */
#define EDITSIG         29
#define VOTING          30
#define XINFO           31
#define MSYSOP          32
#define LOG		33
#define BIG2            34
#define REPLY           35
#define HIT             36
#define DBACK           37
#define NOTE            38
#define EDITING         39
#define MAILALL         40
/*Ptt*/
#define MJ              41
#define LOGIN           42       /* main menu */
#define DICT            43
#define BRIDGE          44
#define ARCHIE          45
#define GOPHER          46
#define SCHED		47
#define CATV            48
#define EDITEXP         49
#define CAL             50
#define PROVERB         51
#define ANNOUNCE        52       /* announce */
#define EDNOTE          53 
#define GAME            54
#define MARIE           55
#define CHICKEN         56
#define XBOARDS         57
#define TICKET		58
#define B_MENU          59
#define RACE            60
#define BINGO           61
#define XAXB		62
#define TETRIS		63
#define CDICT		64
#define XSYSTEM		65
#define XUSER		66
#define BANK		67
#define BLACKJACK	68
#define OSONG           69

/* RPG ��� */
#define RMENU		70
#define RGUILD 		71
#define RTRAIN		72
#define RINFO		73
#define RPK		74
#define RHELP		75
#define RCHOOSE		76
#define RINTRO		77
#define EDITPIC		78
#define RSHOP		79
#define RTOPLIST	80

#define DICE		81
#define RICH		82
#define STOCK		83
#define MINE            84
#define CHICKENPAGE	85
#define CHICKENTALK	86
#define GP		87
#define NINE		88
#define FIVE		89
#define CHESS		90
#define	POWERBOOK	91
#define	PRECORD		92
#define	MSGMENU		93
#define NumFight	94
#define	DRAGON		95
#define BBSNET		96
#define SEVENCARD	97
#define CHESSMJ		98
#define NETGAME		99
#define BET		100
#define NoteMoney	101
#define LISTMAIN	102
#define LISTEDIT	103
#define FORTUNE		104
#define REJECTMAIL	105
#define BDREJECTMAIL    106
#define WISH            107
#define TIME_MACHINE	108

/* ----------------------------------------------------- */
/* menu.c �����Ҧ�                                       */
/* ----------------------------------------------------- */

#define XEASY   0x333           /* Return value to un-redraw screen */
#define QUIT    0x666           /* Return value to abort recursive functions */


/* ----------------------------------------------------- */
/* read.c �����Ҧ�                                       */
/* ----------------------------------------------------- */

/* Read menu command return states */

#define RC_NONE		0	/* nothing to do */
#define RC_FULL		0x0100	/* Entire screen was destroyed in this oper */
#define RC_BODY		0x0200	/* Only the top three lines not destroyed */
#define RC_DRAW		0x0400
#define RC_FOOT		0x0800
#define RC_ITEM		0x1000
#define RC_CHDIR	0x2000	/* Index file was changed */
#define RC_NEWDIR	0x4000	/* Directory has changed, re-read files */

#define RS_FORWARD      0x01    /* backward */
#define RS_TITLE        0x02    /* author/title */
#define RS_RELATED      0x04
#define RS_FIRST        0x08    /* find first article */
#define RS_CURRENT      0x10    /* match current read article */
#define RS_THREAD       0x20    /* search the first article */
#define RS_AUTHOR       0x40    /* search author's article */

#define RS_NEXT         0x8000  /* Direct read next file */
#define RS_PREV         0x10000 /* Direct read prev file */

#define CURSOR_FIRST    (RS_RELATED | RS_TITLE | RS_FIRST)
#define CURSOR_NEXT     (RS_RELATED | RS_TITLE | RS_FORWARD)
#define CURSOR_PREV     (RS_RELATED | RS_TITLE)
#define RELATE_FIRST    (RS_RELATED | RS_TITLE | RS_FIRST | RS_CURRENT)
#define RELATE_NEXT     (RS_RELATED | RS_TITLE | RS_FORWARD | RS_CURRENT)
#define RELATE_PREV     (RS_RELATED | RS_TITLE | RS_CURRENT)
#define THREAD_NEXT     (RS_THREAD | RS_FORWARD)
#define THREAD_PREV     (RS_THREAD)
#define AUTHOR_NEXT     (RS_AUTHOR | RS_FORWARD)
#define AUTHOR_PREV     (RS_AUTHOR)

#if 0
#define DONOTHING       0       /* Read menu command return states */
#define FULLUPDATE      1       /* Entire screen was destroyed in this oper */
#define PARTUPDATE      2       /* Only the top three lines were destroyed */
#define DOQUIT          3       /* Exit read menu was executed */
#define NEWDIRECT       4       /* Directory has changed, re-read files */
#define READ_NEXT       5       /* Direct read next file */
#define READ_PREV       6       /* Direct read prev file */
#define DIRCHANGED      8       /* Index file was changed */
#define READ_REDRAW     9
#define PART_REDRAW     10
#endif
/*
woju
*/
#define POS_NEXT        101     /* cursor_pos(locmem, locmem->crs_ln + 1, 1);*/


/* for currmode */

#define MODE_STARTED    0x01       /* �O�_�w�g�i�J�t�� */
#define MODE_MENU	0x02       /* �O�_�b MENU */
#define MODE_DELETED    0x04       /* deleted mode */
#define	MODE_POST	0x08	/* �O�_�i�H�b currboard �o��峹 */
#define	MODE_BOARD	0x10	/* �O�_�i�H�b currboard �R���Bmark�峹 */
#define	MODE_SECRET	0x20	/* �O�_�� secret board */
#define	MODE_DIGEST	0x40	/* �O�_�� digest mode */
#define	MODE_DIRTY	0x80	/* �O�_��ʹL userflag */
#define MODE_SELECT     0x100 
#define MODE_FAVORITE	0x200	/* �O�_���ߦn�ݪO */
#define MODE_TINLIKE	0x400	/* �O�_�� tin-like �\Ū */

/* for curredit */

#define EDIT_MAIL       1       /* �ثe�O mail/board ? */
#define EDIT_LIST       2       /* �O�_�� mail list ? */
#define EDIT_BOTH       4       /* both reply to author/board ? */
#define EDIT_ITEM	8
#ifdef HAVE_ANONYMOUS
#define EDIT_ANONYMOUS 10       /* �ΦW�Ҧ� */
#endif

/* for Tag List */
#define MAX_TAGS        256     /* max taglist size */
#define	TAG_NIN		0	/* ���ݩ� TagList :old */
#define TAG_COMP        0       /* ��� TagList */
#define TAG_TOGGLE      1       /* ���� TagList */
#define TAG_INSERT      2       /* �[�J TagList */
#define TAG_GET_RECORD  3       /* ���o TagList ���Ҧ� fileheader */
#define TAG_ALL         4       /* �аO�Ҧ� chrono --> �� currstat == RMAIL */
#define TAG_THREAD      5       /* �аO Thread */

/* for announce.c */
#define GEM_FMODE         1       /* �ɮ׼Ҧ� */
#define GEM_PERM          2       /* �i�s���ذ� */
#define GEM_LOCK_TAG      4       /* ��w�O���аO */
#define GEM_LOCK_PATH     8       /* ��w��ذϸ��| */
#define GEM_GOPHER        16      /* ��ذϳs�u�� */
#define GEM_RELOAD        32      /* ��ذ� �j�� Reload */

#endif                          /* _MODES_H_ */
