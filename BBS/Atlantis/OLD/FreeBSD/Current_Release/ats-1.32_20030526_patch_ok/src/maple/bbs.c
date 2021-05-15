/*-------------------------------------------------------*/
/* bbs.c        ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : bulletin boards' routines                    */
/* create : 95/03/29                                     */
/* update : 02/09/08 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "put_good.h"

#define HRM 2

extern l_post_msg();
extern item_update_passwd(char);

#ifdef USE_MULTI_TITLE
extern char title_color;
#endif

extern char currdirect[MAXPATHLEN];
extern char *globalboard[];
extern char global_board_cn;
extern char paste_filename[14];
extern char *Ctime(time_t *);
extern char station_list[][13];   /* 1.31-Stable §ïª©¥Î */

extern int is_global_board(char *);
extern int cmpuids(int uid, user_info *urec);
extern int online_message(char *uident, char *msg);
extern int v_and_V(void);
extern int now_num;
extern int mail_reply();
extern int check_newpost(boardstat *);

extern boardstat *nbrd;

time_t board_note_time;
time_t board_visit_time;

static char *brd_title;
static int continue_flag;

char editflag;
char *boardprefix;
int local_article;
int anonymous;

#define UPDATE_USEREC   (currmode |= MODE_DIRTY)

static void g_board_names(boardheader *fhdr) {
  AddNameList(fhdr->brdname);
}

int b_askperm_edit(void) {
   if(currmode & MODE_BOARD) {
      char buf[64];

      setbfile(buf, currboard, "ask_addperm");
      pressanykey("¬ÝªO (¨ã¶iªOÁôÂÃÅv­­©l¦³®Ä) ¡G%s",
                  vedit(buf, NA) ? "¥¼§ïÅÜ" : "§ó·s§¹²¦" );

      return FULLUPDATE;
   }
   return DONOTHING;
}

int b_perm_edit(void) {
   if(currmode & MODE_BOARD) {
      char buf[64];

      setbfile(buf, currboard, "permlist");
      pressanykey("¬ÝªO¨Ó»«¦W³æ (¹wÁôÂÃ¬ÝªOÅv­­©l¦³®Ä) ¡G%s",
                  vedit(buf, NA) ? "¥¼§ïÅÜ" : "§ó·s§¹²¦" );

      return FULLUPDATE;
   }
   return DONOTHING;
}

static int b_reject_edit(void) {
   if(currmode & MODE_BOARD) {
      char buf[64];

      setbfile(buf, currboard, "rejectlist");
      pressanykey("¬ÝªO©Úµ´¦W³æ (©Ò¦³¬ÝªO¬Ò¦³®Ä) ¡G%s",
                  vedit(buf, NA) ? "¥¼§ïÅÜ" : "§ó·s§¹²¦" );

      return FULLUPDATE;
   }
   return DONOTHING;
}

void make_blist() {
  CreateNameList();
  apply_boards(g_board_names);
}

static void g_bm_names(boardheader *fhdr) {
  char buf[IDLEN * 3 + 3];
  char* uid;

  strcpy(buf, fhdr->BM);
  uid = strtok(buf, "/");
  while (uid) {
     if (!InNameList(uid) && searchuser(uid))
        AddNameList(uid);
     uid = strtok(0, "/");
  }
}

void make_bmlist() {
  CreateNameList();
  apply_boards(g_bm_names);
}

void set_board() {
  boardheader *bp;

  bp = getbcache(currboard);
  board_note_time = bp->bupdate;
  brd_title = bp->BM;
  if(brd_title[0] <= ' ') brd_title = "¼x¨D¤¤";

  sprintf(currBM, "ªO¥D¡G%s", brd_title);
  brd_title = (bp->bvote == 1 ? "¥»¬ÝªO¶i¦æ§ë²¼¤¤" : bp->title + 7);

  currmode = (currmode & MODE_DIRTY) | MODE_STARTED;
  if(HAS_PERM(PERM_SYSOP) || (is_board_manager(currboard) &&
    !is_global_board(currboard)) || (HAS_PERM(PERM_BM) && is_BM(currBM + 6) ||
    is_BM(currBM + 6) && strcmp(bp->station, DEFAULTST)) )
    currmode |= (MODE_BOARD | MODE_POST);
  else if(haspostperm(currboard)) currmode |= MODE_POST;
}

/* Dopin ÀË¬d¬O§_¦³¶]¹L¯Á¤Þ */
int check_links(void) {
   char buf[48];
   FILE *fp;

   sprintf(buf, BBSHOME "/index/%s", currboard);
   if(dashf(buf)) return 1;
   else return 0;
}

static int reader_results() {
   if(is_board_manager(currboard) || is_BM(currBM + 6))
      DL_func("bin/chk_result.so:check_results");
   else return DONOTHING;
}

static int ad_mon(void) {
   DL_func("bin/add_money.so:add_money");
   return FULLUPDATE;
}

static void readtitle() {
  showtitle(currBM, brd_title);
  prints("\
[¡ö]Â÷¶} [¡÷]¾\\Åª [^P]µoªí¤å³¹ [b]³Æ§Ñ¿ý [d]§R°£ [z]ºëµØ°Ï [TAB]¤åºK [h]elp\n\
[7m  ½s¸¹   ¤é ´Á  §@  ªÌ       ¤å  ³¹  ¼Ð  ÃD                    %-s  [0m",
  check_links() ? "[1;37;41mºëµØ°Ï¦ê¦C¼Ò¦¡[7m" : "              ");
}

void readdoent(int num, fileheader *ent) {
  int type;
  char *mark, *title, color;
  char cuncun[80];
  char genbuf[100];
  int tuid;
  user_info *uentp;
  int ci, cj;

  type = brc_unread(ent->filename) ? '+' : ' ';

  if((currmode & MODE_BOARD) && (ent->filemode & FILE_DIGEST))
    type = (type == ' ') ? '*' : '#';
  else if(ent->filemode & FILE_MARKED)
    type = (type == ' ') ? 'm' : 'M';

  if(ent->filemode & FILE_TAGED) type = 'D';

  title = subject(mark = ent->title);
  if(title == mark) {
    color = '1';
    mark = "¡¼";
  }
  else {
    color = '3';
    mark = "R:";
  }

  if(title[47]) strcpy(title + 44, " ¡K");  /* §â¦h¾lªº string ¬å±¼ */

  if(strncmp(currtitle, title, 40)) {
    if(title[0] == '[' && title[5] == ']' ||
       title[0] == '<' && title[5] == '>') {
      for(ci = 0 ; ci < 6 ; ci++) genbuf[ci] = title[ci];
      genbuf[6] = 0;
      sprintf(cuncun, "[1;37;40m%s[0;37;40m%s[0m", genbuf, &title[6]);
    }
    else strcpy(cuncun, title);
  }

  for(ci = 0 ; ci < 6 ; ci ++) if(ent->date[ci] == '/') break;
  ci = atoi(&ent->date[ci+1]) % 7;

  /* Dopin : ½u¤W°ª«G«× */
  cj = 0;
  if(tuid = getuser(ent->owner)) {
    uentp = (user_info *) search_ulist(cmpuids, tuid);

    if(uentp) {
      if(HAS_PERM(PERM_SYSOP)) cj = 1;               /* SYSOP µ´¹ï¥iµø */
      else if(uentp->userlevel & PERM_SYSOP &&       /* ¹ï¤èµµÁô¤£¥iµø */
      uentp->userlevel & PERM_DENYPOST) ;
                                    /* §Ú¬Ý¤£¨ìÁô§Î ¥B ¹ï¤èÁô§Î ¤£¥iµø */
      else if(!HAS_PERM(PERM_SEECLOAK) && uentp->invisible) goto CJ_HERE;
      else if(is_rejected(uentp)) {
                           /* §Ú±N¨ä³]¬°Ãa¤H ¦ý§Ú¥¼¿ï¾Ü¬Ý¤£¨£Ãa¤H ¥iµø */
        if(is_rejected(uentp) & 1 && !(cuser.welcomeflag & 256)) cj = 1;
        if(is_rejected(uentp) & 2) cj = 0;    /* ¹ï¤è±N§Ú³]Ãa¤H ¤£¥iµø */
      }
      else cj = 1;
    }
  }

CJ_HERE:
  if(strncmp(currtitle, title, 40))
    prints("%6d %c[1;33;40m%c[0;%2d;40m%-5s[0;%2d;40m%2d[%d;37;40m%-13.12"
           "s[0;37;40m%s %s[m\n",
           num, type, ent->report ? ent->report-32 : ' ', 31 + ci,
           ent->date[0] ? ent->date : "--/--", ent->goodpost ? 32 : 30,
           ent->goodpost, cj, ent->owner, mark, cuncun);
  else
    prints("%6d %c[1;33;40m%c[0;%2d;40m%-5s[0;%2d;40m%2d[%d;37;40m%-13.12"
           "s[1;3%cm[1m%s %s[m\n",
           num, type, ent->report ? ent->report-32 : ' ', 31 + ci,
           ent->date[0] ? ent->date : "--/--", ent->goodpost ? 32 : 30,
           ent->goodpost, cj, ent->owner, color, mark,
           title);
}

int cmpbnames(char *bname, boardheader *brec) {
  return (!ci_strncmp(bname, brec->brdname, sizeof(brec->brdname)));
}

int cmpfilename(fileheader *fhdr) {
  return (!strcmp(fhdr->filename, currfile));
}

int online_msg(int ent, fileheader *fhdr, char *direct) {
   online_message(fhdr->owner, "¶Ç»¼°T®§ : ");
   return FULLUPDATE;
}

