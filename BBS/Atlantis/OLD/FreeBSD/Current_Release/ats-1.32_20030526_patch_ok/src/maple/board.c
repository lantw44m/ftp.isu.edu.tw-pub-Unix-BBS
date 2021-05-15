/*-------------------------------------------------------*/
/* board.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : ¬ÝªO¡B¸s²Õ¥\¯à                               */
/* create : 95/03/29                                     */
/* update : 02/09/08 (Dopin)                             */
/*-------------------------------------------------------*/

#include "stations.h"

#define BRC_MAXSIZE     24576
#define BRC_MAXNUM      80
#define BRC_STRLEN      26
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))

int brc_size, brc_changed = 0;
int brc_list[BRC_MAXNUM], brc_num;
char brc_buf[BRC_MAXSIZE];
char brc_name[BRC_STRLEN];

static time_t brc_expire_time;

#ifdef USE_MULTI_TITLE
extern char title_color;
#endif
extern boardheader *bcache;
extern int numboards;
extern int set_bs(void);
extern int b_favor_set(char, char, int);
extern int add_my(char, char *);
extern int check_trs_dir(MENU *, char *fname, char);

extern struct commands cmdlist[];

int now_num = 9999;

int board(), tw_board(), local_board(), Boards(), Post(), Read(), Select();
int New();

char *globalboard[] = {
  "Announce",
  "Test",
};
const char global_board_cn = 2;

char *dir_ptr = "[DIR]";

boardstat *nbrd;

int *zapbuf;
int brdnum, yank_flag = 0;
int ith_board;

char *boardprefix;

static char *str_local_board = "¡¸";  /* ¥Nªí local board class */
static char *str_tw_board = "ùþ";  /* ¥Nªí non-tw board class */

static void setup_man(boardheader *board) {
   char genbuf[200];
   FILE *fp;

   setapath(genbuf, board->brdname);
   mkdir(genbuf, 0755);
}

void init_station_info(void) {
   FILE *fp;
   FILE *fps;
   FILE *fpm;
   char buf[100];
   int i,j,l;
   char ch;

   fp = fopen(BBSHOME"/stfiles/station_list", "r");
   fps = fopen(BBSHOME"/stfiles/station_list_cn", "r");

   for(i = 0 ; i < station_num ; i++) {
      for(j = 0 ; j < 20 ; j++) {
         ch = fgetc(fp);
         if(ch == 10 || ch == 0) {
            buf[j] = 0;
            break;
         }
         else
            buf[j] = ch;
      }
      strcpy(station_list[i], buf);

      sprintf(buf, BBSHOME"/run/%s.sysop_name", station_list[i]);
      fpm = fopen(buf, "r");
      if(fpm == NULL)
         goto Over_SYSOP;

      for(l = 0 , j = 0 ; j < 100 ; j++) {
         ch = fgetc(fpm);
         if(ch == 10 || ch == 0 || feof(fpm)) {
            if(!feof(fpm) && ch == 10) {
               station_sysops[i][l] = '/';
               l++;
            }
            else {
               if(station_sysops[i][l-1] == '/')
                  station_sysops[i][l-1] = 0;
               else
                  station_sysops[i][l] = 0;
               break;;
            }
         }
         else {
            station_sysops[i][l] = ch;
            l++;
            if(l >= 29) {
               station_sysops[i][l] = 0;
               break;
            }
         }
      }
Over_SYSOP:
      for(j = 0 ; j < 20 ; j++) {
         ch = fgetc(fps);
         if(ch == 10 || ch == 0) {
            buf[j] = 0;
            break;
         }
         else
            buf[j] = ch;
      }
      strcpy(station_list_cn[i], buf);
   }
   fclose(fp);
   fclose(fps);
   fclose(fpm);
}

/* ----------------------------------------------------- */
/* home/userid/.boardrc maintain                         */
/* ----------------------------------------------------- */

static char *fn_boardrc = ".boardrc";

int is_favor(char *cboard) {
   char fpath[80];
   FILE *fp;

   setuserfile(fpath, "favor_boards");
   if((fp = fopen(fpath, "r")) == NULL)
   return 0;
   fclose(fp);

   return belong(fpath, cboard);
}

/* Dopin: µ¹¤p¬ÝªO¥Îªº ` -> \033 */
void check_ascii(char *ptr) {
   int i;

   for(i=0 ; i<128 ; i++) {
      if(ptr[i] == 0) break;

      if(ptr[i] == '`') ptr[i] = 27;
   }
}

static char *brc_getrecord(char *ptr, char *name, int *pnum, int *list) {
  int num;
  char *tmp;

  strncpy(name, ptr, BRC_STRLEN);
  ptr += BRC_STRLEN;
  num = (*ptr++) & 0xff;
  tmp = ptr + num * sizeof(int);

  if(num > BRC_MAXNUM) num = BRC_MAXNUM;
  *pnum = num;
  memcpy(list, ptr, num * sizeof(int));
  return tmp;
}

static char *brc_putrecord(char *ptr, char *name, int num, int *list) {
  if(num > 0 && list[0] > brc_expire_time) {
    if(num > BRC_MAXNUM) num = BRC_MAXNUM;
    while(num > 1 && list[num - 1] < brc_expire_time) num--;

    strncpy(ptr, name, BRC_STRLEN);
    ptr += BRC_STRLEN;
    *ptr++ = num;
    memcpy(ptr, list, num * sizeof(int));
    ptr += num * sizeof(int);
  }

  return ptr;
}

