/*-------------------------------------------------------*/
/* config.h                   (SOB 0.22 ATS Verion 1.32) */
/*-------------------------------------------------------*/
/* target : site-configurable settings                   */
/* create : 95/03/29                                     */
/* update : 02/11/07 (Dopin)                             */
/*-------------------------------------------------------*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

/* ----------------------------------------------------- */
/* �w�q BBS ���W��}                                     */
/* ------------------------------------------------------*/

#define BOARDNAME       "�ȯS������"                    /* ���寸�W */
#define BBSNAME         "Atlantis"                      /* �^�寸�W */
#define MYHOSTNAME      "localhost"                     /* ����W�� */
#define MYIPADDRESS     "111.222.333.444"               /* ������} */

#define MYVERSION       "ATLANTIS BBS 1.32-RELEASE"     /* �{������ */
#define MYBBSID         "ATSRCU"                        /* ��H�N�X */

#define BBSHOME         "/home/bbsrs"                   /* BBS ���a */
#define BBSUSER         "bbsrs"                         /* BBS �ϥΪ̱b�� */
#define BBSUID          9997                            /* BBS User ID    */
#define BBSGID          97                              /* BBS Group ID   */

#define BBSSHELL        BBSHOME "/bin/bbsrf"            /* �e�ݵ{�� */
#define BBSPROG         BBSHOME "/bin/bbs"              /* �D�{����m */
#define BBSCODE         BBSHOME "/src/maple"            /* �{���X�ؿ� */
#define BBSHELP         BBSHOME "/ascii/help"           /* �D�U�ɥؿ� */

/* ----------------------------------------------------- */
/* ���c�w�q �мf�V�P�_��ڪ��p �]���A�� BBS ����|�X���D */
/* (1.31-S New Patch)                                    */
/* ------------------------------------------------------*/

/* --------------------- �Ъ`�N �o���̥u��ܤ@�w�q ---------------------- */
#define  NEW_STATION                  /* �w�q�� �s��/�з� ���x            */
#undef   OLD_ATSVERSION               /* �w�q���ª��ȯ����� (1.20a �H��)  */
                                      /* �����ª����ϥγ̤p�ϥΪ̸�Ƶ��c */
                                      /* ���ݰ����ഫ�{�� single_multi_st */
#undef   TRANS_FROM_SOB               /* �w�q���F�y�ഫ                   */
#undef   TRANS_FROM_FB3               /* �w�q�������ഫ                   */
#undef   TRANS_FROM_COLA              /* �w�q���i���ഫ                   */
/* ---------------------------------------------------------------------- */

#ifdef   TRANS_FROM_SOB
  #define DEFAULTBOARD   "0Announce"  /* SOB �w�]�ݪO�W �`�N��W�٥��ݥ��T */
#else
  #define DEFAULTBOARD   "Announce"   /* �w�]�ݪO ���ˬd boards/Announce */
                                      /* �O�_�T��s�b */
#endif

#ifdef   TRANS_FROM_FB3
  #undef ENCPASSLEN                   /* �w�q�Y�����ഫ ���L�]�w ENCPASSLEN */
#endif

#ifdef   TRANS_FROM_FB3
  #define  FB_FAMILY                  /* �v�T�s�դ��� ���i undef ���D��ӧ� */
#endif

#ifdef   TRANS_FROM_COLA
  #define  FB_FAMILY                  /* �v�T�s�դ��� ���i undef ���D��ӧ� */
  #define  CHECK_COLA_ASCII           /* �P�w Cola �[�J�� ASCII ��ĳ�ϥ� */
#endif                                /* �s�� Atlantis �������ഫ �Ь����� */

#ifndef  OLD_ATSVERSION
#undef   MIN_USEREC_STRUCT            /* �w�q�ϥγ̤p�ϥΪ̸�Ƶ��c ���\�� */
#endif                                /* ����ĳ�ϥ� �|���h�\�h�\�� ���D��  */
                                      /* �x���Y���w�魭�� �B���Q�]�Ӧh�\�� */

/* ----------------------------------------------------- */
/* �պA�W��                                              */
/* ------------------------------------------------------*/

#define WITHOUT_CHROOT             /* ���ݭn root set-uid */
#define HAVE_MOVIE                 /* ��ܰʺA�i�ܪO */
#define HAVE_INFO                  /* ��ܵ{����������  �ФŧR�� */
#define HAVE_License               /* ��� GNU ���v�e�� �ФŧR�� */
#define INTERNET_PRIVATE_EMAIL     /* �i�H�H�p�H�H��� Internet */
#define BIT8                       /* �䴩����t�� */