int online_quest(int ent, fileheader *fhdr, char *direct) {
   my_query(fhdr->owner);
   return FULLUPDATE;
}

/* woju */
int cmpfmode(fileheader *fhdr) {
  return (fhdr->filemode & currfmode);
}

int do_select(int ent, fileheader *fhdr, char *direct) {
  static char bname[(IDLEN+1)*2];
  char bpath[60];
  struct stat st;

  out2line(0, 0, "");

  make_blist();
  namecomplete(MSG_SELECT_BOARD, bpath);
  if(*bpath) strcpy(bname, bpath);

  setbpath(bpath, bname);
  if((*bname == '\0') || (stat(bpath, &st) == -1)) {
    out2line(0, 2, err_bid);
    return FULLUPDATE;
  }

  if(!is_member(bname, 'B', "permlist", NULL)) return FULLUPDATE;
  brc_initial(bname);
  set_board();
  setbdir(direct, currboard);

  return NEWDIRECT;
}

/* ----------------------------------------------------- */
/* §ï¨} innbbsd Âà¥X«H¥ó¡B³s½u¬å«H¤§³B²zµ{§Ç             */
/* ----------------------------------------------------- */

outgo_post(fileheader *fh, char *board) {
  FILE *foo;

  if(foo = fopen("innd/out.bntp", "a")) {
    fprintf(foo, "%s\t%s\t%s\t%s\t%s\n", board,
                 fh->filename, cuser.userid, cuser.username, fh->title);
    fclose(foo);
  }
}

void cancelpost(fileheader *fhdr, int by_BM) {
  int fd;
  char fpath[STRLEN];
  char db[26], dj[26];

  setbfile(fpath, currboard, fhdr->filename);
  if ((fd = open(fpath, O_RDONLY)) >= 0) {
#define NICK_LEN        80
    char *ptr, *left, *right, nick[NICK_LEN + 1];
    FILE *fout;
    int ch;
    fileheader postfile;
    char fn2[STRLEN], *brd, genbuf[200];
    boardheader bh;

    if(!getbh(&bh, currboard)) return;

    sprintf(db, "deleted.%s", bh.station);
    sprintf(dj, "junk.%s", bh.station);

    brd = by_BM ? db : dj;
    setbpath(fn2, brd);
    stampfile(fn2, &postfile);
    memcpy(postfile.owner, fhdr->owner, IDLEN + TTLEN + 10);
    postfile.savemode = 'D';
    /* Dopin 070801 */
    sprintf(postfile.title, "%-32.32s -%s", fhdr->title, cuser.userid);
    /* Dopin */

    if((fhdr->savemode == 'S') && /* ¥~Âà«H¥ó */
       (atoi(fhdr->filename + 2) > login_start_time - 7 * 86400)) {
       /* 7 ¤Ñ¤§¤º¦³®Ä */
      ptr = nick;
      ch = read(fd, ptr, NICK_LEN);
      ptr[ch] = '\0';
      if(!strncmp(ptr, str_author1, LEN_AUTHOR1) ||
        !strncmp(ptr, str_author2, LEN_AUTHOR2)) {

        if(left = (char *) strchr(ptr, '(')) {
          right = NULL;
          for(ptr = ++left; ch = *ptr; ptr++) {
            if(ch == ')') right = ptr;
            else if(ch == '\n') break;
          }

          if(right != NULL) {
            *right = '\0';

            if(fout = fopen("innd/cancel.bntp", "a")) {
              fprintf(fout, "%s\t%s\t%s\t%s\t%s\n",
                currboard, fhdr->filename, fhdr->owner   /*  cuser.userid*/
                ,left, fhdr->title);
              fclose(fout);
            }
          }
        }
      }
    }

    close(fd);
    Rename(fpath, fn2);
    setbdir(genbuf, brd);
    append_record(genbuf, &postfile, sizeof(postfile));
#undef  NICK_LEN
  }
}

/* ----------------------------------------------------- */
/* µoªí¡B¦^À³¡B½s¿è¡BÂà¿ý¤å³¹                            */
/* ----------------------------------------------------- */

void do_reply_title(int row, char *title) {
  char genbuf[200];
  char genbuf2[4];

  if(ci_strncmp(title, str_reply, 4)) sprintf(save_title, "Re: %s", title);
  else strcpy(save_title, title);

  save_title[TTLEN - 1] = '\0';
  sprintf(genbuf, "±Ä¥Î­ì¼ÐÃD¡m%.69s¡n¶Ü?[Y] ", str_decode_M3(save_title, 1));
  getdata(row, 0, genbuf, genbuf2, 4, LCECHO, 0);
  if (*genbuf2 == 'n')
    getdata(++row, 0, "¼ÐÃD¡G", save_title, TTLEN, DOECHO, save_title);
}

#ifdef LOCK_ARTICAL
/* Dopin 04/09/03 */
static int lock_post(int ent, fileheader *fhdr, char *direct) {
  char buf[2], lock_file[FNLEN+27];

  if(!is_station_sysop() && !(currmode & MODE_BOARD)) return DONOTHING;
  if(currmode & MODE_SELECT) {
     pressanykey("½Ð©ó¤å³¹¤@Äý¥þ¤å¼Ò¦¡¤¤°õ¦æ¥»¥\\¯à");
     return FULLUPDATE;
  }

  if(!is_board_manager(currboard) && !is_BM(currBM + 6)) {
     pressanykey("¦¹¬ÝªO«D±z©ÒºÞÁÒ");
     return FULLUPDATE;
  }

  if(fhdr->report == 'l' || fhdr->report == 's') {
     pressanykey("¦¹¤å¤w¸gÂê©w");

     if(!is_board_manager(currboard)) return FULLUPDATE;
  }
  else if(fhdr->report) {
     pressanykey("¦¹¤å¤w´£³ø ½Ð³ø½Ð SYSOP ¨ú®ø«á¦AÂê©w");
     return FULLUPDATE;
  }

  sprintf(lock_file, "%s/%s", currboard, fhdr->filename);

  getdata(b_lines, 0, is_board_manager(currboard) ? "(L)ªO¥DÂê©w (S)¯¸°ÈÂê©w"
          " (U)¨ú®øÂê©w [Q] ? " : "(L)Âê©w¤å³¹ [Q] ? ", buf, 2, LCECHO, 0);

  if(*buf == 'u') {
     if(is_board_manager(currboard)) {
        getdata(b_lines, 0, "½T©w¨ú®øÂê©w y/N ? ", buf, 2, LCECHO, 0);
        if(*buf == 'y') fhdr->report = 0;
        log_usies("UnLock", lock_file);
     }
  }
  else {
     if(*buf == 's') { if(!is_board_manager(currboard)) return FULLUPDATE; }
     else if(*buf != 'l') return FULLUPDATE;

     fhdr->filemode &= ~FILE_TAGED;
     fhdr->report = *buf;
     log_usies("Lock", lock_file);
  }

  substitute_record(direct, fhdr, sizeof(*fhdr), ent);
  return FULLUPDATE;
}
#endif

static void do_reply(fileheader *fhdr) {
  char genbuf[200];
  int ent_as_flag = 0;

#ifdef LOCK_ARTICAL
  if(fhdr->report == 's') return;
#endif

  getdata(b_lines - 1, 0, "¡¶ ¦^À³¦Ü (F)¬ÝªO (M)§@ªÌ«H½c (B)¤GªÌ¬Ò¬O (O)¨ä¥¦"
          " (Q)¨ú®ø¡H[F] ", genbuf, 3, LCECHO, 0);
  switch (genbuf[0])
  {
  case 'o':
    ent_as_flag = -1;
  case 'm':
    mail_reply(ent_as_flag, fhdr, 0);
  case 'q':
  case 'Q':
    break;

  case 'b':
    curredit = EDIT_BOTH;
  default:
    strcpy(currtitle, fhdr->title);
    strcpy(quote_user, fhdr->owner);
    quote_file[79] = fhdr->savemode;
    do_post();
  }
  *quote_file = 0;
}

/* woju */
brdperm(char* brdname, char* userid) {
   boardheader *bp;
   int uid = searchuser(userid);

   bp = getbcache(currboard);
   if(uid && bp) {
     int level = bp->level;
     char* ptr = bp->BM;
     char buf[64], manager[IDLEN + 1];
     userec xuser;

     get_record(fn_passwd, &xuser, sizeof(xuser), uid);
     if((level & PERM_POSTMASK) || ((level)?xuser.userlevel&(level):1))
       return 1;

     if(ptr[0] <= ' ') return 0;

     if(userid_is_BM(userid, ptr)) return 1;

     if((level & 0xffff) != PERM_SYSOP) return 0;

     strncpy(manager, ptr, IDLEN + 1);
     if(ptr = strchr(manager, '/')) *ptr = 0;
     sethomefile(buf, manager, fn_overrides);
     return (belong(buf, userid));
   }
   return 0;
}

