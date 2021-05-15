#include "bbs.h"

extern char station_list_cn[][15];
extern int station_num;

/* ----------------------------------------------------- */
/* �U�زέp�ά�����T�C�� Dopin: �٥[�F�\�h�t�ΩR�O      */
/* ----------------------------------------------------- */

void update_bbs_system(char row) {
   char buf[100];

   move(row, 2);
   outs("��J���N��}�l��s BBS �t�� (���ݼƤQ��) �Ъ`�N zbbsd/mbbsd �ݤ�ʭ��] ");
   igetkey();

   strcpy(buf, "make clean update > /dev/null 2>&1");
   chdir(BBSCODE);
   system(buf);

   move(++row, 2);
   outs("����J���N������u ... \n");
   igetkey();

   reset_tty();
   exit(1);
}

struct boardstruct {
   char ident[6];
   usint level;
   char keyword;
   char declare[19];
   char class[40];
};
typedef struct boardstruct boardstruct;

int s_attrib(void) {
   char atb[19], ans[2], gbuf[100];
   int i, j;
   FILE *fp;

   if(cuser.now_stno > 0) return DONOTHING;

   clear();
   for(i = 0 ; i < station_num; i++) {
      move(2+i, 2);
      sprintf(gbuf, "�п�J (%2d) %s ���x���ݩ� (0)���}�� (1)��� [0] : ",
              i+1, station_list_cn[i]);
      outs(gbuf);
      j = strlen(gbuf);
      getdata(2+i, 2+j, "", ans, 2, DOECHO, 0);
      j = atoi(ans);
      if(j) j = 1;
      else j = 0;

      atb[i] = j;
   }
   move(3+i);
   outs("�z����J�ݩʬ� : ");
   for(j = 0 ; j < station_num ; j++)
      outc(atb[j] + '0');

   getdata(5+i, 2, "�бz�T�w y/N ? ", ans, 2, LCECHO, 0);
   if(ans[0] == 'y') {
      fp = fopen(BBSHOME"/stfiles/station_attrib", "wb+");
      move(6+i, 2);
      if(!fp) {
         outs("�}�ɥ���!");
         goto PRES_SA;
      }
      fwrite(&atb, sizeof(char), 19, fp);
      fclose(fp);

      outs("��s����");
   }

PRES_SA:
   pressanykey(NULL);
   return FULLUPDATE;
}

#ifndef NO_USE_MULTI_STATION
update_alls_h(char s_n, char station_name[19][IDLEN+1]) {
   char *include_head = "#include \"bbs.h\"\n#include \"all_s_head.h\"\n\n";
   char *ptc = "\nvoid point_to_classlist(void) {\n";
   int i;
   FILE *fp;

   fp = fopen("src/include/all_stations.h", "w+");
   if(!fp) return -1;

   fprintf(fp, "%s", include_head);

   for(i = 0 ; i < s_n ; i++)
   fprintf(fp, "#include \"../../stfiles/%s.h\"\n", station_name[i]);
   fprintf(fp, "%s\n", ptc);

   for(i = 0 ; i < s_n ; i++)
   fprintf(fp, "   ptr_st[%d] = class%slist;\n", i, station_name[i]);
   fprintf(fp, "\n   return;\n}");

   fclose(fp);

   return 0;
}

int access_ash(void) {
   char station_name[19][IDLEN+1], buf[50];
   int i, j, s_n;
   FILE *fp;

   fp = fopen("stfiles/station_number", "r");
   if(!fp) return -1;
   fgets(buf, 8, fp);
   fclose(fp);
   i = atoi(buf);
   if(i < 1 || i > 19) return -1;
   else s_n = i;

   fp = fopen("stfiles/station_list", "r");
   if(!fp) return -1;
   for(i = 0 ; i < s_n ; i++) {
      fgets(buf, 20, fp);
      if(buf[0] >= 'A') {
         for(j = 0 ; j < IDLEN && buf[j] >= 'A'; j++)
            station_name[i][j] = buf[j];
         station_name[i][j] = 0;
      }
      else break;
   }
   fclose(fp);
   s_n = i;

   return update_alls_h(s_n, station_name);
}
#endif