void brc_update() {
  if(brc_changed && cuser.userlevel) {
    char dirfile[STRLEN], *ptr;
    char tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
    char tmp_name[BRC_STRLEN];
    int tmp_list[BRC_MAXNUM], tmp_num;
    int fd, tmp_size;

    ptr = brc_buf;
    if(brc_num > 0) ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);

    setuserfile(dirfile, fn_boardrc);
    if((fd = open(dirfile, O_RDONLY)) != -1) {
      tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
      close(fd);
    }
    else tmp_size = 0;

    tmp = tmp_buf;
    while(tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z')) {
      tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
      if(strncmp(tmp_name, currboard, BRC_STRLEN))
        ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
    }
    brc_size = (int)(ptr - brc_buf);

    if((fd = open(dirfile, O_WRONLY | O_CREAT, 0664)) != -1) {
      ftruncate(fd, 0);
      write(fd, brc_buf, brc_size);
      close(fd);
    }
    brc_changed = 0;
  }
}

int brc_initial(char *boardname) {
  char dirfile[STRLEN], *ptr;
  int fd;

  if(strcmp(currboard, boardname) == 0) return brc_num;

  brc_update();
  strcpy(currboard, boardname);
  if(brc_buf[0] == '\0') {
    setuserfile(dirfile, fn_boardrc);
    if((fd = open(dirfile, O_RDONLY)) != -1) {
      brc_size = read(fd, brc_buf, sizeof(brc_buf));
      close(fd);
    }
    else  brc_size = 0;
  }

  ptr = brc_buf;
  while(ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z')) {
    ptr = brc_getrecord(ptr, brc_name, &brc_num, brc_list);
    if(strncmp(brc_name, currboard, BRC_STRLEN) == 0) return brc_num;
  }

  strncpy(brc_name, boardname, BRC_STRLEN);
  brc_num = brc_list[0] = 1;
  return 0;
}

void brc_addlist(char *fname) {
  int ftime, n, i;

  if(!cuser.userlevel) return;

  ftime = atoi(&fname[2]);
  if(ftime <= brc_expire_time) return;

  if(brc_num <= 0) {
    brc_list[brc_num++] = ftime;
    brc_changed = 1;
    return;
  }

  if((brc_num == 1) && (ftime < brc_list[0])) return;

  for(n = 0; n < brc_num; n++) {
    if(ftime == brc_list[n]) return;
    else if(ftime > brc_list[n]) {
      if(brc_num < BRC_MAXNUM) brc_num++;

      for (i = brc_num - 1; --i >= n; brc_list[i + 1] = brc_list[i]);
      brc_list[n] = ftime;
      brc_changed = 1;
      return;
    }
  }
  if(brc_num < BRC_MAXNUM) {
    brc_list[brc_num++] = ftime;
    brc_changed = 1;
  }
}

int brc_unread(char *fname) {
  int ftime, n;

  ftime = atoi(&fname[2]);
  if(ftime <= brc_expire_time) return 0;

  if(brc_num <= 0) return 1;
  for(n = 0; n < brc_num; n++) {
    if(ftime > brc_list[n]) return 1;
    else if(ftime == brc_list[n]) return 0;
  }

  return 0;
}

/* ----------------------------------------------------- */
/* .bbsrc processing                                     */
/* ----------------------------------------------------- */

char *str_bbsrc = ".bbsrc";

/* Dopin: §ï±¼ static ¦]¬° Tree ·|¥Î¨ì */
void load_zapbuf() {
  register int n, size;
  char fname[60];

  /* MAXBOARDS ==> ¦Ü¦h¬Ý±o¨£ 4 ­Ó·sªO */
  n = numboards + 4;
  size = n * sizeof(int);
  zapbuf = (int *)malloc(size);
  while(n) zapbuf[--n] = login_start_time;
  setuserfile(fname, str_bbsrc);

  if((n = open(fname, O_RDONLY, 0600)) != -1) {
    read(n, zapbuf, size);
    close(n);
  }

  if(!nbrd) nbrd = (boardstat *) malloc(MAXBOARD * sizeof(boardstat));
  brc_expire_time = login_start_time - 365 * 86400;
}

static void save_zapbuf() {
  register int fd, size;
  char fname[60];

  setuserfile(fname, str_bbsrc);
  if((fd = open(fname, O_WRONLY | O_CREAT, 0600)) != -1) {
    size = numboards * sizeof(int);
    write(fd, zapbuf, size);
    close(fd);
  }
}

