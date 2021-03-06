/* admin.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : administration routines                      */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#define _ADMIN_C_

#include "bbs.h"

int
x_reg ()			/* 解決無法認證 */
{
  if (dashf ("register.new.tmp"))
    {
      system ("cat register.new.tmp >> register.new");
      system ("rm register.new.tmp");
      pressanykey ("弄好了，下次小心點啦！");
    }
  return RC_FULL;
}

/* ----------------------------------------------------- */
/* 看板管理                                              */
/* ----------------------------------------------------- */

extern int cmpbnames ();

int
invalid_brdname (brd)		/* 定義錯誤看板名稱 */
     char *brd;
{
  register char ch;

  ch = *brd++;
  if (not_alnum (ch))
    return 1;
  while (ch = *brd++)
    {
      if (not_alnum (ch) && ch != '_' && ch != '-' && ch != '.')
	return 1;
    }
  return 0;
}


unsigned
setperms (pbits)
     unsigned pbits;
{
  register int i;
  char choice[4];

  move (4, 0);
  for (i = 0; i < NUMPERMS / 2; i++)
    {
      prints ("%c. %-20s %-15s %c. %-20s %s\n"
	      ,'A' + i, permstrings[i], ((pbits >> i) & 1 ? "●" : "  ")
	      ,i < 10 ? 'Q' + i : '0' + i - 10,
	      permstrings[i + 16], ((pbits >> i + 16) & 1 ? "●" : "  "));
    }
  clrtobot ();
  while (getdata (b_lines - 1, 0, "請按 [A-5] 切換設定，按 [Return] 結束：",
		  choice, 3, LCECHO, 0))
    {
      i = choice[0] - 'a';
      if (i < 0)
	i = choice[0] - '0' + 26;

      if( i>=12 && !HAS_PERM(PERM_SYSOP) )
      {
        bell();
        pressanykey("高層權限 必須由 站長 設定");
        return (pbits);
      }
      else if( ( i==11 || i==13 || i==15 || i>=27 ) && !HAS_PERM(PERM_SUPER))
      {
        bell();
        pressanykey("特殊權限 必須由 超級站長 設定");
        return (pbits);
      }
      else if( ( i>29 ) && !HAS_PERM(PERM_GOD))
      {
        bell();
        pressanykey("超級權限 必須由 貓神 設定");
        return (pbits);
      }
      
      
      if (i >= NUMPERMS)
	bell ();
      else
	{
	  pbits ^= (1 << i);
	  move (i % 16 + 4, i <= 15 ? 24 : 64);
	  prints ((pbits >> i) & 1 ? "●" : "  ");
	}
    }
  return (pbits);
}


/* -------------- */
/* 自動設立精華區 */
/* -------------- */


static void
setup_man (board)
     boardheader *board;
{
  char genbuf[200];

  setapath (genbuf, board->brdname);
  mkdir (genbuf, 0755);
}


static void
bperm_msg (board)
     boardheader *board;
{
  prints ("\n設定 [%s] 看板之(%s)權限：", board->brdname,
	  board->brdattr & BRD_POSTMASK ? "發表" : "閱\讀");
}
static char *classname[] =
{
  "1.站務", "2.校園", "3.學術",
  "4.電腦", "5.藝術", "6.休閒",
  "7.閒聊", "8.其他", "9.個人",
  "[或自行輸入]",
  NULL};