/* Dopin: ���o�� �H�ŦX�Q���ݭn �H�K���ͪ��s���s�դ����T �Ъ`�N���}�� */
char *h_local = "\n\nlocal_board, 0,    \""
                "00�ȯS�Դ���           ����/�t��\", 0,\n";

#ifndef TRANS_FROM_FB3
char *h_tair =
"Favor, PERM_BASIC, \"FFavor                 �ڪ��̷R\", 0,\n"
"Boards, 0,         \"BBoards �w�w�w�w�w�s�w �Ҧ��ݪO\", 0,\n"
"New, 0,            \"NNew              �u�w �\\\\Ū�Ҧ��s�@�~\", 0,\n"
"Post, PERM_POST,   \"PPost             �u�w �o��峹\", 0,\n"
"Read, 0,           \"RRead             �u�w �\\\\Ū�峹\", 0,\n"
"Select, 0,         \"SSelect           �|�w ��ܬݪO\", 0,\n\n"
"NULL, 0, NULL, 0 };";
#else
char *h_tair =
"Favor, PERM_BASIC, \"FFavor               �ڪ��̷R\", 0,\n"
"Boards, 0,         \"BBoards �w�w�w�w�s�w �Ҧ��ݪO\", 0,\n"
"New, 0,            \"NNew            �u�w �\\\\Ū�Ҧ��s�@�~\", 0,\n"
"Post, PERM_POST,   \"PPost           �u�w �o��峹\", 0,\n"
"Read, 0,           \"RRead           �u�w �\\\\Ū�峹\", 0,\n"
"Select, 0,         \"SSelect         �|�w ��ܬݪO\", 0,\n\n"
"NULL, 0, NULL, 0 };";
#endif
/* ��o�� */

