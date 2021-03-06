#include "bbs.h"
#include "agree.h"

void show_ags_record(ags *r) {
   move(3,  2); prints("標  題 : %s", r->title);
   move(4,  2); prints("路  徑 : %s", r->fpath);
   move(5,  2); prints("發起者 : %s", r->author);
   move(6,  2); prints("起始日 : %s", r->start);
   move(7,  2); prints("終止日 : %s", r->end);
   move(8,  2); prints("人  次 : %d", r->count);
   move(9,  2); prints("門  檻 : %d", r->minlimit);
   move(10, 2); prints("具  名 : %s", r->anonymous ? "否" : "是");
   move(11, 2); prints("權限值 : %d", r->perm);
   move(13, 2); prints("公告於 : %s", r->postboard);

   return;
}

int check_had_agree(ags *record, uschar number) {
   char buf[100];
   FILE *fp;

   set_agree_file(buf, number);
   return belong(buf, cuser.userid);
}

void show_record(ags *record, int number) {
   move(0, 1);
   prints("[1;37;41m 目前處理第 %3d 筆 資料 資料內容 : %s 開放連署 : %s[m",
          number, record->minlimit ? "有" : "無", record->flag ? "有" : "無");
}

int manager_agree(uschar record) {
   char act, fg, ans[6], genbuf[200];
   int i, ch;
   FILE *fp;
   ags temp;

   clear();
   sprintf(genbuf, "touch %s", LOCKFILE);
   system(genbuf);

   fp = fopen(TARGET, "rb");
   if(fp == NULL) {
      pressanykey("開檔發生錯誤 請檢查程式與檔案");
      goto RE_M;
   }

   if(fread(&work, sizeof(work), 1, fp) == 1) fclose(fp);
   else {
      getdata(1, 2, "程式讀檔錯誤 可能為新檔 或檔案損毀 格式化新檔 y/N ?",
              ans, 2, LCECHO, 0);
      if(ans[0] != 'y') {
         pressanykey("鍵入任意鍵回到上一層 並請至工作站檢查舊檔 ...");
         goto FC_M;
      }

      fclose(fp);
      memset(work, 0, sizeof(work));
      pressanykey("回到上一層選單 ...");
      goto FO_W;
   }

   ch = ' ';
LOOP_M:
   while(!(ch == KEY_LEFT || ch == 'q' || ch == 'Q')) {
      clear();

      show_record(&work[record], (int)record);
      show_ags_record(&work[record]);
      out2line(0, b_lines, " [1;37;42m   (M)修改 (D)刪除 (C)關閉聯署 "
                   "(A)開放連署 (S)查詢意見 (O)公布結果 (Q)離開   [m ");

      act = fg = 0;
      ch = igetkey();

      if(ch == 'S' || ch == 's') {
         set_agree_file(genbuf, record);
         more (genbuf);
      }

      if(ch == 'o' || ch == 'O') {
         act = 5;

         sprintf(genbuf, "確定清除本筆資料 並將連署資料公告至 %s 看板 y/N ? ",
                 work[record].postboard);
         getdata(2, 2, genbuf, ans, 2, LCECHO, 0);

         if(ans[0] == 'y') {
            FILE *fps;
            char fbuf[80];
            time_t now = time(NULL);

            sprintf(genbuf, "cp " TARGET " " BACKUP);
            system(genbuf);

            fp = fopen(TEMPFILE, "w+");

            fprintf(fp, "作者: [系統公告] 看板: %s\n"
                        "標題: <連署> %s\n"
                        "時間: %s\n\n",
                        work[record].postboard, work[record].title,
                        (char *)Cdate(&now));

            fprintf(fp,
                "系統狀態 : 站務執行公告結果    連署限制 : %s\n"
                "發起人   : %-13s       具名連署 : %s\n"
                "起始日期 : %-5s               結束日期 : %s\n"
                "連署門檻 : %-5d 人            連署人數 : %-5d 人\n"
                "連署結果 : %s\n",
                limit[perm_acs(1, &work[record].perm, 0)-1],
                work[record].author, work[record].anonymous ? "否" : "是",
                work[record].start, work[record].end,
                work[record].minlimit, work[record].count
                ,(work[record].minlimit <= work[record].count) ?
                "成功\" : "失敗"
            );

            fprintf(fp, "\n簽署人員記錄清單 :\n");
            set_agree_file(genbuf, record);
            if((fps = fopen(genbuf, "r")) != NULL) {
               while(fgets(fbuf, 80, fps))
                  if(!work[record].anonymous)
                     fprintf(fp, "%s", fbuf);
                  else {
                     fbuf[13] = 0;
                     fprintf(fp, "%-13s %s", "[Anonymous]", &fbuf[14]);
                  }
               remove(genbuf);
               fclose(fps);
            }

            addorigin(fp);
            fclose(fp);

            void_log('B', TEMPFILE, work[record].postboard,
                     work[record].title);

            memset(&work[record], 0, sizeof(ags));
            pressanykey("公告完成 回到上一層選單");

            goto FO_W;
         }
      }

      if(ch == 'd' || ch == 'D') {
         act = 2;
         getdata(2, 2, "確定清除此筆資料 y/N ? ", ans, 2, LCECHO, 0);
         if(ans[0] == 'y') {
            memset(&work[record], 0, sizeof(ags));
            fg = 1;
         }
         set_agree_file(genbuf, record);
         remove(genbuf);
      }

      if(ch == 'C' || ch == 'c' || ch == 'A' || ch == 'a') {
         if(ch == 'C' || ch == 'c') fg = 'c';
         else fg = 'a';

         if(fg == 'c') {
            act = 4;
            work[record].flag = 0;
         }
         else {
            act = 3;
            work[record].flag = 1;
         }
      }

      if(ch == 'm' || ch == 'M') {
         act = 1;
         memcpy(&temp, &work[record], sizeof(ags));
         eolrange(2, b_lines-1);

         move(2, 2); prints("原標題 : %s", temp.title);
         getdata(3, 2, "新連署標題 : ", temp.title, 50, DOECHO, 0);
         move(4, 2); prints("原路徑 : %s", temp.fpath);
         getdata(5, 2, "新路徑 : ", temp.fpath, 68, DOECHO, 0);
         move(6, 2); prints("原發起人帳號 : %s", temp.author);
         getdata(7, 2, "新連署發起人帳號 : ", temp.author, IDLEN+1, DOECHO, 0);
         move(8, 2); prints("原發起日期 : %s", temp.start);
         getdata(9, 2, "新連署發起日期 mm/dd : ", temp.start, 6, DOECHO, 0);
         move(10, 2); prints("原終止日期 : %s", temp.end);
         getdata(11, 2, "新連署結束日期 mm/dd (無則 --/--) : ",
                       temp.end, 6, DOECHO, 0);
M_RP:
         move(12, 2); prints("原連署之最低門檻人數 : %d 人", temp.minlimit);
         getdata(13, 2, "新連署連署門檻人數 (MAX 65535) : ",
                 ans, 6, DOECHO, 0);
         i = atoi(ans);
         if(i < atoi(MINLIMIT) || i > 9999) {
            out2line(1, 14,
"  門檻人數輸入有誤 需介於 " MINLIMIT " 至 9999 人 請鍵入任意鍵重新輸入 ..."
            );
            igetkey();
            goto M_RP;
         }
         else temp.minlimit = i;

         move(15, 2);
         prints("原連署條件權限 : %d",
                perm_acs(1, &temp.perm, NULL));
M_PM:
         getdata(16, 2,
 "新連署條件 : (1)認證(2)信箱無上限(3)保留帳號(4)板主(5)站務(6)站長(7)無 : ",
         ans, 2, DOECHO, 0);
         if(ans[0] < '1' || ans[0] > '7') goto M_PM;
         perm_acs(2, &temp.perm, ans[0]);

         move(17, 2);
         prints("原連署帳號記錄 : %s具名", temp.anonymous ? "不" : "");
         getdata(18, 2, "是否設為匿名連署 y/N ? ", ans, 2, LCECHO, 0);
         if(ans[0] == 'y') temp.anonymous = 1;
         else temp.anonymous = 0;

         getdata(19, 2, "公告至何看板(請注意大小寫) : ", temp.postboard,
                 26, DOECHO, POSTBOARD);

         getdata(21, 2, "請您確定 y/N ? ", ans, 2, LCECHO, 0);
         if(ans[0] == 'y') {
            work[record] = temp;
            fg = 1;
         }
      }

      if(act && fg) pressanykey("資料 %s 完成", action[act-1]);
   }

FO_W:
   fp = fopen(TARGET, "wb+");
FW_M:
   fwrite(&work, sizeof(work), 1, fp);
FC_M:
   fclose(fp);
RE_M:
   remove(LOCKFILE);
   return FULLUPDATE;
}

