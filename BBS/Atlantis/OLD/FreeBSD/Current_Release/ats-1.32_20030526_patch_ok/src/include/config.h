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
/* 定義 BBS 站名位址                                     */
/* ------------------------------------------------------*/

#define BOARDNAME       "亞特蘭提斯"                    /* 中文站名 */
#define BBSNAME         "Atlantis"                      /* 英文站名 */
#define MYHOSTNAME      "localhost"                     /* 網域名稱 */
#define MYIPADDRESS     "111.222.333.444"               /* 網路位址 */

#define MYVERSION       "ATLANTIS BBS 1.32-RELEASE"     /* 程式版本 */
#define MYBBSID         "ATSRCU"                        /* 轉信代碼 */

#define BBSHOME         "/home/bbsrs"                   /* BBS 的家 */
#define BBSUSER         "bbsrs"                         /* BBS 使用者帳號 */
#define BBSUID          9997                            /* BBS User ID    */
#define BBSGID          97                              /* BBS Group ID   */

#define BBSSHELL        BBSHOME "/bin/bbsrf"            /* 前端程式 */
#define BBSPROG         BBSHOME "/bin/bbs"              /* 主程式位置 */
#define BBSCODE         BBSHOME "/src/maple"            /* 程式碼目錄 */
#define BBSHELP         BBSHOME "/ascii/help"           /* 求助檔目錄 */

/* ----------------------------------------------------- */
/* 結構定義 請審慎判斷實際狀況 設錯你的 BBS 絕對會出問題 */
/* (1.31-S New Patch)                                    */
/* ------------------------------------------------------*/

/* --------------------- 請注意 這五者只能擇一定義 ---------------------- */
#define  NEW_STATION                  /* 定義為 新建/標準 站台            */
#undef   OLD_ATSVERSION               /* 定義為舊的亞站版本 (1.20a 以後)  */
                                      /* 欲讓舊版本使用最小使用者資料結構 */
                                      /* 必需執行轉換程式 single_multi_st */
#undef   TRANS_FROM_SOB               /* 定義為沙灘轉換                   */
#undef   TRANS_FROM_FB3               /* 定義為火鳥轉換                   */
#undef   TRANS_FROM_COLA              /* 定義為可樂轉換                   */
/* ---------------------------------------------------------------------- */

#ifdef   TRANS_FROM_SOB
  #define DEFAULTBOARD   "0Announce"  /* SOB 預設看板名 注意其名稱必需正確 */
#else
  #define DEFAULTBOARD   "Announce"   /* 預設看板 請檢查 boards/Announce */
                                      /* 是否確實存在 */
#endif

#ifdef   TRANS_FROM_FB3
  #undef ENCPASSLEN                   /* 定義若火鳥轉換 有無設定 ENCPASSLEN */
#endif

#ifdef   TRANS_FROM_FB3
  #define  FB_FAMILY                  /* 影響群組分類 不可 undef 除非整個改 */
#endif

#ifdef   TRANS_FROM_COLA
  #define  FB_FAMILY                  /* 影響群組分類 不可 undef 除非整個改 */
  #define  CHECK_COLA_ASCII           /* 判定 Cola 加入之 ASCII 建議使用 */
#endif                                /* 新版 Atlantis 有完整轉換 請洽站長 */

#ifndef  OLD_ATSVERSION
#undef   MIN_USEREC_STRUCT            /* 定義使用最小使用者資料結構 此功能 */
#endif                                /* 不建議使用 會失去許多功能 除非站  */
                                      /* 台有嚴重硬體限制 且不想跑太多功能 */

/* ----------------------------------------------------- */
/* 組態規劃                                              */
/* ------------------------------------------------------*/

#define WITHOUT_CHROOT             /* 不需要 root set-uid */
#define HAVE_MOVIE                 /* 顯示動態告示板 */
#define HAVE_INFO                  /* 顯示程式版本說明  請勿刪除 */
#define HAVE_License               /* 顯示 GNU 版權畫面 請勿刪除 */
#define INTERNET_PRIVATE_EMAIL     /* 可以寄私人信件到 Internet */
#define BIT8                       /* 支援中文系統 */

