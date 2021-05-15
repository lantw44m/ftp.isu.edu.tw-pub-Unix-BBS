/*-------------------------------------------------------*/
/* global.h     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : global definitions & variables               */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

/* ----------------------------------------------------- */
/* GLOBAL DEFINITION                                     */
/* ----------------------------------------------------- */


/* �ɦW�]�w */

#define FN_PASSWD       ".PASSWDS"      /* User records */
#define FN_BOARD        ".BOARDS"       /* board list */
#define FN_RPG		".RPGS"		/* RPG records */
#define FN_USIES        "usies"         /* BBS log */
#define FN_USSONG       "ussong"        /* �I�q */
#define FN_VOTE		".VCH"		/* �벼 */
#define FN_LIST		".LIST"		/* �W�� */
#define FN_PAL		"pal"
#define FN_ALOHA	"aloha"

#define FN_TOPSONG      "etc/topsong"
#define FN_TOPRPG       "etc/toprpg"
#define FN_GAMEMONEY    "game/money"
#define FN_MONEYLOG     "etc/moneystat"
#define FN_OVERRIDES    "overrides"	/* �n�ͦW�� */
#define FN_REJECT       "reject"	/* �a�H�W�� */
#define FN_CANVOTE      "can_vote"	/* �i�H�벼�W�� */
#define FN_WATER        "water"		/* ����W�� */
#define FN_APPLICATION  "application"
//#define FN_VISABLE      "visable"
#define FN_MYFAVORITE   "favorite"
#define FN_ALOHAED	"alohaed"	/* �W���q�� */
#define FN_POSTLIST	"postlist"	/* �s�峹�q�� */

#define FN_TICKET_RECORD "game/ticket.data"
#define FN_TICKET_USER   "game/ticket.user"
#define FN_TICKET        "game/ticket.result"

#define DEFAULT_BOARD   str_sysop

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

char *my_ctime();
char *subject();
void pressanykey();
void showtitle();
void showplans();
void user_display();
void log_usies();
void talkreply();
void uinfo_query();
void touch_boards();
void namecomplete();
void usercomplete();
void stampfile();

#define TRACE   log_usies

/* #define      setutmpmode(x)  (currutmp->mode = (x)) */


/* ----------------------------------------------------- */
/* �T���r��G�W�ߥX�ӡA�H�Q�䴩�U�ػy��                  */
/* ----------------------------------------------------- */

#define STR_CURSOR      "�p"
#define STR_UNCUR       "  "

#define STR_AUTHOR1     "�@��:"
#define STR_AUTHOR2     "�o�H�H:"
#define STR_POST1       "�ݪO:"
#define STR_POST2       "����:"

#define LEN_AUTHOR1     5
#define LEN_AUTHOR2     7

#define STR_GUEST       "guest"

#define MSG_SEPERATOR   "\
�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w"

#define MSG_CLOAKED     "�K�K,���_���o!"
#define MSG_UNCLOAK     "���{����F...."
#define MSG_SEECLOAK    "��o�F�z����A�U�����e����!"

#define MSG_WORKING     "�B�z���A�еy��..."

#define MSG_CANCEL      "�����C"
#define MSG_USR_LEFT    "User �w�g���}�F"
#define MSG_NOBODY      "�ثe�L�H�W�u"
#define MSG_MY_FAVORITE "�ڪ��̷R�ݪO"

#define MSG_DEL_OK      "�R������"
#define MSG_DEL_CANCEL  "�����R��"
#define MSG_DEL_ERROR   "�R�����~"
#define MSG_DEL_NY      "�нT�w�R��(Y/N)?[N] "

#define MSG_FWD_OK      "�峹��H����!"
#define MSG_FWD_ERR1    "��H���~: �t�εo�Ϳ��~"
#define MSG_FWD_ERR2    "��H���~: �a�}���~�A�d�L���H"

#define MSG_SURE_NY     "�бz�T�w(Y/N)�H[N] "
#define MSG_SURE_YN     "�бz�T�w(Y/N)�H[Y] "

#define MSG_BID         "�п�J�ݪO�W�١G"
#define MSG_UID         "�п�J�ϥΪ̥N���G"
#define MSG_PASSWD      "�п�J�z���K�X: "

#define MSG_BIG_BOY     "�k��"
#define MSG_BIG_GIRL    "�k��"
#define MSG_LITTLE_BOY  "�p��"
#define MSG_LITTLE_GIRL "���f"
#define MSG_MAN         "�j��"
#define MSG_WOMAN       "�j�j"
#define MSG_PLANT       "�Ӫ��H"
#define MSG_MIME        "�����H"

#define MSG_NORMAL      0   /* �@����y   */
#define MSG_ALOHA       1   /* �W�U���q�� */
#define MSG_BROADCAST   2   /* �s�����y   */

#define MSG_ALOHA_LOGIN "<�W���q��>"
#define MSG_ALOHA_LOGOUT "<�U���q��>"

#define ERR_BOARD_OPEN		".BOARD �}�ҿ��~"
#define ERR_BOARD_UPDATE        ".BOARD ��s���~"
#define ERR_PASSWD_OPEN		".PASSWDS �}�ҿ��~"

#define ERR_BID         "�A�d���F�աI�S���o�ӪO��I"
#define ERR_UID         "�o�̨S���o�ӤH�աI"
#define ERR_PASSWD      "�K�X�����I�A���S���_�ΤH�a���W�r�ڡH"
#define ERR_FILENAME    "�ɦW���X�k�I"

