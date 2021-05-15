#include "bbs.h"

#ifdef USE_NEWCOMER
void newcomers(void) {
   char ci, title[32], path[64], msg[6][80] = {0};
   extern char save_title[];
   FILE *fp;

   strcpy(path, BBSHOME "/tmp");
   sprintf(path, tempnam(path, "newcomer_"));

   clear();
   out2line(1, 10, "�s�Ӫ��B�Ͷ� �V�j�a�ۧڤ��Ф@�U�a ! ^_^");
   for(ci = 0 ; ci < 6 ; ci++)
      if(!getdata(ci+12, 0, "", msg[ci], 80, DOECHO, 0)) break;

   if((fp = fopen(path, "w+")) == NULL) return;
#ifndef VEDITOR
   strcpy(save_title, "[�s�����]");
   write_header(fp);
#endif

   fprintf(fp, "�j�a�n :\n\n�ڬO %s (%s), �Ӧ� %s\n", cuser.userid,
           cuser.username, cuser.lasthost);
   fprintf(fp, "���Ѥp%s��Ӧ�������, �Фj�a�h�h���СC\n\n\n",
           cuser.classsex == 1 ? "��" : cuser.classsex == 2 ? "�k�l" : "��");

   if(ci) fputs("�ۧڤ��� :\n\n", fp);
   for(ci = 0 ; ci < 6 ; ci++) if(*msg[ci]) fprintf(fp, "%s\n", msg[ci]);
   fclose(fp);

   sprintf(title, "�s��W��: %s", cuser.username);
//   addorigin(fp);
   void_log('B', path, COMERDIR, title);

   item_update_passwd('l');
   xuser.backup_char[2] |= 1;
   item_update_passwd('u');
}
#endif
