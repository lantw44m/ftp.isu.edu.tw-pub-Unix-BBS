/*-------------------------------------------------------*/
/* menu.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : menu/help/movie routines                     */
/* create : 95/03/29                                     */
/* update : 02/09/08 (Dopin)                             */
/*-------------------------------------------------------*/

#include "choice_class.h"
#include "menu.h"

/* ------------------------------------- */
/* help & menu processring               */
/* ------------------------------------- */

#ifdef USE_MULTI_TITLE
char title_color;
static char title_buffer[256];
#endif

int refscreen = NA;

extern char *boardprefix;
extern force_board(char *);
extern char station_list_cn[][15];
extern usint power(int, int);

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

#ifdef  EMAIL_JUSTIFY
/* Dopin: ­«·s±Hµo»{ÃÒ«H */
void mail_justify(userec);

int go_justify(void) {
  char buf[50];

  if(HAS_PERM(PERM_LOGINOK))
    pressanykey("±z¤w³q¹L»{ÃÒ ¤£»Ý­n¦A¨Ï¥Î«H¥ó»{ÃÒ ¦p­n§ó§ï½Ð³]©w­Ó¤H¸ê®Æ");
  else {
    getdata(b_lines, 0, "½T©w¦A¦¸±H¥X»{ÃÒ«H y/N ? ", buf, 2, LCECHO, 0);
    if(*buf == 'y') {
      getdata(b_lines, 0, "¬O§_§ó§ï«H½c¸ê®Æ y/N ? ", buf, 2, LCECHO, 0);
      if(*buf == 'y') {
        getdata(b_lines, 0, "½Ð¿é¤J·sªº«H½c¦ì¸m : ", buf, 50, DOECHO,
                cuser.email);

        if(strcmp(buf, cuser.email)) {
          item_update_passwd('l');
          strcpy(xuser.email, buf);
          strcpy(cuser.email, buf);
          item_update_passwd('u');
        }
      }
      mail_justify(cuser);  /* ±H»{ÃÒ«Hµ{¦¡ */
    }
  }

  return FULLUPDATE;
}
#endif

int get_dir_num(char *fname) {
   FILE* fp;
   fileheader fhdr;
   int n = 0;

   if (fp = fopen(fname, "r")) {
      char type;

      while (fread(&fhdr, sizeof(fhdr), 1, fp) == 1)
         n++;

      fclose(fp);
   }
   return n;
}