/* Dopin */
void do_unanonymous_post(char mode, char* fpath) {
   fileheader mhdr;
   boardheader bh;
   char title[128], buf[(IDLEN+1)*2],  genbuf[200];
   FILE* fp;

   if(mode != 'U') { /* ¬ÝªO¿ù»~/ÁôªO ¤£¤½¶}µo¤å¸ê®Æ */
      if(!getbh(&bh, currboard) || bh.level & PERM_SECRET ||
         bh.level & PERM_ENTERSC) return;
   /* ¦pªG­n§â AllPost ³]¬°¤½¶}ªO ¨ºÁôªO´N¤£¥i¥H¤½¶} ¤Ï¤§°µ¬°¨t²ÎºÊ±±´N¨S®t */

#ifndef USE_NO_MULTI_STATION /* ª`·N³æ¯¸»P¦h¯¸¨ú¦W³W«hªº¤£¦P */
      sprintf(buf, "AllPost.%s", bh.station);
#else
      strcpy(buf, "AllPost");
#endif
   }

   setbpath(genbuf, mode == 'U' ? "UnAnonymous" : buf);
   if (dashd(genbuf)) {
      stampfile(genbuf, &mhdr);
      unlink(genbuf);
      Link(fpath, genbuf);
      strcpy(mhdr.owner, cuser.userid);
      strcpy(mhdr.title, save_title);
      mhdr.savemode = 0;
      mhdr.filemode = 0;
      setbdir(title, mode == 'U' ? "UnAnonymous" : buf);
      append_record(title, &mhdr, sizeof(mhdr));
   }
}

int do_post() {
  fileheader postfile;
  char fpath[80], buf[80];
  int aborted = 0;
  char genbuf[200];
  FILE *fpa;
  boardheader bh;
#ifdef COUNT_MONEY_WHEN_POST
  int filelen;
  time_t begin_t, over_t;
#endif

  if(!getbh(&bh, currboard)) return DONOTHING;

  clear();
  if(!(currmode & MODE_POST) ||
    (!HAS_PERM(PERM_BOARD) && !strcmp(currboard, DEFAULTBOARD))) {
    pressanykey("¹ï¤£°_¡A±z¥Ø«eµLªk¦b¦¹µoªí¤å³¹¡I");
    return FULLUPDATE;
  }

  if(deny_me() && !HAS_PERM(PERM_SYSOP)) {
     pressanykey("«Ü©êºp¡A§A³QªO¥D°±¤î POST ªºÅv¤O...");
     return FULLUPDATE;
  }

  more("etc/post.note", NA);
  prints("µoªí¤å³¹©ó¡i %s ¡j¬ÝªO\n\n", currboard);

  if (quote_file[0])
    do_reply_title(20, currtitle);
  else {
#ifdef ARTICAL_CLASS
    char *s_title = {
    "1.[¤ß±o] 2.[«ØÄ³] 3.[¸ê°T] 4.[°Q½×] 5.[°ÝÃD] 6.[¶¢²á] 7.[ÇÉÇ»] 8.[¨ä¥L] "
    };

    if(!(cuser.welcomeflag & 1024)) {
      getdata(21, 0, s_title, genbuf, 2, LCECHO, 0);
      if(genbuf[0] > '0' && genbuf[0] < '9') {

        char title_b[13] = { "Ãþ§O¡G" };

        for( ; aborted < 7 ; aborted++)
          title_b[aborted + 6] = save_title[aborted] =
          s_title[(genbuf[0] - '0' - 1) * 9 + 2 + aborted];

        title_b[12] = 0;
        move(21, 0);
        clrtoeol();
        out2line(1, 20, title_b);
      }
    }
#endif
    getdata(21, 0, "¼ÐÃD¡G", &save_title[aborted], TTLEN-aborted-1, DOECHO, 0);
  }

  if(!save_title[aborted]) return FULLUPDATE;
  curredit &= ~EDIT_MAIL;
  setutmpmode(POSTING);

  /* ¥¼¨ã³Æ Internet Åv­­ªÌ¡A¥u¯à¦b¯¸¤ºµoªí¤å³¹ */
  if(HAS_PERM(PERM_INTERNET)) local_article = 0;
  else local_article = 1;

  buf[0] = 0;
#ifdef COUNT_MONEY_WHEN_POST
  time(&begin_t);
#endif

  cuser.normal_post = 1;
  aborted = vedit(buf, YEA);
  cuser.normal_post = 0;
  if(aborted == -1
#ifdef CHECK_POST_TIME
     || !check_post_time()
#endif
    ) {
    unlink(buf);
    pressanykey(NULL);
    return FULLUPDATE;
  }

#ifdef COUNT_MONEY_WHEN_POST
  time(&over_t);
  filelen = dp_filelength(buf);
#endif

  /* build filename */
  setbpath(fpath, currboard);
  stampfile(fpath, &postfile);

  Rename(buf, fpath);
  strcpy(postfile.owner, cuser.userid);

  /* set owner to Anonymous , for Anonymous board */
#ifdef HAVE_ANONYMOUS
   if(anonymous) strcpy(postfile.owner, "ATS");
#endif

  strcpy(postfile.title, save_title);
  if(aborted == 1) {            /* local save */
    postfile.savemode = 'L';
    postfile.filemode = FILE_LOCAL;

    if(anonymous) {
      fpa = fopen("adm/anonymous_post", "a+");
      fseek(fpa, SEEK_END , 0);
      fprintf(fpa, "%s %s %s\n", cuser.userid, postfile.title, currboard);
      fclose(fpa);
    }
  }
  else {
    postfile.savemode = 'S';
    if(anonymous) {
      fpa = fopen("adm/anonymous_post", "a+");
      fseek(fpa, SEEK_END , 0);
      fprintf(fpa, "%s %s %s\n", cuser.userid, postfile.title, currboard);
      fclose(fpa);
    }
  }

  setbdir(buf, currboard);
  if(append_record(buf, &postfile, sizeof(postfile)) != -1) {
    if(currmode & MODE_SELECT)
      append_record(currdirect,&postfile,sizeof(postfile));
    if(aborted != 1) outgo_post(&postfile, currboard);

    brc_addlist(postfile.filename);
    outs("¶¶§Q¶K¥X§G§i ");

    strcpy(genbuf, bh.title);
    genbuf[4] = 0;
    if(!(bh.yankflags[0])
#ifndef COUNT_PERSONAL
    && strcmp(genbuf, "­Ó¤H") && strcmp(genbuf, "¸sÅé")
#endif
    ) {
      cuser.numposts++;

#ifdef COUNT_MONEY_WHEN_POST
      int add_money = getpostmoney(begin_t, over_t, filelen, 'P');

      prints("³o¬O±zªº²Ä %d ½g¤å³¹ ±o %d ª÷¹ô¡C", cuser.numposts, add_money);
      cuser.havemoney += add_money;
#else
      prints("³o¬O±zªº²Ä %d ½g¤å³¹¡C", cuser.numposts);
#endif

#ifdef UPDATE_WHEN_POST
      item_update_passwd('l');
      xuser.numposts = cuser.numposts;
#ifdef COUNT_MONEY_WHEN_POST
      xuser.havemoney = cuser.havemoney;
#endif
      item_update_passwd('u');
#else
      UPDATE_USEREC;
#endif
    }

    /* ¦^À³¨ì­ì§@ªÌ«H½c */
    if(curredit & EDIT_BOTH) {
      char *str, *msg = "¦^À³¦Ü§@ªÌ«H½c";

      if(str = strchr(quote_user, '.')) {
        if(bbs_sendmail(fpath, save_title, str + 1) < 0) msg = "§@ªÌµLªk¦¬«H";
      }
      else {
        sethomepath(genbuf, quote_user);
        stampfile(genbuf, &postfile);
        unlink(genbuf);
        Link(fpath, genbuf);

        strcpy(postfile.owner, cuser.userid);
        strcpy(postfile.title, save_title);
        postfile.savemode = 'B';/* both-reply flag */
        sethomedir(genbuf, quote_user);
        if(append_record(genbuf, &postfile, sizeof(postfile)) == -1)
          msg = err_uid;
      }
      outs(msg);
      curredit ^= EDIT_BOTH;
    }
    if(!anonymous && aborted != 2) do_unanonymous_post('N', fpath);
    else                           do_unanonymous_post('U', fpath);
  }

  if(HAS_PERM(PERM_LOGINOK)) pressanykey(NULL);
  return FULLUPDATE;
}

static int reply_post(int ent, fileheader *fhdr, char *direct) {
  if(!(currmode & MODE_POST)) return DONOTHING;

  setdirpath(quote_file, direct, fhdr->filename);
  do_reply(fhdr);
  *quote_file = 0;
  return FULLUPDATE;
}

static int edit_post(int ent, fileheader *fhdr, char *direct) {
   char emode;
   char fpath[80], fpath0[80], genbuf[200];
   int edit_mode;
   fileheader postfile;
   extern bad_user(char* name);

#ifdef LOCK_ARTICAL
   if(fhdr->report) return DONOTHING; /* Dopin: ´£³ø»PÂê©wªº¤å³¹³£¤£¯à­×§ï */
   if(strstr(currboard, "AllPost.")) return DONOTHING;
#endif

   if(HAS_PERM(PERM_SYSOP) ||
      !strcmp(fhdr->owner, cuser.userid) && strcmp(cuser.userid, "guest") &&
      !belong("etc/all_anonymous", currboard) && !bad_user(cuser.userid))
      edit_mode = 0;
   else edit_mode = 2;

   setdirpath(genbuf, direct, fhdr->filename);
   local_article = fhdr->filemode & FILE_LOCAL;
   strcpy(save_title, fhdr->title);

   emode = cuser.extra_mode[5];
   cuser.extra_mode[5] =
#ifndef RECORD_MODIFY_ARTICAL
   cuser.normal_post =
#endif
   1;
#ifdef RECORD_MODIFY_ARTICAL
   cuser.normal_post = 2;
#endif

   if(vedit(genbuf, edit_mode) != -1) {
      strcpy(fhdr->title, save_title);
      substitute_record(direct, fhdr, sizeof(*fhdr), ent);
   }

   cuser.normal_post = 0;
   cuser.extra_mode[5] = emode;
   return NEWDIRECT;
}

