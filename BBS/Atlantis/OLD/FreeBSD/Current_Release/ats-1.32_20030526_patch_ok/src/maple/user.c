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
    "�N��        �G%s\n"
    "�ʺ�        �G%s\n",
    u->userid,
    u->username);
  if(HAS_PERM(PERM_SYSOP) || !strcmp(cuser.userid, u->userid))
     prints(
    "�u��m�W    �G%s\n"
    "�~���}    �G%s\n"
    "�q�l�l��H�c�G%s\n"
#ifdef RPG_SETTING
    "�ر�        �G%s\n"
#endif
#ifndef MIN_USEREC_STRUCT
    "�ʧO        �G%s\n"
#endif
    "�ͤ�        �G%02i/%02i/%02i\n",
    u->realname,
    u->address,
    u->email,
#ifdef RPG_SETTING
    sex[(u->sex < sizeof(sex) / sizeof(*sex)) ? u->sex: 0],
#endif
#ifndef MIN_USEREC_STRUCT
    u->classsex == 3 ? "����" : u->classsex == 2 ? "�k" : u->classsex == 1 ?
    "�k" : "���w�q",
#endif
    u->month, u->day, u->year);

  prints("���U���    �G%s", ctime(&u->firstlogin));

  prints("�̪���{����G%s",
         strcmp(u->userid, "Kaoru") ? ctime(&u->lastlogin) : "�t�Τ����\n");

  if (real) prints("�̪���{�����G%s\n", u->lasthost);

  prints("�W������    �G%d ��\n", u->numlogins);

  prints("�峹�ƥ�    �G%d �g\n", u->numposts);

  sethomedir(genbuf, u->userid);
  if(HAS_PERM(PERM_SYSOP))
  prints("�p�H�H�c    �G%d ��\n", get_num_records(genbuf, sizeof(fileheader)));

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
       prints("�{�Ҹ��    �G%s\n�ϥΪ��v��  �G%s\n", u->justify, genbuf);
  }
  else
  {
    diff = (time(0) - login_start_time) / 60;
    prints("���d����    �G%d �p�� %2d ��\n", diff / 60, diff % 60);
  }

  // Thor: �Q�ݬݳo�� user �O���Ǫ������D
  if (u->userlevel >= PERM_BM)
  {
    int i, len, ch;
    boardheader *bhdr;
    char *list;
    resolve_boards();

    len = strlen(u->userid);

    outs("����O�D    �G");

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
  prints("�����X��    �G");
  for(ci = 0 ; ci < 25 ; ci++)
     if(HAS_PERM(PERM_SYSOP) || ci == cuser.now_stno)
        prints("%c", u->station_member[ci] ? 'X' : '-');
  outc('\n');
#endif
  outs((u->userlevel & PERM_LOGINOK) ?
    "\n�z�����U�{�Ǥw�g�����A�w��[�J����" :
    "\n�p�G�n���@�v���A�аѦҥ������G���z���U");

#ifdef  NEWUSER_LIMIT
  if ((u->lastlogin - u->firstlogin < 3 * 86400) && !HAS_PERM(PERM_POST))
    outs("\n�s��W���A�T�ѫ�}���v��");
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
        "(3)�]�w�v�� (M)�[�J���� (D)���h���� (S)�s�W���� [0]���� ",
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
     "(6)�����ܧ�/�D��s�W (7)�R���D�� (8)�˳ƪZ��/���H�ܧ� (9)�ܧ�¾�~��O");
     }
#endif
     getdata(b_lines - 1, 0, real ?
  "(1)�ק��� (2)�]�w�K�X (3)�]�w�v�� (4)�M���b�� (5)��ID "
#ifdef CHANGE_USER_MAIL_LIMIT
  "(L)�H�c "
#endif
  "[0]���� " : "�п�� (1)�ק��� (2)�]�w�K�X ==> [0]���� ",
       ans, 3, LCECHO, 0);
  }

  if(ans[0] > '2' && !real)
    ans[0] = '0';
