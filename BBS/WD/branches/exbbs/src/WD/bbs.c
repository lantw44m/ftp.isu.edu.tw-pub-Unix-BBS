/* admin.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : bulletin boards' routines                    */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/
#include "bbs.h"

#define MAXPATHLEN  1024
extern int mail_reply ();
extern char currdirect[64];
extern int TagNum;
extern struct BCACHE *brdshm;

extern void Read();

time_t board_note_time;
time_t board_visit_time;

static char *brd_title;
char real_name[20];
int local_article;
char currmaildir[32];
#define UPDATE_USEREC   (currmode |= MODE_DIRTY)

extern long wordsnum;    /* ­pºâ¦r¼Æ */

int
header_replace(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
      FILE *fp1, *fp2;
      char srcfile[64], tmpfile[64], buf[256];

      setdirpath (srcfile, direct, fhdr->filename);
      
      strcpy(tmpfile,"tmp/");
      strcat(tmpfile,fhdr->filename);

      f_cp(srcfile, tmpfile, O_TRUNC);

      fp1 = fopen(tmpfile, "r");
      fp2 = fopen(srcfile, "w");
      
      fgets(buf,sizeof(buf),fp1);      fputs(buf,fp2);

      fgets(buf,sizeof(buf),fp1);
      strcpy(buf,"¼ÐÃD: ");
      strcat(buf,fhdr->title);
      strcat(buf,"\n");                fputs(buf,fp2);
      
      while(fgets(buf,sizeof(buf),fp1))
         fputs(buf,fp2);
      fclose(fp1);
      fclose(fp2);
      f_rm(tmpfile);
   return 1;
}

void
security_check(fhdr, string1, n1, n2, n3, n4)
  fileheader *fhdr;
  char *string1;
  int n1;
  int n2;
  int n3;
  int n4;
{
   FILE * fp;
   time_t now = time (0);
   fileheader mhdr;
   char fpath[256], fpath1[STRLEN], poster[]="[¨t²Î³ø§i]";

   setbpath (fpath, "BBSpost");
   stampfile (fpath, &mhdr);
   strcpy (mhdr.owner, poster);
   strcat (mhdr.title, string1);
   strcat (mhdr.title, " ÀË¬d ");
   strcat (mhdr.title, cuser.userid);

   setbdir (fpath1, "BBSpost");

   if (rec_add (fpath1, &mhdr, sizeof (mhdr)) == -1)
   {
     outs (err_uid);
     return;
   }
   
   if ((fp = fopen (fpath, "w")) != NULL)
   {
      fprintf (fp, "§@ªÌ: %s\n", poster);
      fprintf (fp, "¼ÐÃD: %s\n®É¶¡: %s\n", mhdr.title, ctime(&now));

      fprintf (fp, " action : %s\n", string1);
      fprintf( fp, " userid : %s (%s)\n", cuser.userid, cuser.username);
      fprintf( fp, " currboard : %s\n", currboard);
      fprintf( fp, " filename : %s\n", fhdr->filename);
      fprintf( fp, " title : %s\n", fhdr->title);
      
      if(strcmp(string1,"POST")==0)
      {
        fprintf( fp, " wordsnum : %d\n", n1);
        fprintf( fp, " spendtime : %d\n", n2);
        fprintf( fp, " money : %d\n", n3);
        fprintf( fp, " exp : %d\n", n4);
      }
      fclose (fp);
    }
    
  return;
}

void
log_board (board, usetime)
  char *board;
  time_t usetime;
{
  time_t now;
  boardheader bh;
  int bid = getbnum (board);

  now = time (0);
  rec_get (fn_board, &bh, sizeof (bh), bid);
  if (usetime >= 10)
  {
    ++bh.totalvisit;
    bh.totaltime += usetime;
    strcpy (bh.lastvisit, cuser.userid);
    bh.lastime = now;
  }
  substitute_record (fn_board, &bh, sizeof (bh), bid);
}

void
log_board2( mode, usetime )
char *mode;
time_t usetime;
{
    time_t      now;
    FILE        *fp;
    char        buf[ 256 ];

    now = time(0);
    sprintf( buf, "USE %-20.20s Stay: %5ld (%s) %s",
       mode, usetime ,cuser.userid, ctime(&now));
    if( (fp = fopen(BBSHOME"/usboard", "a" )) != NULL && usetime > 5) {
        fputs( buf, fp );
        fclose( fp );
    }
}

void
log_board3( mode, str, num )
char *mode;
char *str;
int  *num;
{
    time_t      now;
    FILE        *fp;
    char        buf[ 256 ];

    now = time(0);
    sprintf( buf, "%3s %-20.20s with: %5ld (%s) %s",
      mode, str, num ,cuser.userid,ctime(&now));
    if( (fp = fopen(BBSHOME"/usboard", "a" )) != NULL ) {
        fputs( buf, fp );
        fclose( fp );
    }
}

static int
g_board_names (fhdr)
     boardheader *fhdr;
{
  AddNameList (fhdr->brdname);
  return 0;
}


void
make_blist ()
{
  CreateNameList ();
  apply_boards (g_board_names);
}


static int
g_bm_names(bh)
  boardheader *bh;
{
  char buf[IDLEN * 3 + 3];
  char* uid;

  strcpy(buf, bh->BM);
  uid = strtok(buf, "/");       /* shakalaca.990721: §ä²Ä¤@­ÓªO¥D */
  while (uid)
  {
    if (!InNameList(uid) && searchuser(uid))
      AddNameList(uid);
    uid = strtok(0, "/");       /* shakalaca.990721: «ü¦V¤U¤@­Ó */
  }
  return 0;
}

/* shakalaca.990721: ©Ò¦³ BM ¦W³æ */
void
make_bmlist()
{
  CreateNameList();
  apply_boards(g_bm_names);
}


void
set_board ()
{
  boardheader *bp;
  boardheader *getbcache ();

  bp = getbcache (currboard);
  board_note_time = bp->bupdate;
  brd_title = bp->BM;
  if (brd_title[0] <= ' ')
    brd_title = "¼x¨D¤¤";
  sprintf (currBM, "ªO¥D¡G%s", brd_title);
  brd_title = (bp->bvote == 1 ? "¥»¬ÝªO¶i¦æ§ë²¼¤¤" : bp->title + 7);

  currmode = (currmode & MODE_DIRTY) | MODE_STARTED;
  
  if (HAS_PERM (PERM_ANNOUNCE) ||
     (HAS_PERM (PERM_BM) && is_BM (currBM + 6) ) ||
     (HAS_PERM (PERM_SYSOP)) )
    {
      currmode |= (MODE_BOARD | MODE_POST);
    }
  else if (haspostperm (currboard))
    currmode |= MODE_POST;
}


static void
readtitle ()
{
  showtitle (currBM, brd_title);
  outs ("\
[¡ö]Â÷¶} [¡÷]¾\\Åª [^P]µoªí¤å³¹ [b]³Æ§Ñ¿ý [d]§R°£ [z]ºëµØ°Ï [TAB]¤åºK [^Z]¨D§U\n\
" COLOR1 "[1m  ½s¸¹   ");
  if (currmode & MODE_TINLIKE)
    outs ("½g ¼Æ");
  else
    outs ("¤é ´Á");
  outs ("  §@  ªÌ       ¤å  ³¹  ¼Ð  ÃD                                    [m");
}


void
doent (num, ent, row, bar)
     int num;
     fileheader *ent;
     int row;
     int bar;
{
  int type,tag;
  char *mark, *title, color;
  static char *colors[5] =
  {"[1;36m", "[1;33m",
   "[1;32m", "[1;33m","[1;37m"};
  char buf[128], outmsg[256];
  
  if (currstat != RMAIL)
  {
    type = brc_unread (ent->filename) ? '+' : ' ';

    tag = ' ';
/*  if (TagNum && !Tagger(atoi(ent->filename + 2) , num, TAG_NIN))
    tag = '*';
*/
//    if(ent->filemode & FILE_DELETED)
//       return -1;
       
    if ((currmode & MODE_BOARD) && (ent->filemode & FILE_DIGEST))
      type = (type == ' ') ? '*' : '#';
    if (ent->filemode & FILE_MARKED)
      type = (type == ' ') ? 'm' : 'M';

    /* itoc.001203: ¥[±Kªº¤å³¹«e­±¥X²{ X ¦r¼Ë */
    if (ent->filemode & FILE_REFUSE)
      type = (type == ' ') ? 'x' : 'X';
      
    if (ent->filemode & FILE_DELETED)
      type = (type == ' ') ? '!' : '-';
  }
  else
  {
    type = "+ Mm"[ent->filemode];

    if (ent->filemode & FILE_REPLYOK)
      type = (ent->filemode & FILE_MARKED) ? 'R' : 'r';
  }
  /* shakalaca.990421: §ï¤F¤@¤U, ¦]¬° mail ¤¤¬Ý°_¨Ó¤£¦n¬Ý */

  if (ent->filemode & FILE_TAGED)
    type = 't';

  title = str_ttl (mark = ent->title);
  if (title == mark)
    {
      color = '6';
      mark = "¡¼";
    }
  else
    {
      color = '3';
      mark = "R:";
    }

  if (title[47])
    strcpy (title + 44, " ¡K");  /* §â¦h¾lªº string ¬å±¼ */
    
  if(bar==1)
    strcpy(outmsg, "¡´[1;33;44m");
  else
    strcpy(outmsg, "  [1;37;40m");

  sprintf (buf, "%4d %c%c%s%-7s[37m%-13.12s",
     num, type,tag,
     colors[(unsigned int) (ent->date[4] + ent->date[5]) % 5],
     ent->date,
     ent->owner);
     
  strcat(outmsg, buf);
     
  if (strncmp (currtitle, title, TTLEN))
    color = '7';

  sprintf (buf, "[1;3%cm%s %s[m", color, mark, title);
  strcat(outmsg, buf);

  outmsgline(outmsg, row);  
//  clrtoeol();
  move(b_lines,0);
  return;
}


int
cmpbnames (bname, brec)
     char *bname;
     boardheader *brec;
{
  return (!ci_strncmp (bname, brec->brdname, sizeof (brec->brdname)));
}


int
cmpfilename (fhdr)
     fileheader *fhdr;
{
  return (!strcmp (fhdr->filename, currfile));
}

#if 0
untag(fhdr, dirpath)
  fileheader *fhdr;
  char *dirpath;
{
  int now;

  fhdr->filemode ^= FILE_TAGED;
  now = getindex (dirpath, fhdr->filename, sizeof (fileheader));
  substitute_record (dirpath, fhdr, sizeof (*fhdr), now);
}
#endif
/*
   woju
 */
int
cmpfmode (fhdr)
     fileheader *fhdr;
{
  return (fhdr->filemode & currfmode);
}


int
cmpfowner (fhdr)
     fileheader *fhdr;
{
  return !strcasecmp (fhdr->owner, currowner);
}

int
check_file(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  char buf[64];
  setbfile(buf,currboard,fhdr->filename);
  pressanykey("%d bytes, %s", dashs(buf), fhdr->date);
  return RC_FULL;
}

int
do_select (ent, fhdr, direct)
     int ent;
     fileheader *fhdr;
     char *direct;
{
  char bname[20], bpath[60];
//  char buf[80];
  struct stat st;
//  boardheader *bhdr, *getbcache();

  move (b_lines-1, 0);
  clrtoeol ();

  make_blist ();
  namecomplete (msg_bid, bname);
    
//  wordcomplete (MSG_SELECT_BOARD, bname, 1);

  setbpath (bpath, bname);
  if ((*bname == '\0') || (stat (bpath, &st) == -1))
    {
//      move (2, 0);
//      clrtoeol ();
      pressanykey (err_bid);
      return RC_FULL;
    }
/*
   board_visit_time = 0x7fffffff;
*/

  brc_initial (bname);

  return RC_NEWDIR;
  
  set_board ();                  // ??
  setbdir (direct, currboard);   // ??

//  move (1, 0);
//  clrtoeol ();
  return RC_NEWDIR;
}
/* ----------------------------------------------------- */
/* §ï¨} innbbsd Âà¥X«H¥ó¡B³s½u¬å«H¤§³B²zµ{§Ç             */
/* ----------------------------------------------------- */

outgo_post(fh, board)
  fileheader *fh;
  char *board;
{
  char buf[256];
  if(strcmp(fh->owner,cuser.userid))
    sprintf (buf, "%s\t%s\t%s\t%s\t%s", board,
      fh->filename, fh->owner, "Âà¿ý", fh->title);
  else
    sprintf (buf, "%s\t%s\t%s\t%s\t%s", board,
      fh->filename, fh->owner, cuser.username, fh->title);

  f_cat ("innd/out.http", buf);
  f_cat ("innd/out.bntp", buf);
}

