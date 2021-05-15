/*-------------------------------------------------------*/
/* global.h     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : global definitions & variables               */
/* create : 95/03/29                                     */
/* update : 02/09/05 (Dopin)                             */
/*-------------------------------------------------------*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

/* ----------------------------------------------------- */
/* GLOBAL DEFINITION                                     */
/* ----------------------------------------------------- */

/* �ɦW�]�w */

#define FN_PASSWD       ".PASSWDS"      /* User records */
#define FN_BOARD        ".BOARDS"       /* board list */
#define FN_USIES        "usies"         /* BBS log */

#define DEFAULT_BOARD   str_sysop
#define NUMPERMS        32

void my_outmsg(char *);
int HAS_PERM(usint);

/* ��L�]�w */

#ifndef EXTEND_KEY
#define EXTEND_KEY
#define KEY_TAB         9
#define KEY_ESC         27
#define KEY_UP          0x0101
#define KEY_DOWN        0x0102
#define KEY_RIGHT       0x0103
#define KEY_LEFT        0x0104
#define KEY_HOME        0x0201
#define KEY_INS         0x0202
#define KEY_DEL         0x0203
#define KEY_END         0x0204
#define KEY_PGUP        0x0205
#define KEY_PGDN        0x0206
#endif

#define Ctrl(c)         ( c & 037 )

#ifdef SYSV
#undef CTRL                     /* SVR4 CTRL macro is hokey */
#define CTRL(c) ('c'&037)       /* This gives ESIX a warning...ignore it! */
#endif

#define chartoupper(c)  ((c >= 'a' && c <= 'z') ? c+'A'-'a' : c)
#define char_lower(c)  ((c >= 'A' && c <= 'Z') ? c|32 : c)

/* ----------------------------------------------------- */
/* External function declarations                        */
/* ----------------------------------------------------- */

char *str_decode_M3(uschar *, char);
char *Belong();
char *my_ctime();
char *mono(char*);
char *subject();
char *trim();
void showtitle();
void show_help();
void showplans();
void user_display();
void log_usies();
void talkreply();
void uinfo_query();
void touch_boards();
void namecomplete();
void usercomplete();
void stampfile();
int  scr_exec();
int  edit_note();
int  t_users();
int  m_read();
int  t_idle();
int  getdata(int line, int col, char* prompt, char* buf, int len, int echo, char* ans);
#define TRACE   log_usies

/* #define      setutmpmode(x)  (currutmp->mode = (x)) */

/* ----------------------------------------------------- */
/* �T���r��G�W�ߥX�ӡA�H�Q�䴩�U�ػy��                  */
/* ----------------------------------------------------- */

#ifndef USE_SQ_CURSOR
/* �o�ǩw�q�u�O���ª��t�� �Q����H�N �p�ϥδ`�Ǵ�� �o��L�@�� */
#ifdef TRANS_FROM_SOB
#define STR_CURSOR      "��"
#endif
#ifdef TRANS_FROM_FB3
#define STR_CURSOR      "��"
#endif
#ifdef TRANS_FROM_COLA
#define STR_CURSOR      "��"
#endif
#ifdef NEW_STATION
#define STR_CURSOR      "> "
#endif
#ifdef OLD_ATSVERSION
#define STR_CURSOR      "> "
#endif
/* ��o�� */
#endif

#define STR_UNCUR       "  "

#ifndef TRANS_FROM_FB3
#define STR_AUTHOR1     "�@��:"
#define STR_AUTHOR2     "�o�H�H:"
#define STR_POST1       "�ݪO:"
#define STR_POST1C      "���:"
#define STR_POST2       "����:"
#else
#define STR_AUTHOR1     "�@��:"
#define STR_AUTHOR2     "�o�H�H:"
#define STR_POST1       "�H��:"
#define STR_POST1C      "�o�H��:"
#define STR_POST2       "����:"
#endif

#define CHICKENTALK     0
#define LEN_AUTHOR1     5
#define LEN_AUTHOR2     7

#define STR_GUEST       "guest"


#define MSG_SEPERATOR   "\
�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w"

#define MSG_CLOAKED     "�����I�����ΤF�I�ݤ����... :P"
#define MSG_UNCLOAK     "�ڭn���{����F...."

#define MSG_WORKING     "�B�z���A�еy��..."

#define MSG_WAWA        "��z�z�z�z�ڰڰڰڰڡ���!!!  ~~~>_<~~~"

#define MSG_CANCEL      "�����C"
#define MSG_USR_LEFT    "User �w�g���}�F"
#define MSG_NOBODY      "�ثe�L�H�W�u"

#define MSG_DEL_OK      "�R������"
#define MSG_DEL_CANCEL  "�����R��"
#define MSG_DEL_ERROR   "�R�����~"
#define MSG_DEL_NY      "�нT�w�R��(Y/N)?[N] "