static int cross_post(int ent, fileheader *fhdr, char *direct) {
  char xboard[(IDLEN+1)*2], fname[MAXPATHLEN], xfpath[80], xtitle[80],
       genbuf[96], genbuf2[4];
  int author = 0;
  FILE *xptr;
  fileheader xfile;
  boardheader bh;

#ifdef LOCK_ARTICAL
  if(fhdr->report == 's') return DONOTHING;
#endif

  eolrange(1, 2);

#ifndef NO_USE_NULTI_STATION    /* Dopin: ¦³¨Ï¥Î¦h¯¸¤~»Ý­n³o­Ó */
  getdata(1, 0, "Âà¦Ü¦ó¯¸ (©Îª½±µ Enter Âà¥»¯¸) : ", genbuf, IDLEN+1,
          DOECHO, 0);
  if(genbuf[0]) {
    getdata(2, 0, "¬ÝªO¦W : ", xboard, 26, DOECHO, 0);

    if(getbh(&bh, xboard)) {
      if(strcmp(bh.station, genbuf)) return FULLUPDATE;
      if(strcmp(bh.brdname, xboard)) return FULLUPDATE;
    }
    else return FULLUPDATE;
  }
  else
#endif
  {
    make_blist();
    move(2, 0);
    namecomplete("Âà¿ý ¥»¤å³¹ ©ó¬ÝªO¡G", xboard);
  }

  if(*xboard == '\0' || !haspostperm(xboard)) return FULLUPDATE;
  if(!getbh(&bh, xboard)) return FULLUPDATE;

  ent = 1;
  if(HAS_PERM(PERM_SYSOP))
    getdata(2, 0, "(1)­ì¤åÂà¸ü (2)ÂÂÂà¿ý®æ¦¡¡H[1] ", genbuf, 3, DOECHO, 0);
  else strcpy(genbuf, "2");

  if(genbuf[0] != '2') {
     ent = 0;
     getdata(2, 0, "«O¯d­ì§@ªÌ¦WºÙ¶Ü?[Y] ", genbuf2, 3, DOECHO, 0);
     if (genbuf2[0] != 'n' && genbuf2[0] != 'N') author = 1;
  }

  if(ent) sprintf(xtitle, "<Âà¿ý> %.66s", fhdr->title);
  else strcpy(xtitle, fhdr->title);

  if(!ent) {
    sprintf(genbuf, "±Ä¥Î­ì¼ÐÃD¡m%.59s¡n¶Ü ? [Y] ", str_decode_M3(xtitle, 1));
    getdata(2, 0, genbuf, genbuf2, 4, LCECHO, 0);

    if(*genbuf2 == 'n')
      if(getdata(2, 0, "¼ÐÃD¡G", genbuf, TTLEN, DOECHO, xtitle))
        strcpy(xtitle, genbuf);
  }

  getdata(2, 0, "(S)¦sÀÉ (L)¯¸¤º (Q)¨ú®ø¡H[Q] ", genbuf, 3, LCECHO, 0);
  if((genbuf[0] == 'l' || genbuf[0] == 's')
#ifdef CHECK_POST_TIME
     && check_post_time()
#endif
  ) {
    int currmode0 = currmode;

    currmode = 0;
    setbpath(xfpath, xboard);
    stampfile(xfpath, &xfile);
    if(author) strcpy(xfile.owner, fhdr->owner);
    else strcpy(xfile.owner, cuser.userid);
    strcpy(xfile.title, xtitle);

    if(genbuf[0] == 'l') {
      xfile.savemode = 'L';
      xfile.filemode = FILE_LOCAL;
    }
    else xfile.savemode = 'S';

    setbfile(fname, currboard, fhdr->filename);
    if(ent) {
      xptr = fopen(xfpath, "w");

      strcpy(save_title, xfile.title);
      strcpy(xfpath, currboard);
      strcpy(currboard, xboard);
      write_header(xptr, 0);
      strcpy(currboard, xfpath);

      fprintf(xptr, "¡° [¥»¤åÂà¿ý¦Û %s ¬ÝªO]\n\n", currboard);

      b_suckinfile(xptr, fname);
      fclose(xptr);
    }
    else {
      unlink(xfpath);
      Link(fname, xfpath);
    }

    setbdir(fname, xboard);
    append_record(fname, (char *) &xfile, sizeof(xfile));
    if(!xfile.filemode) outgo_post(&xfile, xboard);

    if(!(bh.yankflags[1])) {
       cuser.numposts++;
       UPDATE_USEREC;
    }

    pressanykey("¤å³¹Âà¿ý§¹¦¨");
    currmode = currmode0;
  }

  return FULLUPDATE;
}

static int read_post(int ent, fileheader *fhdr, char *direct) {
  char genbuf[200];
  int more_result;

  if(fhdr->owner[0] == '-') return DONOTHING;
#ifdef LOCK_ARTICAL
  if(fhdr->report == 's' && !is_board_manager(currboard)) return READ_NEXT;
#endif

  setdirpath(genbuf, direct, fhdr->filename);

  strcpy(vetitle, fhdr->title);
  if ((more_result = more(genbuf, YEA)) == -1)
    return DONOTHING;
#ifdef PUTGOODS_SUG
  show_puts(genbuf);
#endif

  brc_addlist(fhdr->filename);
  strncpy(currtitle, subject(fhdr->title), 40);
  strncpy(currowner, subject(fhdr->owner), IDLEN + 1);

/*
woju
*/
  switch (more_result) {
  case 1:
     return READ_PREV;
  case 2:
     return RELATE_PREV;
  case 3:
     return READ_NEXT;
  case 4:
     return RELATE_NEXT;
  case 5:
     return RELATE_FIRST;
  case 6:
     return FULLUPDATE;
  case 7:
  case 8:
    if (currmode & MODE_POST)
    {
      strcpy(quote_file, genbuf);
      do_reply(fhdr);
      *quote_file = 0;
    }
    return FULLUPDATE;
  case 9:
     return 'A';
  case 10:
     return 'a';
  case 11:
     return '/';
  case 12:
     return '?';
  }

#ifdef USE_MULTI_TITLE
  sprintf(genbuf, "[37;%2dm  ¾\\Åª¤å³¹  [31;47m  (R/Y)[30m¦^«H [31m"
    "(=[]<>)[30m¬ÛÃö¥DÃD [31m(¡ô¡õ)[30m¤W¤U«Ê [31m(¡ö)[30mÂ÷¶}  "
    "                [0m",
    title_color);
  outmsg(genbuf);
#else
  outmsg("[34;46m  ¾\\Åª¤å³¹  [31;47m  (R/Y)[30m¦^«H [31m"
    "(=[]<>)[30m¬ÛÃö¥DÃD [31m(¡ô¡õ)[30m¤W¤U«Ê [31m(¡ö)[30mÂ÷¶}  "
    "                [0m");
#endif

  switch (igetkey())
  {
  case 'q':
  case KEY_LEFT:
    break;

  case ' ':
  case KEY_RIGHT:
  case KEY_DOWN:
  case KEY_PGDN:
  case 'n':
    return READ_NEXT;

  case KEY_UP:
  case 'p':
  case Ctrl('P'):
  case KEY_PGUP:
    return READ_PREV;

  case '=':
    return RELATE_FIRST;

  case ']':
  case 't':
    return RELATE_NEXT;

  case '[':
    return RELATE_PREV;

  case '.':
  case '>':
    return THREAD_NEXT;

  case ',':
  case '<':
    return THREAD_PREV;

  /* Dopin */
#ifdef CTRL_G_REVIEW
  case Ctrl('G'):
     DL_func("bin/message.so:get_msg", 1);
     return FULLUPDATE;
#endif

  /* Dopin */
  case Ctrl('R'):
     DL_func("bin/message.so:get_msg", 0);
     return FULLUPDATE;

#ifdef CHOICE_RMSG_USER
  case Ctrl('V'):
    choice_water_id();
    return FULLUPDATE;
#endif

  case Ctrl('U'):
    t_users();
    return FULLUPDATE;

  case Ctrl('B'):
    m_read();
    return FULLUPDATE;

  case KEY_ESC: if (KEY_ESC_arg == 'n') {
     edit_note();
     return FULLUPDATE;
     }
     return DONOTHING;

#ifdef TAKE_IDLE
  case Ctrl('I'):
    t_idle();
    return FULLUPDATE;
#endif

  case 'y':
  case 'r':
  case 'R':
  case 'Y':
    if(currmode & MODE_POST)
    {
      strcpy(quote_file, genbuf);
      do_reply(fhdr);
      *quote_file = 0;
    }
  }
  return FULLUPDATE;
}

/* ----------------------------------------------------- */
/* ±Ä¶°ºëµØ°Ï                                            */
/* ----------------------------------------------------- */

int b_man() {
  char buf[64];
  boardheader bh;

  if(!getbh(&bh, currboard)) return DONOTHING;
  setapath(buf, currboard);

  a_menu(currboard, buf, (HAS_PERM(PERM_SYSOP) || is_board_manager(currboard)
      && !is_global_board(currboard)) ? 2 : (currmode & MODE_BOARD ? 1 : 0));

  return FULLUPDATE;
}

