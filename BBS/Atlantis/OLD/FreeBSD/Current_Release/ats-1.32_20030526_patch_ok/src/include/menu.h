static int u_movie() {
  cuser.uflag ^= MOVIE_FLAG;
  return 0;
}

/* ----------------------------------------------------- */
/* administrator's maintain menu                         */
/* ----------------------------------------------------- */

int m_user(), m_newbrd(), m_board(), m_register(), x_file();

int x_update();
#ifndef NO_USE_MULTI_STATION
int m_station();
#endif

int e_all(), c_st_name(), Add_MonS();

#ifdef  HAVE_ADM_SHELL
int x_csh();
#endif

#ifdef  HAVE_MAILCLEAN
int m_mclean();
#endif

#ifdef  HAVE_REPORT
int m_trace();
#endif

#ifdef SYS_VOTE
int m_vote();
#endif

static struct commands adminlist[] = {

  "bin/menager.so:m_newbrd",
  PERM_CHATROOM
#ifdef USE_BOARDS_MANAGER
            | PERM_EXTRA1
#endif
  , "NNew Board     ¶}ÅP·s¬ÝªO      ", 1,
  "bin/menager.so:m_board",
  PERM_CHATROOM
#ifdef USE_BOARDS_MANAGER
            | PERM_EXTRA1
#endif
  , "BBoard         ³]©w¬ÝªO        ", 1,
  "bin/menager.so:m_user",
  PERM_CHATROOM
#ifdef USE_ACCOUNTS_PERM
           | PERM_ACCOUNTS
#endif
#ifdef USE_BOARDS_MANAGER
          | PERM_EXTRA1
#endif
  , "UUser          ¨Ï¥ÎªÌ¸ê®Æ      ", 1,

  "bin/m_st.so:x_update",
  PERM_CHATROOM, "GGroup         ­×§ï¬ÝªO¸s²Õ¤ÀÃþ", 1,
  "bin/st_name.so:c_st_name",
  PERM_CHATROOM,"SStation       §ó§ï¤¤¤å¯¸¦W    ", 1,
  "bin/e_sys_f.so:e_all",
  PERM_CHATROOM,    "FFiles_ALL     ½s¿è¯¸¤º¦U¶µÀÉ®×", 1,
  "bin/register.so:m_register",
  PERM_SYSOP,  "RRegister      ¼f®Öµù¥Uªí³æ    ", 1,
  "bin/add_money.so:Add_MonS",
  PERM_SYSOP,    "AAddMoney      ¦Û°ÊµoÁ~µ{¦¡    ", 1,

#ifndef NO_USE_MULTI_STATION
  "bin/m_st.so:m_station",
  PERM_SYSOP,   "MMANAGER       ¯¸¥xºÞ²z        ", 1,
#endif

#ifdef  HAVE_MAILCLEAN
  m_mclean, PERM_SYSOP,    "MMail Clean    ²M²z¨Ï¥ÎªÌ«H½c  ", 0,
#endif

#ifdef  HAVE_ADM_SHELL
  x_csh, PERM_SYSOP,       "SShell         °õ¦æ¨t²Î Shell  ", 0,
#endif

#ifdef  HAVE_REPORT
  m_trace, PERM_SYSOP,     "TTrace         ³]©w°O¿ý°£¿ù¸ê°T", 0,
#endif

NULL, 0, NULL, 0 };

/* ----------------------------------------------------- */
/* class menu                                            */
/* ----------------------------------------------------- */

/* Dopin: ²¾¦Ü all_stations.h ¤¤ ¦U¤À¯¸¤£¦P */

/* ----------------------------------------------------- */
/* mail menu                                             */
/* ----------------------------------------------------- */

int m_new(), m_read(), m_send(), m_list(), mail_list(), m_sysop(),
    mail_all(), mail_mbox(), m_group();

#ifdef INTERNET_PRIVATE_EMAIL
int m_internet();
#endif

