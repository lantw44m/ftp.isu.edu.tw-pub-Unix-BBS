#include "bbs.h"

#ifdef USE_NEWCOMER
void newcomers(void) {
   char ci, title[32], path[64], msg[6][80] = {0};
   extern char save_title[];
   FILE *fp;

   strcpy(path, BBSHOME "/tmp");
   sprintf(path, tempnam(path, "newcomer_"));

   clear();
   out2line(1, 10, "新來的朋友嗎 向大家自我介紹一下吧 ! ^_^");
   for(ci = 0 ; ci < 6 ; ci++)
      if(!getdata(ci+12, 0, "", msg[ci], 80, DOECHO, 0)) break;

   if((fp = fopen(path, "w+")) == NULL) return;
#ifndef VEDITOR
   strcpy(save_title, "[新手報到]");
   write_header(fp);
#endif

   fprintf(fp, "大家好 :\n\n我是 %s (%s), 來自 %s\n", cuser.userid,
           cuser.username, cuser.lasthost);
   fprintf(fp, "今天小%s初來此站報到, 請大家多多指教。\n\n\n",
           cuser.classsex == 1 ? "弟" : cuser.classsex == 2 ? "女子" : "的");

   if(ci) fputs("自我介紹 :\n\n", fp);
   for(ci = 0 ; ci < 6 ; ci++) if(*msg[ci]) fprintf(fp, "%s\n", msg[ci]);
   fclose(fp);

   sprintf(title, "新手上路: %s", cuser.username);
//   addorigin(fp);
   void_log('B', path, COMERDIR, title);

   item_update_passwd('l');
   xuser.backup_char[2] |= 1;
   item_update_passwd('u');
}
#endif