int
m_newbrd ()
{
  boardheader newboard;
  char ans[4];
  int bid, classnum = 0;
  char genbuf[256],number[]="09999\0";
  extern char *boardprefix;

  stand_title ("建立新板");
  memset (&newboard, 0, sizeof (newboard));
  newboard.opentime = time(0);

  do
    {
      if (!getdata (3, 0, msg_bid, newboard.brdname, IDLEN + 1, DOECHO, 0))
	return -1;
    }
  while (invalid_brdname (newboard.brdname));

  if (HAS_PERM (PERM_SYSOP))
    while (classname[classnum] != NULL)
      outs (classname[classnum++]);
  do
    {
      getdata (6, 0, "看板類別：", genbuf, 5, DOECHO, 0);
      if (HAS_PERM (PERM_SYSOP) ||
	  ((strstr (boardprefix, genbuf) != NULL) && strlen (genbuf) == 4))
	break;
      outs ("只能開轄區內類別的版");
    }
  while (1);

  if (genbuf[0] >= '1' && genbuf[0] <= '9')
    {
      strncpy (newboard.title, classname[*genbuf - '1'] + 2, 4);
    }
  else if (strlen (genbuf) >= 4)
    {
      strncpy (newboard.title, genbuf, 4);
    }
  newboard.title[4] = ' ';
  newboard.brdattr = 0;
  newboard.totaltime = 0;
  newboard.totalvisit = 0;
        
  getdata (7, 0, "看板屬性：1.一般  2.子選單  3.分類看板 ", genbuf, 3, LCECHO, 0);

  switch(*genbuf)
  {
    case '1':  
      newboard.brdattr &= ~BRD_CLASS;
      newboard.brdattr &= ~BRD_GROUPBOARD;
      newboard.brdattr &= ~BRD_NOCOUNT;
      strncpy(newboard.title+5,"○",2);
      break;
    case '2':
      newboard.brdattr &= ~BRD_CLASS;
      newboard.brdattr |= BRD_GROUPBOARD;
      newboard.brdattr |= BRD_NOCOUNT;
      strncpy(newboard.title+5,"Σ",2);
      break;
    case '3':
      newboard.brdattr |= BRD_CLASS;
      newboard.brdattr &= ~BRD_GROUPBOARD;
      newboard.brdattr |= BRD_NOCOUNT;
      strncpy(newboard.title+5,"□",2);
      break;
  }
                              
  if(!(newboard.brdattr & BRD_CLASS) && !(newboard.brdattr & BRD_GROUPBOARD))
  {
    getdata (8, 0, "轉信屬性：1.站內  2.轉出  3.轉入  4.轉入與轉出 ", genbuf, 3, LCECHO,0);      
    switch(*genbuf)
    {
      case '1':
        newboard.brdattr &= ~BRD_OUTGOING;
        newboard.brdattr &= ~BRD_INGOING;
        strncpy(newboard.title+5,"○",2);
        break;
      case '2':
        newboard.brdattr |= BRD_OUTGOING;
        newboard.brdattr &= ~BRD_INGOING;
        strncpy(newboard.title+5,"☉",2);
        break;
      case '3':
        newboard.brdattr |= BRD_INGOING;
        newboard.brdattr &= ~BRD_OUTGOING;
        strncpy(newboard.title+5,"◎",2);
        break;
      case '4':
        newboard.brdattr |= BRD_OUTGOING;
        newboard.brdattr |= BRD_INGOING;
        strncpy(newboard.title+5,"●",2);
        break;
    }
  }      

  getdata (9, 0, "看板主題：", genbuf, BTLEN + 1, DOECHO, 0);
  if (genbuf[0])
      strcpy (newboard.title + 7, genbuf);

  newboard.postotal=500;
  itoa(newboard.postotal, &number);
  getdata( 9, 60, "文章上限：", genbuf, 5, LCECHO, number);
  if(genbuf[0])
    newboard.postotal = atoi(genbuf);


  setbpath (genbuf, newboard.brdname);
  mkdir (genbuf, 0755);
  if (getbnum (newboard.brdname) > 0)
    {
      pressanykey (err_bid);
      return -1;
    }
  getdata (10, 0, "板主名單：", newboard.BM, IDLEN * 3 + 3, DOECHO, 0);


  if (HAS_PERM (PERM_SYSOP))
    {
      getdata (11, 0, "可以ZAP(Y/N)？", ans, 2, LCECHO, "Y");
      if (*ans == 'n')
	newboard.brdattr |= BRD_NOZAP;
      getdata (11, 30, "接受BRDMAIL(Y/N) ?", ans, 2, LCECHO, "N");
      if (*ans == 'y')
	newboard.brdattr |= BRD_BRDMAIL;
    }
  newboard.level = 0;
  if (!(newboard.brdattr & BRD_GROUPBOARD || newboard.brdattr & BRD_CLASS))
    {
      getdata (12, 0, "是否為個人板(Y/N) ?", ans, 2, LCECHO, "N");
      if (*ans == 'y' || *ans == 'Y')
        newboard.brdattr |= BRD_PERSONAL;
      getdata (12, 30, HAS_PERM (PERM_SYSOP) ?
	       "看板隱藏/私人/公開(Y/P/N) ?" : "看板私人/公開(P/N) ?"
	       ,ans, 2, LCECHO, "N");
      if (*ans == 'y' && HAS_PERM (PERM_SYSOP))
	{
	  newboard.brdattr |= BRD_HIDE;
	  newboard.brdattr |= BRD_POSTMASK;
	}
      else if (*ans == 'p')
	{
	  newboard.brdattr |= BRD_HIDE;
	}

      getdata (13, 0, "列入統計排行(Y/N) ?", ans, 2, LCECHO, "Y");
      if (*ans == 'n')
	newboard.brdattr |= BRD_NOCOUNT;

      if (HAS_PERM (PERM_SYSOP) && !(newboard.brdattr & BRD_HIDE))
	{
	  getdata (13, 30, "設定讀寫權限(Y/N) ?", ans, 2, LCECHO, "Y");
	  if (*ans == 'y')
	    {
	      getdata (14, 0, "限制 [R]閱\讀 (P)發表 ?", ans, 2, LCECHO, "P");
	      if (*ans == 'p')
		newboard.brdattr |= BRD_POSTMASK;
	      move (1, 0);
	      clrtobot ();
	      bperm_msg (&newboard);
	      newboard.level = setperms (newboard.level);
	      clear ();
	    }
	}
    }
  else
    {
      if (!newboard.BM[0] || newboard.BM[0] == ' ')
	strcpy (newboard.BM, "[目錄]");
      newboard.brdattr |= BRD_POSTMASK;
      newboard.level |= (1 << 15);
    }
  if ((bid = getbnum ("")) > 0)
    {
      substitute_record (fn_board, &newboard, sizeof (newboard), bid);
    }
  else if (rec_add (fn_board, &newboard, sizeof (newboard)) == -1)
    {
      pressanykey (NULL);
      return -1;
    }
  setup_man (&newboard);
  touch_boards ();
  log_usies ("NewBoard", newboard.title);
  pressanykey ("新板成立");
  return 0;
}

