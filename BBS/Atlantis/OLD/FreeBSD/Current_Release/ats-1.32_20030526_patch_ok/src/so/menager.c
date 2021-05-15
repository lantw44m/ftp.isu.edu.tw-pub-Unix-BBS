#include "bbs.h"

extern char station_limit[];

/* Dopin: 簡陋暴力型不當板名檢查 Q_Q */
static int invalid_brdname1(char *brd) {
   char ci = 0;
   int ch;

   while(brd[ci]) {
      ch = (int)brd[ci];
      if(not_alnum(ch)) if(ch != '_' && ch != '-' && ch != '.') return 1;
      ci++;
}

   return 0;
}

static void bperm_msg(boardheader *board) {
  prints("\n設定 [%s] 看板之(%s)權限：", board->brdname,
         board->level & PERM_POSTMASK ? "發表" : "閱\讀");
}

static void setup_man(boardheader *board) {
   char genbuf[200];
   FILE *fp;

   setapath(genbuf, board->brdname);
   mkdir(genbuf, 0755);
}

/* Dopin */
int m_newbrd_create(char *station, char *boardname, char *brdcname, char *bmn,
                    char *sysop, usint level) {
  boardheader newboard;
  char ans[4];
  int bid, i;
  char genbuf[200];

  if(!is_station_sysop()
#ifdef USE_BOARDS_MANAGER
     && !HAS_PERM(PERM_EXTRA1)
#endif
  )
    return FULLUPDATE;

  if(!boardname) stand_title("建立新板");
  memset(&newboard, 0, sizeof(newboard));

  if(boardname) strcpy(newboard.brdname, boardname);
  else {
     do {
        if(!getdata(1, 0, msg_bid, newboard.brdname, 26, DOECHO, 0))
           return -1;
     } while(invalid_brdname1(newboard.brdname));
  }

  if(boardname)
     strcpy(newboard.title, brdcname);
  else
     getdata(2, 0, "看板主題： ", newboard.title, BTLEN + 1, DOECHO, 0);

  setbpath(genbuf, newboard.brdname);
  if (getbnum(newboard.brdname) > 0 || mkdir(genbuf, 0755) == -1)
  {
    if(!boardname) pressanykey(err_bid);
    return -1;
  }

  if(boardname)
     strcpy(newboard.BM, bmn);
  else
     getdata(3, 0, "板主名單： ", newboard.BM, IDLEN * 3 + 3, DOECHO, 0);

  if(boardname) {
     if(station)
        strcpy(newboard.station, station);
     else
        strcpy(newboard.station, DEFAULTST);
  }
  else {
#ifndef NO_USE_MULTI_STATION
     if(!strcmp(cuser.station, DEFAULTST)) {
        char tst[100];
        sprintf(tst, "所屬站台(如不確定請打 %s 或直接 Enter) : ", DEFAULTST);
        getdata(4, 0, tst, genbuf, 16, DOECHO, 0);
        if(!genbuf[0])
           strcpy(newboard.station, DEFAULTST);
        else
           strcpy(newboard.station, genbuf);
     }
     else
        strcpy(newboard.station, cuser.station);
#else
     strcpy(newboard.station, DEFAULTST);
#endif
  }

  if(boardname)
     strcpy(newboard.sysop, sysop);
  else {
#ifndef SYSOP_ONLY_DEFAULTST
     if(!strcmp(cuser.station, DEFAULTST))
        getdata(5, 0, "負責站長： ", newboard.sysop, 16, DOECHO, 0);
     else
#endif
        strcpy(newboard.sysop, cuser.userid);
  }
  if(boardname) {
     newboard.yankflags[10] = 0;
     newboard.yankflags[0] = 1;
     newboard.yankflags[1] = 1;
     newboard.pastbrdname[0] = 0;
  }
  else {
#ifndef NO_USE_MULTI_STATION
     if(station_limit[cuser.now_stno]) newboard.yankflags[0] = 1;
     else
#endif
     {
        getdata(6, 0, "是否列入文章記錄 (增加發文數) [Y/n] : ", genbuf, 2,
                LCECHO, 0);
        if(genbuf[0] == 'n') newboard.yankflags[0] = 1;
        else newboard.yankflags[0] = 0;
     }
     getdata(7, 0, "是否可以執行文章推薦 [Y/n] : ", genbuf, 2, LCECHO, 0);
     if(genbuf[0] == 'n') newboard.yankflags[1] = 1;
     else newboard.yankflags[1] = 0;
  }

  newboard.level = 0;
  if(boardname)
     newboard.level |= level;
  else {
     getdata(8, 0, "設定讀寫權限(Y/N)？[N] ", ans, 2, LCECHO, 0);
     if(*ans == 'y') {
       getdata(9, 0, "限制 [R]閱\讀 (P)發表？ ", ans, 2, LCECHO, 0);
       if(*ans == 'p') newboard.level = PERM_POSTMASK;

       move(1, 0);
       clrtobot();
       bperm_msg(&newboard);

       newboard.level = setperms(newboard.level, NULL);
       clear();
     }
  }

  if((bid = getbnum("")) > 0) {
    substitute_record(fn_board, &newboard, sizeof(newboard), bid);
  }
  else if(append_record(fn_board, &newboard, sizeof(newboard)) == -1) {
    if(!boardname) pressanykey(NULL);
    return -1;
  }
  setup_man(&newboard);
  touch_boards();

  if(!boardname) {
     outs("\n新板成立");
     log_usies("NewBoard", newboard.title);
     pressanykey(NULL);
  }
  return 0;
}