#ifndef NO_USE_MULTI_STATION
int del_s(void) {
   char row, buf[100], stlist[19][IDLEN+1], stlistcn[19][40];
   int i, j, k, mun, stn, dlst, dlsp;
   FILE *fp, *fpr;
   userec mu;

   if(!HAS_PERM(PERM_SYSOP) || cuser.now_stno) return DONOTHING;

   clear();
   strcpy(buf, BBSHOME "/stfiles/station_number");
   fp = fopen(buf, "r");
   if(!fp) return -1;
   fgets(buf, 8, fp);
   fclose(fp);
   stn = atoi(buf);
   if(stn < 1 || stn > 19) return -1;

   strcpy(buf, BBSHOME "/stfiles/station_list");
   fp = fopen(buf, "r");
   if(!fp) return -1;
   strcpy(buf, BBSHOME "/stfiles/station_list_cn");
   fpr = fopen(buf, "r");
   if(!fpr) return -1;

   for(i = 0 ; i < stn ; i++) {
      if(fgets(buf, IDLEN+1, fp)) {
         for(j = 0 ; j < IDLEN && isalpha(buf[j]) ; j++) ;
         buf[j] = 0;
         strcpy(stlist[i], buf);
      }

      if(fgets(buf, 40, fpr)) {
         for(j = 0 ; j < 15 && (unsigned char)buf[j] > '\r'; j++) ;
         buf[j] = 0;
         strcpy(stlistcn[i], buf);
      }
   }
   fclose(fp);
   fclose(fpr);

   for(i = 0 ; i < stn ; i++) if(!isalpha(stlist[i][0])) return -1;

   j = stn / 2;
   if(stn%2) j++;
   for(i = 0 ; i < j; i++) {
      move(i+1, 2);
      prints("[0;37;40m([m%2d[0;37;40m)[m %-13s %s", i*2+1, stlist[i*2], stlistcn[i*2]);
      if(i*2+2 <= stn) {
         move(i+1, 42+26);
         prints("[0;37;40m([m%2d[0;37;40m)[m %-13s %s", i*2+2, stlist[i*2+1], stlistcn[i*2+1]);
      }
   }

   row = (char)i;
   move(row+2, 2);
   sprintf(buf, "�п�ܭn�R�������x (2-%d / Enter for Quit) : ", stn);
   outs(buf);
   j = strlen(buf);
   getdata(row+2, 2+j, "", buf, stn/10 + 2, DOECHO, 0);
   j = atoi(buf);
   if(j < 2 || j > stn) goto CANCEL_DEL_S;

   row++;
   getdata(row+2, 2, "���x�R����L�k�_�� �A�T�{ y/N ? ", buf, 2, LCECHO, 0);
   if(!(buf[0] == 'y')) goto CANCEL_DEL_S;

   dlst = j - 1;
   row++;
   move(row+2, 2);
   outs("Updating : ");

   fp = fopen(BBSHOME "/stfiles/station_number", "w+");
   if(fp) {
      fprintf(fp, "%d\n", stn-1);
      fclose(fp);
      outs("station_number ");
   }

   fp = fopen(BBSHOME "/stfiles/station_list", "w+");
   if(fp) {
      for(j = 0 ; j < stn ; j++)
         if(j != dlst)
            fprintf(fp, "%s\n", stlist[j]);
      outs("station_list ");
   }

   fp = fopen(BBSHOME "/stfiles/station_list_cn", "w+");
   if(fp) {
      for(j = 0 ; j < stn ; j++)
         if(j != dlst)
            fprintf(fp, "%s\n", stlistcn[j]);
      outs("station_list_cn ");
   }

   {
      char station_name[19][IDLEN+1] = {0};
      for(j = 0, k = 0 ; j < stn ; j++)
         if(j != dlst) {
            strcpy(station_name[k], stlist[j]);
            k++;
         }
      if(!update_alls_h(stn-1, station_name))
         outs("all_stations.h ");
   }
   row++;

   sprintf(buf, BBSHOME "/run/%s.sysop_name", stlist[dlst]);
   fp = fopen(buf, "r");
   if(!fp) goto NO_SYSOP_NAME;

   j = 0;
   while (fgets(buf, IDLEN+1, fp)) {
      for(i = 0 ; i < IDLEN+1 && isalpha(buf[i]) ; i++) ;
         buf[i] = 0;
         strcpy(stlistcn[j], buf);
         j++;
   }
   fclose(fp);

   if(j > 0) dlsp = j;
   else goto NO_SYSOP_NAME;

   getdata(row+2, 2, "�@�֩ް��޲z�ӯ����x���s�v�� y/N ? ", buf, 2, LCECHO, 0);
   row++;
   if(!(buf[0] == 'y')) goto NO_SYSOP_NAME;

   k = (int)row;
DLSP:
   row = (char)k;
   j = dlsp / 2;
   if(dlsp%2) j++;
   for(i = 0 ; i < j; i++) {
      char fg;

      if(mun = getuser(stlistcn[i*2])) {
         memcpy(&mu, &xuser, sizeof(mu));
         if(mu.userlevel & PERM_BOARD) fg = '*';
         else fg = '-';
      }
      else fg = '?';

      move(row+i+2, 2);
      prints("(%2d) %s %c", i*2+1, stlistcn[i*2], fg);
      if(i*2+2 <= dlsp) {
         if(mun = getuser(stlistcn[i*2+1])) {
            memcpy(&mu, &xuser, sizeof(mu));
            if(mu.userlevel & PERM_BOARD) fg = '*';
            else fg = '-';
         }
         else fg = '?';

         move(row+i+2, 42);
         prints("(%2d) %s %c", i*2+2, stlistcn[i*2+1], fg);
      }
   }

   row += j;
   sprintf(buf, "��ܲ��������D�v���� ID ���X (1-%d / Enter for Next Step) : ", dlsp);
   move(row+2, 2);
   outs(buf);
   move(row+3, 2);
   prints("%-75s", " ");

   j = strlen(buf);
   getdata(row+2, 2+j, "", buf, 2, LCECHO, 0);
   j = atoi(buf);

   if(j < 1 || j > dlsp) {
      row++; row++;
      goto NO_SYSOP_NAME;
   }

   for(i = 0 ; i < stn ; i++) {
      if(i != dlst) {
         sprintf(buf, BBSHOME "/run/%s.sysop_name", stlist[i]);
         if(belong(buf, stlistcn[j-1])) {
            move(row+3, 2);
            prints("%s ���L�����D�v�d�]�w �L�k���� ...", stlistcn[j-1]);
            goto DLSP;
         }
      }
   }

   if(mun = getuser(stlistcn[j-1])) {
      sprintf(buf, "ID : %-13s S_Code : %d  �T�w�������v�� y/N ? ",
              stlistcn[j-1], mun);
      move(row+3, 2);
      outs(buf);
      j = strlen(buf);
      getdata(row+3, 2+j, "", buf, 2, LCECHO, 0);
      if(buf[0] == 'y') {
         memcpy(&mu, &xuser, sizeof(mu));
         if(mu.userlevel | PERM_BOARD)
            mu.userlevel &= 0xFFFFFFFF - PERM_BOARD;
         substitute_record(fn_passwd, &mu, sizeof(mu), mun);
      }
   }
   goto DLSP;

NO_SYSOP_NAME:
   move(row+2, 2);
   outs("Remove : ");
   sprintf(buf, "rm -f etc/issue.%s", stlist[dlst]);
   system(buf);
   outs("issue ");
   sprintf(buf, "rm -f etc/welcome.%s", stlist[dlst]);
   system(buf);
   outs("welcome ");
   sprintf(buf, "rm -f etc/NoStatBoards.%s", stlist[dlst]);
   system(buf);
   outs("NSB ");
   sprintf(buf, "rm -f etc/sysop.%s", stlist[dlst]);
   system(buf);
   outs("sysop ");
   sprintf(buf, "rm -f ascii/log.%s", stlist[dlst]);
   system(buf);
   outs("log ");
   outs("Note ");
   sprintf(buf, "rm -f run/%s.sysop", stlist[dlst]);
   system(buf);
   prints("sp ", stlist[dlst]);
   sprintf(buf, "rm -f run/%s.sysop_name", stlist[dlst]);
   system(buf);
   outs("_name ");
   sprintf(buf, "rm -f stfiles/%s.h", stlist[dlst]);
   system(buf);
   outs("head");
   row++;

DEL_SB_OK:
   update_bbs_system(row+3);

CANCEL_DEL_S:
   pressanykey("����...");

DEL_S_QUIT:
   return FULLUPDATE;
}

