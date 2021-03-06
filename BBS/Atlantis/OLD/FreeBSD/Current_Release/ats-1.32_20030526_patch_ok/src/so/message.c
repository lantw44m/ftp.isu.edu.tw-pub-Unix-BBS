#include "bbs.h"
#include <varargs.h>

extern screenline* big_picture;

struct msg {
   char userid[IDLEN+1];
   char message[77];
   char time[6];
};
typedef struct msg msg;

void online_message(char *uident, char *msg) {
   int tuid;
   extern cmpuids();
   user_info *uentp;

   uschar mode0 = currutmp->mode;
   char c0 = currutmp->chatid[0];
   int currstat0 = currstat;

   if((tuid = searchuser(uident)) && tuid != usernum) {
      uentp = (user_info *) search_ulistn(cmpuids, tuid, 1);
      if(!uentp) return;   /* Dopin : 如果不在線上 跳出此程式 */

      if(uentp->mode != XMODE)
         if(HAS_PERM(PERM_SYSOP) || !(is_rejected(uentp) & 2))
            my_write(uentp->pid, msg);
   }
}

void get_msg(va_list pvar) {
   FILE *fp;
   msg msg_list[100];
   char genbuf[200];
   char local_row, past_msg_st = 2;
   int i, j, k, p, pi, ch, fg, record, total, totime, flag = 0, yi, xi, cst;
   screenline *screeng;
   int mode =  va_arg(pvar, int);

   if(cuser.extra_mode[3] == 1) return;

   cst = currstat;
   screeng = calloc(t_lines, sizeof(screenline));
   memcpy(screeng, big_picture, t_lines * sizeof(screenline));
   getyx(&yi, &xi);

   cuser.extra_mode[3] = 1;
#ifdef RECORD_NEW_MSG
   currutmp->msgs.newmsg = 0;
#endif

   if(!mode) {
      if(!currutmp->msgs.last_pid)
#ifdef CTRL_R_REVIEW
         if(cuser.welcomeflag & 2048) goto Quit_GET_MSG;
         else                         goto Init_Get_Msg;
#else
         goto Quit_GET_MSG;
#endif

      show_last_call_in();
#ifdef CTRL_R_REVIEW
      if(!(cuser.welcomeflag & 2048))
         out2line(1, 1, "[1;35;40m────────────────────"
                        "───────────Ctrl+R 回顧模式─[m");
      ch = my_write(currutmp->msgs.last_pid, "回敬魔法: ");
      if(!(cuser.welcomeflag & 2048)) if(ch == 255) goto Init_Get_Msg;
#else
      my_write(currutmp->msgs.last_pid, "回敬魔法: ");
#endif
      goto Quit_GET_MSG;
   }

#ifdef REVIEW_WATER_BALL
Init_Get_Msg:
   if(cuser.uflag & MOVIE_FLAG) {
      flag = 1;
      cuser.uflag ^= MOVIE_FLAG;
   }

First_Load:
   fg = total = 0;
#ifdef RECORD_NEW_MSG
   currutmp->msgs.newmsg = 0;
#endif

Begin_Load:
   setuserfile(genbuf, fn_writelog);
   fp = fopen(genbuf, "r");
   if(fp == NULL)
      goto Quit_GET_MSG;

   record = totime = 0;
   while(!feof(fp) && !ferror(fp) && record < 99) {
      fgets(genbuf, 200, fp);

      if(feof(fp) || ferror(fp)) break;
      if(genbuf[0] == 'T') continue;

      if(genbuf[0] == 27) {
         if(fg == 1) {
            for(i = 0 ; i < 28 && genbuf[i+13] != ' ' ; i++) ;
            genbuf[i+13] = 0;
            strcpy(msg_list[record].userid, &genbuf[13]);

            i += 23;
            j = i;
            while(!(genbuf[i] == ' ' && genbuf[i+1] == 27) && i < 128) i++;
            genbuf[i] = 0;
            strcpy(msg_list[record].message, &genbuf[j]);

            i += 15;
            for(j = i ; j < i + 5 ; j++)
            strcpy(&msg_list[record].time[j-i], &genbuf[j]);
            msg_list[record].time[5] = 0;
         }

         totime++;

         if(!fg) total++;
         else if(total - totime < 99) record++;
      }
   }
   fclose(fp);

   if(fg == 0) {
      fg = 1;
      goto Begin_Load;
   }

   if(!mode) {
      if(record < 6) local_row = (char)record;
      else local_row = 6;
   }
   else local_row = 20;

   if(!mode) {
      eolrange(0, local_row ? local_row+2 : local_row+3);
#ifndef RECORD_NEW_MSG
      out2line(1, 1, "─[1;37;41m 火  球  回  顧 [m──────────"
                     "─[1;33;40mr[m 存入信箱─[1;33;40ms[m 更新訊息─"
                     "[1;33;40mc[m 清除訊息──<");
#endif
      out2line(1, local_row ? local_row+2 : local_row+3,
               "──────────────────────────"
               "[1;33;40m↑↓[m切換─[1;33;40m→[m回訊─[1;33;40m"
               "←[m離開─<");
   }

   pi = p = i = 0;
   ch = 255;

   while(ch != 'Q' && ch != 'q' && ch != KEY_LEFT) {
#ifdef RECORD_NEW_MSG
      if(!mode && (past_msg_st != currutmp->msgs.newmsg)) {
         past_msg_st = currutmp->msgs.newmsg;

         out2line(1, 1, "");
         move(1, 0);
         prints("─[1;37;41m 火  球  回  顧 [m─%s───────[1;33;40mr"
                "[m 存入信箱─[1;33;40ms[m 更新訊息─[1;33;40mc[m 清除訊"
                "息──<",
                currutmp->msgs.newmsg ? "[1;37;41m新訊息[m" : "───");
      }
#endif

      if(pi != i || ch == 0 || ch == 255) {
         if(mode) clear();
         else if(record) eolrange(2, local_row+1);

         if(!record) out2line(0, 2, "                  "
                                    "[1;37;45m☆[1;37;46m 目前並無任何來訊 "
                                    "[1;37;45m☆[m");
         else {
            if(ch == 255) goto Go1;

            for(j = 0, k = 0 ; j < record ; j++)
               if(j - i * local_row < local_row && j - i * local_row >= 0) {
                  if(!mode) {
                     move(k+2, 3);

                     if(strlen(msg_list[j].message) <= 51) prints(
                        "[1;37;41m%3d[1;37;43m %-10.10s [1;37;44m %-54s "
                        "[1;37;42m%5s[m", j+1, msg_list[j].userid,
                        msg_list[j].message, msg_list[j].time);
                     else prints(
                        "[1;37;41m%3d[1;37;43m %-10.10s [1;37;44m %-60.60s"
                        " [m", j+1, msg_list[j].userid, msg_list[j].message);
               }
               else {
                  move(k+2, 4);
                  prints("%3d %5s %-12.11s %-44.44s", j+1, msg_list[j].time,
                        msg_list[j].userid, msg_list[j].message);
               }
               k++;
            }
            pi = i;
         }

         if(mode) {
            move(1, 0);
#ifdef RECORD_NEW_MSG
            prints("[0;30;47m  [1;37;41m 編號 [1;37;42m 時間 [1;37;43m   "
                   "帳號 [1;37;44m 訊息內容  (l)顯示所有訊息(/)最前來訊($)"
                   "最後來訊  %6s [m",
                   currutmp->msgs.newmsg ? "新訊息" : "");
#else
            prints("[0;30;47m  [1;37;41m 編號 [1;37;42m 時間 [1;37;43m   "
            "帳號     [1;37;44m 訊息內容    (l)顯示所有訊息(/)最前來"
            "訊($)最後來訊   [m");
#endif

            move(b_lines-1, 0);
            prints("[1;37;45m  (↑)(↓)/(b)(space)上下篇/頁 (s)更新(c)清除"
                   "(r)存信箱(m)寄信(→)回訊(q)離開   [m");
          }
      }
      ch = cursor_key(record ? p % local_row + 2 : 2, 0);

      if(record && mode && (ch >= '0' && ch <= '9')) {
         out2line(1, 0, "");
         sprintf(genbuf, "%c", (char)ch);
         getdata(0, 2, "跳至第幾項 ? ", &genbuf[100], 4, DOECHO, genbuf);
         p = atoi(&genbuf[100]) - 1;
         out2line(1, 0, "");
      }

      if(record && ch == KEY_RIGHT) {
         online_message(msg_list[p].userid, "送出回應訊息: ");
         out2line(1, 0, "");
         ch = 0;
      }

      if(ch == 'r') {
         getdata(0, 0, "確定將訊息轉至個人信箱並清除之 ? (y/N) : ", genbuf,
                 2, DOECHO, 0);
         out2line(1, 0, "");

         if(!(genbuf[0] == 'y' || genbuf[0] == 'Y')) {
            ch = 0;
            goto Cancel_Remove;
         }
         else {
            setuserfile(genbuf, fn_writelog);
            mail2user(cuser.userid, "[SYSTEM]", " 傳訊記錄 ", genbuf, 'H');
            setuserfile(genbuf, fn_writelog);
            remove(genbuf);
            goto First_Load;
         }
      }

      if(record && mode && ch == 'm') {
          if(check_reject(msg_list[p].userid) && record) {
             clear();
             move(1, 0);
             prints("寄信給 %s", msg_list[p].userid);
             move(4, 0);
             showplans(msg_list[p].userid);
             my_send(msg_list[p].userid);
          }
          ch = 0;
      }

      if(mode && ch == 'l') {
         t_display();
         pi = 255;
         continue;
      }

      if(ch == 'c') {
         getdata(0, 0, "確定清除傳訊 ? (y/N) : ", genbuf, 2, DOECHO, 0);
         out2line(1, 0, "");
         if(!(genbuf[0] == 'y' || genbuf[0] == 'Y')) goto Cancel_Remove;

         setuserfile(genbuf, fn_writelog);
         remove(genbuf);
         goto First_Load;
Cancel_Remove:
         ch = 0;
         continue;
      }

      if(ch == 's') {
         if(!mode) past_msg_st = !currutmp->msgs.newmsg;
         goto First_Load;
      }

      if(record) {
         if(ch == KEY_DOWN || ch == 'n') p++;
         if(ch == KEY_UP || ch == 'p')   p--;
         if(ch == KEY_PGUP || ch == 'b') p -= local_row;
         if(ch == KEY_PGDN || ch == ' ') p += local_row;

Go1:
         if(ch == '$' || ch == 255) {
            p = 9999;
            if(ch == 255) ch = 0;
         }

         if(mode && ch == '/') p = -9999;

         if(p >= record) p = record ? record-1 : 0;
         if(p < 0) p = 0;

         while(p / local_row > i || p / local_row < i) {
            if(p / local_row > i) i++;
            if(p / local_row < i) i--;
         }
      }
   }
#endif

Quit_GET_MSG:
   move(yi, xi);
   memcpy(big_picture, screeng, t_lines * sizeof(screenline));
   free(screeng);
   redoscr();

   if(flag)  cuser.uflag |= MOVIE_FLAG;
   cuser.extra_mode[3] = 0;
   currstat = cst;
}
