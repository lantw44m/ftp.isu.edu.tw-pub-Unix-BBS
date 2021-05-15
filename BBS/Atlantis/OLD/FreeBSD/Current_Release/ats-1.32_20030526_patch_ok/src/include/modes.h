/*-------------------------------------------------------*/
/* modes.h      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : user operating mode & status                 */
/* create : 95/03/29                                     */
/* update : 02/08/11 (Dopin)                             */
/*-------------------------------------------------------*/

#ifndef _MODES_H_
#define _MODES_H_

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
#define CLASS           7
#define LOGIN           8       /* main menu */
#define ANNOUNCE        9       /* announce */
#define POSTING         10      /* boards & class */

#define READBRD         11
#define READING         12
#define READNEW         13
#define SELECT          14
#define RMAIL           15      /* mail menu */
#define SMAIL           16
#define CHATING         17      /* talk menu */
#define XMODE           18
#define FRIEND          19
#define LAUSERS         20

#define LUSERS          21
#define MONITOR         22
#define PAGE            23
#define QUERY           24
#define TALK            25
#define EDITPLAN        26      /* user menu */
#define EDITSIG         27
#define VOTING          28
#define XINFO           29
#define MSYSOP          30

#define WWW             31
#define BRIDGE          32
#define REPLY           33
#define HIT             34
#define DBACK           35
#define NOTE            36
#define EDITING         37
#define MAILALL         38
#define ATSFUNC         39
#ifdef STAR_CHICKEN
#define CHICKEN_3       40
#endif

#define GLINK           41
#define TREE            42

#define BBSNET          0

#ifdef TAKE_IDLE
static char *IdleTypeTable[] =
{
  "���b��b��",
  "���H�ӹq",
  "�V����",
  "�����P��",
  "�������A",
  "�ڦb���"
};
#endif

#ifdef  _MODES_C_
static char *ModeTypeTable[] =
{
  "�o�b",                       /* IDLE */
  "�D���",                     /* MMENU */
  "�t�κ��@",                   /* ADMIN */
  "�l����",                   /* MAIL */
  "��Ϳ��",                   /* TMENU */
  "�ϥΪ̿��",                 /* UMENU */
  "XYZ ���",                   /* XMENU */
  "�����ݪO",                   /* CLASS */
  "�W���~��",                   /* LOGIN */
  "���G��",                     /* ANNOUNCE */
  "�o��峹",                   /* POSTING */
  "�ݪO�C��",                   /* READBRD */
  "�\\Ū�峹",                  /* READING */
  "�s�峹�C��",                 /* READNEW */
  "��ܬݪO",                   /* SELECT */
  "Ū�H",                       /* RMAIL */
  "�g�H",                       /* SMAIL */
  "��ѫ�",                     /* CHATING */
  "��L",                       /* XMODE */
  "�M��n��",                   /* FRIEND */
  "�W�u�ϥΪ�",                 /* LAUSERS */
  "�ϥΪ̦W��",                 /* LUSERS */
  "�l�ܯ���",                   /* MONITOR */
  "�I�s",                       /* PAGE */
  "�d��",                       /* QUERY */
  "���",                       /* TALK  */
  "�s�W����",                   /* EDITPLAN */
  "�sñ�W��",                   /* EDITSIG */
  "�벼��",                     /* VOTING */
  "�]�w���",                   /* XINFO */
  "�H������",                   /* MSYSOP */
  "�L�L�L",                     /* WWW */
  "�����P",                     /* BRIDGE */
  "�^��",                       /* REPLY */
  "�Q���y����",                 /* HIT */
  "���y�ǳƤ�",                 /* DBACK */
  "���O��",                     /* NOTE */
  "�s��峹",                   /* EDITING */
  "�o�t�γq�i",                 /* MAILALL */
  "�����\\���",                /* ATSFUNC */
  "�������ڤu��"                /* STAR CHICKEN */
};

#endif                          /* _MODES_C_ */

/* ----------------------------------------------------- */
/* menu.c �����Ҧ�                                       */
/* ----------------------------------------------------- */

#define QUIT    0x666           /* Return value to abort recursive functions */
#define XEASY   0x333           /* Return value to un-redraw screen */

/* ----------------------------------------------------- */
/* read.c �����Ҧ�                                       */
/* ----------------------------------------------------- */

#define RS_FORWARD      0x01    /* backward */
#define RS_TITLE        0x02    /* author/title */
#define RS_RELATED      0x04
#define RS_FIRST        0x08    /* find first article */
#define RS_CURRENT      0x10    /* match current read article */
#define RS_THREAD       0x20    /* search the first article */
#define RS_AUTHOR       0x40    /* search author's article */

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

/* woju */
#define POS_NEXT        101     /* cursor_pos(locmem, locmem->crs_ln + 1, 1);*/

/* for currmode */
#define MODE_STARTED    1       /* �O�_�w�g�i�J�t�� */
#define MODE_POST       2       /* �O�_�i�H�b currboard �o��峹 */
#define MODE_BOARD      4       /* �O�_�i�H�b currboard �R���Bmark�峹 */
#define MODE_DIGEST     0x10    /* �O�_�� digest mode */
#define MODE_ETC        0x20    /* �O�_�� etc mode */
#define MODE_SELECT     0x40    /* �O�_�� select mode */
#define MODE_DIRTY      0x80    /* �O�_��ʹL userflag */

/* for curredit */
#define EDIT_MAIL       1       /* �ثe�O mail/board ? */
#define EDIT_LIST       2       /* �O�_�� mail list ? */
#define EDIT_BOTH       4       /* both reply to author/board ? */
#endif                          /* _MODES_H_ */