int create_s(void) {
   char row, station[IDLEN+1], sysop[IDLEN+1], station_cn[15], buf[100];
   int i, j;
   FILE *fs;
   userec mu;

   if(!HAS_PERM(PERM_SYSOP) || cuser.now_stno) return DONOTHING;

   clear();
   row = 1;
ENTER_S_NAME:
   getdata(row, 2, "�п�J�n�إߤ��^�寸�W (2-12 �r��) : ", buf, IDLEN+1, DOECHO, 0);
   i = strlen(buf);
   move(row+1, 2);
   if(i < 2) {
      outs("����...");
      goto PS_RE_FULL;
   }

   for(j = 0 ; j < i ; j++)
      if(!isalpha(buf[j])) {
         pressanykey("���W�ȥi�ϥ� �^��r���j�p�g �Э��s��J");
         goto ENTER_S_NAME;
      }

   if(belong("stfiles/station_list", buf)) {
      outs("���W���� �Э��s��J");
      goto ENTER_S_NAME;
   }
   strcpy(station, buf);
   row++;

   getdata(row++, 2, "�п�J�ӯ��x����W : ", station_cn, 15, DOECHO, 0);

   for(i=0 ; i<IDLEN ; i++) buf[i] = 0;
ENTER_S_SYSOP:
   move(row, 2);
   usercomplete("�п�J���x���b�� : ", sysop);
   row++;

   move(row, 2);
   if(!sysop[0] || !sysop[1]) {
      outs("����...");
      goto PS_RE_FULL;
   }

   fs = fopen(BBSHOME "/stfiles/station_number", "r");
   if(!fs) {
      outs("���x�ƥ��� �}�ɥ���!");
      goto PS_RE_FULL;
   }

   fgets(buf, 8, fs);
   fclose(fs);
   i = atoi(buf);
   if(i < 0 || i > 18) {
      outs("���x�ƥةw�q�ɿ��~ �� ���x�Ƥw�F�W��");
      goto PS_RE_FULL;
   }

   for(j = 0 ; j < 6 ; j++) {
      char *n_n[] =
      { "�^�W", "���W" , "�ƥةw�q", "�����s����", "����", "���޼ƥةw�q" };
      char *filename[] =
      { "station_list", "station_list_cn" , "station_number" };
      char *sysopfile[] = { "sysop", "sysop_name" };
      char s_h[14];

      if(j == 3) sprintf(s_h, "%s.h", station);
      if(j < 4) sprintf(buf, BBSHOME "/stfiles/%s", j < 3 ? filename[j] : s_h);
      else sprintf(buf, BBSHOME "/run/%s.%s", station, sysopfile[j-4]);

      fs = fopen(buf, j < 2 ? "a+" : "w+");
      if(!fs) {
          sprintf(buf, "���x%s�� �}�ɥ��� !", n_n[j]);
          outs(buf);
          goto PS_RE_FULL;
      }

      if(j == 0) fprintf(fs, "%s\n", station);
      else if(j == 1) fprintf(fs, "%s\n", station_cn);
      else if(j == 2) fprintf(fs, "%d", i+1);
      else if(j == 3) {
         fprintf(fs, "static struct commands class%slist[] = { ", station);
         fprintf(fs, "%s", h_local);
         fprintf(fs, "%s", h_tair);
      }
      else if(j == 4) fprintf(fs, "1\n");
      else fprintf(fs, "%s\n", sysop);

      fclose(fs);

      row++;
      move(row, 2);
      sprintf(buf, "���x%s�� ��s����", n_n[j]);
      outs(buf);
   }
   row++;

   move(row, 2);
   outs("��s etc �ؿ��U�����ɮ� ... ");
   sprintf(buf, "cp etc/issue.Default etc/issue.%s", station);
   system(buf);
   sprintf(buf, "cp etc/welcome.Default etc/welcome.%s", station);
   system(buf);
   sprintf(buf, "touch etc/NoStatBoards.%s", station);
   system(buf);
   sprintf(buf, "etc/sysop.%s", station);
   fs = fopen(buf, "w+");
   fprintf(fs, "%s        ���Ⱥ޲z", sysop);
   fclose(fs);
   outs("����");
   row++;

   move(row, 2);
   if(!access_ash()) outs("���x���Y�ɧ�s����");
   else {
      outs("���x���Y�ɧ�s���~ ���ˬd����");
      goto PS_RE_FULL;
   }
   row++;

   move(row, 2);
   if(i = getuser(sysop)) {
      memcpy(&mu, &xuser, sizeof(mu));
      mu.userlevel |= PERM_LOGINOK | PERM_BM | PERM_BOARD;
      substitute_record(fn_passwd, &mu, sizeof(mu), i);
      prints("��s�����ӯ��x�� %s ���޲z�v��", sysop);
   }
   else prints("���x�� %s ���޲z�v�� ��s����", sysop);
   row++; row++;

   update_bbs_system(++row);

PS_RE_FULL:
   pressanykey(NULL);
   return FULLUPDATE;
}

