#include "bbs.h"
#include <varargs.h>

struct help_struct {
  char key[8];
  char title[64];
  char path[128];
  char perm;
  char backup[55];
};
typedef struct help_struct hpst;

hpst acshpst[128];

int open_hpst(char *path) {
   int i;
   FILE *fp;

   if((fp = fopen(path, "rb")) == NULL) return -1;
   i = fread(&acshpst, sizeof(acshpst), 1, fp);
   fclose(fp);

   return i;
}

int access_hpst(char *fpath) {
   char buf[96];
   int record = 0, i, j, k, p, pi, ch, local_row;

   if(open_hpst(fpath) == 1) {
      for(i = 0 ; i < 128 ; i++)
         if(!strstr(acshpst[i].title, "¥¼³]©w")) record++;
         else break;
   }
   else {
      pressanykey("¨D§U¸ê®ÆÀÉ®æ¦¡¿ù»~");
      return -1;
   }

   if(record < 20) local_row = record;
   else            local_row = 20;

   pi = p = i = ch = 0;

   while(ch != 'Q' && ch != 'q' && ch != KEY_LEFT) {
      if(pi != i || ch == 0) {
         clear();

         for(j = 0, k = 0 ; j < record ; j++)
            if(j - i * local_row < local_row && j - i * local_row >= 0) {
               move(k+2, 5);

               prints("%3d  %-8.8s %s[m %-55.55s", j+1,
                      acshpst[j].key, acshpst[j].perm == 0 ? "[1;33;40m¤@¯ë"
                      : acshpst[j].perm == 1 ? "[1;36;40mªO¥D" : "[1;31;40m"
                      "¯¸ªø", acshpst[j].title);

               k++;
            }

         move(1, 1);
         prints("[0;30;47m  [0;37;41m ½s¸¹ [0;37;42m  ¼ö  Áä  [0;37;46m"
                "Åv­­[1;37;44m  %-21.21s [0;37;44m «ö Áä ¥\\ ¯à »¡ ©ú "
                "[0;37;43m  ¦@ [1;37;43m%3d[0;37;43m µ§  [m",
                acshpst[0].backup, record);
         out2line(1, b_lines-1, " [0;37;45m   (¡ô)(¡õ)/(b)(space)¤W¤U¶µ¥Ø/­¶"
                                "¦¸  (/)³Ì«e¶µ  ($)³Ì«á¶µ  (¡÷)¸Ô²Ó  (q)Â÷¶} "
                                " [m");

         pi = i;
      }

      ch = cursor_key(p % local_row + 2, 0);

      if(ch == KEY_RIGHT) {
         more(acshpst[p].path, "YEA");
         ch = 0;
      }

      if(ch >= '0' && ch <= '9') {
        out2line(1, 0, "");
        sprintf(buf, "%c", (char)ch);
        getdata(0, 2, "¸õ¦Ü²Ä´X¶µ ? ", buf, 4, DOECHO, buf);
        p = atoi(buf) - 1;
        out2line(1, 0, "");
      }

      if(ch == KEY_DOWN || ch == 'n') p++;
      if(ch == KEY_UP || ch == 'p')   p--;
      if(ch == KEY_PGUP || ch == 'b') p -= local_row;
      if(ch == KEY_PGDN || ch == ' ') p += local_row;

Go1:
      if(ch == '$' || ch == 255) {
         p = 9999;
         if(ch == 255) ch = 0;
      }
      if(ch == '/') p = -9999;

      if(p >= record) p = record ? record-1 : 0;
      if(p < 0) p = 0;

      while(p / local_row > i || p / local_row < i) {
         if(p / local_row > i) i++;
         if(p / local_row < i) i--;
      }
   }
}

/* Dopin: TRANS Mode Number to Mode Definition */
char *mode_trans[50] = {
   "IDLE",
   "MMENU",
   "ADMIN",
   "MAIL",
   "TMENU",
   "UMENU",
   "XMENU",
   "CLASS",
   "LOGIN",
   "ANNOUNCE",
   "POSTING",
   "READBRD",
   "READING",
   "READNEW",
   "SELECT",
   "RMAIL",
   "SMAIL",
   "CHATING",
   "XMODE",
   "FRIEND",
   "LAUSERS",
   "LUSERS",
   "MONITOR",
   "PAGE",
   "QUERY",
   "TALK",
   "EDITPLAN",
   "EDITSIG",
   "VOTING",
   "XINFO",
   "MSYSOP",
   "WWW",
   "BRIDGE",
   "REPLY",
   "HIT",
   "DBACK",
   "NOTE",
   "EDITING",
   "MAILALL",
   "ATSFUNC",
   "CHICKEN_3",
   "GLINK",
   "TREE",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL"
};

int help(va_list pvar) {
   int mode;
   char buf[64];

   mode = va_arg(pvar, int);

   if(mode < 0 || mode > 49) return;
   sprintf(buf, BBSHELP "/%s.help", mode_trans[mode]);
   access_hpst(buf);
}