#define LOGINATTEMPTS   (3)        /* �̤j�i�����~���� */
#define LOGINASNEW      (1)        /* �ĥΤW���ӽбb����� */
#define VOTE            (1)        /* ���ѧ벼�\�� */
#define INTERNET_EMAIL             /* �䴩 InterNet Email �\��(�t Forward) */

#undef  EMAIL_JUSTIFY              /* �o�X InterNet Email �����{�ҫH�� */
#undef  NEWUSER_LIMIT              /* �s��W�����T�ѭ��� */
#undef  INVISIBLE                  /* ���� BBS user �Ӧۦ�B */
#define MULTI_LOGINS               /* ���\�P�ɦh���W�� */

#undef  POSTBUG                    /* board/mail post �S�� bug �F �Ъ��}�� */
#undef  HAVE_REPORT                /* �t�ΰl�ܳ��i ����ĳ�ϥ� */
#define INITIAL_SETUP              /* ��}���A�٨S�إ߹w�]�ݪO[SYSOP] */
#define HAVE_MMAP                  /* �ĥ� mmap(): memory mapped I/O */
                                   /* �b SunOS�BFreeBSD �W�i�H�[�t 30 �� */
                                   /* Linux �� mmap() �����D�A�мȮɤ��n�� */

#define HAVE_ANONYMOUS             /* ���� Anonymous �O */
#define HAVE_ORIGIN                /* ��ܤ峹�Ӧۦ�B */
#undef  RECORD_MODIFY_ARTICAL      /* �O���峹�ܧ�̻P�ɶ� */
#undef  HAVE_MAILCLEAN             /* �M�z�Ҧ��ϥΪ̭ӤH�H�c */
#undef  HAVE_SUICIDE               /* ���ѨϥΪ̦۱��\�� */

/* ----------------------------------------------------- */
/* ���x�򥻳]�w                     Atlantis BBS 1.31-S1 */
/* ----------------------------------------------------- */

#define DEFAULTST      "atlantis"  /* �w�q�w�]���x�W�� �p���ܧ�аѦ� */
                                   /* [���] �p���� ATS �w�]���x */
#define NOC_DFB                    /* �w�q���\�ϥΪ̤����j��\Ū�w�]�ݪO */
                                   /* ��ĳ�� keichii (ats) */

#define KEEPWD1        "ats"       /* �w�q���W Anonymous �t�αb�� */
#define KEEPWD2        "atlantis"  /* �P�W ����b���u�i��藍�i�R�� */

#define SHOW_FROMHOST              /* �ϥΪ̤@����ܬG�m�ӷ� */
#ifdef SHOW_FROMHOST
  #define TRANS_FROMHOST           /* �G�m�ഫ�\�� �@�� buggy/Izero (zoo) */
  #ifdef  TRANS_FROMHOST
    #define FROMHOST_BY_FILE       /* �G�m�ഫ�ϥ��ɮ� �@�� Dopin (ats) */
  #endif                           /* �p��������ɮ� �Фϩw�q�Q�ΰO����}�C */
  #define USER_DEFINE_HOST         /* �ϥΪ̥i�H�۩w�G�m �\���ĳ aoki (ats) */
#endif                             /* ��s�� Eros/LAW (hcu) */

#define USE_MULTI_TITLE            /* �w�q�ϥ��H�����D�C */
#define USE_SQ_CURSOR              /* �w�q�ϥδ`�Ǵ�� */
#ifdef USE_SQ_CURSOR
  #define CURS_STRING "�ȯS������" /* �Ш̻ݭn�ܧ� �C�@�r�ݬ����Φr */
  #define CURS_STR_NUM 10          /* �й�� CURSOR_STRING �ƥج��b���`�� */
#endif

/* ----------------------------------------------------- */
/* ���x�޲z��h                     Atlantis BBS 1.31-S1 */
/* ----------------------------------------------------- */

#define SYSOP_CAN_LOGIN            /* �O�_���\ SYSOP �ѻ��ݵn�J */
#define SUPERVISOR      "SYSOP"    /* �w�q�W�ŨϥΪ� �\�h�\��Ȧ� ID �i���� */
#define SYSOP_ONLY_DEFAULTST       /* �w�q�u���� SYSOP �v���̬��w�]���x���� */