static struct commands maillist[] = {
  m_new, PERM_READMAIL,    "RNew           ¾\\Åª·s¶i¶l¥ó    ", 0,
  m_read, PERM_READMAIL,   "RRead          ¦h¥\\¯àÅª«H¿ï³æ  ", 0,
  m_send, PERM_BASIC,      "SSend          ¯¸¤º±H«H        ", 0,
  "bin/m_list.so:m_list",
  PERM_LOGINOK,            "LList          ½s¿è¸s²Õ¦W³æ    ", 1,
  mail_list, PERM_LOGINOK, "MMail List     ¸s²Õ±H«H        ", 0,
  m_sysop, 0,              "YYes, sir!     ½Ô´A¯¸ªø        ", 1,

#ifdef INTERNET_PRIVATE_EMAIL
  m_internet, PERM_INTERNET, "RInternet      ±H«H¨ìºô»Úºô¸ô  ", 0,
  "bin/m_group.so:m_group",
  PERM_SYSOP,       "GGroup         ¸s²Õºô¸ô±H«H¥\\¯à", 1,
#endif
  "bin/zip_dir.so:mail_mbox",
  PERM_INTERNET,   "ZZip UserHome  ¥´¥]¨p¤H¸ê®Æ    ", 1,
  "bin/m_all.so:mail_all",
  PERM_SYSOP,      "AAll           ±H«Hµ¹©Ò¦³¨Ï¥ÎªÌ", 1,

NULL, 0, NULL, 0 };

/* ----------------------------------------------------- */
/* Talk menu                                             */
/* ----------------------------------------------------- */

int t_users(), t_list(), t_idle(), t_query(), t_monitor();
int t_pager(), t_talk(), t_chat(), t_override(), t_reject();
int t_display();

#ifdef HAVE_IRC
int x_irc();
#endif

static struct commands talklist[] = {

  t_users, 0,               "UUsers         «_ÀIªÌ¤@Äý      ", 0,
  t_monitor, PERM_BASIC,    "MMonitor       «_ÀIªÌ°ÊºA      ", 0,
  t_override, PERM_LOGINOK, "OOverRide      ½s¿è¦n¤Í¦W³æ    ", 0,
  t_reject, PERM_LOGINOK,   "BBlack         ½s¿èÃa¤H¦W³æ    ", 0,
  t_pager, PERM_BASIC,      "PPager         ¤Á´«©I¥s¾¹      ", 0,
#ifdef TAKE_IDLE
  t_idle, 0,                "IIdle          µo§b¥ð®§        ", 0,
#endif
  t_query, 0,               "QQuery         ¬d¸ß«_ÀIªÌ      ", 0,
  t_talk, PERM_PAGE,        "TTalk          «_ÀIªÌªº¹ï¸Ü    ", 0,
  t_chat, PERM_LOGINOK,     "CChat          «_ÀIªÌªº»E·|©Ò  ", 0,
  t_display, 0,             "DDisplay       Åã¥Ü¤W´X¦¸¼ö°T  ", 0,

NULL, 0, NULL, 0 };

/* ----------------------------------------------------- */
/* User menu                                             */
/* ----------------------------------------------------- */

int u_register(), b_favor(), u_editfiles(), u_cloak(), u_ansi();
int u_lock(), u_list();

#ifdef  EMAIL_JUSTIFY
int go_justify();
#endif

#ifdef  HAVE_SUICIDE
int u_kill();
#endif

#ifdef QUERY_REMOTEUSER
void query_rmuser();
#endif

static struct commands userlist[] = {

#ifdef  HAVE_SUICIDE
  u_kill, PERM_BASIC, "IKill          §ë®ü¦ÛºÉ", 0,
#endif