int
m_mod_expire (char *bname)
{
  boardheader bh, newbh;
  int bid;
  char genbuf[8],number[]="09999",buf[64];

  bid = getbnum (bname);
  if (rec_get (fn_board, &bh, sizeof (bh), bid) == -1)
  {
    pressanykey (err_bid);
    return -1;
  }
  memcpy (&newbh, &bh, sizeof (boardheader));
  itoa(bh.postotal, &number);
  sprintf(buf,"設定看板 [%s] 的文章上限：",bh.brdname);
  getdata(b_lines, 0, buf, genbuf, 5, LCECHO, number);
  if(genbuf[0])
  {
     newbh.postotal = atoi(genbuf);
     substitute_record (fn_board, &newbh, sizeof (boardheader), bid);
     touch_boards ();
  }
  return 0;
}

int
m_mod_board (char *bname)
{
  boardheader bh, newbh;
  int bid, classnum = 0, avg;
  char genbuf[256],number[]="09999\0";
  extern char *boardprefix;

  bid = getbnum (bname);
  if (rec_get (fn_board, &bh, sizeof (bh), bid) == -1)
    {
      pressanykey (err_bid);
      return -1;
    }
  move (1, 0);
  clrtobot ();
  prints ("看板名稱：%-15s 板主名單：%s\n看板說明：%-49s 文章上限：%d %s\n",
	  bh.brdname, bh.BM, bh.title, bh.postotal,
	  (bh.brdattr & BRD_OUTGOING && bh.brdattr & BRD_INGOING) ? "x2" : "");
  {
    int t = bh.totaltime;
    int day=t/86400, hour=(t/3600)%24, min=(t/60)%60, sec=t%60;
    avg = bh.totalvisit != 0 ? bh.totaltime / bh.totalvisit : 0;
    prints("開板時間：%s\n", Ctime(&bh.opentime));
    prints("拜訪人數：%-15d拜訪總時：%d 天 %d 小時 %d 分 %d 秒 平均停留：%d 秒\n",
	    bh.totalvisit, day, hour, min, sec, avg);
  }
  prints ("最新訪客：%-15s最新時間：%s\n"
	  ,bh.lastvisit, Ctime (&bh.lastime));
  bperm_msg (&bh);
  prints ("%s設限\n可zap:%s 列入統計:%s"
          "轉出:%s 群組:%s 私人:%s 隱藏:%s 信箱:%s 優良:%s 個人:%s",
	  bh.level ? "有" : "不",
	  bh.brdattr & BRD_NOZAP ? "x" : "o",
	  bh.brdattr & BRD_NOCOUNT ? "x" : "o",
	  bh.brdattr & BRD_OUTGOING ? "o" : "x",
	  bh.brdattr & BRD_GROUPBOARD ? "o" : "x",
	  bh.brdattr & BRD_HIDE ? "o" : "x",
	  (bh.brdattr & BRD_HIDE) && (bh.brdattr & BRD_POSTMASK) ? "o" : "x",
	  bh.brdattr & BRD_BRDMAIL ? "o" : "x",
	  bh.brdattr & BRD_GOOD ? "o" : "x",
	  bh.brdattr & BRD_PERSONAL ? "o" : "x");
//	  bh.brdattr & BRD_ANONYMOUS ? "o" : "x");

  if (!HAS_PERM (PERM_SYSOP) && !strcmp(bname,"Security"))
    {
      pressanykey (NULL);
      return 0;
    }

  getdata (9, 0, "看板 (D)刪除 (E)設定 (B)板主 [Q]取消？", genbuf, 3, LCECHO, 0);

  switch (*genbuf)
    {
    case 'd':
      if (!HAS_PERM (PERM_SYSOP))
	break;
      getdata (10, 0, msg_sure_ny, genbuf, 3, LCECHO, "N");
      if (genbuf[0] != 'y')
	{
	  outs (MSG_DEL_CANCEL);
	}
      else
	{
	  strcpy (bname, bh.brdname);
	  sprintf (genbuf, "/bin/rm -fr boards/%s man/boards/%s", bname, bname);
	  system (genbuf);
	  memset (&bh, 0, sizeof (bh));
	  sprintf (bh.title, "[%s] deleted by %s", bname, cuser.userid);
	  substitute_record (fn_board, &bh, sizeof (bh), bid);
	  touch_boards ();
	  log_usies ("DelBoard", bh.title);
	  outs ("刪板完畢");
	}
      break;

    case 'e':

      move (10, 0);
      outs ("直接按 [Return] 不修改該項設定");
      memcpy (&newbh, &bh, sizeof (bh));

      move(11,0);

      if (HAS_PERM (PERM_SYSOP))
	while (classname[classnum] != NULL)
	  outs (classname[classnum++]);
      do
	{
	  getdata (12, 0, "看板類別：", genbuf, 5, DOECHO, bh.title);
	  if (HAS_PERM (PERM_SYSOP) ||
	   ((strstr (boardprefix, genbuf) != NULL) && strlen (genbuf) == 4))
	    break;
	  outs ("只能開轄區內類別的版");
	}
      while (1);

      if (genbuf[0] >= '1' && genbuf[0] <= '9')
	strncpy (newbh.title, classname[*genbuf - '1'] + 2, 4);
      else if (strlen (genbuf) >= 4)
	strncpy (newbh.title, genbuf, 4);

      while (getdata (12, 20, "新看板名稱：", genbuf, IDLEN + 1, DOECHO,0))
	{
	  if (getbnum (genbuf))
	    pressanykey ("錯誤! 板名雷同");
	  else if (!invalid_brdname (genbuf))
	    {
	      strcpy (newbh.brdname, genbuf);
	      break;
	    }
	}


      newbh.title[4] = ' ';

  newbh.brdattr=bh.brdattr;
  
//  brdattr='1';
//  if(bh.brdattr & BRD_CLASS) brdattr='2';
//  if(bh.brdattr & BRD_GROUPBOARD) brdattr='3';
//  if(bh.brdattr & BRD_GOOD) brdattr='4';
  
  getdata (13, 0, "看板屬性：1.一般  2.子選單  3.分類看板  4.優良  ", 
           genbuf, 2, LCECHO, 0);

  if(genbuf[0])
  
  switch(*genbuf)
  {
    case '1':
      newbh.brdattr &= ~BRD_CLASS;
      newbh.brdattr &= ~BRD_GROUPBOARD;
      newbh.brdattr &= ~BRD_NOCOUNT;
//      strncpy(newbh.title+5,"○",2);
      break;
    case '2':
      newbh.brdattr &= ~BRD_CLASS;
      newbh.brdattr |= BRD_GROUPBOARD;
      newbh.brdattr |= BRD_NOCOUNT;
      strncpy(newbh.title+5,"Σ",2);
      break;
    case '3':
      newbh.brdattr |= BRD_CLASS;
      newbh.brdattr &= ~BRD_GROUPBOARD;
      newbh.brdattr |= BRD_NOCOUNT;
      strncpy(newbh.title+5,"->",2);
      break;
    case '4':
      newbh.brdattr &= ~BRD_CLASS;
      newbh.brdattr &= ~BRD_GROUPBOARD;
      newbh.brdattr &= ~BRD_NOCOUNT;
      newbh.brdattr |= BRD_GOOD;
//      strncpy(newbh.title+5,"☆",2);
      break;
  }

  if(!(newbh.brdattr & BRD_CLASS) && !(newbh.brdattr & BRD_GROUPBOARD))
  {
//    brdattr='4';
//    if(!(bh.brdattr & BRD_OUTGOING)) brdattr='3';
//    if(!(bh.brdattr & BRD_INGOING)) brdattr='2';
//    if((bh.brdattr & BRD_OUTGOING) && (bh.brdattr & BRD_INGOING))
//       brdattr='1';
          
    getdata (14, 0, "轉信屬性：1.站內  2.轉出  3.轉入  4.轉入與轉出 ", 
             genbuf, 2, LCECHO, 0);
             
    if(genbuf[0])
      switch(*genbuf)
    {
      case '1':
        newbh.brdattr &= ~BRD_OUTGOING;
        newbh.brdattr &= ~BRD_INGOING;
        strncpy(newbh.title+5,"○",2);
        break;
      case '2':
        newbh.brdattr |= BRD_OUTGOING;
        newbh.brdattr &= ~BRD_INGOING;
        strncpy(newbh.title+5,"☉",2);
        break;
      case '3':
        newbh.brdattr |= BRD_INGOING;
        newbh.brdattr &= ~BRD_OUTGOING;
        strncpy(newbh.title+5,"◎",2);
        break;
      case '4':
        newbh.brdattr |= BRD_OUTGOING;
        newbh.brdattr |= BRD_INGOING;
        strncpy(newbh.title+5,"●",2);
        break;
    }
  }

      getdata (15, 0, "看板主題：", genbuf, BTLEN + 1, DOECHO, bh.title + 7);
      if (genbuf[0])
	  strcpy (newbh.title + 7, genbuf);

      itoa(bh.postotal, &number);
      getdata( 15, 60, "文章上限：", genbuf, 5, LCECHO, number);
      if(genbuf[0])
         newbh.postotal = atoi(genbuf);

      if (getdata (16, 0, "新板主名單：", genbuf, IDLEN * 3 + 3,
		   DOECHO, bh.BM))
	{
	  str_trim (genbuf);
	  strcpy (newbh.BM, genbuf);
	}
      if (HAS_PERM (PERM_SYSOP))
	{
	  getdata (17, 0, "可以ZAP(Y/N) ?", genbuf, 2, LCECHO,
		   newbh.brdattr & BRD_NOZAP ? "N" : "Y");
	  if (*genbuf == 'n')
	    newbh.brdattr |= BRD_NOZAP;
	  else
	    newbh.brdattr &= ~BRD_NOZAP;
	  getdata (17, 30, "接受BRDMAIL(Y/N) ?", genbuf, 2, LCECHO,
		   newbh.brdattr & BRD_BRDMAIL ? "Y" : "N");
	  if (*genbuf == 'y')
	    newbh.brdattr |= BRD_BRDMAIL;
	  else
	    newbh.brdattr &= ~BRD_BRDMAIL;

	}
      if (!(newbh.brdattr & BRD_GROUPBOARD || newbh.brdattr & BRD_CLASS))
	{
	  char ans[4];
          getdata (18, 0, "是否為個人板 (Y/N) ?", ans, 2, LCECHO, bh.brdattr & BRD_PERSONAL ? "y" : "n");
          if (*ans == 'y' || *ans == 'Y')
            newbh.brdattr |= BRD_PERSONAL;
          else
            newbh.brdattr &= ~BRD_PERSONAL;
	  getdata (18, 30, HAS_PERM (PERM_SYSOP) ? "看板隱藏/私人/公開(Y/P/N) ?"
		   : "看板私人/公開(P/N) ?", ans, 2, LCECHO,
		   !(newbh.brdattr & BRD_HIDE) ? "N" :
		   (newbh.brdattr & BRD_POSTMASK) ? "Y" : "P");
	  if (*ans == 'y' && HAS_PERM (PERM_SYSOP))
	    {
	      newbh.brdattr |= BRD_HIDE;
	      newbh.brdattr |= BRD_POSTMASK;
	    }
	  else if (*ans == 'p')
	    {
	      newbh.brdattr |= BRD_HIDE;
	      newbh.brdattr &= ~BRD_POSTMASK;
	    }
	  else
	    newbh.brdattr &= ~BRD_HIDE;

	  getdata (19, 0, "列入統計排行(Y/N) ?", genbuf, 2, LCECHO,
		   newbh.brdattr & BRD_NOCOUNT ? "N" : "Y");
	  if (*genbuf == 'n')
	    newbh.brdattr |= BRD_NOCOUNT;
	  else
	    newbh.brdattr &= ~BRD_NOCOUNT;
	  if (HAS_PERM (PERM_SYSOP) && !(newbh.brdattr & BRD_HIDE))
	    {
	      getdata (19, 30, "是否更改存取權限(Y/N) ?", genbuf, 2,
		       LCECHO, "N");
	      if (*genbuf == 'y')
		{
		  getdata (20, 0, "限制 (R)閱\讀 (P)發表 ?", genbuf, 2, LCECHO,
			   (newbh.brdattr & BRD_POSTMASK ? "P" : "R"));
		  if (newbh.brdattr & BRD_POSTMASK)
		    {
		      if (*genbuf == 'r')
			newbh.brdattr &= ~BRD_POSTMASK;
		    }
		  else
		    {
		      if (*genbuf == 'p')
			newbh.brdattr |= BRD_POSTMASK;
		    }

		  move (1, 0);
		  clrtobot ();
		  bperm_msg (&newbh);
		  newbh.level = setperms (newbh.level);
		}
	    }
	}
      else
	{
	  newbh.brdattr |= BRD_POSTMASK;
	  newbh.level |= (1 << 15);
	}
      getdata (b_lines, 0, msg_sure_ny, genbuf, 2, LCECHO, "y");

      if ((*genbuf == 'y') && memcmp (&newbh, &bh, sizeof (bh)))
	{
	  if (strcmp (bh.brdname, newbh.brdname))
	    {
	      char src[60], tar[60];

	      setbpath (src, bh.brdname);
	      setbpath (tar, newbh.brdname);
	      f_mv (src, tar);

	      setapath (src, bh.brdname);
	      setapath (tar, newbh.brdname);
	      f_mv (src, tar);
	    }
	  setup_man (&newbh);
	  substitute_record (fn_board, &newbh, sizeof (newbh), bid);
	  touch_boards ();
	  log_usies ("SetBoard", newbh.brdname);
	}
      break;
    case 'b':
      memcpy (&newbh, &bh, sizeof (bh));
      if (getdata (11, 0, "新板主名單：", genbuf, IDLEN * 3 + 3, DOECHO, bh.BM))
	{
	  str_trim (genbuf);
	  strcpy (newbh.BM, genbuf);
	}
      substitute_record (fn_board, &newbh, sizeof (newbh), bid);
      touch_boards ();
      log_usies ("SetBoardBM", newbh.brdname);
    }
  return 0;
}