/* Dopin 08/20/02 ¥[¤J¥H¤U¨ç¦¡ */
static int clear_report(int ent, fileheader *fhdr, char *direct) {
   int ch;
   if(!HAS_PERM(PERM_SYSOP)) return DONOTHING;

   out2line(1, b_lines, "½T©w²M°£´£³øºX¼Ð y/N ? ");
   ch = igetkey();
   if(!(ch == 'y' || ch == 'Y')) return FULLUPDATE;

   fhdr->report = 0;
   substitute_record(direct, fhdr, sizeof(*fhdr), ent);
   return FULLUPDATE;
}

/* Dopin 02/10/31 */
static int tag_same(int ent, fileheader *fhdr, char *direct) {
   char ans[2], buf[TTLEN+1];
   int ch;

   if(!is_BM(currBM + 6) && !is_board_manager(currboard)) return;

   out2line(1, b_lines, "");
   getdata(b_lines, 2, "¼Ð°O (U)¦P§@ªÌ (T)¦P¼ÐÃD (Q)Â÷¶} [Q]: ", ans, 2,
           LCECHO, 0);

   if(ans[0] == 'u')
   ch = fileheader_marked(direct, FILE_TAGED, fhdr->owner, 'U');
   if(ans[0] == 't')
   ch = fileheader_marked(direct, FILE_TAGED, fhdr->title, 'T');

   if(!ch) pressanykey("¼Ð°O/¤Ï¼Ð°O §¹¦¨");

   return NEWDIRECT;
}

/* Show artical file full path    Dopin 09/10/02 */
static int b_showfname(int ent, fileheader *fhdr, char *direct) {
   char buf[80];

   if(!HAS_PERM(PERM_SYSOP)) return DONOTHING;

   sprintf(buf, "boards/%s/%s", currboard, fhdr->filename);
   pressanykey("%s", buf);
}

/* Dopin 07/09/01 */
static int del_good(int ent, fileheader *fhdr, char *direct) {
   time_t now;
   FILE *fp;

   if(is_BM(currBM + 6) || is_board_manager(currboard)) {
      char genbuf[80];

      if(fhdr->goodpost == 0) goto Clear_OVER;

#ifdef PUTGOODS_SUG
      if(cuser.extra_mode[5] == 3) genbuf[0] = 'y';
      else
#endif
         getdata(b_lines-1, 0, "½T©w±N±ÀÂË¼ÆÂk¹s y/N ? ", genbuf, 2,
                 LCECHO, 0);

      if(genbuf[0] == 'y') {
         fp = fopen(BBSHOME "/adm/clear_puts", "a+");
         now = time(NULL);
         sprintf(genbuf, "%s", Cdate(&now));
         genbuf[8] = 0;
         fprintf(fp, "%3d %-15s %-35.35s %-8s %-13s\n", fhdr->goodpost,
                      fhdr->filename, fhdr->title, genbuf, cuser.userid);
         fclose(fp);

         fhdr->goodpost = 0;
         substitute_record(direct, fhdr, sizeof(*fhdr), ent);

#ifdef PUTGOODS_SUG
         sprintf(genbuf, BBSHOME "/boards/%s/%s.suggest", currboard,
                 fhdr->filename);
         remove(genbuf);
#endif
      }
   }
Clear_OVER:
   return FULLUPDATE;
}

struct put_posts {
   char posts;
   char articals[11][15];
};
typedef struct put_posts pps;

/* Dopin 07/09/01 */
static int put_good_post(int ent, fileheader *fhdr, char *direct) {
   char genbuf[80];
   pps arti_buf;
   char i, j, n_posts = 0;
   FILE *fp;
   time_t now;
   boardheader bh;

   if(!getbh(&bh, currboard)) return;

   if(!HAS_PERM(PERM_LOGINOK) || cuser.userlevel < PERM_LOGINOK ||
      bh.yankflags[1])
      goto OVER_PUT;

   move(b_lines-1 , 0);
   if(!strcmp(fhdr->owner, cuser.userid)) {
      pressanykey("µLªk±ÀÂË¦Û¤vªº¤å³¹ ...");
      goto OVER_PUT;
   }

   getdata(b_lines-1, 0, "±ÀÂË¦¹½g¤å³¹ y/N ? ", genbuf, 2, LCECHO, 0);
   if(genbuf[0] != 'y') goto OVER_PUT;

   sprintf(genbuf, BBSHOME "/home/%s/put", cuser.userid);
   fp = fopen(genbuf, "r+");

   move(b_lines-1 , 0);
   if(fp) {
      if(fread(&arti_buf, sizeof(pps), 1, fp) == 1)
         if(arti_buf.posts < GOOD_POSTS) arti_buf.posts++;
         else {
            arti_buf.posts = 5;
            pressanykey("¤@¤Ñ¥u¯à±ÀÂË %d ½g¤å³¹", GOOD_POSTS);
            fseek(fp, SEEK_SET, 0);
            fwrite(&arti_buf, sizeof(pps), 1, fp);
            fclose(fp);
            goto OVER_PUT;
         }
   }
   else {
       fp = fopen(genbuf, "w+");
       arti_buf.posts = 1;
       for(i = 0; i < GOOD_POSTS ; i++)
          for(j = 0 ; j < 15 ; j++)
             arti_buf.articals[i][j] = 0;
   }

   for(i = 0 ; i < GOOD_POSTS ; i++)
      if(strstr(arti_buf.articals[i], fhdr->filename)) {
         fclose(fp);
         pressanykey("§A¤w¸g±ÀÂË¹L¦¹¤å³¹ ...");
         goto OVER_PUT;
      }

   if(fhdr->goodpost >= 99) {
      fclose(fp);
      fhdr->goodpost = 99;
      substitute_record(direct, fhdr, sizeof(*fhdr), ent);
      pressanykey("¦¹½g¤å³¹¤§±ÀÂË¼Æ¤w¹F³Ì¤j­È");
      goto OVER_PUT;
   }
   else {
      char tbuf[50];
      fhdr->goodpost++;
      strcpy(arti_buf.articals[arti_buf.posts-1], fhdr->filename);
      fseek(fp, SEEK_SET, 0);
      fwrite(&arti_buf, sizeof(pps), 1, fp);
      substitute_record(direct, fhdr, sizeof(*fhdr), ent);
      fclose(fp);

      fp = fopen(BBSHOME "/adm/put_articals", "a+");
      now = time(NULL);
      sprintf(tbuf, "%s", Cdate(&now));
      tbuf[8] = 0;
      fprintf(fp, "%-15s %-45.45s %-8s %-13s\n", fhdr->filename, fhdr->title,
              tbuf, cuser.userid);
      fclose(fp);

#ifdef PUTGOODS_SUG
      sprintf(genbuf, BBSHOME "/boards/%s/%s.suggest",
                      currboard, fhdr->filename);
      if((fp = fopen(genbuf, "a+")) != NULL) {
         char pmsg[50];
         int ch;

         fclose(fp);
         out2line(1, b_lines-1, "­n¥[¤J·N¨£¶Ü y/N ");
         ch = igetkey();
         if(tolower(ch) == 'y') {
            getdata(b_lines-1, 0, "½Ð¿é¤J·N¨£ : ", pmsg, 50, DOECHO, 0);
            if(pmsg[0]) {
               fp = fopen(genbuf, "a+");
               if(fp) {
                  fprintf(fp, "[1;31;40m¡÷ [1;37;40m%-13s : [0;37;40m%-50"
                              ".49s [1;33;40m%-s[m\n",
                              cuser.userid, pmsg, tbuf);
                  fclose(fp);
               }
            }
         }
      }
#endif
   }

OVER_PUT:
   return FULLUPDATE;
}

static int cite_post(int ent, fileheader *fhdr, char *direct) {
  char fpath[256];
  char title[TTLEN+1];

#ifdef LOCK_ARTICAL
  if(fhdr->report == 's') return DONOTHING;
#endif

  setbfile(fpath, currboard, fhdr->filename);
  strcpy(title, "¡º ");
  strncpy(title+3, fhdr->title, TTLEN-3);
  title[TTLEN] = '\0';
  a_copyitem(fpath, title, fhdr->owner);
  b_man();
  return FULLUPDATE;
}

/* Dopin 10/17/99 */
a_cite_posts(int ent, fileheader* fhdr, char* direct) {
   char f_target[256], f_origin[256], buf[ANSILINELEN], ans[2];
   int r_value = FULLUPDATE;
   FILE *fp, *fin;

#ifdef LOCK_ARTICAL
   if(fhdr->report == 's') return DONOTHING;
#endif
   if(!is_BM(currBM + 6) && !is_board_manager(currboard)) goto Return_TSX;

   move(b_lines-1, 0);
   if(!paste_filename[0]) {
      pressanykey("¨S¦³³]©wªþ¥[©óºëµØ°ÏÀÉ®×,½Ð¶i¤JºëµØ°Ï²¾¦Ü¸ÓÀÉÁä¤J 'i'");
      goto Return_TSX;
   }

   sprintf(f_target, "%s/%s", paste_path, paste_filename);
   setbfile(f_origin, currboard, fhdr->filename);

   if((fp = fopen(f_target, "r")) != NULL) {
      fclose(fp);
      fp = fopen(f_target, "a");

      if((fin = fopen(f_origin, "r")) != NULL) {
         memset(buf, '-', 74);
         buf[74] = '\0';
         fprintf(fp, "\n> %s <\n\n", buf);
         getdata(b_lines - 1, 1, "¬O§_¦¬¿ýÃ±¦WÀÉ³¡¥÷(Y/N)¡H[Y] ", ans, 2,
                 DOECHO, 0);

         while (fgets(buf, sizeof(buf), fin)) {
            if((ans[0] == 'n' || ans[0] == 'N') && (!strcmp(buf, "--\n") ||
               !strcmp(buf, "-- \n"))) break;

              fputs(buf, fp);
         }

         fclose(fin);
      }

      fclose(fp);
      r_value = POS_NEXT;
      pressanykey("ÀÉ®×ªþ¥[§¹¦¨");
      goto Return_TSX;
   }
   else
      pressanykey("ºëµØ°ÏÀÉ®×«ü¼Ðµo¥Í¿ù»~...½Ð­«·s¶i¤JºëµØ°Ï²¾¦Ü¸ÓÀÉÁä¤J 'i'");

Return_TSX:
   return r_value;
}

