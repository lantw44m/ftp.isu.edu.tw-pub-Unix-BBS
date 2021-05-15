/*-------------------------------------------------------*/
/* user.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* author : opus.bbs@bbs.cs.nthu.edu.tw                  */
/* target : user configurable setting routines           */
/* create : 95/03/29                                     */
/* update : 02/08/24 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"
#define UPDATE_USEREC   (currmode |= MODE_DIRTY)

extern item_update_passwd(char);

extern int numboards;
extern boardheader *bcache;
extern void resolve_boards();
extern int nitoa3(int, char *);

#ifdef RPG_STATUS
extern char itemtable1[40][20];
extern char itemtable2[40][20];
extern char itemtable3[100][25];
#endif

char *sex[8] = { MSG_BIG_BOY, MSG_BIG_GIRL, MSG_LITTLE_BOY, MSG_LITTLE_GIRL,
                 MSG_MAN, MSG_WOMAN, MSG_PLANT, MSG_MIME };

void user_display(userec *u, int real) {
  char ci;
  int diff;
  char genbuf[200];

  clrtobot();
  prints(
    "¥N¸¹        ¡G%s\n"
    "¼ÊºÙ        ¡G%s\n",
    u->userid,
    u->username);
  if(HAS_PERM(PERM_SYSOP) || !strcmp(cuser.userid, u->userid))
     prints(
    "¯u¹ê©m¦W    ¡G%s\n"
    "©~¦í¦í§}    ¡G%s\n"
    "¹q¤l¶l¥ó«H½c¡G%s\n"
#ifdef RPG_SETTING
    "ºØ±Ú        ¡G%s\n"
#endif
#ifndef MIN_USEREC_STRUCT
    "©Ê§O        ¡G%s\n"
#endif
    "¥Í¤é        ¡G%02i/%02i/%02i\n",
    u->realname,
    u->address,
    u->email,
#ifdef RPG_SETTING
    sex[(u->sex < sizeof(sex) / sizeof(*sex)) ? u->sex: 0],
#endif
#ifndef MIN_USEREC_STRUCT
    u->classsex == 3 ? "¤£¸Ô" : u->classsex == 2 ? "¤k" : u->classsex == 1 ?
    "¨k" : "¥¼©w¸q",
#endif
    u->month, u->day, u->year);

  prints("µù¥U¤é´Á    ¡G%s", ctime(&u->firstlogin));

  prints("³Ìªñ¥úÁ{¤é´Á¡G%s",
         strcmp(u->userid, "Kaoru") ? ctime(&u->lastlogin) : "¨t²Î¤£Åã¥Ü\n");

  if (real) prints("³Ìªñ¥úÁ{¾÷¾¹¡G%s\n", u->lasthost);

  prints("¤W¯¸¦¸¼Æ    ¡G%d ¦¸\n", u->numlogins);

  prints("¤å³¹¼Æ¥Ø    ¡G%d ½g\n", u->numposts);

  sethomedir(genbuf, u->userid);
  if(HAS_PERM(PERM_SYSOP))
  prints("¨p¤H«H½c    ¡G%d «Ê\n", get_num_records(genbuf, sizeof(fileheader)));

  if (real)
  {
#ifndef NO_USE_MULTI_STATION
    access_mask('l', &u->userlevel, u->userid);
#endif
    strcpy(genbuf, "bTCPRp#@XWBA#VSMtN12abcdefghijkl");
    for (diff = 0; diff < NUMPERMS ; diff++)
      if (!(u->userlevel & (1 << diff)))
        genbuf[diff] = '-';
    if(HAS_PERM(PERM_SYSOP))
       prints("»{ÃÒ¸ê®Æ    ¡G%s\n¨Ï¥ÎªÌÅv­­  ¡G%s\n", u->justify, genbuf);
  }
  else
  {
    diff = (time(0) - login_start_time) / 60;
    prints("°±¯d´Á¶¡    ¡G%d ¤p®É %2d ¤À\n", diff / 60, diff % 60);
  }

  // Thor: ·Q¬Ý¬Ý³o­Ó user ¬O¨º¨Çª©ªºª©¥D
  if (u->userlevel >= PERM_BM)
  {
    int i, len, ch;
    boardheader *bhdr;
    char *list;
    resolve_boards();

    len = strlen(u->userid);

    outs("¾á¥ôªO¥D    ¡G");

    for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
    {
      list = bhdr->BM;
      ch = list[0];
      if ((ch > ' ') && (ch < 128))
      {
        do
        {
          if (!ci_strncmp(list, u->userid, len))
          {
            ch = list[len];
            if ((ch == 0) || (ch == '/'))
            {
              outs(bhdr->brdname);
              outc(' ');
              break;
            }
          }
          while (ch = *list++)
            if (ch == '/')
              break;
        } while (ch);
      }
    }
    outc('\n');
  }

#ifndef NO_USE_MULTI_STATION
  prints("¦¨­ûºX¼Ð    ¡G");
  for(ci = 0 ; ci < 25 ; ci++)
     if(HAS_PERM(PERM_SYSOP) || ci == cuser.now_stno)
        prints("%c", u->station_member[ci] ? 'X' : '-');
  outc('\n');
#endif
  outs((u->userlevel & PERM_LOGINOK) ?
    "\n±zªºµù¥Uµ{§Ç¤w¸g§¹¦¨¡AÅwªï¥[¤J¥»¯¸" :
    "\n¦pªG­n´£ª@Åv­­¡A½Ð°Ñ¦Ò¥»¯¸¤½§GÄæ¿ì²zµù¥U");

#ifdef  NEWUSER_LIMIT
  if ((u->lastlogin - u->firstlogin < 3 * 86400) && !HAS_PERM(PERM_POST))
    outs("\n·s¤â¤W¸ô¡A¤T¤Ñ«á¶}©ñÅv­­");
#endif
}

void uinfo_query(userec *u, int real, int unum) {
  userec x;
  register int j, k, fail, mail_changed;
  register usint i;
  char ans[4], buf[STRLEN];
  char genbuf[200];
  FILE *fp;

  char *genpasswd();

  fail = mail_changed = 0;
  memcpy(&x, u, sizeof(userec));

#ifndef NO_USE_MULTI_STATION
  if(!HAS_PERM(PERM_SYSOP) && HAS_PERM(PERM_BOARD) && is_station_sysop()) {
     if(!cuser.extra_mode[2])
        getdata(b_lines - 1, 0,
        "(3)³]©wÅv­­ (M)¥[¤J¦¨­û (D)°£¥h¦¨­û (S)·s¼W¯¸ºÞ [0]µ²§ô ",
        ans, 3, DOECHO, 0);
     else
        goto OKOK_01;
  }
  else
#endif
  {
OKOK_01:
#ifdef RPG_SETTING
     if(real && !strcmp(cuser.userid, SUPERVISOR)) {
        move(b_lines - 2, 0);
        outs(
     "(6)ª÷¿úÅÜ§ó/¹D¨ã·s¼W (7)§R°£¹D¨ã (8)¸Ë³ÆªZ¾¹/±¡¤HÅÜ§ó (9)ÅÜ§óÂ¾·~¯à¤O");
     }
#endif
     getdata(b_lines - 1, 0, real ?
  "(1)­×§ï¸ê®Æ (2)³]©w±K½X (3)³]©wÅv­­ (4)²M°£±b¸¹ (5)§ïID "
#ifdef CHANGE_USER_MAIL_LIMIT
  "(L)«H½c "
#endif
  "[0]µ²§ô " : "½Ð¿ï¾Ü (1)­×§ï¸ê®Æ (2)³]©w±K½X ==> [0]µ²§ô ",
       ans, 3, LCECHO, 0);
  }

  if(ans[0] > '2' && !real)
    ans[0] = '0';
#ifndef NO_USE_MULTI_STATION
  else {
     if(is_station_sysop() && !HAS_PERM(PERM_SYSOP) &&
        !(ans[0] == '3' || ans[0] == 'm' || ans[0] == 'd' || ans[0] == 's')) {
        if(!cuser.extra_mode[2]) ans[0] = '0';  /* Dopin: ²¾¨ì¦¹¬O²Î¤@®æ¦¡ */
     }
  }
