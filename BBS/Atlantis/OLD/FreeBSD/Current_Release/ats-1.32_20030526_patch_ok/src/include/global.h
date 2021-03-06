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

/* 檔名設定 */

#define FN_PASSWD       ".PASSWDS"      /* User records */
#define FN_BOARD        ".BOARDS"       /* board list */
#define FN_USIES        "usies"         /* BBS log */

#define DEFAULT_BOARD   str_sysop
#define NUMPERMS        32

void my_outmsg(char *);
int HAS_PERM(usint);

/* 鍵盤設定 */

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
/* 訊息字串：獨立出來，以利支援各種語言                  */
/* ----------------------------------------------------- */

#ifndef USE_SQ_CURSOR
/* 這些定義只是照舊的系統 想改請隨意 如使用循序游標 這邊無作用 */
#ifdef TRANS_FROM_SOB
#define STR_CURSOR      "●"
#endif
#ifdef TRANS_FROM_FB3
#define STR_CURSOR      "◆"
#endif
#ifdef TRANS_FROM_COLA
#define STR_CURSOR      "◆"
#endif
#ifdef NEW_STATION
#define STR_CURSOR      "> "
#endif
#ifdef OLD_ATSVERSION
#define STR_CURSOR      "> "
#endif
/* 到這裡 */
#endif

#define STR_UNCUR       "  "

#ifndef TRANS_FROM_FB3
#define STR_AUTHOR1     "作者:"
#define STR_AUTHOR2     "發信人:"
#define STR_POST1       "看板:"
#define STR_POST1C      "領域:"
#define STR_POST2       "站內:"
#else
#define STR_AUTHOR1     "作者:"
#define STR_AUTHOR2     "發信人:"
#define STR_POST1       "信區:"
#define STR_POST1C      "發信站:"
#define STR_POST2       "站內:"
#endif

#define CHICKENTALK     0
#define LEN_AUTHOR1     5
#define LEN_AUTHOR2     7

#define STR_GUEST       "guest"


#define MSG_SEPERATOR   "\
───────────────────────────────────────"

#define MSG_CLOAKED     "哈哈！我隱形了！看不到勒... :P"
#define MSG_UNCLOAK     "我要重現江湖了...."

#define MSG_WORKING     "處理中，請稍候..."

#define MSG_WAWA        "嗚哇哇哇哇啊啊啊啊啊∼∼∼!!!  ~~~>_<~~~"

#define MSG_CANCEL      "取消。"
#define MSG_USR_LEFT    "User 已經離開了"
#define MSG_NOBODY      "目前無人上線"

#define MSG_DEL_OK      "刪除完畢"
#define MSG_DEL_CANCEL  "取消刪除"
#define MSG_DEL_ERROR   "刪除錯誤"
#define MSG_DEL_NY      "請確定刪除(Y/N)?[N] "

#define MSG_FWD_OK      "文章轉寄完成!"
#define MSG_FWD_ERR1    "轉寄失誤: system error"
#define MSG_FWD_ERR2    "轉寄失誤: address error"

#define MSG_SURE_NY     "請您確定(Y/N)？[N] "
#define MSG_SURE_YN     "請您確定(Y/N)？[Y] "

#define MSG_BID         "請輸入看板名稱："
#define MSG_UID         "請輸入使用者代號："
#define MSG_PASSWD      "請輸入您的密碼: "

#define MSG_BIG_BOY     "無限的可能  人類"
#define MSG_BIG_GIRL    "多變的特性  獸族"
#define MSG_LITTLE_BOY  "自然的鼓動  妖精"
#define MSG_LITTLE_GIRL "大地的朋友  地靈"
#define MSG_MAN         "謎樣的世界  ????"
#define MSG_WOMAN       "古老的傳說  龍族"
#define MSG_PLANT       "法治的象徵  神族"
#define MSG_MIME        "混沌的選擇  魔族"

#define ERR_BOARD_OPEN  ".BOARD 開啟錯誤"
#define ERR_BOARD_UPDATE        ".BOARD 更新有誤"
#define ERR_PASSWD_OPEN ".PASSWDS 開啟錯誤"

#define ERR_BID         "你搞錯了啦！沒有這個板喔！"
#define ERR_UID         "這裡沒有這個人啦！"
#define ERR_PASSWD      "密碼不對喔！你有沒有冒用人家的名字啊？"
#define ERR_FILENAME    "檔名不合法！"


#define MSG_SELECT_BOARD        \
        "[7m【 選擇看板 】[0m\n請輸入看板名稱(按空白鍵自動搜尋)："

#ifdef USE_MULTI_TITLE
#define MSG_POSTER      \
" 文章選讀 [31;47m (y)[30m回信 [31m(=[]<>)[30m相關主題 [31m(/?)[30m搜尋標題 [31m(aA)[30m搜尋作者 [31m(x)[30m轉錄 [31m(V)[30m投票   [0m"
#else
#define MSG_POSTER      \
"[34;46m 文章選讀 [31;47m (y)[30m回信 [31m(=[]<>)[30m相關主題 [31m(/?)[30m搜尋標題 [31m(aA)[30m搜尋作者 [31m(x)[30m轉錄 [31m(V)[30m投票   [0m"
#endif

#ifdef USE_MULTI_TITLE
#define MSG_MAILER      \
" 鴻雁往返  [31;47m  (R)[30m回信  [31m(x)[30m轉達  [31m(y)[30m群組回信  [31m(D)[30m刪除  [31m[G][30m繼續 ?                 [0m"
#else
#define MSG_MAILER      \
"[34;46m  鴻雁往返  [31;47m  (R)[30m回信  [31m(x)[30m轉達  [31m(y)[30m群組回信  [31m(D)[30m刪除  [31m[G][30m繼續 ?                [0m"
#endif

#define MSG_SHORTULIST  "[7m\
使用者代號    目前狀態   │使用者代號    目前狀態   │使用者代號    目前狀態   [0m"

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