#define USE_ACCOUNTS_PERM          /* �w�q�ϥ� PERM_ACCOUNTS (�b���`��)�\�� */
#define USE_BOARDS_MANAGER         /* �w�q�ϥ� PERM_EXTRA1 (�ݪO�`��)�\��   */

#define MAIL_TO_SYSOP              /* �w�q�ϥΪ̥i�H�H�H�� SYSOP �b�� */
#define CHECK_LIST_ID_SELF         /* �w�q �n��/�a�H/�W�u �W�椣�i�]�w�ۤw */
#define DENY_MESSAGE_WHILE_PURPLE  /* �w�q�����ɥu�������v���i�H����y */

#define CHANGE_USER_MAIL_LIMIT     /* �p���Q�u���ܧ�ϥΪ̫H�c�W�� �Фϩw�q */

/* ----------------------------------------------------- */
/* ���x�����\��]�w                 Atlantis BBS 1.32-A0 */
/* ----------------------------------------------------- */

#define MultiIssue             /* �ϥζüƶi���e�� */
#ifdef  MultiIssue
  #define USE_LITTLE_NOTE      /* �w�q�ϥ��H���i���~�i�w�q�ϥΰʺA�p�ݪO */
#endif

#define ColorClass             /* �ϥαm��ݪO���� */
#ifdef  ColorClass
  #define CHColor 0            /* �w�q�m��ݪO���� ���G�����(1) �C�G��(0) */
#endif

#undef  UPDATE_WHEN_POST       /* �o���Y��ϥΪ̤峹�ƥؼg�J .PASSWDS */
#ifdef  UPDATE_WHEN_POST
  #undef COUNT_MONEY_WHEN_POST /* �o���N�o�� */
#endif

#define REC_ALL_FROM           /* �O���Ҧ��ϥΪ̤W���ӷ� ���q�L�{�ҵo����� */
#ifdef REC_ALL_FROM
  #undef REC_LOGINOK           /* �w�q�L�{�ҨϥΪ̵o��O���W���ӷ� */
#endif

#undef  COUNT_PERSONAL         /* �ӤH / �s�� �����ݪO�p��o��� */
#define COUNT_MAXLOGINS        /* �O���̰��W�u�H�� */

#define REVIEW_WATER_BALL      /* �w�q���Ѥ��y�^�U */
#ifdef  REVIEW_WATER_BALL
  #define RECORD_NEW_MSG       /* �O���s�ӰT���p����ܤ� */
  #define CTRL_R_REVIEW        /* �w�q�ϥ� (Ctrl+R)^2 ����ǰT�^�U */
  #define CTRL_G_REVIEW        /* �w�q�ϥ�  Ctrl+G    ����ǰT�^�U */
#endif

#define TRANS_ST_TASK          /* �w�q�ϥ� ���x�W���ɼ����ഫ (ATS 1.32-B1) */
                               /* ��ĳ�� keysound (shadowcity) */

#undef  SAVE_MAIL              /* �w�q�ϥ� save_mail �禡 */
#undef  CHOICE_RMSG_USER       /* �w�q�ϥ� ��ܶǰT��H���� */
#define LOCK_ARTICAL           /* �w�q�ϥ� ��w�峹�\�� */

#define SHOW_BRDMSG            /* �w�q��� �ݪO�p�۵P��ܬݪO�X�й�����T */
                               /* ��ĳ�� sagit/ktasl (blue.ccsh.kh.edu.tw) */

#define GOOD_BABY              /* �w�q�ϥ� ���_�_�ۧڬ��� ���[�\�� */
#ifdef GOOD_BABY               /* ��ĳ�� bobi (ats.twbbs.org) */
  #undef MOVIE_AND_GOODBABY    /* �ʵe��s�@�� �ˬd�@���ϥΦۧڬ����ɶ�����
                                  �ӤW�u���ϥΪ̬O�_�b���W �ϩw�q�h�w�I����
                                  �ɤ~�ˬd�@�� (�C�b�p��)                   */
#endif

/* ----------------------------------------------------- */

/* �ݪO��h                         Atlantis BBS 1.32-A0 */
/* ----------------------------------------------------- */

#define BM_PRINCIPLE                    /* �w�q�Ҧ��O�D��h �ϩw�q������O�D */

#undef  DEL_BRD_WITH_MAN                /* �R���ݪO�@�֧R����ذ� */

