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
/* �w�q BBS ���W��}                                     */
/* ------------------------------------------------------*/

#define BOARDNAME       "���ӳ��y"			/* ���寸�W */
#define BBSNAME         "NIIT EXBBS"			/* �^�寸�W */
#define MYHOSTNAME      "exbbs.ilantech.edu.tw"		/* ���} */
#define MYIP		"140.111.79.2"			/* IP */
#define MYVERSION       "WD-BBS 2.3"			/* �{������ */
#define MYBBSID         "EXBBS"				/* ��H�N�X */
#define BBSHOME         "/bbs"				/* BBS ���a */
#define BBSUSER         "bbs"
#define BBSUID          999
#define BBSGID          900
#define BBSSHELL        "/bbs/bin/bbsrf"		/* �e�ݵ{�� */
#define BBSPROG         "/bbs/bin/bbs"			/* �D�{�� */
#define TAG_VALID	"[EX BBS]To "

#define CURSOR_BAR
#ifdef CURSOR_BAR
 #define BAR_COLOR "[1;33;46m"
#endif


/* ----------------------------------------------------- */
/* �պA�W��                                              */
/* ------------------------------------------------------*/

#define HAVE_CHKLOAD		/* check cpu load */
#ifdef HAVE_CHKLOAD
  #define MAX_CPULOAD     (32)            /* CPU �̰�load */
  #define MAX_SWAPUSED    (32)            /* SWAP�̰��ϥβv */
#endif

#define WITHOUT_CHROOT		/* ���ݭn root set-uid */
#define HAVE_MOVIE		/* ��ܰʺA�i�ܪO */
#define INTERNET_PRIVATE_EMAIL	/* �i�H�H�p�H�H��� Internet */
#define BIT8			/* �䴩����t�� */
#define DEFAULTBOARD	"SYSOP"	/* �w�]�ݪO */
#define LOGINATTEMPTS	(3)	/* �̤j�i�����~���� */
#define LOGINASNEW		/* �ĥΤW���ӽбb����� */
#define VOTE		(1)	/* ���ѧ벼�\�� */
#define INTERNET_EMAIL		/* �䴩 InterNet Email �\��(�t Forward) */
#define EMAIL_JUSTIFY		/* �o�X InterNet Email �����{�ҫH�� */
#undef  NEWUSER_LIMIT		/* �s��W�����T�ѭ��� */
#define POSTNOTIFY		/* �s�峹�q���\�� */
#undef  INVISIBLE		/* ���� BBS user �Ӧۦ�B */
#define MULTI_NUMS	(2)	/* �̦h���ƤW���H�� (guest���~) */
#define MULTI_GUEST	(16)	/* �̦h���ƤW����guest */
#define INITIAL_SETUP		/* ��}���A�٨S�إ߹w�]�ݪO[SYSOP] */
#define HAVE_MMAP		/* �ĥ� mmap(): memory mapped I/O */
#define HAVE_ANONYMOUS		/* ���� Anonymous �O */
#define HAVE_ORIGIN		/* ��� author �Ӧۦ�B */
#define HAVE_MAILCLEAN		/* �M�z�Ҧ��ϥΪ̭ӤH�H�c */
#define HAVE_SUICIDE		/* ���ѨϥΪ̦۱��\�� */
#define WHERE			/* �O�_���G�m�\�� */
#define HAVE_NOTE_2		/* wildcat:�p�ݪO�\�� */

/* ----------------------------------------------------- */
/* �H BBS ���W�Ҧ������X�W                               */
/* ----------------------------------------------------- */

#define MAXUSERS        (65536)         /* �̰����U�H�� */
#define MAXBOARD        (512)           /* �̤j�}���Ӽ� */
#define MAXACTIVE       (512)           /* �̦h�P�ɤW���H�� */
#define MAX_FRIEND      (128)           /* ���J cache ���̦h�B�ͼƥ� */
#define MAX_REJECT      (128)            /* ���J cache ���̦h�a�H�ƥ� */
#define MAX_LOGIN_INFO  (128)           /* �̦h�W�u�q���H�� */
#define MAX_POST_INFO   (128)            /* �̦h�s�峹�q���H�� */
#define MAX_NAMELIST    (128)           /* �̦h��L�S�O�W��H�� */
#define MAX_MOVIE       (128)		/* �̦h�ʺA�ݪ��� */
#define MAX_FROM        (128)		/* �̦h�G�m�� */
#define MAX_REVIEW      (32)            /* �̦h���y�^�U */

/* ----------------------------------------------------- */
/* ��L�t�ΤW���Ѽ�                                      */
/* ----------------------------------------------------- */

#define MAX_HISTORY     64		/* �ʺA�ݪO�O�� 12 �����v�O�� */
#define MAXKEEPMAIL     (100)		/* �̦h�O�d�X�� MAIL�H */
#define MAXEXKEEPMAIL   (500)		/* �̦h�H�c�[�j�h�֫� */
#define MAX_NOTE        (64)            /* �̦h�O�d�X�g�d���H */
#define MAXSIGLINES     (8)             /* ñ�W�ɤޤJ�̤j��� */
#define MAXQUERYLINES   (16)            /* ��� Query/Plan �T���̤j��� */
#define MAXPAGES        (900)           /* more.c ���峹���ƤW�� (lines/22) */
#define MAXMSGS         (16)		/* ���y(���T)�ԭ@�W�� */
#define MOVIE_INT       (12)		/* �����ʵe���g���ɶ� 12 �� */

