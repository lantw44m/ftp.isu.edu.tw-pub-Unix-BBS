#include "bbs.h"

extern void do_hold_mail(char *, char *, char *);

int m_group() {
  int aborted = 0;
  char ans[4];
  static char eans[4];
  int j;
  char genbuf[200];

  setuserfile(genbuf, "mlist.0");
  j = strlen(genbuf) - 1;

  getdata(b_lines - 1, 0, "Email-list (E)�s�� (D)�R�� (M)�o�H (Q)�����H[Q] ",
          ans, 4, LCECHO, 0);

  if(ans[0] == 'd') aborted = 1;
  if(ans[0] == 'e') aborted = 2;
  if(ans[0] == 'm') aborted = 3;

  if(aborted) {
    if(!getdata(b_lines - 1, 0, "�п�� Email-list ��(0-9)�H ", eans, 4,
      DOECHO, eans)) eans[0] = '0';

    if(eans[0] >= '0' && eans[0] <= '9') {
      genbuf[j] = eans[0];
      if(aborted == 1) {
        unlink(genbuf);
        outs(msg_del_ok);
      }
      else if(aborted == 2) {
        aborted = vedit(genbuf, NA);
        if(!aborted) outs("��s����");
      }
      else if (more(genbuf, NA) != -1) {
         char address[STRLEN];
         char xtitle[TTLEN];
         char fpath[STRLEN] = "";
         FILE* fp;

         getdata(b_lines - 1, 0, "�D�D�G", xtitle, TTLEN, DOECHO, 0);
         if (!*xtitle)
            return -2;
         curredit |= EDIT_MAIL;
         setutmpmode(SMAIL);

         if (vedit(fpath, NA) == -1) {
            unlink(fpath);
            clear();
            return -2;
         }
         clear();
         more(genbuf, NA);
         getdata(b_lines - 1, 0, "�T�w�n�H���C�����H��? (Y/N) [Y]", ans, 4,
                 LCECHO, 0);
         switch (*ans)
         {
         case 'n':
            outs("\n�H��w����");
            break;
         default:
            fp = fopen(genbuf, "r");
            while (fgets(address, STRLEN, fp)) {
               strtok(address, " ,\n");
               if (*address) {
                  outmsg(address);
                  refresh();
                  bbs_sendmail(fpath, xtitle, address);
               }
            }
            fclose(fp);
            getdata(b_lines - 1, 0, "�w���Q�H�X�A�O�_�ۦs���Z(Y/N)�H[N]",
               ans, 4, LCECHO, 0);
            if (*ans == 'y') {
               sprintf(save_title, "%s (mlist.%c)", xtitle, *eans);
               do_hold_mail(fpath, 0, cuser.userid);
            }
         }
         unlink(fpath);
      }
    }
    pressanykey(NULL);
  }
  return 0;
}