#define LOGINATTEMPTS   (3)        /* 最大進站失誤次數 */
#define LOGINASNEW      (1)        /* 採用上站申請帳號制度 */
#define VOTE            (1)        /* 提供投票功能 */
#define INTERNET_EMAIL             /* 支援 InterNet Email 功能(含 Forward) */

#undef  EMAIL_JUSTIFY              /* 發出 InterNet Email 身份認證信函 */
#undef  NEWUSER_LIMIT              /* 新手上路的三天限制 */
#undef  INVISIBLE                  /* 隱藏 BBS user 來自何處 */
#define MULTI_LOGINS               /* 允許同時多次上站 */

#undef  POSTBUG                    /* board/mail post 沒有 bug 了 請物開啟 */
#undef  HAVE_REPORT                /* 系統追蹤報告 不建議使用 */
#define INITIAL_SETUP              /* 剛開站，還沒建立預設看板[SYSOP] */
#define HAVE_MMAP                  /* 採用 mmap(): memory mapped I/O */
                                   /* 在 SunOS、FreeBSD 上可以加速 30 倍 */
                                   /* Linux 的 mmap() 有問題，請暫時不要用 */

#define HAVE_ANONYMOUS             /* 提供 Anonymous 板 */
#define HAVE_ORIGIN                /* 顯示文章來自何處 */
#undef  RECORD_MODIFY_ARTICAL      /* 記錄文章變更者與時間 */
#undef  HAVE_MAILCLEAN             /* 清理所有使用者個人信箱 */
#undef  HAVE_SUICIDE               /* 提供使用者自殺功能 */

/* ----------------------------------------------------- */
/* 站台基本設定                     Atlantis BBS 1.31-S1 */
/* ----------------------------------------------------- */

#define DEFAULTST      "atlantis"  /* 定義預設站台名稱 如欲變更請參考 */
                                   /* [文件] 如何更改 ATS 預設站台 */
#define NOC_DFB                    /* 定義允許使用者不必強制閱讀預設看板 */
                                   /* 建議者 keichii (ats) */

#define KEEPWD1        "ats"       /* 定義站上 Anonymous 系統帳號 */
#define KEEPWD2        "atlantis"  /* 同上 此兩帳號只可更改不可刪除 */

#define SHOW_FROMHOST              /* 使用者一覽顯示故鄉來源 */
#ifdef SHOW_FROMHOST
  #define TRANS_FROMHOST           /* 故鄉轉換功能 作者 buggy/Izero (zoo) */
  #ifdef  TRANS_FROMHOST
    #define FROMHOST_BY_FILE       /* 故鄉轉換使用檔案 作者 Dopin (ats) */
  #endif                           /* 如不打算用檔案 請反定義利用記憶體陣列 */
  #define USER_DEFINE_HOST         /* 使用者可以自定故鄉 功能建議 aoki (ats) */
#endif                             /* 更新者 Eros/LAW (hcu) */

#define USE_MULTI_TITLE            /* 定義使用隨機標題列 */
#define USE_SQ_CURSOR              /* 定義使用循序游標 */
#ifdef USE_SQ_CURSOR
  #define CURS_STRING "亞特蘭提斯" /* 請依需要變更 每一字需為全形字 */
  #define CURS_STR_NUM 10          /* 請對照 CURSOR_STRING 數目為半形總數 */
#endif

/* ----------------------------------------------------- */
/* 站台管理原則                     Atlantis BBS 1.31-S1 */
/* ----------------------------------------------------- */

#define SYSOP_CAN_LOGIN            /* 是否允許 SYSOP 由遠端登入 */
#define SUPERVISOR      "SYSOP"    /* 定義超級使用者 許多功能僅此 ID 可執行 */
#define SYSOP_ONLY_DEFAULTST       /* 定義只有具 SYSOP 權限者為預設站台站長 */

