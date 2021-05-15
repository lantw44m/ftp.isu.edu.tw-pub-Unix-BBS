#include "bbs.h"
#include "agree.h"

void show_ags_record(ags *r) {
   move(3,  2); prints("¼Ð  ÃD : %s", r->title);
   move(4,  2); prints("¸ô  ®| : %s", r->fpath);
   move(5,  2); prints("µo°_ªÌ : %s", r->author);
   move(6,  2); prints("°_©l¤é : %s", r->start);
   move(7,  2); prints("²×¤î¤é : %s", r->end);
   move(8,  2); prints("¤H  ¦¸ : %d", r->count);
   move(9,  2); prints("ªù  Âe : %d", r->minlimit);
   move(10, 2); prints("¨ã  ¦W : %s", r->anonymous ? "§_" : "¬O");
   move(11, 2); prints("Åv­­­È : %d", r->perm);
   move(13, 2); prints("¤½§i©ó : %s", r->postboard);

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
   prints("[1;37;41m ¥Ø«e³B²z²Ä %3d µ§ ¸ê®Æ ¸ê®Æ¤º®e : %s ¶}©ñ³s¸p : %s[m",
          number, record->minlimit ? "¦³" : "µL", record->flag ? "¦³" : "µL");
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
      pressanykey("¶}ÀÉµo¥Í¿ù»~ ½ÐÀË¬dµ{¦¡»PÀÉ®×");
      goto RE_M;
   }

   if(fread(&work, sizeof(work), 1, fp) == 1) fclose(fp);
   else {
      getdata(1, 2, "µ{¦¡ÅªÀÉ¿ù»~ ¥i¯à¬°·sÀÉ ©ÎÀÉ®×·l·´ ®æ¦¡¤Æ·sÀÉ y/N ?",
              ans, 2, LCECHO, 0);
      if(ans[0] != 'y') {
         pressanykey("Áä¤J¥ô·NÁä¦^¨ì¤W¤@¼h ¨Ã½Ð¦Ü¤u§@¯¸ÀË¬dÂÂÀÉ ...");
         goto FC_M;
      }

      fclose(fp);
      memset(work, 0, sizeof(work));
      pressanykey("¦^¨ì¤W¤@¼h¿ï³æ ...");
      goto FO_W;
   }

   ch = ' ';