static int Cite_post(int ent, fileheader *fhdr, char *direct) {
  char fpath[256];
  char title[TTLEN+1];

#ifdef LOCK_ARTICAL
  if(fhdr->report == 's') return DONOTHING;
#endif

  setbfile(fpath, currboard, fhdr->filename);
  sprintf(title, "%s%.72s",
     (currutmp->pager > 1) ? "" : "¡º ", fhdr->title);
  title[TTLEN] = '\0';
  a_copyitem(fpath, title, fhdr->owner);
  load_paste();
  if (*paste_path)
     a_menu(paste_title, paste_path, paste_level);
  return FULLUPDATE;
}


static int Cite_posts(int ent, fileheader* fhdr, char* direct) {
   char fpath[256];

#ifdef LOCK_ARTICAL
   if(fhdr->report == 's') return DONOTHING;
#endif

   setbfile(fpath, currboard, fhdr->filename);
   load_paste();
   if (*paste_path && paste_level && dashf(fpath)) {
      fileheader fh;
      char newpath[MAXPATHLEN];

      strcpy(newpath, paste_path);
      stampfile(newpath, &fh);
      unlink(newpath);
      Link(fpath, newpath);
      strcpy(fh.owner, fhdr->owner);
      sprintf(fh.title, "%s%.72s",
         (currutmp->pager > 1) ? "" : "¡º ", fhdr->title);
      strcpy(strrchr(newpath, '/') + 1, ".DIR");
      append_record(newpath, &fh, sizeof(fh));
      return POS_NEXT;
   }
   bell();
   return DONOTHING;
}


static int Tag_posts(int ent, fileheader* fhdr, char* direct) {
   char fpath[256];

#ifdef LOCK_ARTICAL
   if(fhdr->report == 's') return DONOTHING;
#endif

   setbfile(fpath, currboard, fhdr->filename);
   load_paste();
   if(*paste_path && paste_level && dashf(fpath)) {
      fileheader fh;
      char newpath[MAXPATHLEN];

      strcpy(newpath, paste_path);
      stampfile(newpath, &fh);
      unlink(newpath);
      Link(fpath, newpath);
      strcpy(fh.owner, fhdr->owner);
      sprintf(fh.title, "%s%.72s",
         (currutmp->pager > 1) ? "" : "¡º ", fhdr->title);
      strcpy(strrchr(newpath, '/') + 1, ".DIR");
      append_record(newpath, &fh, sizeof(fh));
      return RELATE_NEXT;
   }
   bell();
   return DONOTHING;
}

static int Tag_mails(int ent, fileheader* fhdr, char* direct) {
   char fpath[256];

#ifdef LOCK_ARTICAL
  if(fhdr->report == 's') return DONOTHING;
#endif

   setbfile(fpath, currboard, fhdr->filename);
   if (HAS_PERM(PERM_BASIC) && dashf(fpath)) {
      fileheader fh;
      char newpath[MAXPATHLEN];

      sethomepath(newpath, cuser.userid);
      stampfile(newpath, &fh);
      unlink(newpath);
      Link(fpath, newpath);
      if (HAS_PERM(PERM_SYSOP))
         strcpy(fh.owner, fhdr->owner);
      else
         strcpy(fh.owner, cuser.userid);
      strcpy(fh.title, fhdr->title);
      strcpy(strrchr(newpath, '/') + 1, ".DIR");
      fh.savemode = 0;
      fh.filemode = FILE_READ;
      append_record(newpath, &fh, sizeof(fh));
      return RELATE_NEXT;
   }
   bell();
   return DONOTHING;
}

int edit_title(int ent, fileheader *fhdr, char *direct) {
  char genbuf[200];

#ifdef LOCK_ARTICAL
  if(fhdr->report == 's') return DONOTHING;
#endif

  if(HAS_PERM(PERM_SYSOP) || !strcmp(fhdr->owner, cuser.userid)) {
    fileheader tmpfhdr = *fhdr;
    int dirty = 0;

    if(getdata(b_lines - 1, 0, "¼ÐÃD¡G", genbuf, TTLEN, DOECHO, tmpfhdr.title))
    {
      strcpy(tmpfhdr.title, genbuf);
      dirty++;
    }

    if(HAS_PERM(PERM_SYSOP)) {
       if(getdata(b_lines - 1, 0, "§@ªÌ¡G",
                  genbuf, IDLEN + 2, DOECHO, tmpfhdr.owner)) {
          strcpy(tmpfhdr.owner, genbuf);
          dirty++;
       }

       if(getdata(b_lines - 1, 0, "¤é´Á¡G",
                  genbuf, 6, DOECHO, tmpfhdr.date)) {
          sprintf(tmpfhdr.date, "%+5s", genbuf);
          dirty++;
       }
    }

    if(getdata(b_lines - 1, 0, "½T©w(y/N) ? ", genbuf, 3, LCECHO, 0) &&
       *genbuf == 'y' && dirty) {
       *fhdr = tmpfhdr;
       substitute_record(direct, fhdr, sizeof(*fhdr), ent);
       if(currmode & MODE_SELECT)
       {
         int now;
         setbdir(genbuf,currboard);
         now=getindex(genbuf,fhdr->filename);
         substitute_record(genbuf,fhdr,sizeof(*fhdr),now);
       }
    }
    return FULLUPDATE;
  }
  return DONOTHING;
}

static post_tag(int ent, fileheader *fhdr, char *direct) {
   if(currstat == READING && !(currmode & MODE_BOARD)) return DONOTHING;

#ifdef LOCK_ARTICAL
   if(fhdr->report) {
      fhdr->filemode &= ~FILE_TAGED;
      return DONOTHING;
   }
#endif

   fhdr->filemode ^= FILE_TAGED;
   if(currmode & MODE_SELECT) {
      int now;
      char genbuf[100];

      setbdir(genbuf,currboard);
      now=getindex(genbuf,fhdr->filename);
      substitute_record(genbuf,fhdr,sizeof(*fhdr),now);
      sprintf(genbuf, "boards/%s/SR.%s", currboard, cuser.userid);
      substitute_record(genbuf, fhdr, sizeof(*fhdr), ent);
   }
   else
      substitute_record(direct, fhdr, sizeof(*fhdr), ent);
   return POS_NEXT;
}

static int post_del_tag(int ent, fileheader *fhdr, char *direct) {
  char genbuf[3];

  if ((currstat != READING) || (currmode & MODE_BOARD)) {
     getdata(1, 0, "½T©w§R°£¼Ð°O¤å³¹(Y/N)? [N]", genbuf, 3, LCECHO, 0);
     if (genbuf[0] == 'y')
     {
       char buf[STRLEN];

       currfmode = FILE_TAGED;
       if(currmode & MODE_SELECT){
         unlink(direct);
         currmode ^= MODE_SELECT;
         setbdir(direct, currboard);
         delete_files(direct, cmpfmode);
       }
       if(delete_files(direct, cmpfmode)) return DIRCHANGED;
     }
     return FULLUPDATE;
  }
  return DONOTHING;
}

static int edit_import(void) {
   char fpath[100];
   int aborted;

   if(!HAS_PERM(PERM_SYSOP)) return DONOTHING;

   sprintf(fpath, BBSHOME"/boards/%s/import_articals", currboard);
   aborted = vedit(fpath, NA);

   prints("±ÀÂË¤å³¹¤@Äý¡G%s\n", aborted ? "¥¼§ïÅÜ" : "§ó·s§¹²¦");

   return FULLUPDATE;
}

static int remark_post(int ent, fileheader *fhdr, char *direct) {
   char ch[2];
   char genbuf[100];
   char buf_artical[100];
   FILE *fp;

   if(!is_board_manager(currboard) && !(currmode & MODE_BOARD))
      return DONOTHING;

   if(fhdr->report) {
#ifdef LOCK_ARTICAL
      pressanykey("¦¹¤å³¹¤w´£³ø¹L©Î¤wÂê©w");
#else
      pressanykey("¦¹¤å³¹¤w´£³ø¹L");
#endif
      goto Remark_Over;
   }

   clear();
   move(2, 5);
   prints("%7d %-6.5s %-14.13s %-41.40s", ent, fhdr->date, fhdr->owner,
          fhdr->title);

   sprintf(buf_artical, "%7d %-6.5s %-14.13s %-41.40s", ent, fhdr->date,
           fhdr->owner, fhdr->title);
   getdata(4, 5, "½Ð¿é¤J¤å³¹µ¥¯Å [a/b/c/d/e] : ", ch, 2, LCECHO, 0);
   move(6, 5);

   if(ch[0] >= 'a' && ch[0] <= 'e') {
      prints("±z©Ò§P©wªºµ¥¯Å¬° [%c]", ch[0]);

      sprintf(genbuf, BBSHOME"/boards/%s/import_articals", currboard);
      fp = fopen(genbuf, "a+");

      if(fp == NULL) {
         pressanykey("¶}ÀÉ¥¢±Ñ,½Ð±N¦¹¿ù»~¦^³øµ¹ Dopin");
         goto Remark_Over;
      }

      fprintf(fp, "%s [%c]\n", buf_artical, ch[0]);
      fclose(fp);

      fhdr->report = ch[0];
      substitute_record(direct, fhdr, sizeof(*fhdr), ent);
      pressanykey("¤w±N¥»µ§¸ê®Æªþ¥[©óÀÉ§À");
   }
   else pressanykey("¿é¤J¿ù»~, ¨ú®ø");

Remark_Over:
   return FULLUPDATE;
}

