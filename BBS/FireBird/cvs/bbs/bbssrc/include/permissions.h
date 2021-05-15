/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: permissions.h,v 1.1 2000/01/15 01:45:24 edwardc Exp $
*/

/* These are all the permissions available in the BBS */

#define PERM_BASIC      000001
#define PERM_CHAT       000002
#define PERM_PAGE       000004
#define PERM_POST       000010
#define PERM_LOGINOK    000020
#define PERM_DENYPOST   000040
#define PERM_CLOAK      000100
#define PERM_SEECLOAK   000200
#define PERM_XEMPT      000400
#define PERM_WELCOME    001000
#define PERM_BOARDS     002000
#define PERM_ACCOUNTS   004000
#define PERM_CHATCLOAK  010000
#define PERM_OVOTE      020000
#define PERM_SYSOP      040000
#define PERM_POSTMASK  0100000     
#define PERM_ANNOUNCE  0200000
#define PERM_OBOARDS   0400000
#define PERM_ACBOARD   01000000
#define PERM_NOZAP     02000000
#define PERM_FORCEPAGE 04000000
#define PERM_EXT_IDLE  010000000
#define PERM_SPECIAL1  020000000
#define PERM_SPECIAL2  040000000
#define PERM_SPECIAL3  0100000000
#define PERM_SPECIAL4  0200000000
#define PERM_SPECIAL5  0400000000
#define PERM_SPECIAL6  01000000000
#define PERM_SPECIAL7  02000000000
#define PERM_SPECIAL8  04000000000


/* This is the default permission granted to all new accounts. */
#define PERM_DEFAULT    (PERM_BASIC | PERM_CHAT | PERM_PAGE | \
                         PERM_POST | PERM_LOGINOK)

/* These permissions are bitwise ORs of the basic bits. They work that way
   too. For example, anyone with PERM_SYSOP or PERM_OBOARDS or both has
   PERM_BLEVELS. */

#define PERM_ADMINMENU  (PERM_ACCOUNTS|PERM_OVOTE|PERM_SYSOP|PERM_OBOARDS|PERM_WELCOME|PERM_ACBOARD)
#define PERM_MULTILOG   PERM_SYSOP
#define PERM_ESYSFILE   (PERM_SYSOP | PERM_WELCOME | PERM_ACBOARD)
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS | PERM_WELCOME | PERM_CLOAK)
#define PERM_SEEULEVELS (PERM_SYSOP | PERM_CHATCLOAK | PERM_SEECLOAK)
#define PERM_BLEVELS    (PERM_SYSOP | PERM_OBOARDS)
#define PERM_MARKPOST   (PERM_SYSOP | PERM_BOARDS)
#define PERM_UCLEAN     (PERM_SYSOP | PERM_ACCOUNTS)
#define PERM_NOTIMEOUT  PERM_SYSOP

#define PERM_READMAIL   PERM_BASIC
#define PERM_VOTE       PERM_BASIC

/* These are used only in Internet Mail Forwarding */
/* You may want to be more restrictive than the default, especially for an
   open access BBS. */

#define PERM_SETADDR    PERM_POST      /* to set address for forwarding */
#define PERM_FORWARD    PERM_BASIC     /* to do the forwarding */

/* Don't mess with this. */
#define HAS_PERM(x)     ((x)?currentuser.userlevel&(x):1)
#define DEFINE(x)     ((x)?currentuser.userdefine&(x):1)
#define HAS_DEFINE(x,y)	((y)?x&(y):1)
/* HAS_DEFINE(userdefine, flag) */

#ifndef EXTERN
extern char *permstrings[];
#else

/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
   depending on how/if you use them. */
