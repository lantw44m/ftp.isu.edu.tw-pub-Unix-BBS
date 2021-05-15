#include "bbs.h"
#include "agree.h"

void show_ags_record(ags *r) {
   move(3,  2); prints("��  �D : %s", r->title);
   move(4,  2); prints("��  �| : %s", r->fpath);
   move(5,  2); prints("�o�_�� : %s", r->author);
   move(6,  2); prints("�_�l�� : %s", r->start);
   move(7,  2); prints("�פ�� : %s", r->end);
   move(8,  2); prints("�H  �� : %d", r->count);
   move(9,  2); prints("��  �e : %d", r->minlimit);
   move(10, 2); prints("��  �W : %s", r->anonymous ? "�_" : "�O");
   move(11, 2); prints("�v���� : %d", r->perm);
   move(13, 2); prints("���i�� : %s", r->postboard);

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
   prints("[1;37;41m �ثe�B�z�� %3d �� ��� ��Ƥ��e : %s �}��s�p : %s[m",
          number, record->minlimit ? "��" : "�L", record->flag ? "��" : "�L");
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
      pressanykey("�}�ɵo�Ϳ��~ ���ˬd�{���P�ɮ�");
      goto RE_M;
   }

   if(fread(&work, sizeof(work), 1, fp) == 1) fclose(fp);
   else {
      getdata(1, 2, "�{��Ū�ɿ��~ �i�ର�s�� ���ɮ׷l�� �榡�Ʒs�� y/N ?",
              ans, 2, LCECHO, 0);
      if(ans[0] != 'y') {
         pressanykey("��J���N��^��W�@�h �ýЦܤu�@���ˬd���� ...");
         goto FC_M;
      }

      fclose(fp);
      memset(work, 0, sizeof(work));
      pressanykey("�^��W�@�h��� ...");
      goto FO_W;
   }

   ch = ' ';
LOOP_M:
   while(!(ch == KEY_LEFT || ch == 'q' || ch == 'Q')) {
      clear();

      show_record(&work[record], (int)record);
      show_ags_record(&work[record]);
      out2line(0, b_lines, " [1;37;42m   (M)�ק� (D)�R�� (C)�����p�p "
                   "(A)�}��s�p (S)�d�߷N�� (O)�������G (Q)���}   [m ");

      act = fg = 0;
      ch = igetkey();

      if(ch == 'S' || ch == 's') {
         set_agree_file(genbuf, record);
         more (genbuf);
      }

      if(ch == 'o' || ch == 'O') {
         act = 5;

         sprintf(genbuf, "�T�w�M��������� �ñN�s�p��Ƥ��i�� %s �ݪO y/N ? ",
                 work[record].postboard);
         getdata(2, 2, genbuf, ans, 2, LCECHO, 0);

         if(ans[0] == 'y') {
            FILE *fps;
            char fbuf[80];
            time_t now = time(NULL);

            sprintf(genbuf, "cp " TARGET " " BACKUP);
            system(genbuf);

            fp = fopen(TEMPFILE, "w+");

            fprintf(fp, "�@��: [�t�Τ��i] �ݪO: %s\n"
                        "���D: <�s�p> %s\n"
                        "�ɶ�: %s\n\n",
                        work[record].postboard, work[record].title,
                        (char *)Cdate(&now));

            fprintf(fp,
                "�t�Ϊ��A : ���Ȱ��椽�i���G    �s�p���� : %s\n"
                "�o�_�H   : %-13s       ��W�s�p : %s\n"
                "�_�l��� : %-5s               ������� : %s\n"
                "�s�p���e : %-5d �H            �s�p�H�� : %-5d �H\n"
                "�s�p���G : %s\n",
                limit[perm_acs(1, &work[record].perm, 0)-1],
                work[record].author, work[record].anonymous ? "�_" : "�O",
                work[record].start, work[record].end,
                work[record].minlimit, work[record].count
                ,(work[record].minlimit <= work[record].count) ?
                "���\\" : "����"
            );

            fprintf(fp, "\nñ�p�H���O���M�� :\n");
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
            pressanykey("���i���� �^��W�@�h���");

            goto FO_W;
         }
      }

      if(ch == 'd' || ch == 'D') {
         act = 2;
         getdata(2, 2, "�T�w�M��������� y/N ? ", ans, 2, LCECHO, 0);
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

         move(2, 2); prints("����D : %s", temp.title);
         getdata(3, 2, "�s�s�p���D : ", temp.title, 50, DOECHO, 0);
         move(4, 2); prints("����| : %s", temp.fpath);
         getdata(5, 2, "�s���| : ", temp.fpath, 68, DOECHO, 0);
         move(6, 2); prints("��o�_�H�b�� : %s", temp.author);
         getdata(7, 2, "�s�s�p�o�_�H�b�� : ", temp.author, IDLEN+1, DOECHO, 0);
         move(8, 2); prints("��o�_��� : %s", temp.start);
         getdata(9, 2, "�s�s�p�o�_��� mm/dd : ", temp.start, 6, DOECHO, 0);
         move(10, 2); prints("��פ��� : %s", temp.end);
         getdata(11, 2, "�s�s�p������� mm/dd (�L�h --/--) : ",
                       temp.end, 6, DOECHO, 0);
M_RP:
         move(12, 2); prints("��s�p���̧C���e�H�� : %d �H", temp.minlimit);
         getdata(13, 2, "�s�s�p�s�p���e�H�� (MAX 65535) : ",
                 ans, 6, DOECHO, 0);
         i = atoi(ans);
         if(i < atoi(MINLIMIT) || i > 9999) {
            out2line(1, 14,
"  ���e�H�ƿ�J���~ �ݤ��� " MINLIMIT " �� 9999 �H ����J���N�䭫�s��J ..."
            );
            igetkey();
            goto M_RP;
         }
         else temp.minlimit = i;

         move(15, 2);
         prints("��s�p�����v�� : %d",
                perm_acs(1, &temp.perm, NULL));
M_PM:
         getdata(16, 2,
 "�s�s�p���� : (1)�{��(2)�H�c�L�W��(3)�O�d�b��(4)�O�D(5)����(6)����(7)�L : ",
         ans, 2, DOECHO, 0);
         if(ans[0] < '1' || ans[0] > '7') goto M_PM;
         perm_acs(2, &temp.perm, ans[0]);

         move(17, 2);
         prints("��s�p�b���O�� : %s��W", temp.anonymous ? "��" : "");
         getdata(18, 2, "�O�_�]���ΦW�s�p y/N ? ", ans, 2, LCECHO, 0);
         if(ans[0] == 'y') temp.anonymous = 1;
         else temp.anonymous = 0;

         getdata(19, 2, "���i�ܦ�ݪO(�Ъ`�N�j�p�g) : ", temp.postboard,
                 26, DOECHO, POSTBOARD);

         getdata(21, 2, "�бz�T�w y/N ? ", ans, 2, LCECHO, 0);
         if(ans[0] == 'y') {
            work[record] = temp;
            fg = 1;
         }
      }

      if(act && fg) pressanykey("��� %s ����", action[act-1]);
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

   sprintf(buf, "%s%s%s", "  ", str, " ��J���N�����} ...");
   out2line(1, 0, buf);
   igetkey();
   out2line(1, 0, "");
}