static int mark_post(int ent, fileheader *fhdr, char *direct) {
   if(!(currmode & MODE_BOARD) && (is_global_board(currboard) ||
      !HAS_PERM(PERM_SYSOP)) )
      return DONOTHING;

   fhdr->filemode ^= FILE_MARKED;
   if(currmode & MODE_SELECT) {
      int now;
      char genbuf[100];

      setbdir(genbuf,currboard);
      now = getindex(genbuf,fhdr->filename);
      substitute_record(genbuf, fhdr, sizeof(*fhdr), now);
   }
   else substitute_record(direct, fhdr, sizeof(*fhdr), ent);

   return PART_REDRAW;
}

int del_range(int ent, fileheader *fhdr, char *direct) {
  char num1[8], num2[8];
  int inum1, inum2;

  if((currstat != READING) || (currmode & MODE_BOARD) &&
     (!is_global_board(currboard) || HAS_PERM(PERM_SYSOP)) ||
     HAS_PERM(PERM_SYSOP)) {
    getdata(1, 0, "[³]©w§R°£½d³ò] °_ÂI¡G", num1, 5, DOECHO, 0);
    inum1 = atoi(num1);

    if(inum1 <= 0) {
      outmsg("°_ÂI¦³»~");
      refresh();
      sleep(1);
      return FULLUPDATE;
    }

    getdata(1, 28, "²×ÂI¡G", num2, 5, DOECHO, 0);
    inum2 = atoi(num2);
    if(inum2 < inum1) {
      outmsg("²×ÂI¦³»~");
      refresh();
      sleep(1);
      return FULLUPDATE;
    }

    getdata(1, 48, msg_sure_ny, num1, 3, LCECHO, 0);
    if(*num1 == 'y') {
       outmsg("³B²z¤¤,½Ðµy«á...");
       refresh();

       if(currmode & MODE_SELECT) {
           int fd,size=sizeof(fileheader);
           char genbuf[100];
           fileheader rsfh;
           int i=inum1,now;

           if(currstat==RMAIL) sethomedir(genbuf,cuser.userid);
           else setbdir(genbuf,currboard);

           if((fd=(open(direct,O_RDONLY, 0)))!=-1) {
             if(lseek(fd, (off_t)(size * (inum1 - 1)), SEEK_SET) != -1) {
               while(read(fd,&rsfh,size) == size) {
                 if(i>inum2) break;
                 now=getindex(genbuf,rsfh.filename);
                 strcpy(currfile,rsfh.filename);
                 delete_file (genbuf, sizeof(fileheader), now, cmpfilename);
                 i++;
               }
             }
             close(fd);
           }
       }
       delete_range(direct, inum1, inum2, NULL);
       fixkeep(direct, inum1);
       return DIRCHANGED;
    }
    return FULLUPDATE;
  }
  return DONOTHING;
}

static void lazy_delete(fileheader *fhdr) {
  char buf[IDLEN+1];

  /* sprintf(buf, "-%s", fhdr->owner); */
  strncpy(fhdr->owner, buf, IDLEN + 1);
  /* Dopin 070801 */
  sprintf(fhdr->title, "< ¥»¤å¤w³Q [1;33;40m%s[m §R°£ >", cuser.userid);
  /* Dopin */
  fhdr->savemode = 'L';
}

static int del_post(int ent, fileheader *fhdr, char *direct) {
  char buf[100];
  char *t;
  int not_owned;
  char genbuf[4];
  boardheader bh;

#ifdef LOCK_ARTICAL
  if(fhdr->report) return DONOTHING;
#endif

  if(!getbh(&bh, currboard)) return DONOTHING;

  if((fhdr->filemode & FILE_MARKED) || (fhdr->filemode & FILE_DIGEST) ||
    (fhdr->owner[0] == '-')) return DONOTHING;
  not_owned = strcmp(fhdr->owner, cuser.userid);

  if(is_board_manager(currboard)) goto BY_PASS_2;
  if (!(currmode & MODE_BOARD) && not_owned || !strcmp(cuser.userid, "guest"))
    return DONOTHING;

BY_PASS_2:
  if(!HAS_PERM(PERM_SYSOP) && is_global_board(currboard)) return DONOTHING;

  getdata(1, 0, msg_del_ny, genbuf, 3, LCECHO, 0);
  if (genbuf[0] == 'y') {
    strcpy(currfile, fhdr->filename);

   if (!delete_file (direct, sizeof(fileheader), ent, cmpfilename)) {
      if( currmode & MODE_SELECT ){
        int now;
        char genbuf[100];
        setbdir(genbuf,currboard);
        now=getindex(genbuf,fhdr->filename);
        delete_file(genbuf, sizeof(fileheader),now,cmpfilename);
      }
      cancelpost(fhdr, not_owned);

      strcpy(buf, bh.title);
      buf[4] = 0;

      if(!not_owned && !(bh.yankflags[0])
#ifndef COUNT_PERSONAL
          && strcmp(buf, "­Ó¤H") && strcmp(buf, "¸sÅé")
#endif
      ) {
        if(cuser.numposts) cuser.numposts--;

#ifdef COUNT_MONEY_WHEN_POST
        sprintf(buf, "boards/%s/%s", currboard, fhdr->filename);

        not_owned = dp_filelength(buf);
        if(cuser.havemoney >= pos) cuser.havemoney -= not_owned;
        else cuser.havemoney = 0;
#endif

#ifdef UPDATE_WHEN_POST
        item_update_passwd('l');
#ifdef COUNT_MONEY_WHEN_POST
        xuser.havemoney = cuser.havemoney;
#endif
        xuser.numposts = cuser.numposts;
        item_update_passwd('u');
#else
        UPDATE_USEREC;
#endif

#ifdef COUNT_MONEY_WHEN_POST
        pressanykey("%s¡A±zªº¤å³¹´î¬° %d ½g ¦©°£²M¼ä¶O %d ¤¸",
                    msg_del_ok, cuser.numposts, getpostmoney(0, 0, pos, 'D'));
#else
        pressanykey("%s¡A±zªº¤å³¹´î¬° %d ½g", msg_del_ok, cuser.numposts);
#endif
      }
      else outs("¤å³¹¤w§R°£");

      refresh();
      sleep(1);

      return DIRCHANGED;
    }
  }
  return FULLUPDATE;
}


/* ----------------------------------------------------- */
/* ¨Ì§ÇÅª·s¤å³¹                                          */
/* ----------------------------------------------------- */

static int sequent_ent;


static int sequent_messages(fileheader *fptr) {
  static int idc;
  char ans[6];
  char genbuf[200];

  if(fptr == NULL) return (idc = 0);
  if(++idc < sequent_ent) return 0;
  if(!brc_unread(fptr->filename)) return 0;

  if(continue_flag) genbuf[0] = 'y';
  else {
     prints("Åª¨ú¤å³¹©ó¡G[%s] §@ªÌ¡G[%s]\n¼ÐÃD¡G[%s]",
     currboard, fptr->owner, fptr->title);
  }

  if(genbuf[0] != 'y' && genbuf[0]) {
    clear();
    return (genbuf[0] == 'q' ? QUIT : 0);
  }

  setbfile(genbuf, currboard, fptr->filename);
  brc_addlist(fptr->filename);

  strcpy(vetitle, fptr->title);
  if(more(genbuf, YEA) == 0)
     outmsg("[31;47m  [31m(R)[30m¦^«H  [31m(¡õ,n)[30m¤U¤@«Ê  [31m(¡ö,q)"
            "[30mÂ÷¶}  [0m");
  continue_flag = 0;

  switch (igetkey()) {
  case KEY_LEFT:
  case 'e':
  case 'q':
  case 'Q':
    break;

  case 'y':
  case 'r':
  case 'Y':
  case 'R':
    if(fptr->report == 's') break;;

    if(currmode & MODE_POST) {
      strcpy(quote_file, genbuf);
      do_reply(fptr);
      *quote_file = 0;
    }
    break;

  case ' ':
  case KEY_DOWN:
  case '\n':
  case 'n':
    continue_flag = 1;
  }

  clear();
  return 0;
}

static int sequential_read(int ent, fileheader *fhdr, char *direct) {
  char buf[40];

  clear();
  sequent_messages((fileheader *) NULL);
  sequent_ent = ent;
  continue_flag = 0;
  setbdir(buf, currboard);
  apply_record(buf, sequent_messages, sizeof(fileheader));
  return FULLUPDATE;
}

