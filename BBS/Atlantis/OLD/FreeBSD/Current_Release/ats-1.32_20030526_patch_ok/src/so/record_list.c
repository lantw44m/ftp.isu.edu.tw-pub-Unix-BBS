#include "bbs.h"

/* Dopin: structure for index search records */
struct index_read {
  int no;
  char type;
  char chinese_title[80];
  char full_path[152];
  };
typedef struct index_read index_read;

#ifdef ORDERSONG
char check_task(char *str, char *src, char *des, char *say) {
   char flag = 0, *ptr, *ps[3];
   char *tasks[] = { "<~Src~>", "<~Des~>", "<~Say~>" };
   char cbuf[256];
   int i, element;

   ps[0] = src;
   ps[1] = des;
   ps[2] = say;

   for(i = 0 ; i < 3 ; i++) {
      strcpy(cbuf, str);
      if(ptr = strstr(cbuf, tasks[i])) {
         strcpy(ptr, ps[i]);
         element = ptr - cbuf;
         strcpy(&cbuf[strlen(cbuf)], &str[element + strlen(tasks[i])]);
         flag = 1;
         strcpy(str, cbuf);
      }
   }

   return flag;
}

int do_choice_song(char anonymous, char *to, char *fpath, char *title,
                   char *msg, char mode) {
   char ch, flag = 0, sbuf[70], buf[MAXPATHLEN], genbuf[MAXPATHLEN];
   char *anonymous_msg[] =
   { "�L�W��", "�����W���H", "...", "�R�R���S��������� ..."};
   int i, j;
   FILE *fs, *ft;

#ifdef OSONG_NUM_LIMIT
   if(count_multi() > 1) {  /* �קK���H�Q�Φh���n����Ƥ��P�B Bug �h�I�q */
      pressanykey("�h���n���ɤ��i�I�q");
      goto DO_C_SONG_OV;
   }

   if(cuser.backup_char[1] >= OSONG_NUM_LIMIT) {
      pressanykey("  �@�ѳ̦h�u���I %d ���q", OSONG_NUM_LIMIT);
      goto DO_C_SONG_OV;
   }
#endif

#ifdef SONGCOST
   if(check_money('-', SONGCOST_VALUE) == -1) {
      pressanykey("  ��p �z���������� �I�@���q�ݪ�O %d ��", SONGCOST_VALUE);
      goto DO_C_SONG_OV; /* Dopin: �ڪ��a��f ���w�ΦP�@�Ө禡�X�f */
   }
#endif

   sprintf(sbuf, "%s ", anonymous != 'y' ? cuser.userid : anonymous_msg[0]);
   i = strlen(sbuf);
   if(to[0])
      sprintf(&sbuf[i], "%s %s ", msg[0] ? "��" : "�I��", to);
   else if(!msg[0]) {
      strcpy(&sbuf[i], anonymous_msg[3]);
      goto MSG_SETOV;
   }

   if(msg[0]) {
      i = strlen(sbuf);
      sprintf(&sbuf[i], "�� : %s", msg);
   }

MSG_SETOV:
   sprintf(buf, BBSHOME "/home/%s/OrderSong.temp", cuser.userid);
   fs = fopen(fpath, "r");
   if(!fs) {
      move(1, 2);
      outs("���楢�� �Ь����ȤH�� ... ");
      goto DO_C_SONG_OV;
   }

   ft = fopen(buf, "w+");
   for(i = 0 ; i < 11 ; i++)
      if(fgets(genbuf, 256, fs)) {
         for(j = 0 ; j < 256 && (unsigned char)genbuf[j] > 13 ; j++) ;
            genbuf[j] = 0;
            if(check_task(genbuf,
               anonymous == 'y' ? anonymous_msg[0] : cuser.userid,
               !to[0] ? anonymous_msg[1] : to,
               !msg[0] ? anonymous_msg[2] : msg))
               flag = 1;
               fprintf(ft, "%s\n", genbuf);
      }

   if(!flag) fprintf(ft, "%s", sbuf);
   fclose(fs); fclose(ft);

   strcpy(genbuf, BBSHOME "/adm/songs");
   sprintf(genbuf, tempnam(genbuf, "song_"));
   ft = fopen(genbuf, "w+");
   fputs(sbuf, ft);
   fclose(ft);

   for(i = 0 ; i < 28 && (unsigned char)title[i] > 10 ; i++) ;
   title[i] = 0;
   sprintf(sbuf, "�� <%s> %.28s", cuser.userid, title);
   void_log('S', buf, NULL, sbuf);

#ifdef ORDER_SONG_CAN_MAIL
   if(getuser(to)
#ifndef ANONYMOUS_ORSONG
      && anonymous != 'y'
#endif
   ) {
      Copy(buf, genbuf);
      getdata(mode ? 0 : b_lines-1, 0, " �O�_�@�ֱH�ܥL(�o)���H�c y/N ? ",
              sbuf, 2, LCECHO, 0);
      if(sbuf[0] == 'y') mail2user(to, "[SYSTEM]", " �߱��I�� ", genbuf, 'H');
      out2line(1, mode ? 0 : b_lines-1, "");
   }
#endif

   sprintf(sbuf, "%s �I�� %s", anonymous != 'y' ? cuser.userid : "�L�W��",
           to[0] ? to : "�����W���H");
   void_log('B', buf, "OrderSongs", sbuf);

#ifdef OSONG_NUM_LIMIT
   cuser.backup_char[1]++;
   item_update_passwd('l');
   xuser.backup_char[1] = cuser.backup_char[1];
   item_update_passwd('u');
#endif

   pressanykey("�I������");
DO_C_SONG_OV:
   return DONOTHING;
}
#endif

