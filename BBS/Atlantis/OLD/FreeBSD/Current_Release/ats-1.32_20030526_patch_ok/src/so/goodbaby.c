#include "bbs.h"

#ifdef GOOD_BABY
union rej {
  struct {
    char a;
    char b;
    char c;
    char d;
  } A;

  struct {
    usint i;
  } B;
} rtemp = {0};


int reject_me() {
  char ch, buf[4];
  int hr;

  if(belong("etc/reject_me", cuser.userid)) {
    usint now = (usint)time(NULL);

    item_update_passwd('l');
    pressanykey("警告 !!! 一旦設定時效 期間將無法再上站 !! 請審慎考慮");
    getdata(23, 0, "(A)設定多久內不可上站  (B)設定每日何時不能上站 [Q] : ",
            buf, 2, LCECHO, 0);
    ch = buf[0];

    if(ch == 'a') {
      getdata(b_lines, 0, "設定幾小時不可再上站 (1-336) [Q] ? ", buf, 4,
              DOECHO, 0);
      if(buf[0]) {
        hr = atoi(buf);
        if(hr <= 0 && hr > 336) goto SET_ERROR;
        xuser.reject_me_time = now + hr * 3600;
      }
      else goto END_SET;
    }

    if(ch == 'b') {
      getdata(b_lines, 0, "設定每日不能上線開始時間 (0-23) [Enter 取消] ? ",
              buf, 3, DOECHO, 0);
      if(buf[0]) {
        rtemp.A.a = (char)atoi(buf);
        if(rtemp.A.a < 0 || rtemp.A.a > 23) goto SET_ERROR;
      }
      else goto END_SET;

      getdata(b_lines, 0, "設定每日不能上線結束時間 (0-23) [Enter 取消] ? ",
              buf, 3, DOECHO, 0);
      if(buf[0]) {
         rtemp.A.b = (char)atoi(buf);
         if(rtemp.A.b < 0 || rtemp.A.b > 23) goto SET_ERROR;
      }
      else goto END_SET;

      /* Dopin: 同一個時間節區 錯誤 */
      if(rtemp.A.a == rtemp.A.b) goto SET_ERROR;
      else xuser.reject_me_time = rtemp.B.i;

      pressanykey("設定時間自 %d 時至 %d 時", rtemp.A.a, rtemp.A.b);
    }

    cuser.reject_me_time = xuser.reject_me_time;
    item_update_passwd('u');

    getdata(b_lines, 0, "現在就結束 BBS 連線 y/N ? ", buf, 2, LCECHO, 0);
    if(buf[0] == 'y') {
      reset_tty();
      exit(1);
    }
    goto END_SET;

SET_ERROR:
    pressanykey("取消 時限設定錯誤 ...");
  }
  else pressanykey("抱歉 您未授權可以使用本功\能... 如有需要請向站長申請");

END_SET:
  return FULLUPDATE;
}

void check_reject_me() {
  usint now = (usint)time(NULL);

  if(cuser.reject_me_time && belong("etc/reject_me", cuser.userid)) {
    if(cuser.reject_me_time < 65536) {
      char h, fg = 0;

      get_tm_time(NULL, NULL, NULL, &h, NULL);
      rtemp.B.i = cuser.reject_me_time;
      if(rtemp.A.a > rtemp.A.b) { if(h >= rtemp.A.a || h < rtemp.A.b) fg = 1; }
      else                      { if(h >= rtemp.A.a && h < rtemp.A.b) fg = 1; }

      if(fg) {
        pressanykey("您於 %d 時至 %d 時間不得上線", rtemp.A.a, rtemp.A.b);
        goto RESET_TTY;
      }
      else return;
    }
    else {
      if(now < cuser.reject_me_time) {
        pressanykey("抱歉 您還有 %d 分鐘才可上線",
                    (cuser.reject_me_time - now) / 60);
RESET_TTY:
        reset_tty();
        exit(1);
      }
    }
  }
  cuser.reject_me_time = 0;
}
#endif
