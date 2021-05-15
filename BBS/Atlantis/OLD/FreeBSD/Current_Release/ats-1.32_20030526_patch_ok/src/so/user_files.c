#include "bbs.h"

#define CALL_MSG        3

extern char editflag;

int l_call_msg(void) {
   friend_edit(CALL_MSG);

#ifdef CHECK_LIST_ID_SELF
   check_listname();
#endif

   return FULLUPDATE;
}

void show_favor(void) {
   char chi;
   char *chc[] = { "��", "��" };
   char *mesg[] = {
   "�����W���q���\\��",
   "���جP�i�����",
   "�^���Ƹ�i�����",
   "�a�H�W�椹�\\�H�H",
   "",
   "�����ѩw�I����",
   "�����ѤW���ӷ��Ѭd��",
   "�ݪO�C����ܻ�����",
   "�ϥΪ̤@���ݤ����a�H",

#ifdef INPUT_TOOLS
   "���ϥβŸ���J�u��",
#else
   "",
#endif

#ifdef ARTICAL_CLASS
   "�o��ɤ��ϥΤ�������",
#else
   "",
#endif

#ifdef CTRL_R_REVIEW
   "���ϥ� Ctrl+R ���y�^�U�Ҧ�",
#else
   "",
#endif

#ifdef SHOW_BRDMSG
   "�ݪO�p�۵P����ܬݪO��T",
#else
   "",
#endif

#ifdef NOC_DFB
   "�i�����\\Ū " DEFAULTBOARD " �����i",
#else
   "",
#endif

   "�����Ӧۯ��~�H��",
   "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", ""
   };

   clear();
   for(chi = 0 ; chi < 32 ; chi++) {
      move(2 + chi/2, 40 * (chi%2) + 2);
      prints("%2d %s  %s", chi+1,
      &chc[(power(2 ,(int)chi) & xuser.welcomeflag) / power(2 ,(int)chi)][0],
      &mesg[chi][0]);
   }
}

int t_like(void) {
   FILE *fp1;
   char genbuf[64];
   int i;

   item_update_passwd('l');
Begin_change_favor:
   show_favor();

   getdata(b_lines - 3, 10, "               �п�ܭn�ܧ󪺶��ظ��X : ",
           genbuf, 3, LCECHO, 0);

   i = atoi(genbuf);
   if(i >= 1 && i <= 32) {
      if(power(2, i-1) & xuser.welcomeflag)
         bitwr(&xuser.welcomeflag, i-1, '-');
      else
         bitwr(&xuser.welcomeflag, i-1, '+');

      goto Begin_change_favor;
   }

   getdata(b_lines - 3, 10, "                   �бz�T�w (y/N) : ", genbuf, 2,
           LCECHO, 0);

   if(genbuf[0] == 'y') {
      item_update_passwd('u');
      cuser.welcomeflag = xuser.welcomeflag;
   }

   sprintf(genbuf, BBSHOME "/home/%s/by_pass", xuser.userid);
   if(xuser.welcomeflag & 8) {
      fp1 = fopen(genbuf, "w+");
      fclose(fp1);
   }
   else unlink(genbuf);

   sprintf(genbuf, BBSHOME "/home/%s/reject_imail", xuser.userid);
   if(xuser.welcomeflag & 16384) {
      fp1 = fopen(genbuf, "w+");
      fclose(fp1);
   }
   else unlink(genbuf);

   currutmp->welcomeflag = cuser.welcomeflag;
   return FULLUPDATE;
}

int u_editsig() {
  int aborted;
  char ans[4];
  int j;
  char genbuf[200];

  j = showsignature(genbuf);

  getdata(0, 0, "ñ�W�� (E)�s�� (D)�R�� (Q)�����H[Q] ", ans, 4, LCECHO, 0);

  aborted = 0;
  if(ans[0] == 'd') aborted = 1;
  if(ans[0] == 'e') aborted = 2;

  if(aborted) {
    if(!getdata(1, 0, "�п��ñ�W��(1-9)�H[1] ", ans, 4, DOECHO, 0))
      ans[0] = '1';

    if(ans[0] >= '1' && ans[0] <= '9') {
      genbuf[j] = ans[0];
      if(aborted == 1) {
        unlink(genbuf);
        outs(msg_del_ok);
      }
      else {
        setutmpmode(EDITSIG);
        editflag = 'n';
        aborted = vedit(genbuf, NA);
        if(!aborted) pressanykey("ñ�W�ɧ�s����");
      }
    }
  }

  return 0;
}