#define MSG_FWD_OK      "�峹��H����!"
#define MSG_FWD_ERR1    "��H���~: system error"
#define MSG_FWD_ERR2    "��H���~: address error"

#define MSG_SURE_NY     "�бz�T�w(Y/N)�H[N] "
#define MSG_SURE_YN     "�бz�T�w(Y/N)�H[Y] "

#define MSG_BID         "�п�J�ݪO�W�١G"
#define MSG_UID         "�п�J�ϥΪ̥N���G"
#define MSG_PASSWD      "�п�J�z���K�X: "

#define MSG_BIG_BOY     "�L�����i��  �H��"
#define MSG_BIG_GIRL    "�h�ܪ��S��  �~��"
#define MSG_LITTLE_BOY  "�۵M������  ����"
#define MSG_LITTLE_GIRL "�j�a���B��  �a�F"
#define MSG_MAN         "���˪��@��  ????"
#define MSG_WOMAN       "�j�Ѫ��ǻ�  �s��"
#define MSG_PLANT       "�k�v���H�x  ����"
#define MSG_MIME        "�V�P�����  �]��"

#define ERR_BOARD_OPEN  ".BOARD �}�ҿ��~"
#define ERR_BOARD_UPDATE        ".BOARD ��s���~"
#define ERR_PASSWD_OPEN ".PASSWDS �}�ҿ��~"

#define ERR_BID         "�A�d���F�աI�S���o�ӪO��I"
#define ERR_UID         "�o�̨S���o�ӤH�աI"
#define ERR_PASSWD      "�K�X�����I�A���S���_�ΤH�a���W�r�ڡH"
#define ERR_FILENAME    "�ɦW���X�k�I"


#define MSG_SELECT_BOARD        \
        "[7m�i ��ܬݪO �j[0m\n�п�J�ݪO�W��(���ť���۰ʷj�M)�G"

#ifdef USE_MULTI_TITLE
#define MSG_POSTER      \
" �峹��Ū [31;47m (y)[30m�^�H [31m(=[]<>)[30m�����D�D [31m(/?)[30m�j�M���D [31m(aA)[30m�j�M�@�� [31m(x)[30m��� [31m(V)[30m�벼   [0m"
#else
#define MSG_POSTER      \
"[34;46m �峹��Ū [31;47m (y)[30m�^�H [31m(=[]<>)[30m�����D�D [31m(/?)[30m�j�M���D [31m(aA)[30m�j�M�@�� [31m(x)[30m��� [31m(V)[30m�벼   [0m"
#endif

#ifdef USE_MULTI_TITLE
#define MSG_MAILER      \
" �E������  [31;47m  (R)[30m�^�H  [31m(x)[30m��F  [31m(y)[30m�s�զ^�H  [31m(D)[30m�R��  [31m[G][30m�~�� ?                 [0m"
#else
#define MSG_MAILER      \
"[34;46m  �E������  [31;47m  (R)[30m�^�H  [31m(x)[30m��F  [31m(y)[30m�s�զ^�H  [31m(D)[30m�R��  [31m[G][30m�~�� ?                [0m"
#endif