int
m_board ()
{
  char bname[20];
  stand_title ("看板設定");
  make_blist ();
  namecomplete (msg_bid, bname);
  if (!*bname)
    return 0;
  m_mod_board (bname);
}

/* ----------------------------------------------------- */
/* 使用者管理                                            */
/* ----------------------------------------------------- */

int
m_user ()
{
  userec muser;
  int id;
  char genbuf[200];

  stand_title ("使用者設定");
  usercomplete (msg_uid, genbuf);
  if (*genbuf)
    {
      move (2, 0);
      if (id = getuser (genbuf))
	{
	  memcpy (&muser, &xuser, sizeof (muser));
	  user_display (&muser, 1);
	  uinfo_query (&muser, 1, id);
	}
      else
	{
	  pressanykey (err_uid);
	}
    }
  return 0;
}


#ifdef HAVE_TIN
int
post_in_tin (username)
     char *username;
{
  char buf[256];
  FILE *fh;
  int counter = 0;

  sethomefile (buf, username, ".tin/posted");
  fh = fopen (buf, "r");
  if (fh == NULL)
    return 0;
  else
    {
      while (fgets (buf, 255, fh) != NULL)
	{
	  if (buf[9] != 'd' && strncmp (&buf[11], "csie.bbs.test", 13))
	    counter++;
	  if (buf[9] == 'd')
	    counter--;
	}
      fclose (fh);
      return counter;
    }

}
#endif