  "bin/user_files.so:u_editfiles",
  PERM_LOGINOK, "PPersonal      ³]©w[1;37;42m­Ó¤H[1;37;46m¬ÛÃöÀÉ®×[m", 1,
  b_favor, PERM_BASIC,       "BBoardFavor    ³]©w¬ÝªO[1;31;40m§Úªº³Ì·R[m", 0,
  u_ansi, 0,                 "AANSI          ¤Á´«[36m±m[35m¦â[37m[30;47m¶Â[1;37m¥Õ[40;0m¼Ò¥Ü", 0,
  u_movie, 0,                "MMovie         ¤Á´«[0;33;40m°Êµe¼Ò¦¡[m    ", 0,
  u_cloak, PERM_CLOAK,       "CCloak         [0;30;47mÁô¨­³N[m          ", 0,
  u_lock, PERM_BASIC,        "LLockScreen    [1;37;45mÂê©w¿Ã¹õ[m        ", 0,
  "bin/user_files.so:u_register",
  PERM_BASIC,    "RRegister      ¶ñ¼g[1;37;43mµù¥U¥Ó½Ð³æ[m  ", 1,

#ifdef QUERY_REMOTEUSER
  "bin/query_rmuser.so:query_rmuser",
  PERM_LOGINOK,"QQueryRmtUser  ¬d¸ß[1;37;44m¤W½u°O¿ý[m    ", 1,
#endif

#ifdef  EMAIL_JUSTIFY
  go_justify, PERM_BASIC,    "SSendRegister  ­«·s[1;37;46m±Hµo»{ÃÒ«H¨ç[m", 0,
#endif

#ifdef USE_ULIST
  "bin/ulist.so:u_list",
  PERM_SYSOP,        "UUsers         [1;37;41m¦C¥Xµù¥U¦W³æ[m    ", 1,
#endif

  NULL, 0, NULL, 0 };

/* ----------------------------------------------------- */
/* XYZ tool menu                                         */
/* ----------------------------------------------------- */

#ifdef  HAVE_License
int x_gpl();
#endif

#ifdef HAVE_INFO
int x_program();
#endif

#ifdef SYS_VOTE
int x_vote(), x_results();
#endif

#ifdef RPG_FIGHT
int t_fight();
#endif

#ifdef DRAW
int show_me();
#endif

#ifdef ORDERSONG
int find_song();
#endif

#ifdef DREYE
int main_dreye();
#endif

#ifdef STAR_CHICKEN
int p_pipple();
#endif

#ifdef AGREE_SYS
int main_agree();
#endif

static struct commands xyzlist[] = {

#ifdef HAVE_INFO
  "bin/x_program.so:x_program",
  0,            "PProgram ¡m[5;1;37;47m  ª©¥»»Pª©Åv«Å§i  [m¡n", 1,
#endif

#ifdef RPG_FIGHT
  t_fight, PERM_LOGINOK,   "FFight   ¡m[1;37;42m  ¹ï¾Ôµ{¦¡¶§¬Kª©  [m¡n", 0,
#endif

#ifdef DRAW
  show_me, PERM_LOGINOK,   "DDraw    ¡m[1;37;46m  ©â¼úµ{¦¡ÅéÅçª©  [m¡n", 0,
#endif

#ifdef OSONG_SEARCH
  "bin/record_list.so:find_song",
  PERM_LOGINOK,
  #ifdef SONGCOST
  /* Dopin: ¦¹¿ï¶µ¦C½Ð¦Û¦æ½Õ¾ã®æ¦¡ ¨Ã¨Ì¹ê»ÚªºÂIºqªá¶O ¦b¦¹¥H 10 ¤¸¬°¨Ò */
                           "CChSong  ¡m[1;37;44m  ÂIºqµ{¦¡ ($10)  [m¡n", 1,
  #else
                           "CChSong  ¡m[1;37;44m  ÂIºqµ{¦¡ÄÀ¥Xª©  [m¡n", 1,
  #endif
#endif

#ifdef DREYE
  "bin/dreye.so:main_dreye",
  PERM_LOGINOK, "DDreye   ¡m[1;37;41m  Ä¶¨å³q½u¤W¦r¨å  [m¡n", 1,
#endif

#ifdef STAR_CHICKEN
  "bin/pip_3_ats.so:p_pipple",
  PERM_LOGINOK,  "FFChicken¡m[1;37;45m ¬PªÅ¾Ô°«ÂûÄÀ¥Xª© [m¡n", 1,
#endif

#ifdef AGREE_SYS
  "bin/agree.so:main_agree",
  PERM_LOGINOK,  "AAgreeSys¡m[1;37;43m ¦Û°Ê³s¸p¨t²Îµ{¦¡ [m¡n", 1,
#endif

#ifdef GOOD_BABY
  "bin/goodbaby.so:reject_me",
  PERM_LOGINOK,  "BBanMe   ¡m[1;37;46m  ¨ÄÄ_Ä_¦Û§Ú¬ù§ô  [m¡n", 1,
#endif

#ifdef  EMAIL_JUSTIFY
 "bin/check_mkey.so:check_magic_key",
  PERM_BASIC,    "MMagicKey¡m[1;37;42m ¶ñ¼g»{ÃÒ«H½s½X¸¹ [m¡n", 1,
#endif

NULL, 0, NULL, 0 };