/* ----------------------------------------------------- */
/* �H BBS ���W�Ҧ������X�W (Dopin: �@�ܧ�ݲM SHM)       */
/* ----------------------------------------------------- */

#define MAXUSERS        (128)         /* �̰����U�H�� */
#define MAXBOARD        (128)         /* �̤j�}���Ӽ� */
#define MAXACTIVE       (32)          /* �̦h�P�ɤW���H�� */

#undef  NO_FORCE_LISTS                /* �w�q "���T�{" �W��O�_�W�L�W�� */
#ifndef NO_FORCE_LISTS
  #define MAX_BOARDFAVOR  (128)       /* �ȬO�w�q�� check_file_line �Ǧ^�P�_ */
#endif

#define MAX_FRIEND      (64)          /* ���J cache ���̦h�B�ͼƥ� */
#define MAX_ALOHA       (64)          /* ���J cache ���̦h�W�u�q�� by Dopin */
#define MAX_REJECT      (32)          /* ���J cache ���̦h�a�H�ƥ� */

/* ----------------------------------------------------- */
/* �h���t�ά��� (ATS 1.31-S New Patch)                   */
/* ----------------------------------------------------- */

#undef  NO_USE_MULTI_STATION          /* �w�q�O�_ "���ϥ�" �h���t�Υ\�� */
#define SYSOP_SEE_ALL                 /* �w�q�O�_ SYSOP �i�H�ݨ��Ҧ��H */

#ifndef NO_USE_MULTI_STATION          /* �p�G�S�w�q "���ϥ�" �h���t�� */
  #define ONLY_SEE_SAME_STATION       /* �w�q�O�_�u�ݪ���P���x���H */

  #ifdef ONLY_SEE_SAME_STATION        /* �b�u��ݨ��P���x�����p�U */
    #define FRIEND_CAN_SEE_ANYWHERE   /* �w�q�u�n���]�n�� �b���ӯ����ݪ��� */
    #define DEFAULT_ST_SEE_ALL        /* �w�q�w�]���x�i�H�ݨ�������x���H */

    #ifdef DEFAULT_ST_SEE_ALL         /* �b�w�]���i�H�ݨ�������x�����p�U */
      #define CANT_SEE_HIDE_STATAION  /* �w�q����ݨ����ï����H */
    #endif
  #else                               /* �b�i�H�ݨ�Ҧ����x���H�����p�U */
    #define CANT_SEE_HIDE_STATAION    /* �w�q����ݨ����ï����H */
  #endif
#endif

/* ----------------------------------------------------- */
/* BBS Daemon ���� �ۥѰ�/���x���� (1.31-S New Patch)    */
/* ----------------------------------------------------- */

#ifndef NO_USE_MULTI_STATION
  #define MAX_STATIONS  19            /* �̤j���x�� ����\�h�Ŷ������c�j�p */
  #undef  SEE_ALL_STATION             /* �ϥΪ̤������O�_�ݨ�L�k�i�J�����x */
#else
  #define MAX_STATIONS  1             /* ���w�q�h�� ���x�۵M�u���@�� */
#endif

#define DEFAULT_BBSD 'Z' // �� 'M' �b���i�D�t�ιw�]�� bbsd �O zbbsd �� mbbsd

#undef  USE_FREE_PORT                 /* �w�q�ϥΦۥѰ�Ҧ� �_�h�̯��x���� */
#ifdef  USE_FREE_PORT
  #define ZBBSD_FREE_PORT             /* in.zbbsd �P mbbsd ���ۥѰ�w�q */
  #define MBBSD_FREE_PORT             /* ��̽лP DEFAULT_BBSD �]�w�@�P */
#else
  #define ZBBSDP 3001                 /* ���ϥΦۥѰ�ɳo��өw�q���i���� */
  #define MBBSDP 9001                 /* ���O�i�H�ק�𪺰_�I��m */
#endif

/* ----------------------------------------------------- */
/* ��L�t�ΤW���Ѽ�                                      */
/* ----------------------------------------------------- */

#define MAXKEEPMAIL     (100)      /* �̦h�O�d�X�� MAIL�H */
#define MAX_NOTE        (20)       /* �̦h�O�d�X�g�d���H */
#define MAXSIGLINES     (6)        /* ñ�W�ɤޤJ�̤j��� */
#define MAXSTLOGS       (5)        /* �ϥΪ̦W���ɳ̤j��� */
#define MAXQUERYLINES   (100)      /* ��� Query/Plan �T���� Dopin: MAX 250 */
#define MAXPAGES        (1500)     /* more.c ���峹���ƤW�� (lines/22) */
#define MOVIE_INT       (30)       /* �����ʵe���g���ɶ��G30 �� */
#define INDEX_READ_NUM  (128)      /* Dopin: �w�q��C�Ҧ��@���̦hŪ�X�� */

