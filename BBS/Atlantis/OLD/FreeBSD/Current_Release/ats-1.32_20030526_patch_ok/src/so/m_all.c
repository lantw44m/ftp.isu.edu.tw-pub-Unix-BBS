#include "bbs.h"

int mail_all() {
   FILE *fp;
   fileheader mymail;
   char fpath[TTLEN];
   char genbuf[200];
   extern struct UCACHE *uidshm;
   int i, unum;
   char* userid;
   void (*hsig)();


   stand_title("給所有使用者的系統通告");
   setutmpmode(SMAIL);
   getdata(2, 0, "主題：", fpath, 64, DOECHO, 0);
   sprintf(save_title, "[系統通告][1;32m %s[m", fpath);

   setuserfile(fpath, fn_notes);

   if (fp = fopen(fpath, "w")) {
      fprintf(fp, "※ [系統通告] 這是封給所有使用者的信\n");
      fprintf(fp, "----------------------------------------------------------"
      "-----------------\n");
      fclose(fp);
    }

   *quote_file = 0;

   curredit |= EDIT_MAIL;
   if (vedit(fpath, YEA) == -1) {
      curredit = 0;
      unlink(fpath);
      outs(msg_cancel);
      pressanykey(NULL);
      return;
   }
   curredit = 0;

   hsig = signal(SIGHUP, SIG_IGN);

   setutmpmode(MAILALL);
   stand_title("寄信中...");

   sethomepath(genbuf, cuser.userid);
   stampfile(genbuf, &mymail);
   unlink(genbuf);
   Link(fpath, genbuf);
   unlink(fpath);
   strcpy(fpath, genbuf);

   strcpy(mymail.owner, "SYSOP");
   strcpy(mymail.title, save_title);
   mymail.savemode = 0;
   mymail.filemode |= FILE_MARKED;
   sethomedir(genbuf, cuser.userid);
   if (append_record(genbuf, &mymail, sizeof(mymail)) == -1)
      outs(err_uid);

   for(unum = uidshm->number, i = 0; i < unum; i++) {
      userid = uidshm->userid[i];
      if(strcmp(userid, "guest") && strcmp(userid, "new") && strcmp(userid,
         cuser.userid)) {
         sethomepath(genbuf, userid);
         stampfile(genbuf, &mymail);
         unlink(genbuf);
         Link(fpath, genbuf);

         strcpy(mymail.owner, "SYSOP");
         strcpy(mymail.title, save_title);
         mymail.savemode = 0;
         mymail.filemode |= FILE_MARKED;
         sethomedir(genbuf, userid);
         if (append_record(genbuf, &mymail, sizeof(mymail)) == -1)
            outs(err_uid);
         sprintf(genbuf, "%*s %5d / %5d", IDLEN + 1, userid, i + 1, unum);
         outmsg(genbuf);
         refresh();
      }
   }
   signal(SIGHUP,  hsig);
   return;
}