#define USE_ACCOUNTS_PERM          /* 定義使用 PERM_ACCOUNTS (帳號總管)功能 */
#define USE_BOARDS_MANAGER         /* 定義使用 PERM_EXTRA1 (看板總管)功能   */

#define MAIL_TO_SYSOP              /* 定義使用者可以寄信給 SYSOP 帳號 */
#define CHECK_LIST_ID_SELF         /* 定義 好友/壞人/上線 名單不可設定自已 */
#define DENY_MESSAGE_WHILE_PURPLE  /* 定義紫隱時只有站長權限可以丟水球 */

#define CHANGE_USER_MAIL_LIMIT     /* 如不想彈性變更使用者信箱上限 請反定義 */

/* ----------------------------------------------------- */
/* 站台雜項功能設定                 Atlantis BBS 1.32-A0 */
/* ----------------------------------------------------- */

#define MultiIssue             /* 使用亂數進站畫面 */
#ifdef  MultiIssue
  #define USE_LITTLE_NOTE      /* 定義使用隨機進站才可定義使用動態小看板 */
#endif

#define ColorClass             /* 使用彩色看板分類 */
#ifdef  ColorClass
  #define CHColor 0            /* 定義彩色看板分類 高亮度顯示(1) 低亮度(0) */
#endif

#undef  UPDATE_WHEN_POST       /* 發文後即把使用者文章數目寫入 .PASSWDS */
#ifdef  UPDATE_WHEN_POST
  #undef COUNT_MONEY_WHEN_POST /* 發文後就發錢 */
#endif

#define REC_ALL_FROM           /* 記錄所有使用者上站來源 未通過認證發文顯示 */
#ifdef REC_ALL_FROM
  #undef REC_LOGINOK           /* 已通過認證使用者發文記錄上站來源 */
#endif

#undef  COUNT_PERSONAL         /* 個人 / 群體 分類看板計算發文數 */
#define COUNT_MAXLOGINS        /* 記錄最高上線人次 */

#define REVIEW_WATER_BALL      /* 定義提供水球回顧 */
#ifdef  REVIEW_WATER_BALL
  #define RECORD_NEW_MSG       /* 記錄新來訊狀況並顯示之 */
  #define CTRL_R_REVIEW        /* 定義使用 (Ctrl+R)^2 執行傳訊回顧 */
  #define CTRL_G_REVIEW        /* 定義使用  Ctrl+G    執行傳訊回顧 */
#endif

#define TRANS_ST_TASK          /* 定義使用 站台名片檔標籤轉換 (ATS 1.32-B1) */
                               /* 建議者 keysound (shadowcity) */

#undef  SAVE_MAIL              /* 定義使用 save_mail 函式 */
#undef  CHOICE_RMSG_USER       /* 定義使用 選擇傳訊對象熱鍵 */
#define LOCK_ARTICAL           /* 定義使用 鎖定文章功能 */

#define SHOW_BRDMSG            /* 定義顯示 看板小招牌顯示看板旗標對應資訊 */
                               /* 建議者 sagit/ktasl (blue.ccsh.kh.edu.tw) */

#define GOOD_BABY              /* 定義使用 乖寶寶自我約束 附加功能 */
#ifdef GOOD_BABY               /* 建議者 bobi (ats.twbbs.org) */
  #undef MOVIE_AND_GOODBABY    /* 動畫更新一次 檢查一次使用自我約束時間內不
                                  該上線的使用者是否在站上 反定義則定點報時
                                  時才檢查一次 (每半小時)                   */
#endif

/* ----------------------------------------------------- */

/* 看板原則                         Atlantis BBS 1.32-A0 */
/* ----------------------------------------------------- */

#define BM_PRINCIPLE                    /* 定義所有板主原則 反定義為首位板主 */

#undef  DEL_BRD_WITH_MAN                /* 刪除看板一併刪除精華區 */

/* ----------------------------------------------------- */
/* 隨 BBS 站規模成長而擴增 (Dopin: 一變更需清 SHM)       */
/* ----------------------------------------------------- */