int go_agree(ags *record, uschar number) {
   char ans[2], msg[50], buf[100];

   if(check_had_agree(record, number)) {
      error_agree("�z�w�ѥ[�L�����s�p");
      return DONOTHING;
   }
   if(!check_agree(record)) {
      error_agree("�����s�p�|���}�l �Ω|���}��s�p");
      return DONOTHING;
   }

   if(!perm_acs(0, &record->perm, NULL)) {
      error_agree("��p ! �z�õL�v���Ѥ������s�p");
      return DONOTHING;
   }

   if(record->count >= record->minlimit) {
      error_agree("�����s�p�w�F���e�H��");
      return DONOTHING;
   }

   clear();
   more(record->fpath, YEA);

   eolrange(b_lines-2, b_lines);
   sprintf(buf, "�����s�p%s��W �T�w�P�N�s�p y/N ? ",
           record->anonymous ? "��" : "");
   getdata(b_lines-1, 2, buf, ans, 2, LCECHO, 0);
   if(ans[0] == 'y') {
      char mo, da, ho, mi;
      FILE *fp;

      if((fp = fopen(LOCKFILE, "r")) != NULL) {
         fclose(fp);
         error_agree("���ȤH�����b��s�s�p��� �еy�ԦA�s�p");
         goto RET_GA;
      }

      if((fp = fopen(TARGET, "rb")) == NULL) {
         error_agree("�O���t���ɶ}�ҥ��� ���p�����ȤH��");
         goto RET_GA;
      }

      if(fread(&work, sizeof(work), 1, fp) != 1) {
         fclose(fp);
         error_agree("�s�p�t���ɮפj�p���~ ���p�����ȤH��");
         goto RET_GA;
      }

      fp = fopen(TARGET, "wb+");
      fwrite(&work, sizeof(work), 1, fp);
      fclose(fp);

      out2line(1, b_lines-1, "");
      getdata(b_lines-1, 2,
         "�N�� (Enter for None) : ", msg, 50, DOECHO, 0);

      set_agree_file(buf, number);
      get_tm_time(NULL, &mo, &da, &ho, &mi);
      if((fp = fopen(buf, "a+")) == NULL) {
         error_agree("�O���t���ɶ}�ҥ��� ���p�����ȤH��");
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

      pressanykey("���������s�p");
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

         out2line(1, 1, "[0;30;47m  [1;37;41m �s�� [1;37;42m ����  "
                  "[1;37;43m    �o�_��    [1;37;44m �s�p���D        "
                  "                     [1;37;46m �H�� [1;37;45m ���d [m");
         out2line(1, b_lines, "[1;37;45m (��)(��)/(b)(space)�W�U�g/�� "
                  "(S)��s (M)���@ (��)�s�p (q)���} (/)�̫e ($)�̫� [m");
      }

      ch = cursor_key((p%20)+2 , 0);

      if(ch >= '0' && ch <= '9') {
         out2line(1, 0, "");
         sprintf(genbuf, "%c", (char)ch);
         getdata(0, 2, "���ܲĴX�� ? ", &genbuf[100], 4, DOECHO, genbuf);
         p = atoi(&genbuf[100]) - 1;
         out2line(1, 0, "");
         goto Go1;
      }

      if(ch == KEY_RIGHT)
         if(go_agree(&work[p], p) == FULLUPDATE) goto Begin_Load;

      if(ch == 'M' || ch == 'm') {
         if(!HAS_PERM(PERM_SYSOP) && !HAS_PERM(PERM_EXTRA1))
            error_agree("���\\�୭�����P�ݪO�`�ϥ� O_o;;;");
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