#define MSG_MAILER      \
COLOR2"  �ڪ��H�c  "\
COLOR1"[1m [33m(r)[37mŪ�H [33m(f)[37m��H [33m(y)[37m�s�զ^�H [33m(d)[37m�R�� [33m(c)[37m�ƻs�峹  [44m    [0m"

#define MSG_SHORTULIST  \
"[7m�ϥΪ̥N��    �ثe���A   �x�ϥΪ̥N��    �ثe���A   �x�ϥΪ̥N��    �ثe���A  [0m"

#define P_BOARD "�藍�_,���O�u��ݪO�n�Ͷi�J,�ЦV�O�D�ӽФJ�ҳ\\�i"


#ifdef  _MAIN_C_

/* ----------------------------------------------------- */
/* GLOBAL VARIABLE                                       */
/* ----------------------------------------------------- */

char trans_buffer[256];         /* �@��ǻ��ܼ� add by Ptt */

int usernum;
pid_t currpid;                  /* current process ID */
usint currstat;
int currmode = 0;
int curredit = 0;
int showansi = 1;
time_t login_start_time;
time_t update_time;
//time_t schedule_time;
//char schedule_string[100];
userec cuser;                   /* current user structure */
userec xuser;                   /* lookup user structure */
rpgrec rpguser;
rpgrec rpgtmp;
char quote_file[80] = "\0";
char quote_user[80] = "\0";
time_t paste_time;
char paste_title[STRLEN];
char paste_path[256];
int  paste_level;
char currtitle[TTLEN + 1] = "\0";
//char vetitle[40] = "\0";
char currowner[STRLEN] = "\0";
char currauthor[IDLEN + 2] = "\0";
char currfile[FNLEN];           /* current file name @ bbs.c mail.c */
uschar currfmode;               /* current file mode */
char currmsg[80];
char currboard[IDLEN + 2];
usint currbrdattr;
char currBM[IDLEN * 3 + 10];
char reset_color[4] = "[m";
//char margs[64] = "\0";          
char watermode = 0;
int inmore = 0;
//char cuser_cursor[16]="���y";
/* global string variables */


/* filename */

char *fn_passwd         = FN_PASSWD;
char *fn_board          = FN_BOARD;
char *fn_rpg		= FN_RPG;
char *fn_note_ans       = "note.ans";
char *fn_register       = "register.new";
char *fn_plans          = "plans";
char *fn_writelog       = "writelog";
char *fn_talklog        = "talklog";
char *fn_overrides      = FN_OVERRIDES;
char *fn_reject         = FN_REJECT;
char *fn_canvote        = FN_CANVOTE;
char *fn_notes          = "notes";
char *fn_water          = FN_WATER;
char *fn_myfavorite     = FN_MYFAVORITE;
char *fn_digest         = "/.DIG";
char *fn_deleted        = "/.DEL";

char *fn_alohaed	= FN_ALOHAED;
char *fn_postlist	= FN_POSTLIST;
char *fn_RejectMail     = "RejectMail";
char *fn_BdRejectMail   = "bdrejectmail";

/* message */

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
char *str_dotdir        = ".DIR";

#else                           /* _MAIN_C_ */


/* ----------------------------------------------------- */
/* GLOBAL VARIABLE                                       */
/* ----------------------------------------------------- */

extern char trans_buffer[256];         /* �@��ǻ��ܼ� add by Ptt */

extern int usernum;
extern pid_t currpid;
extern usint currstat;
extern int currmode;
extern int curredit;
extern int showansi;
extern int talkrequest;
extern time_t login_start_time;
extern time_t update_time;
//extern time_t schedule_time;
//extern char schedule_string[100];
extern userec cuser;            /* current user structure */
extern userec xuser;            /* lookup user structure */
extern rpgrec rpguser;
extern rpgrec rpgtmp;

extern char quote_file[80];
extern char quote_user[80];
extern time_t paste_time;
extern char paste_title[STRLEN];
extern char paste_path[256];
extern int  paste_level;
extern char currowner[STRLEN];
extern char currauthor[IDLEN + 2];
extern uschar currfmode;               /* current file mode */
extern char currtitle[TTLEN + 1];
//extern char vetitle[44];
extern char currfile[FNLEN];
extern char currmsg[80];

extern char currboard[];        /* name of currently selected board */
extern usint currbrdattr;
extern char currBM[];           /* BM of currently selected board */
extern char reset_color[];
//extern char margs[];
extern char watermode;
extern int inmore;
//extern char cuser_cursor[];
/* global string variable */

/* filename */

extern char *fn_passwd;
extern char *fn_board;
extern char *fn_rpg;
extern char *fn_note_ans;
extern char *fn_register;
extern char *fn_plans;
extern char *fn_writelog;
extern char *fn_talklog;
extern char *fn_overrides;
extern char *fn_reject;
extern char *fn_canvote;
extern char *fn_notes;
extern char *fn_water;
extern char *fn_visable;
extern char *fn_myfavorite;
extern char *fn_digest;
extern char *fn_deleted;

extern char *fn_RejectMail;
extern char *fn_BdRejectMail;

/* message */

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
extern char *BoardName;
extern char *str_dotdit;

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


void doent();   /* read,mail,announce list */

#define RACE_NORACE     "�L�~�C��"
#define RACE_POST       "����j�H"
#define RACE_READ       "Ū��֤�"
#define RACE_IDLE       "�`�n�{��"
#define RACE_CHAT       "��Ѳr�N"
#define RACE_MSG        "���y�a�x"
#define RACE_GAME       "�C���g�P"

#endif                          /* _GLOBAL_H_ */