void error_agree(char *str) {
   char buf[100];

   sprintf(buf, "%s%s%s", "  ", str, " 鍵入任意鍵離開 ...");
   out2line(1, 0, buf);
   igetkey();
   out2line(1, 0, "");
}

int go_agree(ags *record, uschar number) {
   char ans[2], msg[50], buf[100];

   if(check_had_agree(record, number)) {
      error_agree("您已參加過本項連署");
      return DONOTHING;
   }
   if(!check_agree(record)) {
      error_agree("本項連署尚未開始 或尚未開放連署");
      return DONOTHING;
   }

   if(!perm_acs(0, &record->perm, NULL)) {
      error_agree("抱歉 ! 您並無權限參予此項連署");
      return DONOTHING;
   }

   if(record->count >= record->minlimit) {
      error_agree("此項連署已達門檻人數");
      return DONOTHING;
   }

   clear();
   more(record->fpath, YEA);

   eolrange(b_lines-2, b_lines);
   sprintf(buf, "此項連署%s具名 確定同意連署 y/N ? ",
           record->anonymous ? "不" : "");
   getdata(b_lines-1, 2, buf, ans, 2, LCECHO, 0);
   if(ans[0] == 'y') {
      char mo, da, ho, mi;
      FILE *fp;

      if((fp = fopen(LOCKFILE, "r")) != NULL) {
         fclose(fp);
         error_agree("站務人員正在更新連署資料 請稍候再連署");
         goto RET_GA;
      }

      if((fp = fopen(TARGET, "rb")) == NULL) {
         error_agree("記錄系統檔開啟失敗 請聯絡站務人員");
         goto RET_GA;
      }

      if(fread(&work, sizeof(work), 1, fp) != 1) {
         fclose(fp);
         error_agree("連署系統檔案大小有誤 請聯絡站務人員");
         goto RET_GA;
      }

      fp = fopen(TARGET, "wb+");
      fwrite(&work, sizeof(work), 1, fp);
      fclose(fp);

      out2line(1, b_lines-1, "");
      getdata(b_lines-1, 2,
         "意見 (Enter for None) : ", msg, 50, DOECHO, 0);

      set_agree_file(buf, number);
      get_tm_time(NULL, &mo, &da, &ho, &mi);
      if((fp = fopen(buf, "a+")) == NULL) {
         error_agree("記錄系統檔開啟失敗 請聯絡站務人員");
         goto RET_GA;
      }

      if(msg[0]) {
         fprintf(fp, "%-13s %-50s %2d/%2d %2d:%2d\n",
                 cuser.userid, msg, mo, da, ho, mi);
         work[number].count++;
      }
      else fprintf(fp, "%-13s %-63s %2d/%2d %2d:%2d\n",
                   cuser.userid, "[0;37;40mNo Data[m", mo, da, ho, mi);
      fclose(fp);

      fp = fopen(TARGET, "wb+");
      fwrite(&work, sizeof(work), 1, fp);
      fclose(fp);

      pressanykey("完成此項連署");
   }

RET_GA:
   return FULLUPDATE;
}