/* woju Ref: bbs.c: brdperm(char* brdname, char* userid) */
int Ben_Perm(boardheader *bptr) {
  register usint level;
  register char *ptr, *str;
  char ci, buf[64];

/* Dopin: ¶i¨t²ÎºûÅ@°Ï®É SYSOP ¦b¹w³]¯¸¥iµø©Ò¦³¬ÝªO ¤À¯¸ªø¦b¨ä¯¸¥iµø©ÒÄÝ¬ÝªO */
  if(is_board_manager(bptr->brdname) && cuser.extra_mode[8] == 1) {
#ifndef NO_USE_MULTI_STATION
    if(!cuser.now_stno)
#endif
    {
      if(HAS_PERM(PERM_SYSOP)) return 1;
      else return 0;
    }
#ifndef NO_USE_MULTI_STATION
    else if(strcmp(bptr->station, cuser.station)) return 0;
    else return 1;
#endif
  }

  if(HAS_PERM(PERM_SYSOP) /* Dopin: ¯¸ªø¤£¨ü©Úµ´¦W³æ­­¨î */
#ifdef USE_BOARDS_MANAGER /* Dopin: ¬ÝªOÁ`ºÞ¤£¨ü©Úµ´¦W³æ­­¨î */
     || HAS_PERM(PERM_EXTRA1)
#endif
  ) goto Reject_PASS_by_1;

  setbfile(buf, bptr->brdname, "rejectlist");
  if(belong(buf, cuser.userid)) return 0;

Reject_PASS_by_1:
  if(cuser.extra_mode[0] == 1) {
    if(!strcmp(bptr->brdname, DEFAULTBOARD)) return 1;

    if(!is_favor(bptr->brdname)) return 0;
#ifndef NO_USE_MULTI_STATION
    else goto R_PASS;
#endif
  }

#ifndef NO_USE_MULTI_STATION
  /* Dopin: ¸ó¯¸¥x¬ÝªO³B²z */
  sprintf(buf, "etc/%s", cuser.station);
  if(belong(buf, bptr->brdname)) goto R_PASS;

  /* Dopin: ¬°¼s°ì¯¸¥x ¥²©w¥iµø */
  if(is_global_board(bptr->brdname)) return 1;

  /* Dopin: ¤£¦b¦P¤@¯¸¥x ¬Ý¤£¨ì */
  if(strcmp(bptr->station, cuser.station)) return 0;
#endif

R_PASS:
  ptr = bptr->BM;
  level = bptr->level;

  if(is_board_manager(bptr->brdname)
#ifdef USE_BOARDS_MANAGER
    || HAS_PERM(PERM_EXTRA1)
#endif
  ) return 1;
  if(is_BM(ptr)) return 1;
  if(level & PERM_POSTMASK) return 1;
  if(HAS_PERM(level) && (level <= PERM_BM || level >= PERM_RMK1)) return 1;
  if(level & PERM_SPCMEMBER && HAS_PERM(PERM_SPCMEMBER)) return 1;
  if(level & PERM_ENTERSC) return 1;   /* ¶i¤J¯µ±K¬ÝªO ¾\Åª®É¤~§P©w */

  /* ¯¦±K¬ÝªO¡G®Ö¹ï¤¹³\¶i¤JªÌ¦W³æ */
  if(level & PERM_SECRET) {
    setbfile(buf, bptr->brdname, "permlist");
    return belong(buf, cuser.userid);
  }

  return 0;
}

void load_boards(char *bname) {
  boardheader *bptr;
  boardstat *ptr;
  char brdclass[5];
  int n;

  resolve_boards();
  if(!zapbuf) load_zapbuf();

  ith_board = brdnum = 0;
  for(n = 0; n < numboards ; n++) {
    bptr = &bcache[n];
    if(bptr->brdname[0] == '\0') continue;

    if(bname) {
       if(strcmp(bname, bptr->brdname)) continue;

       ptr = &nbrd[brdnum++];
       ptr->name = bptr->brdname;
       ptr->title = bptr->title;
       ptr->BM = bptr->BM;
       ptr->pos = n;
       ptr->total = -1;
       ptr->bvote = bptr->bvote;
       ptr->zap = (zapbuf[n] == 0);

       goto End_LOADB;
   }

#ifndef FB_FAMILY
    if(boardprefix) {
      if(boardprefix == str_local_board) {
        strncpy(brdclass, bptr->title + 4, 2);
        brdclass[2] = '\0';
      }
      else {
        strncpy(brdclass, bptr->title, 4);
        brdclass[4] = '\0';
      }
      if(!strncmp(boardprefix, "Collection", strlen("Collection"))) {
         if((bptr->level & PERM_POST) == 0) continue;
      }
      else if(boardprefix == str_tw_board) {
        if(strstr(bptr->title, str_tw_board)) continue;
      }
      else if(strstr(boardprefix, brdclass) == NULL) continue;
    }
#else
    /* Dopin For FB Title Set 08/10/02 */
    if(boardprefix) {
       strncpy(brdclass, &bptr->title[1], 4);
       brdclass[4] = '\0';

       if(strstr(boardprefix, brdclass) == NULL) continue;
    }
#endif

    if (Ben_Perm(bptr) && (yank_flag == 1 ||
        yank_flag == 2 && have_author(bptr->brdname) ||
        yank_flag != 2 && zapbuf[n]))
    {
      ptr = &nbrd[brdnum++];
      ptr->name = bptr->brdname;
      ptr->title = bptr->title;
      ptr->BM = bptr->BM;
      ptr->pos = n;
      ptr->total = -1;
      ptr->bvote = bptr->bvote;
      ptr->zap = (zapbuf[n] == 0);
    }
  }

  /* ¦pªG user ±N©Ò¦³ boards ³£ zap ±¼¤F */

  if(!brdnum && !boardprefix)
     if(yank_flag == 0) yank_flag = 1;
     else if(yank_flag == 2) yank_flag = 0;

End_LOADB:
}

static int search_board(int num) {
  static char genbuf[IDLEN + 2];
  char buf[200];

  if(num >= 0) {
     sprintf(buf, "%s[%s]: ", "½Ð¿é¤J¬ÝªO¦WºÙ", genbuf);
     getdata(b_lines - 1, 0, buf, buf, IDLEN + 1, DOECHO, 0);

     if(*buf) strcpy(genbuf, buf);
     out2line(0, b_lines - 1, "");
     move(b_lines, 0);
     if(genbuf[0]) {
       int n = num;

       do {
         if(++n >= brdnum) n = 0;
         if(strstr_lower(nbrd[n].name, genbuf) || strstr_lower(nbrd[n].title, genbuf))
           return n;
       } while(n != num);
     }
  }
  else {
     move(0,0);
     clrtoeol();
     CreateNameList();
     for(num = 0; num < brdnum; num++) AddNameList(nbrd[num].name);
     namecomplete(MSG_SELECT_BOARD, genbuf);
     for(num = 0; num < brdnum; num++)
       if(!strcmp(nbrd[num].name, genbuf)) return num;
  }
  return -1;
}

