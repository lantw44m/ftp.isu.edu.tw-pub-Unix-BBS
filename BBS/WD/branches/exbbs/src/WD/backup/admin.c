/* admin.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : administration routines                      */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#define _ADMIN_C_

#include "bbs.h"

int
x_reg ()			/* ¸Ñ¨MµLªk»{ÃÒ */
{
  if (dashf ("register.new.tmp"))
    {
      system ("cat register.new.tmp >> register.new");
      system ("rm register.new.tmp");
      pressanykey ("§Ë¦n¤F¡A¤U¦¸¤p¤ßÂI°Õ¡I");
    }
  return RC_FULL;
}

/* ----------------------------------------------------- */
/* ¬ÝªOºÞ²z                                              */
/* ----------------------------------------------------- */

extern int cmpbnames ();

int
invalid_brdname (brd)		/* ©w¸q¿ù»~¬ÝªO¦WºÙ */
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
	      ,'A' + i, permstrings[i], ((pbits >> i) & 1 ? "¡´" : "  ")
	      ,i < 10 ? 'Q' + i : '0' + i - 10,
	      permstrings[i + 16], ((pbits >> i + 16) & 1 ? "¡´" : "  "));
    }
  clrtobot ();
  while (getdata (b_lines - 1, 0, "½Ð«ö [A-5] ¤Á´«³]©w¡A«ö [Return] µ²§ô¡G",
		  choice, 3, LCECHO, 0))
    {
      i = choice[0] - 'a';
      if (i < 0)
	i = choice[0] - '0' + 26;

      if( i>=12 && !HAS_PERM(PERM_SYSOP) )
      {
        bell();
        pressanykey("°ª¼hÅv­­ ¥²¶·¥Ñ ¯¸ªø ³]©w");
        return (pbits);
      }
      else if( ( i==11 || i==13 || i==15 || i>=27 ) && !HAS_PERM(PERM_SUPER))
      {
        bell();
        pressanykey("¯S®íÅv­­ ¥²¶·¥Ñ ¶W¯Å¯¸ªø ³]©w");
        return (pbits);
      }
      else if( ( i>29 ) && !HAS_PERM(PERM_GOD))
      {
        bell();
        pressanykey("¶W¯ÅÅv­­ ¥²¶·¥Ñ ¿ß¯« ³]©w");
        return (pbits);
      }
      
      
      if (i >= NUMPERMS)
	bell ();
      else
	{
	  pbits ^= (1 << i);
	  move (i % 16 + 4, i <= 15 ? 24 : 64);
	  prints ((pbits >> i) & 1 ? "¡´" : "  ");
	}
    }
  return (pbits);
}


/* -------------- */
/* ¦Û°Ê³]¥ßºëµØ°Ï */
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
  prints ("\n³]©w [%s] ¬ÝªO¤§(%s)Åv­­¡G", board->brdname,
	  board->brdattr & BRD_POSTMASK ? "µoªí" : "¾\\Åª");
}
static char *classname[] =
{
  "1.¯¸°È", "2.®Õ¶é", "3.¾Ç³N",
  "4.¹q¸£", "5.ÃÀ³N", "6.¥ð¶¢",
  "7.¶¢²á", "8.¨ä¥L", "9.­Ó¤H",
  "[©Î¦Û¦æ¿é¤J]",
  NULL};