void do_find_wr(index_read *rp, int num, int rec, int read_ok, int e_mode) {
   char genbuf[10];
   int i, j, k, p, pi, ch, ck;

   clear();
   if(!rec) {
      move(10, 22);
      outs("[1;37;41m��������ŦX����� ����J���N���~��[m ");
      igetkey();
      return;
   }

   pi = p = i = ch = ck = 0;

   while(ch != 'Q' && ch != 'q' && ch != KEY_LEFT) {
      if(pi != i || ch == 0) {
         clear();
         for(j=0, k=0 ; j<num && rp[j].no <= rec; j++)
            if(rp[j].no - i*20 <= 20 && rp[j].no - i*20 > 0) {
               move(k+2, 4);
               prints("%4d   %s", rp[j].no, rp[j].chinese_title);
               k++;
            }
         pi = i;

         move(1, 0);
         prints("[1;30;47m   [1;37;43m �s�� [1;37;44m                   "
                "���D                        %s  [1;37;45m �\\Ū[m",
                e_mode == 3 ? "(C) �I�q" : "         ");

         if(read_ok) outs("[1;37;45m   �i  [m");
         else outs("[1;37;45m   �_  [m");

         out2line(1, b_lines-1,
                  "[1;37;46m  (p)(��)�W�g(n)(��)�U�g (b)/(Space)�W/�U�� (��)"
                  "�\\Ū (/)/($)�̫e/��g (q)���}  [m");
      }

      ch = cursor_key((p%20)+2 , 0);

      if(ch == Ctrl('R')) {
         if(currutmp->msgs.last_pid) {
            show_last_call_in();
            my_write(currutmp->msgs.last_pid, "�ϼu�]�k�^�h�G");
            out2line(1, 0, "");
         }
         ch = 0;
      }

      if(ch >= '0' && ch <= '9') {
         out2line(1, 1, "");
         sprintf(genbuf, "%c", (char)ch);
         getdata(1, 2, "���ܲĴX�� ? ", &genbuf[2], 6, DOECHO, genbuf);
         p = atoi(&genbuf[2]) - 1;
         out2line(1, 1, "");
      }

      if(ch == '$' || ch == 255) {
         p = 9999;
         if(ch == 255)
         ch = 0;
      }

      if(ch == '/' || ch == -255) {
         p = -9999;
         if(ch == -255) ch = 0;
      }

      if(ch == KEY_RIGHT
#ifdef OSONG_SEARCH
        || ((ch == 'c' || ch == 'C') && e_mode == 3)
#endif
      ) {
RT_KY:
         for(k=0 ; k<200 ; k++)
            if(rp[p].full_path[k] <= ' ') {
               rp[p].full_path[k] = 0;
               break;
            }

         if(strstr(rp[p].full_path, "M.")) {
#ifdef OSONG_SEARCH
            if(ch == 'c' || ch == 'C') {
               char af[2], cbf[IDLEN+1], msg1[26];

               getdata(0, 0, " �I�� y/N ? ", cbf, 2, LCECHO, 0);
               if(cbf[0] == 'y') {
                  getdata(0, 14, "�ΦW y/N ? ", af, 2, LCECHO, 0);
                  getdata(0, 27, "�� : ", cbf, IDLEN+1, DOECHO, 0);
                  getdata(0, 33+strlen(cbf), "�T�� : ", msg1, 26, DOECHO, 0);

                  do_choice_song(af[0], cbf, rp[p].full_path,
                                 rp[p].chinese_title, msg1, 0);
               }
               ch = 0;
            }
            else
#endif
            {
               ck = more(rp[p].full_path, YEA);
               if(ck == 1 || ck == 12) ch = 'p';
               else if(ck == 3) ch = 'n';
               else ck = 0;
            }
         }
         else continue;
         pi++;
      }

      if(ch == KEY_DOWN || ch == 'n') p++;
      if(ch == KEY_UP || ch == 'p') p--;
      if(ch == KEY_PGUP || ch == 'b') p -= 20;
      if(ch == KEY_PGDN || ch == ' ') p += 20;
      if(p < 0) p = 0;
      if(p >= rec) p = rec-1;

      while(p / 20 > i || p / 20 < i) {
         if(p / 20 > i) i++;
         if(p / 20 < i) i--;
      }

      if(ck) goto RT_KY;
   }

   return;
}