int check_newpost(boardstat *ptr) {
  fileheader fh;
  struct stat st;
  char fname[FNLEN];
  char genbuf[200];
  int fd, total;

  ptr->total = ptr->unread = 0;
  setbdir(genbuf, ptr->name);
  if((fd = open(genbuf, O_RDWR)) < 0) return 0;
  fstat(fd, &st);
  total = st.st_size / sizeof(fh);

  if(total <= 0) {
    close(fd);
    return 0;
  }

  ptr->total = total;
  if(!brc_initial(ptr->name)) ptr->unread = 1;
  else {
    lseek(fd, (total - 1) * sizeof(fh), SEEK_SET);
    if(read(fd, fname, FNLEN) > 0 && brc_unread(fname)) ptr->unread = 1;
  }

  close(fd);
  return 1;
}

static int unread_position(char *dirfile, boardstat *ptr) {
  fileheader fh;
  char fname[FNLEN];
  register int num, fd, step, total;

  total = ptr->total;
  num = total + 1;
  if(ptr->unread && (fd = open(dirfile, O_RDWR)) > 0) {
    if(!brc_initial(ptr->name)) num = 1;
    else {
      num = total - 1;
      step = 4;
      while(num > 0) {
        lseek(fd, num * sizeof(fh), SEEK_SET);
        if(read(fd, fname, FNLEN) <= 0 || !brc_unread(fname)) break;
        num -= step;
        if(step < 32) step += step >> 1;
      }
      if(num < 0) num = 0;
      while(num < total) {
        lseek(fd, num * sizeof(fh), SEEK_SET);
        if(read(fd, fname, FNLEN) <= 0 || brc_unread(fname)) break;
        num++;
      }
    }
    close(fd);
  }

  if(num < 0) num = 0;
  return num;
}

static void brdlist_foot() {
#ifdef USE_MULTI_TITLE
  prints("[37;%2dm  ¿ï¾Ü¬ÝªO  [31;47m  (c)[30m·s¤å³¹¼Ò¦¡  [31m(v/V)[30m"
  "¼Ð°O¤wÅª/¥¼Åª  [31m(y)[30m¿z¿ï%s  [31m(z)[30m¤Á´«¿ï¾Ü      [0m",
    title_color, yank_flag ? "³¡¥÷" : "¥þ³¡");
#else
  prints("[34;46m  ¿ï¾Ü¬ÝªO  [31;47m  (c)[30m·s¤å³¹¼Ò¦¡  [31m(v/V)[30m"
    "¼Ð°O¤wÅª/¥¼Åª  [31m(y)[30m¿z¿ï%s  [31m(z)[30m¤Á´«¿ï¾Ü      [0m",
    yank_flag ? "³¡¥÷" : "¥þ³¡");
#endif
}

/* woju */
have_author(char* brdname) {
   char dirname[200];
   int n, fd;

   ioctl(0, FIONREAD, &n);
   if(n) return 0;
   sprintf(dirname, "´M§ä¦³ [37;45m%.15s[m ¤å³¹ªº¬Ýª©: %-16.15s [%3d / %d]",
           currauthor, brdname, ++ith_board, numboards);
   outmsg(dirname);

   refresh();
   setbdir(dirname, brdname);

   strcpy(currowner, currauthor);
   if((fd = open(dirname, O_RDONLY, 0)) != -1) {
      struct stat st;
      fileheader* fhdr;
      int num, i;

      fstat(fd, &st);
      if(fhdr = malloc(st.st_size)) {
         num = read(fd, fhdr, st.st_size) / sizeof(fileheader);
         close(fd);
      }
      else {
         close(fd);
         return 0;
      }

      for(i = 0; i < num; i++)
        if(!strcasecmp(fhdr[i].owner, currauthor)) {
          free(fhdr);
          return ++i;
        }
      free(fhdr);
   }
   return 0;
}