/* ----------------------------------------------------- */
/* 清除使用者信箱                                        */
/* ----------------------------------------------------- */


#ifdef  HAVE_MAILCLEAN
FILE *cleanlog;
char curruser[IDLEN + 2];
extern int delmsgs[];
extern int delcnt;

static int
domailclean (fhdrp)
     fileheader *fhdrp;
{
  static int newcnt, savecnt, deleted, idc;
  char buf[STRLEN];

  if (!fhdrp)
    {
      fprintf (cleanlog, "new = %d, saved = %d, deleted = %d\n",
	       newcnt, savecnt, deleted);
      newcnt = savecnt = deleted = idc = 0;
      if (delcnt)
	{
//	  sethomedir (buf, curruser);
	  sprintf(buf, "home/%s/mailbox/.DIR", cuser.userid);
	  while (delcnt--)
	    rec_del (buf, sizeof (fileheader), delmsgs[delcnt]);
	}
      delcnt = 0;
      return 1;
    }
  idc++;
  if (!(fhdrp->filemode & MAIL_ALREADY_READ))
    newcnt++;
  else if (fhdrp->filemode & FILE_MARKED)
    savecnt++;
  else
    {
      deleted++;
      sethomefile (buf, curruser, fhdrp->filename);
      unlink (buf);
      delmsgs[delcnt++] = idc;
    }
  return 0;
}