int m_newbrd(void) {
    m_newbrd_create(NULL, NULL, NULL, NULL, NULL, 0);
}

int m_board() {
  boardheader bh, newbh;
  char bname[26];
  int bid;
  char genbuf[200];

  if(!is_station_sysop()
#ifdef USE_BOARDS_MANAGER
     && !HAS_PERM(PERM_EXTRA1)
#endif
  )

  clear();
  stand_title("看板設定");
  make_blist();
  namecomplete(msg_bid, bname);
  if(!*bname) return 0;

  if(is_global_board(bname) && !HAS_PERM(PERM_SYSOP)) return 0;

  bid = getbnum(bname);
  if(get_record(fn_board, &bh, sizeof(bh), bid) == -1) {
    pressanykey(err_bid);
    return -1;
  }

  prints("看板名稱：%s\n看板說明：%s\n板主名單：%s",
    bh.brdname, bh.title, bh.BM);

  bperm_msg(&bh);
  prints("%s設限", (bh.level & ~PERM_POSTMASK) ? "有" : "不");
  prints("  文章計算 : %s  推薦許\可 %s", !bh.yankflags[0] ? "有" : "無",
          !bh.yankflags[1] ? "可" : "否");

  getdata(7, 0, "看板 (D)刪除 (E)設定 (Q)取消？[Q] ", genbuf, 3, LCECHO, 0);

  switch (*genbuf) {
  case 'd':
    getdata(8, 0, msg_sure_ny, genbuf, 3, LCECHO, 0);
    if(genbuf[0] != 'y') outs(MSG_DEL_CANCEL);
    else
    {
      sprintf(genbuf, "/bin/rm -fr boards/%s"
#ifdef DEL_BRD_WITH_MAN
              " man/boards/%s"
#endif
              , bh.brdname
#ifdef DEL_BRD_WITH_MAN
              , bh.brdname
#endif
       );
      system(genbuf);

      memset(&bh, 0, sizeof(bh));
      sprintf(bh.title, "[%s] deleted by %s", bname, cuser.userid);
      substitute_record(fn_board, &bh, sizeof(bh), bid);
      touch_boards();

      log_usies("DelBoard", bh.title);
      pressanykey("刪板完畢");
    }
    break;

  case 'e':
    memcpy(&newbh, &bh, sizeof(bh));

       while(getdata(9, 0, "新看板名稱：", genbuf, 26, DOECHO, newbh.brdname))
       {
         if(strcasecmp(newbh.brdname, genbuf) && getbnum(genbuf))
            move(0, 3, "錯誤! 板名雷同");
         else if(!invalid_brdname1(genbuf)) {
           strcpy(newbh.brdname, genbuf);
           break;
         }
       }

    if(station_limit[cuser.now_stno]) newbh.yankflags[0] = 1;
    else {
       getdata(10, 0, "是否列入文章記錄 (增加發文數) [Y/n] : ", genbuf, 2,
               DOECHO, 0);
       if(genbuf[0] == 'n' || genbuf[0] == 'N') newbh.yankflags[0] = 1;
       else newbh.yankflags[0] = 0;
    }
    getdata(11, 0, "是否可以執行文章推薦 [Y/n] : ", genbuf, 2, DOECHO, 0);
    if(genbuf[0] == 'n' || genbuf[0] == 'N') newbh.yankflags[1] = 1;
    else newbh.yankflags[1] = 0;

    if(getdata(12, 0, "新的看板說明： ", genbuf, BTLEN + 1, DOECHO,
       newbh.title)) strcpy(newbh.title, genbuf);

    if(getdata(13, 0, "新的板主名單： ", genbuf, IDLEN * 3 + 3, DOECHO,
       newbh.BM)) {
      trim(genbuf);
      strcpy(newbh.BM, genbuf);
    }

#ifndef NO_USE_MULTI_STATION
    if(!strcmp(cuser.station, DEFAULTST)) {
       getdata(14, 0, "所屬站台： ", genbuf, 16, DOECHO, newbh.station);
       if(!genbuf[0]) strcpy(newbh.station, DEFAULTST);
       else strcpy(newbh.station, genbuf);
    }
    else strcpy(newbh.station, cuser.station);
#else
    strcpy(newbh.station, DEFAULTST);
#endif

#ifndef NO_USE_MULTI_STATION
    if(!strcmp(cuser.station, DEFAULTST)) {
       getdata(15, 0, "負責站長：<無則省略> ", genbuf, 16, DOECHO,
               newbh.sysop);
       if(!genbuf[0]) strcpy(newbh.sysop, "SYSOP");
    }
    else strcpy(newbh.sysop, cuser.userid);
#else
    strcpy(newbh.sysop, "SYSOP");
#endif

    getdata(16, 0, "是否更改存取權限(Y/N)？[N] ", genbuf, 2, LCECHO, 0);
    if(*genbuf == 'y') {
      char ans[4];

      getdata(17, 0, "限制 (R)閱\讀 (P)發表？ ", ans, 2, LCECHO,
         newbh.level & PERM_POSTMASK ? "P" : "R");
      if(newbh.level & PERM_POSTMASK) {
        if(*ans == 'r') newbh.level &= ~PERM_POSTMASK;
      }
      else if(*ans == 'p') newbh.level |= PERM_POSTMASK;

      move(1, 0);
      clrtobot();
      bperm_msg(&newbh);
      newbh.level = setperms(newbh.level, NULL);
    }

    out2line(1, b_lines-1, "");
    getdata(b_lines - 1, 0, msg_sure_ny, genbuf, 4, LCECHO, 0);

    if((*genbuf == 'y') && memcmp(&newbh, &bh, sizeof(bh))) {
      if(strcmp(bh.brdname, newbh.brdname)) {
        char src[60], tar[60];

        setbpath(src, bh.brdname);
        setbpath(tar, newbh.brdname);
        Rename(src, tar);

        setapath(src, bh.brdname);
        setapath(tar, newbh.brdname);
        Rename(src, tar);
      }
      setup_man(&newbh);
      substitute_record(fn_board, &newbh, sizeof(newbh), bid);
      touch_boards();
/*
woju
*/
      log_usies("SetBoard", newbh.brdname);
    }
  }
  return 0;
}

/* ----------------------------------------------------- */
/* 使用者管理                                            */
/* ----------------------------------------------------- */

int m_user() {
  userec muser;
  int id;
  char genbuf[200];

  if(!is_station_sysop()      /* 改成這樣 有個缺點 就是看板總管不能兼分站長 */
#ifdef USE_BOARDS_MANAGER     /* 是可以改的完整對應 但要加的碼太多 衡量結果 */
    && !HAS_PERM(PERM_EXTRA1) /* 還是算了 -_-;;; */
#endif
#ifdef USE_ACCOUNTS_PERM
    && !HAS_PERM(PERM_ACCOUNTS)
#endif
  )
     return 0;

  stand_title("使用者設定");
  usercomplete(msg_uid, genbuf, 0);
  if (*genbuf)
  {
    cuser.extra_mode[7] = 1;
    move(2, 0);
    if(id = getuser(genbuf)) {
      memcpy(&muser, &xuser, sizeof(muser));
      user_display(&muser, 1);
      uinfo_query(&muser, 1, id);
    }
    else {
      pressanykey(err_uid);
      clrtoeol();
    }
    cuser.extra_mode[7] = 0;
  }
  return 0;
}