int u_editplan() {
  char genbuf[200];

  getdata(b_lines, 0, "�W�� (D)�R�� (E)�s�� [Q]�����H[Q] ", genbuf, 3, LCECHO,
          0);

  if(genbuf[0] == 'e') {
    int aborted;

    setutmpmode(EDITPLAN);
    setuserfile(genbuf, fn_plans);
    editflag = 'n';
    aborted = vedit(genbuf, NA);
    if(!aborted) pressanykey("�W����s����");
  }
  else if(genbuf[0] == 'd') {
    setuserfile(genbuf, fn_plans);
    unlink(genbuf);
    pressanykey("�W���R������");
  }

  return 0;
}

int u_editfiles() {
   char ans[2];

   out2line(b_lines, 0, "");
   getdata(b_lines, 0, "�s�� (A)�W���� (B)ñ�W�� (C)�W�u�q�� (D)��ƱK�X "
           "(E)�ߦn�ﶵ (Q)���} [Q] : ",           ans, 2, LCECHO, 0);

   switch(ans[0]) {
      case 'a':
         u_editplan();
         break;

      case 'b':
         u_editsig();
         break;

      case 'c':
         l_call_msg();
         break;

      case 'd':
         u_info();
         break;

      case 'e':
         t_like();

      default:
         break;
   }
   return FULLUPDATE;
}

int u_register() {
  char rname[20], addr[50];
  char phone[20], career[40], email[50];
  char ans[3], *ptr;
  FILE *fn;
  time_t now;
  char genbuf[200];

  if(HAS_PERM(PERM_LOGINOK)) {
    pressanykey("�z�w�q�L�{�ҤF");
    return 0;
  }

  if(fn = fopen(fn_register, "r")) {
    while(fgets(genbuf, STRLEN, fn)) {
      if(ptr = strchr(genbuf, '\n')) *ptr = '\0';

      if(strncmp(genbuf, "uid: ", 5) == 0 &&
        strcmp(genbuf + 5, cuser.userid) == 0) {
        fclose(fn);
        outs("�z�����U�ӽг�|�b�B�z���A�Э@�ߵ���");
        return XEASY;
      }
    }
    fclose(fn);
  }

  getdata(b_lines-1, 0, "�z�T�w�n��g���U���(Y/N)�H[N] ", ans, 3, LCECHO, 0);
  if(ans[0] != 'y') return 0;

  move(2, 0);
  clrtobot();
  strcpy(rname, cuser.realname);
  strcpy(addr, cuser.address);
  strcpy(email, cuser.email);
  career[0] = phone[0] = '\0';

  while(1) {
    clear();
    move(1, 0);
    prints("%s(%s) �z�n�A�оڹ��g�H�U�����:", cuser.userid, cuser.username);

    getdata(6, 0, "�u��m�W(�ХΤ���): ", rname, 20, DOECHO, rname);
    getdata(8, 0, "�A�ȳ��(�Ǯըt�ũγ��¾��): ", career, 40, DOECHO,
            career);
    getdata(10, 0,  "�ثe��}(�t��ǩΪ��P���X): ", addr, 50, DOECHO, addr);
    getdata(12, 0, "�s���q��(�]�A���~�����ϰ�X): ", phone, 20, DOECHO, phone);
    getdata(14, 0, "E-Mail Address(�����{�ҥ�): ", email, 50, DOECHO, email);

    getdata(b_lines - 1, 0, "�H�W��ƬO�_���T(Y/N)�H(Q)�������U [N] ", ans, 3,
            LCECHO, 0);
    if(ans[0] == 'q') return 0;
    if(ans[0] == 'y') break;
  }

  strcpy(cuser.realname, rname);
  strcpy(cuser.address, addr);
  strcpy(cuser.email, email);

  if(fn = fopen(fn_register, "a")) {
    now = time(NULL);
    trim(career);
    trim(addr);
    trim(phone);
    trim(phone);

    fprintf(fn, "num: %d, %s", usernum, ctime(&now));
    fprintf(fn, "uid: %s\n", cuser.userid);
    fprintf(fn, "name: %s\n", rname);
    fprintf(fn, "career: %s\n", career);
    fprintf(fn, "addr: %s\n", addr);
    fprintf(fn, "phone: %s\n", phone);
    fprintf(fn, "email: %s\n", email);
    fprintf(fn, "----\n");
    fclose(fn);
  }
  return 0;
}