#undef CHECK_POST_TIME
#ifdef CHECK_POST_TIME
  #define POST_RANGE    (10)       /* �C�g��i���j�h�֮ɶ� �Ъ`�N ���O�� */
  #define POST_LOWER    (60)       /* �X��������O�ֳt�o�� */
  #define POST_NUMBER   (5)        /* �X�g�H�W�ˬd�O�_�ֳt�o�� */
#endif

/* ----------------------------------------------------- */
/* �o�b�L�[�۰�ñ�h / �t�εo�b�\��                       */
/* ------------------------------------------------------*/

#define SHOW_IDLE_TIME                /* ��ܶ��m�ɶ� ��ĳ�ϥ� */
#define DOTIMEOUT                     /* �w�q�o�b�L�[�۰�ñ�h (��X) */
                                      /* Dopin �ץ�����T��i�H undef */

#ifdef  DOTIMEOUT
  #define IDLE_TIMEOUT    (60*30)     /* �@�뱡�p�� timeout */
  #define MONITOR_TIMEOUT (60*15)     /* monitor �ɤ� timeout */
#endif

#undef TAKE_IDLE                      /* �w�q���ѨϥΪ̵o�b�\�� ����ĳ�ϥ�   */
                                      /* ��]�O���ӱb�� Bug �o�b�L�[�� SHM   */
                                      /* �i��|���� �p�o�ͮɽЧQ�� chkpasswd */
                                      /* ����״_ */

/* ----------------------------------------------------- */
/* chat.c & xchatd.c ���ĥΪ� port �� protocol           */
/* ------------------------------------------------------*/

#ifndef TRANS_FROM_SOB
#define CHATPORT        5722          /* ��ѫ� PORT */
#else
#define CHATPORT        5922
#endif
#define MAXROOM         32            /* �̦h���X���]�[�H */

#define EXIT_LOGOUT      0
#define EXIT_LOSTCONN   -1
#define EXIT_CLIERROR   -2
#define EXIT_TIMEDOUT   -3
#define EXIT_KICK       -4

#define CHAT_LOGIN_OK       "OK"
#define CHAT_LOGIN_EXISTS   "EX"
#define CHAT_LOGIN_INVALID  "IN"
#define CHAT_LOGIN_BOGUS    "BG"

#define BADCIDCHARS " *"               /* Chat Room ���T�Ω� nick ���r�� */

/* ----------------------------------------------------- */
/* �t�ΰѼ�                                              */
/* ----------------------------------------------------- */

#define MAGIC_KEY       1234           /* �����{�ҫH��s�X */

#ifndef TRANS_FROM_SOB
#define BRDSHM_KEY      1215
#define UIDSHM_KEY      1217
#define UTMPSHM_KEY     1219
#else
#define BRDSHM_KEY      1415
#define UIDSHM_KEY      1417
#define UTMPSHM_KEY     1419
#endif

/* ----------------------------------------------------- */
/* �ӽбb���ɭn�D�ӽЪ̯u����                          */
/* ----------------------------------------------------- */

#define SHOWUID                        /* �ݨ��ϥΪ� UID */
#define SHOWTTY                        /* �ݨ��ϥΪ� TTY */
#define SHOWPID                        /* �ݨ��ϥΪ� PID */

#define REALINFO                       /* �u��m�W */
#ifdef  REALINFO
  #undef  ACTS_REALNAMES               /* �D�ؿ��� (U)ser ��ܯu��m�W */
  #undef  POST_REALNAMES               /* �K���ɪ��W�u��m�W */
  #undef  MAIL_REALNAMES               /* �H�����H��ɪ��W�u��m�W */
  #undef  QUERY_REALNAMES              /* �Q Query �� User �i���u��m�W */
#endif

/* ----------------------------------------------------- */
/* Dopin : �~�[�{���]�w (�Ш̻ݭn�ܧ�w�q)               */
/* ----------------------------------------------------- */

/* �峹���D���O */
#define ARTICAL_CLASS         // ²�櫬�峹���O ��@�� Dopin (ats.twbbs.org)

