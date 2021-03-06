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
  , "NNew Board     開闢新看板      ", 1,
  "bin/menager.so:m_board",
  PERM_CHATROOM
#ifdef USE_BOARDS_MANAGER
            | PERM_EXTRA1
#endif
  , "BBoard         設定看板        ", 1,
  "bin/menager.so:m_user",
  PERM_CHATROOM
#ifdef USE_ACCOUNTS_PERM
           | PERM_ACCOUNTS
#endif
#ifdef USE_BOARDS_MANAGER
          | PERM_EXTRA1
#endif
  , "UUser          使用者資料      ", 1,

  "bin/m_st.so:x_update",
  PERM_CHATROOM, "GGroup         修改看板群組分類", 1,
  "bin/st_name.so:c_st_name",
  PERM_CHATROOM,"SStation       更改中文站名    ", 1,
  "bin/e_sys_f.so:e_all",
  PERM_CHATROOM,    "FFiles_ALL     編輯站內各項檔案", 1,
  "bin/register.so:m_register",
  PERM_SYSOP,  "RRegister      審核註冊表單    ", 1,
  "bin/add_money.so:Add_MonS",
  PERM_SYSOP,    "AAddMoney      自動發薪程式    ", 1,

#ifndef NO_USE_MULTI_STATION
  "bin/m_st.so:m_station",
  PERM_SYSOP,   "MMANAGER       站台管理        ", 1,
#endif

#ifdef  HAVE_MAILCLEAN
  m_mclean, PERM_SYSOP,    "MMail Clean    清理使用者信箱  ", 0,
#endif

#ifdef  HAVE_ADM_SHELL
  x_csh, PERM_SYSOP,       "SShell         執行系統 Shell  ", 0,
#endif

#ifdef  HAVE_REPORT
  m_trace, PERM_SYSOP,     "TTrace         設定記錄除錯資訊", 0,
#endif

NULL, 0, NULL, 0 };

/* ----------------------------------------------------- */
/* class menu                                            */
/* ----------------------------------------------------- */

/* Dopin: 移至 all_stations.h 中 各分站不同 */

/* ----------------------------------------------------- */
/* mail menu                                             */
/* ----------------------------------------------------- */

int m_new(), m_read(), m_send(), m_list(), mail_list(), m_sysop(),
    mail_all(), mail_mbox(), m_group();

#ifdef INTERNET_PRIVATE_EMAIL
int m_internet();
#endif

static struct commands maillist[] = {
  m_new, PERM_READMAIL,    "RNew           閱\讀新進郵件    ", 0,
  m_read, PERM_READMAIL,   "RRead          多功\能讀信選單  ", 0,
  m_send, PERM_BASIC,      "SSend          站內寄信        ", 0,
  "bin/m_list.so:m_list",
  PERM_LOGINOK,            "LList          編輯群組名單    ", 1,
  mail_list, PERM_LOGINOK, "MMail List     群組寄信        ", 0,
  m_sysop, 0,              "YYes, sir!     諂媚站長        ", 1,

#ifdef INTERNET_PRIVATE_EMAIL
  m_internet, PERM_INTERNET, "RInternet      寄信到網際網路  ", 0,
  "bin/m_group.so:m_group",
  PERM_SYSOP,       "GGroup         群組網路寄信功\能", 1,
#endif
  "bin/zip_dir.so:mail_mbox",
  PERM_INTERNET,   "ZZip UserHome  打包私人資料    ", 1,
  "bin/m_all.so:mail_all",
  PERM_SYSOP,      "AAll           寄信給所有使用者", 1,

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

  t_users, 0,               "UUsers         冒險者一覽      ", 0,
  t_monitor, PERM_BASIC,    "MMonitor       冒險者動態      ", 0,
  t_override, PERM_LOGINOK, "OOverRide      編輯好友名單    ", 0,
  t_reject, PERM_LOGINOK,   "BBlack         編輯壞人名單    ", 0,
  t_pager, PERM_BASIC,      "PPager         切換呼叫器      ", 0,
#ifdef TAKE_IDLE
  t_idle, 0,                "IIdle          發呆休息        ", 0,
#endif
  t_query, 0,               "QQuery         查詢冒險者      ", 0,
  t_talk, PERM_PAGE,        "TTalk          冒險者的對話    ", 0,
  t_chat, PERM_LOGINOK,     "CChat          冒險者的聚會所  ", 0,
  t_display, 0,             "DDisplay       顯示上幾次熱訊  ", 0,

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
  u_kill, PERM_BASIC, "IKill          投海自盡", 0,
#endif