static int
cleanmail (urec)
     userec *urec;
{
  struct stat statb;
  char genbuf[200];

  if (urec->userid[0] == '\0' || !strcmp (urec->userid, str_new))
    return;
//  sethomedir (genbuf, urec->userid);
  sprintf(genbuf, "home/%s/mailbox/.DIR", cuser.userid);
  fprintf (cleanlog, "%s：", urec->userid);
  if (stat (genbuf, &statb) == -1 || statb.st_size == 0)
    fprintf (cleanlog, "no mail\n");
  else
    {
      strcpy (curruser, urec->userid);
      delcnt = 0;
      rec_apply (genbuf, domailclean, sizeof (fileheader));
      domailclean (NULL);
    }
  return 0;
}


int
m_mclean ()
{
  char ans[4];

  getdata (b_lines - 1, 0, msg_sure_ny, ans, 4, LCECHO, "N");
  if (ans[0] != 'y')
    return XEASY;

  cleanlog = fopen ("mailclean.log", "w");
  outmsg ("This is variable msg_working!");

  move (b_lines - 1, 0);
  if (rec_apply (fn_passwd, cleanmail, sizeof (userec)) == -1)
    {
      outs (ERR_PASSWD_OPEN);
    }
  else
    {
      fclose (cleanlog);
      outs ("清除完成! 記錄檔 mailclean.log.");
    }
  return XEASY;
}
#endif /* HAVE_MAILCLEAN */

/* ----------------------------------------------------- */
/* 處理 Register Form                                    */
/* ----------------------------------------------------- */