/* skyo.0507 modify �[�J�᭱�� PERM ��K�� menu.ini ��ӡ^ */
char *permstrings[] = {
        "���v�O       (PERM_BASIC)",  /* PERM_BASIC */
        "�i�J��ѫ�     (CHAT)",        /* PERM_CHAT */
        "�I�s�L�H���   (PAGE)",        /* PERM_PAGE */
        "�o��峹       (POST)",        /* PERM_POST */
        "�ϥΪ̸�ƥ��T (LOGINOK)",     /* PERM_LOGINOK */
        "�T��o��峹   (DENYPOST)",    /* PERM_DENYPOST */
        "�����N         (CLOAK)",	/* PERM_CLOAK */
        "�ݬ������N     (SEECLOAK)",    /* PERM_SEECLOAK */
        "�b���ä[�O�d   (XEMPT)",       /* PERM_XEMPT */
        "�s��i���e��   (WELCOME)",     /* PERM_WELCOME */
        "�O�D           (BOARDS)",      /* PERM_BOARDS */
        "�b���޲z��     (ACCOUNTS)",    /* PERM_ACCOUNTS */
        "�������n��     (CHATCLOAK)",   /* PERM_CHATCLOAK */
        "�벼�޲z��     (OVOTE)",       /* PERM_OVOTE */
        "�t�κ��@�޲z�� (SYSOP)",       /* PERM_SYSOP */
        "Read/Post ���� (POSTMASK)",    /* PERM_POSTMASK */
        "��ذ��`��     (ANNOUNCE)",    /* PERM_ANNOUNCE*/
        "�Q�װ��`��     (OBOARDS)",     /* PERM_OBOARDS*/
        "���ʬݪ��`��   (ACBOARD)",     /* PERM_ACBOARD*/
        "���� ZAP �Q�װ�(NOZAP)", 	/* PERM_NOZAP*/
        "�j��I�s       (FORCEPAGE)",   /* PERM_FORCEPAGE*/
        "�����o�b�ɶ�   (EXT_IDLE)",    /* PERM_EXT_IDLE*/
        "�S���v�� 1     (SPECIAL1)",    /* PERM_SPECIAL1*/
        "�S���v�� 2     (SPECIAL2)",    /* PERM_SPECIAL2*/
        "�S���v�� 3     (SPECIAL3)",    /* PERM_SPECIAL3*/
        "�S���v�� 4     (SPECIAL4)",    /* PERM_SPECIAL4*/
        "�S���v�� 5     (SPECIAL5)",    /* PERM_SPECIAL5*/
        "�S���v�� 6     (SPECIAL6)",    /* PERM_SPECIAL6*/
        "�S���v�� 7     (SPECIAL7)",    /* PERM_SPECIAL7*/
        "�S���v�� 8     (SPECIAL8)",    /* PERM_SPECIAL8*/
        NULL
};
#endif

#define DEF_FRIENDCALL   0x00000001
#define DEF_ALLMSG       0x00000002
#define DEF_FRIENDMSG    0x00000004
#define DEF_SOUNDMSG     0x00000008
#define DEF_COLOR        0x00000010
#define DEF_ACBOARD      0x00000020
#define DEF_ENDLINE      0x00000040
#define DEF_EDITMSG      0x00000080
#define DEF_NOTMSGFRIEND 0x00000100
#define DEF_NORMALSCR    0x00000200
#define DEF_NEWPOST      0x00000400
#define DEF_CIRCLE       0x00000800
#define DEF_FIRSTNEW     0x00001000
#define DEF_LOGFRIEND    0x00002000
#define DEF_LOGINFROM    0x00004000
#define DEF_NOTEPAD      0x00008000
#define DEF_NOLOGINSEND  0x00010000
#define DEF_THESIS	 	 0x00020000	/* youzi */
#define DEF_MSGGETKEY    0x00040000
#define DEF_GRAPH        0x00080000
#define DEF_TOP10        0x00100000
#define DEF_RANDSIGN	 0x00200000
#define DEF_S_HOROSCOPE  0x00400000
#define DEF_COLOREDSEX	 0x00800000


#define NUMDEFINES 24
#ifndef EXTERN
extern char *user_definestr[];
#else
/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
   depending on how/if you use them. */
char *user_definestr[] = {
        "�I�s�������ɥi���n�ͩI�s",     /* DEF_FRIENDCALL */
        "�����Ҧ��H���T��",             /* DEF_ALLMSG */
        "�����n�ͪ��T��",               /* DEF_FRIENDMSG */
        "����T���o�X�n��",             /* DEF_SOUNDMSG */
        "�ϥαm��",             		/* DEF_COLOR */
        "��ܬ��ʬݪ�",             	/* DEF_ACBOARD */
        "��ܿ�檺�T����",             /* DEF_ENDLINE */
        "�s�����ܪ��A��",     		/* DEF_EDITMSG */
        "�T����ĥΤ@��/��²�Ҧ�",		/* DEF_NOTMSGFRIEND */
        "���ĥΤ@��/��²�Ҧ�",		/* DEF_NORMALSCR */
        "�����Q�װϥH New ���",		/* DEF_NEWPOST */
        "�\\Ū�峹�O�_�ϥ�¶�����",   	/* DEF_CIRCLE */
        "�\\Ū�峹��а���Ĥ@�g��Ū",	/* DEF_FIRSTNEW */
        "�i������ܦn�ͦW��",   		/* DEF_LOGFRIEND */
        "�n�ͤW���q��",                 /* DEF_LOGINFROM */
        "�[�ݯd����",                   /* DEF_NOTEPAD*/
        "���n�e�X�W���q�����n��",       /* DEF_NOLOGINSEND */
        "�D�D���ݪ�",                   /* DEF_THESIS */
        "����T�����Ԧ^���βM��",       /* DEF_MSGGETKEY */
        "�i�����[�ݤW���H����",         /* DEF_GRAPH */
        "�i�����[�ݤQ�j�Ʀ�O",         /* DEF_TOP10 */
        "�ϥζü�ñ�W��",				/* DEF_RANDSIGN */
        "��ܬP�y",						/* DEF_S_HOROSCOPE */
        "�P�y�ϥ��C�����ܩʧO",		/* DEF_COLOREDSEX */
		NULL
};
#endif