void showtitle(char *title, char *mid1) {
  char buf[40], *mid, midbuf[TTLEN];
  int spc, pad;

  mid = strcpy(midbuf, mid1);
  spc = strlen(mid);

  if(title[0] == 0) title++;
  else if (chkmail(0)) {
    mid = "[41;5m ¸ô¦æ³¾±a¨Ó¤F·sªº°T®§ [0;1;44m";
    /* CyberMax: spc ¬O¤Ç°t mid ªº¤j¤p. */
    spc = 22;
  }

  if(strstr(mid, "¶i¦æ§ë²¼¤¤")) {
     sprintf(buf, "[42;5m%s[0;1;44m", mid);
     strcpy(mid, buf);
  }

#ifdef RECORD_NEW_MSG
  spc = 61 - strlen(title) - spc - strlen(currboard);
#else
  spc = 67 - strlen(title) - spc - strlen(currboard);
#endif

  /* woju */
  if(spc < 0) {
    mid[strlen(mid) + spc] = 0;
    spc = 0;
  }
  pad = 1 - spc & 1;
  memset(buf, ' ', spc >>= 1);
  buf[spc] = '\0';

  clear();

#ifdef USE_MULTI_TITLE
  title_color = rand()%6 + 41;

  sprintf(title_buffer,
#ifdef RECORD_NEW_MSG
    "[%2dm¡i%s¡j%s[%2dm%s[%2dm%s[%2dm%s %4s [1;3%s¡m%s¡n[0m\n",
    title_color, title, buf, title_color, mid, title_color, buf, title_color,
    " " + pad,
    currutmp->msgs.newmsg ? "°T®§" : currmode & MODE_SELECT ? "¨t¦C" :
    currmode & MODE_ETC ? "¨ä¥L" : "",
    currmode & MODE_DIGEST ? "2m¤åºK" : "7m¬ÝªO", currboard
#else
    "[%2dm¡i%s¡j%s[%2dm%s[%2dm%s[%2dm%s [1;3%s¡m%s¡n[m\n",
    title_color, title, buf, title_color, mid, title_color, buf, title_color,
    " " + pad, currmode & MODE_SELECT ? "6m¨t¦C" : currmode & MODE_ETC ?
    "5m¨ä¥L" : currmode & MODE_DIGEST ? "2m¤åºK" : "7m¬ÝªO", currboard
#endif
  );
  outs(title_buffer);

#else

  prints(
#ifdef RECORD_NEW_MSG
    "[1;44;37m¡i%s¡j%s[33m%s%s%s %4s [1;3%s¡m%s¡n[0m\n",
    title, buf, mid, buf, " " + pad,
    currutmp->msgs.newmsg ? "°T®§" : currmode & MODE_SELECT ? "¨t¦C" :
    currmode & MODE_ETC ? "¨ä¥L" : "",
    currmode & MODE_DIGEST ? "2m¤åºK" : "7m¬ÝªO", currboard
#else
    "[1;44;37m¡i%s¡j%s[33m%s%s%s [1;3%s¡m%s¡n[0m\n",
    title, buf, mid, buf, " " + pad,
    currmode & MODE_SELECT ? "6m¨t¦C" : currmode & MODE_ETC ? "5m¨ä¥L" :
    currmode & MODE_DIGEST ? "2m¤åºK" : "7m¬ÝªO", currboard);
#endif
  );

#endif
}

/* ------------------------------------ */
/* °Êµe³B²z                              */
/* ------------------------------------ */

#define FILMROW 11
unsigned char menu_row = 13;
unsigned char menu_column = 15;
char mystatus[160];

#define MAX_HISTORY     3      /* °ÊºA¬ÝªO«O«ù 3 µ§¾ú¥v°O¿ý */

/* ===== changer by mgtsai, Sep 15th, '96 ===== */