/* ----------------------------------------------------- */
/* main menu                                             */
/* ----------------------------------------------------- */

static int admin() {
  cuser.extra_mode[8] = 1;
  domenu(ADMIN, "¨t²ÎºûÅ@", 'X', adminlist, 0);
  cuser.extra_mode[8] = 0;
  return 0;
}

int Favor() {
   cuser.extra_mode[0] = 1;
   load_boards(DEFAULTBOARD);
   Boards();
   cuser.extra_mode[0] = 0;
   load_boards(DEFAULTBOARD);

   return 0;
}

static int Mail() {
  domenu(MAIL, "¹q¤l¶l¥ó", 'R', maillist, 0);
  return 0;
}

static int Talk() {
  domenu(TMENU, "²á¤Ñ»¡¸Ü", 'U', talklist, 0);
  return 0;
}

static int User() {
  domenu(UMENU, "­Ó¤H³]©w", 'A', userlist, 0);
  return 0;
}

static int Xyz() {
  domenu(XMENU, "¯S¦â±M°Ï", 'N', xyzlist, 0);
  return 0;
}

int Announce(),  Goodbye();

#ifndef NO_USE_MULTI_STATION
int a_change();
#endif

#ifdef HAVE_TIN
int x_tin();
#endif

#ifdef HAVE_GOPHER
int x_gopher();
#endif

int Boards1();

struct commands cmdlist[] = {

  admin, PERM_CHATROOM
#ifdef USE_ACCOUNTS_PERM
         | PERM_ACCOUNTS
#endif
#ifdef USE_BOARDS_MANAGER
         | PERM_EXTRA1
#endif
  ,                       "00Admin       ¡i ¨t²ÎºûÅ@°Ï ¡j ", 0,

#ifdef HAVE_TIN
  x_tin, PERM_LOGINOK,    "11Discuss     ¡i News°Q½×°Ï ¡j ", 0,
#endif

#ifdef HAVE_GOPHER
  x_gopher, PERM_LOGINOK, "22Gopher      ¡i¦a¹«¬d¸ß¨t²Î¡j ", 0,
#endif

  Announce, 0,            "AAnnounce     ¡i ºëµØ¤½§GÄæ ¡j ", 0,
  Boards1, 0,             "BBoards       ¡i ©Ò¦³°Q½×°Ï ¡j ", 0,
  Class, 0,               "CClass        ¡i ¤À²Õ°Q½×°Ï ¡j ", 0,
  Mail,  PERM_BASIC,      "MMail         ¡i ¨p¤H«H¥ó°Ï ¡j ", 0,
  Talk, 0,                "TTalk         ¡i ¥ð¶¢²á¤Ñ°Ï ¡j ", 0,
  User, 0,                "UUser         ¡i ­Ó¤H³]©w°Ï ¡j ", 0,
  Xyz, 0,                 "XXyz          ¡i ¨È¯¸¥\\¯à°Ï ¡j ", 0,

#ifndef NO_USE_MULTI_STATION
  "bin/station_set.so:a_change",
  0,            "OOtherBBS     ¡i  ¤Á´«¯¸¥x  ¡j ", 1,
#endif

  "bin/goodbye.so:Goodbye",
  0,             "GGoodbye      ¡i  µn¥XÂ÷¶}  ¡j ", 1,

NULL, 0, NULL, 0 };
/* INDENT ON */
