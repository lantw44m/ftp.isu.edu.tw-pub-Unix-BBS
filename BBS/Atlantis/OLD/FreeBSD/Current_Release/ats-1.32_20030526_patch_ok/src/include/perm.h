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
#define PERM_EXTRA1        0100000  /* Q 32768 */ /* SOB �� PERM_POSTMASK */
#define PERM_SPCMEMBER     0200000  /* �H�U��  */
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

#undef LEAST_PERM_DEFAULT    /* (�c�Q�� : ErOs (hcu.twbbs.org) */

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

#define PERM_INTERNET   PERM_LOGINOK    /* �����{�ҹL�����~��H�H�� Internet */

// #define HAS_PERM(x)     ((x)?cuser.userlevel&(x):1)
/* Dopin: �令�h�� PERM */

#define HAVE_PERM(x)    (cuser.userlevel&(x))
#define PERM_HIDE(u) ((u)->userlevel & PERM_SYSOP && (u)->userlevel & PERM_DENYPOST)


/* ----------------------------------------------------- */
/* �U���v��������N�q                                    */
/* ----------------------------------------------------- */


#ifndef _ADMIN_C_
extern char *permstrings[];
#else
#define NUMPERMS        32

char *permstrings[] = {
  "���v�O",                    /* PERM_BASIC */
  "�i�J��ѫ�",                  /* PERM_CHAT */
  "��H���",                    /* PERM_PAGE */
  "�o��峹",                    /* PERM_POST */
  "���U�{�ǻ{��",                /* PERM_LOGINOK */
  "�H��L�W��",                  /* PERM_MAILLIMIT */
  "�����v��",                    /* PERM_CLOAK */
  "�ݨ�������",                  /* PERM_SEECLOAK */
  "�ä[�O�d�b��",                /* PERM_XEMPT */  /* �򥻬ݪO�^���T�w�즹 */
  "�T��o��   [�����h�W������]", /* PERM_DENYPOST */
  "�O�D�v��   [�����O�D���ݭn]", /* PERM_BM */
#ifndef USE_ACCOUNTS_PERM
  "�b���`��   [�O�d]",           /* PERM_ACCOUNTS */
#else
  "�b���`��   [�ϥ�]",           /* PERM_ACCOUNTS */
#endif
  "�������X�� [�t�Ψϥ�]",       /* PERM_CHATCLOAK */
  "�������v�� [�]�w������]",     /* PERM_BOARD */
  "����       [�мf�V�ᤩ]",     /* PERM_SYSOP */
#ifndef USE_BOARDS_MANAGER
  "�ƥΦ���",                    /* �½c�W�� �f���v�Q�мf�V���t �ݧ��   */
#else
  "�ݪO�`���v��",                /* �U�����{������ */
#endif
  "�S����",                    /* �½c�W�� board.c Ben_Perm() �禡���� */
  "�i�O���K   [�C��i���i�O�f�W��]",   /* [1;36;40m�������i�ʥ��v���Ѧ��}�l[m */
  "���K�ݪO",        /* Dopin: �n�]�����K�O ���ݷf�t�ݪO����\Ū�P���v�� */

  "�i�O���K�ӽ�", /* �i�߰ݥ[�J�W�� �� 12 �����̷s�X�R�w�q �ҥH level ���� */
  "�X�R�ƥ� 1",                  /* �w�i�ϥΨ�̤j�� 32 �� */
  "�X�R�ƥ� 2",
  "�X�R�ƥ� 3",
  "�X�R�ƥ� 4",
  "�X�R�ƥ� 5",
  "�X�R�ƥ� 6",
  "�X�R�ƥ� 7",
  "�X�R�ƥ� 8",
  "�X�R�ƥ� 9",
  "�X�R�ƥ�10",
  "�X�R�ƥ�11",                 /* �즹 */

  "Post/Read ����"              /* PERM_POSTMASK �\Ū/�o�孭�� */ /* [1;36;40m��o��[m */
};

#endif

#endif                          /* _PERM_H_ */