int find_wr(char f_mode, char *find_brdname) {
   char lastdir[128], pastdir[128], checkdir[128], temp[200], temp1[200];
   char titlelist[128], buf[IDLEN+2], tcbuf[20], board_man_name[80];
   char flag, mode, range;
   int i, j, arti, counts, chk;
   FILE *fp, *fps;
   index_read read_fp[INDEX_READ_NUM];

   item_update_passwd('l');

   clear();
   if(!check_links() && f_mode < 3) {
      move(3, 24);
      prints("[1;37;41m���ݪO���䴩��ذϦ�C�j�M�Ҧ�[m");
      pressanykey(NULL);
      return FULLUPDATE;
   }

   move(2, 10);
   outs(strcmp(currboard, "SongBooks") && f_mode < 3 ?
        "[1;37;42m���\\�ର�Q�Φr����j�M��ذϤ峹���D�Χ@��[m" :
        "        [1;37;42m�ЧQ�κq���W�κt�۪̦W�٨ӧ�M�z�ҭn���q��[m"
   );

   move(4, 10);
   if(f_mode < 3 && strcmp(currboard, "SongBooks")) {
      outs("[1;37;46m�z�i�H�Q�ο�ܷj�M [1;31;46m�@��[1;37;46m , �N��ذ�"
           "���Ҧ��ӧ@�̪��峹������X[m");
   move(5, 10);
      outs("[1;37;46m�άO��ܷj�M [1;33;46m���D[1;37;46m , �N��ذϤ��Ҧ�"
           "�ŦX�r�ꪺ�峹���D���Ƨ�X[m");
   move(6, 10);
      outs("[1;37;46m��i�H��� [1;32;46m���[1;37;46m �j�M�Y��@�̪� �Y��"
           "���D�峹[m");
   }

   move(b_lines - 8, 0);
   if(f_mode < 3 && strcmp(currboard, "SongBooks"))
      outs("  [1;37;41m�]�@�̦W�i��O�������̦W,�Y�L�k�έ�@�̦W�j�M��ɽ�"
           "���O�D�W�ΧQ�μ��D�j�M[m");
   else outs("                  [1;37;41m�j�M��۱��q���۪̥i��J�t�̨۪�@"
             "���m�W[m");

   mode = f_mode;
   strcpy(buf, find_brdname);

   if(mode >= 2) sprintf(temp, BBSHOME "/index/b_%s", buf);
   else sprintf(temp, BBSHOME"/index/%s", buf);

   fp = fopen(temp, "r");
   if(fp == NULL) return FULLUPDATE;

   if(mode < 2) {
      board_man_name[0] = '/';
      strcpy(&board_man_name[1], buf);
   }
   else strcpy(board_man_name, buf);

   if(mode == 1) {
      sprintf(lastdir, BBSHOME"/home/%s/finding", xuser.userid);
      fps = fopen(lastdir, "w+");

      if(fps == NULL) {
         fclose(fp);
         return FULLUPDATE;
      }
   }

   if(f_mode < 3 && strcmp(currboard, SONG_CHOICE_MANDIR)) {
      getdata(b_lines - 4, 0,
      "          �п�ܷj�M���� : (1)�@�� (2)��� (3)���D (Q / Enter)���} : ",
      buf, 2, LCECHO, 0);
      flag = (char)atoi(buf);
   }
   else flag = 3;

   if(flag <= 0 || flag > 3) {
      flag = 0;
      return FULLUPDATE;
   }

   if(f_mode > 2 || !strcmp(currboard, SONG_SEARCH_MANDIR)) {
      getdata(b_lines - 2, 0,
              "  �п�J�q�W�κt�۪̦W �� Enter �d�ߩҦ���� �� Q ���} : ",
              tcbuf, 18, DOECHO, 0);
      if(tcbuf[0] == 'q' || tcbuf[0] == 'Q') return FULLUPDATE;
   }
   else if(flag < 3) {
      move(b_lines - 3, 0);
      usercomplete("          �п�ܭn�j�M���@�̩Φ����� : ", buf, 0);
   }

   if(flag > 1 && flag <= 3 && f_mode <= 2) {
   getdata(b_lines - 2, 0, "          �п�J�������r ", tcbuf, 19,
           DOECHO, 0);
   }

   pastdir[0] = checkdir[0] = 0;
   arti = counts = 0;
   do {
      if(arti >= INDEX_READ_NUM) break;

      fgets(lastdir, 150, fp);
      if(!feof(fp)) {
         if(!strchr(lastdir, '/')) {
            i = 0;
            strcpy(titlelist, lastdir);
            while(lastdir[i] > ' ' && i < 128) i++;

            titlelist[i] = 0;
            if(titlelist[i-1] == '.') titlelist[i-1] = 0;

            if(flag == 1) chk = !strcmp(buf, titlelist);
            if(flag == 2) chk = !strcmp(buf, titlelist) &&
                                (int)strstr(&titlelist[i+1], tcbuf);
            if(flag == 3) chk = (int)strstr(&titlelist[i+1], tcbuf);

            if(chk) {
               if(strcmp(pastdir, checkdir) || !pastdir[0]) {
                  if(mode == 1) {
                     if(!pastdir[0]) sprintf(temp, "\n[1;33;40m%s[1;32;40m"
                                             "/[m\n", board_man_name);
                     else sprintf(temp, "\n[1;33;40m%s[1;32;40m%s[m",
                                        board_man_name, pastdir);
                     fputs(temp, fps);
                  }
               }
               if(mode >= 2) {
                  sprintf(temp, "%s", &lastdir[i+1]);
                  read_fp[counts].no = arti+1;
                  read_fp[counts].type = 'F';
                  strcpy(read_fp[counts].chinese_title, temp);

                  fgets(temp1, 150, fp);
                  for(j=0 ; j<150 ; j++) if(temp1[j] <= ' ') temp1[j] = 0;
                  sprintf(temp, BBSHOME"/man/boards/%s%s",
                  board_man_name, temp1);
                  strcpy(read_fp[counts].full_path, temp);
               }
               else {
                  if(flag == 3) sprintf(temp, "[1;31;40m%s [0;37;40m%s[m",
                                        titlelist, &lastdir[i+1]);
                  else sprintf(temp, "[0;37;40m%s[m", &lastdir[i+1]);
                  fputs(temp, fps);
               }

               strcpy(checkdir, pastdir);
               counts++;
               arti++;
            }
         }
         else strcpy(pastdir, lastdir);
      }
   } while (!feof(fp));

   if(mode == 1) {
      fprintf(fps, "\n�@��� [1;33;40m%d[m �����...\n", arti);
      fclose(fps);
   }
   fclose(fp);

   if(mode > 1)
      if(board_man_name[0]) do_find_wr(read_fp, counts, arti, 1, f_mode);
      else do_find_wr(read_fp, counts, arti, 0, f_mode);
   else {
      sprintf(lastdir, BBSHOME "/home/%s/finding", xuser.userid);
      more(lastdir, "YEA");
      remove(lastdir);
   }
   return 0;
}

