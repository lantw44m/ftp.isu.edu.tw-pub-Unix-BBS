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
   char *chc[] = { "□", "■" };
   char *mesg[] = {
   "關閉上站通知功\能",
   "近日壽星進站顯示",
   "英雄事跡進站顯示",
   "壞人名單允許\寄信",
   "",
   "不提供定點報時",
   "不提供上站來源供查詢",
   "看板列表不顯示說明欄",
   "使用者一覽看不到壞人",

#ifdef INPUT_TOOLS
   "不使用符號輸入工具",
#else
   "",
#endif

#ifdef ARTICAL_CLASS
   "發文時不使用分類項目",
#else
   "",
#endif

#ifdef CTRL_R_REVIEW
   "不使用 Ctrl+R 水球回顧模式",
#else
   "",
#endif

#ifdef SHOW_BRDMSG
   "看板小招牌不顯示看板資訊",
#else
   "",
#endif

#ifdef NOC_DFB
   "進站不閱\讀 " DEFAULTBOARD " 的公告",
#else
   "",
#endif

   "不收來自站外信件",
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

   getdata(b_lines - 3, 10, "               請選擇要變更的項目號碼 : ",
           genbuf, 3, LCECHO, 0);

   i = atoi(genbuf);
   if(i >= 1 && i <= 32) {
      if(power(2, i-1) & xuser.welcomeflag)
         bitwr(&xuser.welcomeflag, i-1, '-');
      else
         bitwr(&xuser.welcomeflag, i-1, '+');

      goto Begin_change_favor;
   }

   getdata(b_lines - 3, 10, "                   請您確定 (y/N) : ", genbuf, 2,
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

  getdata(0, 0, "簽名檔 (E)編輯 (D)刪除 (Q)取消？[Q] ", ans, 4, LCECHO, 0);

  aborted = 0;
  if(ans[0] == 'd') aborted = 1;
  if(ans[0] == 'e') aborted = 2;

  if(aborted) {
    if(!getdata(1, 0, "請選擇簽名檔(1-9)？[1] ", ans, 4, DOECHO, 0))
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
        if(!aborted) pressanykey("簽名檔更新完畢");
      }
    }
  }

  return 0;
}

int u_editplan() {
  char genbuf[200];

  getdata(b_lines, 0, "名片 (D)刪除 (E)編輯 [Q]取消？[Q] ", genbuf, 3, LCECHO,
          0);

  if(genbuf[0] == 'e') {
    int aborted;

    setutmpmode(EDITPLAN);
    setuserfile(genbuf, fn_plans);
    editflag = 'n';
    aborted = vedit(genbuf, NA);
    if(!aborted) pressanykey("名片更新完畢");
  }
  else if(genbuf[0] == 'd') {
    setuserfile(genbuf, fn_plans);
    unlink(genbuf);
    pressanykey("名片刪除完畢");
  }

  return 0;
}

int u_editfiles() {
   char ans[2];

   out2line(b_lines, 0, "");
   getdata(b_lines, 0, "編輯 (A)名片檔 (B)簽名檔 (C)上線通知 (D)資料密碼 "
           "(E)喜好選項 (Q)離開 [Q] : ",           ans, 2, LCECHO, 0);

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
    pressanykey("您已通過認證了");
    return 0;
  }

  if(fn = fopen(fn_register, "r")) {
    while(fgets(genbuf, STRLEN, fn)) {
      if(ptr = strchr(genbuf, '\n')) *ptr = '\0';

      if(strncmp(genbuf, "uid: ", 5) == 0 &&
        strcmp(genbuf + 5, cuser.userid) == 0) {
        fclose(fn);
        outs("您的註冊申請單尚在處理中，請耐心等候");
        return XEASY;
      }
    }
    fclose(fn);
  }

  getdata(b_lines-1, 0, "您確定要填寫註冊單嗎(Y/N)？[N] ", ans, 3, LCECHO, 0);
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
    prints("%s(%s) 您好，請據實填寫以下的資料:", cuser.userid, cuser.username);

    getdata(6, 0, "真實姓名(請用中文): ", rname, 20, DOECHO, rname);
    getdata(8, 0, "服務單位(學校系級或單位職稱): ", career, 40, DOECHO,
            career);
    getdata(10, 0,  "目前住址(含寢室或門牌號碼): ", addr, 50, DOECHO, addr);
    getdata(12, 0, "連絡電話(包括長途撥號區域碼): ", phone, 20, DOECHO, phone);
    getdata(14, 0, "E-Mail Address(身分認證用): ", email, 50, DOECHO, email);

    getdata(b_lines - 1, 0, "以上資料是否正確(Y/N)？(Q)取消註冊 [N] ", ans, 3,
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