/* ----------------------------------------------------- */
/* �o�b�L�[�۰�ñ�h    */
/* ------------------------------------------------------*/

#define DOTIMEOUT

#ifdef  DOTIMEOUT
#define IDLE_TIMEOUT    (29*60)		/* �@�뱡�p�� timeout */
// #define MONITOR_TIMEOUT (29*60)         /* monitor �ɤ� timeout */
#define SHOW_IDLE_TIME                  /* ��ܶ��m�ɶ� */
#endif

/* ----------------------------------------------------- */
/* chat.c & xchatd.c ���ĥΪ� port �� protocol           */
/* ------------------------------------------------------*/

#define CHATPORT	3838

#define MAXROOM         32             /* �̦h���X���]�[�H */

#define EXIT_LOGOUT     0
#define EXIT_LOSTCONN   -1
#define EXIT_CLIERROR   -2
#define EXIT_TIMEDOUT   -3
#define EXIT_KICK       -4

#define CHAT_LOGIN_OK       "OK"
#define CHAT_LOGIN_EXISTS   "EX"
#define CHAT_LOGIN_INVALID  "IN"
#define CHAT_LOGIN_BOGUS    "BG"

#define BADCIDCHARS " *:/\,;.?"        /* Chat Room ���T�Ω� nick ���r�� */


/* ----------------------------------------------------- */
/* �t�ΰѼ�      cache                                   */
/* ----------------------------------------------------- */
#define MAGIC_KEY       1999   /* �����{�ҫH��s�X */

#define SEM_ENTER      -1      /* enter semaphore */
#define SEM_LEAVE      1       /* leave semaphore */
#define SEM_RESET      0       /* reset semaphore */

#define BRDSHM_KEY      1215
#define UIDSHM_KEY      1217
#define UTMPSHM_KEY     1219
#define FILMSHM_KEY     1221    /* �ʺA�ݪ� , �`�� */
#define FROMSHM_KEY     1223    /* whereis, �̦h�ϥΪ� */

#define BRDSEM_KEY      2005
#define FILMSEM_KEY     2000    /* semaphore key */
#define FROMSEM_KEY     2003    /* semaphore key */

/* ----------------------------------------------------- */
/* �ӽбb���ɭn�D�ӽЪ̯u����                          */
/* ----------------------------------------------------- */

#define SHOWUID                 /* �ݨ��ϥΪ� UID */
#define SHOWTTY                 /* �ݨ��ϥΪ� TTY */
#define SHOWPID                 /* �ݨ��ϥΪ� PID */

#define REALINFO                /* �u��m�W */

#ifdef  REALINFO
#undef  ACTS_REALNAMES          /* �D�ؿ��� (U)ser ��ܯu��m�W */
#undef  POST_REALNAMES          /* �K���ɪ��W�u��m�W */
#undef  MAIL_REALNAMES          /* �H�����H��ɪ��W�u��m�W */
#undef  QUERY_REALNAMES         /* �Q Query �� User �i���u��m�W */
#endif

/* ----------------------------------------------------- */
/* http                                                  */
/* ----------------------------------------------------- */

#define USE_LYNX   	      /* �ϥΥ~��lynx dump ? */
#undef USE_PROXY
#ifdef  USE_PROXY
#define PROXYSERVER "140.111.72.8"
#define PROXYPORT   3128
#endif

#define LOCAL_PROXY           /* �O�_�}��local ��proxy */
#ifdef  LOCAL_PROXY
#define HPROXYDAY   3         /* local��proxy refresh�Ѽ� */
#endif

/* ----------------------------------------------------- */
/* ���ѥ~���{��                                          */
/* ----------------------------------------------------- */

#define HAVE_EXTERNAL

  #ifdef HAVE_EXTERNAL
    #undef HAVE_TIN        /* ���� news reader */
    #undef HAVE_GOPHER     /* ���� gopher */
    #undef HAVE_WWW        /* ���� www browser */
    #define HAVE_BIG2       /* ���� big2 game */
    #define HAVE_MJ         /* ���� mj game */
    #define HAVE_BRIDGE     /* ���� Ok Bridge */
    #undef HAVE_MUD        /* ���� MUD */
    #define HAVE_CAL        /* ���\�p��� */
    #undef HAVE_ARCHIE     /* have arche */
    #define BBSDOORS        /* ���� bbs doors */
    #define HAVE_GAME       /* ���Ѻ����s�u�C�� */
    #define HAVE_ADM_SHELL  /* ���� csh �H�K�޲z */
  #endif

#endif

/* ----------------------------------------------------- */
/* �w�q���C��                                          */
/* ----------------------------------------------------- */

#define ANNOUNCE_BRD	"Announce"
#define VOTEBOARD	"VoteBoard"

#define COLOR1	"[46;37m"
#define COLOR2	"[1m[44;33m"