void movie(int i) {
  int max_film = 0;
  int max_history;
  int k;
  static short history[MAX_HISTORY];
  static char myweek[] = "¤Ñ¤@¤G¤T¥|¤­¤»";
  struct tm *ptime;
  time_t now;
  FILE *fp;

  if((currstat != CLASS) && (cuser.uflag & MOVIE_FLAG)) {
    fileheader item;
    char notes[999][FNLEN];
    char pbuf[256], buf[256];
    FILE *fp;
    int num, id, j;

    bzero(notes, sizeof notes);
    if(rand()%2 || i == 999) {
IS_NOTE:
       id = 0;

#ifndef NO_USE_MULTI_STATION
       sprintf(pbuf, "man/boards/Note.%s/NoteList/.DIR", cuser.station);
       sprintf(buf, "man/boards/Note.%s/NoteList/", cuser.station);
#else
       strcpy(pbuf, "man/boards/Note/NoteList/.DIR");
       strcpy(buf, "man/boards/Note/NoteList/");
#endif
    }
    else {
       id = 1;

#ifndef NO_USE_MULTI_STATION
       sprintf(pbuf, "man/boards/Note.%s/OrderSongs/.DIR", cuser.station);
       sprintf(buf, "man/boards/Note.%s/OrderSongs", cuser.station);
#else
       strcpy(pbuf, "man/boards/Note/OrderSongs/.DIR");
       strcpy(buf, "man/boards/Note/OrderSongs");
#endif
    }

    num = get_dir_num(pbuf);

    if(fp = fopen(pbuf, "r")) {
       for(max_film = 20, j = 0; j < num; j++) {
          if(fread(&item, sizeof(item), 1, fp) == 1) item.title[50] = 0;
          strcpy(notes[j], item.filename);
          if(feof(fp)) break;
       }

       fclose(fp);
       max_film = num - 1;
    }

    if(max_film < 5) {
       if(id) goto IS_NOTE;
       else   goto SHOW_USER_ST;
    }

    max_history = max_film - 2;
    if(max_history > MAX_HISTORY - 1) max_history = MAX_HISTORY - 1;

    k = i;
    if(i < 999) {
       do {
         if(!k) k = (rand() % max_film) + 1;

         for(now = max_history; now >= 0; now--)
           if(k == history[now]) {
             k = 0;
             break;
           }
       } while (k == 0);

       memcpy(history, &history[1], max_history * sizeof(short));
       history[max_history] = now = k;

       j = FILMROW + 1;
    }
    else {
       k = 0;
       j = b_lines;
    }

    sprintf(pbuf, "%s/%s", buf, notes[i == 999 ? 0 : k]);
    if(*notes[i] && (fp = fopen(pbuf, "r"))) {
      for(k = i < 999 ? 1 : 0 ; k < j ; k++) {
        out2line(1, k, "");

        if(now) {
          if(fgets(buf, ANSILINELEN, fp))
            prints(buf, cuser.userid, cuser.userid);
          else now = 0;
        }
      }
      fclose(fp);
      outs(reset_color);
    }
  }

SHOW_USER_ST:
  if(!cuser.extra_mode[3]) {
     char *msgs[] = { "¦n¤Í", "¥´¶}", "¨¾ÂZ", "¨¾¤ô" };

     time(&now);
     ptime = localtime(&now);
     i = ptime->tm_wday << 1;

#ifdef USE_MULTI_TITLE
     sprintf(mystatus,
          "[37;%2dm [%2d/%2d ¬P´Á%c%c %02d:%02d] [30;47m ¥Ø«e¨È«°¸Ì¦³"
          " [31m%5d[30m ¤H §Ú¬O [31m%-13s[30m  [Call¾÷] [31m%s [0m",
          title_color, ptime->tm_mon + 1, ptime->tm_mday, myweek[i],
          myweek[i + 1], ptime->tm_hour, ptime->tm_min, count_ulist(),
          cuser.userid, msgs[currutmp->pager]);
#else
     sprintf(mystatus,
          "[34;46m [%2d/%2d ¬P´Á%c%c %02d:%02d] [30;47m ¥Ø«e¨È«°¸Ì¦³"
          " [31m%5d[30m ¤H §Ú¬O [31m%-13s[30m  [Call¾÷] [31m%s [0m",
          ptime->tm_mon + 1, ptime->tm_mday, myweek[i], myweek[i + 1],
          ptime->tm_hour, ptime->tm_min, count_ulist(), cuser.userid,
          msgs[currutmp->pager]);
#endif
     outmsg(mystatus);
     refresh();
  }
}

/* ===== end ===== */

/* Dopin 09/20/02 */
#ifdef USE_LITTLE_NOTE
              /* ¦h¥[ mode ¦]¬° CLASS ­nÃB¥~§PÂ_ */