int
m_newbrd ()
{
  boardheader newboard;
  char ans[4];
  int bid, classnum = 0;
  char genbuf[256],number[]="09999\0";
  extern char *boardprefix;

  stand_title ("«Ø¥ß·sªO");
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
      getdata (6, 0, "¬ÝªOÃþ§O¡G", genbuf, 5, DOECHO, 0);
      if (HAS_PERM (PERM_SYSOP) ||
	  ((strstr (boardprefix, genbuf) != NULL) && strlen (genbuf) == 4))
	break;
      outs ("¥u¯à¶}ÁÒ°Ï¤ºÃþ§Oªºª©");
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
        
  getdata (7, 0, "¬ÝªOÄÝ©Ê¡G1.¤@¯ë  2.¤l¿ï³æ  3.¤ÀÃþ¬ÝªO ", genbuf, 3, LCECHO, 0);

  switch(*genbuf)
  {
    case '1':  
      newboard.brdattr &= ~BRD_CLASS;
      newboard.brdattr &= ~BRD_GROUPBOARD;
      newboard.brdattr &= ~BRD_NOCOUNT;
      strncpy(newboard.title+5,"¡³",2);
      break;
    case '2':
      newboard.brdattr &= ~BRD_CLASS;
      newboard.brdattr |= BRD_GROUPBOARD;
      newboard.brdattr |= BRD_NOCOUNT;
      strncpy(newboard.title+5,"£U",2);
      break;
    case '3':
      newboard.brdattr |= BRD_CLASS;
      newboard.brdattr &= ~BRD_GROUPBOARD;
      newboard.brdattr |= BRD_NOCOUNT;
      strncpy(newboard.title+5,"¡¼",2);
      break;
  }
                              
  if(!(newboard.brdattr & BRD_CLASS) && !(newboard.brdattr & BRD_GROUPBOARD))
  {
    getdata (8, 0, "Âà«HÄÝ©Ê¡G1.¯¸¤º  2.Âà¥X  3.Âà¤J  4.Âà¤J»PÂà¥X ", genbuf, 3, LCECHO,0);      
    switch(*genbuf)
    {
      case '1':
        newboard.brdattr &= ~BRD_OUTGOING;
        newboard.brdattr &= ~BRD_INGOING;
        strncpy(newboard.title+5,"¡³",2);
        break;
      case '2':
        newboard.brdattr |= BRD_OUTGOING;
        newboard.brdattr &= ~BRD_INGOING;
        strncpy(newboard.title+5,"¡ó",2);
        break;
      case '3':
        newboard.brdattr |= BRD_INGOING;
        newboard.brdattr &= ~BRD_OUTGOING;
        strncpy(newboard.title+5,"¡·",2);
        break;
      case '4':
        newboard.brdattr |= BRD_OUTGOING;
        newboard.brdattr |= BRD_INGOING;
        strncpy(newboard.title+5,"¡´",2);
        break;
    }
  }      

  getdata (9, 0, "¬ÝªO¥DÃD¡G", genbuf, BTLEN + 1, DOECHO, 0);
  if (genbuf[0])
      strcpy (newboard.title + 7, genbuf);

  newboard.postotal=500;
  itoa(newboard.postotal, &number);
  getdata( 9, 60, "¤å³¹¤W­­¡G", genbuf, 5, LCECHO, number);
  if(genbuf[0])
    newboard.postotal = atoi(genbuf);


  setbpath (genbuf, newboard.brdname);
  mkdir (genbuf, 0755);
  if (getbnum (newboard.brdname) > 0)
    {
      pressanykey (err_bid);
      return -1;
    }
  getdata (10, 0, "ªO¥D¦W³æ¡G", newboard.BM, IDLEN * 3 + 3, DOECHO, 0);


  if (HAS_PERM (PERM_SYSOP))
    {
      getdata (11, 0, "¥i¥HZAP(Y/N)¡H", ans, 2, LCECHO, "Y");
      if (*ans == 'n')
	newboard.brdattr |= BRD_NOZAP;
      getdata (11, 30, "±µ¨üBRDMAIL(Y/N) ?", ans, 2, LCECHO, "N");
      if (*ans == 'y')
	newboard.brdattr |= BRD_BRDMAIL;
    }
  newboard.level = 0;
  if (!(newboard.brdattr & BRD_GROUPBOARD || newboard.brdattr & BRD_CLASS))
    {
      getdata (12, 0, "¬O§_¬°­Ó¤HªO(Y/N) ?", ans, 2, LCECHO, "N");
      if (*ans == 'y' || *ans == 'Y')
        newboard.brdattr |= BRD_PERSONAL;
      getdata (12, 30, HAS_PERM (PERM_SYSOP) ?
	       "¬ÝªOÁôÂÃ/¨p¤H/¤½¶}(Y/P/N) ?" : "¬ÝªO¨p¤H/¤½¶}(P/N) ?"
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

      getdata (13, 0, "¦C¤J²Î­p±Æ¦æ(Y/N) ?", ans, 2, LCECHO, "Y");
      if (*ans == 'n')
	newboard.brdattr |= BRD_NOCOUNT;

      if (HAS_PERM (PERM_SYSOP) && !(newboard.brdattr & BRD_HIDE))
	{
	  getdata (13, 30, "³]©wÅª¼gÅv­­(Y/N) ?", ans, 2, LCECHO, "Y");
	  if (*ans == 'y')
	    {
	      getdata (14, 0, "­­¨î [R]¾\\Åª (P)µoªí ?", ans, 2, LCECHO, "P");
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
	strcpy (newboard.BM, "[¥Ø¿ý]");
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
  pressanykey ("·sªO¦¨¥ß");
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
  sprintf(buf,"³]©w¬ÝªO [%s] ªº¤å³¹¤W­­¡G",bh.brdname);
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
  prints ("¬ÝªO¦WºÙ¡G%-15s ªO¥D¦W³æ¡G%s\n¬ÝªO»¡©ú¡G%-49s ¤å³¹¤W­­¡G%d %s\n",
	  bh.brdname, bh.BM, bh.title, bh.postotal,
	  (bh.brdattr & BRD_OUTGOING && bh.brdattr & BRD_INGOING) ? "x2" : "");
  {
    int t = bh.totaltime;
    int day=t/86400, hour=(t/3600)%24, min=(t/60)%60, sec=t%60;
    avg = bh.totalvisit != 0 ? bh.totaltime / bh.totalvisit : 0;
    prints("¶}ªO®É¶¡¡G%s\n", Ctime(&bh.opentime));
    prints("«ô³X¤H¼Æ¡G%-15d«ô³XÁ`®É¡G%d ¤Ñ %d ¤p®É %d ¤À %d ¬í ¥­§¡°±¯d¡G%d ¬í\n",
	    bh.totalvisit, day, hour, min, sec, avg);
  }
  prints ("³Ì·s³X«È¡G%-15s³Ì·s®É¶¡¡G%s\n"
	  ,bh.lastvisit, Ctime (&bh.lastime));
  bperm_msg (&bh);
  prints ("%s³]­­\n¥izap:%s ¦C¤J²Î­p:%s"
          "Âà¥X:%s ¸s²Õ:%s ¨p¤H:%s ÁôÂÃ:%s «H½c:%s Àu¨}:%s ­Ó¤H:%s",
	  bh.level ? "¦³" : "¤£",
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

  getdata (9, 0, "¬ÝªO (D)§R°£ (E)³]©w (B)ªO¥D [Q]¨ú®ø¡H", genbuf, 3, LCECHO, 0);

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
	  outs ("§RªO§¹²¦");
	}
      break;

    case 'e':

      move (10, 0);
      outs ("ª½±µ«ö [Return] ¤£­×§ï¸Ó¶µ³]©w");
      memcpy (&newbh, &bh, sizeof (bh));

      move(11,0);

      if (HAS_PERM (PERM_SYSOP))
	while (classname[classnum] != NULL)
	  outs (classname[classnum++]);
      do
	{
	  getdata (12, 0, "¬ÝªOÃþ§O¡G", genbuf, 5, DOECHO, bh.title);
	  if (HAS_PERM (PERM_SYSOP) ||
	   ((strstr (boardprefix, genbuf) != NULL) && strlen (genbuf) == 4))
	    break;
	  outs ("¥u¯à¶}ÁÒ°Ï¤ºÃþ§Oªºª©");
	}
      while (1);

      if (genbuf[0] >= '1' && genbuf[0] <= '9')
	strncpy (newbh.title, classname[*genbuf - '1'] + 2, 4);
      else if (strlen (genbuf) >= 4)
	strncpy (newbh.title, genbuf, 4);

      while (getdata (12, 20, "·s¬ÝªO¦WºÙ¡G", genbuf, IDLEN + 1, DOECHO,0))
	{
	  if (getbnum (genbuf))
	    pressanykey ("¿ù»~! ªO¦W¹p¦P");
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
  
  getdata (13, 0, "¬ÝªOÄÝ©Ê¡G1.¤@¯ë  2.¤l¿ï³æ  3.¤ÀÃþ¬ÝªO  4.Àu¨}  ", 
           genbuf, 2, LCECHO, 0);

  if(genbuf[0])
  
  switch(*genbuf)
  {
    case '1':
      newbh.brdattr &= ~BRD_CLASS;
      newbh.brdattr &= ~BRD_GROUPBOARD;
      newbh.brdattr &= ~BRD_NOCOUNT;
//      strncpy(newbh.title+5,"¡³",2);
      break;
    case '2':
      newbh.brdattr &= ~BRD_CLASS;
      newbh.brdattr |= BRD_GROUPBOARD;
      newbh.brdattr |= BRD_NOCOUNT;
      strncpy(newbh.title+5,"£U",2);
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
//      strncpy(newbh.title+5,"¡¸",2);
      break;
  }

  if(!(newbh.brdattr & BRD_CLASS) && !(newbh.brdattr & BRD_GROUPBOARD))
  {
//    brdattr='4';
//    if(!(bh.brdattr & BRD_OUTGOING)) brdattr='3';
//    if(!(bh.brdattr & BRD_INGOING)) brdattr='2';
//    if((bh.brdattr & BRD_OUTGOING) && (bh.brdattr & BRD_INGOING))
//       brdattr='1';
          
    getdata (14, 0, "Âà«HÄÝ©Ê¡G1.¯¸¤º  2.Âà¥X  3.Âà¤J  4.Âà¤J»PÂà¥X ", 
             genbuf, 2, LCECHO, 0);
             
    if(genbuf[0])
      switch(*genbuf)
    {
      case '1':
        newbh.brdattr &= ~BRD_OUTGOING;
        newbh.brdattr &= ~BRD_INGOING;
        strncpy(newbh.title+5,"¡³",2);
        break;
      case '2':
        newbh.brdattr |= BRD_OUTGOING;
        newbh.brdattr &= ~BRD_INGOING;
        strncpy(newbh.title+5,"¡ó",2);
        break;
      case '3':
        newbh.brdattr |= BRD_INGOING;
        newbh.brdattr &= ~BRD_OUTGOING;
        strncpy(newbh.title+5,"¡·",2);
        break;
      case '4':
        newbh.brdattr |= BRD_OUTGOING;
        newbh.brdattr |= BRD_INGOING;
        strncpy(newbh.title+5,"¡´",2);
        break;
    }
  }

      getdata (15, 0, "¬ÝªO¥DÃD¡G", genbuf, BTLEN + 1, DOECHO, bh.title + 7);
      if (genbuf[0])
	  strcpy (newbh.title + 7, genbuf);

      itoa(bh.postotal, &number);
      getdata( 15, 60, "¤å³¹¤W­­¡G", genbuf, 5, LCECHO, number);
      if(genbuf[0])
         newbh.postotal = atoi(genbuf);

      if (getdata (16, 0, "·sªO¥D¦W³æ¡G", genbuf, IDLEN * 3 + 3,
		   DOECHO, bh.BM))
	{
	  str_trim (genbuf);
	  strcpy (newbh.BM, genbuf);
	}
      if (HAS_PERM (PERM_SYSOP))
	{
	  getdata (17, 0, "¥i¥HZAP(Y/N) ?", genbuf, 2, LCECHO,
		   newbh.brdattr & BRD_NOZAP ? "N" : "Y");
	  if (*genbuf == 'n')
	    newbh.brdattr |= BRD_NOZAP;
	  else
	    newbh.brdattr &= ~BRD_NOZAP;
	  getdata (17, 30, "±µ¨üBRDMAIL(Y/N) ?", genbuf, 2, LCECHO,
		   newbh.brdattr & BRD_BRDMAIL ? "Y" : "N");
	  if (*genbuf == 'y')
	    newbh.brdattr |= BRD_BRDMAIL;
	  else
	    newbh.brdattr &= ~BRD_BRDMAIL;

	}
      if (!(newbh.brdattr & BRD_GROUPBOARD || newbh.brdattr & BRD_CLASS))
	{
	  char ans[4];
          getdata (18, 0, "¬O§_¬°­Ó¤HªO (Y/N) ?", ans, 2, LCECHO, bh.brdattr & BRD_PERSONAL ? "y" : "n");
          if (*ans == 'y' || *ans == 'Y')
            newbh.brdattr |= BRD_PERSONAL;
          else
            newbh.brdattr &= ~BRD_PERSONAL;
	  getdata (18, 30, HAS_PERM (PERM_SYSOP) ? "¬ÝªOÁôÂÃ/¨p¤H/¤½¶}(Y/P/N) ?"
		   : "¬ÝªO¨p¤H/¤½¶}(P/N) ?", ans, 2, LCECHO,
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

	  getdata (19, 0, "¦C¤J²Î­p±Æ¦æ(Y/N) ?", genbuf, 2, LCECHO,
		   newbh.brdattr & BRD_NOCOUNT ? "N" : "Y");
	  if (*genbuf == 'n')
	    newbh.brdattr |= BRD_NOCOUNT;
	  else
	    newbh.brdattr &= ~BRD_NOCOUNT;
	  if (HAS_PERM (PERM_SYSOP) && !(newbh.brdattr & BRD_HIDE))
	    {
	      getdata (19, 30, "¬O§_§ó§ï¦s¨úÅv­­(Y/N) ?", genbuf, 2,
		       LCECHO, "N");
	      if (*genbuf == 'y')
		{
		  getdata (20, 0, "­­¨î (R)¾\\Åª (P)µoªí ?", genbuf, 2, LCECHO,
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
      if (getdata (11, 0, "·sªO¥D¦W³æ¡G", genbuf, IDLEN * 3 + 3, DOECHO, bh.BM))
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
  stand_title ("¬ÝªO³]©w");
  make_blist ();
  namecomplete (msg_bid, bname);
  if (!*bname)
    return 0;
  m_mod_board (bname);
}

/* ----------------------------------------------------- */
/* ¨Ï¥ÎªÌºÞ²z                                            */
/* ----------------------------------------------------- */

int
m_user ()
{
  userec muser;
  int id;
  char genbuf[200];

  stand_title ("¨Ï¥ÎªÌ³]©w");
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
/* ²M°£¨Ï¥ÎªÌ«H½c                                        */
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
  fprintf (cleanlog, "%s¡G", urec->userid);
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
      outs ("²M°£§¹¦¨! °O¿ýÀÉ mailclean.log.");
    }
  return XEASY;
}
#endif /* HAVE_MAILCLEAN */

/* ----------------------------------------------------- */
/* ³B²z Register Form                                    */
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
  {"±b¸¹½s¸¹", "¥Ó½Ð¥N¸¹", "¯u¹ê©m¦W", "±q¦ó³B±oª¾",
   "¹q¤l¶l¥ó«H½c", "¥Ø«e¦í§}", "³sµ¸¹q¸Ü", "ªA°È³æ¦ì", "¶ñªÅ¦ì", NULL};
  static char *reason[] =
  {"¿é¤J¯u¹ê©m¦W", "¸Ô¶ñ¾Ç®Õ¬ì¨t»P¦~¯Å",
   "¶ñ¼g§¹¾ãªº¦í§}¸ê®Æ", "¸Ô¶ñ³sµ¸¹q¸Ü", "½T¹ê¶ñ¼gµù¥U¥Ó½Ðªí",
   "¥Î¤¤¤å¶ñ¼g¥Ó½Ð³æ", "½Ð¶ñ¼g±q¦ó³B±oª¾¥»¯¸", NULL};

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
      pressanykey ("¨ä¥L SYSOP ¤]¦b¼f®Öµù¥U¥Ó½Ð³æ");
      return -1;
    }
  f_mv (regfile, fname);
  if ((fn = fopen (fname, "r")) == NULL)
    {
      pressanykey ("¨t²Î¿ù»~¡AµLªkÅª¨úµù¥U¸ê®ÆÀÉ: %s", fname);
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
	  pressanykey ("¨t²Î¿ù»~¡A¬dµL¦¹¤H");
	}
      else
	{
	  memcpy (&muser, &xuser, sizeof (muser));
	  move (1, 0);
	  prints ("±b¸¹¦ì¸m    ¡G%d\n", unum);
	  user_display (&muser, 1);
	  move (13, 0);
	  prints ("[1;32m---- ½Ð¯¸ªøÄY®æ¼f®Ö¨Ï¥ÎªÌ¸ê®Æ - ³o¬O¸Ó User ²Ä %d ¦¸µù¥U ---[m\n", muser.rtimes);
	  for (n = 0; field[n]; n++)
	    prints ("%-12s¡G%s\n", finfo[n], fdata[n]);
	  if (muser.userlevel & PERM_LOGINOK)
	    {
	      getdata (b_lines - 1, 0, "¦¹±b¸¹¤w¸g§¹¦¨µù¥U, §ó·s(Y/N/Skip)¡H[N] ",
		       ans, 3, LCECHO, "Y");
	      if (ans[0] != 'y' && ans[0] != 's')
		ans[0] = 'd';
	    }
	  else
	    getdata (b_lines - 1, 0, "¬O§_±µ¨ü¦¹¸ê®Æ(Y/N/Q/Del/Skip)¡H[S] ",
		     ans, 3, LCECHO, "S");
	  move (2, 0);
	  clrtobot ();

	  switch (ans[0])
	    {
	    case 'y':

	      prints ("¥H¤U¨Ï¥ÎªÌ¸ê®Æ¤w¸g§ó·s:\n");
	      mail2user (muser,"[µù¥U¦¨¥\\Åo]",BBSHOME"/etc/registered");
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

	    case 'q':		/* ¤Ó²Ö¤F¡Aµ²§ô¥ð®§ */

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
	      prints ("½Ð´£¥X°h¦^¥Ó½Ðªí­ì¦]¡A«ö <enter> ¨ú®ø\n");
	      for (n = 0; reason[n]; n++)
		prints ("%d) ½Ð%s\n", n, reason[n]);

/*
   woju
 */
	      if (getdata (10 + n, 0, "°h¦^­ì¦]¡G", buf, 60, DOECHO, 0))
		{
		  int i;
		  fileheader mhdr;
		  char title[128], buf1[80];
		  FILE *fp;

		  i = buf[0] - '0';
		  if (i >= 0 && i < n)
		    strcpy (buf, reason[i]);
		  sprintf (genbuf, "[°h¦^­ì¦]] ½Ð%s", buf);

		  //sethomepath (buf1, muser.userid);
		  sprintf(buf1, "home/%s/mailbox", muser.userid);
		  stampfile (buf1, &mhdr);
		  strcpy (mhdr.owner, cuser.userid);
		  strncpy (mhdr.title, "[µù¥U¥¢±Ñ]", TTLEN);
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
	      prints ("¨ú®ø°h¦^¦¹µù¥U¥Ó½Ðªí");

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
      outmsg ("¥Ø«e¨ÃµL·sµù¥U¸ê®Æ");
      return XEASY;
    }

  stand_title ("¼f®Ö¨Ï¥ÎªÌµù¥U¸ê®Æ");
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
  getdata (b_lines - 1, 0, "¶}©l¼f®Ö¶Ü(Y/N)¡H[Y] ", ans, 3, LCECHO, "Y");
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
    sprintf(buf,"´M§ä·l·´id\n²Ä [%d] µ§¸ê®Æ\n",coun);
    outs(buf);
    refresh();
    if(bad_user_id(user.userid))
    {
      user_display(&user, 1);
      uinfo_query(&user, 1, coun);
      outs("[44m               ¥ô¤@Áä[37m:·j´M¤U¤@­Ó          [33m Q[37m: Â÷¶}                         [m");
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
  getdata (0, 0, "½Ð¿é¤J¨Ï¥ÎªÌÃöÁä¦r [©m¦W|email|ID|¹q¸Ü|¦a§}]:", key, 21, DOECHO, 0);
  while ((fread (&user, sizeof (user), 1, fp1)) > 0)
    {
      coun++;
      move (1, 0);
      sprintf (buf, "²Ä [%d] µ§¸ê®Æ\n", coun);
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
"[1;44;33m       ¥ô¤@Áä[37m:·j´M¤U¤@­Ó          [33m Q[37m: Â÷¶}                        [m ");
	  ch = igetch ();
	  if (ch == 'q' || ch == 'Q')
	    return 0;
	  clear ();
	  move (0, 0);
	  outs ("½Ð¿é¤J¨Ï¥ÎªÌÃöÁä¦r [©m¦W|email|ID|¹q¸Ü|¦a§}]:");
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
   usercomplete("¿é¤J¹ï¤èªºID¡G", id);
   if (!id[0] || !getdata(14, 0, "­nµo¦h¤Ö¿ú¼úª÷¡H", buf, 5, LCECHO,0)) return;
   money = atoi(buf);
   if(money > 0 && (inugold(id, money) != -1))
   {
     sprintf(buf,"§@ªÌ: %s \n"
                 "¼ÐÃD:[µo©ñ¼úª÷] µoµ¹§A %d ª÷¹ô­ò¡I\n"
                 "®É¶¡: %s\n",cuser.userid,money,ctime(&now));
     fputs(buf,fp);
     while(!getdata(15,0,"½Ð¿é¤J²z¥Ñ¡G",reason,60,DOECHO ,"¦¬¿ý xxx ªOºëµØ°Ï"));
     sprintf(buf,"[1;32m%s[37m µoµ¹§A [33m%d [37m¤¸ª÷¹ô¡C\n"
                 "¥Lªº²z¥Ñ¬O¡G[33m %s [m",cuser.userid,money,reason);
     fputs(buf,fp);
     fclose(fp);
     sprintf(buf,"home/%s", id);
     stampfile(buf, &mymail);
     strcpy(mymail.owner, cuser.userid);
     rename ("tmp/givebonus",buf);
     sprintf(mymail.title,"[µo©ñ¼úª÷] °e§A %d ¤¸ª÷¹ô­ò¡I",money);
     sprintf(buf,"home/%s/.DIR",id);
     rec_add(buf, &mymail, sizeof(mymail));
     sprintf(buf,"[1;33m%s %s [37m§âª÷¹ô [33m%d ¤¸ [37mµoµ¹[33m %s[37m",
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