int main_agree(void) {
   char genbuf[200];
   int i, j, k, p, pi, ch, fg = 0, record, total, totime, flag;
   FILE *fp;

   if(cuser.uflag & MOVIE_FLAG) {
      flag = 1;
      cuser.uflag ^= MOVIE_FLAG;
   }
   else flag = 0;
   cuser.extra_mode[3] = 1;

First_Load:
   total = 0;
Begin_Load:
   fp = fopen(TARGET, "rb");
   if(fp == NULL) goto Quit_GET_MSG;

   record = totime = 0;
   memset(work, 0, sizeof(work));
   while(!feof(fp) && !ferror(fp) && record < MAXAGREES) {
      if(fread(&work[record], sizeof(ags), 1, fp)) record++;
      else break;
   }
   fclose(fp);

   if(!fg) {
      p = 0;
      pi = p = i = 0;
      ch = -255;
   }
   else ch = 0;

   clear();

   while(ch != 'Q' && ch != 'q' && ch != KEY_LEFT) {
      fg = 1;
      if(pi != i || ch == 0 || ch == 255 || ch == -255) {
         clear();

         if(!record) {
            move(2, 26);
            outs(err_msg[0]);
         }

         if(ch == 255 || ch == -255)
            goto Go1;

         for(j=0, k=0 ; j < record ; j++)
            if(j - i*20 < 20 && j - i*20 >= 0) {
               move(k+2, 4);
               if(work[j].title[0])
                  prints(
                     "[%d;37;40m%3d %6s  %-12s  [3%1dm%-38.37s%4d  %4d[m",
                     check_agree(&work[j]), j+1, work[j].end, work[j].author,
                     perm_acs(1, &work[j].perm, 0), work[j].title,
                     work[j].count, work[j].minlimit);
               else
                  prints("[0;37;40m%3d %6s %s[m", j+1, "", err_msg[1]);

               k++;
            }
         pi = i;

         out2line(1, 1, "[0;30;47m  [1;37;41m 編號 [1;37;42m 結束  "
                  "[1;37;43m    發起者    [1;37;44m 連署標題        "
                  "                     [1;37;46m 人數 [1;37;45m 關卡 [m");
         out2line(1, b_lines, "[1;37;45m (↑)(↓)/(b)(space)上下篇/頁 "
                  "(S)更新 (M)維護 (→)連署 (q)離開 (/)最前 ($)最後 [m");
      }

      ch = cursor_key((p%20)+2 , 0);

      if(ch >= '0' && ch <= '9') {
         out2line(1, 0, "");
         sprintf(genbuf, "%c", (char)ch);
         getdata(0, 2, "跳至第幾項 ? ", &genbuf[100], 4, DOECHO, genbuf);
         p = atoi(&genbuf[100]) - 1;
         out2line(1, 0, "");
         goto Go1;
      }

      if(ch == KEY_RIGHT)
         if(go_agree(&work[p], p) == FULLUPDATE) goto Begin_Load;

      if(ch == 'M' || ch == 'm') {
         if(!HAS_PERM(PERM_SYSOP) && !HAS_PERM(PERM_EXTRA1))
            error_agree("本功\能限站長與看板總使用 O_o;;;");
         else {
            manager_agree(p);
            goto Begin_Load;
         }
         continue;
      }

      if(ch == KEY_LEFT || ch == 'q' || ch == 'Q') {
Quit_GET_MSG:
         if(flag) cuser.uflag ^= MOVIE_FLAG;
            cuser.extra_mode[3] = 0;
            break;
      }

      if(ch == 's' || ch == 'S') goto First_Load;

      if(ch == KEY_DOWN || ch == 'n') p++;
      if(ch == KEY_UP || ch == 'p') p--;
      if(ch == KEY_PGUP || ch == 'b') p -= 20;
      if(ch == KEY_PGDN || ch == ' ') p += 20;
Go1:
      if(ch == '$' || ch == 255) {
         p = 9999;
         if(ch == 255) ch = 0;
      }

      if(ch == '/' || ch == -255) {
         p = -9999;
         if(ch == -255) ch = 0;
      }

      if(p >= record) p = record-1;
      if(p < 0) p = 0;

      while(p / 20 > i || p / 20 < i) {
         if(p / 20 > i) i++;
         if(p / 20 < i) i--;
      }
   }

   return FULLUPDATE;
}