#define MAXUSERS        (128)         /* 最高註冊人數 */
#define MAXBOARD        (128)         /* 最大開版個數 */
#define MAXACTIVE       (32)          /* 最多同時上站人數 */

#undef  NO_FORCE_LISTS                /* 定義 "不確認" 名單是否超過上限 */
#ifndef NO_FORCE_LISTS
  #define MAX_BOARDFAVOR  (128)       /* 僅是定義讓 check_file_line 傳回判斷 */
#endif

#define MAX_FRIEND      (64)          /* 載入 cache 之最多朋友數目 */
#define MAX_ALOHA       (64)          /* 載入 cache 之最多上線通知 by Dopin */
#define MAX_REJECT      (32)          /* 載入 cache 之最多壞人數目 */

/* ----------------------------------------------------- */
/* 多站系統相關 (ATS 1.31-S New Patch)                   */
/* ----------------------------------------------------- */

#undef  NO_USE_MULTI_STATION          /* 定義是否 "不使用" 多站系統功能 */
#define SYSOP_SEE_ALL                 /* 定義是否 SYSOP 可以看見所有人 */

#ifndef NO_USE_MULTI_STATION          /* 如果沒定義 "不使用" 多站系統 */
  #define ONLY_SEE_SAME_STATION       /* 定義是否只看的到同站台的人 */

  #ifdef ONLY_SEE_SAME_STATION        /* 在只能看見同站台的情況下 */
    #define FRIEND_CAN_SEE_ANYWHERE   /* 定義只要互設好友 在那個站都看的到 */
    #define DEFAULT_ST_SEE_ALL        /* 定義預設站台可以看到全部站台的人 */

    #ifdef DEFAULT_ST_SEE_ALL         /* 在預設站可以看到全部站台的情況下 */
      #define CANT_SEE_HIDE_STATAION  /* 定義不能看到隱藏站的人 */
    #endif
  #else                               /* 在可以看到所有站台的人的情況下 */
    #define CANT_SEE_HIDE_STATAION    /* 定義不能看到隱藏站的人 */
  #endif
#endif

/* ----------------------------------------------------- */
/* BBS Daemon 相關 自由埠/站台隱藏 (1.31-S New Patch)    */
/* ----------------------------------------------------- */

#ifndef NO_USE_MULTI_STATION
  #define MAX_STATIONS  19            /* 最大站台數 限制許多空間的結構大小 */
  #undef  SEE_ALL_STATION             /* 使用者切換站是否看到無法進入的站台 */
#else
  #define MAX_STATIONS  1             /* 不定義多站 站台自然只有一個 */
#endif

#define DEFAULT_BBSD 'Z' // 或 'M' 在此告訴系統預設的 bbsd 是 zbbsd 或 mbbsd

#undef  USE_FREE_PORT                 /* 定義使用自由埠模式 否則依站台順序 */
#ifdef  USE_FREE_PORT
  #define ZBBSD_FREE_PORT             /* in.zbbsd 與 mbbsd 之自由埠定義 */
  #define MBBSD_FREE_PORT             /* 兩者請與 DEFAULT_BBSD 設定一致 */
#else
  #define ZBBSDP 3001                 /* 未使用自由埠時這兩個定義不可拿掉 */
  #define MBBSDP 9001                 /* 但是可以修改埠的起點位置 */
#endif

/* ----------------------------------------------------- */
/* 其他系統上限參數                                      */
/* ----------------------------------------------------- */

#define MAXKEEPMAIL     (100)      /* 最多保留幾封 MAIL？ */
#define MAX_NOTE        (20)       /* 最多保留幾篇留言？ */
#define MAXSIGLINES     (6)        /* 簽名檔引入最大行數 */
#define MAXSTLOGS       (5)        /* 使用者名片檔最大行數 */
#define MAXQUERYLINES   (100)      /* 顯示 Query/Plan 訊息行 Dopin: MAX 250 */
#define MAXPAGES        (1500)     /* more.c 中文章頁數上限 (lines/22) */
#define MOVIE_INT       (30)       /* 切換動畫的週期時間：30 秒 */
#define INDEX_READ_NUM  (128)      /* Dopin: 定義串列模式一次最多讀幾筆 */

