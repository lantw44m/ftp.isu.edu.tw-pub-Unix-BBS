#include "bbs.h"

extern void item_update_passwd(char);

extern char station_list_cn[][15];
extern int station_num;

#ifndef NO_USE_MULTI_STATION

extern char station_list[][13];
extern char station_sysops[MAX_STATIONS][31];
extern char station_limit[];
extern uschar enter_number;
extern uschar enter_list[MAX_STATIONS];

int set_ds(void) {
   item_update_passwd('l');
   change_station_set(1);
   item_update_passwd('u');

   return FULLUPDATE;
}

int a_change(void) {
   change_station_set(0);

   return FULLUPDATE;
}

int change_station_set(int mode) {
   char genbuf[100];
   char flag = 1;
   int i, p, ch;

   clear();
   move(1, 4);
   prints("%-13s%-5.4s%-15s%-30s%-8.8s",
          "­^¤å¯¸¦W", " °ð ", "¤¤¤å¯¸¦W", "¯¸°È¤H­û", "¶i¤J±ø¥ó");

   move(3, 0);
   for(i = 0 ; i < enter_number ; i++) {
      move(3 + i, 4);
      prints("%-13s%-5.4d%-15s%-30.30s%-8.8s\n",
      station_list[enter_list[i]],
#ifndef USE_FREE_PORT
      enter_list[i] + (DEFAULT_BBSD == 'Z' ? ZBBSDP : MBBSDP),
#else
      station_2_port(DEFAULT_BBSD, enter_list[i]),
#endif
      station_list_cn[enter_list[i]],
      station_sysops[enter_list[i]],
      station_limit[enter_list[i]] ? "­­¨î¦W³æ" : "µL");
   }

   move(b_lines, 1);
   prints("[1;37;45m (p)(¡ô)¤W¤@¯¸  (n)(¡õ)¤U¤@¯¸  [1;37;46m%s[1;37;45m  "
          "(¡÷)¶i¤J¯¸¥x  (q)(¡ö) Â÷¶}         [m",
          mode ? "³]©w¹w³]¯¸" : " ¿ï¾Ü¯¸¥x ");

   cursor_show(3 , 1);
   p = 3;

   cuser.extra_mode[1] = 0;
   while(1) {
      ch = cursor_key(p , 1);

      if(ch == KEY_RIGHT) {
         if(mode == 0) {
            char sbuf[50];
            sprintf(sbuf, BBSHOME "/run/%s.sysop_name", station_list[p-3]);
#ifdef SEE_ALL_STATION       /* ¦]¬°¬Ýªº¨ì©Ò¦³ªº¯¸¥x ¬G»Ý¦b³o§PÂ_¬O§_¥i¶i¤J */
            char sbuf[50];
            sprintf(sbuf, BBSHOME "/run/%s.sysop_name", station_list[p-3]);

            if(!station_limit[p-3])
               if(cuser.station_member[p-3] && !(HAS_PERM(PERM_SYSOP) ||
                  belong(genbuf, cuser.userid) && HAS_PERM(PERM_BOARD)))
               continue;

            if(!station_limit[p-3] || HAS_PERM(PERM_SYSOP) ||
               cuser.station_member[p-3] || belong(genbuf, cuser.userid)) {
#endif
               strcpy(cuser.station, station_list[enter_list[p-3]]);
               cuser.now_stno = enter_list[p-3];
               cuser.extra_mode[1] = 1;
               currutmp->now_stno = cuser.now_stno;
               strcpy(currboard, DEFAULTBOARD);

               access_mask('l', &cuser.userlevel, cuser.userid);
               if(HAS_PERM(PERM_SYSOP) || belong(sbuf, cuser.userid))
                  cuser.userlevel |= PERM_CHATROOM;
               else cuser.userlevel &= ~PERM_CHATROOM;
#ifdef SEE_ALL_STATION
            }
            else continue;
#endif
         }
         break;
      }

      if(ch == KEY_LEFT || ch == 'q' || ch == 'Q') {
         flag = 0;
         break;
      }

      if(ch == KEY_DOWN || ch == 'n') p++;
      if(ch == KEY_UP || ch == 'p') p--;

      if(p - 3 < 0) p = enter_number + 2;
      if(p - 3 >= enter_number) p = 3;
   }

   if(mode == 0 && cuser.extra_mode[1]) {
      int fg = 0;
#ifdef MultiIssue
      fg = check_multi_index('I', cuser.now_stno, genbuf);
#else
      sprintf(genbuf, "etc/issue.%s", cuser.station);
#endif
      if(!fg) more(genbuf, YEA);
      sprintf(genbuf, "etc/welcome.%s", cuser.station);
      more(genbuf, YEA);
      load_boards(DEFAULTBOARD);
   }

   sprintf(genbuf, "Announce.%s", cuser.station);
   sprintf(&genbuf[50], "boards/%s", genbuf);
   if(dashd(&genbuf[50])) force_board(genbuf);

   if(flag) return FULLUPDATE;
   else return DONOTHING;
}
#endif

int c_st_name(void) {
   FILE *fp;
   int i;
   char buf[20];

   if(!is_station_sysop())
      return DONOTHING;

   getdata(b_lines - 1, 0,
   "½Ð¿é¤J·sªº¤¤¤å¯¸¦W : ", buf, 15, DOECHO, 0);
   if(!buf[0]) return FULLUPDATE;

   strcpy(station_list_cn[cuser.now_stno], buf);
   fp = fopen(BBSHOME "/stfiles/station_list_cn", "w+");
   for(i = 0 ; i < station_num ; i++)
      fprintf(fp, "%s\n", station_list_cn[i]);

   fclose(fp);
   return FULLUPDATE;
}