static int show_menu(struct commands *p, char mode) {
  register int n = 0;
  register char *s;  /* 160 = 256 - 96 QQ;;; °²³] menu ³Ì¤j¥Î¨ì 96 bytes*/
  char ci, buf[256], sn[11][160] = {0}; /* 256 «h¬O ¨È¯¸ ASCII MAX ­È */
  FILE *fp;            /* 11 ROW */

  movie(currstat);

  if(!check_multi_index('N', cuser.now_stno, buf)) {
    if(fp = fopen(buf, "r")) { /* Åª¤p¬ÝªO¸ê®Æ */
       ci = 0;
       for(ci = 0 ; ci < 11 ; ci ++) if(!fgets(sn[ci], 160, fp)) break;
       fclose(fp);
    }
  }

  ci = 0;
  move(menu_row, 0);
          /* ¦³¿ï³æ¸ê®Æ ©Î ¦³¤p¬ÝªO */
  while((s = p[n].desc) || sn[ci][0]) {
     if(s) { /* ¦pªG¦³¿ï³æ¸ê®Æ */
        if(HAS_PERM(p[n].level)) { /* ¬Ýªº¨ì¿ï³æªº¸Ü */
           strcpy(buf, s+2); /* MMenu ... ±q e ¶}©l¨ú­È */
           if(currstat == CLASS || !sn[ci][0]) /* ¤ÀÃþ¬ÝªO ©Î ¨S¤p¬ÝªO ROW */
              prints("%*s  ([1;36m%c[m)%s\n", menu_column, "", s[1], buf);
           else /* §_«h¨q¸ê®Æ */
              prints("%*s  ([1;36m%c[m)%-28s%s", /* ¤p¬ÝªO¨q§¹´«¤U¦æ */
                     menu_column, "", s[1], buf, sn[ci++]);
        }
        n++;
     }
     else { /* ­Y¨S¤p¬ÝªO¸ê®Æ */
        if(currstat == CLASS) break; /* ¦pªG¬O¤ÀÃþ¬ÝªO ª½±µ¸õ¶} ¤£»ÝÅã¥Ü */
        if(currstat == CLASS) break; /* ¦pªG¬O¤ÀÃþ¬ÝªO ª½±µ¸õ¶} ¤£»ÝÅã¥Ü */
        else prints("%36s%-s", "", sn[ci++]);    /* ¨q¥XªÅ¹j«á¦A¨q¤p¬ÝªO */
     }
  }

  return n - 1;
}

#else

static int show_menu(struct commands *p, int mode) {
  register int n = 0;
  register char *s;

  movie(currstat);
  move(menu_row, 0);

  while (s = p[n].desc)
  {
     if(HAS_PERM(p[n].level))
        prints("%*s  ([1;36m%c[0m)%s\n", menu_column, "", s[1], s + 2);

SKIP_TO_HERE:
     n++;
  }
  return n - 1;
}
#endif

