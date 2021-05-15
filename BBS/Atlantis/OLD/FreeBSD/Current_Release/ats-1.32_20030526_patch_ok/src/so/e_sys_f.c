/* ----------------------------------------------------- */
/* �U�زέp�ά�����T�C�� Dopin: �٥[�F�\�h�t�ΩR�O      */
/* ----------------------------------------------------- */

#include "bbs.h"

void reload_system(void) {
   reload_ucache();
   reload_bcache();
   resetutmpent();
}

void x_edit_system_file(int mode, char *fpath, char *file_name, char *fn) {
   FILE *fp;

   if(!is_station_sysop()) return;

   pressanykey("%s [%s]�G%s", file_name, fn,
   vedit(fpath, NA) ? "������" : "��s����");
}

void e_signed_log(char *fpath, char *fn) {
   char *filename = "�o��ޤJ�����W����";
#ifdef TRANS_ST_TASK
   char cmd_b[256];
#endif

   clear();
   sprintf(fpath, "rm -f ascii/log.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );

   getdata(2, 2, "(1)�s�� (D)�R�� [1] ? ", fn, 2, LCECHO, 0);

   if(fn[0] == 'd') {
      getdata(2, 2, "�T�w�R�������W���� y/N ? ", fn, 2, LCECHO, 0);
      if(fn[0] == 'y') {
         system(fpath);

#ifdef TRANS_ST_TASK
         sprintf(fpath, "rm -f ascii/%s_trans",
  #ifndef NO_USE_MULTI_STATION
                 cuser.station
  #else
                 DEFAULTST
  #endif
         );
         system(fpath);
#endif
      }
   }
   else {
      sprintf(fn, "log.%s",
#ifndef NO_USE_MULTI_STATION
      cuser.station
#else
      DEFAULTST
#endif
      );
      x_edit_system_file(1, (fpath+6), filename, fn);

#ifdef TRANS_ST_TASK
      strcpy(fpath, BBSHOME "/bin/trans_station_ascii");
      sprintf(fn, "ascii/log.%s",
  #ifndef NO_USE_MULTI_STATION
           cuser.station
  #else
           DEFAULTST
  #endif
      );

      sprintf(cmd_b, "%s %s %s %s %s %s", fpath, fn, BBSNAME, MYHOSTNAME,
              MYIPADDRESS,
  #ifndef NO_USE_MULTI_STATION
              cuser.station
  #else
              DEFAULTST
  #endif
      );
      system(cmd_b);
#endif
   }
}

void e_sysop_name(char *fpath, char *fn) {
   char *filename = "�Q�ԴA�����W�U";

   sprintf(fpath, "etc/sysop.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );

   sprintf(fn, "sysop.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );
   x_edit_system_file(1, fpath, filename, fn);
}

int e_board_expire(char *fpath, char *fn) {
   char *filename = "�ݪO����";

   strcpy(fpath, "etc/expire.conf");
   strcpy(fn, "expire.conf");

   x_edit_system_file(1, fpath, filename, fn);
}

int e_board_limit(char *fpath, char *fn) {
   char *filename = "�ݪO�̤j�ƶq����";

   strcpy(fpath, "etc/note.max");
   strcpy(fn, "expire.conf");

   x_edit_system_file(1, fpath, filename, fn);
}

int e_welcome(char *fpath, char *fn) {
   char *filename = "�i�����G��";

   sprintf(fpath, "etc/welcome.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );
   sprintf(fn, "welcome.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );

   x_edit_system_file(1, fpath, filename, fn);
}

int e_anonymous_edit(char mode, char *fpath, char *fn) {
   char *filename[2] = { "�ΦW�ݪO", "��ܦ��ΦW�ݪO" };
   char *fname[2] = { "all_anonymous", "ask_anonymous" };

   sprintf(fpath, "etc/%s", fname[mode]);
   strcpy(fn, fname[mode]);

   x_edit_system_file(1, fpath, filename[mode], fn);
}

#ifdef MultiIssue
void welcome_edit(char mode, char *fpath, char *fn) {
   char ans[3];
   char filename[25];

   if(!dashd("etc/SN")) mkdir("etc/SN", 0770);
   if(!dashd("etc/issue")) mkdir("etc/issue", 0770);

   sprintf(fpath, "mkdir etc/%s/%s",
           mode == 'I' ? "issue" : "SN",
#ifndef NO_USE_MULTI_STATION
           cuser.station
#else
           DEFAULTST
#endif
   );
   system(fpath);

   eolrange(b_lines-5, b_lines);
   getdata(b_lines-1, 0, mode == 'I' ?
"  (?)�s��ĬY�Ӷi���e�� [? = 1 -> 99] (S)�]�w�H���i���e���ƶq (Q)���} [Q] : "
                                     :
"  (?)�s��ĬY�Ӥp�ݪO [? = 1 -> 99] (S)�]�w�ʺA�p�ݪO�ƶq (Q)���} [Q] : ",
   ans, 3, LCECHO, 0);

   if(!ans[0] || !isdigit(ans[0]) && ans[0] != 's') return;
   if(ans[0] == 's') {
      strcpy(fn, "IssueNum");
      strcpy(filename, mode == 'I' ? "�H���i���e��" : "�ʺA�p�ݪO" "�w�q��");

      sprintf(fpath, "etc/%s/%s/IssueNum",
              mode == 'I' ? "issue" : "SN",
#ifndef NO_USE_MULTI_STATION
              cuser.station
#else
              DEFAULTST
#endif
      );
   }
   else {
      fn[0] = (char)atoi(ans);
      if(fn[0] < 1 || fn[0] > 99) return;

      sprintf(filename, "�� %d ��%s", fn[0],
                        mode == 'I' ? "�i���e��" : "�p�ݪO");
      sprintf(fpath, "etc/%s/%s/%d",
              mode == 'I' ? "issue" : "SN",
#ifndef NO_USE_MULTI_STATION
              cuser.station,
#else
              DEFAULTST,
#endif
              fn[0]);
      sprintf(fn, "%d", fn[0]);
   }
      x_edit_system_file(1, fpath, filename, fn);
}
#else
void welcome_edit(char *fpath, char *fn) {
   char *filename = "�i���e��";

   sprintf(fpath, "etc/issue.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );
   sprintf(fn, "issue.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );

   x_edit_system_file(1, fpath, filename, fn);
}
#endif

void x_none_cal(char *fpath, char *fn) {
   char *filename = "���C�J�έp���ݪO�C��";

   sprintf(fpath, "etc/NoStatBoards.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );
   sprintf(fn, "NoStatBoards.%s",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );

   x_edit_system_file(1, fpath, filename, fn);
}

int e_all(void) {
   char row, buf[2], fpath[80], fn[30];

   clear();
   row = b_lines-4;
   move(row++, 2);
   outs("�п�ܭn�s�誺�ɮ� : ");

   if(!cuser.now_stno && HAS_PERM(PERM_SYSOP)) {
      move(row++, 2);
      outs(
   "(A)�ΦW�ݪO (S)��ܦ��ΦW�ݪO (E)�ݪO���� (M)�ݪO�ƶq�W�� (R)��s�t��");
   }

   getdata(row, 2, "(1)���W����(2)���O���ݪO(3)�i���e��(4)���ަW�U(5)���Ȥ��i"
                   "(6)�p�ݪO(Q)���} ",
   buf, 2, LCECHO, 0);

   if(
#ifndef NO_USE_MULTI_STATION
      !cuser.now_stno &&
#endif
      HAS_PERM(PERM_SYSOP)) {
      if(buf[0] == 'a') e_anonymous_edit(0, fpath, fn);
      if(buf[0] == 's') e_anonymous_edit(1, fpath, fn);
      if(buf[0] == 'e') e_board_expire(fpath, fn);
      if(buf[0] == 'm') e_board_limit(fpath, fn);
      if(buf[0] == 'r') reload_system();
   }

   if(buf[0] == '1') e_signed_log(fpath, fn);
   if(buf[0] == '2') x_none_cal(fpath, fn);
   if(buf[0] == '3')
#ifdef  MultiIssue
      welcome_edit('I', fpath, fn);
#else
      welcome_edit(fpath, fn);
#endif
   if(buf[0] == '4') e_sysop_name(fpath, fn);
   if(buf[0] == '5') e_welcome(fpath, fn);
#ifdef USE_LITTLE_NOTE
   if(buf[0] == '6') welcome_edit('S', fpath, fn);
#endif

   return 0;
}