/* ----------------------------------------------------- */
/* µoªí¡B¦^À³¡B½s¿è¡BÂà¿ý¤å³¹                            */
/* ----------------------------------------------------- */


void
do_reply_title (row, title)
  int row;
  char *title;
{
  char genbuf[256], ans[3];

  if (ci_strncmp (title, str_reply, 4))
    sprintf (save_title, "Re: %s", title);
  else
    strcpy (save_title, title);
  save_title[TTLEN - 1] = '\0';
  sprintf (genbuf, "±Ä¥Î­ì¼ÐÃD¡m%.60s¡n¶Ü¡H [Y] ", save_title);
  getdata (row, 0, genbuf, ans, 2, LCECHO, 0);
  if (*ans == 'n')
    getdata (++row, 0, "¼ÐÃD¡G", save_title, TTLEN, DOECHO, 0);
}


static void
do_reply (fhdr)
     fileheader *fhdr;
{
  char genbuf[200];

// Ptt ¬ÝªO³s¸p¨t²Î
  if(!strcmp(currboard,VOTEBOARD))
    DL_func("SO/votebrd.so:va_do_voteboardreply",fhdr);
  else
  {
    getdata (b_lines - 1, 0,
      "¡¶ ¦^À³¦Ü (F)¬ÝªO (M)§@ªÌ«H½c (B)¤GªÌ¬Ò¬O (Q)¨ú®ø¡H[F] ",
      genbuf, 3, LCECHO, 0);
    switch (genbuf[0])
    {
      case 'm':
        mail_reply (0, fhdr, 0);
      case 'q':
        *quote_file = 0;
        break;

      case 'b':
        curredit = EDIT_BOTH;
      default:
        strcpy (currtitle, fhdr->title);
        strcpy (quote_user, fhdr->owner);
        //quote_file[79] = fhdr->filemode;
        currfmode = fhdr->filemode;
        do_post ();
    }
  }
  *quote_file = 0;
}


/*
   woju
 */
brdperm (char *brdname, char *userid)
{
  boardheader *bp;
  boardheader *getbcache ();
  int uid = searchuser (userid);

  bp = getbcache (currboard);
  if (uid && bp)
    {
      int level = bp->level;

//      int brdattr = bp->brdattr;
      char *ptr = bp->BM;

//      char buf[64], manager[IDLEN + 1];
      userec xuser;

      rec_get (fn_passwd, &xuser, sizeof (xuser), uid);
      if ((level & BRD_POSTMASK) || ((level) ? xuser.userlevel & (level) : 1))
  return 1;

//      if ((brdattr & BRD_ANONYMOUS) && (currutmp->invisible & 1))
//  return 1;

      if (ptr[0] <= ' ')
  return 0;

      if (userid_is_BM (userid, ptr))
  return 1;

      if ((level & 0xffff) != PERM_SYSOP)
  return 0;
/*
      strncpy (manager, ptr, IDLEN + 1);
      if (ptr = strchr (manager, '/'))
          *ptr = 0;
      sethomefile (buf, manager, fn_overrides);
      return (belong (buf, userid));
*/
    }
  return 0;
}

#ifdef POSTNOTIFY
do_postnotify (char *fpath)
{
  fileheader mhdr;
  char title[128], buf1[80], buf[80];
  FILE *fp;
  char genbuf[256];

  setuserfile (genbuf, "postnotify.ok");
  if (fp = fopen (genbuf, "r"))
    {
      char last_fname[80];
      boardheader *bp;
      boardheader *getbcache ();

      strcpy (last_fname, fpath);
      bp = getbcache (currboard);
      while (fgets (buf, 80, fp))
  if (brdperm (currboard, buf))
    {
      sethomefile(buf1, buf, "postlist");
      if (!belong_list(buf1, buf))
      {
        del_distinct(genbuf, cuser.userid);
        fclose(fp);
        return;
      }

      sprintf(buf1, "home/%s/mailbox", cuser.userid);
      stampfile (buf1, &mhdr);
      strcpy (mhdr.owner, cuser.userid);
      strcpy (mhdr.title, "[·s]");
      strncat (mhdr.title, save_title, TTLEN - 4);
      mhdr.savemode = mhdr.filemode = 0;
      sprintf(title, "home/%s/mailbox/.DIR", cuser.userid);
      rec_add (title, &mhdr, sizeof (mhdr));
      
//      mail2user(cuser, save_title, );
      unlink (buf1);
      f_cp (last_fname, buf1, O_TRUNC);
      strcpy (last_fname, buf1);
    }
      fclose (fp);
    }
}
#endif

do_unanonymous_post (char *fpath)
{
  fileheader mhdr;
  char title[128];
  char genbuf[200];

  setbpath (genbuf, "UnAnonymous");
  if (dashd (genbuf))
    {
      stampfile (genbuf, &mhdr);
      unlink (genbuf);
      f_ln (fpath, genbuf);
      strcpy (mhdr.owner, cuser.userid);
      strcpy (mhdr.title, save_title);
      mhdr.savemode = mhdr.filemode = 0;
      setbdir (title, "UnAnonymous");
      rec_add (title, &mhdr, sizeof (mhdr));
    }
}

do_all_post (char *fpath)
{
  fileheader mhdr;
  char title[128];
  char genbuf[200];

  setbpath (genbuf, "All_Post");
  if (dashd (genbuf))
  {
    stampfile (genbuf, &mhdr);
    unlink (genbuf);
    f_ln (fpath, genbuf);
    strcpy (mhdr.owner, cuser.userid);
    strcpy (mhdr.title, save_title);
    mhdr.savemode = mhdr.filemode = 0;
    setbdir (title, "All_Post");
    rec_add (title, &mhdr, sizeof (mhdr));
  }
  sethomefile(genbuf, cuser.userid ,"allpost");
  if(!dashd (genbuf))
    mkdir(genbuf,0755);
  stampfile (genbuf, &mhdr);
  unlink (genbuf);
  f_ln (fpath, genbuf);
  strcpy (mhdr.owner, cuser.userid);
  strcpy (mhdr.title, save_title);
  mhdr.savemode = mhdr.filemode = 0;
  sprintf (title,BBSHOME"/home/%s/allpost/.DIR", cuser.userid);
  rec_add (title, &mhdr, sizeof (mhdr));
}

/* Ptt test */
getindex (fpath, fname, size)
     char *fpath;
     char *fname;
     int size;
{
  int fd, now = 0;
  fileheader fhdr;

  if ((fd = open (fpath, O_RDONLY, 0)) != -1)
    {
      while ((read (fd, &fhdr, size) == size))
  {
    now++;
    if (!strcmp (fhdr.filename, fname))
      {
        close (fd);
        return now;
      }
  }
      close (fd);
    }
  return 0;
}