static int
scan_register_form (regfile)
     char *regfile;
{
  char genbuf[200];
  static char *logfile = "register.log";
  static char *field[] =
  {"num", "uid", "name", "howto", "email",
   "addr", "phone", "career", NULL};
  static char *finfo[] =
  {"帳號編號", "申請代號", "真實姓名", "從何處得知",
   "電子郵件信箱", "目前住址", "連絡電話", "服務單位", "填空位", NULL};
  static char *reason[] =
  {"輸入真實姓名", "詳填學校科系與年級",
   "填寫完整的住址資料", "詳填連絡電話", "確實填寫註冊申請表",
   "用中文填寫申請單", "請填寫從何處得知本站", NULL};

  userec muser;
  FILE *fn, *fout, *freg;
  char fdata[8][STRLEN];
  char fname[STRLEN], buf[STRLEN];
  char ans[4], *ptr, *uid;
  int n, unum;

  uid = cuser.userid;
  sprintf (fname, "%s.tmp", regfile);
  if (dashf (fname))
    {
      pressanykey ("其他 SYSOP 也在審核註冊申請單");
      return -1;
    }
  f_mv (regfile, fname);
  if ((fn = fopen (fname, "r")) == NULL)
    {
      pressanykey ("系統錯誤，無法讀取註冊資料檔: %s", fname);
      return -1;
    }

  memset (fdata, 0, sizeof (fdata));
  while (fgets (genbuf, STRLEN, fn))
    {
      if (ptr = (char *) strstr (genbuf, ": "))
	{
	  *ptr = '\0';
	  for (n = 0; field[n]; n++)
	    {
	      if (strcmp (genbuf, field[n]) == 0)
		{
		  strcpy (fdata[n], ptr + 2);
		  if (ptr = (char *) strchr (fdata[n], '\n'))
		    *ptr = '\0';
		}
	    }
	}
      else if ((unum = getuser (fdata[1])) == 0)
	{
	  move (2, 0);
	  clrtobot ();
	  for (n = 0; field[n]; n++)
	    prints ("%s     : %s\n", finfo[n], fdata[n]);
	  pressanykey ("系統錯誤，查無此人");
	}
      else
	{
	  memcpy (&muser, &xuser, sizeof (muser));
	  move (1, 0);
	  prints ("帳號位置    ：%d\n", unum);
	  user_display (&muser, 1);
	  move (13, 0);
	  prints ("[1;32m---- 請站長嚴格審核使用者資料 - 這是該 User 第 %d 次註冊 ---[m\n", muser.rtimes);
	  for (n = 0; field[n]; n++)
	    prints ("%-12s：%s\n", finfo[n], fdata[n]);
	  if (muser.userlevel & PERM_LOGINOK)
	    {
	      getdata (b_lines - 1, 0, "此帳號已經完成註冊, 更新(Y/N/Skip)？[N] ",
		       ans, 3, LCECHO, "Y");
	      if (ans[0] != 'y' && ans[0] != 's')
		ans[0] = 'd';
	    }
	  else
	    getdata (b_lines - 1, 0, "是否接受此資料(Y/N/Q/Del/Skip)？[S] ",
		     ans, 3, LCECHO, "S");
	  move (2, 0);
	  clrtobot ();

	  switch (ans[0])
	    {
	    case 'y':

	      prints ("以下使用者資料已經更新:\n");
	      mail2user (muser,"[註冊成功\囉]",BBSHOME"/etc/registered");
	      muser.userlevel |= (PERM_LOGINOK | PERM_PAGE | PERM_CHAT | PERM_POST);
	      sprintf (genbuf, "%s:%s:%s", fdata[6], fdata[7], uid);
	      strncpy (muser.justify, genbuf, REGLEN);
	      sethomefile (buf, muser.userid, "justify");
	      if (fout = fopen (buf, "a"))
		{
		  fprintf (fout, "%s\n", genbuf);
		  fclose (fout);
		}
	      substitute_record (fn_passwd, &muser, sizeof (muser), unum);

	      if (fout = fopen (logfile, "a"))
		{
		  for (n = 0; field[n]; n++)
		    fprintf (fout, "%s: %s\n", field[n], fdata[n]);
		  n = time (NULL);
		  fprintf (fout, "Date: %s\n", Cdate (&n));
		  fprintf (fout, "Approved: %s\n", uid);
		  fprintf (fout, "----\n");
		  fclose (fout);
		}
	      break;

	    case 'q':		/* 太累了，結束休息 */

	      if (freg = fopen (regfile, "a"))
		{
		  for (n = 0; field[n]; n++)
		    fprintf (freg, "%s: %s\n", field[n], fdata[n]);
		  fprintf (freg, "----\n");
		  while (fgets (genbuf, STRLEN, fn))
		    fputs (genbuf, freg);
		  fclose (freg);
		}

	    case 'd':
	      break;

	    case 'n':

	      for (n = 0; field[n]; n++)
		prints ("%s: %s\n", finfo[n], fdata[n]);
	      move (9, 0);
	      prints ("請提出退回申請表原因，按 <enter> 取消\n");
	      for (n = 0; reason[n]; n++)
		prints ("%d) 請%s\n", n, reason[n]);

/*
   woju
 */
	      if (getdata (10 + n, 0, "退回原因：", buf, 60, DOECHO, 0))
		{
		  int i;
		  fileheader mhdr;
		  char title[128], buf1[80];
		  FILE *fp;

		  i = buf[0] - '0';
		  if (i >= 0 && i < n)
		    strcpy (buf, reason[i]);
		  sprintf (genbuf, "[退回原因] 請%s", buf);

		  //sethomepath (buf1, muser.userid);
		  sprintf(buf1, "home/%s/mailbox", muser.userid);
		  stampfile (buf1, &mhdr);
		  strcpy (mhdr.owner, cuser.userid);
		  strncpy (mhdr.title, "[註冊失敗]", TTLEN);
		  mhdr.savemode = mhdr.filemode = 0;
//		  sethomedir (title, muser.userid);
                  sprintf(title, "home/%s/mailbox/.DIR", muser.userid);
		  if (rec_add (title, &mhdr, sizeof (mhdr)) != -1)
		    {
		      fp = fopen (buf1, "w");
		      fprintf (fp, "%s\n", genbuf);
		      fclose (fp);
		    }
/*
   strncpy(muser.address, genbuf, NAMELEN);
   substitute_record(fn_passwd, &muser, sizeof(muser), unum);
 */
		  if (fout = fopen (logfile, "a"))
		    {
		      for (n = 0; field[n]; n++)
			fprintf (fout, "%s: %s\n", field[n], fdata[n]);
		      n = time (NULL);
		      fprintf (fout, "Date: %s\n", Cdate (&n));
		      fprintf (fout, "Rejected: %s [%s]\n----\n", uid, buf);
		      fclose (fout);
		    }
		  break;
		}
	      move (10, 0);
	      clrtobot ();
	      prints ("取消退回此註冊申請表");

	    default:		/* put back to regfile */

	      if (freg = fopen (regfile, "a"))
		{
		  for (n = 0; field[n]; n++)
		    fprintf (freg, "%s: %s\n", field[n], fdata[n]);
		  fprintf (freg, "----\n");
		  fclose (freg);
		}
	    }
	}
    }
  fclose (fn);
  unlink (fname);
  return (0);
}