LOOP_M:
   while(!(ch == KEY_LEFT || ch == 'q' || ch == 'Q')) {
      clear();

      show_record(&work[record], (int)record);
      show_ags_record(&work[record]);
      out2line(0, b_lines, " [1;37;42m   (M)­×§ï (D)§R°£ (C)Ãö³¬Áp¸p "
                   "(A)¶}©ñ³s¸p (S)¬d¸ß·N¨£ (O)¤½¥¬µ²ªG (Q)Â÷¶}   [m ");

      act = fg = 0;
      ch = igetkey();

      if(ch == 'S' || ch == 's') {
         set_agree_file(genbuf, record);
         more (genbuf);
      }

      if(ch == 'o' || ch == 'O') {
         act = 5;

         sprintf(genbuf, "½T©w²M°£¥»µ§¸ê®Æ ¨Ã±N³s¸p¸ê®Æ¤½§i¦Ü %s ¬ÝªO y/N ? ",
                 work[record].postboard);
         getdata(2, 2, genbuf, ans, 2, LCECHO, 0);

         if(ans[0] == 'y') {
            FILE *fps;
            char fbuf[80];
            time_t now = time(NULL);

            sprintf(genbuf, "cp " TARGET " " BACKUP);
            system(genbuf);

            fp = fopen(TEMPFILE, "w+");

            fprintf(fp, "§@ªÌ: [¨t²Î¤½§i] ¬ÝªO: %s\n"
                        "¼ÐÃD: <³s¸p> %s\n"
                        "®É¶¡: %s\n\n",
                        work[record].postboard, work[record].title,
                        (char *)Cdate(&now));

            fprintf(fp,
                "¨t²Îª¬ºA : ¯¸°È°õ¦æ¤½§iµ²ªG    ³s¸p­­¨î : %s\n"
                "µo°_¤H   : %-13s       ¨ã¦W³s¸p : %s\n"
                "°_©l¤é´Á : %-5s               µ²§ô¤é´Á : %s\n"
                "³s¸pªùÂe : %-5d ¤H            ³s¸p¤H¼Æ : %-5d ¤H\n"
                "³s¸pµ²ªG : %s\n",
                limit[perm_acs(1, &work[record].perm, 0)-1],
                work[record].author, work[record].anonymous ? "§_" : "¬O",
                work[record].start, work[record].end,
                work[record].minlimit, work[record].count
                ,(work[record].minlimit <= work[record].count) ?
                "¦¨¥\\" : "¥¢±Ñ"
            );

            fprintf(fp, "\nÃ±¸p¤H­û°O¿ý²M³æ :\n");
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
            pressanykey("¤½§i§¹¦¨ ¦^¨ì¤W¤@¼h¿ï³æ");

            goto FO_W;
         }
      }

      if(ch == 'd' || ch == 'D') {
         act = 2;
         getdata(2, 2, "½T©w²M°£¦¹µ§¸ê®Æ y/N ? ", ans, 2, LCECHO, 0);
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

         move(2, 2); prints("­ì¼ÐÃD : %s", temp.title);
         getdata(3, 2, "·s³s¸p¼ÐÃD : ", temp.title, 50, DOECHO, 0);
         move(4, 2); prints("­ì¸ô®| : %s", temp.fpath);
         getdata(5, 2, "·s¸ô®| : ", temp.fpath, 68, DOECHO, 0);
         move(6, 2); prints("­ìµo°_¤H±b¸¹ : %s", temp.author);
         getdata(7, 2, "·s³s¸pµo°_¤H±b¸¹ : ", temp.author, IDLEN+1, DOECHO, 0);
         move(8, 2); prints("­ìµo°_¤é´Á : %s", temp.start);
         getdata(9, 2, "·s³s¸pµo°_¤é´Á mm/dd : ", temp.start, 6, DOECHO, 0);
         move(10, 2); prints("­ì²×¤î¤é´Á : %s", temp.end);
         getdata(11, 2, "·s³s¸pµ²§ô¤é´Á mm/dd (µL«h --/--) : ",
                       temp.end, 6, DOECHO, 0);
M_RP:
         move(12, 2); prints("­ì³s¸p¤§³Ì§CªùÂe¤H¼Æ : %d ¤H", temp.minlimit);
         getdata(13, 2, "·s³s¸p³s¸pªùÂe¤H¼Æ (MAX 65535) : ",
                 ans, 6, DOECHO, 0);
         i = atoi(ans);
         if(i < atoi(MINLIMIT) || i > 9999) {
            out2line(1, 14,
"  ªùÂe¤H¼Æ¿é¤J¦³»~ »Ý¤¶©ó " MINLIMIT " ¦Ü 9999 ¤H ½ÐÁä¤J¥ô·NÁä­«·s¿é¤J ..."
            );
            igetkey();
            goto M_RP;
         }
         else temp.minlimit = i;

         move(15, 2);
         prints("­ì³s¸p±ø¥óÅv­­ : %d",
                perm_acs(1, &temp.perm, NULL));
M_PM:
         getdata(16, 2,
 "·s³s¸p±ø¥ó : (1)»{ÃÒ(2)«H½cµL¤W­­(3)«O¯d±b¸¹(4)ªO¥D(5)¯¸°È(6)¯¸ªø(7)µL : ",
         ans, 2, DOECHO, 0);
         if(ans[0] < '1' || ans[0] > '7') goto M_PM;
         perm_acs(2, &temp.perm, ans[0]);

         move(17, 2);
         prints("­ì³s¸p±b¸¹°O¿ý : %s¨ã¦W", temp.anonymous ? "¤£" : "");
         getdata(18, 2, "¬O§_³]¬°°Î¦W³s¸p y/N ? ", ans, 2, LCECHO, 0);
         if(ans[0] == 'y') temp.anonymous = 1;
         else temp.anonymous = 0;

         getdata(19, 2, "¤½§i¦Ü¦ó¬ÝªO(½Ðª`·N¤j¤p¼g) : ", temp.postboard,
                 26, DOECHO, POSTBOARD);

         getdata(21, 2, "½Ð±z½T©w y/N ? ", ans, 2, LCECHO, 0);
         if(ans[0] == 'y') {
            work[record] = temp;
            fg = 1;
         }
      }

      if(act && fg) pressanykey("¸ê®Æ %s §¹¦¨", action[act-1]);
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

   sprintf(buf, "%s%s%s", "  ", str, " Áä¤J¥ô·NÁäÂ÷¶} ...");
   out2line(1, 0, buf);
   igetkey();
   out2line(1, 0, "");
}