#ifdef OSONG_ANN
void access_osong_ann(void) {
   char buf[32];
   FILE *fp;
   song a;

   sprintf(buf, "home/%s/temp_s", cuser.userid);
   if((fp = fopen(buf, "rb")) == NULL) pressanykey("�I�q�Ȧs��Ū������");
   else {
      if(fread(&a, sizeof(a), 1, fp)) {
         fclose(fp);
         do_choice_song(a.type, a.to, a.f_path, a.title, a.msg, 1);
      }
      else pressanykey("�I�q���� �Ь����ȤH��");
   }
}
#endif

#ifdef OSONG_SEARCH
int find_song(void) {
   int i;

#ifdef OSONG_ANN
   i = 'D';
#else
   i = 'S';
#endif

#ifdef OSONG_ANN
#ifdef OSONG_SEARCH
   out2line(1, b_lines, " (S)�ϥηj�M�I�q (D)�ؿ��I�q [S] ? ");
   i = igetkey();

   if(i == 'd' || i == 'D')
#endif
#endif

#ifdef OSONG_ANN
   {
      cuser.extra_mode[6] = 1;
      a_menu("�q�����A", "man/boards/" SONG_CHOICE_MANDIR "/", 0);
      cuser.extra_mode[6] = 0;
      return 0;
   }
#endif

#ifdef OSONG_ANN
#ifdef OSONG_SEARCH
   else
#endif
#endif

#ifdef OSONG_SEARCH
   {
Do_Find_SONG:
   i = find_wr(3, SONG_SEARCH_MANDIR);
   if(!i) goto Do_Find_SONG;
   }
#endif

FIND_SD:
   return i;
}
#endif

int find_cb2(void) {
   int i;

Do_Find_CB2:
   i = find_wr(2, currboard);
   if(!i) goto Do_Find_CB2;

   return i;
}

int find_cb1(void) {
   int i;

Do_Find_CB1:
   i = find_wr(1, currboard);
   if(!i) goto Do_Find_CB1;

   return i;
}