static int save_mail(int ent, fileheader *fh, char* direct) {
   fileheader mhdr;
   char fpath[MAXPATHLEN];
   char genbuf[MAXPATHLEN];
   char *p;

#ifdef LOCK_ARTICAL
   if(fh->report == 's') return DONOTHING;
#endif

   if(ent < 0) strcpy(fpath, direct);
   else {
      strcpy(genbuf, direct);
      if(p = strrchr(genbuf, '/')) *p = '\0';
      sprintf(fpath, "%s/%s", genbuf, fh->filename);
   }

   if(!dashf(fpath) || !HAS_PERM(PERM_BASIC)) {
      bell();
      return DONOTHING;
   }

   sethomepath(genbuf, cuser.userid);
   stampfile(genbuf, &mhdr);
   unlink(genbuf);
   Link(fpath, genbuf);

   if(HAS_PERM(PERM_SYSOP)) strcpy(mhdr.owner, fh->owner);
   else strcpy(mhdr.owner, cuser.userid);

   strncpy(mhdr.title, fh->title + ((currstat == ANNOUNCE) ? 3 : 0), TTLEN);
   mhdr.savemode = '\0';
   mhdr.filemode = FILE_READ;
   sethomedir(fpath, cuser.userid);

   if(append_record(fpath, &mhdr, sizeof(mhdr)) == -1) {
      bell();
      return DONOTHING;
   }
   return POS_NEXT;
}

/* ----------------------------------------------------- */
/* ¬ÝªO³Æ§Ñ¿ý¡B¤åºK¡BºëµØ°Ï                              */
/* ----------------------------------------------------- */

static int b_notes_edit() {
  boardheader bh;
  int pos;

  if(!(pos = search_record(fn_board, &bh, sizeof(bh), cmpbnames, currboard)))
     return DONOTHING;

  if(!HAS_PERM(PERM_SYSOP) && is_global_board(currboard)) return 0;
  if(!is_board_manager(currboard) && !is_BM(currBM + 6)) return 0;

  if(currmode & MODE_BOARD) {
    struct stat st;
    char buf[64];
    int aborted;

    setbfile(buf, currboard, fn_notes);
    if(aborted = vedit(buf, NA)) pressanykey(msg_cancel);
    else {
      getdata(3, 0, "½Ð³]©w¦³®Ä´Á­­(0 - 9999)¤Ñ¡H", buf, 5, DOECHO, 0);
      aborted = atol(buf);
      bh.bupdate = aborted ? time(0) + aborted * 86400 : 0;

      substitute_record(fn_board, &bh, sizeof(bh), pos);
      touch_boards();
    }
    return FULLUPDATE;
  }
  return 0;
}

static int b_water_edit() {
  if(currmode & MODE_BOARD) {
    struct stat st;
    char buf[64];
    int aborted;

    setbfile(buf, currboard, fn_water);
    vedit(buf, NA);
    return FULLUPDATE;
  }
  return 0;
}

static int b_notes() {
  char buf[64];

  setbfile(buf, currboard, fn_notes);
  if(more(buf, YEA) == -1) {
    clear();
    move(4, 20);
    outs("¥»¬ÝªO©|µL¡u³Æ§Ñ¿ý¡v¡C");
  }
  return FULLUPDATE;
}

int board_select() {
  char fpath[80];
  char genbuf[100];
  currmode &= ~MODE_SELECT;
  sprintf(fpath,"SR.%s",cuser.userid);

  if(currstat==RMAIL) {
    setuserfile(genbuf, fpath);
    unlink(genbuf);
  }
  else {
    setbfile(genbuf, currboard, fpath);
    unlink(genbuf);
  }

  if(currstat==RMAIL) sethomedir(currdirect,cuser.userid);
  else setbdir(currdirect, currboard);
  return NEWDIRECT;
}

int board_digest() {
  if(currmode & MODE_SELECT) board_select();

  currmode ^= MODE_DIGEST;
  if(currmode & MODE_DIGEST)      currmode &= ~MODE_POST;
  else if(haspostperm(currboard)) currmode |= MODE_POST;

  setbdir(currdirect, currboard);
  return NEWDIRECT;
}

int board_etc() {
  if(!HAS_PERM(PERM_SYSOP)) return DONOTHING;

  currmode ^= MODE_ETC;
  if(currmode & MODE_ETC)         currmode &= ~MODE_POST;
  else if(haspostperm(currboard)) currmode |= MODE_POST;

  setbdir(currdirect, currboard);
  return NEWDIRECT;
}

static int good_post(int ent, fileheader *fhdr, char *direct) {
  char genbuf[200];
  char genbuf2[200];

  if((currmode & MODE_DIGEST) || !(currmode & MODE_BOARD)) return DONOTHING;

  if(fhdr->filemode & FILE_DIGEST)
    fhdr->filemode = (fhdr->filemode & ~FILE_DIGEST);
  else {
    fileheader digest;
    char *ptr, buf[64];

    memcpy(&digest, fhdr, sizeof(digest));
    digest.filename[0] = 'G';
    strcpy(buf, direct);
    ptr = strrchr(buf, '/') + 1;
    ptr[0] = '\0';

    sprintf(genbuf, "%s%s", buf, digest.filename);
    if(!dashf(genbuf)) {
      digest.savemode = digest.filemode = 0;
      sprintf(genbuf2, "%s%s", buf, fhdr->filename);
      Link(genbuf2, genbuf);
      strcpy(ptr, fn_mandex);
      append_record(buf, &digest, sizeof(digest));
    }
    fhdr->filemode = (fhdr->filemode & ~FILE_MARKED) | FILE_DIGEST;
  }

  substitute_record(direct, fhdr, sizeof(*fhdr), ent);
  if(currmode & MODE_SELECT) {
    int now;
    char genbuf[100];
    setbdir(genbuf,currboard);
    now=getindex(genbuf,fhdr->filename);
    substitute_record(genbuf,fhdr,sizeof(*fhdr),now);
  }
  return PART_REDRAW;
}

static int b_help() {
  DL_func("bin/help.so:help", READING);
  return FULLUPDATE;
}

/* ----------------------------------------------------- */
/* ¬ÝªO¥\¯àªí                                            */
/* ----------------------------------------------------- */

static int b_vote() {
   DL_func("bin/b_vote.so:b_vote");
   return FULLUPDATE;
}

static int b_results() {
   DL_func("bin/b_results.so:b_results");
   return FULLUPDATE;
}

static int b_vote_maintain() {
   DL_func("bin/m_vote.so:b_vote_maintain");
   return FULLUPDATE;
}

struct one_key read_comms[] = {
  KEY_TAB, board_digest,
  Ctrl('E'), board_etc,
  'b', b_notes,
  'c', cite_post,
  'C', Cite_post,
  Ctrl('C'), Cite_posts,
  Ctrl('X'), a_cite_posts,
  Ctrl('A'), Tag_posts,
  Ctrl('H'), clear_report,   /* Dopin: ½Ð¨Ì±zªºª¬ªp§ó§ï¼öÁä (²M°£´£³ø°O¿ý) */
  Ctrl('T'), Tag_mails,
  Ctrl('W'), b_perm_edit,
  Ctrl('K'), b_reject_edit,
  Ctrl('N'), b_showfname,    /* Dopin: ¬d¸ß¤å³¹§¹¾ã¸ô®|»PÀÉ¦W */
  Ctrl('Y'), b_askperm_edit, /* Dopin: ¯µ±K¬ÝªO­ì«h«Å¥Ü */
  Ctrl('O'), tag_same,       /* Dopin: ¼Ð°O ¦P §@ªÌ/¼ÐÃD ¤§¤å³¹ */
  'r', read_post,
  'z', b_man,
  'D', del_range,
/* woju
  'S', sequential_read, */
  Ctrl('S'), save_mail,
  'E', edit_post,
  'X', reader_results,
  'f', online_msg,
  'Q', online_quest,
  'T', edit_title,
  'g', put_good_post,
  's', do_select,
  'R', b_results,
  'V', b_vote,
  'v', v_and_V,
  'M', b_vote_maintain,
  'W', b_notes_edit,
  't', post_tag,
  'w', b_water_edit,
  Ctrl('D'), post_del_tag,
  'x', cross_post,
  'h', b_help,
  'H', lock_post,
  'J', good_post,
  'O', ad_mon,
  'K', del_good,
  'y', reply_post,
  'd', del_post,
  'I', l_post_msg,
  'm', mark_post,
  'i', remark_post,
  Ctrl('Z'), edit_import,
  Ctrl('P'), do_post,

  '\0', NULL
};

ReadSelect() {
   int mode0 = currutmp->mode;
   int stat0 = currstat;
   char genbuf[200];

   currstat = XMODE;
   if(do_select(0, 0, genbuf) == NEWDIRECT) Read();
   currutmp->mode = mode0;
   currstat = stat0;
}

void log_board(char *mode, time_t usetime) {
   time_t      now;
   FILE        *fp;
   char        buf[256];

   now = time(0);
   sprintf(buf, "%s USE %-20.20s Stay: %5ld (%s)\n",
   Ctime( &now )+4, mode, usetime ,cuser.userid);

   if((fp = fopen("adm/board.read", "a")) != NULL) {
      fputs( buf, fp );
      fclose( fp );
   }
}

int Read() {
  int mode0 = currutmp->mode;
  int stat0 = currstat;
  time_t usetime;
  char buf[48];
  boardheader *bh = getbcache(currboard);

  setutmpmode(READING);
  set_board();

  if(board_visit_time < board_note_time) {
    setbfile(buf, currboard, fn_notes);
    more(buf, YEA);
  }

  setbdir(buf, currboard);
  curredit &= ~EDIT_MAIL;
  usetime = time(0);

  if(bh) {
     i_read(READING, buf, readtitle, readdoent, read_comms, bh);

     log_board(currboard, time(0) - usetime);
     brc_update();
  }

  currutmp->mode = mode0;
  currstat = stat0;
  return 0;
}

int Select() {
  char genbuf[200];
  setutmpmode(SELECT);
  do_select(0, NULL, genbuf);
  now_num = 9999;
  return 0;
}

int Post() {
  do_post();
  return 0;
}