#undef CHECK_POST_TIME
#ifdef CHECK_POST_TIME
  #define POST_RANGE    (10)       /* 每篇文張須隔多少時間 請注意 單位是秒 */
  #define POST_LOWER    (60)       /* 幾秒鐘內算是快速發文 */
  #define POST_NUMBER   (5)        /* 幾篇以上檢查是否快速發文 */
#endif

/* ----------------------------------------------------- */
/* 發呆過久自動簽退 / 系統發呆功能                       */
/* ------------------------------------------------------*/

#define SHOW_IDLE_TIME                /* 顯示閒置時間 建議使用 */
#define DOTIMEOUT                     /* 定義發呆過久自動簽退 (踢出) */
                                      /* Dopin 修正讓其確實可以 undef */

#ifdef  DOTIMEOUT
  #define IDLE_TIMEOUT    (60*30)     /* 一般情況之 timeout */
  #define MONITOR_TIMEOUT (60*15)     /* monitor 時之 timeout */
#endif

#undef TAKE_IDLE                      /* 定義提供使用者發呆功能 不建議使用   */
                                      /* 原因是有個帳號 Bug 發呆過久後 SHM   */
                                      /* 可能會錯亂 如發生時請利用 chkpasswd */
                                      /* 執行修復 */

/* ----------------------------------------------------- */
/* chat.c & xchatd.c 中採用的 port 及 protocol           */
/* ------------------------------------------------------*/

#ifndef TRANS_FROM_SOB
#define CHATPORT        5722          /* 聊天室 PORT */
#else
#define CHATPORT        5922
#endif
#define MAXROOM         32            /* 最多有幾間包廂？ */

#define EXIT_LOGOUT      0
#define EXIT_LOSTCONN   -1
#define EXIT_CLIERROR   -2
#define EXIT_TIMEDOUT   -3
#define EXIT_KICK       -4

#define CHAT_LOGIN_OK       "OK"
#define CHAT_LOGIN_EXISTS   "EX"
#define CHAT_LOGIN_INVALID  "IN"
#define CHAT_LOGIN_BOGUS    "BG"

#define BADCIDCHARS " *"               /* Chat Room 中禁用於 nick 的字元 */

/* ----------------------------------------------------- */
/* 系統參數                                              */
/* ----------------------------------------------------- */

#define MAGIC_KEY       1234           /* 身分認證信函編碼 */

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
/* 申請帳號時要求申請者真實資料                          */
/* ----------------------------------------------------- */

#define SHOWUID                        /* 看見使用者 UID */
#define SHOWTTY                        /* 看見使用者 TTY */
#define SHOWPID                        /* 看見使用者 PID */

#define REALINFO                       /* 真實姓名 */
#ifdef  REALINFO
  #undef  ACTS_REALNAMES               /* 主目錄的 (U)ser 顯示真實姓名 */
  #undef  POST_REALNAMES               /* 貼文件時附上真實姓名 */
  #undef  MAIL_REALNAMES               /* 寄站內信件時附上真實姓名 */
  #undef  QUERY_REALNAMES              /* 被 Query 的 User 告知真實姓名 */
#endif

/* ----------------------------------------------------- */
/* Dopin : 外加程式設定 (請依需要變更定義)               */
/* ----------------------------------------------------- */

/* 文章標題類別 */
#define ARTICAL_CLASS         // 簡單型文章類別 原作者 Dopin (ats.twbbs.org)