#ifndef NO_USE_MULTI_STATION
  else {
     if(is_station_sysop() && !HAS_PERM(PERM_SYSOP) &&
        !(ans[0] == '3' || ans[0] == 'm' || ans[0] == 'd' || ans[0] == 's')) {
        if(!cuser.extra_mode[2]) ans[0] = '0';  /* Dopin: ���즹�O�Τ@�榡 */
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
    outs("�гv���ק�C");

    if (getdata(i++, 0, "�ʺ١G", buf, 24, DOECHO, u->username))
      strcpy(x.username, buf);

    if (getdata(i++, 0, "�u��m�W�G", buf, 20, DOECHO, u->realname))
      strcpy(x.realname, buf);

    if (getdata(i++, 0,  "�~��a�}�G", buf, 50, DOECHO, u->address))
      strcpy(x.address, buf);

#ifndef MIN_USEREC_STRUCT
    i++;
CSEX_OK1:
    getdata(i, 0, "�ʧO�G(1)�k�� (2)�k�� (3)���� ", buf, 2, DOECHO, 0);
    j = atoi(buf);
    if(!real) if(j > 3 || j < 1) goto CSEX_OK1;  /* ���w���ޤ��|��� */
    if(j) x.classsex = (char)j;
#endif

#ifdef RPG_SETTING
    if(real) {
      sprintf(genbuf,
      "�رڡG(1)�H�� (2)�~�� (3)���� (4)�a�F (5)���� (6)�s�� (7)���� (8)�]�� "
      "[%i]�G", u->sex + 1);

      getdata(i++, 0, genbuf, buf, 3, DOECHO, 0);
      if(buf[0] >= '1' && buf[0] <= '8') x.sex = buf[0] - '1';
       else x.sex = u->sex;
    }
#endif

    while (1 && real) {
      int len;

      sprintf(genbuf, "%02i/%02i/%02i",
        u->month, u->day, u->year);
      len = getdata(i, 0, "�ͤ� ���/���/�褸�G", buf, 9, DOECHO, genbuf);
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

    getdata(i++, 0, "�q�l�H�c(*�H�{�ҫH��*)�G", buf, 50, DOECHO, u->email);
    if (*buf && strcmp(buf, u->email)) {
      strcpy(x.email, buf);
      mail_changed = 1 - real;
    }
    else
       strcpy(x.email, u->email);

    if (real)
    {
      if (getdata(i++, 0, "�{�Ҹ�ơG", buf, 44, DOECHO, u->justify))
        strcpy(x.justify, buf);

      if (getdata(i++, 0, "�̪���{�����G", buf, 64, DOECHO, u->lasthost))
        strcpy(x.lasthost, buf);

      sprintf(genbuf, "%d", u->numlogins);
      if (getdata(i++, 0, "�W�u���ơG", buf, 10, DOECHO, genbuf))
        if ((fail = atoi(buf)) >= 0)
          x.numlogins = fail;

      sprintf(genbuf, "%d", u->numposts);
      if (getdata(i++, 0, "�峹�ƥءG", buf, 10, DOECHO, genbuf))
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
      if (!getdata(i++, 0, "�п�J��K�X�G", buf, PASSLEN, NOECHO, 0) ||
        !checkpasswd(u->passwd, buf))
      {
        outs("\n\n�z��J���K�X�����T\n");
        fail++;
        break;
      }
    }

    if (!getdata(i++, 0, "�г]�w�s�K�X�G", buf, PASSLEN, NOECHO, 0))
    {
      outs("\n\n�K�X�]�w����, �~��ϥ��±K�X\n");
      fail++;
      break;
    }
    strncpy(genbuf, buf, PASSLEN);

    getdata(i++, 0, "���ˬd�s�K�X�G", buf, PASSLEN, NOECHO, 0);
    if (strncmp(buf, genbuf, PASSLEN))
    {
      outs("\n\n�s�K�X�T�{����, �L�k�]�w�s�K�X\n");
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

    if (getdata(b_lines - 3, 0, "�s���ϥΪ̥N���G", genbuf, IDLEN + 1, DOECHO, 0))
    {
      if (searchuser(genbuf))
      {
        outs("���~! �w�g���P�� ID ���ϥΪ�");
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
    prints("�ϥΪ̭즳������ : %d", x.havemoney);
    getdata(2, 0, "�п�J������ ", buf, 10, DOECHO, 0);
    x.havemoney += atoi(buf);
    prints("�ϥΪ̥[������� : %d", x.havemoney);

    move(7, 0);
    prints("�ϥΪ̭즳�ʧO���X : %d", x.classsex);
    getdata(8, 0, "�п�J�ʧO���X 0 �L 1 �k 2 �k", buf, 2, DOECHO, 0);
    x.classsex = atoi(genbuf);

    move(10, 0);
    prints("�ϥΪ̭즳�D��� : %d", x.havetoolsnumber);
    getdata(11, 0, "�п�J�n�s�W���D�㸹�X : ", buf, 5, DOECHO, 0);
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

    getdata(22, 0, "�п�J���R���D��s��", buf, 3, DOECHO, 0);
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

    getdata(22, 0, "�~��R�� (y/N) ? ", buf, 2, LCECHO, 0);
    if(buf[0] == 'y') goto KEEP_DEL_ITEM;
    break;

  case '8':
    if(strcmp(cuser.userid, SUPERVISOR)) break;

    clear();
    getdata(b_lines -18, 0, "�˳ƪZ�����X :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnlv[0][0] = k;
    getdata(b_lines -16, 0, "�˳ƪZ���Ѽ� :  ", buf, 4, DOECHO, 0);
    x.curwepnlv[1][0] = buf[0];

    getdata(b_lines -14, 0, "�˳ƪZ���[�@ :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[0] = k;
    getdata(b_lines -12, 0, "�˳ƪZ���[�G :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[1] = k;
    getdata(b_lines -10, 0, "�˳ƪZ���[�T :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[2] = k;
    getdata(b_lines - 8, 0, "�˳ƪZ���[�| :  ", buf, 4, DOECHO, 0);
    k = atoi(buf);
    x.curwepnspc[3] = k;

    getdata(b_lines - 6, 0, "���H :  ", genbuf, IDLEN+1, DOECHO, 0);
    strcpy(x.lover, genbuf);
    getdata(b_lines - 4, 0, "�����x :  ", buf, 4, DOECHO, 0);
    x.commander = atoi(buf);
    break;

  case '9':
    if(strcmp(cuser.userid, SUPERVISOR))
       break;
    clear();
    getdata(1, 0, "¾�~�W :  ", genbuf, 20, DOECHO, 0);
    strcpy(x.working, genbuf);
    getdata(2, 0, "¾�~�ɦW(�������L�]�w) :  ", genbuf, 7, DOECHO, 0);
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
    getdata(12, 0, "¾�~�ޯ�@ :  ", buf, 3, DOECHO, 0);
    x.spcskl[0] = atoi(buf);
    getdata(13, 0, "¾�~�ޯ�G :  ", buf, 3, DOECHO, 0);
    x.spcskl[1] = atoi(buf);
    getdata(14, 0, "¾�~�ޯ�T :  ", buf, 3, DOECHO, 0);
    x.spcskl[2] = atoi(buf);
    getdata(15, 0, "¾�~�ޯ�| :  ", buf, 3, DOECHO, 0);
    x.spcskl[3] = atoi(buf);
    getdata(16, 0, "¾�~�ޯभ :  ", buf, 3, DOECHO, 0);
    x.spcskl[4] = atoi(buf);
    getdata(17, 0, "¾�~�ޯऻ :  ", buf, 3, DOECHO, 0);
    x.spcskl[5] = atoi(buf);
    getdata(18, 0, "�ץ��g��� :  ", genbuf, 7, DOECHO, 0);
    x.addexp = atoi(genbuf);
    getdata(19, 0, "�Z����O :  ", genbuf, 11, DOECHO, 0);
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
           pressanykey("�L�k�A�s�W,���޼ƥؤw�W�L�|��");
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
           pressanykey("�����ɮפw��s,�аȥ��� Y �T�w,�H�K��Ƥ��P�B");
        }

        x.userlevel |= PERM_BOARD;
        break;
     }
#endif
#ifdef CHANGE_USER_MAIL_LIMIT
  /* Dopin 11/20/02 �ܧ�H�c���ƻP�e�q�j�p���W�� */
  case 'l':
     if(!HAS_PERM(PERM_SYSOP) /* ���] ���� �P �b���`�� �i�ܧ�ϥΪ̫H�c�j�p */
#ifdef USE_ACCOUNTS_PERM      /* �p�G�����b���`�޳] �o�T�椣���[ */
        && !HAS_PERM(PERM_ACCOUNTS)
#endif
     ) break;
     eolrange(b_lines-6, b_lines);
     strcpy(buf, x.userlevel & PERM_BM ? "300" : x.userlevel & PERM_LOGINOK ?
                 "150" : "50");
     sprintf(&buf[10], "%d",
             x.max_mail_number ? x.max_mail_number : MAXKEEPMAIL);

     move(b_lines-5, 0);
     prints("��ϥΪ̫H��ƶq�W�� %s ��", &buf[10]);

     getdata(b_lines-4, 0, "�s���W���ƥ� : " , &buf[20], 5, DOECHO, &buf[10]);
     k = atoi(&buf[20]);
     if(k > 0) x.max_mail_number = k;
     else {
        pressanykey("�H��ƥػݬ������ ...");
        fail = 1;
     }

     sprintf(&buf[10], "%d",
             x.max_mail_kbytes ? x.max_mail_kbytes : atoi(buf));
     move(b_lines-3, 0);
     prints("��ϥΪ̫H��e�q�W�� %s Kbytes", &buf[10]);

     getdata(b_lines-2, 0, "�s���W���e�q : " , &buf[20], 5, DOECHO, &buf[10]);
     k = atoi(&buf[20]);
     if(k > 0) x.max_mail_kbytes = k;
     else {
        pressanykey("�H��e�q�ݬ������ ...");
        fail = 1;
     }

     move(b_lines, 0);  /* Dopin: �º�O���F�n�� �i�[�i���[ */
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
    getdata(b_lines - 1, 0, "�� �п�J�K�X�A�H�Ѱ��ù���w�G",
      genbuf, PASSLEN, NOECHO, 0);
  } while (!checkpasswd(cuser.passwd, genbuf));

  return FULLUPDATE;
}

#ifdef  HAVE_SUICIDE
int u_kill()
{
  char genbuf[200];

  getdata(b_lines - 1, 0, "�� �p�G�T�w�n�M���z���b���A�п�J�K�X�G",
    genbuf, PASSLEN, NOECHO, 0);

  if (*genbuf == '\0' || !checkpasswd(cuser.passwd, genbuf))
  {
    outmsg("�z��J���K�X�����T�A�b�����M���C");
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

  pressanykey("�˷R���B�͡A�C�s���b�A������y�A���̫�|�����a�I");
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
         if(pressanykey("��J �� ���_�d��") == KEY_LEFT) break;
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
      prints("[36m�i ñ�W��.%c �j[m\n", ch);
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
   outs("[1;37;41m���\\��@���ϥΧY�A�L���|�ܧ�ر�(�S�����ҥ~) �мf�V���[m");
   move(2, 5);
   outs("��رګe�Х��N�z��¾�~�D��ӽ��k�s([1;33;40m����¾�L���w�n�ӽ�[m)�_�h�{���L�k����");

   move(4, 5);
   outs("�A�i�H����ন�H�U�ر� (������Хܪ�) :");
   move(5, 5);
   outs("(1)�~�� ([1;31;40m2[m)���� (3)�a�F (4)���� (5)�s�� (6)���� (7)�]��");
   getdata(7, 5, "�п�J�z����� (2/Q) : ", buf, 2, DOECHO, 0);

   if(xuser.working[0]) {
      move(8, 5);
      outs("�z��¾�~�w�Q�w�q�L,�Ц� [1;36;40mApply_Class[m �ӽ��k�s...");
      goto Skip_TS;
   }

   if(buf[0] == '2') {
      item_update_passwd('l');
      move(8, 5);
      outs("�ন����N���g��� [1;31;40m700[m (������i������ܭt��)");
      getdata(9, 5, "�t�αN�]�w�z���򥻸�ƨò����z���Z��,�а��̫᪺�T�{ y/N ",
              buf, 2, LCECHO, 0);
      if(buf[0] == 'y') {
         xuser.sex = 2;
         cuser.sex = xuser.sex;

         strcpy(xuser.working, "����");
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