int go_agree(ags *record, uschar number) {
   char ans[2], msg[50], buf[100];

   if(check_had_agree(record, number)) {
      error_agree("±z¤w°Ñ¥[¹L¥»¶µ³s¸p");
      return DONOTHING;
   }
   if(!check_agree(record)) {
      error_agree("¥»¶µ³s¸p©|¥¼¶}©l ©Î©|¥¼¶}©ñ³s¸p");
      return DONOTHING;
   }

   if(!perm_acs(0, &record->perm, NULL)) {
      error_agree("©êºp ! ±z¨ÃµLÅv­­°Ñ¤©¦¹¶µ³s¸p");
      return DONOTHING;
   }

   if(record->count >= record->minlimit) {
      error_agree("¦¹¶µ³s¸p¤w¹FªùÂe¤H¼Æ");
      return DONOTHING;
   }

   clear();
   more(record->fpath, YEA);

   eolrange(b_lines-2, b_lines);
   sprintf(buf, "¦¹¶µ³s¸p%s¨ã¦W ½T©w¦P·N³s¸p y/N ? ",
           record->anonymous ? "¤£" : "");
   getdata(b_lines-1, 2, buf, ans, 2, LCECHO, 0);
   if(ans[0] == 'y') {
      char mo, da, ho, mi;
      FILE *fp;

      if((fp = fopen(LOCKFILE, "r")) != NULL) {
         fclose(fp);
         error_agree("¯¸°È¤H­û¥¿¦b§ó·s³s¸p¸ê®Æ ½Ðµy­Ô¦A³s¸p");
         goto RET_GA;
      }

      if((fp = fopen(TARGET, "rb")) == NULL) {
         error_agree("°O¿ý¨t²ÎÀÉ¶}±Ò¥¢±Ñ ½ÐÁpµ¸¯¸°È¤H­û");
         goto RET_GA;
      }

      if(fread(&work, sizeof(work), 1, fp) != 1) {
         fclose(fp);
         error_agree("³s¸p¨t²ÎÀÉ®×¤j¤p¦³»~ ½ÐÁpµ¸¯¸°È¤H­û");
         goto RET_GA;
      }

      fp = fopen(TARGET, "wb+");
      fwrite(&work, sizeof(work), 1, fp);
      fclose(fp);

      out2line(1, b_lines-1, "");
      getdata(b_lines-1, 2,
         "·N¨£ (Enter for None) : ", msg, 50, DOECHO, 0);

      set_agree_file(buf, number);
      get_tm_time(NULL, &mo, &da, &ho, &mi);
      if((fp = fopen(buf, "a+")) == NULL) {
         error_agree("°O¿ý¨t²ÎÀÉ¶}±Ò¥¢±Ñ ½ÐÁpµ¸¯¸°È¤H­û");
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

      pressanykey("§¹¦¨¦¹¶µ³s¸p");
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

         out2line(1, 1, "[0;30;47m  [1;37;41m ½s¸¹ [1;37;42m µ²§ô  "
                  "[1;37;43m    µo°_ªÌ    [1;37;44m ³s¸p¼ÐÃD        "
                  "                     [1;37;46m ¤H¼Æ [1;37;45m Ãö¥d [m");
         out2line(1, b_lines, "[1;37;45m (¡ô)(¡õ)/(b)(space)¤W¤U½g/­¶ "
                  "(S)§ó·s (M)ºûÅ@ (¡÷)³s¸p (q)Â÷¶} (/)³Ì«e ($)³Ì«á [m");
      }

      ch = cursor_key((p%20)+2 , 0);

      if(ch >= '0' && ch <= '9') {
         out2line(1, 0, "");
         sprintf(genbuf, "%c", (char)ch);
         getdata(0, 2, "¸õ¦Ü²Ä´X¶µ ? ", &genbuf[100], 4, DOECHO, genbuf);
         p = atoi(&genbuf[100]) - 1;
         out2line(1, 0, "");
         goto Go1;
      }

      if(ch == KEY_RIGHT)
         if(go_agree(&work[p], p) == FULLUPDATE) goto Begin_Load;

      if(ch == 'M' || ch == 'm') {
         if(!HAS_PERM(PERM_SYSOP) && !HAS_PERM(PERM_EXTRA1))
            error_agree("¥»¥\\¯à­­¯¸ªø»P¬ÝªOÁ`¨Ï¥Î O_o;;;");
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