  "bin/user_files.so:u_editfiles",
  PERM_LOGINOK, "PPersonal      設定[1;37;42m個人[1;37;46m相關檔案[m", 1,
  b_favor, PERM_BASIC,       "BBoardFavor    設定看板[1;31;40m我的最愛[m", 0,
  u_ansi, 0,                 "AANSI          切換[36m彩[35m色[37m[30;47m黑[1;37m白[40;0m模示", 0,
  u_movie, 0,                "MMovie         切換[0;33;40m動畫模式[m    ", 0,
  u_cloak, PERM_CLOAK,       "CCloak         [0;30;47m隱身術[m          ", 0,
  u_lock, PERM_BASIC,        "LLockScreen    [1;37;45m鎖定螢幕[m        ", 0,
  "bin/user_files.so:u_register",
  PERM_BASIC,    "RRegister      填寫[1;37;43m註冊申請單[m  ", 1,

#ifdef QUERY_REMOTEUSER
  "bin/query_rmuser.so:query_rmuser",
  PERM_LOGINOK,"QQueryRmtUser  查詢[1;37;44m上線記錄[m    ", 1,
#endif

#ifdef  EMAIL_JUSTIFY
  go_justify, PERM_BASIC,    "SSendRegister  重新[1;37;46m寄發認證信函[m", 0,
#endif

#ifdef USE_ULIST
  "bin/ulist.so:u_list",
  PERM_SYSOP,        "UUsers         [1;37;41m列出註冊名單[m    ", 1,
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
  0,            "PProgram 《[5;1;37;47m  版本與版權宣告  [m》", 1,
#endif

#ifdef RPG_FIGHT
  t_fight, PERM_LOGINOK,   "FFight   《[1;37;42m  對戰程式陽春版  [m》", 0,
#endif

#ifdef DRAW
  show_me, PERM_LOGINOK,   "DDraw    《[1;37;46m  抽獎程式體驗版  [m》", 0,
#endif

#ifdef OSONG_SEARCH
  "bin/record_list.so:find_song",
  PERM_LOGINOK,
  #ifdef SONGCOST
  /* Dopin: 此選項列請自行調整格式 並依實際的點歌花費 在此以 10 元為例 */
                           "CChSong  《[1;37;44m  點歌程式 ($10)  [m》", 1,
  #else
                           "CChSong  《[1;37;44m  點歌程式釋出版  [m》", 1,
  #endif
#endif

#ifdef DREYE
  "bin/dreye.so:main_dreye",
  PERM_LOGINOK, "DDreye   《[1;37;41m  譯典通線上字典  [m》", 1,
#endif

#ifdef STAR_CHICKEN
  "bin/pip_3_ats.so:p_pipple",
  PERM_LOGINOK,  "FFChicken《[1;37;45m 星空戰鬥雞釋出版 [m》", 1,
#endif

#ifdef AGREE_SYS
  "bin/agree.so:main_agree",
  PERM_LOGINOK,  "AAgreeSys《[1;37;43m 自動連署系統程式 [m》", 1,
#endif

#ifdef GOOD_BABY
  "bin/goodbaby.so:reject_me",
  PERM_LOGINOK,  "BBanMe   《[1;37;46m  乖寶寶自我約束  [m》", 1,
#endif

#ifdef  EMAIL_JUSTIFY
 "bin/check_mkey.so:check_magic_key",
  PERM_BASIC,    "MMagicKey《[1;37;42m 填寫認證信編碼號 [m》", 1,
#endif

NULL, 0, NULL, 0 };

/* ----------------------------------------------------- */
/* main menu                                             */
/* ----------------------------------------------------- */

static int admin() {
  cuser.extra_mode[8] = 1;
  domenu(ADMIN, "系統維護", 'X', adminlist, 0);
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
  domenu(MAIL, "電子郵件", 'R', maillist, 0);
  return 0;
}

static int Talk() {
  domenu(TMENU, "聊天說話", 'U', talklist, 0);
  return 0;
}

static int User() {
  domenu(UMENU, "個人設定", 'A', userlist, 0);
  return 0;
}

static int Xyz() {
  domenu(XMENU, "特色專區", 'N', xyzlist, 0);
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
  ,                       "00Admin       【 系統維護區 】 ", 0,

#ifdef HAVE_TIN
  x_tin, PERM_LOGINOK,    "11Discuss     【 News討論區 】 ", 0,
#endif

#ifdef HAVE_GOPHER
  x_gopher, PERM_LOGINOK, "22Gopher      【地鼠查詢系統】 ", 0,
#endif

  Announce, 0,            "AAnnounce     【 精華公佈欄 】 ", 0,
  Boards1, 0,             "BBoards       【 所有討論區 】 ", 0,
  Class, 0,               "CClass        【 分組討論區 】 ", 0,
  Mail,  PERM_BASIC,      "MMail         【 私人信件區 】 ", 0,
  Talk, 0,                "TTalk         【 休閒聊天區 】 ", 0,
  User, 0,                "UUser         【 個人設定區 】 ", 0,
  Xyz, 0,                 "XXyz          【 亞站功\能區 】 ", 0,

#ifndef NO_USE_MULTI_STATION
  "bin/station_set.so:a_change",
  0,            "OOtherBBS     【  切換站台  】 ", 1,
#endif

  "bin/goodbye.so:Goodbye",
  0,             "GGoodbye      【  登出離開  】 ", 1,

NULL, 0, NULL, 0 };
/* INDENT ON */