#define MSG_SHORTULIST  "[7m\
�ϥΪ̥N��    �ثe���A   �x�ϥΪ̥N��    �ثe���A   �x�ϥΪ̥N��    �ثe���A   [0m"

#ifdef  _MAIN_C_

/* ----------------------------------------------------- */
/* GLOBAL VARIABLE                                       */
/* ----------------------------------------------------- */

int usernum;
pid_t currpid;                  /* current process ID */
usint currstat;
int currmode = 0;
int curredit = 0;
int showansi = 1;
time_t login_start_time;
userec cuser;                   /* current user structure */
userec xuser;                   /* lookup user structure */
char quote_file[MAXPATHLEN] = "\0";
time_t paste_time;
char paste_title[TTLEN + 1];
char paste_path[MAXPATHLEN];
int  paste_level;
char quote_user[80] = "\0";
char currtitle[TTLEN + 1] = "\0";
char vetitle[TTLEN + 1] = "\0";
char currowner[IDLEN + 2] = "\0";
char currauthor[IDLEN + 2] = "\0";
char currfile[FNLEN];           /* current file name @ bbs.c mail.c */
uschar currfmode;               /* current file mode */
char currmsg[100];
char currboard[(IDLEN+1)*2];
char currBM[IDLEN * 3 + 10];
char reset_color[4] = "[m";

/* global string variables */


/* filename */

char *fn_passwd         = FN_PASSWD;
char *fn_board          = FN_BOARD;
char *fn_note_ans       = "note.ans";
char *fn_register       = "register.new";
char *fn_plans          = "plans";
char *fn_writelog       = "writelog";
char *fn_talklog        = "talklog";
char *fn_overrides      = "overrides";
char *fn_reject         = "reject";
char *fn_notes          = "notes";
char *fn_water          = "water";
char *fn_mandex         = "/.Names";

/* message */

char *msg_wawa          = MSG_WAWA;

char *msg_seperator     = MSG_SEPERATOR;
char *msg_mailer        = MSG_MAILER;
char *msg_shortulist    = MSG_SHORTULIST;

char *msg_cancel        = MSG_CANCEL;
char *msg_usr_left      = MSG_USR_LEFT;
char *msg_nobody        = MSG_NOBODY;

char *msg_sure_ny       = MSG_SURE_NY;
char *msg_sure_yn       = MSG_SURE_YN;

char *msg_bid           = MSG_BID;
char *msg_uid           = MSG_UID;

char *msg_del_ok        = MSG_DEL_OK;
char *msg_del_ny        = MSG_DEL_NY;

char *msg_fwd_ok        = MSG_FWD_OK;
char *msg_fwd_err1      = MSG_FWD_ERR1;
char *msg_fwd_err2      = MSG_FWD_ERR2;

char *err_board_update  = ERR_BOARD_UPDATE;
char *err_bid           = ERR_BID;
char *err_uid           = ERR_UID;
char *err_filename      = ERR_FILENAME;

char *str_mail_address  = "." BBSUSER "@" MYHOSTNAME;
char *str_new           = "new";
char *str_reply         = "Re: ";
char *str_space         = " \t\n\r";
char *str_sysop         = "SYSOP";
char *str_author1       = STR_AUTHOR1;
char *str_author2       = STR_AUTHOR2;
char *str_post1         = STR_POST1;
char *str_post2         = STR_POST2;
char *BoardName         = BOARDNAME;
char *str_ansicode      = "[0123456789;,";

#else                           /* _MAIN_C_ */


/* ----------------------------------------------------- */
/* GLOBAL VARIABLE                                       */
/* ----------------------------------------------------- */


extern int mbbsd;
extern int usernum;
extern pid_t currpid;
extern usint currstat;
extern int currmode;
extern int curredit;
extern int showansi;
extern int talkrequest;
extern time_t login_start_time;

extern userec cuser;            /* current user structure */
extern userec xuser;            /* lookup user structure */

extern char quote_file[MAXPATHLEN];
extern time_t paste_time;
extern char paste_title[TTLEN + 1];
extern char paste_path[MAXPATHLEN];
extern int  paste_level;
extern char quote_user[80];
extern char currowner[IDLEN + 2];
extern char currauthor[IDLEN + 2];
extern uschar currfmode;               /* current file mode */
extern char currtitle[TTLEN + 1];
extern char vetitle[TTLEN + 1];
extern char currfile[FNLEN];
extern char currmsg[100];

extern char currboard[];        /* name of currently selected board */
extern char currBM[];           /* BM of currently selected board */
extern char reset_color[];

extern boardheader *getbcache(char *); /* get SHM boardheader cache */

/* global string variable */

/* filename */

extern char *fn_passwd;
extern char *fn_board;
extern char *fn_note_ans;
extern char *fn_register;
extern char *fn_plans;
extern char *fn_writelog;
extern char *fn_talklog;
extern char *fn_overrides;
extern char *fn_reject;
extern char *fn_notes;
extern char *fn_water;
extern char *fn_mandex;

/* message */

extern char *msg_wawa;

extern char *msg_seperator;
extern char *msg_mailer;
extern char *msg_shortulist;

extern char *msg_cancel;
extern char *msg_usr_left;
extern char *msg_nobody;

extern char *msg_sure_ny;
extern char *msg_sure_yn;

extern char *msg_bid;
extern char *msg_uid;

extern char *msg_del_ok;
extern char *msg_del_ny;

extern char *msg_fwd_ok;
extern char *msg_fwd_err1;
extern char *msg_fwd_err2;

extern char *err_board_update;
extern char *err_bid;
extern char *err_uid;
extern char *err_filename;

extern char *str_mail_address;
extern char *str_new;
extern char *str_reply;
extern char *str_space;
extern char *str_sysop;
extern char *str_author1;
extern char *str_author2;
extern char *str_post1;
extern char *str_post2;
extern char *str_ansicode;
extern char *BoardName;


#ifdef XINU
extern int errno;

#endif

#endif                          /* _MAIN_C_ */


extern int errno;
extern jmp_buf byebye;          /* for exception condition like I/O error */

extern user_info *currutmp;

extern int dumb_term;
extern int t_lines, t_columns;  /* Screen size / width */
extern int b_lines;             /* Screen bottom line number: t_lines-1 */
extern int p_lines;             /* a Page of Screen line numbers: tlines-4 */

extern char fromhost[];
extern char save_title[];       /* used by editor when inserting */

extern int KEY_ESC_arg;

#endif                          /* _GLOBAL_H_ */