int
do_post()
{
  fileheader postfile;
  char fpath[128], buf[128];
  int aborted;
  char genbuf[256]; // *owner;
//  boardheader *bp;
  boardheader *getbcache ();
  time_t spendtime;
  int num = 0, i;
  char *postprefix[] =
  {"[¤½§i] ", "[³qª¾] ", "[¤å¥ó] ", "[°ÝÃD] ", "[³Ð§@] ", "[·s»D] ", "[¶¢²á] ",
   "[´ú¸Õ] ", "[¥Ó½Ð] ",NULL};

//  bp = getbcache (currboard);

// ¸TÁn¦W³æ½T»{
  setbfile (buf, currboard, FN_LIST);
  if (!HAS_PERM (PERM_SYSOP) && brd_bad_guy (buf, cuser.userid))
  {
    pressanykey ("¹ï¤£°_¡A±z³Qª©¥D¦C¤J¸TÁn¦W³æ¤F...");
    return RC_FULL;
  }

  if ( !HAS_PERM (PERM_POST) || HAS_PERM (PERM_FORBIDPOST) || !(currmode & MODE_POST) || !brdperm (currboard, cuser.userid))
  {
    pressanykey ("¹ï¤£°_¡A±z¥Ø«eµLªk¦b¦¹µoªí¤å³¹¡I");
    return RC_FULL;
  }

// Ptt ¬ÝªO³s¸p¨t²Î
  if(!strcmp(currboard,VOTEBOARD))
  {
    DL_func("SO/votebrd.so:do_voteboard");
    return RC_FULL;
  }

/*
  setbfile (buf, currboard, FN_LIST);
  if (dashf (buf) && belong ("etc/have_postcheck", currboard))
  {
    if (!HAS_PERM (PERM_SYSOP) && !belong_list (buf, cuser.userid))
    {
      pressanykey ("¹ï¤£°_,¦¹ªO¥u­ã¬ÝªO¦n¤Í¤~¯àµoªí¤å³¹,½Ð¦VªO¥D¥Ó½Ð");
      return RC_FULL;
    }
  }
*/
  clear();
  more ("etc/post.note", NA);
  prints ("µoªí¤å³¹©ó¡i %s ¡j¬ÝªO\n", currboard);

  if (quote_file[0])
    do_reply_title (20, currtitle);
  else
  {
    memset (save_title, 0, TTLEN);
    while (postprefix[num] != NULL)
    {
      prints ("%d.%s", num + 1, postprefix[num]);
      num++;
    }
    getdata (20, 0, "½Ð¿ï¾Ü¤å³¹Ãþ§O©Î¦Û¦æ¿é¤JÃþ§O(©Î«öEnter¸õ¹L)¡G", genbuf, 9, DOECHO, 0);
    i = atoi (genbuf);
    if (i > 0 && i <= num)  /* ¿é¤J¼Æ¦r¿ï¶µ */
      strncpy (save_title, postprefix[i - 1], strlen (postprefix[i - 1]));
    else if (strlen (genbuf) >= 3)  /* ¦Û¦æ¿é¤J */
      strncpy (save_title, genbuf, strlen (genbuf));
    else      /* ªÅ¥Õ¸õ¹L */
      save_title[0] = '\0';

    getdata (21, 0, "¼ÐÃD¡G", save_title, TTLEN, DOECHO, save_title);
    strip_ansi (save_title, save_title, ONLY_COLOR);
  }
  if (save_title[0] == '\0')
    return RC_FULL;

  curredit &= ~EDIT_MAIL;
  curredit &= ~EDIT_ITEM;
  setutmpmode (POSTING);

  /* ¥¼¨ã³Æ Internet Åv­­ªÌ¡A¥u¯à¦b¯¸¤ºµoªí¤å³¹ */

  if (HAS_PERM (PERM_INTERNET))
    local_article = 0;
  else
    local_article = 1;

  buf[0] = 0;

  spendtime = time (0);
  aborted = vedit (buf, 1);
  spendtime = time (0) - spendtime;
  if (aborted == -1)
  {
    unlink (buf);
    pressanykey (NULL);
    return RC_FULL;
  }

  /* build filename */

  setbpath (fpath, currboard);
  stampfile (fpath, &postfile);
  f_mv (buf, fpath);
  strcpy (postfile.owner, cuser.userid);

  /* set owner to Anonymous , for Anonymous board */
/*
#ifdef HAVE_ANONYMOUS
  if (currbrdattr & BRD_ANONYMOUS && strcmp (real_name, "r"))
  {
    strcat (real_name, ".");
    owner = real_name;
  }
  else
  {
#endif

    owner = cuser.userid;

#ifdef HAVE_ANONYMOUS
  }
#endif
*/

  strcpy (postfile.owner, cuser.userid);
  strcpy (postfile.title, save_title);
  postfile.savemode = postfile.filemode=0;
  
  if (local_article == 1)    /* local save */
    postfile.filemode |= FILE_LOCAL;

  if(wordsnum>5000)
    postfile.filemode |= FILE_PROBLEM;

  if(currfmode & FILE_REFUSE)
    postfile.filemode |= FILE_REFUSE;
  currfmode=0;
  
  setbdir (buf, currboard);
  if (rec_add (buf, &postfile, sizeof (postfile)) != -1)
  {
    if (currmode & MODE_SELECT)
    rec_add (currdirect, &postfile, sizeof (postfile));

    if (local_article != 1 && (currbrdattr & BRD_OUTGOING))
      outgo_post (&postfile, currboard);
      
    brc_addlist (postfile.filename);

    if (!(currbrdattr & BRD_NOCOUNT))
    {
      int money = wordsnum*5;
      int exp = (wordsnum <= spendtime ? (wordsnum) : (spendtime));
      if (spendtime>=30) money+=3000;
      if (spendtime>=60) money+=6000;
      if (money>1000000) money=999999;
      if (exp > 1000000) exp = 999999;
        if (rpguser.race == 1) exp *= rpguser.level;

        clear ();
        move (7, 0);
        prints ("\
              [1;36m¡i[37m­p ºâ ½Z ¹S[36m¡j\n
              [37m ³o¬O±zªº[33m²Ä %d ½g[37m¤å³¹¡A\n
              [36m¡i¶O®É¡j[33m %d [37m¤À[33m % d [37m¬í¡A
              [36m¡i¦r¼Æ¡j[33m %d [37m¦r¡A
              [36m¡i½Z¹S¡j[33m %d [37m»È¨â¡A
              [36m¡i¸gÅç­È¡j[33m %d [37m ÂI¡C",
       ++cuser.numposts, spendtime / 60, spendtime % 60, wordsnum, money, exp);
       cuser.exp+=exp;
       cuser.silvermoney+=money%10000;
       cuser.goldmoney+=money/10000;
//       UPDATE_USEREC;
       substitute_record (fn_passwd, &cuser, sizeof (userec), usernum);

        pressanykey ("¤å³¹µoªí§¹²¦ :)");
        
        if(postfile.filemode & FILE_PROBLEM)
           security_check(postfile, "POST", wordsnum, spendtime, money, exp);
    }
    else
      pressanykey ("¥»¬ÝªO¤å³¹¤£¦C¤J¬ö¿ý¡A·q½Ð¥]²[¡C");

    log_board3("POS", currboard, cuser.numposts);

  /* ¦^À³¨ì­ì§@ªÌ«H½c */
    if (curredit & EDIT_BOTH)
    {
      char *str, *msg = "¦^À³¦Ü§@ªÌ«H½c";

      if (str = strchr (quote_user, '.'))
      {
        if (bbs_sendmail (fpath, save_title, str + 1) < 0)
          msg = "§@ªÌµLªk¦¬«H";
      }
      else
      {
//        sethomepath (genbuf, quote_user);
        sprintf(genbuf, "home/%s/mailbox", cuser.userid);
        stampfile (genbuf, &postfile);
        unlink (genbuf);
        f_cp (fpath, genbuf, O_TRUNC);

        strcpy (postfile.owner, cuser.userid);
        strcpy (postfile.title, save_title);
//        postfile.savemode = 'B';  /* both-reply flag */
//        sethomedir (genbuf, quote_user);
        sprintf(genbuf, "home/%s/mailbox/.DIR", cuser.userid);
        if (rec_add (genbuf, &postfile, sizeof (postfile)) == -1)
          msg = err_uid;
      }
      outs (msg);
      curredit ^= EDIT_BOTH;
    }
    do_all_post(fpath); // ¬ö¿ý©Ò¦³¯¸¤ºªº¶K¤å
// wildcat : do_all_post ¨ú¥N
//    if (currbrdattr & BRD_ANONYMOUS)  /*  ¤Ï°Î¦W  */
//      do_unanonymous_post (fpath);

// #ifdef POSTNOTIFY    /* ·s¤å³¹³qª¾ */
//    if (!(currbrdattr & BRD_ANONYMOUS) && !(currbrdattr & BRD_HIDE))
//      do_postnotify (fpath);
// #endif
  }
  return RC_FULL;
}

/*
static int
reply_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  if (!(currmode & MODE_POST))
    return RC_NONE;

  setdirpath (quote_file, direct, fhdr->filename);
  if(!strcmp(currboard,VOTEBOARD))
    DL_func("SO/votebrd.so:va_do_voteboardreply",fhdr);
  else
    do_reply (fhdr);
  *quote_file = 0;
  return RC_FULL;
}
*/

int
edit_post(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
//  char fpath[80], fpath0[80];
  extern bad_user (char *name);
  char genbuf[200];
//  fileheader postfile;

  if (!strcmp(currboard,"Security")) return RC_NONE;

  if (currstat == RMAIL)
  {
    setdirpath (genbuf, direct, fhdr->filename);
    vedit (genbuf, 2);
    return RC_FULL;
  }

  /* itoc.001203: ¥[±Kªº¤å³¹¤£¯à edit */
  if (
      (fhdr->filemode & FILE_REFUSE)
      && ! ( HAS_PERM(PERM_BM) && is_BM(currBM + 6) )
      && (strcmp(fhdr->owner, cuser.userid))
     )
     return RC_NONE;

  if (!(HAS_PERM (PERM_BM) && is_BM (currBM + 6) )
      && strcmp (fhdr->owner, cuser.userid)
      || !strcmp (cuser.userid, "guest")
      || bad_user (cuser.userid))
    return RC_NONE;

  setdirpath (genbuf, direct, fhdr->filename);
  local_article = fhdr->filemode & FILE_LOCAL;
  strcpy (save_title, fhdr->title);

  if (vedit (genbuf, 0) != -1)
  {
//    setbpath (fpath, currboard);
//    stampfile (fpath, &postfile);
//    unlink (fpath);
//    setbfile (fpath0, currboard, fhdr->filename);
//    f_mv (fpath0, fpath);
//    strcpy (fhdr->filename, postfile.filename);
//    strcpy (fhdr->title, save_title);
//    brc_addlist (postfile.filename);
    fhdr->savemode |= FILE_EDIT_AGAIN;
    substitute_record (direct, fhdr, sizeof (*fhdr), ent);

// #ifdef POSTNOTIFY
//    if (currbrdattr & BRD_ANONYMOUS)
//      do_postnotify (fpath);
// #endif
  }
  return RC_FULL;
}


static int
cross_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char xboard[20], fname[80], xfpath[80], xtitle[80], inputbuf[10];
  fileheader xfile;
  FILE * xptr;
  int author = 0;
  char genbuf[200];

  /* itoc.001203: ¥[±Kªº¤å³¹¤£¯àÂà¿ý */
  if (fhdr->filemode & FILE_REFUSE &&
      !(HAS_PERM (PERM_BM) && is_BM (currBM + 6))
     )
     return RC_NONE;

  make_blist ();
  move (2, 0);
  clrtoeol ();
  move (3, 0);
  clrtoeol ();
  move (1, 0);
  namecomplete ("Âà¿ý¥»¤å³¹©ó¬ÝªO¡G", xboard);
  if (*xboard == '\0' || !haspostperm (xboard))
    return RC_FULL;

  ent = 1;
  if (HAS_PERM (PERM_SYSOP) || !strcmp (fhdr->owner, cuser.userid))
  {
    getdata (2, 0, "(1)­ì¤åÂà¸ü (2)ÂÂÂà¿ý®æ¦¡¡H[2] ",
    genbuf, 2, DOECHO, 0);
    if (genbuf[0] == '1')
    {
      ent = 0;
      getdata (2, 0, "«O¯d­ì§@ªÌ¦WºÙ¶Ü¡H [Y] ", inputbuf, 2, DOECHO, 0);
      if (inputbuf[0] != 'n' && inputbuf[0] != 'N') author = 1;
    }
  }

  if (ent)
    sprintf (xtitle, "[Âà¿ý] %.66s", fhdr->title);
  else
    strcpy (xtitle, fhdr->title);

//  sprintf (genbuf, "±Ä¥Î­ì¼ÐÃD¡m%.60s¡n¶Ü¡H [Y] ", xtitle);
//  getdata (2, 0, genbuf, genbuf2, 4, LCECHO, 0);
//  if (*genbuf2 == 'n')
//  {
//    if (getdata (2, 0, "¼ÐÃD¡G", genbuf, TTLEN, DOECHO, xtitle))
//      strcpy (xtitle, genbuf);
//  }

  getdata (2, 0, "(S)¦sÀÉ (L)¯¸¤º (Q)¨ú®ø¡H [S] ", genbuf, 2, LCECHO, "S");
  if (genbuf[0] == 'l' || genbuf[0] == 's')
  {
    int currmode0 = currmode;

    currmode = 0;
    setbpath (xfpath, xboard);
    stampfile (xfpath, &xfile);
    if (author)
      strcpy (xfile.owner, fhdr->owner);
    else
      strcpy (xfile.owner, cuser.userid);
    strcpy (xfile.title, xtitle);
    xfile.savemode = xfile.filemode = 0;
    
    if (genbuf[0] == 'l')
      xfile.filemode |= FILE_LOCAL;

    setbfile (fname, currboard, fhdr->filename);
    if (ent)
    {
      xptr = fopen (xfpath, "w");
      strcpy (save_title, xfile.title);
      strcpy (xfpath, currboard);
      strcpy (currboard, xboard);
      write_header (xptr);
      strcpy (currboard, xfpath);

      fprintf (xptr, "¡° [¥»¤åÂà¿ý¦Û %s ¬ÝªO]\n\n", currboard);

      b_suckinfile (xptr, fname);
      addsignature (xptr);
      fclose (xptr);
    }
    else
    {
      unlink (xfpath);
      f_cp (fname, xfpath, O_TRUNC);
    }

    setbdir (fname, xboard);
    rec_add (fname, (char *) &xfile, sizeof (xfile));
    if (!xfile.filemode)
      outgo_post (&xfile, xboard);
    cuser.numposts++;
    UPDATE_USEREC;
    pressanykey ("¤å³¹Âà¿ý§¹¦¨");
    currmode = currmode0;
  }
  return RC_FULL;
}

static int
read_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char genbuf[256];
  int more_result;
  time_t startime = time (0);

  /* itoc.001203: ¥[±Kªº¤å³¹¥u¦³­ì§@ªÌ¥H¤ÎªO¥D¯à¾\Åª */

  setbfile(genbuf,currboard,fhdr->filename);
  strcat(genbuf,".X\0");
  
  if (fhdr->filemode & FILE_REFUSE && 
      !(HAS_PERM (PERM_BM) && is_BM (currBM + 6)) &&
      !(HAS_PERM (PERM_SECRETARY)) &&
      strcmp(cuser.userid, fhdr->owner) &&
      !belong(genbuf,cuser.userid)
     )
    return RC_FULL;

  if (fhdr->owner[0] == '-')
    return RC_NONE;

  setdirpath (genbuf, direct, fhdr->filename);

//  if(dashd(genbuf))
//    read_dir(genbuf,fhdr->title);

  if ((more_result = more (genbuf, YEA)) == -1)
    return RC_NONE;

  brc_addlist (fhdr->filename);
  strncpy (currtitle, str_ttl(fhdr->title), TTLEN);
  strncpy (currowner, str_ttl(fhdr->owner), STRLEN);
  if (time (0) - startime > 3)
  {
    int exp = time(0) - startime;
    inexp (rpguser.race == 2 ? exp * rpguser.level /2 : exp/5);
  }

  switch (more_result)
  {
    case 1:
      return RS_PREV;
    case 2:
      return RELATE_PREV;
    case 3:
      return RS_NEXT;
    case 4:
      return RELATE_NEXT;
    case 5:
      return RELATE_FIRST;
    case 6:
      return RC_FULL;
    case 7:
    case 8:
      if (currmode & MODE_POST)
      {
        strcpy (quote_file, genbuf);
        do_reply (fhdr);
        *quote_file = 0;
      }
      return RC_FULL;
    case 9:
      return 'A';
    case 10:
      return 'a';
    case 11:
      return '/';
    case 12:
      return '?';
  }
  return RC_FULL;
}



/* ----------------------------------------------------- */
/* ±Ä¶°ºëµØ°Ï                                            */
/* ----------------------------------------------------- */

man()
{
  char buf[64]; // buf1[64], xboard[20], fpath[256];
//  boardheader * bp;
  boardheader * getbcache ();

  if (currstat == RMAIL)
  {
/*  
    move (2, 0); clrtoeol ();
    move (3, 0); clrtoeol ();
    move (1, 0);
    make_blist ();
    namecomplete ("¿é¤J¬Ýª©¦WºÙ (ª½±µ Enter ¶i¤J¨p¤H«H¥ó§¨)¡G", buf);
    if (*buf)
      strcpy (xboard, buf);
    else
      strcpy (xboard, "0");
    if (xboard && (bp = getbcache (xboard)))
    {
      setapath (fpath, xboard);
      setutmpmode (ANNOUNCE);
      a_menu (xboard, fpath, HAS_PERM (PERM_ALLBOARD) ? 2 : is_BM (bp->BM) ? 1 : 0);
    }
    else if(HAS_PERM(PERM_MAILLIMIT) || HAS_PERM(PERM_BM))
    {
*/
      int mode0 = currutmp->mode;
      int stat0 = currstat;
      sethomeman (buf, cuser.userid);
//      sprintf(buf,"home/%s/mailbox",cuser.userid);
//      sprintf (buf1, "%s ªº«H¥ó§¨", cuser.userid);
//      setutmpmode (ANNOUNCE);
      setutmpmode(RMAIL);
      a_menu ("¨p¤H«H¥ó§¨", buf, 2);
      currutmp->mode = mode0;
      currstat = stat0;
      return RC_FULL;
//    }
  }
  else
  {
    setapath (buf, currboard);
    setutmpmode (ANNOUNCE);
    a_menu (currboard, buf, HAS_PERM (PERM_ALLBOARD) ? 2 :
      currmode & MODE_BOARD ? 1 : 0);
  }
  return RC_FULL;
}

int
add_group(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
   char fpath[MAXPATHLEN]; // buf[MAXPATHLEN];
   fileheader item;

   strcpy(fpath, direct);
//   strtok(fpath,".");
   setdirpath(fpath, direct, fhdr->filename);
//   fpath[strlen(fpath)-1]='\0';
   
   stampdir (fpath, &item);
   strcpy (item.title, "¡» ");
   strcpy (item.owner, cuser.userid);
   item.filemode = MAIL_ALREADY_READ;

  if (!getdata (b_lines - 1, 1, "[·s¼W¥Ø¿ý] ½Ð¿é¤J¼ÐÃD¡G",
                          &item.title[3], 55, DOECHO, 0))
  {
     rmdir (fpath);
     return 0;
  }
  
  if (rec_add (direct, &item, sizeof(item)) == -1)
  {
    rmdir (fpath);
    return 0;
  }
  
  return RC_FULL;
}

int
cite(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char fpath[256];
  char title[TTLEN + 1];
  
  strcpy (title, "¡º ");
  strncpy (title + 3, fhdr->title, TTLEN - 3);
  title[TTLEN] = '\0';

  if (fhdr->filemode & FILE_REFUSE &&
      !(HAS_PERM (PERM_BM) && is_BM (currBM + 6)) &&
      strcmp(cuser.userid, fhdr->owner))
    return RC_FULL;

  if (currstat == RMAIL)
  {
//  setuserfile (fpath, fhdr->filename);
//  add_tag (ent, fhdr, direct);
    sprintf(fpath, "home/%s/mailbox/%s", cuser.userid, fhdr->filename);
    a_copyitem (fpath, title, cuser.userid);
    man ();
  }  
  else
  {
    setbfile (fpath, currboard, fhdr->filename);
    a_copyitem (fpath, title, cuser.userid);
    man ();
  }
  
  return RC_FULL;
}

Cite_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char fpath[256];
  char title[TTLEN + 1];
  
  setbfile (fpath, currboard, fhdr->filename);
  
  /* itoc.001203: ¥[±Kªº¤å³¹¥u¦³­ì§@ªÌ¥H¤ÎªO¥D¯à¾\Åª */
  
  if (fhdr->filemode & FILE_REFUSE &&
      !(HAS_PERM (PERM_BM) && is_BM (currBM + 6)) &&
      strcmp(cuser.userid, fhdr->owner))
      return RC_FULL;                          

  strcpy (title, "¡º ");
  strncpy (title + 3, fhdr->title, TTLEN - 3);
  
  // sprintf (title, "%s%.72s",(currutmp->pager > 1) ? "" : "¡º ", fhdr->title);
  title[TTLEN] = '\0';
  a_copyitem (fpath, title, cuser.userid);
  load_paste ();
  if (*paste_path)
    a_menu (paste_title, paste_path, paste_level, ANNOUNCE);
  return RC_FULL;
}

Cite_posts (int ent, fileheader * fhdr, char *direct)
{
  char fpath[256];

  setbfile (fpath, currboard, fhdr->filename);
  
  /* itoc.001203: ¥[±Kªº¤å³¹¥u¦³­ì§@ªÌ¥H¤ÎªO¥D¯à¾\Åª */
  
  if (fhdr->filemode & FILE_REFUSE &&
      !(HAS_PERM (PERM_BM) && is_BM (currBM + 6)) &&
      strcmp(cuser.userid, fhdr->owner))
      return RC_FULL;
                      
//  add_tag ();
  load_paste ();
  if (*paste_path && paste_level && dashf (fpath))
  {
    fileheader fh;
    char newpath[MAXPATHLEN];

    strcpy (newpath, paste_path);
    stampfile (newpath, &fh);
/* shakalaca.990714: ±NÀÉ®×¿W¥ß
    unlink (newpath); */
    f_cp (fpath, newpath, O_TRUNC);
    strcpy (fh.owner, cuser.userid);
    sprintf (fh.title, "%s%.72s",
    (currutmp->pager > 1) ? "¡º " : "¡º ", fhdr->title);
    strcpy (strrchr (newpath, '/') + 1, ".DIR");
    rec_add (newpath, &fh, sizeof (fh));
    return POS_NEXT;
  }
  bell ();
  return RC_NONE;
}

int
edit_title (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char genbuf[128];

  if (!strcmp(currboard,"Security")) return RC_NONE;
  
  if (HAS_PERM (PERM_SYSOP) || !strcmp (fhdr->owner, cuser.userid) || (currmode & MODE_BOARD))
  {
    fileheader tmpfhdr = *fhdr;
    int dirty = 0;

    if (getdata (b_lines - 1, 0, "¼ÐÃD¡G", genbuf, TTLEN, DOECHO, tmpfhdr.title))
    {

      strip_ansi (genbuf, genbuf, 0);

      strcpy (tmpfhdr.title, genbuf);
      strcpy (save_title, genbuf);
      dirty++;
    }

    if(HAS_PERM (PERM_SYSOP))
    {
      if (getdata (b_lines - 1, 0, "§@ªÌ¡G", genbuf, IDLEN + 2, DOECHO, tmpfhdr.owner))
      {
        strcpy (tmpfhdr.owner, genbuf);
        dirty++;
      }

      if (getdata (b_lines - 1, 0, "¤é´Á¡G", genbuf, 6, DOECHO, tmpfhdr.date))
      {
        sprintf (tmpfhdr.date, "%+5s", genbuf);
        dirty++;
      }
    }

    if (getdata (b_lines - 1, 0, "½T©w(Y/N)?[n] ", genbuf, 3, DOECHO, 0) &&
      (*genbuf == 'y' || *genbuf == 'Y' )&& dirty)
    {
      *fhdr = tmpfhdr;
      substitute_record (direct, fhdr, sizeof (*fhdr), ent);
      
      header_replace(ent, fhdr, direct);
      
      if (currmode & MODE_SELECT)
      {
        int now;
        setbdir (genbuf, currboard);
        now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
        substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
      }
    }
    return RC_FULL;
  }
  return RC_NONE;
}

int
add_tag(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  int now;
  char genbuf[100];

  if (!strcmp(currboard,"Security")) return RC_NONE;

  if (currstat == RMAIL)
  {
    fhdr->filemode ^= FILE_TAGED;
//    sethomedir (genbuf, cuser.userid);
    sprintf(genbuf, "home/%s/mailbox/.DIR", cuser.userid);

    if (currmode & SELECT)
    {
      now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
      substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
      sprintf (genbuf, "home/%s/SR.%s", cuser.userid, cuser.userid);
      substitute_record (genbuf, fhdr, sizeof (*fhdr), ent);
    }
    else
    {
      substitute_record (genbuf, fhdr, sizeof (*fhdr), ent);
    }
    return POS_NEXT;
  }
//  if(currstat == READING) return RC_NONE;
  if (currmode & MODE_BOARD)
  {
    fhdr->filemode ^= FILE_TAGED;
    if (currmode & MODE_SELECT)
    {
      setbdir (genbuf, currboard);
      now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
      substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
      sprintf (genbuf, "boards/%s/SR.%s", currboard, cuser.userid);
      substitute_record (genbuf, fhdr, sizeof (*fhdr), ent);
      return POS_NEXT;
    }
    substitute_record (direct, fhdr, sizeof (*fhdr), ent);
    return POS_NEXT;
  }
  return RC_NONE;
}


int
del_tag (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char ans[3], tmpfname[64];
//  int number;
  FILE *fp, *fptmp;
  fileheader fhdrtmp;
  int payback=0;

  if (currstat == RMAIL)
  {
    getdata (1, 0, "½T©w§R°£¼Ð°O«H¥ó(Y/N)? [N]", ans, 2, LCECHO, 0);
    if (ans[0] == 'y')
    {
      currfmode = FILE_TAGED;
      if (delete_files (direct, cmpfmode))
        return RC_CHDIR;
    }
    return RC_FULL;
  }
  if ((currstat != READING) || (currmode & MODE_BOARD))
  {
    if (!strcmp(currboard,"Security") && !HAS_PERM(PERM_SECRETARY))
       return RC_NONE;
       
    getdata (1, 0, "½T©w§R°£¼Ð°O¤å³¹(Y/N)? [N]", ans, 3, LCECHO, 0);
    if (ans[0] == 'y')
    {
      currfmode = FILE_TAGED;
      if (currmode & MODE_SELECT)
      {
        unlink (direct);
        currmode ^= MODE_SELECT;
        setbdir (direct, currboard);
        delete_files (direct, cmpfmode);
      }
      else
      {
        fp = fopen(direct, "r");
        strcpy(tmpfname, direct);
        strcat(tmpfname, "_tmp");
        fptmp = fopen(tmpfname, "w");
        while (fread(&fhdrtmp, sizeof(fhdrtmp), 1, fp))

          if(fhdrtmp.filemode & currfmode)
          {
            if (strcmp(fhdrtmp.owner, cuser.userid)==0)
              payback++;
            cancel_post(&fhdrtmp,0,2);
//            setdirpath(genbuf, direct, fhdrtmp.filename);
//            unlink(genbuf);
          }
          else
            fwrite(&fhdrtmp, sizeof(fhdrtmp), 1, fptmp);
            
        fclose(fp);
        fclose(fptmp);
        unlink(direct);
        f_mv(tmpfname, direct);              
      }
      
  if(currstat!=RMAIL && currstat !=ANNOUNCE)
  {
    UPDATE_USEREC;
    if (cuser.goldmoney < payback)
       demoney((payback-cuser.goldmoney)*10000);
    degold(payback);
    deexp(payback*1024);
        
    if(payback>0)
      pressanykey("§R¨ì¦Û¤vªº¤å³¹¦@¦³ %d ½g",payback);
  }
      /*
      
       if (number=delete_files(direct, cmpfmode)){
        log_board3("TAG", currboard, number);
                 return RC_CHDIR;
        }
      */
    }
    return RC_FULL;
  }
  return RC_NONE;
}

int
gem_tag (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char genbuf[3];

  load_paste(); //Åª¤J paste_file ¨Ó©w¦ì
  if(!*paste_path)
  {
    pressanykey("©|¥¼©w¦ì,½Ð¶i¤JºëµØ°Ï¤¤§A·Q¦¬¿ýªº¥Ø¿ý«ö [P]");
    return RC_FOOT;
  }

  if (currstat == RMAIL)
  {
    getdata (1, 0, "½T©w¦¬¿ý¼Ð°O«H¥ó(Y/N)? [Y]", genbuf, 3, LCECHO, 0);
    if (genbuf[0] != 'n')
    {
      currfmode = FILE_TAGED;
      if (gem_files (direct, cmpfmode))
        return RC_CHDIR;
    }
    return RC_FULL;
  }
  if ((currstat != READING) || (currmode & MODE_BOARD))
  {
    getdata (1, 0, "½T©w¦¬¿ý¼Ð°O¤å³¹(Y/N)? [N]", genbuf, 3, LCECHO, 0);
    if (genbuf[0] == 'y')
    {
      currfmode = FILE_TAGED;
      if (currmode & MODE_SELECT)
      {
        unlink (direct);
        currmode ^= MODE_SELECT;
        setbdir (direct, currboard);
        gem_files (direct, cmpfmode);
      }
      else
        gem_files(direct, cmpfmode);
      return RC_CHDIR;
    }
    return RC_FULL;
  }
  return RC_NONE;
}


int
mark(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
/*
  if (currstat == RMAIL)
  {
    fhdr->filemode ^= FILE_MARKED;
    fhdr->filemode &= ~FILE_TAGED;
    sprintf(currmaildir, "home/%s/mailbox/.DIR", cuser.userid);
//    if(dashs(currmaildir)==-1)
//       sethomedir (currmaildir, cuser.userid);
    substitute_record (currmaildir, fhdr, sizeof (*fhdr), ent);
    return (RC_DRAW);
  }
*/
  if (currstat == READING && !(currmode & MODE_BOARD)
      || (currmode & MODE_DELETED))
    return RC_NONE;

  if (currmode & MODE_BOARD && currstat == READING)
  {
    if (fhdr->filemode & FILE_MARKED)
      deumoney (fhdr->owner, 200);
    else
      inumoney (fhdr->owner, 200);
  }

  fhdr->filemode ^= FILE_MARKED;

  if (currmode & MODE_SELECT)
  {
    int now;
    char genbuf[100];

    if (currstat != READING)
//      sethomedir(genbuf, cuser.userid);
      sprintf(genbuf, "home/%s/mailbox/.DIR", cuser.userid);
    else
      setbdir (genbuf, currboard);
    now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
    substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
  }
  else
    substitute_record (direct, fhdr, sizeof (*fhdr), ent);

  return RC_DRAW;
}

/* itoc.001203: ¤å³¹¥[±K */
int
refusemark(ent, fhdr, direct)
  int ent;
   fileheader *fhdr;
   char *direct;
{
  int now;
  char genbuf[128];
  
  if (currstat != READING || 
      (
       !(HAS_PERM (PERM_BM) && is_BM (currBM + 6))
       && strcmp(fhdr->owner, cuser.userid)
      )
     )
    return RC_NONE;

  fhdr->filemode ^= FILE_REFUSE;

  if (currmode & MODE_SELECT)
  {
    setbdir(genbuf, currboard);
    now = getindex(genbuf, fhdr->filename, sizeof(fileheader));
    substitute_record(genbuf, fhdr, sizeof(*fhdr), now);
  }
  else
    substitute_record(direct, fhdr, sizeof(*fhdr), ent);
  return RC_DRAW;
}

int
refusemark_for(ent, fhdr, direct)
  int ent;
   fileheader *fhdr;
   char *direct;      
{
   char ans[3], buf[256];
   if (currstat != READING
       ||
       (
        !(HAS_PERM (PERM_BM) && is_BM (currBM + 6))
        && strcmp(fhdr->owner, cuser.userid)
        || !(fhdr->filemode & FILE_REFUSE)
       )
   )
   return RC_NONE;
      
//   setutmpmode(REJECTMAIL);
   showtitle("½s¯S§O¦W³æ", currboard);
   setbfile(buf,currboard,fhdr->filename);
   strcat(buf,".X\0");
   
   getdata(1,0,"½s¿è(E) §R°£(D) ¨S¨Æ[Q] ",ans,2,LCECHO,0);
   
    if(ans[0] == 'e' || ans[0] == 'E')
    {
       vedit(buf,NA);
    }
    if(ans[0] == 'd' || ans[0] == 'D')
    {
       unlink(buf);
    }
    return RC_FULL;
}

int
deletedmark(ent, fhdr, direct)
  int ent;
   fileheader *fhdr;
   char *direct;
{
  char genbuf[128];
  fileheader deleted;
  int now;

  if ((currmode & MODE_DELETED) || !(currmode & MODE_BOARD)
      || (fhdr->filemode & FILE_DIGEST)
      || (fhdr->filemode & FILE_MARKED))
    return RC_NONE;
    
  memcpy (&deleted, fhdr, sizeof (fileheader));
//  deleted.filename[0] = 'D';

  if (fhdr->filemode & FILE_DELETED)
  {
    fhdr->filemode = (fhdr->filemode & ~FILE_DELETED);
    sprintf(genbuf,"boards/%s/%s",currboard,fn_deleted);
    now = getindex (genbuf, deleted.filename);
    strcpy (currfile, deleted.filename);
    delete_file (genbuf, sizeof (fileheader), now, cmpfilename);
  }
  else
  {
    deleted.savemode = deleted.filemode = 0;
    sprintf(genbuf,"boards/%s/%s",currboard,fn_deleted);
    rec_add (genbuf, &deleted, sizeof (fileheader));
    fhdr->filemode = (fhdr->filemode & ~FILE_MARKED) | FILE_DELETED;
  }

  substitute_record (direct, fhdr, sizeof (*fhdr), ent);
  if (currmode & MODE_SELECT)
  {
    setbdir (genbuf, currboard);
    now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
    substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
  }
  return RC_DRAW;
}

int
del_range (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char num1[8], num2[8];
  int inum1, inum2, i, now;
  
  int fd, size = sizeof (fileheader);
  char genbuf[128];
  fileheader rsfh;    

  if (!strcmp(currboard,"Security") && !HAS_PERM(PERM_SECRETARY)) 
    return RC_NONE;
  if ((currstat != READING) || (currmode & MODE_BOARD))
  {
    getdata (1, 0, "[³]©w§R°£½d³ò] °_ÂI¡G", num1, 5, DOECHO, 0);
    inum1 = atoi (num1);
    if (inum1 <= 0)
    {
      outmsg ("°_ÂI¦³»~");
      refresh ();
      sleep (1);
      return RC_FOOT;
    }
    getdata (1, 28, "²×ÂI¡G", num2, 5, DOECHO, 0);
    inum2 = atoi (num2);
    if (inum2 < inum1)
    {
      outmsg ("²×ÂI¦³»~");
      refresh ();
      sleep (1);
      return RC_FULL;
    }
    getdata (1, 48, msg_sure_ny, num1, 3, LCECHO, 0);
    if (*num1 == 'y')
    {
      outmsg ("³B²z¤¤,½Ðµy«á...");
      refresh ();
      if (currmode & MODE_SELECT)
      {
        i = inum1;
        if (currstat == RMAIL)
//          sethomedir (genbuf, cuser.userid);
          sprintf(genbuf, "home/%s/mailbox/.DIR", cuser.userid);
        else
          setbdir (genbuf, currboard);

        if ((fd = (open (direct, O_RDONLY, 0))) != -1)
        {
          if (lseek (fd, (off_t) (size * (inum1 - 1)), SEEK_SET) != -1)
          {
            while (read (fd, &rsfh, size) == size)
            {
              if (i > inum2)
                break;
                
              now = getindex (genbuf, rsfh.filename, size);
              strcpy (currfile, rsfh.filename);
              delete_file (genbuf, sizeof (fileheader), now, cmpfilename);
              i++;
            }
          }
          close (fd);
        }
      }

    // del_range by robert 2001.06.12

   if(currstat!=RMAIL && currstat != ANNOUNCE)
   {    
    i = inum1;
    setbdir (genbuf, currboard);
    if ((fd = (open (direct, O_RDONLY, 0))) != -1)
    {
      if (lseek (fd, (off_t) (size * (inum1 - 1)), SEEK_SET) != -1)
      {
         while (read (fd, &rsfh, size) == size)
         {
           if ((i > inum2)
               || (rsfh.filemode & FILE_MARKED)
               || (rsfh.filemode & FILE_DIGEST)
              )
             break;

             strcpy (currfile, rsfh.filename);
                cancel_post(&rsfh,0,2);
             i++;
         }
      }
      close (fd);
    }
   }
   
   i=delete_range (direct, inum1, inum2);
//   if(i>0)
//     pressanykey("§R¨ì¦Û¤vªº¤å³¹¦³ %d ½g",i);
   return RC_NEWDIR;
  }
  return RC_FULL;
 }
 return RC_NONE;
}

#if 0
static void
lazy_delete (fhdr)
  fileheader * fhdr;
{
  char buf[20];

  sprintf (buf, "-%s", fhdr->owner);
  strncpy (fhdr->owner, buf, IDLEN + 1);
  strcpy (fhdr->title, "<< article deleted >>");
}

int
del_one (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  if ((currstat != READING) || (currmode & MODE_BOARD))
  {
    strcpy (currfile, fhdr->filename);

    if (!update_file (direct, sizeof (fileheader), ent, cmpfilename, lazy_delete))
    {
      cancelpost (fhdr, YEA);
      lazy_delete (fhdr);
      return RC_DRAW;
    }
  }
  return RC_NONE;
}
#endif

static int
del_post (ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  int not_owned, money, net = 1;
  char genbuf[128];

  if (!strcmp(currboard,"Security") && !HAS_PERM(PERM_SECRETARY))
    return RC_NONE;

  if ((fhdr->filemode & FILE_MARKED)
    || (fhdr->filemode & FILE_DIGEST) || (fhdr->owner[0] == '-'))
    return RC_NONE;

  /* itoc.001203: ¥[±Kªº¤å³¹¤£¯à§R°£ */
//  if (fhdr->filemode & (FILE_MARKED | FILE_DIGEST | FILE_REFUSE))
//     return RC_NONE;

  not_owned = strcmp (fhdr->owner, cuser.userid);
  
  // wildcat : ¯¸ªø¥i¥H³s½u¬å«H
//  if (HAS_PERM(PERM_SYSOP) && answer("¬O§_­n³s½u¬å«H(Y/N) [N]") == 'y')
//    net = 1;      

  if (!(currmode & MODE_BOARD) && not_owned || !strcmp (cuser.userid, "guest"))
    return RC_NONE;

  getdata (1, 0, msg_del_ny, genbuf, 2, LCECHO, 0);
  if (genbuf[0] == 'y')
  {
    strcpy (currfile, fhdr->filename);
    setbfile (genbuf, currboard, fhdr->filename);
    money = (int) dashs (genbuf);
    if (!delete_file (direct, sizeof (fileheader), ent, cmpfilename))
    {
      if (currmode & MODE_SELECT)
      {
        int now;
        setbdir (genbuf, currboard);
        now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
        delete_file (genbuf, sizeof (fileheader), now, cmpfilename);
      }
     
      cancel_post (fhdr, not_owned, net);
     
      if (!not_owned && !(currbrdattr & BRD_NOCOUNT))
      {
        move (b_lines - 1, 0);
        clrtoeol ();
        UPDATE_USEREC;
        deexp(money);
        money=money/10000;
        if (money < 1) money = 1;
        if (money > 99) money = 99;
        if(cuser.goldmoney < money)
          demoney((money-cuser.goldmoney)*10000);
        degold(money);
          
        pressanykey ("%s¡A±zªº¤å³¹´î¬° %d ½g¡A¤ä¥I²M¼ä¶O %d ª÷", msg_del_ok,
          cuser.numposts > 0 ? --cuser.numposts : cuser.numposts, money);
        substitute_record (fn_passwd, &cuser, sizeof (userec), usernum);
      }
      return RC_CHDIR;
    }
  }
  return RC_FULL;
}


save_mail (int ent, fileheader * fh, char *direct)
{
  fileheader mhdr;
  char fpath[MAXPATHLEN], genbuf[MAXPATHLEN] , *p;

  if (ent < 0)
    strcpy (fpath, direct);
  else
  {
    strcpy (genbuf, direct);
    if (p = strrchr (genbuf, '/'))
      * p = '\0';
    sprintf (fpath, "%s/%s", genbuf, fh->filename);
  }
  if (!dashf (fpath) || !HAS_PERM (PERM_BASIC))
  {
    bell ();
    return RC_NONE;
  }
  
  sprintf(currmaildir, "home/%s/mailbox/.DIR", cuser.userid);
  sprintf(genbuf, "home/%s/mailbox", cuser.userid);
//  if(dashs(currmaildir)==-1)
//  {
//     sethomedir (currmaildir, cuser.userid);
//     sethomepath (genbuf, cuser.userid);
//  }
  
  stampfile (genbuf, &mhdr);
  unlink (genbuf);
  f_cp (fpath, genbuf, O_TRUNC);
  if (HAS_PERM (PERM_SYSOP))
    strcpy (mhdr.owner, fh->owner);
  else
    strcpy (mhdr.owner, cuser.userid);
  strncpy (mhdr.title, fh->title + ((currstat == ANNOUNCE) ? 3 : 0), TTLEN);
  mhdr.filemode = MAIL_ALREADY_READ;  
	  
  if (rec_add (currmaildir, &mhdr, sizeof (mhdr)) == -1)
  {
    bell ();
    return RC_NONE;
  }
  return POS_NEXT;
}

/* ----------------------------------------------------- */
/* ¬ÝªO³Æ§Ñ¿ý¡B¤åºK¡BºëµØ°Ï                              */
/* ----------------------------------------------------- */

static int
board_edit ()
{
  boardheader bp;
  boardheader * getbcache ();
  int bid, mode = 0;
  time_t now = time(0);
  char genbuf[BTLEN], buf[512]; // ans;

  if (currmode & MODE_BOARD)
  {
    bid = getbnum (currboard);

    if (rec_get (fn_board, &bp, sizeof (boardheader), bid) == -1)
    {
      pressanykey (err_bid);
      return -1;
    }

    if (bp.brdattr & BRD_PERSONAL || HAS_PERM(PERM_SYSOP))
      mode = 1;
    
    sprintf(buf,
//    "¬ÝªO 1)¶R¤W­­ 2)§ï¤¤¤å¦WºÙ 3)¬ÝªO»¡©ú 4)¶iª©µe­± 5)¥i¨£¦W³æ 6)³]±K½X %s:",
    "¬ÝªO 1)§ï¤¤¤å¦WºÙ 2)¶iª©µe­± 3)¥i¨£¦W³æ 4)³]±K½X ");
//    mode ? "5)¶i¶¥" : "");

    getdata (1, 0, buf, genbuf, 2, DOECHO, 0);
    switch(genbuf[0])
    {
/*
      case '1':
        clrchyiuan(1, 15);
        move(3, 0);
        prints("¥Ø«e¬ÝªOªº¤å³¹¤W­­¬° %-5d ½g"
               "«O¯d®É¶¡¬° %-5d ¤Ñ\n",
               bp.maxpost,bp.maxtime);
               
        outs("¤@­Ó³æ¦ì¬°[1;32m¤@¦Ê½g¤å³¹[m©Î¬O[1;32m¤T¤Q¤Ñ[m ,"
             "¤@­Ó³æ¦ì»Ý [1;33m3000 ª÷¹ô[m");
         
    getdata(7, 0, "§A­n (1)¶R¤å³¹¤W­­ (2)¶R«O¦s®É¶¡", buf, 2, DOECHO, 0);
    if (buf[0] == '1' || buf[0] == '2')
    {
      int num = 0;

          while (num <= 0)
          {
            getdata(9, 0, "§A­n¶R´X­Ó³æ¦ì", genbuf, 3, DOECHO, 0);
            num = atoi(genbuf);
          }

          if (check_money(num * 3000, GOLD))
            break;

          if (buf[0] == '1')
          {
            if (bp.maxpost >= 99999)
            {
              pressanykey("¤å³¹¼Æ¤w¹F¤W­­");
              break;
            }
            else
            {
              bp.maxpost += num*100;
              sprintf(buf, "%-13.13s ÁÊ¶R¬ÝªO %s ¤å³¹¤W­­ %d ½g , %s",
                cuser.userid, bp.brdname,num*100, ctime(&now));
              f_cat(BBSHOME"/log/board_edit", buf);
              log_usies ("BuyPL", currboard);
              pressanykey("¬ÝªO¤å³¹¤W­­¼W¥[¬° %d ½g", bp.maxpost);
            }
          }
          else
          {
            if (bp.maxtime >= 9999)
            {
              pressanykey("«O¦s®É¶¡¤w¹F¤W­­");
              break;
            }
            else
            {
              bp.maxtime += num * 30;
              sprintf(buf,"%-13.13s ÁÊ¶R¬ÝªO %s ¤å³¹«O¯d®É¶¡ %d ¤Ñ , %s",
                cuser.userid,bp.brdname,num*30,ctime(&now));
              f_cat(BBSHOME"/log/board_edit",buf);
              log_usies ("BuyBT", currboard);
              pressanykey("¬ÝªO¤å³¹«O¯d®É¶¡¼W¥[¬° %d ¤Ñ",bp.maxtime);
            }
          }
          degold(num*3000);
        }
        break;
*/
      case '1':
        move (1, 0);
        clrtoeol ();
        getdata (1, 0, "½Ð¿é¤J¬ÝªO·s¤¤¤å±Ô­z:"
          ,genbuf, BTLEN - 16, DOECHO, bp.title + 7);
        if (!genbuf[0]) return 0;
        strip_ansi (genbuf, genbuf, 0);
        if(strcmp(bp.title+7,genbuf))
        {
          sprintf(buf,"%-13.13s §ó´«¬ÝªO %s ±Ô­z [%s] -> [%s] , %s",
            cuser.userid,bp.brdname,bp.title+7,genbuf,ctime(&now));
          f_cat(BBSHOME"/log/board_edit",buf);
          log_usies ("NameBoard", currboard);
          strcpy (bp.title + 7, genbuf);
        }
        break;

/*
      case '2':
        clrchyiuan(1, 5);
        move(1, 0);
        outs("¹ï¥»¬ÝªOªº´y­z (¦@¤T¦æ) :");
        getdata (2, 0, ":", bp.desc[0], 79, DOECHO, bp.desc[0]);
        getdata (3, 0, ":", bp.desc[1], 79, DOECHO, bp.desc[1]);
        getdata (4, 0, ":", bp.desc[2], 79, DOECHO, bp.desc[2]);
        sprintf(buf,"%-13.13s §ó´«¬ÝªO %s »¡©ú , %s",
          cuser.userid,bp.brdname,ctime(&now));
        f_cat(BBSHOME"/log/board_edit",buf);
        log_usies ("SetBoardDesc", currboard);
        break;
*/
      case '2':
        setbfile (buf, currboard, fn_notes);
        if (vedit (buf, NA) == -1)
        {
          getdata(3,0,"»Ý­n§R°£ÀÉ®×¶Ü¡H (y/N) ", genbuf, 2, LCECHO, "N");
          
          if(genbuf[0]=='y')
            f_rm(buf);
          pressanykey(NULL);
        }
//        else
//        {
//          int aborted;
//          getdata (3, 0, "½Ð³]©w¦³®Ä´Á­­(0 - 9999)¤Ñ¡H", buf, 5, DOECHO,"9999");
//          aborted = atol (buf);
//          bp.bupdate = aborted ? time (0) + aborted * 86400 : 0;
//        }
        break;

      case '3':
        setbfile(buf, currboard, FN_LIST);
        ListEdit(buf);
        return RC_FULL;

      case '4':
      {
        char genbuf[PASSLEN+1],buf[PASSLEN+1];

        move (1, 0);
        clrtoeol ();
        if(!HAS_PERM(PERM_ALLBOARD))
        {
          if(!getdata (1, 0, "½Ð¿é¤J­ì¥»ªº±K½X" ,genbuf, PASSLEN, PASS, 0)
             ||
             !chkpasswd(bp.passwd, genbuf))
             {
               pressanykey("±K½X¿ù»~");
               return -1;
             }
        }
        if (!getdata(1, 0, "½Ð³]©w·s±K½X¡G", genbuf, PASSLEN, PASS,0))
        {
          pressanykey("±K½X³]©w¨ú®ø, Ä~Äò¨Ï¥ÎÂÂ±K½X");
          return -1;
        }
        strncpy(buf, genbuf, PASSLEN);

        getdata(1, 0, "½ÐÀË¬d·s±K½X¡G", genbuf, PASSLEN, PASS,0);
        if (strncmp(genbuf, buf, PASSLEN))
        {
          pressanykey("·s±K½X½T»{¥¢±Ñ, µLªk³]©w·s±K½X");
          return -1;
        }
        buf[8] = '\0';
        strncpy(bp.passwd, genpasswd(buf), PASSLEN);
        log_usies ("SetBrdPass", currboard);
      }
      break;
/*
      case '5':
        if (!mode)
          break;
        else
        {
          getdata(1, 0, "±z­n 1)§ïÄÝ©Ê 2)§ïÃþ§O 3)¬Ý¤µ¤é°O¿ý:", genbuf, 2,DOECHO, 0);
          switch(genbuf[0])
          {
          case '1':
          {
            int oldattr=bp.brdattr;
            ans = answer("¬ÝªOª¬ºA§ó§ï¬° (o)¶}©ñ (p)¨p¤H (h)ÁôÂÃ?");
            if(ans == 'p')
            {
              bp.brdattr &= ~BRD_POSTMASK;
              bp.brdattr |= BRD_HIDE;
            }
            else if(ans == 'h')
            {
              bp.brdattr |= BRD_POSTMASK;
              bp.brdattr |= BRD_HIDE;
            }
            else
            {
              if(answer("¬O§_­nÅý guest ¬Ý¨ì§AªºªO? (Y/n)") == 'n')
                bp.brdattr &= ~BRD_POSTMASK;
              else
                bp.brdattr |= BRD_POSTMASK;
              bp.brdattr &= ~BRD_HIDE;
            }
            if(bp.brdattr != oldattr)
            {
              sprintf(buf,"%-13.13s §ó§ï¬ÝªO [%s] ¤§ÄÝ©Ê¬° %s , %s",
                cuser.userid,bp.brdname,
                ans == 'p' ? "¨p¤H" : ans == 'h' ? "ÁôÂÃ" :"¶}©ñ",ctime(&now));
              f_cat(BBSHOME"/log/board_edit",buf);
              log_usies("ATTR_Board",currboard);
            }
          }
          break;

          case '2':
            move (1, 0);
            clrtoeol ();
            getdata (1, 0, "½Ð¿é¤J¬ÝªO·sÃþ§O:",genbuf, 5, DOECHO, bp.title);
            if (!genbuf[0]) return 0;
            strip_ansi (genbuf, genbuf, 0);
            if(strncmp(bp.title,genbuf,4))
            {
              sprintf(buf,"%-13.13s §ó´«¬ÝªO %s Ãþ§O [%-4.4s] -> [%s] ,%s",
                cuser.userid,bp.brdname,bp.title,genbuf,ctime(&now));
              f_cat(BBSHOME"/log/board_edit",buf);
              log_usies("PREFIX",currboard);
              strncpy (bp.title , genbuf, 4);
            }
            break;
          case '3':
            sprintf(buf,"/usr/bin/grep \"USE %s\" %s/usboard >  %s/tmp/usboard.%s",
              currboard, BBSHOME, BBSHOME, currboard);
            system(buf);
            sprintf(buf,BBSHOME"/tmp/usboard.%s",currboard);
            more(buf, YEA);
            log_usies("BOARDLOG", currboard);

          default:
            break;
          }
          goto end;
        }
*/
      default:
        pressanykey("©ñ±ó­×§ï");
        break;
    }
    
// end:

    substitute_record (fn_board, &bp, sizeof (boardheader), bid);
    touch_boards ();
    return RC_FULL;
  }
  return 0;
}




static int
b_notes_edit ()
{
  if (currmode & MODE_BOARD)
  {
    char buf[64],ans[3] ;
    int aborted;

    setbfile (buf, currboard, fn_notes);
    aborted = vedit (buf, NA);
    if (aborted == -1)
    {
       getdata(2,0,"»Ý­n§R°£ÀÉ®×¶Ü¡H (y/N) ", ans, 2, LCECHO, "N");
       if(ans[0]=='y')
          f_rm(buf);
       pressanykey(NULL);
    }
//      pressanykey (msg_cancel);
//    else
//    {
//      boardheader fh;
//      int pos;
//      getdata (3, 0, "½Ð³]©w¦³®Ä´Á­­(0 - 9999)¤Ñ¡H", buf, 5, DOECHO, "9999");
//      aborted = atol (buf);
//      pos = rec_search (fn_board, &fh, sizeof (fh), cmpbnames, (int) currboard);
   /* SiE: ³o¸Ì»Prec_searchªº¶Ç¤J­È«¬ºA¤£¦P..À³¸Ó¬OÃz½Õªº­ì¦] */
//      fh.bupdate = aborted ? time (0) + aborted * 86400 : 0;
//      substitute_record (fn_board, &fh, sizeof (fh), pos);
//      touch_boards ();
//    }
    return RC_FULL;
  }
  return 0;
}

static int
b_list_edit()
{
  char buf[STRLEN];

  if (currmode & MODE_BOARD)
  {
    setbfile(buf, currboard, FN_LIST);
    ListEdit(buf);
    return RC_FULL;
  }
  return RC_NONE;
}

static int
bh_attr_edit()
{
  boardheader bh, *getbcache();
  int bid, oldattr, avg;
  time_t now=time(0);
  char genbuf[BTLEN], buf[512];
  
  if (currmode & MODE_BOARD)
  {
    bid = getbnum (currboard);
    if (rec_get (fn_board, &bh, sizeof (boardheader), bid) == -1)
    {
      pressanykey (err_bid);
      return -1;
    }

    move (1, 0);
    clrtobot ();
    prints ("¬ÝªO¦WºÙ¡G%-15s ªO¥D¦W³æ¡G%s\n¬ÝªO»¡©ú¡G%-49s ¤å³¹¤W­­¡G%d %s\n",
       bh.brdname, bh.BM, bh.title, bh.postotal,
       (bh.brdattr & BRD_OUTGOING && bh.brdattr & BRD_INGOING) ? "x2" : "");
    {
      int t = bh.totaltime;
      int day=t/86400, hour=(t/3600)%24, min=(t/60)%60, sec=t%60;
      avg = bh.totalvisit != 0 ? bh.totaltime / bh.totalvisit : 0;
      prints("¶}ªO®É¶¡¡G%s\n", Ctime(&bh.opentime));
      prints("«ô³X¤H¼Æ¡G%-15d«ô³XÁ`®É¡G%d ¤Ñ %d ¤p®É %d ¤À %d ¬í ¥­§¡°±¯d¡G%d¬í\n",
             bh.totalvisit, day, hour, min, sec, avg);
    }
    prints ("³Ì·s³X«È¡G%-15s³Ì·s®É¶¡¡G%s\n"
          ,bh.lastvisit, Ctime (&bh.lastime));
//    bperm_msg (&bh);
    prints ("%s³]­­\n¥izap:%s ¦C¤J²Î­p:%s "
            "Âà¥X:%s Âà¤J:%s ¸s²Õ:%s ¨p¤H:%s ÁôÂÃ:%s «H½c:%s Àu¨}:%s ­Ó¤H:%s",
            bh.level ? "¦³" : "¤£",
            bh.brdattr & BRD_NOZAP ? "x" : "o",
            bh.brdattr & BRD_NOCOUNT ? "x" : "o",
            bh.brdattr & BRD_OUTGOING ? "o" : "x",
            bh.brdattr & BRD_INGOING ? "o" : "x",
            bh.brdattr & BRD_GROUPBOARD ? "o" : "x",
            bh.brdattr & BRD_HIDE ? "o" : "x",
            (bh.brdattr & BRD_HIDE) && (bh.brdattr & BRD_POSTMASK) ? "o" : "x",
            bh.brdattr & BRD_BRDMAIL ? "o" : "x",
            bh.brdattr & BRD_GOOD ? "o" : "x",
            bh.brdattr & BRD_PERSONAL ? "o" : "x");
             
    getdata (9, 0, "¬O§_§ó§ï¬ÝªOª¬ºA? (y/N) ", genbuf, 3, LCECHO, "y");
    if(genbuf[0]=='y')
    {
      oldattr=bh.brdattr;
      getdata (10, 0, "½Ð¿é¤J¬ÝªO·s¤¤¤å±Ô­z:"
               ,genbuf, BTLEN - 13, DOECHO, bh.title + 7);

      if (genbuf[0])
      {
        strip_ansi (genbuf, genbuf, 0);
        if(strcmp(bh.title+7,genbuf))
        {
          sprintf(buf,"%s %-13.13s §ó´«¬ÝªO %s ±Ô­z [%s] -> [%s]",
          ctime(&now),cuser.userid,bh.brdname,bh.title+7,genbuf);
          f_cat(BBSHOME"/log/board_edit.log",buf);
          strcpy (bh.title + 7, genbuf);
        }
      }
      
      if(!(bh.brdattr & BRD_CLASS) && !(bh.brdattr & BRD_GROUPBOARD))
      {
      
         getdata (11, 0, "Âà«HÄÝ©Ê: 1.¯¸¤º 2.Âà¥X 3.Âà¤J 4.Âà¤J»PÂà¥X ",genbuf, 2, LCECHO, 0);
         if(genbuf[0])
         switch(genbuf[0])
         {
           case '1':
           bh.brdattr &= ~BRD_OUTGOING;
           bh.brdattr &= ~BRD_INGOING;
        strncpy(bh.title+5,"¡³",2);
        break;
           case '2':
           bh.brdattr |= BRD_OUTGOING;
           bh.brdattr &= ~BRD_INGOING;
        strncpy(bh.title+5,"¡ó",2);
        break;
           case '3':
           bh.brdattr |= BRD_INGOING;
           bh.brdattr &= ~BRD_OUTGOING;
        strncpy(bh.title+5,"¡·",2);
        break;
           case '4':
           bh.brdattr |= BRD_OUTGOING;
           bh.brdattr |= BRD_INGOING;
        strncpy(bh.title+5,"¡´",2);
        break;
         }
      }

      if(bh.brdattr & BRD_PERSONAL)
      {
        getdata(12,0,"¬ÝªOÄÝ©Ê: 1.¶}©ñ 2.¨p¤H 3.ÁôÂÃ? ",genbuf,2,LCECHO, 0);
        switch(genbuf[0])
        {
           case '1':
             bh.brdattr &= ~BRD_POSTMASK;
             bh.brdattr &= ~BRD_HIDE;
             break;
           case '2':
             bh.brdattr |= BRD_HIDE;
             bh.brdattr &= ~BRD_POSTMASK;
             break;
           case '3':
             bh.brdattr |= BRD_POSTMASK;
             bh.brdattr |= BRD_HIDE;
        }

        if(bh.brdattr != oldattr)
        {
          sprintf(buf,"%s %-13.13s §ó§ï¬ÝªO [%s] ¤§ÄÝ©Ê¬° %s .",
            ctime(&now),cuser.userid,bh.brdname,
            genbuf[0] == '1' ? "¨p¤H" : genbuf[0] == '2' ? "ÁôÂÃ" : "¶}©ñ");
          f_cat(BBSHOME"/log/board_edit.log",buf);
        }
      }
      
      getdata(13, 0, "¬ÝªO«H½c: 1.Ãö³¬ 2.¶}©ñ ", genbuf, 2, LCECHO, 0);
      switch(genbuf[0])
      {
         case '1':
           bh.brdattr &= ~BRD_BRDMAIL;
           break;
         case '2':
           bh.brdattr |= BRD_BRDMAIL;
           break;
      }
        
      /*
      getdata (1, 0, "½Ð¿é¤J¬ÝªO·sÃþ§O:",genbuf, 5, DOECHO, bp.title );
      
      if (!genbuf[0]) return 0;
      
      strip_ansi (genbuf, genbuf, 0);
      if(strncmp(bp.title,genbuf,4))
      {
        sprintf(buf,"%s %-13.13s §ó´«¬ÝªO %s Ãþ§O [%s] -> [%s]",
          ctime(&now),cuser.userid,bp.brdname,bp.title,genbuf);
        f_cat(BBSHOME"/log/board_edit.log",buf);
        strncpy (bp.title , genbuf, 4);
      }
      */
      
      substitute_record (fn_board, &bh, sizeof (boardheader), bid);
      touch_boards ();
      log_usies ("SetBoard", currboard);
      return RC_FULL;
    }
    return RC_FULL;
  }
  return 0;
}

static int
bh_title_edit ()
{
  boardheader bp, *getbcache();
  int bid;
  time_t now=time(0);
  char genbuf[BTLEN], buf[512];
  
  if (currmode & MODE_BOARD)
  {
    bid = getbnum (currboard);
    if (rec_get (fn_board, &bp, sizeof (boardheader), bid) == -1)
    {
      pressanykey (err_bid);
      return -1;
    }
    
    getdata (b_lines, 0, "½Ð¿é¤J¬ÝªO·s¤¤¤å±Ô­z:"
      ,genbuf, BTLEN - 13, DOECHO, bp.title + 7);
      
    if (!genbuf[0]) return 0;
    
    strip_ansi (genbuf, genbuf, 0);
    if(strcmp(bp.title+7,genbuf))

    {
      sprintf(buf,"%s %-13.13s §ó´«¬ÝªO %s ±Ô­z [%s] -> [%s]",
        ctime(&now),cuser.userid,bp.brdname,bp.title+7,genbuf);
      f_cat(BBSHOME"/log/board_edit.log",buf);
      strcpy (bp.title + 7, genbuf);
    }
    
    substitute_record (fn_board, &bp, sizeof (boardheader), bid);
    touch_boards ();
    log_usies ("SetBoard", currboard);
    return RC_FULL;
  }
  return 0;
}

static int
bh_passwd_edit ()
{
  boardheader bp;
  boardheader * getbcache ();
  int bid;
  if (currmode & MODE_BOARD)
  {
    char genbuf[PASSLEN+1],buf[PASSLEN+1];

    bid = getbnum (currboard);
    if (rec_get (fn_board, &bp, sizeof (boardheader), bid) == -1)
    {
      pressanykey (err_bid);
      return -1;
    }
    move (1, 0);
    clrtoeol ();
    if(!HAS_PERM(PERM_ALLBOARD))
    {
      if(!getdata (1, 0, "½Ð¿é¤J­ì¥»ªº±K½X" ,genbuf, PASSLEN, PASS, 0) ||
         !chkpasswd(bp.passwd, genbuf))
         {
           pressanykey("±K½X¿ù»~");
           return -1;
         }
    }
    if (!getdata(1, 0, "½Ð³]©w·s±K½X¡G", genbuf, PASSLEN, PASS,0))
    {
      pressanykey("±K½X³]©w¨ú®ø, Ä~Äò¨Ï¥ÎÂÂ±K½X");
      return -1;
    }
    strncpy(buf, genbuf, PASSLEN);

    getdata(1, 0, "½ÐÀË¬d·s±K½X¡G", genbuf, PASSLEN, PASS,0);
    if (strncmp(genbuf, buf, PASSLEN))
    {
      pressanykey("·s±K½X½T»{¥¢±Ñ, µLªk³]©w·s±K½X");
      return -1;
    }
    buf[PASSLEN] = '\0';
    strncpy(bp.passwd, genpasswd(buf), PASSLEN);

    substitute_record (fn_board, &bp, sizeof (boardheader), bid);
    touch_boards ();
    log_usies ("SetBrdPass", currboard);
    return RC_FULL;
  }
  return 0;
}

/* wildcat modify 981221 */
int
b_notes ()
{
  char buf[64];

  setbfile (buf, currboard, fn_notes);
  if (more (buf, YEA) == -1) // ¥i¥H¬Ý¦h­¶
  {
    clear ();
    pressanykey ("¥»¬ÝªO©|µL¡u³Æ§Ñ¿ý¡v¡C");
  }
  return RC_FULL;
}


int
board_select ()
{
  struct stat st;
  char fpath[128];
  char genbuf[128];
  currmode &= ~(MODE_SELECT | MODE_TINLIKE);

  sprintf (genbuf, "SR.%s", cuser.userid);
  setbfile (fpath, currboard, genbuf);
  unlink (fpath);

  /* shakalaca.000112: ¶W¹L 30min ¤~±N index §R°£, §@ cache ¥Î */
  setbfile (fpath, currboard, "SR.thread");
  if (stat(fpath, &st) == 0 && st.st_mtime < time(0) - 60 * 30)
    unlink (fpath);

  if (currstat == RMAIL)
//    sethomedir (currdirect, cuser.userid);
    sprintf(currdirect, "home/%s/mailbox/.DIR", cuser.userid);
  else
    setbdir (currdirect, currboard);

  return RC_NEWDIR;
}

int
board_digest()
{
  if (currmode & MODE_SELECT)
    board_select ();

  currmode ^= MODE_DIGEST;
  if (currmode & MODE_DIGEST)
    currmode &= ~MODE_POST;
  else if (haspostperm (currboard))
    currmode |= MODE_POST;

  setbdir (currdirect, currboard);
  return RC_NEWDIR;
}

int
board_deleted()
{
  if (currmode & MODE_SELECT)
    board_select ();

  currmode ^= MODE_DELETED;
  if (currmode & MODE_DELETED)
    currmode &= ~MODE_POST;
  else if (haspostperm (currboard))
    currmode |= MODE_POST;

  setbdir (currdirect, currboard);
  return RC_NEWDIR;
}

static int
good_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char genbuf[128], genbuf2[128];
  fileheader digest;
  int now;
  
  if ((currmode & MODE_DIGEST) || !(currmode & MODE_BOARD)
      || (currmode & MODE_DELETED))
    return RC_NONE;

  memcpy (&digest, fhdr, sizeof (fileheader));
  digest.filename[0] = 'G';

  if (fhdr->filemode & FILE_DIGEST)
  {
    // setbgdir (genbuf2, currboard);
    sprintf(genbuf2,"boards/%s/%s",currboard,fn_digest);
    now = getindex (genbuf2, digest.filename);
    strcpy (currfile, digest.filename);
    delete_file (genbuf2, sizeof (fileheader), now, cmpfilename);
    sprintf (genbuf2, BBSHOME "/boards/%s/%s", currboard, currfile);
    unlink (genbuf2);
    fhdr->filemode = (fhdr->filemode & ~FILE_DIGEST);
//    deumoney (fhdr->owner, 500);
  }
  else
  {
    sprintf (genbuf, "boards/%s/%s", currboard, fhdr->filename);
    sprintf (genbuf2, "boards/%s/%s", currboard, digest.filename);
    
    if (!dashf (genbuf2))
    {
      digest.savemode = digest.filemode = 0;
      f_cp (genbuf, genbuf2, O_TRUNC);
      sprintf (genbuf2, "boards/%s/%s", currboard, fn_digest);
      rec_add (genbuf2, &digest, sizeof (digest));
    }
    fhdr->filemode = (fhdr->filemode & ~FILE_MARKED) | FILE_DIGEST;
//    inumoney (fhdr->owner, 500);
  }
  substitute_record (direct, fhdr, sizeof (*fhdr), ent);
  if (currmode & MODE_SELECT)
  {
    setbdir (genbuf, currboard);
    now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
    substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
  }
  return RC_DRAW;
}

/* ----------------------------------------------------- */
/* ¬ÝªO¥\¯àªí                                            */
/* ----------------------------------------------------- */
//extern int b_vote ();
//extern int b_results ();
// extern int b_vote_maintain ();

struct one_key read_comms[] =
{
  KEY_TAB, board_digest,
  'b', b_notes,
//  'B', bh_title_edit,
  'B', board_edit,
  'c', cite,
  'C', gem_tag,
  'd', del_post,
  'D', del_range,
  'E', edit_post,
  'g', good_post,
//  'h', deletedmark,
//  'H', board_deleted,
  'i', bh_passwd_edit,
  'm', mark,
  'o', b_list_edit,
  'Q', bh_attr_edit,
  'r', read_post,
//  's', do_select,
  's', check_file,
  't', add_tag,
  'T', edit_title,
  'W', b_notes_edit,
  'x', man,
  'X', refusemark,
  'Y', refusemark_for,
  'z', man,  
  Ctrl ('C'), Cite_posts,
  Ctrl ('D'), del_tag,
  Ctrl ('P'), do_post,
  Ctrl ('S'), save_mail,
  Ctrl ('X'), cross_post,
  '\0', NULL
};

void
Read ()
{
  int mode0 = currutmp->mode;
  int stat0 = currstat;
  int bid;
  char buf[40];
  time_t startime = time (0);
  extern struct BCACHE * brdshm;

  resolve_boards ();
  setutmpmode (READING);
  
  set_board ();
    setbfile (buf, currboard, fn_notes);
    more (buf, YEA);
  bid = getbnum (currboard);
  currutmp->brc_id = bid;

/*
  if(Ben_Perm(&brdshm->bcache[bid]-1) != 1)
    {
      pressanykey(P_BOARD);
      return;
    }
*/    

  setbfile (buf, currboard, FN_LIST);
  if (!HAS_PERM(PERM_SYSOP) && !is_BM(currBM+6) &&
      currbrdattr & BRD_HIDE && !belong_list(buf, cuser.userid))
  {
    pressanykey ("¹ï¤£°_, ¦¹ªO¥u­ã¬ÝªO¦n¤Í¶i¤J, ½Ð¦VªO¥D¥Ó½Ð¤J¹Ò³\\¥i");
    return;
  }

  setbdir (buf, currboard);
  curredit &= ~EDIT_MAIL;
  i_read (READING, buf, readtitle, doent, read_comms, &(brdshm->total[bid - 1]));

  log_board (currboard, time (0) - startime);
  log_board2 (currboard, time (0) - startime);
  brc_update ();

  currutmp->brc_id = 0;
  currutmp->mode = mode0;
  currstat = stat0;
//  return;
}


void
ReadSelect ()
{
//  int mode0 = currutmp->mode;
//  int stat0 = currstat;
//  char genbuf[200];

//  currstat = XMODE;

//  do_select (0, 0, NULL);
    if (do_select(0, NULL, NULL) == RC_NEWDIR)
       Read ();

//  currutmp->mode = mode0;
//  currstat = stat0;

}


int
Select ()
{
  char genbuf[200];

  setutmpmode (SELECT);
  do_select (0, NULL, genbuf);
  return 0;
}


int
Post ()
{
  do_post ();
  return 0;
}

// shakalaca.010221: §R°£¤å³¹, §PÂ_¬O§_¬°Âà¥X¦Ó°e¥X cancel message.
// net ¨M©w¬O§_­n°e¥X message (by sysop), by_BM ¨M©w­n°e¤J jun or delete
// µù: ¹w³]ªº¦sÀÉ Local or Net ±o¦h¥[¤W¬ÝªOªº³]©w§PÂ_:)

int
cancel_post(fh, not_owned, net_del)
  fileheader *fh;
  int not_owned, net_del;
{
  char *brd;
  int fd;
  char fn[256], fpath[256], buf[256];
  fileheader backfile;

  //  shakalaca.010221: ¦pªG¬O BM ©ñ¤J deleted, §_«h©ñ¤J junk
  //  brd = not_owned ? "deleted" : "junk";
  
  brd = HAS_PERM(PERM_SYSOP) ? "junk" : 
        (net_del==2) ? "Temp" : "deleted";  
  
  // shakalaca.010221: ³]©w¬ÝªO¸ô®|
  setbfile (fn, brd, fh->filename);
  // shakalaca.010221: ³]©wÀÉ®×¦s¦b¸ô®|
  setbfile(fpath, currboard, fh->filename);
  // shakalaca.010221: §â­ì©lÀÉ®×²¾¨ì junk or deleted ¤U­±.
  f_mv (fpath, fn);

  strcat(fpath,".X\0");
  f_rm (fpath);
        
  // shakalaca.010221: ¦b fn ¤§¤U«Ø¥ß¤@­Ó timestamp ÀÉ®×, ¨Ã±N fn
  // §ï¬°¸ÓÀÉ®×ªº¸ô®| (¬Û¹ï©ó BBSHOME)
  //  stampfile (fn, &backfile);

  // shakalaca.010221: ±N touch ªºÀÉ®×§R°£, §_«h·|¦³ bug ?
  //  unlink(fn);

  // shakalaca.010221: ±N­ì©lÀÉ®×ªº§@ªÌ, ¼ÐÃD, date ¤Î filemode ¤@¨Ã¦s¤J
  // §@ªÌ: IDLEN + 2  ¼ÐÃD: TTLEN + 1  date: 6  filemode: 1

  memcpy (&backfile, fh, sizeof(fileheader));

  if(backfile.filemode & FILE_PROBLEM)
     security_check(fh,"DELETE",0,0,0,0);
  
  if(backfile.filemode & FILE_REFUSE)
     strcpy(backfile.owner, cuser.userid);

  log_board3("DEL", currboard, 1);

  // shakalaca.010221: ¦b¸Ó¥Ø¿ý¤U«Ø¥ß .DIR ¹ï¬M
  setbdir (fpath, brd);
  rec_add (fpath, &backfile, sizeof (fileheader));          

  setbfile(fpath, brd, fh->filename);
  if (!(fh->filemode & FILE_LOCAL) && (net_del==1)
      && ((fd = open(fpath, O_RDONLY)) >= 0))
  {
#define NICK_LEN    80

    char *ptr, *left, *right, nick[NICK_LEN];
//    FILE *fout;
    int ch;

    ch = read(fd, nick, NICK_LEN);
    close(fd);
    ptr = nick;
    ptr[ch - 1] = '\0';

    if (!strncmp(ptr, str_author1, LEN_AUTHOR1) ||
      !strncmp(ptr, str_author2, LEN_AUTHOR2))
    {
      if (left = (char *) strchr(ptr, '('))
      {
        right = NULL;
        for (ptr = ++left; ch = *ptr; ptr++)
        {
          if (ch == ')')
            right = ptr;
          else if (ch == '\n')
            break;
        }

        if (right != NULL)
        {
          *right = '\0';
          sprintf(buf, "%s\t%s\t%s\t%s\t%s",
                  currboard, fh->filename, fh->owner,left, fh->title);
          f_cat ("innd/cancel.bntp", buf);
          f_cat ("innd/cancel.http", buf);
          
        }
      }
    }
#undef  NICK_LEN
  }

  return 1;
}

/* ----------------------------------------------------- */
/* Â÷¶} BBS ¯¸                                           */
/* ----------------------------------------------------- */


void
note()
{
  static char *fn_note_tmp = "note.tmp";
  static char *fn_note_dat = "note.dat";
  int total, i, collect, len;
  struct stat st;
  char buf[256], buf2[80];
  int fd, fx;
  FILE *fp, *foo;
  struct notedata
  {
    time_t date;
    char userid[IDLEN + 1];
    char username[19];
    char buf[3][80];
  };
  struct notedata myitem;
//  if(check_money(1,GOLD)) return;
  setutmpmode(EDNOTE);
  do
  {
    myitem.buf[0][0] = myitem.buf[1][0] = myitem.buf[2][0] = '\0';
    move(12, 0);
    clrtobot();
    outs("\n½Ð¯d¨¥ (¦Ü¦h¤T¦æ)¡A«ö[Enter]µ²§ô");
    for (i = 0; (i < 3) &&
      getdata(16 + i, 0, "¡G", myitem.buf[i], 78, DOECHO,0); i++);
    getdata(b_lines - 1, 0, "(S)Àx¦s (E)­«·s¨Ó¹L (Q)¨ú®ø¡H[S] ", buf, 3, LCECHO,"S");
/*
woju
*/
    if (buf[0] == 'q' || i == 0 && *buf != 'e')
      return;
  } while (buf[0] == 'e');
//  degold(1);
  strcpy(myitem.userid, cuser.userid);
  strncpy(myitem.username, cuser.username, 18);
  myitem.username[18] = '\0';
  time(&(myitem.date));

  /* begin load file */

  if ((foo = fopen(BBSHOME"/.note", "a")) == NULL)
    return;

  if ((fp = fopen(fn_note_ans, "w")) == NULL)
    return;

  if ((fx = open(fn_note_tmp, O_WRONLY | O_CREAT, 0644)) <= 0)
    return;

  if ((fd = open(fn_note_dat, O_RDONLY)) == -1)
  {
    total = 1;
  }
  else if (fstat(fd, &st) != -1)
  {
    total = st.st_size / sizeof(struct notedata) + 1;
    if (total > MAX_NOTE)
      total = MAX_NOTE;
  }

  fputs("[1m                             "COLOR1" [33m¡» [37m¤ß ±¡ ¯d ¨¥ ªO [33m¡» [0m \n\n",fp);
  collect = 1;

  while (total)
  {
    sprintf(buf, "[1;32m¡¼ùù [33m%s[37m(%s)",
      myitem.userid, myitem.username);
    len = strlen(buf);
    strcat(buf," [32m" + (len&1));

    for (i = len >> 1; i < 36; i++)
      strcat(buf, "ùù");
    sprintf(buf2, "ùù[33m %.14s [32mùù¡¼[0m\n",
      Cdate(&(myitem.date)));
    strcat(buf, buf2);
    fputs(buf, fp);

    if (collect)
      fputs(buf, foo);

    sprintf(buf, "%s\n%s\n%s\n", myitem.buf[0], myitem.buf[1], myitem.buf[2]);
    fputs(buf, fp);

    if (collect)
    {
      fputs(buf, foo);
      fclose(foo);
      collect = 0;
    }

    write(fx, &myitem, sizeof(myitem));

    if (--total)
      read(fd, (char *) &myitem, sizeof(myitem));
  }
  fclose(fp);
  close(fd);
  close(fx);
  f_mv(fn_note_tmp, fn_note_dat);
  more(fn_note_ans, YEA);
}


int
m_sysop()
{
  FILE *fp;
  char genbuf[200];

  setutmpmode(MSYSOP);
  if (fp = fopen(BBSHOME"/etc/sysop", "r"))
  {
    int i, j;
    char *ptr;

    struct SYSOPLIST
    {
      char userid[IDLEN + 1];
      char duty[40];
    }         sysoplist[9];

    j = 0;
    while (fgets(genbuf, 128, fp))
    {
      if (ptr = strchr(genbuf, '\n'))
      {
        *ptr = '\0';
        ptr = genbuf;
        while (isalnum(*ptr))
           ptr++;
        if (*ptr)
        {
          *ptr = '\0';
          do
          {
            i = *++ptr;
          } while (i == ' ' || i == '\t');
          if (i)
          {
            strcpy(sysoplist[j].userid, genbuf);
            strcpy(sysoplist[j++].duty, ptr);
          }
        }
      }
    }


    move(13, 0);
    clrtobot();
    prints("%16s   %-16sÅv³d¹º¤À\n", "½s¸¹", "¯¸ªø ID"/*, msg_seperator*/);

    for (i = 0; i < j; i++)
    {
      prints("%15d.   [1;%dm%-16s%s[0m\n",
        i + 1, 31 + i % 7, sysoplist[i].userid, sysoplist[i].duty);
    }
    prints("%-14s0.   [1;%dmÂ÷¶}[0m", "", 31 + j % 7);
    getdata(b_lines - 1, 0, "                   ½Ð¿é¤J¥N½X[0]¡G", genbuf, 4, DOECHO,"1");
    i = genbuf[0] - '0' - 1;
    if (i >= 0 && i < j)
    {
      clear();
      do_send(sysoplist[i].userid, NULL);
    }
  }
  return 0;
}


int
Goodbye()
{
//  extern void movie();
  char genbuf[100];

  getdata(b_lines, 0, "±z½T©w­nÂ÷¶}¡q" BOARDNAME "¡r¶Ü(Y/N)¡H[N] ",
    genbuf, 3, LCECHO,0);

  if (*genbuf != 'y')
    return 0;
//  movie(999);
  if (cuser.userlevel)
  {
//    getdata(b_lines, 0, "(G)®¨®¨Â÷¥h (M)¦«¹Ú¯¸ªø (N)¨è¯d¨¥ªO [G] ",
    getdata(b_lines, 0, "(0)¼g¼g¯d¨¥ªOÅo (1)Åý§Ú®¨®¨ªº¨«§a ",
      genbuf, 3, LCECHO,0);
    if (genbuf[0] == 'm')
      m_sysop();
    else if (genbuf[0] == '0' || genbuf[0] == 'n')
      note();
  }

  t_display();
  clear();
  prints("[1;31m¿Ë·Rªº [31m%s([37m%s)[31m¡A§O§Ñ¤F¦A«×¥úÁ{"COLOR1
    " %s [40;33m¡I\n\n¥H¤U¬O±z¦b¯¸¤ºªºµù¥U¸ê®Æ:[m\n",
    cuser.userid, cuser.username, BoardName);
  user_display(&cuser, 0);
  pressanykey(NULL);

  more(BBSHOME"/etc/Logout",NA);
  pressanykey(NULL);
  if (currmode)
    u_exit("EXIT ");
  reset_tty();
  exit(0);
}
