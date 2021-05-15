#include "bbs.h"

/* Dopin 12/04/01 Create / 10/10/02 Modify */
int add_my(char mode, char *board) {
   char flag = 0;
   char arti_level, user_id[15], genbuf[256];
   char access_board[20];
   int i, mun, plus_money, aborted;
   int level_list[4] = { 500, 250, 100, 50 };
   FILE *fp;
   userec mu;

   if(!(is_BM(currBM + 6) || HAS_PERM(PERM_SYSOP))) return DONOTHING;
   clear();

   if(mode) goto GO_ADDM;
   if(!board) {
      move(b_lines - 1, 0);
      strcpy(access_board, currboard);
      sprintf(&genbuf[100], BBSHOME"/boards/%s/import_acsed", currboard);

      if((fp = fopen(&genbuf[100], "r")) != NULL) {
         pressanykey("�z����w�g�ϥιL�����{���F");
         fclose(fp);
         goto SCS;
      }

      sprintf(genbuf, BBSHOME"/boards/%s/allow_add", currboard);
      fp = fopen(genbuf, "r");
      if(fp == NULL) {
         pressanykey("���O�����\\�O�D�ۦ�ϥΥ��{��");
         goto SCS;
      }
      else
        fclose(fp);
   }
   else
      strcpy(access_board, board);

GO_ADDM:
   move(4, 0);
   if(!mode)
      sprintf(genbuf, BBSHOME "/boards/%s/import_articals", access_board);
   else
      strcpy(genbuf, BBSHOME "/adm/m_list");

   if(!board && !mode) ;
   else {
      aborted = vedit(genbuf, NA);
      if(!mode)
         prints("�ݪO [%s] �����ɡG%s",
                access_board, aborted ? "������" : "��s����");
      else
         pressanykey("�ϥΪ��~���� %s", aborted ? "������" : "��s����");

      if(mode) {
         clear();
         more(BBSHOME "/adm/m_date", NA);
      }

      getdata(b_lines-1, 2, !mode ? "�T�w���� y/N ? " :
              "�H�W���W���o�񤧮ɶ� �T�w�o���~�� y/N ? ",
              access_board, 2, LCECHO, 0);

      if(access_board[0] != 'y') {
         pressanykey("�����o��");
         goto SCS;
      }
   }

   fp = fopen(genbuf, "r");
   if(fp == NULL) {
      if(!board) {
         pressanykey("�����ɤ��s�b,�Х��Q�� i ���I����");
         goto SCS;
      }
      else {
         pressanykey("�}�ɥ���...");
         goto SCS;
      }
   }

   clear();
   move(0, 0);
   do {
      fgets(genbuf, 80, fp);

      if(feof(fp) || ferror(fp))
         break;

      if(mode) genbuf[5] = 0;   /* �p�G�O�T�w�~�괣�� �̦h����� �o�̥i�ק� */

      for(i = 15 ; genbuf[i] > ' ' && i < IDLEN + 1 + 15 ; i++);
         genbuf[i] = 0;
      strcpy(user_id, &genbuf[15]);

      if(!mode) {
         arti_level = genbuf[73] - 'b';
         if(arti_level < 0 || arti_level > 4) continue;
         plus_money = level_list[arti_level];
      }
      else plus_money = atoi(genbuf);

      if(mun = getuser(user_id)) {
         memcpy(&mu, &xuser, sizeof(mu));

         mu.havemoney += plus_money;
         if(!mode) mu.good_posts++;
         substitute_record(fn_passwd, &mu, sizeof(mu), mun);
         prints("%-12.12s %7.7d", user_id, plus_money);
      }
   } while(!feof(fp) && !ferror(fp));

   fclose(fp);

   if(!mode) {
      sprintf(genbuf, BBSHOME "/boards/%s/import_articals", access_board);
      if(!board) {
         sprintf(&genbuf[60],BBSHOME "/boards/%s/import_acsed", access_board);
         sprintf(&genbuf[120], "mv %s %s", genbuf, &genbuf[60]);
         system(&genbuf[120]);
      }
      else {
         remove(genbuf);
      }
   }
   else {
      strcpy(genbuf, "date > " BBSHOME "/adm/m_date");
      system(genbuf);
   }

   item_update_passwd('l');
   pressanykey("�B�z����");

SCS:
   if(!board || mode) return FULLUPDATE;
   else return DONOTHING;
}

int add_money(void) {
   add_my(0, NULL);

   return FULLUPDATE;
}

/* Dopin 08/20/02 ��ӧ令�o�� */
int Add_MonS() {
  char genbuf[30];

  if(!HAS_PERM(PERM_SYSOP)) {
     out2line(1, 2, "���{���ȶ}�񤩨� SYSOP �v���̨ϥ�");
     goto ADS_S;
  }

  out2line(1, b_lines, "");
  getdata(b_lines, 2, "���� (1)�o��ݪO�����ڶ� (2)�T�w�~�� (Q)���} : ",
          genbuf, 2, DOECHO, 0);
  if(genbuf[0] > '2' || genbuf[0] < '1') goto ADS_S;

  if(genbuf[0] == '1') {
     make_blist();
     move(1, 0); namecomplete("�п�J���s�פ��ݪO�W ", genbuf);
     if(genbuf[0]) add_my(0, genbuf);
     else pressanykey("����...");
  }
  else add_my(1, NULL);
ADS_S:
  return FULLUPDATE;
}