#endif

  if (ans[0] == '1' || ans[0] == '3') {
    clear();
    i = 2;
    move(i++, 0);
    outs(msg_uid);
    outs(x.userid);
  }

  switch (ans[0])
  {
  case '1':
    if(!cuser.extra_mode[2] && !HAS_PERM(PERM_SYSOP)) break;

    move(0, 0);
    outs("½Ð³v¶µ­×§ï¡C");

    if (getdata(i++, 0, "¼ÊºÙ¡G", buf, 24, DOECHO, u->username))
      strcpy(x.username, buf);

    if (getdata(i++, 0, "¯u¹ê©m¦W¡G", buf, 20, DOECHO, u->realname))
      strcpy(x.realname, buf);

    if (getdata(i++, 0,  "©~¦í¦a§}¡G", buf, 50, DOECHO, u->address))
      strcpy(x.address, buf);

#ifndef MIN_USEREC_STRUCT
    i++;
CSEX_OK1:
    getdata(i, 0, "©Ê§O¡G(1)¨k©Ê (2)¤k©Ê (3)¤£¸Ô ", buf, 2, DOECHO, 0);
    j = atoi(buf);
    if(!real) if(j > 3 || j < 1) goto CSEX_OK1;  /* °²©w¯¸ºÞ¤£·|§ï¿ù */
    if(j) x.classsex = (char)j;
#endif

#ifdef RPG_SETTING
    if(real) {
      sprintf(genbuf,
      "ºØ±Ú¡G(1)¤HÃþ (2)Ã~±Ú (3)§¯ºë (4)¦aÆF (5)¥¼ª¾ (6)Às±Ú (7)¯«±Ú (8)Å]±Ú "
      "[%i]¡G", u->sex + 1);

      getdata(i++, 0, genbuf, buf, 3, DOECHO, 0);
      if(buf[0] >= '1' && buf[0] <= '8') x.sex = buf[0] - '1';
       else x.sex = u->sex;
    }
#endif

    while (1 && real) {
      int len;

      sprintf(genbuf, "%02i/%02i/%02i",
        u->month, u->day, u->year);
      len = getdata(i, 0, "¥Í¤é ¤ë¤ë/¤é¤é/¦è¤¸¡G", buf, 9, DOECHO, genbuf);
      if (len && len != 8)
        continue;
      if (!len)
      {
        x.month = u->month;
        x.day   = u->day;
        x.year  = u->year;
      }
      else if (len == 8)
      {
        x.month = (buf[0] - '0') * 10 + (buf[1] - '0');
        x.day   = (buf[3] - '0') * 10 + (buf[4] - '0');
        x.year  = (buf[6] - '0') * 10 + (buf[7] - '0');
      }
      else
        continue;
      if (!real && (x.month > 12 || x.month < 1 ||
        x.day > 31 || x.day < 1 || x.year > 90 || x.year < 40))
        continue;
      i++;
      break;
    }

    getdata(i++, 0, "¹q¤l«H½c(*±H»{ÃÒ«H¥Î*)¡G", buf, 50, DOECHO, u->email);
    if (*buf && strcmp(buf, u->email)) {
      strcpy(x.email, buf);
      mail_changed = 1 - real;
    }
    else
       strcpy(x.email, u->email);

    if (real)
    {
      if (getdata(i++, 0, "»{ÃÒ¸ê®Æ¡G", buf, 44, DOECHO, u->justify))
        strcpy(x.justify, buf);

      if (getdata(i++, 0, "³Ìªñ¥úÁ{¾÷¾¹¡G", buf, 64, DOECHO, u->lasthost))
        strcpy(x.lasthost, buf);

      sprintf(genbuf, "%d", u->numlogins);
      if (getdata(i++, 0, "¤W½u¦¸¼Æ¡G", buf, 10, DOECHO, genbuf))
        if ((fail = atoi(buf)) >= 0)
          x.numlogins = fail;

      sprintf(genbuf, "%d", u->numposts);
      if (getdata(i++, 0, "¤å³¹¼Æ¥Ø¡G", buf, 10, DOECHO, genbuf))
        if ((fail = atoi(buf)) >= 0)
          x.numposts = fail;

      fail = 0;
    }
    break;

  case '2':
    if(!cuser.extra_mode[2] && !HAS_PERM(PERM_SYSOP)
#ifdef USE_ACCOUNTS_PERM
       && !HAS_PERM(PERM_ACCOUNTS)
#endif
    ) break;

    i = 19;
    if (!real)
    {
      if (!getdata(i++, 0, "½Ð¿é¤J­ì±K½X¡G", buf, PASSLEN, NOECHO, 0) ||
        !checkpasswd(u->passwd, buf))
      {
        outs("\n\n±z¿é¤Jªº±K½X¤£¥¿½T\n");
        fail++;
        break;
      }
    }

    if (!getdata(i++, 0, "½Ð³]©w·s±K½X¡G", buf, PASSLEN, NOECHO, 0))
    {
      outs("\n\n±K½X³]©w¨ú®ø, Ä~Äò¨Ï¥ÎÂÂ±K½X\n");
      fail++;
      break;
    }
    strncpy(genbuf, buf, PASSLEN);

    getdata(i++, 0, "½ÐÀË¬d·s±K½X¡G", buf, PASSLEN, NOECHO, 0);
    if (strncmp(buf, genbuf, PASSLEN))
    {
      outs("\n\n·s±K½X½T»{¥¢±Ñ, µLªk³]©w·s±K½X\n");
      fail++;
      break;
    }
    buf[8] = '\0';
    strncpy(x.passwd, genpasswd(buf), PASSLEN);
    break;

  case '3':
#ifndef NO_USE_MULTI_STATION
    cuser.extra_mode[4] = 1;
#endif
    i = setperms(x.userlevel, x.userid);
    if (i == x.userlevel)
      fail++;
    else
      x.userlevel = i;
    break;

  case '4':
    if(!HAS_PERM(PERM_SYSOP)) break;

    i = QUIT;
    break;

  case '5':
    if(!HAS_PERM(PERM_SYSOP)
#ifdef USE_ACCOUNTS_PERM
       && !HAS_PERM(PERM_ACCOUNTS)
#endif
    ) break;

    if (getdata(b_lines - 3, 0, "·sªº¨Ï¥ÎªÌ¥N¸¹¡G", genbuf, IDLEN + 1, DOECHO, 0))
    {
      if (searchuser(genbuf))
      {
        outs("¿ù»~! ¤w¸g¦³¦P¼Ë ID ªº¨Ï¥ÎªÌ");
        fail++;
      }
      else
      {
        strcpy(x.userid, genbuf);
      }
    }
    break;

#ifdef RPG_SETTING
  case '6':
    if(strcmp(cuser.userid, SUPERVISOR))
       goto ADD_Fault;

    clear();
    move(1, 0);
    prints("¨Ï¥ÎªÌ­ì¦³ª÷¿ú¼Æ : %d", x.havemoney);
    getdata(2, 0, "½Ð¿é¤Jª÷¿ú¼Æ ", buf, 10, DOECHO, 0);
    x.havemoney += atoi(buf);
    prints("¨Ï¥ÎªÌ¥[ºâª÷¿ú¼Æ : %d", x.havemoney);

    move(7, 0);
    prints("¨Ï¥ÎªÌ­ì¦³©Ê§O¸¹½X : %d", x.classsex);
    getdata(8, 0, "½Ð¿é¤J©Ê§O¸¹½X 0 µL 1 ¨k 2 ¤k", buf, 2, DOECHO, 0);
    x.classsex = atoi(genbuf);

    move(10, 0);
    prints("¨Ï¥ÎªÌ­ì¦³¹D¨ã¼Æ : %d", x.havetoolsnumber);
    getdata(11, 0, "½Ð¿é¤J­n·s¼Wªº¹D¨ã¸¹½X : ", buf, 5, DOECHO, 0);
    if(x.havetoolsnumber >= 20)
       goto ADD_Fault;
    if(atoi(buf) > 0) {
       x.havetools[x.havetoolsnumber] = atoi(buf);
       x.havetoolsnumber++;
    }

ADD_Fault:
    break;

  case '7':
    if(strcmp(cuser.userid, SUPERVISOR))
       break;
KEEP_DEL_ITEM:
    clear();
    move(1, 0);
    for(k=0 ; k < x.havetoolsnumber && k < 20 ; k++) {
       prints("(%2d) ", k+1);
    if(x.havetools[k] <= 40)
       prints("%s\n", itemtable1[x.havetools[k]-1]);
    else
       if(x.havetools[k] <= 80)
          prints("%s\n", itemtable2[x.havetools[k]-41]);
       else
          prints("%s\n", itemtable3[x.havetools[k]-81]);
    }

    getdata(22, 0, "½Ð¿é¤J±ý§R°£¹D¨ã½s¸¹", buf, 3, DOECHO, 0);
    j = atoi(buf);

    if(j > x.havetoolsnumber || j < 1)
       break;

    for(k = j-1 ; k < x.havetoolsnumber-1 && k < 19 ; k++)
       x.havetools[k] = x.havetools[k+1];
    x.havetools[k] = 0;
    x.havetoolsnumber--;

    clear();
    move(1, 0);
    for(k=0 ; k < x.havetoolsnumber && k < 20 ; k++) {
       prints("(%2d) ", k+1);
       if(x.havetools[k] <= 40)
          prints("%s\n", itemtable1[x.havetools[k]-1]);
       else
          if(x.havetools[k] <= 80)
             prints("%s\n", itemtable2[x.havetools[k]-41]);
          else
             prints("%s\n", itemtable3[x.havetools[k]-81]);
    }

    getdata(22, 0, "Ä~Äò§R°£ (y/N) ? ", buf, 2, LCECHO, 0);
    if(buf[0] == 'y') goto KEEP_DEL_ITEM;
    break;

  case '8':
    if(strcmp(cuser.userid, SUPERVISOR)) break;

    clear();
    getdata(b_lines -18, 0, "¸Ë³ÆªZ¾¹¸¹½X :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnlv[0][0] = k;
    getdata(b_lines -16, 0, "¸Ë³ÆªZ¾¹°Ñ¼Æ :  ", buf, 4, DOECHO, 0);
    x.curwepnlv[1][0] = buf[0];

    getdata(b_lines -14, 0, "¸Ë³ÆªZ¾¹¥[¤@ :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[0] = k;
    getdata(b_lines -12, 0, "¸Ë³ÆªZ¾¹¥[¤G :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[1] = k;
    getdata(b_lines -10, 0, "¸Ë³ÆªZ¾¹¥[¤T :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[2] = k;
    getdata(b_lines - 8, 0, "¸Ë³ÆªZ¾¹¥[¥| :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[3] = k;

    getdata(b_lines - 6, 0, "±¡¤H :  ", genbuf, IDLEN+1, DOECHO, 0);
    strcpy(x.lover, genbuf);
    getdata(b_lines - 4, 0, "«ü´§©x :  ", buf, 4, DOECHO, 0);
    x.commander = atoi(buf);
    break;

  case '9':
    if(strcmp(cuser.userid, SUPERVISOR))
       break;
    clear();
    getdata(1, 0, "Â¾·~¦W :  ", genbuf, 20, DOECHO, 0);
    strcpy(x.working, genbuf);
    getdata(2, 0, "Â¾·~ÀÉ¦W(¦¹ª©¥»µL³]©w) :  ", genbuf, 7, DOECHO, 0);
    strcpy(x.class, genbuf);
    getdata(3, 0, "LV :  ", buf, 4, DOECHO, 0);
    x.nowlevel = atoi(buf);
    getdata(4, 0, "HP :  ", buf, 4, DOECHO, 0);
    x.hp = atoi(buf);
    getdata(5, 0, "STR :  ", buf, 4, DOECHO, 0);
    x.str = atoi(buf);
    getdata(6, 0, "MGC :  ", buf, 4, DOECHO, 0);
    x.mgc = atoi(buf);
    getdata(7, 0, "SKL :  ", buf, 4, DOECHO, 0);
    x.skl = atoi(buf);
    getdata(8, 0, "SPD :  ", buf, 4, DOECHO, 0);
    x.spd = atoi(buf);
    getdata(9, 0, "LUK :  ", buf, 4, DOECHO, 0);
    x.luk = atoi(buf);
    getdata(10, 0, "DEF :  ", buf, 4, DOECHO, 0);
    x.def = atoi(buf);
    getdata(11, 0, "MDF :  ", buf, 4, DOECHO, 0);
    x.mdf = atoi(buf);
    getdata(12, 0, "Â¾·~§Þ¯à¤@ :  ", buf, 3, DOECHO, 0);
    x.spcskl[0] = atoi(buf);
    getdata(13, 0, "Â¾·~§Þ¯à¤G :  ", buf, 3, DOECHO, 0);
    x.spcskl[1] = atoi(buf);
    getdata(14, 0, "Â¾·~§Þ¯à¤T :  ", buf, 3, DOECHO, 0);
    x.spcskl[2] = atoi(buf);
    getdata(15, 0, "Â¾·~§Þ¯à¥| :  ", buf, 3, DOECHO, 0);
    x.spcskl[3] = atoi(buf);
    getdata(16, 0, "Â¾·~§Þ¯à¤­ :  ", buf, 3, DOECHO, 0);
    x.spcskl[4] = atoi(buf);
    getdata(17, 0, "Â¾·~§Þ¯à¤» :  ", buf, 3, DOECHO, 0);
    x.spcskl[5] = atoi(buf);
    getdata(18, 0, "­×¥¿¸gÅç­È :  ", genbuf, 7, DOECHO, 0);
    x.addexp = atoi(genbuf);
    getdata(19, 0, "ªZ¾¹¯à¤O :  ", genbuf, 11, DOECHO, 0);
    for(i=0 ; i<10 ; i++) {
       x.wepnlv[0][i] = genbuf[i];
       x.wepnlv[1][i] = 0;
    }
    break;
#endif

#ifndef NO_USE_MULTI_STATION
  case 'm':
     if(!HAS_PERM(PERM_SYSOP) && is_station_sysop()) {
        x.station_member[cuser.now_stno] = 1;
        break;
     }

  case 'd':
     if(!HAS_PERM(PERM_SYSOP) && is_station_sysop()) {
        x.station_member[cuser.now_stno] = 0;
        break;
     }

  case 's':
     if(!HAS_PERM(PERM_SYSOP) && is_station_sysop()
        && strcmp(cuser.station, DEFAULTST)) {
        move(b_lines, 0);

        sprintf(genbuf, BBSHOME"/run/%s.sysop", cuser.station);
        fp = fopen(genbuf, "r");
        if(fp == NULL)
           fp = fopen(genbuf, "w+");

        fgets(genbuf, 3, fp);
        k = atoi(genbuf);
        if(k > 4) {
           pressanykey("µLªk¦A·s¼W,¯¸ºÞ¼Æ¥Ø¤w¶W¹L¥|¦ì");
           fclose(fp);
           break;
        }
        else {
           k++;
           fclose(fp);
           sprintf(genbuf, BBSHOME"/run/%s.sysop", cuser.station);
           fp = fopen(genbuf, "w+");
           fprintf(fp, "%d", k);
           fclose(fp);
           sprintf(genbuf, BBSHOME"/run/%s.sysop_name", cuser.station);
           fp = fopen(genbuf, "a+");
           fprintf(fp, "%s\n", x.userid);
           fclose(fp);
           pressanykey("¯¸ºÞÀÉ®×¤w§ó·s,½Ð°È¥²«ö Y ½T©w,¥H§K¸ê®Æ¤£¦P¨B");
        }

        x.userlevel |= PERM_BOARD;
        break;
     }
#endif
#ifdef CHANGE_USER_MAIL_LIMIT
  /* Dopin 11/20/02 ÅÜ§ó«H½cµ§¼Æ»P®e¶q¤j¤p¤§¤W­­ */
  case 'l':
     if(!HAS_PERM(PERM_SYSOP) /* ¤º³] ¯¸ªø »P ±b¸¹Á`ºÞ ¥iÅÜ§ó¨Ï¥ÎªÌ«H½c¤j¤p */
#ifdef USE_ACCOUNTS_PERM      /* ¦pªG¤£µ¹±b¸¹Á`ºÞ³] ³o¤T¦æ¤£¥²¥[ */
        && !HAS_PERM(PERM_ACCOUNTS)
#endif
     ) break;
     eolrange(b_lines-6, b_lines);
     strcpy(buf, x.userlevel & PERM_BM ? "300" : x.userlevel & PERM_LOGINOK ?
                 "150" : "50");
     sprintf(&buf[10], "%d",
             x.max_mail_number ? x.max_mail_number : MAXKEEPMAIL);

     move(b_lines-5, 0);
     prints("­ì¨Ï¥ÎªÌ«H¥ó¼Æ¶q¤W­­ %s «Ê", &buf[10]);

     getdata(b_lines-4, 0, "·sªº¤W­­¼Æ¥Ø : " , &buf[20], 5, DOECHO, &buf[10]);
     k = atoi(&buf[20]);
     if(k > 0) x.max_mail_number = k;
     else {
        pressanykey("«H¥ó¼Æ¥Ø»Ý¬°¥¿¾ã¼Æ ...");
        fail = 1;
     }

     sprintf(&buf[10], "%d",
             x.max_mail_kbytes ? x.max_mail_kbytes : atoi(buf));
     move(b_lines-3, 0);
     prints("­ì¨Ï¥ÎªÌ«H¥ó®e¶q¤W­­ %s Kbytes", &buf[10]);

     getdata(b_lines-2, 0, "·sªº¤W­­®e¶q : " , &buf[20], 5, DOECHO, &buf[10]);
     k = atoi(&buf[20]);
     if(k > 0) x.max_mail_kbytes = k;
     else {
        pressanykey("«H¥ó®e¶q»Ý¬°¥¿¾ã¼Æ ...");
        fail = 1;
     }

     move(b_lines, 0);  /* Dopin: ¯Âºé¬O¬°¤F¦n¬Ý ¥i¥[¥i¤£¥[ */
     break;
#endif

  default:
#ifndef NO_USE_MULTI_STATION
    cuser.extra_mode[4] = 0;
#endif
    return;
  }

  if(fail) return;

  getdata(b_lines - 1, 0, msg_sure_ny, ans, 3, LCECHO, 0);
  if(*ans == 'y') {
#ifndef NO_USE_MULTI_STATION
    if(cuser.extra_mode[4] == 1) access_mask('u', &x.userlevel, x.userid);
#endif
    if (strcmp(u->userid, x.userid))
    {
      char src[STRLEN], dst[STRLEN];

      sethomepath(src, u->userid);
      sethomepath(dst, x.userid);
      Rename(src, dst);
      setuserid(unum, x.userid);
    }
    memcpy(u, &x, sizeof(x));
    if (mail_changed) {
#ifdef  EMAIL_JUSTIFY
      x.userlevel &= ~PERM_LOGINOK;
      mail_justify(x);
#endif
    }

    if (i == QUIT) {
      char src[STRLEN], dst[STRLEN];

      sprintf(src, "home/%s", x.userid);
      sprintf(dst, "tmp/%s", x.userid);
      if (Rename(src, dst))
      {
        sprintf(genbuf, "/bin/rm -fr %s", src);
        system(genbuf);
      }
      /* woju */
      log_usies("KILL", x.userid);
      x.userid[0] = '\0';
      setuserid(unum, x.userid);
    }
    else
       log_usies("SetUser", x.userid);
    substitute_record(fn_passwd, &x, sizeof(x), unum);
  }
#ifndef NO_USE_MULTI_STATION
  cuser.extra_mode[4] = 0;
#endif
}

int u_info() {
  clear();
  move(2, 0);
  cuser.extra_mode[2] = 1;
  user_display(&cuser, 0);
  uinfo_query(&cuser, 0, usernum);

  strcpy(currutmp->realname, cuser.realname);
  strcpy(currutmp->username, cuser.username);
  cuser.extra_mode[2] = 0;
  return 0;
}

int u_ansi() {
  showansi ^= 1;
  cuser.uflag ^= COLOR_FLAG;
  outs(reset_color);
  return 0;
}

int u_cloak()
{
  outs((currutmp->invisible ^= 1) ? MSG_CLOAKED : MSG_UNCLOAK);
  return XEASY;
}

int u_lock() {
  char genbuf[PASSLEN];

  log_usies("LOCK ", "screen");

  do
  {
    getdata(b_lines - 1, 0, "¡¶ ½Ð¿é¤J±K½X¡A¥H¸Ñ°£¿Ã¹õÂê©w¡G",
      genbuf, PASSLEN, NOECHO, 0);
  } while (!checkpasswd(cuser.passwd, genbuf));

  return FULLUPDATE;
}

#ifdef  HAVE_SUICIDE
int u_kill()
{
  char genbuf[200];

  getdata(b_lines - 1, 0, "¡¶ ¦pªG½T©w­n²M°£±zªº±b¸¹¡A½Ð¿é¤J±K½X¡G",
    genbuf, PASSLEN, NOECHO, 0);

  if (*genbuf == '\0' || !checkpasswd(cuser.passwd, genbuf))
  {
    outmsg("±z¿é¤Jªº±K½X¤£¥¿½T¡A±b¸¹¥¼²M°£¡C");
    igetch();
    return FULLUPDATE;
  }

#ifdef  HAVE_REPORT
  report("suicide");
#endif

  sprintf(genbuf, "#%d %s:%s %d %d",
    usernum, cuser.realname, cuser.username, cuser.numlogins, cuser.numposts);
  log_usies("SUCI ", genbuf);

  sprintf(genbuf, "%s %s", fromhost, Cdate(&(cuser.firstlogin)));
  log_usies("SUCI ", genbuf);

  sethomepath(genbuf, cuser.userid);
  if (Rename(genbuf, "tmp"))
  {
    /* woju */
    sprintf(genbuf, "(cd home; tar cvf SUCI_%s.tar %s); /bin/rm -fr home/%s",
            cuser.userid, cuser.userid, cuser.userid);
    system(genbuf);
  }
  cuser.userid[0] = '\0';
  substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);
  setuserid(usernum, cuser.userid);
  purge_utmp(currutmp);

  pressanykey("¿Ë·RªºªB¤Í¡A«C¤sªø¦b¡Aºñ¤ôªø¬y¡A«¥­Ì«á·|¦³´Á§a¡I");
  sleep(1);
  reset_tty();
  exit(0);
}
#endif

void showplans(char *uid) {
  FILE *planfile;
  uschar i, j;
  char genbuf[256];

  sethomefile(genbuf, uid, fn_plans);
  if(planfile = fopen(genbuf, "r")) {

    i = 0, j = 6;
    while(i++ < MAXQUERYLINES && fgets(genbuf, 256, planfile)) {
      out2line(1, j++, kaede_prints(genbuf));
      if(i && !((i + 6) % b_lines)) {
         j = 0;
         if(pressanykey("Áä¤J ¡ö ¤¤Â_¬d¸ß") == KEY_LEFT) break;
         clear();
      }
    }
    fclose(planfile);
  }
}

int showsignature(char *fname) {
  FILE *fp;
  char buf[256];
  int i, j;
  char ch;

  clear();
  move(2, 0);
  setuserfile(fname, "sig.0");
  j = strlen(fname) - 1;

  for (ch = '1'; ch <= '9'; ch++)
  {
    fname[j] = ch;
    if (fp = fopen(fname, "r"))
    {
      prints("[36m¡i Ã±¦WÀÉ.%c ¡j[m\n", ch);
      for (i = 0; i++ < MAXSIGLINES && fgets(buf, 256, fp); outs(buf));
      fclose(fp);
    }
  }
  return j;
}

#ifdef DRAW
int cul_data(void) {
   int i, mun;
   FILE *fp, *fps;
   char user_id[20];
   char genbuf[8];
   char no_a[4];
   userec mu;

   fp = fopen("draft/list1c", "r");
   fps = fopen("draft/list1b", "w+");

/*
   do {
      fgets(genbuf, 80, fp);
      strcpy(user_id, genbuf);

      for(i = 0 ; user_id[i] > ' ' && i < IDLEN+1 && user_id[i] != '\n' ; i++);
      user_id[i] = 0;

      if(mun = getuser(user_id)) {
         memcpy(&mu, &xuser, sizeof(mu));
         nitoa3(mu.luk, no_a);
         fprintf(fps, "%-13.13s %-3.3s\n", user_id, no_a);
      }

   } while(!feof(fp) && !ferror(fp));
*/

   for(i = 0 ; i < 24 ; i++) {
      fgets(genbuf, 80, fp);
      srandom(file_seed(BBSHOME"/src/maple/fpindex"));
      mun = random()%120;
      genbuf[17] = ' ';
      nitoa3(mun, no_a);
      strcpy(&genbuf[18], no_a);
      fprintf(fps, "%s\n", genbuf);
   }

   fclose(fp);
   fclose(fps);
}
#endif

#ifdef RPG_FIGHT
int transclass(void) {
   char buf[2];
   int i;

   clear();
   move(1, 11);
   outs("[1;37;41m¥»¥\\¯à¤@¥¹¨Ï¥Î§Y¦AµL¾÷·|ÅÜ§óºØ±Ú(¯S®í±ø¥ó¨Ò¥~) ½Ð¼f·V«ä¦Ò[m");
   move(2, 5);
   outs("ÂàºØ±Ú«e½Ð¥ý±N±zªºÂ¾·~¹D¨ã¥Ó½ÐÂk¹s([1;33;40m¦³ÂàÂ¾¹L¥²©w­n¥Ó½Ð[m)§_«hµ{¦¡µLªk°õ¦æ");

   move(4, 5);
   outs("§A¥i¥H¿ï¾ÜÂà¦¨¥H¤UºØ±Ú (¦³¬õ¦â¼Ð¥Üªº) :");
   move(5, 5);
   outs("(1)Ã~±Ú ([1;31;40m2[m)§¯ºë (3)¦aÆF (4)¥¼ª¾ (5)Às±Ú (6)¯«±Ú (7)Å]±Ú");
   getdata(7, 5, "½Ð¿é¤J±zªº¿ï¾Ü (2/Q) : ", buf, 2, DOECHO, 0);

   if(xuser.working[0]) {
      move(8, 5);
      outs("±zªºÂ¾·~¤w³Q©w¸q¹L,½Ð¦Ü [1;36;40mApply_Class[m ¥Ó½ÐÂk¹s...");
      goto Skip_TS;
   }

   if(buf[0] == '2') {
      item_update_passwd('l');
      move(8, 5);
      outs("Âà¦¨§¯ºë±N¦©¸gÅç­È [1;31;40m700[m (¤£¨¬¥ç¥iÂà¦ý¦©¦Ü­t­È)");
      getdata(9, 5, "¨t²Î±N³]©w±zªº°ò¥»¸ê®Æ¨Ã²¾°£±zªºªZ¾¹,½Ð°µ³Ì«áªº½T»{ y/N ",
              buf, 2, LCECHO, 0);
      if(buf[0] == 'y') {
         xuser.sex = 2;
         cuser.sex = xuser.sex;

         strcpy(xuser.working, "§¯ºë");
         xuser.nowlevel = 3;
         xuser.hp = 3;
         xuser.str = 0;
         xuser.mgc = 1;
         xuser.skl = 3;
         xuser.spd = 3;
         xuser.luk = 0;
         xuser.def = 0;
         xuser.mdf = 2;

         for(i=0 ; i<10; i++) {
         xuser.wepnlv[0][i] = '-';
         xuser.wepnlv[1][i] = 0;
         }

         xuser.wepnlv[0][3] = 'C';
         xuser.curwepnlv[0][0] = 0;
         xuser.curwepnlv[1][0] = 0;
         for(i=0 ; i<4 ; i++)
            xuser.curwepnspc[i] = 0;

         xuser.addexp -= 700;
         strcpy(xuser.class, "010000");

         item_update_passwd('u');
         item_update_passwd('l');
      }
   }

Skip_TS:
   pressanykey(NULL);
   return FULLUPDATE;
}
#endif