/* 心情點歌機  */
#define ORDERSONG             // 兩用點歌機 原作者 Dopin (ats.twbbs.org)
#ifdef  ORDERSONG
  #define OSONG_NUM_LIMIT 3   /* 定義一天只能點幾首歌 無限制則 undef */
  #define ORDER_SONG_CAN_MAIL /* 定義點播歌曲可以轉寄至對象信箱 */
  #ifdef ORDER_SONG_CAN_MAIL
     #undef ANONYMOUS_ORSONG  /* 定義可匿名點歌轉寄對方信箱 */
  #endif
  #define OSONG_ANN           /* SongBooks 可使用目錄點歌 */
  #ifdef  OSONG_ANN
    #define SONG_CHOICE_MANDIR "SongBooks"
                              /* 目錄式點歌所在精華區名稱 */
  #endif
  #define OSONG_SEARCH        /* 定義搜尋式點歌 */
  #ifdef  OSONG_SEARCH
    #define SONG_SEARCH_MANDIR "SongBooks"
                              /* 搜尋式點歌所在精華區名稱 需搭配 hn_index */
  #endif
  #undef  SONGCOST            /* 定義點歌是否需要花錢 */
  #ifdef  SONGCOST
    #define SONGCOST_VALUE 10 /* 定義點歌要花多少錢 */
  #endif
#endif

/* 符號輸入工具 */
#define INPUT_TOOLS           // 符號工具 原作者 weichung (bbs.ntit.edu.tw)
                              //          修改者 Dopin (ats.twbbs.org)

/* 抽獎程式體驗版 */
#undef  DRAW                  // 抽獎程式 原作者 Dopin (ats.twbbs.org)

/* 角色扮演系統 */            // 原作者 Dopin (ats.twbbs.org)
#ifndef MIN_USEREC_STRUCT
  #undef RPG                  // 定義是否存取 RPG 相關欄位
  #ifdef  RPG
    #define RPG_STATUS        /* 定義是否於 Query 中顯示各項能力 */
    #define RPG_SETTING       /* 定義是否於使用者設定中更改各項能力 */
    #undef  RPG_FIGHT         /* 對戰程式陽春版 又肥又差的東西 Q__Q */
  #endif
#endif

/* 譯典通 連線程式 */
#define DREYE                  // 客戶端 原作者 statue (bbs.yzu.edu.tw)
                               //        修改者 Dopin (ats.twbbs.org)

/* 星空戰鬥雞 */
#define STAR_CHICKEN           // 原作者群如下 :
/* chyiuan (great.dorm12.nctu.edu.tw) [主發展]                             */
/* dsyan, fennet, tiball, wildcat, Frank, startrek 等多位                  */
/* 圖檔原作者 tiball (bbs.nhctc.edu.tw)                                    */
/* 修改者 Dopin (ats.twbbs.org)                                            */

/* 連署系統內建程式 */
#define AGREE_SYS             // 連署系統程式 原作者 Dopin (ats.twbbs.org)

/* 列出註冊名單 */
#define USE_ULIST             // 原 SOB 內建程式

#define QUERY_REMOTEUSER      // 查詢最近上站記錄 Dopin (ats.twbbs.org)

/* ----------------------------------------------------- */
/* 提供外掛程式                                          */
/* ----------------------------------------------------- */

#undef HAVE_EXTERNAL         /* Dopin: 此定義建議取消 節省空間 */

/* Dopin: 下面這邊都無法使用 保留是因為如有需要外掛下列函式 可方便叫用 */
#ifdef  HAVE_EXTERNAL
#undef  HAVE_TIN              /* 提供 news reader */
#undef  HAVE_GOPHER           /* 提供 gopher */
#undef  HAVE_WWW              /* 提供 www browser */
#undef  HAVE_BRIDGE           /* 提供 bridge game */
#undef  HAVE_BBSDOORS         /* 提供 bbs doors */
#undef  HAVE_GAME             /* 提供網路連線遊戲 */
#undef  HAVE_ADM_SHELL        /* 提供 csh 以便管理 */
#endif

/* 到這邊為止 這些是 SOB 時代的東西 原始碼多已過時無法使用 */
/* 如有找到原始碼 且有需要使用再定義使用 */

#endif