/* �߱��I�q��  */
#define ORDERSONG             // ����I�q�� ��@�� Dopin (ats.twbbs.org)
#ifdef  ORDERSONG
  #define OSONG_NUM_LIMIT 3   /* �w�q�@�ѥu���I�X���q �L����h undef */
  #define ORDER_SONG_CAN_MAIL /* �w�q�I���q���i�H��H�ܹ�H�H�c */
  #ifdef ORDER_SONG_CAN_MAIL
     #undef ANONYMOUS_ORSONG  /* �w�q�i�ΦW�I�q��H���H�c */
  #endif
  #define OSONG_ANN           /* SongBooks �i�ϥΥؿ��I�q */
  #ifdef  OSONG_ANN
    #define SONG_CHOICE_MANDIR "SongBooks"
                              /* �ؿ����I�q�Ҧb��ذϦW�� */
  #endif
  #define OSONG_SEARCH        /* �w�q�j�M���I�q */
  #ifdef  OSONG_SEARCH
    #define SONG_SEARCH_MANDIR "SongBooks"
                              /* �j�M���I�q�Ҧb��ذϦW�� �ݷf�t hn_index */
  #endif
  #undef  SONGCOST            /* �w�q�I�q�O�_�ݭn��� */
  #ifdef  SONGCOST
    #define SONGCOST_VALUE 10 /* �w�q�I�q�n��h�ֿ� */
  #endif
#endif

/* �Ÿ���J�u�� */
#define INPUT_TOOLS           // �Ÿ��u�� ��@�� weichung (bbs.ntit.edu.tw)
                              //          �ק�� Dopin (ats.twbbs.org)

/* ����{�����窩 */
#undef  DRAW                  // ����{�� ��@�� Dopin (ats.twbbs.org)

/* �����t�t�� */            // ��@�� Dopin (ats.twbbs.org)
#ifndef MIN_USEREC_STRUCT
  #undef RPG                  // �w�q�O�_�s�� RPG �������
  #ifdef  RPG
    #define RPG_STATUS        /* �w�q�O�_�� Query ����ܦU����O */
    #define RPG_SETTING       /* �w�q�O�_��ϥΪ̳]�w�����U����O */
    #undef  RPG_FIGHT         /* ��Ե{�����K�� �S�ΤS�t���F�� Q__Q */
  #endif
#endif

/* Ķ��q �s�u�{�� */
#define DREYE                  // �Ȥ�� ��@�� statue (bbs.yzu.edu.tw)
                               //        �ק�� Dopin (ats.twbbs.org)

/* �P�ž԰��� */
#define STAR_CHICKEN           // ��@�̸s�p�U :
/* chyiuan (great.dorm12.nctu.edu.tw) [�D�o�i]                             */
/* dsyan, fennet, tiball, wildcat, Frank, startrek ���h��                  */
/* ���ɭ�@�� tiball (bbs.nhctc.edu.tw)                                    */
/* �ק�� Dopin (ats.twbbs.org)                                            */

/* �s�p�t�Τ��ص{�� */
#define AGREE_SYS             // �s�p�t�ε{�� ��@�� Dopin (ats.twbbs.org)

/* �C�X���U�W�� */
#define USE_ULIST             // �� SOB ���ص{��

#define QUERY_REMOTEUSER      // �d�̪߳�W���O�� Dopin (ats.twbbs.org)

/* ----------------------------------------------------- */
/* ���ѥ~���{��                                          */
/* ----------------------------------------------------- */

#undef HAVE_EXTERNAL         /* Dopin: ���w�q��ĳ���� �`�٪Ŷ� */

/* Dopin: �U���o�䳣�L�k�ϥ� �O�d�O�]���p���ݭn�~���U�C�禡 �i��K�s�� */
#ifdef  HAVE_EXTERNAL
#undef  HAVE_TIN              /* ���� news reader */
#undef  HAVE_GOPHER           /* ���� gopher */
#undef  HAVE_WWW              /* ���� www browser */
#undef  HAVE_BRIDGE           /* ���� bridge game */
#undef  HAVE_BBSDOORS         /* ���� bbs doors */
#undef  HAVE_GAME             /* ���Ѻ����s�u�C�� */
#undef  HAVE_ADM_SHELL        /* ���� csh �H�K�޲z */
#endif

/* ��o�䬰�� �o�ǬO SOB �ɥN���F�� ��l�X�h�w�L�ɵL�k�ϥ� */
/* �p������l�X �B���ݭn�ϥΦA�w�q�ϥ� */

#endif
