#include "bbs.h"
#include "dopin.h"
#include "agree.h"
#include "dl_lib.c"

#define Error_log "./check_agree.log"

/* Dopin 11/24/98 */
usint power(int i, int j) {
   usint res=1, cnt;

   for(cnt = 0 ; cnt < j ; cnt ++) res *= i;
   return res;
}

int HAS_PERM(usint perm) {
   int i;
   usint tp;

   if(!perm) return 1;

   for(i = 0 ; i < NUMPERMS ; i++) {
     tp = power(2, i);
     if(tp & perm) if(cuser.userlevel & tp) return 1;
   }

   return 0;
}

userec cuser;

int main(void) {
   char act, fg, ans[6], genbuf[200];
   int i;
   FILE *fp;

   sprintf(genbuf, "touch %s", LOCKFILE);
   system(genbuf);

   fp = fopen(TARGET, "rb");
   if(fp == NULL) {
      fp = fopen(Error_log, "a+");
      fprintf(fp, "Error Open Target File %s\n", TARGET);
      fclose(fp);
      return 1;
   }

   if(fread(&work, sizeof(work), 1, fp) == 1) fclose(fp);
   else {
      fclose(fp);
      fp = fopen(Error_log, "a+");
      fprintf(fp, "Error Reading Target File (Size Error)\n");
      fclose(fp);
      return 1;
   }

   sprintf(genbuf, "cp " TARGET " " BACKUP);
   system(genbuf);

   for(i = 0 ; i < MAXAGREES ; i++) {
      FILE *fps;
      char fbuf[80], m, d;
      time_t now;

      if(!work[i].title[0] || !work[i].count) continue;

      get_tm_time(NULL, &m, &d, NULL, NULL);
      if(work[i].count < work[i].minlimit &&
         (!strcmp(work[i].end, "--/--") ||
         (check_mmdd(work[i].end, m, d) == 1))
      )
         continue;

      fp = fopen(TEMPFILE, "w+");
      time(&now);
      fprintf(fp, "�@��: [�t�Τ��i] �ݪO: %s\n"
                  "���D: <�s�p> %s\n"
                  "�ɶ�: %s\n\n",
                  POSTBOARD, work[i].title, (char *)Cdate(&now));

      fprintf(fp, "�t�Ϊ��A : ���Ȱ��椽�i���G    �s�p���� : %s\n"
                  "�o�_�H   : %-13s       ��W�s�p : %s\n"
                  "�_�l��� : %-5s               ������� : %s\n"
                  "�s�p���e : %-5d �H            �s�p�H�� : %-5d �H\n"
                  "�s�p���G : %s\n",
                  limit[perm_acs(1, &work[i].perm, 0)-1],
                  work[i].author, work[i].anonymous ? "�_" : "�O",
                  work[i].start, work[i].end,
                  work[i].minlimit, work[i].count,
                  (work[i].minlimit <= work[i].count) ?
                  "���\\" : "����"
             );

      fprintf(fp, "\nñ�p�H���O���M�� :\n");
      set_agree_file(genbuf, i);
      if((fps = fopen(genbuf, "r")) != NULL) {
         while(fgets(fbuf, 80, fps)) {
            if(!work[i].anonymous)
               fprintf(fp, "%s", fbuf);
            else {
               fbuf[13] = 0;
               fprintf(fp, "%-13s %s", "[Anonymous]", &fbuf[14]);
            }
         }
         remove(genbuf);
         fclose(fps);
      }

      addorg(0, fp);
      fclose(fp);
      void_log('B', TEMPFILE, POSTBOARD, work[i].title);

      memset(&work[i], 0, sizeof(ags));
   }

   fp = fopen(TARGET, "wb+");
   fwrite(&work, sizeof(work), 1, fp);
   fclose(fp);

   sprintf(genbuf, "%s", LOCKFILE);
   remove(genbuf);

   return 0;
}
