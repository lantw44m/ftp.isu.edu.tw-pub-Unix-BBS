
/*-------------------------------------------------------*/
/* user.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* author : opus.bbs@bbs.cs.nthu.edu.tw                  */
/* target : user configurable setting routines           */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#define _USER_C_

#include "bbs.h"
#define MAXMONEY ((u->totaltime*4)+(u->numlogins*10)+(u->numposts*1000)+10000)

extern int numboards;
extern boardheader *bcache;
extern void resolve_boards();

char *sex[8] = { MSG_BIG_BOY, MSG_BIG_GIRL, MSG_LITTLE_BOY, MSG_LITTLE_GIRL,
                 MSG_MAN, MSG_WOMAN, MSG_PLANT, MSG_MIME };
void
user_display(u, real)
  userec *u;
{
  int secd, diff; 
  int day = 0, hour = 0, min = 0;
  char genbuf[128];

  if (u->sex >= 8) 
    u->sex = 7;

  clrtobot();
//  sethomedir(genbuf, u->userid);
   sprintf(genbuf, "home/%s/mailbox/.DIR", u->userid);
  outs(
"[1;33m���w�w�w�w�w�w�w�w�w�w�w�w[42m   �ϥΪ̸��   [40m�w�w�w�w�w�w�w�w�w�w�w�w��\n");
  prints("\
  [32m�^��N���G[37m%-16.16s[32m�ʺ١G[37m%-20.20s[32m�ʧO�G[37m%-8.8s
  [32m�u��m�W�G[37m%-16.16s[32m��}�G[37m%-40s
  [32m�X�ͤ���G[37m19%02i�~%02i��%02i��  [32m��Mail�G[37m%-40s\n",
u->userid,u->username,sex[u->sex],
u->realname,u->address,
u->year,u->month, u->day,u->email
); 
  prints("  [32m�W�����ơG[37m%-16d[32m���U����G[37m%s"
    ,u->numlogins,ctime(&u->firstlogin)); 
  prints("  [32m�峹�ƥءG[37m%-16d[32m�e���W���G[37m%s"
    ,u->numposts,ctime(&u->lastlogin)); 
  prints("  [32m�p�H�H�c�G[37m%-4d ��         [32m�Q���o��G[37m%s"
    ,rec_num(genbuf, sizeof(fileheader)),ctime(&u->dtime));
  prints(
"  [32m�����ƶq�G[37m%-16ld[32m�ȹ��ƶq�G[37m%-16ld[32m�g��ȡG[37m%-ld\n"
"  [32m�ȹ��W���G[37m%-16ld[32m�H�c�W���G[37m%d ��\n"
"  [32m�H����ơG[37m%-16ld[32m�n�_���ơG[37m%-16ld[32m�߱��G[37m%-4.4s\n"
"  [32m�o�T���ơG[37m%-16d[32m���T���ơG[37m%d\n"
"  [32m�W���a�I�G[37m%s \n"
//"  [32m�ǩI���X�G[37m0%d-%s \n"
,u->goldmoney,u->silvermoney,u->exp
,MAXMONEY,(u->exmailbox+MAXKEEPMAIL)
,u->bequery,u->toquery,u->feeling,u->sendmsg,u->receivemsg
,u->lasthost,u->pagermode
//,u->pagernum);
);

  if (real)
  {
    strcpy(genbuf, "bTCPRp#@XWBA#VSA?crFG??????????");
    for (diff = 0; diff < 31; diff++)
      if (!(u->userlevel & (1 << diff)))
        genbuf[diff] = '-';
    prints("  [32m�{�Ҹ�ơG[37m%-50.50s\n",u->justify);
    prints("  [32m�ϥ��v���G[37m%-32s\n",genbuf);
  }
  secd = u->totaltime % 60;
  diff = u->totaltime / 60;
  day = diff / 1440;
  hour = (diff/60)%24;
  min = diff%60;
  prints("  [32m�W���ɶ��G[37m%d�� %d�p�� %d�� %d��  ����(%d)\n",day,hour,min,secd,count_multi());

  if (u->userlevel >= PERM_BM)
  {
    int i;
    boardheader *bhdr;
    resolve_boards();

    outs("  [32m����O�D�G[37m");

    for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
    {
      if(userid_is_BM(u->userid, bhdr->BM))
          {
              outs(bhdr->brdname);
              outc(' ');
          }
    }
    outc('\n');
  }
prints("[33m���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[m\n");

  outs((u->userlevel & PERM_LOGINOK) ?
    "�z�����U�{�Ǥw�g�����A�w��[�J����" :
    "�p�G�n���@�v���A�аѦҥ������G���z���U");

#ifdef  NEWUSER_LIMIT
  if ((u->lastlogin - u->firstlogin < 3 * 86400) && !HAS_PERM(PERM_POST))
    outs("�s��W���A�T�ѫ�}���v��");
#endif
}


void
uinfo_query(u, real, unum)
  userec *u;
{
  userec x;
  register int i, j, fail, mail_changed;
  char ans[4], buf[STRLEN];
  char genbuf[200];
  int flag=0,temp;

  fail = mail_changed = 0;

  memcpy(&x, u, sizeof(userec));
  getdata(b_lines - 1, 0, real ?
    "(1)�ק��� (2)�]�w�K�X (3)�]�w�v�� (4)�M���b�� (5)��ID [0]���� " :
    "�п�� (1)�ק��� (2)�]�w�K�X ==> [0]���� ",
    ans, 3, DOECHO, 0);

  if (ans[0] > '2' && !real)
  {
    ans[0] = '0';
  }

  if (ans[0] == '1' || ans[0] == '3')
  {
    clear();
    i = 2;
    move(i++, 0);
    outs(msg_uid);
    outs(x.userid);
  }

  switch (ans[0])
  {
  case '1':
    move(0, 0);
    outs("�гv���ק�C");
    getdata(i++, 0," �� ��  �G",x.username, 24, DOECHO,x.username);
    strip_ansi(x.username,x.username,STRIP_ALL);
    getdata(i++, 0,"�~��a�}�G",x.address, 50, DOECHO,x.address);
    strip_ansi(x.address,x.address,STRIP_ALL);
    getdata(i++, 0,"�{�b�߱��G",x.feeling, 5, DOECHO,x.feeling);
    x.feeling[4] = '\0';
    strip_ansi(x.feeling, x.feeling, STRIP_ALL);
    getdata(i++, 0,"�q�l�H�c�G",buf, 50, DOECHO,x.email);
    if(belong_spam(BBSHOME"/etc/spam-list",x.email))
    {
      pressanykey("��p,�����������A�� E-Mail �H�c��m");
      strcpy(buf,x.email);
    }
    if (strcmp(buf,x.email) && !not_addr(buf))
    {
      strcpy(x.email,buf);
      mail_changed = 1 - real;
    }
    strip_ansi(x.email,x.email,STRIP_ALL);

//    sprintf(buf, "%04d", u->pagermode);
//    getdata(i, 0,"�I�s�������G",buf, 5, DOECHO, buf );
//    if(!strncmp(buf,"094",3) && buf[3] != '2' && buf[3] != '4')
//      x.pagermode = atol(buf);
//    else
//      x.pagermode = u->pagermode;      

//    getdata(i++, 30,"�I�s�����X�G",x.pagernum, 7, DOECHO,x.pagernum);

    sprintf(genbuf,"�ʧO�G");
    for(j=0;j<8;j++)
    {
      sprintf(buf, "%d.%s ", j+1, sex[j]);
      strcat(genbuf,buf);
    }
    getdata(i++, 0, genbuf, buf, 2, DOECHO, 0);
    if (buf[0] >= '1' && buf[0] <= '8')
       x.sex = buf[0] - '1';

    while (1)
    {
      sprintf(genbuf,"%02i/%02i/%02i",u->year,u->month,u->day);
      getdata(i, 0, "�X�ͦ~�� 19", buf, 3, DOECHO, genbuf);
      x.year  = (buf[0] - '0') * 10 + (buf[1] - '0');
      getdata(i, 0, "�X�ͤ��   ", buf, 3, DOECHO, genbuf+3);
      x.month = (buf[0] - '0') * 10 + (buf[1] - '0');
      getdata(i, 0, "�X�ͤ��   ", buf, 3, DOECHO, genbuf+6);
      x.day   = (buf[0] - '0') * 10 + (buf[1] - '0');
      if (!real && (x.month > 12 || x.month < 1 ||
        x.day > 31 || x.day < 1 || x.year > 90 || x.year < 40))
        continue;
      i++;
      break;
    }
    if (real)
    {
      unsigned long int l;

// wildcat:���n�� user ���U���ç�W�r? 
      getdata(i++, 0,"�u��m�W�G",x.realname, 20, DOECHO,x.realname);
      strip_ansi(x.realname,x.realname,STRIP_ALL);
      sprintf(genbuf, "%d", x.numlogins);
      if (getdata(i, 0,"�W�u���ơG", buf, 10, DOECHO,genbuf))
        if ((l = atoi(buf)) >= 0 && (l<=x.numlogins))
          x.numlogins = (int) l;
      sprintf(genbuf,"%d", x.numposts);
      if (getdata(i++, 25, "�峹�ƥءG", buf, 10, DOECHO,genbuf))
        if ((l = atoi(buf)) >= 0 && (l<=x.numposts))
          x.numposts = l;
      sprintf(genbuf, "%ld", x.silvermoney);
      if (getdata(i, 0,"���W�ȹ��G", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0) // && (l<=x.silvermoney))
          x.silvermoney = l;
      sprintf(genbuf, "%ld", x.goldmoney);
      if (getdata(i, 25,"���W�����G", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0) // && (l<=x.goldmoney))
          x.goldmoney = l;
      sprintf(genbuf, "%ld", x.exp);
      if (getdata(i++, 50,"�g��ȡG", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0) // && (l<=x.exp))
          x.exp = l;
      sprintf(genbuf, "%ld", x.sendmsg);
      if (getdata(i, 0,"�o���y�ơG", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0 && (l<=x.sendmsg))
          x.sendmsg = l;
      sprintf(genbuf, "%ld", x.receivemsg);
      if (getdata(i++, 25,"�����y�ơG", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0 && (l<=x.receivemsg))
          x.receivemsg = l;
      sprintf(genbuf, "%ld", x.bequery);
      if (getdata(i, 0,"�H��סG", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0 && (l<=x.bequery))
          x.bequery = l;
      sprintf(genbuf, "%ld", x.toquery);
      if (getdata(i++, 25,"�n�_�סG", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0 && (l<=x.toquery))
          x.toquery = l;
      sprintf(genbuf, "%ld", x.totaltime);
      if (getdata(i++, 0,"�W���ɼơG", buf, 10, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0 && (l<=x.totaltime))
          x.totaltime = l;
      sprintf(genbuf, "%d", x.exmailbox);
      if (getdata(i++, 0,"�ʶR�H�c�ơG", buf, 4, DOECHO,genbuf))
        if ((l = atol(buf)) >= 0)
          x.exmailbox = (int) l;

      getdata(i++, 0,"�{�Ҹ�ơG", x.justify, 39, DOECHO, x.justify);
      getdata(i++, 0,"�̪���{�����G", x.lasthost, 24, DOECHO,x.lasthost);

      fail = 0;
    }
    break;

  case '2':
    i = 19;
    if (!real)
    {
      if (!getdata(i++, 0, "�п�J��K�X�G", buf, PASSLEN, PASS,0) ||
        !chkpasswd(u->passwd, buf))
      {
        outs("\n\n�z��J���K�X�����T\n");
        fail++;
        break;
      }
    }

    if (!getdata(i++, 0, "�г]�w�s�K�X�G", buf, PASSLEN, PASS,0))
    {
      outs("\n\n�K�X�]�w����, �~��ϥ��±K�X\n");
      fail++;
      break;
    }
    strncpy(genbuf, buf, PASSLEN);

    getdata(i++, 0, "���ˬd�s�K�X�G", buf, PASSLEN, PASS,0);
    if (strncmp(buf, genbuf, PASSLEN))
    {
      outs("\n\n�s�K�X�T�{����, �L�k�]�w�s�K�X\n");
      fail++;
      break;
    }
//    buf[PASSLEN] = '\0';
    strncpy(x.passwd, genpasswd(buf), PASSLEN);
    break;

  case '3':
    i = setperms(x.userlevel);
    if (i == x.userlevel)
      fail++;
    else {
      flag=1;
      temp=x.userlevel;
      x.userlevel = i;
    }
    break;

  case '4':
    i = QUIT;
    break;

  case '5':
    if (getdata(b_lines - 3, 0, "�s���ϥΪ̥N���G", genbuf, IDLEN + 1,
        DOECHO,x.userid))
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

  default:
    return;
  }

  if (fail)
  {
    pressanykey(NULL);
    return;
  }

  getdata(b_lines - 1, 0, msg_sure_ny, ans, 3, LCECHO,0);
  if (*ans == 'y')
  {
    if (flag) Security(temp,i,cuser.userid,x.userid);
    if (strcmp(u->userid, x.userid))
    {
      char src[STRLEN], dst[STRLEN];

      sethomepath(src, u->userid);
      sethomepath(dst, x.userid);
      f_mv(src, dst);
      setuserid(unum, x.userid);
    }
    memcpy(u, &x, sizeof(x));
    substitute_record(fn_passwd, &u, sizeof(userec), unum);
    if (mail_changed)
    {

#ifdef  EMAIL_JUSTIFY
      x.userlevel &= ~PERM_LOGINOK;
      mail_justify(cuser);
#endif
    }

    if (i == QUIT)
    {
      char src[STRLEN], dst[STRLEN];

      sprintf(src, "home/%s", x.userid);
      sprintf(dst, "tmp/%s", x.userid);
// wildcat : ²��h�F , ���n�� rm home/userid
      if(dashd(src))
        f_mv(src , dst);
/*
woju
*/
      log_usies("KILL", x.userid);
      x.userid[0] = '\0';
      setuserid(unum, x.userid);
    }
    else
       log_usies("SetUser", x.userid);
    substitute_record(fn_passwd, &x, sizeof(userec), unum);
  }
}


int
u_info()
{
  move(1, 0);
  update_data(); 
  user_display(&cuser, 0);
  uinfo_query(&cuser, 0, usernum);
  strcpy(currutmp->username, cuser.username);
  strcpy(currutmp->feeling, cuser.feeling);
  return 0;
}

int
u_cloak()
{
  if(currutmp->invisible & 1)
    currutmp->invisible &= ~1;
  else
    currutmp->invisible |= 1;
    
  pressanykey((currutmp->invisible & 1) ? MSG_CLOAKED : MSG_UNCLOAK);
  return XEASY;
}


unsigned
u_habit()
{
  unsigned fbits;
  register int i;
  char choice[4];
  fbits = cuser.habit;
  stand_title("�ϥΪ̳ߦn�]�w");
  move(2, 0);
  outs("�Ш̷ӱz���ϥβߺD�ۦ�վ�  (�����})�G\n");
  move(4, 0);
  for (i = 0; i < NUMHABITS; i++)
  {
//    prints("%c. %-28s %-7s %c. %-28s %s\n"
    prints("%c. %-36s %-s\n"
       , 'A' + i, habitstrings[i],((fbits >> i) & 1 ? "��" : "  "));
//       , i < 10 ? 'M' + i : '0' + i - 10,
//       , 'M' + i,
//         habitstrings[i+12],((fbits >> i+12) & 1 ? "��" : "  "));
  }
  clrtobot();
  while (getdata(b_lines - 1, 0, "�Ы� [A-G] �����]�w�A�� [Return] �����G",
      choice, 2, LCECHO, 0))
  {
    i = choice[0] - 'a';
    if (i < 0 || i >= NUMHABITS)
      bell();
    else
    {
      fbits ^= (1 << i);
//      move( i%14 + 4, i <= 14 ? 32 : 72);
      move(i+4,40);
      prints((fbits >> i) & 1 ? "��" : "  ");
    }
    /* for HABIT_COLOR, �M���F�m��Ҧ��]��, i==1�O��ܲĤG��HABIT,
       ��HABIT_COLOR���b�ĤG��HABIT��, �o��]�n�����վ�.
                                                - add by wisely - */
    if( i == 1) showansi ^= 1;
  }
  update_data(); /* SiE: sync data prev bug user */
  cuser.habit = fbits;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum); /* �O�� */
  return RC_FULL;
}

#ifdef  HAVE_SUICIDE
int
u_kill()
{
  char genbuf[200];
  if(check_money(1000,GOLD)) return 0;

  getdata(b_lines - 1, 0, "�� �p�G�T�w�n�M���z���b���A�п�J�K�X�G",
    genbuf, PASSLEN, PASS,0);

  if (*genbuf == '\0' || !chkpasswd(cuser.passwd, genbuf))
  {
    outmsg("�z��J���K�X�����T�A�b�����M���C");
    igetch();
    return RC_FULL;
  }
  sprintf(genbuf, "#%d %s:%s %d %d",
    usernum, cuser.realname, cuser.username, cuser.numlogins, cuser.numposts);
  log_usies("SUCI ", genbuf);

  sprintf(genbuf, "%s %s", fromhost, Cdate(&(cuser.firstlogin)));
  log_usies("SUCI ", genbuf);

  sethomepath(genbuf, cuser.userid);
  if (f_mv(genbuf, "tmp"))
  {
/*
woju
*/
    sprintf(genbuf, "(cd home; tar zcf SUCI_%s.tgz %s); /bin/rm -fr home/%s",
            cuser.userid, cuser.userid, cuser.userid);
    system(genbuf);
  }
  cuser.userid[0] = '\0';
  substitute_record(fn_passwd, &cuser, sizeof(cuser), usernum);
  setuserid(usernum, cuser.userid);
  purge_utmp(currutmp);

  pressanykey("�˷R���B�͡A�C�s���b�A������y�A���̫�|�����a�I");
  reset_tty();
  exit(0);
}
#endif


void
showplans(char* uid)
{
  char genbuf[200],ans[4];

  getdata(22,0,"�� [1]�W���� [2]ñ�W�� [3]��ؤ峹 [4]�ӤH�@�~? ",ans,4,LCECHO,0);

  switch(ans[0])
  {
    case '1':    
      sethomefile(genbuf, uid, fn_plans);
      if(dashf(genbuf))
        more(genbuf,YEA);
      else
        pressanykey("%s �ثe�S���W��", uid);
      break;
    case '2':
      break;
    case '3':
      user_gem(uid);
      break;
    case '4':
      user_allpost(uid);
      break;
    default:
      break;
  }
  return;
}


int
showsignature(fname)
  char *fname;
{
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

void
u_editfile()
{
  int i;
  char ans[4],buf[128];
  FILE *fp;
  
  show_file("etc/userfile",12,11,ONLY_COLOR);
  getdata(b_lines,0," �n�ݭ����ɮסH ",ans,4,LCECHO,0);
  if(ans[0] == 'I' || ans[0] == 'i')
  {
    clear();
    sprintf(buf,BBSHOME "/WWW/user/%s.html",cuser.userid); 
    more(buf);
    getdata(1,0,"���� index �s��(E) �R��(D) �S��[Q] ",ans,2,LCECHO,0);
    if(ans[0] == 'e' || ans[0] == 'E')
    {
      setutmpmode(EDITPLAN);
      vedit(buf,NA);
    }
    if(ans[0] == 'd' || ans[0] == 'D')
      unlink(buf);
    else
      u_editfile();
  }
  if (ans[0] >= '1' && ans[0] <= '9')
  {
    i = showsignature(buf);
    buf[i]=ans[0];
    clear();
    show_file(buf,2,MAXSIGLINES,ONLY_COLOR);
    getdata(1,0,"�W���� �s��(E) �R��(D) �S��[Q] ",ans,2,LCECHO,0);
    if(ans[0] == 'e' || ans[0] == 'E')
    {
      setutmpmode(EDITSIG);
      vedit(buf,NA);
    }
    if(ans[0] == 'd' || ans[0] == 'D')
      unlink(buf);
    else
      u_editfile();
  }
  if(ans[0] == '0')
  {
    clear();
//    showplans(cuser.userid);
    setuserfile(buf, fn_plans);
    getdata(1,0,"�W���� �s��(E) �R��(D) �S��[Q] ",ans,2,LCECHO,0);
    if(ans[0] == 'e' || ans[0] == 'E')
    {
      setutmpmode(EDITPLAN);
      vedit(buf,NA);
    }
    if(ans[0] == 'd' || ans[0] == 'D')
      unlink(buf);
    else
      u_editfile();
  }
  if (ans[0] == 'C' || ans[0] == 'c')
  {
    getdata(b_lines-1,0,"�Z���W�١G",buf,10,DOECHO,currutmp->cursor);
    strip_ansi(buf, buf, STRIP_ALL);
    strcpy(currutmp->cursor,buf);
    setuserfile(buf,"cursor");
    fp=fopen(buf,"wt");
    fprintf(fp,"%s",currutmp->cursor);
    fclose(fp);
    pressanykey("�Z����s!");
    u_editfile();
/*    
    clear();
    show_file(buf,2,2,STRIP_ALL);
    getdata(1,0,"�Z�� �s��(E) �R��(D) �S��[Q] ",ans,2,LCECHO,0);
    if(ans[0] == 'e' || ans[0] == 'E')
    {
      setutmpmode(EDITING);
      vedit(buf,NA);
    }
    if(ans[0] == 'd' || ans[0] == 'D')
      unlink(buf);
    else
      u_editfile();
    cursor_load(buf);
  */
  }
}

/* --------------------------------------------- */
/* �ϥΪ̶�g���U���                            */
/* --------------------------------------------- */

static void
getfield(line, info, desc, buf, len)
  int line, len;
  char *info, *desc, *buf;
{
  char prompt[STRLEN];
  char genbuf[200];

  sprintf(genbuf, "����]�w�G%-30.30s (%s)", buf, info);
  move(line, 2);
  outs(genbuf);
  sprintf(prompt, "%s�G", desc);
  if (getdata(line + 1, 2, prompt, genbuf, len, DOECHO,0))
    strcpy(buf, genbuf);
  move(line, 2);
  prints("%s�G%s", desc, buf);
  clrtoeol();
}

int
u_justify()
{
  if (cuser.userlevel & PERM_LOGINOK)
  {
    pressanykey("�z�������T�{�w�g�����A���ݭ��H�����{�ҫH��C");
    return XEASY;
  }
  else              
    mail_justify(cuser);
    
  return 0;
}

int
u_register()
{
  char rname[20], addr[50] ,howto[50]="�нT���g";
  char phone[20], career[40], email[50],birthday[9],sex_is[2],year,mon,day;
  char ans[3], *ptr;
  FILE *fn;
  time_t now;
  char genbuf[200];
  
  if (cuser.userlevel & PERM_LOGINOK)
  {
    pressanykey("�z�������T�{�w�g�����A���ݶ�g�ӽЪ�");
    return XEASY;
  }
  if (fn = fopen(fn_register, "r"))
  {
    while (fgets(genbuf, STRLEN, fn))
    {
      if (ptr = strchr(genbuf, '\n'))
        *ptr = '\0';
      if (strncmp(genbuf, "uid: ", 5) == 0 &&
        strcmp(genbuf + 5, cuser.userid) == 0)
      {
        fclose(fn);
        pressanykey("�z�����U�ӽг�|�b�B�z���A�Э@�ߵ���");
        return XEASY;
      }
    }
    fclose(fn);
  }
/*
  getdata(b_lines - 1, 0, "�z�O�_�n��g���U���(Y/N)�H[N] ", ans, 3, LCECHO,0);
  if (ans[0] != 'y')
    return RC_FULL;
*/
  move(2, 0);
  clrtobot();
  strcpy(rname, cuser.realname);
  strcpy(addr, cuser.address);
  strcpy(email, cuser.email);
  sprintf(birthday, "%02i/%02i/%02i",
        cuser.year, cuser.month, cuser.day);
  sex_is[0]=(cuser.sex >= '0' && cuser.sex <= '7') ? cuser.sex+'1': '1';sex_is[1]=0;
  career[0] = phone[0] = '\0';
  while (1)
  {
    clear();
    move(3, 0);
    prints("%s[1;32m�i[m%s[1;32m�j[m �z�n�A�оڹ��g�H�U�����:(�L�ܧ�Ы�enter���L)",
      cuser.userid, cuser.username);
    getfield(6, "�нT���g����m�W", "�u��m�W", rname, 20);
    getfield(8, "�Ǯըt�ũγ��¾��", "�A�ȳ��", career, 40);
    getfield(10, "�]�A��ǩΪ��P���X", "�ثe��}", addr, 50);
    getfield(12, "�]�A���~�����ϰ�X", "�s���q��", phone, 20);
    while (1)
    {
    int len;
    getfield(14, " 19xx/��/�� �p: 77/12/01","�ͤ�",birthday,9);
    len = strlen(birthday);
    if(!len)
       {
         sprintf(birthday, "%02i/%02i/%02i",
         cuser.year, cuser.month, cuser.day);
         year=cuser.year;
         mon=cuser.month;
         day=cuser.day;
       }
    else if (len==8)
       {
        year  = (birthday[0] - '0') * 10 + (birthday[1] - '0');
        mon = (birthday[3] - '0') * 10 + (birthday[4] - '0');
        day   = (birthday[6] - '0') * 10 + (birthday[7] - '0');
       }
    else
        continue;
    if (mon > 12 || mon < 1 || day > 31 || day < 1 || year > 90 || year < 40)
        continue;
    break;
    }
    getfield(16,"1.���� 2.�j�� 3.���} 4.����","�ʧO",sex_is,2);
    getfield(18, "�����{�ҥ�", "E-Mail Address", email, 50);
    getfield(20, "�q���䪾�D�o�ӯ���", "�q��o��", howto, 50);

    getdata(b_lines - 1, 0, "�H�W��ƬO�_���T(Y/N)�H(Q)�������U [N] ", ans, 3, LCECHO,0);
    if (ans[0] == 'q')
      return 0;
    if (ans[0] == 'y')
      break;
  }
  /* wildcat test ���U�q�� */
  sysop_bbcall("�ϥΪ̵��U %d : %s",usernum,cuser.userid);
  cuser.rtimes++;
  strcpy(cuser.realname, rname);
  strcpy(cuser.address, addr);
  strcpy(cuser.email, email);
  cuser.sex= sex_is[0]-'1';
  cuser.month=mon;cuser.day=day;cuser.year=year;
  if (fn = fopen(fn_register, "a"))
  {
    now = time(NULL);
    str_trim(career);
    str_trim(addr);
    str_trim(phone);
    fprintf(fn, "num: %d, %s", usernum, ctime(&now));
    fprintf(fn, "uid: %s\n", cuser.userid);
    fprintf(fn, "name: %s\n", rname);
    fprintf(fn, "howto: %s\n", howto);
    fprintf(fn, "career: %s\n", career);
    fprintf(fn, "addr: %s\n", addr);
    fprintf(fn, "phone: %s\n", phone);
    fprintf(fn, "email: %s\n", email);
    fprintf(fn, "----\n");
    fclose(fn);
  }
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum); /* �O�� */
  return 0;
}


/* --------------------------------------------- */
/* �C�X�Ҧ����U�ϥΪ�                            */
/* --------------------------------------------- */


extern struct UCACHE *uidshm;
int usercounter, totalusers, showrealname;
ushort u_list_special;

extern int
bad_user_id(char userid[]);

static int
u_list_CB(uentp)
  userec *uentp;
{
  static int i;
  char permstr[8], *ptr;
  register int level;

  if (uentp == NULL)
  {
    move(2, 0);
    clrtoeol();
    prints("[7m  �ϥΪ̥N��   %-25s   �W��  �峹  %s  �̪���{���     [0m\n",

    showrealname ?
/*#if defined(REALINFO) && defined(ACTS_REALNAMES)*/
      "�u��m�W" :
/*#else*/
      "�︹�ʺ�"
/*#endif*/

      ,HAS_PERM(PERM_SEEULEVELS) ? "����" : "");
    i = 3;
    return 0;
  }

  if (bad_user_id(uentp->userid))  /* Ptt */
    return 0;

  if (!(uentp->userlevel & u_list_special))
    return 0;

  if (i == b_lines)
  {
    prints(COLOR1"  �w��� %d/%d �H(%d%%)  "COLOR2"  (Space)[30m �ݤU�@��  [32m(Q)[30m ���}  [0m",
      usercounter, totalusers, usercounter * 100 / totalusers);
    i = igetch();
    if (i == 'q' || i == 'Q')
      return QUIT;
    i = 3;
  }
  if (i == 3)
  {
    move(3, 0);
    clrtobot();
  }

  level = uentp->userlevel;
  strcpy(permstr, "----");
  if (level & PERM_SYSOP)
    permstr[0] = 'S';
  else if (level & PERM_ACCOUNTS)
    permstr[0] = 'A';
  else if (level & PERM_DENYPOST)
    permstr[0] = 'p';

  if (level & (PERM_BOARD))
    permstr[1] = 'B';
  else if (level & (PERM_BM))
    permstr[1] = 'b';

  if (level & (PERM_XEMPT))
    permstr[2] = 'X';
  else if (level & (PERM_LOGINOK))
    permstr[2] = 'R';

  if (level & (PERM_CLOAK | PERM_SEECLOAK))
    permstr[3] = 'C';

  ptr = (char *) Cdate(&uentp->lastlogin);
  ptr[18] = '\0';
  prints("%-14s %-27.27s%5d %5d  %s  %s\n",
    uentp->userid,

    showrealname ?
/*#if defined(REALINFO) && defined(ACTS_REALNAMES)*/
    uentp->realname :
/*#else*/
    uentp->username
/*#endif*/

    ,uentp->numlogins, uentp->numposts,
    HAS_PERM(PERM_SEEULEVELS) ? permstr : "", ptr);
  usercounter++;
  i++;
  return 0;
}


int
u_list()
{
  char genbuf[3];

  setutmpmode(LAUSERS);
  showrealname = 2;
  u_list_special = 127;
  usercounter = 0;
  totalusers = uidshm->number;
  
  if (HAS_PERM(PERM_ACCOUNTS))
  {
    getdata(b_lines - 1, 0, "�[�� [1]�S���� (2)�����H", genbuf, 2, DOECHO,0);
    if (genbuf[0] == '1')
    {
      getdata(b_lines-1, 0, "1.SYS  2.ACC  3.BRD  4.ADM  5.BM  6.ABOVE ? ",
        genbuf, 2, DOECHO, 0);
      if(genbuf[0] > '0' && genbuf[0] <'7')
         switch(genbuf[0])
         {
            case '1': u_list_special = PERM_SYSOP;
                      break;
            case '2': u_list_special = PERM_ACCOUNTS;
                      break;
            case '3': u_list_special = PERM_BOARD;
                      break;
            case '4': u_list_special = PERM_BBSADM;
                      break;
            case '5': u_list_special = PERM_BM;
                      break;
            case '6': u_list_special = (PERM_SYSOP | PERM_ACCOUNTS | PERM_BOARD | PERM_BBSADM | PERM_BM );
                      break;
         }
    }

    getdata(b_lines - 1, 0, "��� [1]�u��m�W (2)�ʺ١H", genbuf, 2, DOECHO,0);
    if (genbuf[0] == '1')
      showrealname = 1;
  }
  else
    return 0;

  u_list_CB(NULL);

  if (rec_apply(fn_passwd, u_list_CB, sizeof(userec)) == -1)
  {
    outs(msg_nobody);
    return XEASY;
  }
  move(b_lines, 0);
  clrtoeol();
  prints(COLOR1"  �w��� %d/%d ���ϥΪ�(�t�ήe�q�L�W��)  "COLOR2"  (�Ы����N���~��)  [m",
    usercounter, totalusers);
  egetch();
  return 0;
}

int
m_postnotify()
{
  FILE *f1;
  int n=0, i;
  char ans[4];
  char genbuf[200],fname[200];
  char id[64][IDLEN+1];

  sethomefile(genbuf, cuser.userid, "postnotify"); 
  if ((f1 = fopen (genbuf, "r")) == NULL)
    return XEASY;

  stand_title ("�f�ַs�峹�q��");
  i = 2;

  while (fgets (genbuf, STRLEN, f1))
  {
    move (i++, 0);
    outs (genbuf);
    strcpy(id[n],genbuf);
    n++;
  }
  sethomefile(genbuf, cuser.userid, "postnotify"); 
  if(!isprint(id[0][0]))
  {
    unlink(genbuf);
    return 0;
  } 
  fclose(f1); 
  getdata (b_lines - 1, 0, "�}�l�f�ֶ�(Y/N)�H[Y] ", ans, 3, LCECHO, "Y");
  if (ans[0] == 'y')
  {
    sethomefile(fname,cuser.userid,"postnotify.ok"); 
    for(i = n-1; i>= 0; i--)
    {
      move(1,0);
      clrtobot();  
      if(!getuser(id[i]))
      {
        pressanykey("�d�L���H %s",id[i]);
        sethomefile(genbuf, cuser.userid, "postnotify");
        del_distinct(genbuf, id[i]);
        continue;
      }  
      move(2,0); 
      prints("[1;32m�^��N�� : [37m%-13.13s   [32m�ʺ�: [37m%s\n",xuser.userid,xuser.username);
      prints("[1;32m�W������ : [37m%-13d   [32m�峹: [37m%d\n",xuser.numlogins,xuser.numposts);
      prints("[1;32m�q�l�H�c : [37m%s[m\n",xuser.email);
      getdata(10,0,"�O�_�n���L�[�J? (y/n/Skip)�H[S]",ans,3,LCECHO,0);
      if(ans[0] == 'y' || ans[0] == 'Y')
      {
        add_distinct(fname, xuser.userid);
        sethomefile(genbuf, cuser.userid, "postnotify"); 
        del_distinct(genbuf, xuser.userid); 
        mail2user(xuser,"[�s�峹�q��] �P�N�[�J",BBSHOME"/etc/pn_agree"); 
      } 
      if(ans[0] == 'n' || ans[0] == 'N')
      {
        sethomefile(genbuf, cuser.userid, "postnotify"); 
        del_distinct(genbuf, xuser.userid); 
        mail2user(xuser,"[�s�峹�q��] �ڵ��[�J",BBSHOME"/etc/pn_dissent"); 
//        sethomefile(genbuf, xuser.userid, "postlist"); 
//        del_distinct(genbuf, cuser.userid); 
      } 
    }
  }
  return 0;
}

int
re_m_postnotify()
{
  char genbuf[200],buf[200],buf2[200];

  sethomefile(buf,cuser.userid,"postnotify.ok");
  sethomefile(buf2,cuser.userid,"postnotify");
  if(!dashf(buf) && !dashf(buf2))
  {
    pressanykey("�ثe�S������H�]�w�A���s�峹�q��"); 
    return 0;
  } 
  if(dashf(buf2))
    m_postnotify();
  if(answer("�O�_�n���s�f��? (y/N)") == 'y')
  {  
    if(dashf(buf))
    {
      sprintf(genbuf,"/bin/cat %s >> %s",buf,buf2); 
      system(genbuf); 
      unlink(buf);
      m_postnotify();
    } 
  }
}