int
m_register ()
{
  FILE *fn;
  int x, y, wid, len;
  char ans[4];
  char genbuf[200];

  if ((fn = fopen (fn_register, "r")) == NULL)
    {
      outmsg ("目前並無新註冊資料");
      return XEASY;
    }

  stand_title ("審核使用者註冊資料");
  y = 2;
  x = wid = 0;

  while (fgets (genbuf, STRLEN, fn) && x < 65)
    {
      if (strncmp (genbuf, "uid: ", 5) == 0)
	{
	  move (y++, x);
	  outs (genbuf + 5);
	  len = strlen (genbuf + 5);
	  if (len > wid)
	    wid = len;
	  if (y >= t_lines - 3)
	    {
	      y = 2;
	      x += wid + 2;
	    }
	}
    }
  fclose (fn);
  getdata (b_lines - 1, 0, "開始審核嗎(Y/N)？[Y] ", ans, 3, LCECHO, "Y");
  if (ans[0] == 'y')
    scan_register_form (fn_register);

  return 0;
}

/* Ptt */
/*
extern int bad_user_id(char userid[]);
  int
search_bad_id()
{
  userec user;
  char ch;
  int coun=0;
  FILE *fp1=fopen(".PASSWDS","r");
  char buf[100];
  clear();
  while( (fread( &user, sizeof(user), 1, fp1))>0 ) {
    coun ++;
    move(0,0);
    sprintf(buf,"尋找損毀id\n第 [%d] 筆資料\n",coun);
    outs(buf);
    refresh();
    if(bad_user_id(user.userid))
    {
      user_display(&user, 1);
      uinfo_query(&user, 1, coun);
      outs("[44m               任一鍵[37m:搜尋下一個          [33m Q[37m: 離開                         [m");
      ch=igetch();
      if(ch=='q' || ch =='Q') return 0;
      clear();
    }
  }
  fclose(fp1);
  return 0;
}
*/

int
search_key_user ()
{
  userec user;
  char ch;
  int coun = 0;
  FILE *fp1 = fopen ("PASSWDS", "r");
  char buf[100], key[22];

  if (!fp1)
    fp1 = fopen (".PASSWDS", "r");
  clear ();
  getdata (0, 0, "請輸入使用者關鍵字 [姓名|email|ID|電話|地址]:", key, 21, DOECHO, 0);
  while ((fread (&user, sizeof (user), 1, fp1)) > 0)
    {
      coun++;
      move (1, 0);
      sprintf (buf, "第 [%d] 筆資料\n", coun);
      outs (buf);
      refresh ();
      if (strstr (user.userid, key) || strstr (user.realname, key) ||
	  strstr (user.username, key) || strstr (user.lasthost, key) ||
	  strstr (user.email, key) || strstr (user.address, key) ||
	  strstr (user.justify, key))
	{
	  user_display (&user, 1);
	  uinfo_query (&user, 1, coun);
	  outs (
"[1;44;33m       任一鍵[37m:搜尋下一個          [33m Q[37m: 離開                        [m ");
	  ch = igetch ();
	  if (ch == 'q' || ch == 'Q')
	    return 0;
	  clear ();
	  move (0, 0);
	  outs ("請輸入使用者關鍵字 [姓名|email|ID|電話|地址]:");
	  outs (key);
	}
    }
  fclose (fp1);
  return 0;
}

adm_givegold()
{
   int money;
   char id[IDLEN+1],buf[256],reason[60];
   FILE *fp=fopen("tmp/givebonus","w");
   fileheader mymail;

   time_t now;
   time(&now);
   move(12,0);
   usercomplete("輸入對方的ID：", id);
   if (!id[0] || !getdata(14, 0, "要發多少錢獎金？", buf, 5, LCECHO,0)) return;
   money = atoi(buf);
   if(money > 0 && (inugold(id, money) != -1))
   {
     sprintf(buf,"作者: %s \n"
                 "標題:[發放獎金] 發給你 %d 金幣唷！\n"
                 "時間: %s\n",cuser.userid,money,ctime(&now));
     fputs(buf,fp);
     while(!getdata(15,0,"請輸入理由：",reason,60,DOECHO ,"收錄 xxx 板精華區"));
     sprintf(buf,"[1;32m%s[37m 發給你 [33m%d [37m元金幣。\n"
                 "他的理由是：[33m %s [m",cuser.userid,money,reason);
     fputs(buf,fp);
     fclose(fp);
     sprintf(buf,"home/%s", id);
     stampfile(buf, &mymail);
     strcpy(mymail.owner, cuser.userid);
     rename ("tmp/givebonus",buf);
     sprintf(mymail.title,"[發放獎金] 送你 %d 元金幣唷！",money);
     sprintf(buf,"home/%s/.DIR",id);
     rec_add(buf, &mymail, sizeof(mymail));
     sprintf(buf,"[1;33m%s %s [37m把金幣 [33m%d 元 [37m發給[33m %s[37m",
     Cdate(&now),cuser.userid,money,id);
     f_cat("log/bonus.log",buf);
   }
   return;
}  

reload_cache ()
{
  reload_ucache ();
  reload_bcache ();
  reload_filmcache ();
  reload_fcache ();
//  sysop_bbcall("%s : Reload All Cache!",cuser.userid); 
  log_usies ("CACHE", "SYSOP Reload ALL CACHE!");
}