int domenu(cmdmode, cmdtitle, cmd, cmdtable, mode)
  char *cmdtitle;
  int cmdmode, cmd;
  struct commands cmdtable[];
  int mode;
{
#ifdef USE_LITTLE_NOTE
  const char m_r = 13, m_c = 2;
#else
  const char m_r = 13, m_c = 15;
#endif
  int lastcmdptr;
  int n, pos, total, i;
  int err;
  int chkmailbox();
  static char cmd0[LOGIN];

  if(is_station_sysop()) cuser.userlevel |= PERM_CHATROOM;
  else cuser.userlevel &= ~PERM_CHATROOM;

  if (cmd0[cmdmode])
     cmd = cmd0[cmdmode];

  setutmpmode(cmdmode);
  if (cmdmode == CLASS)
  {
    menu_row = 3;
    menu_column = 15;
  }

  if(cmdmode == MMENU)
  {
     menu_row = m_r;
     menu_column = m_c;
  }

  showtitle(cmdtitle, station_list_cn[cuser.now_stno]);
  total = show_menu(cmdtable, currmode);
  outmsg(mystatus);

  lastcmdptr = pos = 0;

  do
  {
    i = -1;

    switch (cmd) {

    /* Dopin */
    case Ctrl('R'):
       DL_func("bin/message.so:get_msg", 0);
       refscreen = YEA;
       i = lastcmdptr;
       break;

#ifdef CTRL_G_REVIEW
    case Ctrl('G'):
       DL_func("bin/message.so:get_msg", 1);
       i = lastcmdptr;
       refscreen = YEA;
       break;
#endif

#ifdef CHOICE_RMSG_USER
    case Ctrl('V'):
       choice_water_id();
       i = lastcmdptr;
       refscreen = YEA;
       break;
#endif

    case Ctrl('U'):
       t_users();
       refscreen = YEA;
       i = lastcmdptr;
       break;
    case Ctrl('B'):
       m_read();
       refscreen = YEA;
       i = lastcmdptr;
       break;
    case Ctrl('C'):
       if (*paste_path) {
          a_menu(paste_title, paste_path, paste_level);
          refscreen = YEA;
       }
       i = lastcmdptr;
       break;
    case KEY_ESC:
       if (KEY_ESC_arg == 'c')
          capture_screen();
       else if (KEY_ESC_arg == 'n') {
          edit_note();
          refscreen = YEA;
       }
       i = lastcmdptr;
       break;
#ifdef TAKE_IDLE
    case Ctrl('I'):
       t_idle();
       refscreen = YEA;
       i = lastcmdptr;
       break;
#endif
    case Ctrl('N'):
       New();
       refscreen = YEA;
       i = lastcmdptr;
       break;
    case Ctrl('A'):
       if (mail_man() == FULLUPDATE)
          refscreen = YEA;
       i = lastcmdptr;
       break;
    case KEY_DOWN:
      i = lastcmdptr;

    case KEY_HOME:
    case KEY_PGUP:
      do
      {
        if (++i > total)
          i = 0;
      } while (!HAS_PERM(cmdtable[i].level));
      break;

    case KEY_END:
    case KEY_PGDN:
      i = total;
      break;

    case KEY_UP:
      i = lastcmdptr;
      do
      {
        if (--i < 0)
          i = total;
      } while (!HAS_PERM(cmdtable[i].level));
      break;

    case KEY_LEFT:
    case 'e':
    case 'E':
      if (cmdmode == MMENU)
      {
        cmd = 'G';
      }
      else if ((cmdmode == MAIL) && chkmailbox())
      {
        cmd = 'R';
      }
      else
      {
        if (cmdmode == CLASS)
        {
          menu_row = m_r;
          menu_column = m_c;
        }
        return 0;
      }

    default:
      if((cmd == 's'  || cmd == 'r') && (currstat == MMENU ||
        currstat == TMENU || currstat == XMENU))  {
        if(cmd == 's') ReadSelect();
        else Read();

        refscreen = YEA;
        i = lastcmdptr;
        break;
      }

      if(cmd == '\n' || cmd == '\r' || cmd == KEY_RIGHT) {
        if(cmdmode == CLASS) boardprefix = cmdtable[lastcmdptr].desc + 2;
        out2line(1, b_lines, "");

        /* dso-patch */
        if(cmdtable[lastcmdptr].mode && DL_get(cmdtable[lastcmdptr].cmdfunc)) {
          void *p = (void *)DL_get(cmdtable[lastcmdptr].cmdfunc);
          if(p) cmdtable[lastcmdptr].cmdfunc = p;
          else break;
        }
        /* dso end */

        currstat = XMODE;
        if((err = (*cmdtable[lastcmdptr].cmdfunc) ()) == QUIT) return;
        currutmp->mode = currstat = cmdmode;

        if(err == XEASY) {
          refresh();
          sleep(1);
        }
        else if (err != XEASY + 1 || err == FULLUPDATE) refscreen = YEA;

        if(err != -1) cmd = cmdtable[lastcmdptr].desc[0];
        else cmd = cmdtable[lastcmdptr].desc[1];

        cmd0[cmdmode] = cmdtable[lastcmdptr].desc[0];
      }

      if(cmd >= 'a' && cmd <= 'z') cmd &= ~0x20;
      while(++i <= total) if(cmdtable[i].desc[1] == cmd) break;
    }

    if(i > total || !HAS_PERM(cmdtable[i].level)) continue;

    if(refscreen) {
      showtitle(cmdtitle, station_list_cn[cuser.now_stno]);
      show_menu(cmdtable, currmode);
      outmsg(mystatus);
      refscreen = NA;
    }

    cursor_clear(menu_row + pos, menu_column);

    n = pos = -1;
    while (++n <= (lastcmdptr = i)) if(HAS_PERM(cmdtable[n].level)) pos++;
    cursor_show(menu_row + pos, menu_column);

  } while (((cmd = igetkey()) != EOF) || refscreen);

  abort_bbs();
}
/* INDENT OFF */
