/*-------------------------------------------------------*/
/* util/updateboard.c        (SOB 0.22 ATS Version 1.31) */
/*-------------------------------------------------------*/
/* target : 新增/刪除站台後 看板管理程式                 */
/* create : 02/09/10 by Dopin                            */
/* update : 02/12/07                                     */
/*-------------------------------------------------------*/
/* syntax : updateboard <Mode> <SysopName> <StationName> */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"

int m_newbrd_create(char *station, char *boardname, char *brdcname, char *bmn,
                    char *sysop, usint level) {
  boardheader newboard;
  char ans[4];
  int bid, i;
  char genbuf[200];

  if(!boardname[0] && !boardname[1]) return -1;
  memset(&newboard, 0, sizeof(newboard));

  strcpy(newboard.brdname, boardname);
  strcpy(newboard.title, brdcname);

  setbpath(genbuf, newboard.brdname);
  strcpy(newboard.BM, bmn);
  strcpy(newboard.station, station);

  strcpy(newboard.sysop, sysop);
  newboard.yankflags[10] = 0;
  newboard.yankflags[0] = 1;
  newboard.yankflags[1] = 1;
  newboard.pastbrdname[0] = 0;
  newboard.level = level;

  if(!check_exist_board(newboard.brdname))
     return update_board(&newboard, NULL, 'A');
  else return -1;

  return 0;
}

void new_del_brd_auto(char mode, char *sysop, char *station) {
   char cj, flag = 0, buf[200], cbuf[70], bbuf[100], mbuf[100], genbuf[200];
   char *cr = "開板", *dl = "刪板", *ok = "成功\", *no = "失敗";
   char *kn[5] = { "Note", "deleted", "junk", "SYSOP", "Announce" };
   char *cmk[2] = { "系統", "站務" };
   char *cmkn[5] = { "動態看板", "資源回收桶", "餿水桶", "站務討論區",
                     "公告事項" };
   usint lv[5] = { PERM_LOGINOK | PERM_POSTMASK, PERM_CHATROOM | PERM_POSTMASK,
                   PERM_SECRET, PERM_CHATROOM | PERM_POSTMASK, 0 };

   if(!(mode == 'C' || mode == 'D')) return;
   if(!sysop || !station) return;

   for(cj = 0 ; cj < 5 ; cj++) {
      sprintf(buf, "%s.%s", kn[cj], station);

      sprintf(bbuf, BBSHOME "/boards/%s", buf);
      sprintf(mbuf, BBSHOME "/man/boards/%s", buf);
      if(mode == 'C') {
         sprintf(cbuf, "%s☆ %s", cj < 3 ? cmk[0] : cmk[1], cmkn[cj]);
         printf("%s Create %s\n",
            buf, m_newbrd_create(station, buf, cbuf, sysop, sysop, lv[cj]) ?
            "Error" : "OK");

            sprintf(genbuf, "mkdir %s", bbuf);
            system(genbuf);
            sprintf(genbuf, "mkdir %s", mbuf);
            system(genbuf);
      }
      else {
         del_b_anyway(buf);
         rmdir(bbuf);
         rmdir(mbuf);
      }
   }
}

void show_syntex(void) {
   puts("Syntex : updateboard <Mode> <SYSOP Name> <Station Name>");
   puts("Mode   : <Create/Delete>");
   puts("Atlantis BBS 1.31-Stable by Dopin              09/12/02");
}

int main(int argc, char *argv[]) {
   char mode, buf[200];

   if(argc != 4) {
      show_syntex();
      return 1;
   }

   if(strcmp(argv[1], "Create") && strcmp(argv[1], "Delete")) {
      show_syntex();
      return 1;
   }

   if(strlen(argv[2]) < 2 || strlen(argv[2]) > IDLEN) {
      show_syntex();
      return 1;
   }

   if(strlen(argv[3]) < 2 || strlen(argv[2]) > 12) {
      show_syntex();
      return 1;
   }

   printf("Sizeof BoardHeader = %d bytes\n\n", sizeof(boardheader));

   strcpy(buf, BBSHOME "/src/util/ognizeboard");
   system(buf);
   strcpy(buf, "mv " BBSHOME "/BOARDS.OGNIZED " BBSHOME
               "/backup/BOARDS.ACCESS");
   system(buf);

   if(!strcmp(argv[1], "Create")) mode = 'C';
   else mode = 'D';

   new_del_brd_auto(mode, argv[2], argv[3]);
   return 0;
}