static void show_brdlist(head, clsflag, newflag, num) {
  boardheader bh, newbh;
  int bid;
  int i, pgn;
  boardstat *ptr;

  if((int)cuser.welcomeflag & 128) pgn = 5;
  else pgn = 0;

  clear();
  showtitle("¬ÝªO¦Cªí", station_list_cn[cuser.now_stno]);
#ifdef TAKE_IDLE
  prints("[¡ö]¦^¤W¤@¼h [¡÷]¾\\Åª [¡ô¡õ]¿ï¾Ü [y]¸ü¤J [S]±Æ§Ç [/]·j´M [TAB]µo§b"
         "¼Ò¦¡ [h]¨D§U\n[7m%-20s Ãþ  §O %-32s§ë²¼ ªO    ¥D      [0m",
#else
  prints("[¡ö]¦^¤W¤@¼h [¡÷]¾\\Åª [¡ô¡õ]¿ï¾Ü [y]¸ü¤J [S]±Æ§Ç [/]·j´M [h]¨D§U  "
         "           \n[7m%-20s Ãþ  §O %-32s§ë²¼ ªO    ¥D      [0m",
#endif
         newflag ? "Á`¼Æ ¥¼Åª ¬Ý  ªO" : "  ½s¸¹  ¬Ý  ªO", "¤¤   ¤å   ±Ô   ­z");

  move(b_lines, 0);
  brdlist_foot();

  if(brdnum > 0) {
    int myrow = 2, bc;

    while(++myrow < 18 + pgn) {
      move(myrow, 0);
      if(head < brdnum) {
        ptr = &nbrd[head++];
        if(yank_flag == 2) prints("%5d %c", head, 'A');

        else if (!newflag) {
           if(ptr->total == -1) check_newpost(ptr);
           prints("%5d%c%s", head, ptr->zap ? '-' : ' ',
                  ptr->unread ? "[1;31;40m¡E[m" : "[1;33;40m¡C[m");
        }
        else if(ptr->zap) {
          ptr->total = ptr->unread = 0;
          outs("   ¡ß ¡ß");
        }
        else {
          if(ptr->total == -1)
            check_newpost(ptr);

          if(newflag) prints("%6d%s", ptr->total, ptr->unread ? "[1;31;40m¡E"
                             "[m" : "[1;33;40m¡C[m");
        }

        bid = getbnum(ptr->name);
        if(get_record(fn_board, &bh, sizeof(bh), bid) == -1)
        bh.level = 0;

        bc = 37;
        if(!(bh.level & PERM_POSTMASK)) {
          if(bh.level & PERM_SYSOP || bh.level & PERM_SECRET) bc = 36;
          else if(bh.level & PERM_ENTERSC) {
             if(bh.level & PERM_ENTERSCRQ) bc = 33;
             else bc = 31;
          }
        }

#ifdef ColorClass
        {
          int ic, sum_ic = 0;
          char class_buf[5];

#ifndef FB_FAMILY
  #define IC_PLUS 0
#else
  #define IC_PLUS 1
#endif

           for(ic = 0 ; ic < 4 ; ic++) {
              class_buf[ic] = ptr->title[ic + IC_PLUS];
              sum_ic += (uschar)class_buf[ic];
           }
           class_buf[ic] = 0;

#undef IC_PLUS
           sum_ic = sum_ic % 6 + 31;
           prints(
#ifndef FB_FAMILY
                  "[1;%2d;40m%-13.12s[%d;%2d;40m%-4s[m%-38s[1;31;40m%c[m"
                  " %.13s", bc, ptr->name, CHColor, sum_ic, class_buf,
                  &ptr->title[4], " VR"[ptr->bvote], ptr->BM
#else
                  "[1;%2d;40m%-13.12s[m[[%d;%2d;40m%-4s[m]%-38s[1;31;40m"
                  "%c[m %.13s", bc, ptr->name, CHColor, sum_ic, class_buf,
                  &ptr->title[6], " VR"[ptr->bvote], ptr->BM
#endif
           );
        }
#else
           prints("[1;%2d;40m%-13.12s[m%-42s[1;31;40m%c[m %.13s",
                  bc, ptr->name, ptr->title, " VR"[ptr->bvote], ptr->BM);
#endif
      }
      clrtoeol();
    }
    if((int)cuser.welcomeflag & 128) goto SET_NO_SB;

    ptr = &nbrd[num];
    brc_initial(ptr->name);
    bid = getbnum(ptr->name);
    if(get_record(fn_board, &bh, sizeof(bh), bid) == -1) goto NO_Show_List;

    out2line(0, 18, "[1;36;40m¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w"
                     "¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w[m");

    out2line(0, 19, bh.document[0] ? &bh.document[0] : "");
    out2line(0, 20, bh.document[80] ? &bh.document[80] : "");
    out2line(0, 21, bh.document[160] ? &bh.document[160] : "");
#ifndef SHOW_BRDMSG
    out2line(0, 22, bh.document[240] ? &bh.document[240] : "");
#endif

NO_Show_List:
    if(ptr->total == -1) check_newpost(ptr);

#ifdef SHOW_BRDMSG
    if(!(cuser.welcomeflag & 4096)) {
      char buf[80];
      sprintf(buf, "%-67s", bh.document[240] ? &bh.document[240] : "");

      if(newflag) sprintf(&buf[67], "N %3d / %3d", num+1, brdnum);
      else sprintf(&buf[67], "P %5d", ptr->total);
      out2line(0, 22, buf);
    }
#endif
  }
SET_NO_SB:
}

static int cmpboard(brd, tmp)
boardstat *brd, *tmp;
{
  register int type = 0;

  if(!(cuser.uflag & BRDSORT_FLAG)) type = strncmp(brd->title, tmp->title, 6);
  if(!type) type = strcasecmp(brd->name, tmp->name);

  return type;
}

static void choose_board(int newflag) {
  static int num = 0;
  boardstat *ptr;
  boardheader bh, newbh;
  int bid, i, pgn, head, ch, tmp, cntc;
  char genbuf[320];
  extern time_t board_visit_time;

  setutmpmode(newflag ? READNEW : READBRD);
  brdnum = 0;
  if((int)cuser.welcomeflag & 128) pgn = 20;
  else pgn = 15;

  if(!cuser.userlevel) yank_flag = 1; /* guest yank all boards */

  do {
    if(brdnum <= 0) {
      load_boards(NULL);
      if(brdnum <= 0) break;
      qsort(nbrd, brdnum, sizeof(boardstat), cmpboard);
      head = -1;
    }

    if(num < 0) num = 0;
    else if(num >= brdnum) num = brdnum - 1;

    if(head < 0) {
      if(newflag) {
        tmp = num;
        while(num < brdnum) {
          ptr = &nbrd[num];
          if(ptr->total == -1) check_newpost(ptr);
          if(ptr->unread) break;
          num++;
        }

        if(num >= brdnum) num = tmp;
      }
      head = (num / pgn) * pgn;
      show_brdlist(head, 1, newflag, num);
    }
    else if (num < head || num >= head + pgn) {
      head = (num / pgn) * pgn;
      show_brdlist(head, 0, newflag, num);
    }

    ch = cursor_key(3 + num - head, 0);

    switch (ch) {
    case 'e':
    case KEY_LEFT:
    case EOF:
      ch = 'q';
    case 'q':
      break;

    case 'c':
      if(yank_flag == 2) {
         newflag = yank_flag = 0;
         brdnum = -1;
      }
      show_brdlist(head, 1, newflag ^= 1, num);
      break;

    /* woju */
    case 'A':
    case 'a': {
      if(yank_flag != 2 ) {
        if(getdata(1, 0, "§@ªÌ ", genbuf, IDLEN + 2, DOECHO, currauthor))
           strncpy(currauthor, genbuf, IDLEN + 2);
        if(*currauthor) yank_flag = 2;
        else yank_flag= 0;
      }
      else yank_flag = 0;
      brdnum = -1;
      show_brdlist(head, 1, newflag, num);
      break;
    }
    case KEY_PGUP:

    case 'b':
    case 'P':
      if(num) {
        num -= pgn;
        if(num < 1) num = 0;
        break;
      }

    case KEY_END:
    case '$':
      num = brdnum - 1;
      break;

    case ' ':
    case KEY_PGDN:
    case 'N':
    case Ctrl('F'):
      if(num == brdnum - 1) num = 0;
      else {
        num += pgn;
        if(num >= brdnum)
           num = brdnum - 1;
      }
      break;

    /* Dopin */
    case Ctrl('R'):
      DL_func("bin/message.so:get_msg", 0);
      show_brdlist(head, 1, newflag, num);
      break;

#ifdef CTRL_G_REVIEW
    /* Dopin */
    case Ctrl('G'):
      DL_func("bin/message.so:get_msg", 1);
      show_brdlist(head, 1, newflag, num);
      break;
#endif

#ifdef CHOICE_RMSG_USER
    /* Dopin */
    case Ctrl('V'):
      choice_water_id();
      show_brdlist(head, 1, newflag, num);
      break;
#endif

    case KEY_ESC:
      if(KEY_ESC_arg == 'n') {
        edit_note();
        show_brdlist(head, 1, newflag, num);
      }
      break;

    case Ctrl('U'):
      t_users();
      show_brdlist(head, 1, newflag, num);
      break;

    case Ctrl('B'):
      m_read();
      show_brdlist(head, 1, newflag, num);
      break;

#ifdef TAKE_IDLE
    case Ctrl('I'):
      t_idle();
      show_brdlist(head, 1, newflag, num);
      break;
#endif

    case KEY_UP:
    case 'p':
      if(num-- <= 0) num = brdnum - 1;
      break;

    case KEY_DOWN:
    case 'n':
      if(++num < brdnum) break;

    case '0':
    case KEY_HOME:
      num = 0;
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if((tmp = search_num(ch, brdnum)) >= 0) num = tmp;
      if(num >= brdnum) num = brdnum - 1;
      brdlist_foot();
      break;

    /* Dopin */
    case 'h':
      DL_func("bin/help.so:help", READBRD);
      show_brdlist(head, 1, newflag, num);
      break;

    case Ctrl('A'):
       Announce();
       show_brdlist(head, 1, newflag, num);
       break;

    case '/':
      if((tmp = search_board(num)) >= 0) num = tmp;
      show_brdlist(head, 1, newflag, num);
      break;

    case 'S':
      cuser.uflag ^= BRDSORT_FLAG;
      qsort(nbrd, brdnum, sizeof(boardstat), cmpboard);
      head = 999;
      break;

    case 'y':
      if(yank_flag == 2) yank_flag = 0;
      else yank_flag ^= 1;
      brdnum = -1;
      break;

    case 'z':                   /* opus: no perm check ? */
      if(HAS_PERM(PERM_BASIC)) {
        ptr = &nbrd[num];
        ptr->zap = !ptr->zap;
        ptr->total = -1;
        zapbuf[ptr->pos] = (ptr->zap ? 0 : login_start_time);
        head = 999;
      }
      break;

    case 'f':
    case 'u':
      {
         char ask[2];
         FILE *fp;

         ptr = &nbrd[num];
         setuserfile(genbuf, "favor_boards");

         if(ch == 'f') {
            if(cuser.extra_mode[0] == 1) break;

            if(belong(genbuf, ptr->name))
               pressanykey("§Úªº³Ì·R¦Cªí¤¤¤w¦s¦b¦¹¬ÝªO...");
            else {
               out2line(1, b_lines, "");
               getdata(b_lines, 0, "½T©w¥[¤J ¬ÝªO§Úªº³Ì·R y/N ? ",
                  ask, 2, LCECHO, 0);
               if(ask[0] == 'y') {
                  fp = fopen(genbuf, "a+");
                  fprintf(fp, "%s\n", ptr->name);
                  fclose(fp);
                  pressanykey("¤w¦¨¥\\¥[¤J %s ¬ÝªO", ptr->name);
               }
            }
         }
         else {
            if(cuser.extra_mode[0] == 0) break;

            if(!belong(genbuf, ptr->name))
               pressanykey("§Úªº³Ì·R¬ÝªO¦Cªí¤¤µL¦¹¬ÝªO");
            else {
               out2line(1, b_lines, "");
               getdata(b_lines, 0, "½T©w±q¬ÝªO§Úªº³Ì·R¤¤²¾°£ y/N ? ",
                                    ask, 2, LCECHO, 0);

               if(ask[0] == 'y') {
                  if(remove_file_line(genbuf, ptr->name, 'B'))
                     pressanykey("¬ÝªO²¾°£¥¢±Ñ ©Î¤£¦s¦b©óÀÉ®×³]©w¤¤");
                  else {
                     pressanykey("¬ÝªO %s ²¾°£¦¨¥\\",
                                 ptr->name);
                     ask[0] = 1;
                  }
               }
            }
         }

         if(cuser.extra_mode[0] == 1 && ask[0] == 1) load_boards(NULL);
         show_brdlist(head, 1, newflag, num);
         break;
      }

    case 'B':
      ptr = &nbrd[num];
#ifdef BM_PRINCIPLE
      if(!is_BM(ptr->BM) && !is_board_manager(ptr->name)) break; {
#else
      memset(genbuf, 0, 200);

      strcpy(genbuf, ptr->BM);
      if(genbuf[0] > ' ') {
         for(cntc = 0; cntc < IDLEN+1 ; cntc++)
            if(genbuf[cntc] == '/' || genbuf[cntc] == 0) {
               genbuf[cntc] = 0;
               break;
            }
         strcpy(&genbuf[IDLEN+2], cuser.userid);

         for(cntc = 0; cntc < IDLEN+1 && genbuf[cntc] > 0 ; cntc++)
            genbuf[cntc] = tolower(genbuf[cntc]);
         for(cntc = 0; cntc < IDLEN+1 && genbuf[cntc+IDLEN+2] > 0 ; cntc++)
            genbuf[cntc+IDLEN+2] = tolower(genbuf[cntc+IDLEN+2]);

         if(!strcmp(genbuf, &genbuf[IDLEN+2]))
#endif
         {
            getdata(b_lines - 5, 0,
            "½Ð¿é¤J·s¬ÝªO¤¤¦W: ", genbuf, 32, DOECHO, 0);
            brc_initial(ptr->name);
            strcpy(&ptr->title[7], genbuf);
            brc_num = brc_changed = 1;
            brc_update();
         }
#ifndef BM_PRINCIPLE
         else break;
#endif

         bid = getbnum(ptr->name);
         if(get_record(fn_board, &bh, sizeof(bh), bid) == -1) {
            pressanykey(err_bid);
            break;
         }

         memcpy(&newbh, &bh, sizeof(bh));
         strcpy(&newbh.title[7], genbuf);
         if(memcmp(&newbh, &bh, sizeof(bh))) {
            if(strcmp(bh.brdname, newbh.brdname)) {
               char src[60], tar[60];

               setapath(src, bh.brdname);
               setapath(tar, newbh.brdname);
               Rename(src, tar);
            }
             setup_man(&newbh);
             substitute_record(fn_board, &newbh, sizeof(newbh), bid);
             touch_boards();
             /* woju */
             log_usies("SetBoard", newbh.brdname);
         }
      }
      show_brdlist(head, 1, newflag, num);
      break;

    case 'l':
       ptr = &nbrd[num];

#ifdef BM_PRINCIPLE
       if(!is_BM(ptr->BM) && !is_board_manager(ptr->name)) break;
#else
       memset(genbuf, 0, 200);
       strcpy(genbuf, ptr->BM);

       if(genbuf[0] > ' ') {
          for(cntc = 0; cntc < IDLEN+1 ; cntc++)
             if(genbuf[cntc] == '/' || genbuf[cntc] == 0) {
                genbuf[cntc] = 0;
                break;
             }
       }

       strcpy(&genbuf[IDLEN+2], cuser.userid);
       for(cntc = 0; cntc < IDLEN+1 && genbuf[cntc] > 0 ; cntc++)
          genbuf[cntc] = tolower(genbuf[cntc]);
       for(cntc = 0; cntc < IDLEN+1 && genbuf[cntc+IDLEN+2] > 0 ; cntc++)
          genbuf[cntc+IDLEN+2] = tolower(genbuf[cntc+IDLEN+2]);

       if(!strcmp(genbuf, &genbuf[IDLEN+2]) || is_board_manager(ptr->name))
#endif
       {
          char ans[2];

          brc_initial(ptr->name);
          bid = getbnum(ptr->name);
          if(get_record(fn_board, &bh, sizeof(bh), bid) == -1) {
             pressanykey(err_bid);
             break;
          }

          memcpy(&newbh, &bh, sizeof(bh));
          for(cntc = 0 ; cntc < 4 ; cntc++) {
             sprintf(&genbuf[270], "½Ð¿é¤J»¡©úÄæ¦C %d ¤å¦r±Ô­z : ", cntc+1);
             out2line(1, 18, &genbuf[270]);
             getdata(19, 0, "", &genbuf[cntc * 80] ,
#ifdef SHOW_BRDMSG
                     cntc < 3 ? 80 : 63
#else
                      80
#endif
                     ,DOECHO, 0);
             check_ascii(&genbuf[80 * cntc]);
          }

          getdata(18, 0, "½Ð±z½T©w (y/N) ? ", ans, 2, LCECHO, 0);
          if(ans[0] == 'y') {
             for(cntc = 0 ; cntc < 4 ; cntc++)
                strcpy(&newbh.document[cntc * 80], &genbuf[cntc * 80]);

             setup_man(&newbh);
             substitute_record(fn_board, &newbh, sizeof(newbh), bid);
             touch_boards();
             log_usies("SetBoard", newbh.document);
          }

          show_brdlist(head, 1, newflag, num);
       }
       break;

    case 'v':
    case 'V':
      ptr = &nbrd[num];
      brc_initial(ptr->name);
      if(ch == 'v') {
        ptr->unread = 0;
        zapbuf[ptr->pos] = time((time_t *) &brc_list[0]);
      }
      else zapbuf[ptr->pos] = brc_list[0] = ptr->unread = 1;

      brc_num = brc_changed = 1;
      brc_update();
      show_brdlist(head, 0, newflag, num);

      if(num < brdnum -1) num++;
      break;

    case 's':
       if((tmp = search_board(-1)) < 0) {
          show_brdlist(head, 0, newflag, num);
          break;
       }
       num = tmp;
    case KEY_RIGHT:
    case '\n':
    case '\r':
    case 'r':
      {
        char buf[STRLEN];

        now_num = num;
        ptr = &nbrd[num];
        brc_initial(ptr->name);

        if(yank_flag == 2) {
          setbdir(buf, currboard);
          tmp = have_author(currboard) - 1;
          head = tmp - t_lines / 2;
          getkeep(buf, head > 1 ? head : 1, -(tmp + 1));
        }
        else if(newflag) {
          setbdir(buf, currboard);
          tmp = unread_position(buf, ptr);
          head = tmp - t_lines / 2;
          getkeep(buf, head > 1 ? head : 1, tmp + 1);
        }
        board_visit_time = zapbuf[ptr->pos];
        if(!ptr->zap) time((time_t *) &zapbuf[ptr->pos]);

        Read();

        ptr->total = head = -1;
        setutmpmode(newflag ? READNEW : READBRD);
      }
    }
    if((int)cuser.welcomeflag & 128) goto NO_GO1_SB;

    ptr = &nbrd[num];
    brc_initial(ptr->name);
    bid = getbnum(ptr->name);
    if(get_record(fn_board, &bh, sizeof(bh), bid) == -1) goto NO_GO_List;

    out2line(0, 19, bh.document[0] ? &bh.document[0] : "");
    out2line(0, 20, bh.document[80] ? &bh.document[80] : "");
    out2line(0, 21, bh.document[160] ? &bh.document[160] : "");
#ifndef SHOW_BRDMSG
    out2line(0, 22, bh.document[240] ? &bh.document[240] : "");
#endif

NO_GO_List:
#ifdef SHOW_BRDMSG
    if(!(cuser.welcomeflag & 4096)) {
      char buf[80];
      sprintf(buf, "%-67s", bh.document[240] ? &bh.document[240] : "");

      if(newflag) sprintf(&buf[67], "N %3d / %3d", num+1, brdnum);
      else sprintf(&buf[67], "P %5d", ptr->total);
      out2line(0, 22, buf);
    }
#endif

NO_GO1_SB:
  } while (ch != 'q');

  save_zapbuf();
}

int board() {
  choose_board(1);
  return 0;
}

int local_board() {
  boardprefix = str_local_board;
  choose_board(1);
  return 0;
}

int tw_board() {
  boardprefix = str_tw_board;
  choose_board(1);
  return 0;
}

int Boards() {
  boardprefix = NULL;
  choose_board(0);
  return 0;
}

int Boards1() {
   if(cuser.extra_mode[0] == 1) {
      cuser.extra_mode[0] = 0;
      strcpy(currboard, DEFAULTBOARD);
      load_boards(DEFAULTBOARD);
   }

   Boards();
   return FULLUPDATE;
}

int New() {
  int mode0 = currutmp->mode;
  int stat0 = currstat;

  boardprefix = NULL;
  choose_board(1);
  currutmp->mode = mode0;
  currstat = stat0;
  return 0;
}

int b_favor(void) {
   b_favor_set(0, 0, 0);
   return FULLUPDATE;
}

void force_board(char *bname) {
   boardstat *ptr;

   brdnum = 0;
   if (!cuser.userlevel) return;        /* guest skip force read */
   if(!strcmp("guest", cuser.userid)) return;

   load_boards(bname);
   ptr = &nbrd[0];
   check_newpost(ptr);

   while(ptr->unread) {
      clear();
      move(10, 15);
      prints("[1;37;41m %s ª©¦³·s¤å³¹! ½Ð¾\\Åª§¹·s¤å³¹«á¦AÂ÷¶}... [m",bname);
      pressanykey(NULL);
      brc_initial(ptr->name);
      Read();
      check_newpost(ptr);
   }
}

#ifndef NO_USE_MULTI_STATION

uschar enter_list[MAX_STATIONS] = {0};
uschar enter_number = 0;

#endif

int v_and_V(void) {
   boardstat *ptr;
   char ch[2];

   if(now_num == 9999) return DONOTHING;

   getdata(b_lines-1, 0, "³]©w©Ò¦³¤å³¹ (U)¥¼Åª (V)¤wÅª (Q)¨ú®ø¡H [Q] ", ch,
           2, LCECHO, 0);
   if(ch[0] == 'v' || ch[0] == 'u') {
      ptr = &nbrd[now_num];
      brc_initial(ptr->name);

      if(ch[0] == 'v') {
         ptr->unread = 0;
         zapbuf[ptr->pos] = time((time_t *) &brc_list[0]);
      }
      else zapbuf[ptr->pos] = brc_list[0] = ptr->unread = 1;

      brc_num = brc_changed = 1;
      brc_update();
   }

   return FULLUPDATE;
}