int m_station(void) {
   char buf[2];

   clear();
   if(!HAS_PERM(PERM_SYSOP) || cuser.now_stno) {
      pressanykey("���{���� SYSOP ����,�B�ݦb �Ĺs��(�w�])���x");
      return FULLUPDATE;
   }

   getdata(b_lines-2, 0,
   "  (1) �إ߷s���x (2) �R���즳���x (3) �ܧ󯸥x�ݩ� (Q)���} [Q] : ",
   buf, 2, DOECHO, 0);

   if(buf[0] == '1') create_s();
   if(buf[0] == '2') del_s();
   if(buf[0] == '3') s_attrib();

   return FULLUPDATE;
}
#endif

int x_update() {
   boardstruct bs[14] = {0};
   FILE *fp;
   int i, j, k, flag = 1;
   int spc[5] = {0};
   char fpath[40];
   char genbuf[100];
   char headername[80] = {0};

   if(!is_station_sysop()) goto X_UP_END;

   sprintf(fpath, "stfiles/%s.h",
#ifdef NO_USE_MULTI_STATION
           DEFAULTST
#else
           cuser.station
#endif
   );
   fp = fopen(fpath, "r+");
   if(!fp) goto X_UP_END;

   fgets(genbuf, 100, fp);
   strcpy(headername, genbuf);
   for(j = 0 ; j < 80 ; j++)
      if(headername[j] == 10) headername[j] = 0;

   for(i = 1 ; i < 3 ; i++) fgets(genbuf, 100, fp);

   for( ; i < 17 && flag ; i++) {
      fgets(genbuf, 100, fp);
      if(strstr(genbuf, "Favor, PERM_BASIC,")) {
         flag = 0;
         break;
      }

      for(j = 0 , k = 0 ; j < 80 ; j++)
         if(genbuf[j] == ',' || genbuf[j] == '"') {
            genbuf[j] = 0;

            if(k >= 5) break;
            spc[k] = j;
            k++;
         }

      strcpy(bs[i-3].ident, genbuf);
      bs[i-3].level = (usint)atoi(&genbuf[spc[0]+1]);
      bs[i-3].keyword = genbuf[spc[2]+1];

#ifdef TRANS_FROM_FB3
  #define COLN 18
#else
  #define COLN 20
#endif
      for(j = 0 ; j < COLN ; j++) bs[i-3].declare[j] = genbuf[spc[2]+3+j];
      bs[i-3].declare[COLN] = 0;
      for(j = 0 ; j < 39 ; j++) {
         if(genbuf[spc[2]+COLN+4+j] == 0 || spc[2]+COLN+4+j >= spc[3]) break;
         bs[i-3].class[j] = genbuf[spc[2]+COLN+4+j];
      }
      bs[i-3].class[j] = 0;
   }
   fclose(fp);
#undef COLN

   flag = 0;
EDIT_GROUP:
   clear();
   move(1, 0);
   prints(
#ifdef TRANS_FROM_FB3
          " ��  %-10.10s %-4.4s %-18.18s %-38.37s",
#else
          " ��  %-10.10s %-4.4s %-20.20s %-38.37s",
#endif
          "�\\Ū����", "����", "�s�զW��", "�s���k��");

   for(i = 0 ; i < 14 ; i++) {
      move(2+i, 1);
      if(bs[i].keyword)
         prints(
#ifdef TRANS_FROM_FB3
              "%2d  %-10.10X   %c  %-18.18s %-38.37s",
#else
              "%2d  %-10.10X   %c  %-20.20s %-38.37s",
#endif
              i+1, bs[i].level, bs[i].keyword, bs[i].declare, bs[i].class);
      else prints("%2d  �����ƪť�", i+1);
   }

   getdata(b_lines-6, 5, "�п�ܭn�ܧ�/�s�W������ (1~14) [Enter ���}] : ", genbuf, 3, DOECHO, 0);
   i = atoi(genbuf);
   if(i == 0 && !flag) goto X_UP_END;
   else if(i < 0 || i > 14) goto EDIT_GROUP;
   else if(flag && i == 0) goto SAVE_CHE_G;

   i--;
   flag = 1;

HOT_K:
   genbuf[0] = bs[i].keyword;
   genbuf[1] = 0;
   getdata(b_lines-5, 5, "�п�J�s�ռ��� (1~9, A~Z �ŻP��L���䭫��) : ",
           genbuf, 3, DOECHO, genbuf);

   if(genbuf[0] <= '9' && genbuf[0] >= '1' ||
      genbuf[0] <= 'Z' && genbuf[0] >= 'A')
      bs[i].keyword = genbuf[0];
   else goto HOT_K;

GUP_D:
   getdata(b_lines-4, 5, "�п�J�s�զW�� : ",  genbuf, 19, DOECHO,
           bs[i].declare);

   if(genbuf[0]) strcpy(bs[i].declare, genbuf);
   else goto GUP_D;

GUP_C:
   getdata(b_lines-3, 5, "�п�J�s�դ��� : ",
           genbuf, 40, DOECHO, bs[i].class);

   if(genbuf[0] && genbuf[1]) strcpy(bs[i].class, genbuf);
   else goto GUP_C;

GUP_L:
   move(b_lines-1, 5);
   outs("[0]�L���󭭨� [1]�q�L���U�{�� [2]�㦳�ƥ��v�� [3]�㦳���K�ݪO�v��");
   getdata(b_lines-2, 5, "�п�ܬݨ�Ӹs�ժ����� (0~3) [Enter �]���L����] : ",
   genbuf, 2, DOECHO, 0);

   j = atoi(genbuf);
   if(j == 0)      bs[i].level = 0;
   else if(j == 1) bs[i].level = PERM_LOGINOK;
   else if(j == 2) bs[i].level = PERM_RMK1;
   else if(j == 3) bs[i].level = PERM_SECRET;
   else            goto GUP_L;

   strcpy(bs[i].ident, "board");
   goto EDIT_GROUP;

SAVE_CHE_G:
   getdata(b_lines-6, 5, "�T�w�s�J�t���ɮ� [y/N] ? ", genbuf, 2, DOECHO, 0);
   if(genbuf[0] == 'y' || genbuf[0] == 'Y') {
      sprintf(genbuf, "stfiles/%s.h.work",
#ifdef NO_USE_MULTI_STATION
              DEFAULTST
#else
              cuser.station
#endif
      );

      fp = fopen(genbuf, "w+");
      fprintf(fp, "%s%s", headername, h_local);

      for(i = 0 ; i < 14 ; i++) if(bs[i].keyword)
         fprintf(fp,
#ifdef TRANS_FROM_FB3
                 "%5s, %d, \"%c%c%-18.18s %s\", 0,\n",
#else
                 "%5s, %d, \"%c%c%-20.20s %s\", 0,\n",
#endif
                 bs[i].ident, bs[i].level, bs[i].keyword, bs[i].keyword,
                 bs[i].declare, bs[i].class);

      fprintf(fp, "%s", h_tair);
      fclose(fp);
      Rename(genbuf, fpath);
   }

   else goto X_UP_END;

   clear();
   out2line(1, 2, "�Ы����N�����t�Χ�s (���ݼƤQ��), �� 'q' ���_");
   fpath[0] = igetkey();
   if(fpath[0] == 'q') return FULLUPDATE;

   move(3, 0);
   sprintf(fpath, BBSHOME "/run/%s.count",
#ifdef NO_USE_MULTI_STATION
           DEFAULTST
#else
           cuser.station
#endif
   );
   fp = fopen(fpath, "r");
   if(fp == NULL) fp = fopen(fpath, "w+");

   fgets(&fpath[38], 2, fp);
   fclose(fp);
   i = atoi(&fpath[38]);

   if(i >= 5) {
      out2line(1, 6, "���\\��@�ѳ̦h�u�ह�\\��s 5 ��,�Щ��ѦA�s��...");
      goto X_UP_END;
   }
   else {
      i++;
      fp = fopen(fpath, "w+");
      fprintf(fp, "%d", i);
      fclose(fp);
   }

   putchar(0x0d);
   putchar(0x0a);
   strcpy(fpath, "make clean update > /dev/null 2>&1");
   chdir(BBSCODE);
   system(fpath);

   out2line(1, b_lines-6, DEFAULT_BBSD == 'Z'
            ? "��z���n�� �Y�i�ݨ�z���e�ק諸�s��"
            : "�Э��] mbbsd �H�K��s�s�� ���p���z���w�]���x�t�κ޲z��"
           );
   pressanykey("�t�αN��z��J������ᤤ�_�z���s�u");

   reset_tty();
   exit(1);

X_UP_END:
   return FULLUPDATE;
}
