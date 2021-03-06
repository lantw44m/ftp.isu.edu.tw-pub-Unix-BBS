/*-------------------------------------------------------*/
/* bbs.c        ( NTHU CS MapleBBS Ver 2.36 )            */
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
  uid = strtok(buf, "/");       /* shakalaca.990721: ?????@???O?D */
  while (uid)
  {
    if (!InNameList(uid) && searchuser(uid))
      AddNameList(uid);
    uid = strtok(0, "/");       /* shakalaca.990721: ???V?U?@?? */
  }
  return 0;
}
                                    
/* shakalaca.990721: ???? BM ?W?? */
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
    brd_title = "?x?D??";
  sprintf (currBM, "?O?D?G%s", brd_title);
  brd_title = (bp->bvote == 1 ? "?????O?i????????" : bp->title + 7);

  currmode = (currmode & MODE_DIRTY) | MODE_STARTED;
  if (HAS_PERM (PERM_ALLBOARD) ||
      (HAS_PERM (PERM_BM) && is_BM (currBM + 6)))
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
[??]???} [??]?\\?? [^P]?o?????? [b]?????? [d]?R?? [z]?????? [TAB]???K [^Z]?D?U\n\
" COLOR1 "[1m  ?s??   ");
  if (currmode & MODE_TINLIKE)
    outs ("?g ??");
  else
    outs ("?? ??");
  outs ("  ?@  ??       ??  ??  ??  ?D                                    [m");
}


void
doent (num, ent)
     int num;
     fileheader *ent;
{
  int type,tag;
  char *mark, *title, color;
  static char *colors[7] =
  {"[1;36m", "[1;34m", "[1;33m", "[1;32m", "[1;35m", "[1;36m", "[1;37m"};

  if (currstat != RMAIL)
  {
    type = brc_unread (ent->filename) ? '+' : ' ';

    tag = ' ';  
/*  if (TagNum && !Tagger(atoi(ent->filename + 2) , num, TAG_NIN))
    tag = '*';
*/
    if ((currmode & MODE_BOARD) && (ent->filemode & FILE_DIGEST))
      type = (type == ' ') ? '*' : '#';
    if (ent->filemode & FILE_MARKED)
      type = (type == ' ') ? 'm' : 'M';
  }
  else
  {
    type = "+ Mm"[ent->filemode];

    if (ent->filemode & FILE_REPLYOK)
      type = (ent->filemode & FILE_MARKED) ? 'R' : 'r';
  }
  /* shakalaca.990421: ???F?@?U, ?]?? mail ?????_?????n?? */
  
  if (ent->filemode & FILE_TAGED)
    type = 't';
  
  title = str_ttl (mark = ent->title);
  if (title == mark)
    {
      color = '6';
      mark = "??";
    }
  else
    {
      color = '3';
      mark = "R:";
    }

  if (title[47])
    strcpy (title + 44, " ?K");  /* ???h?l?? string ???? */

  if (strncmp (currtitle, title, TTLEN))
    prints ("%6d %c%c%s%-7s[m%-13.12s%s %s\n", num, type,tag,
      colors[(unsigned int) (ent->date[4] + ent->date[5]) % 7],
      ent->date, ent->owner, mark, title);
  else
    prints ("%6d %c%c%s%-7s[m%-13.12s[1;3%cm%s %s[m\n", num, type,tag,
      colors[(unsigned int) (ent->date[4] + ent->date[5]) % 7],
      ent->date, ent->owner, color, mark, title);
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
do_select (ent, fhdr, direct)
     int ent;
     fileheader *fhdr;
     char *direct;
{
  char bname[20];
  char bpath[60], buf[80];
  struct stat st;

  move (0, 0);
  clrtoeol ();
  make_blist ();
  namecomplete (MSG_SELECT_BOARD, bname);

  setbpath (bpath, bname);
  if ((*bname == '\0') || (stat (bpath, &st) == -1))
    {
      move (2, 0);
      clrtoeol ();
      pressanykey (err_bid);
      return RC_FULL;
    }

/*
   board_visit_time = 0x7fffffff;
 */
  brc_initial (bname);
  setbfile (buf, bname, FN_LIST);
  if (currbrdattr & BRD_HIDE && !belong_list(buf, cuser.userid))
  {
    pressanykey (P_BOARD);
    return RC_FULL;
  }
  set_board ();
  setbdir (direct, currboard);

  move (1, 0);
  clrtoeol ();
  return RC_NEWDIR;
}
/* ----------------------------------------------------- */
/* ???} innbbsd ???X?H???B?s?u???H???B?z?{??             */
/* ----------------------------------------------------- */

outgo_post(fh, board)
  fileheader *fh;
  char *board;
{
  char buf[256];
  if(strcmp(fh->owner,cuser.userid))
    sprintf (buf, "%s\t%s\t%s\t%s\t%s", board,
      fh->filename, fh->owner, "????", fh->title);
  else
    sprintf (buf, "%s\t%s\t%s\t%s\t%s", board,
      fh->filename, fh->owner, cuser.username, fh->title);
  f_cat ("innd/out.bntp", buf);
}


static void
cancelpost (fh, by_BM)
     fileheader *fh;
     int by_BM;
{
  FILE *fin;
  char *ptr, *brd;
  fileheader postfile;
  char genbuf[200], buf[256];
  char nick[STRLEN], fn1[STRLEN], fn2[STRLEN];

  setbfile (fn1, currboard, fh->filename);
  if (fin = fopen (fn1, "r"))
  {
    brd = by_BM ? "deleted" : "junk";
    setbpath (fn2, brd);
    stampfile (fn2, &postfile);
    memcpy (postfile.owner, fh->owner, IDLEN + TTLEN + 10);
    postfile.savemode = 'D';
    log_board3("DEL", currboard, 1);
    if (fh->savemode == 'S')
    {
      nick[0] = '\0';
      while (fgets (genbuf, sizeof (genbuf), fin))
      {
        if (!strncmp (genbuf, str_author1, LEN_AUTHOR1) ||
            !strncmp (genbuf, str_author2, LEN_AUTHOR2))
        {
          if (ptr = strrchr (genbuf, ')'))
          *ptr = '\0';
          if (ptr = (char *) strchr (genbuf, '('))
          strcpy (nick, ptr + 1);
          break;
        }
      }

      sprintf (buf, "%s\t%s\t%s\t%s\t%s",
        currboard, fh->filename, cuser.userid, nick, fh->title);
      f_cat ("innd/cancel.bntp", buf);
    }
    fclose (fin);
    f_mv (fn1, fn2);
    setbdir (genbuf, brd);
    rec_add (genbuf, &postfile, sizeof (postfile));
  }
}


/* ----------------------------------------------------- */
/* ?o???B?^???B?s???B????????                            */
/* ----------------------------------------------------- */


void
do_reply_title (row, title)
  int row;
  char *title;
{
  char genbuf[200];
  char genbuf2[4];

  if (ci_strncmp (title, str_reply, 4))
    sprintf (save_title, "Re: %s", title);
  else
    strcpy (save_title, title);
  save_title[TTLEN - 1] = '\0';
  sprintf (genbuf, "?????????D?m%.60s?n???[Y] ", save_title);
  getdata (row, 0, genbuf, genbuf2, 4, LCECHO, 0);
  if (*genbuf2 == 'n')
    getdata (++row, 0, "???D?G", save_title, TTLEN, DOECHO, 0);
}


static void
do_reply (fhdr)
     fileheader *fhdr;
{
  char genbuf[200];

// Ptt ???O?s?p?t??
  if(!strcmp(currboard,VOTEBOARD))
    DL_func("SO/votebrd.so:va_do_voteboardreply",fhdr);
  else
  {
    getdata (b_lines - 1, 0, 
      "?? ?^???? (F)???O (M)?@???H?c (B)?G?????O (Q)?????H[F] ", 
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
        quote_file[79] = fhdr->savemode;
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
      int brdattr = bp->brdattr;
      char *ptr = bp->BM;
      
//      char buf[64], manager[IDLEN + 1];
      userec xuser;

      rec_get (fn_passwd, &xuser, sizeof (xuser), uid);
      if ((level & BRD_POSTMASK) || ((level) ? xuser.userlevel & (level) : 1))
  return 1;
  
      if ((brdattr & BRD_ANONYMOUS) && currutmp->invisible)
  return 1;

      if (ptr[0] <= ' ')
  return 0;

      if (userid_is_BM (userid, ptr))
  return 1;

      if ((level & 0xffff) != PERM_BBSADM)
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
  char genbuf[200];

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
    
      sethomepath (buf1, buf);
      stampfile (buf1, &mhdr);
      strcpy (mhdr.owner, cuser.userid);
      strcpy (mhdr.title, "[?s]");
      strncat (mhdr.title, save_title, TTLEN - 4);
      mhdr.savemode = 0;
      mhdr.filemode = 0;
      sethomedir (title, buf);
      rec_add (title, &mhdr, sizeof (mhdr));
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
      mhdr.savemode = 0;
      mhdr.filemode = 0;
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
    mhdr.savemode = 0;
    mhdr.filemode = 0;
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
  mhdr.savemode = 0;
  mhdr.filemode = 0;
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

extern long wordsnum;    /* ?p???r?? */

int
do_post ()
{
  fileheader postfile;
  char fpath[80], buf[80];
  int aborted;
  char genbuf[200], *owner;
//  boardheader *bp;
  boardheader *getbcache ();
  time_t spendtime;
  int num = 0, i;
  char *postprefix[] =
  {"[???i] ", "[?q??] ", "[????] ", "[???D] ", "[???@] ", "[?s?D] ", "[????] ",
   "[????] ", "[????] ",NULL};

//  bp = getbcache (currboard);
  
// ?T?n?W???T?{  
  setbfile (buf, currboard, FN_LIST);  
  if (!HAS_PERM (PERM_BBSADM) && brd_bad_guy (buf, cuser.userid))
  {
    pressanykey ("?????_?A?z?Q???D?C?J?T?n?W???F...");
    return RC_FULL;
  }
  
  if ( !HAS_PERM (PERM_POST) || HAS_PERM (PERM_FORBIDPOST) || !(currmode & MODE_POST) || !brdperm (currboard, cuser.userid))
  {
    pressanykey ("?????_?A?z???e?L?k?b???o???????I");
    return RC_FULL;
  }

// Ptt ???O?s?p?t??
  if(!strcmp(currboard,VOTEBOARD))
  {
    DL_func("SO/votebrd.so:do_voteboard");
    return RC_FULL;
  }
  
/*
  setbfile (buf, currboard, FN_LIST);
  if (dashf (buf) && belong ("etc/have_postcheck", currboard))
  {
    if (!HAS_PERM (PERM_BBSADM) && !belong_list (buf, cuser.userid))
    {
      pressanykey ("?????_,???O?u?????O?n???~???o??????,???V?O?D????");
      return RC_FULL;
    }
  }
*/ 
  clear();
  more ("etc/post.note", NA);
  prints ("?o?????????i %s ?j???O\n", currboard);

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
    getdata (20, 0, "?????????????O?????????J???O(????Enter???L)?G", genbuf, 9, DOECHO, 0);
    i = atoi (genbuf);
    if (i > 0 && i <= num)  /* ???J???r???? */
      strncpy (save_title, postprefix[i - 1], strlen (postprefix[i - 1]));
    else if (strlen (genbuf) >= 3)  /* ???????J */
      strncpy (save_title, genbuf, strlen (genbuf));
    else      /* ???????L */
      save_title[0] = '\0';

    getdata (21, 0, "???D?G", save_title, TTLEN, DOECHO, save_title);
    strip_ansi (save_title, save_title, ONLY_COLOR);
  }
  if (save_title[0] == '\0')
    return RC_FULL;

  curredit &= ~EDIT_MAIL;
  curredit &= ~EDIT_ITEM;
  setutmpmode (POSTING);

  /* ?????? Internet ?v?????A?u???b?????o?????? */

  if (HAS_PERM (PERM_INTERNET))
    local_article = 0;
  else
    local_article = 1;

  buf[0] = 0;

  spendtime = time (0);
  aborted = vedit (buf, YEA);
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

#ifdef HAVE_ANONYMOUS
/* Ptt and Jaky */
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

  strcpy (postfile.owner, owner);
  strcpy (postfile.title, save_title);
//  if (aborted == 1)    /* local save */
  if (local_article == 1)    /* local save */
  {
    postfile.savemode = 'L';
    postfile.filemode = FILE_LOCAL;
  }
  else
    postfile.savemode = 'S';

  setbdir (buf, currboard);
  if (rec_add (buf, &postfile, sizeof (postfile)) != -1)
  {
    if (currmode & MODE_SELECT)
    rec_add (currdirect, &postfile, sizeof (postfile));
//    if (aborted != 1 && !(currbrdattr & BRD_NOTRAN))
    if (local_article != 1 && !(currbrdattr & BRD_NOTRAN))
      outgo_post (&postfile, currboard);
    brc_addlist (postfile.filename);

    if (!(currbrdattr & BRD_NOCOUNT))
    {
      if (wordsnum <= 30)
        pressanykey ("???p?A???u?????????C?J?????C");
      else
      {
        int money = (wordsnum <= spendtime ? (wordsnum / 100) : 
                    (spendtime / 100));
        int exp = (wordsnum <= spendtime ? (wordsnum) : (spendtime));
        money *= (float)(((rand () % 10) + 10) / 10);
        exp *= (float)(((rand () % 10) + 10) / 10) ;
        if (money < 1) money = 1;
        if (exp < 10) exp = 10;
        if (rpguser.race == 1) exp *= rpguser.level;

        clear ();
        move (7, 0);
        update_data ();
        prints ("\
              [1;36m?i[37m?p ?? ?Z ?S[36m?j\n
              [37m ?o?O?z??[33m?? %d ?g[37m?????C
              [36m?i?O???j[33m %d [37m??[33m % d [37m???C
              [36m?i?Z?S?j[33m %d [37m(????)
              [36m?i?g?????j[33m %d [37m ?I ",
        ++cuser.numposts, spendtime / 60, spendtime % 60, money, exp);
        substitute_record (fn_passwd, &cuser, sizeof (userec), usernum);
        inexp (exp);
        ingold (money);  // post???o???? by wildcat

        pressanykey ("?????o?????? :)");
        if (money >= 100 || exp >= 10000)
        {
          FILE * fp;
          time_t now = time (0);
          fileheader mhdr;
          char genbuf1[200], fpath1[STRLEN];

          setbpath (genbuf1, "Security");
          stampfile (genbuf1, &mhdr);
          strcpy (mhdr.owner, cuser.userid);
          strncpy (mhdr.title, "POST???d", TTLEN);
          mhdr.savemode = '\0';
          setbdir (fpath1, "Security");
          if (rec_add (fpath1, &mhdr, sizeof (mhdr)) == -1)
          {
            outs (err_uid);
            return 0;
          }
          if ((fp = fopen (genbuf1, "w")) != NULL)
          {
            fprintf (fp, "?@??: %s (%s)\n", cuser.userid, cuser.username);
            fprintf (fp, "???D: %s\n????: %s\n", "POST???d", ctime (&now));
            fprintf (fp, 
"%s ?o???@?g %d ?r???????? %s ?O\n???F %d ???A?o?????? %d ??,?g???? %d ?I"
              ,cuser.userid, wordsnum, currboard, spendtime, money, exp);
            fclose (fp);
          }
        }
      }
//      UPDATE_USEREC;
    }
    else
      pressanykey ("?????O???????C?J?????A?q???]?[?C");

    log_board3("POS", currboard, cuser.numposts);

  /* ?^???????@???H?c */
    if (curredit & EDIT_BOTH)
    {
      char *str, *msg = "?^?????@???H?c";

      if (str = strchr (quote_user, '.'))
      {
        if (bbs_sendmail (fpath, save_title, str + 1) < 0)
          msg = "?@???L?k???H";
      }
      else
      {
        sethomepath (genbuf, quote_user);
        stampfile (genbuf, &postfile);
        unlink (genbuf);
        f_cp (fpath, genbuf, O_TRUNC);

        strcpy (postfile.owner, cuser.userid);
        strcpy (postfile.title, save_title);
        postfile.savemode = 'B';  /* both-reply flag */
        sethomedir (genbuf, quote_user);
        if (rec_add (genbuf, &postfile, sizeof (postfile)) == -1)
          msg = err_uid;
      }
      outs (msg);
      curredit ^= EDIT_BOTH;
    }
    do_all_post(fpath); // ???????????????K??
// wildcat : do_all_post ???N
//    if (currbrdattr & BRD_ANONYMOUS)  /*  ?????W  */
//      do_unanonymous_post (fpath);

#ifdef POSTNOTIFY    /* ?s?????q?? */
    if (!(currbrdattr & BRD_ANONYMOUS) && !(currbrdattr & BRD_HIDE))
      do_postnotify (fpath);
#endif
  }
  return RC_FULL;
}


static int
reply_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  if (!(currmode & MODE_POST))
    return RC_NONE;

  setdirpath (quote_file, direct, fhdr->filename);
// Ptt ?????O?s?p?t??
  if(!strcmp(currboard,VOTEBOARD))
    DL_func("SO/votebrd.so:va_do_voteboardreply",fhdr);
  else
    do_reply (fhdr);
  *quote_file = 0;
  return RC_FULL;
}


int
edit_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char fpath[80], fpath0[80];
  extern bad_user (char *name);
  char genbuf[200];
  fileheader postfile;
  int edit_mode;

  if (!strcmp(currboard,"Security")) return RC_NONE;

  if (currstat == RMAIL)
  {
    setdirpath (genbuf, direct, fhdr->filename);
    vedit (genbuf, belong ("etc/sysop", cuser.userid) ? 0 : 2);
    return RC_FULL;
  }

  if (HAS_PERM (PERM_SYSOP) ||
    !strcmp (fhdr->owner, cuser.userid) && strcmp (cuser.userid, "guest") &&
    !bad_user (cuser.userid))
    edit_mode = 0;
  else
    edit_mode = 2;

  setdirpath (genbuf, direct, fhdr->filename);
  local_article = fhdr->filemode & FILE_LOCAL;
  strcpy (save_title, fhdr->title);

  if (vedit (genbuf, edit_mode) != -1)
  {
    setbpath (fpath, currboard);
    stampfile (fpath, &postfile);
    unlink (fpath);
    setbfile (fpath0, currboard, fhdr->filename);
    f_mv (fpath0, fpath);
    strcpy (fhdr->filename, postfile.filename);
    strcpy (fhdr->title, save_title);
    brc_addlist (postfile.filename);
    substitute_record (direct, fhdr, sizeof (*fhdr), ent);
#ifdef POSTNOTIFY
    if (currbrdattr & BRD_ANONYMOUS)
      do_postnotify (fpath);
#endif
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
  char genbuf2[4];

  make_blist ();
  move (2, 0);
  clrtoeol ();
  move (3, 0);
  clrtoeol ();
  move (1, 0);
  namecomplete ("???????????????O?G", xboard);
  if (*xboard == '\0' || !haspostperm (xboard))
    return RC_FULL;

  ent = 1;
  if (HAS_PERM (PERM_SYSOP) || !strcmp (fhdr->owner, cuser.userid))
  {
    getdata (2, 0, "(1)???????? (2)???????????H[1] ",
    genbuf, 3, DOECHO, "1");
    if (genbuf[0] != '2')
    {
      ent = 0;
      getdata (2, 0, "?O?d???@???W?????[Y] ", inputbuf, 3, DOECHO, 0);
      if (inputbuf[0] != 'n' && inputbuf[0] != 'N') author = 1;
    }
  }

  if (ent)
    sprintf (xtitle, "[????]%.66s", fhdr->title);
  else
    strcpy (xtitle, fhdr->title);

  sprintf (genbuf, "?????????D?m%.60s?n???[Y] ", xtitle);
  getdata (2, 0, genbuf, genbuf2, 4, LCECHO, 0);
  if (*genbuf2 == 'n')
  {
    if (getdata (2, 0, "???D?G", genbuf, TTLEN, DOECHO, xtitle))
      strcpy (xtitle, genbuf);
  }

  getdata (2, 0, "(S)?s?? (L)???? (Q)?????H[S] ", genbuf, 3, LCECHO, "S");
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
    if (genbuf[0] == 'l')
    {
      xfile.savemode = 'L';
      xfile.filemode = FILE_LOCAL;
    }
    else
      xfile.savemode = 'S';

    setbfile (fname, currboard, fhdr->filename);
    if (ent)
    {
      xptr = fopen (xfpath, "w");
      strcpy (save_title, xfile.title);
      strcpy (xfpath, currboard);
      strcpy (currboard, xboard);
      write_header (xptr);
      strcpy (currboard, xfpath);

      fprintf (xptr, "?? [?????????? %s ???O]\n\n", currboard);

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
    pressanykey ("????????????");
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
  char genbuf[200];
  int more_result;
  time_t startime = time (0);

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
/*  woju  */
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
/* ??????????                                            */
/* ----------------------------------------------------- */

man()
{
  char buf[64], buf1[64], xboard[20], fpath[256];
  boardheader * bp;
  boardheader * getbcache ();

  if (currstat == RMAIL)
  {
    move (2, 0); clrtoeol ();
    move (3, 0); clrtoeol ();
    move (1, 0); make_blist ();
    namecomplete ("???J?????W?? (????Enter?i?J?p?H?H????)?G", buf);
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
    else if(HAS_PERM(PERM_MAILLIMIT) || HAS_PERM(PERM_BM)) // wildcat : ???e???O?[ PERM ?????? ^^;
    {
      int mode0 = currutmp->mode;
      int stat0 = currstat;
      sethomeman (buf, cuser.userid);
      sprintf (buf1, "%s ???H????", cuser.userid);
      setutmpmode (ANNOUNCE);
      a_menu (buf1, buf, belong ("etc/sysop", cuser.userid) ? 2 : 1);
      currutmp->mode = mode0;
      currstat = stat0;
      return RC_FULL;
    }
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
cite (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char fpath[256];
  char title[TTLEN + 1];

  if (currstat == RMAIL)
  {
    setuserfile (fpath, fhdr->filename);
    add_tag ();
  }
  else
    setbfile (fpath, currboard, fhdr->filename);

  strcpy (title, "?? ");
  strncpy (title + 3, fhdr->title, TTLEN - 3);
  title[TTLEN] = '\0';
//  a_copyitem (fpath, title, fhdr->owner);
  a_copyitem (fpath, title, cuser.userid);
  /* shakalaca.990517: ?????????n?D, ?s?????O?D */
  man ();
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
  sprintf (title, "%s%.72s",(currutmp->pager > 1) ? "" : "?? ", fhdr->title);
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
//  add_tag ();
  load_paste ();
  if (*paste_path && paste_level && dashf (fpath))
  {
    fileheader fh;
    char newpath[MAXPATHLEN];

    strcpy (newpath, paste_path);
    stampfile (newpath, &fh);
/* shakalaca.990714: ?N?????W??
    unlink (newpath); */
    f_cp (fpath, newpath, O_TRUNC);
    strcpy (fh.owner, cuser.userid);
    sprintf (fh.title, "%s%.72s",
    (currutmp->pager > 1) ? "?? " : "?? ", fhdr->title);
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
  char genbuf[200];

//  if (!strcmp(currboard,"Security")) return RC_NONE;
  if (HAS_PERM (PERM_SYSOP) || !strcmp (fhdr->owner, cuser.userid) || (currmode & MODE_BOARD))
  {
    fileheader tmpfhdr = *fhdr;
    int dirty = 0;

    if (getdata (b_lines - 1, 0, "???D?G", genbuf, TTLEN, DOECHO, tmpfhdr.title))
    {
      strcpy (tmpfhdr.title, genbuf);
      dirty++;
    }

    if(HAS_PERM (PERM_SYSOP))
    {
      if (getdata (b_lines - 1, 0, "?@???G", genbuf, IDLEN + 2, DOECHO, tmpfhdr.owner))
      {
        strcpy (tmpfhdr.owner, genbuf);
        dirty++;
      }

      if (getdata (b_lines - 1, 0, "?????G", genbuf, 6, DOECHO, tmpfhdr.date))
      {
        sprintf (tmpfhdr.date, "%+5s", genbuf);
        dirty++;
      }
    }

    if (getdata (b_lines - 1, 0, "?T?w(Y/N)?[n] ", genbuf, 3, DOECHO, 0) &&
      (*genbuf == 'y' || *genbuf == 'Y' )&& dirty)
    {
      *fhdr = tmpfhdr;
      substitute_record (direct, fhdr, sizeof (*fhdr), ent);
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
add_tag (ent, fhdr, direct)
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
    sethomedir (genbuf, cuser.userid);
    if (currmode & SELECT)
    {
      now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
      substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
      sprintf (genbuf, "home/%s/SR.%s", cuser.userid, cuser.userid);
      substitute_record (genbuf, fhdr, sizeof (*fhdr), ent);
    }
    else
      substitute_record (genbuf, fhdr, sizeof (*fhdr), ent);
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
  char genbuf[3];
  int number;

  if (currstat == RMAIL)
  {
    getdata (1, 0, "?T?w?R?????O?H??(Y/N)? [Y]", genbuf, 3, LCECHO, 0);
    if (genbuf[0] != 'n')
    {
      currfmode = FILE_TAGED;
      if (delete_files (direct, cmpfmode))
        return RC_CHDIR;
    }
    return RC_FULL;
  }
  if ((currstat != READING) || (currmode & MODE_BOARD))
  {
    if (!strcmp(currboard,"Security")) return RC_NONE;
    getdata (1, 0, "?T?w?R?????O????(Y/N)? [N]", genbuf, 3, LCECHO, 0);
    if (genbuf[0] == 'y')
    {
      currfmode = FILE_TAGED;
      if (currmode & MODE_SELECT)
      {
        unlink (direct);
        currmode ^= MODE_SELECT;
        setbdir (direct, currboard);
        delete_files (direct, cmpfmode);
      }
       if (number=delete_files(direct, cmpfmode)){
        log_board3("TAG", currboard, number);
                 return RC_CHDIR;
        }
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

  load_paste();	//???J paste_file ???w??
  if(!*paste_path)
  {
    pressanykey("?|???w??,???i?J?????????A?Q???????????? [P]");
    return RC_FOOT;
  }  

  if (currstat == RMAIL)
  {
    getdata (1, 0, "?T?w???????O?H??(Y/N)? [Y]", genbuf, 3, LCECHO, 0);
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
    getdata (1, 0, "?T?w???????O????(Y/N)? [N]", genbuf, 3, LCECHO, 0);
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
mark (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
/*
  if (currstat == RMAIL)
  {
    fhdr->filemode ^= FILE_MARKED;
    fhdr->filemode &= ~FILE_TAGED;
    sethomedir (currmaildir, cuser.userid);
    substitute_record (currmaildir, fhdr, sizeof (*fhdr), ent);
    return (RC_DRAW);
  }
*/
  if (currstat == READING && !(currmode & MODE_BOARD))
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
      sethomedir(genbuf, cuser.userid);
    else
      setbdir (genbuf, currboard);
    now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
    substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
  }
  else
    substitute_record (direct, fhdr, sizeof (*fhdr), ent);

  return RC_DRAW;
}


int
del_range (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char num1[8], num2[8];
  int inum1, inum2;

  if (!strcmp(currboard,"Security")) return RC_NONE;
  if ((currstat != READING) || (currmode & MODE_BOARD))
  {
    getdata (1, 0, "[?]?w?R???d??] ?_?I?G", num1, 5, DOECHO, 0);
    inum1 = atoi (num1);
    if (inum1 <= 0)
    {
      outmsg ("?_?I???~");
      refresh ();
      sleep (1);
      return RC_FOOT;
    }
    getdata (1, 28, "???I?G", num2, 5, DOECHO, 0);
    inum2 = atoi (num2);
    if (inum2 < inum1)
    {
      outmsg ("???I???~");
      refresh ();
      sleep (1);
      return RC_FULL;
    }
    getdata (1, 48, msg_sure_ny, num1, 3, LCECHO, 0);
    if (*num1 == 'y')
    {
      outmsg ("?B?z??,???y??...");
      refresh ();
      if (currmode & MODE_SELECT)
      {
        int fd, size = sizeof (fileheader);
        char genbuf[100];
        fileheader rsfh;
        int i = inum1, now;
        if (currstat == RMAIL)
          sethomedir (genbuf, cuser.userid);
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
      delete_range (direct, inum1, inum2);
//      fixkeep (direct, inum1);
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
  fhdr->savemode = 'L';
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
  fileheader * fhdr;
  char *direct;
{
  int not_owned, money;
  char genbuf[100];

  if (!strcmp(currboard,"Security")) return RC_NONE;
  if ((fhdr->filemode & FILE_MARKED)
    || (fhdr->filemode & FILE_DIGEST) || (fhdr->owner[0] == '-'))
    return RC_NONE;

  not_owned = strcmp (fhdr->owner, cuser.userid);

  if (!(currmode & MODE_BOARD) && not_owned || !strcmp (cuser.userid, "guest"))
    return RC_NONE;

  getdata (1, 0, msg_del_ny, genbuf, 3, LCECHO, 0);
  if (genbuf[0] == 'y')
  {
    strcpy (currfile, fhdr->filename);
    setbfile (genbuf, currboard, fhdr->filename);
    money = (int) dashs (genbuf) / 90;
    if (!delete_file (direct, sizeof (fileheader), ent, cmpfilename))
    {
      if (currmode & MODE_SELECT)
      {
        int now;

        setbdir (genbuf, currboard);
        now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
        delete_file (genbuf, sizeof (fileheader), now, cmpfilename);
      }
      cancelpost (fhdr, not_owned);
      if (!not_owned && !(currbrdattr & BRD_NOCOUNT))
      {
        UPDATE_USEREC;
        move (b_lines - 1, 0);
        clrtoeol ();
        if (money < 1) money = 1;
        if(cuser.goldmoney > money)
          degold (money);
        else
          demoney(money*10000);
        pressanykey ("%s?A?z?????????? %d ?g?A???I?M???O %d ??", msg_del_ok,
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
  char fpath[MAXPATHLEN];
  char genbuf[MAXPATHLEN];
  char *p;

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
  sethomepath (genbuf, cuser.userid);
  stampfile (genbuf, &mhdr);
  unlink (genbuf);
  f_cp (fpath, genbuf, O_TRUNC);
  if (HAS_PERM (PERM_SYSOP))
    strcpy (mhdr.owner, fh->owner);
  else
    strcpy (mhdr.owner, cuser.userid);
  strncpy (mhdr.title, fh->title + ((currstat == ANNOUNCE) ? 3 : 0), TTLEN);
  mhdr.savemode = '\0';
  mhdr.filemode = FILE_READ;
  sethomedir (fpath, cuser.userid);
  if (rec_add (fpath, &mhdr, sizeof (mhdr)) == -1)
  {
    bell ();
    return RC_NONE;
  }
  return POS_NEXT;
}

/* ----------------------------------------------------- */
/* ???O???????B???K?B??????                              */
/* ----------------------------------------------------- */


static int
b_notes_edit ()
{
  if (currmode & MODE_BOARD)
  {
    char buf[64];
    int aborted;

    setbfile (buf, currboard, fn_notes);
    aborted = vedit (buf, NA);
    if (aborted == -1)
      pressanykey (msg_cancel);
    else
    {
      boardheader fh;
      int pos;

      getdata (3, 0, "???]?w????????(0 - 9999)???H", buf, 5, DOECHO, "9999");
      aborted = atol (buf);
      pos = rec_search (fn_board, &fh, sizeof (fh), cmpbnames, (int) currboard);
/* SiE: ?o???Prec_search?????J?????A???P..?????O?z???????] */
      fh.bupdate = aborted ? time (0) + aborted * 86400 : 0;
      substitute_record (fn_board, &fh, sizeof (fh), pos);
      touch_boards ();
    }
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
bh_title_edit ()
{
  boardheader bp;
  boardheader * getbcache ();
  int bid;
  time_t now=time(0);

  if (currmode & MODE_BOARD)
  {
    char genbuf[BTLEN],buf[512];
    bid = getbnum (currboard);
    if (rec_get (fn_board, &bp, sizeof (boardheader), bid) == -1)
    {
      pressanykey (err_bid);
      return -1;
    }
    if(bp.brdattr & BRD_PERSONAL)
    {
      char ans;
      int oldattr=bp.brdattr;
      
      ans = answer("?O?_???????O???A? (y/N)");
      if(ans == 'Y' || ans == 'y')
      {
        ans = answer("???O???A?????? (o)?}?? (p)?p?H (h)?????");
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
          bp.brdattr &= ~BRD_POSTMASK;
          bp.brdattr &= ~BRD_HIDE;
        }
        if(bp.brdattr != oldattr)
        {
          sprintf(buf,"%s %-13.13s ???????O [%s] ???????? %s .",
            ctime(&now),cuser.userid,bp.brdname,
            ans == 'p' ? "?p?H" : ans == 'h' ? "????" : "?}??");
          f_cat(BBSHOME"/log/board_edit",buf);
        }
      }      
      move (1, 0);
      clrtoeol ();
      getdata (1, 0, "?????J???O?s???O:",genbuf, 5, DOECHO, bp.title );
      if (!genbuf[0]) return 0;
      strip_ansi (genbuf, genbuf, 0);
      if(strncmp(bp.title,genbuf,4))
      {
        sprintf(buf,"%s %-13.13s ???????O %s ???O [%s] -> [%s]",
          ctime(&now),cuser.userid,bp.brdname,bp.title,genbuf);
        f_cat(BBSHOME"/log/board_edit",buf);
        strncpy (bp.title , genbuf, 4);
      }
    }
    move (1, 0);
    clrtoeol ();
    getdata (1, 0, "?????J???O?s???????z:" 
      ,genbuf, BTLEN - 13, DOECHO, bp.title + 7);
    if (!genbuf[0]) return 0;
    strip_ansi (genbuf, genbuf, 0);
    if(strcmp(bp.title+7,genbuf))
    {
      sprintf(buf,"%s %-13.13s ???????O %s ???z [%s] -> [%s]",
        ctime(&now),cuser.userid,bp.brdname,bp.title+7,genbuf);
      f_cat(BBSHOME"/log/board_edit",buf);
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
      if(!getdata (1, 0, "?????J???????K?X" ,genbuf, PASSLEN, PASS, 0) ||
         !chkpasswd(bp.passwd, genbuf))
         {
           pressanykey("?K?X???~");
           return -1;
         }
    }
    if (!getdata(1, 0, "???]?w?s?K?X?G", genbuf, PASSLEN, PASS,0))
    {
      pressanykey("?K?X?]?w????, ?~?????????K?X");
      return -1;
    }
    strncpy(buf, genbuf, PASSLEN);

    getdata(1, 0, "?????d?s?K?X?G", genbuf, PASSLEN, PASS,0);
    if (strncmp(genbuf, buf, PASSLEN))
    {
      pressanykey("?s?K?X?T?{????, ?L?k?]?w?s?K?X");
      return -1;
    }
    buf[8] = '\0';
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
  if (more (buf, YEA) == -1) // ?i?H???h??
  {
    clear ();
    pressanykey ("?????O?|?L?u???????v?C");
  }
  return RC_FULL;
}


int
board_select ()
{
  struct stat st;
  char fpath[80];
  char genbuf[100];
  currmode &= ~(MODE_SELECT | MODE_TINLIKE);
  
  sprintf (genbuf, "SR.%s", cuser.userid);
  setbfile (fpath, currboard, genbuf);
  unlink (fpath);
  
  /* shakalaca.000112: ?W?L 30min ?~?N index ?R??, ?@ cache ?? */
  setbfile (fpath, currboard, "SR.thread");
  if (stat(fpath, &st) == 0 && st.st_mtime < time(0) - 60 * 30) 
    unlink (fpath);
  
  if (currstat == RMAIL)
    sethomedir (currdirect, cuser.userid);
  else
    setbdir (currdirect, currboard);

  return RC_NEWDIR;
}


int
board_digest ()
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


static int
good_post (ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char genbuf[200];
  char genbuf2[200];
  fileheader digest;

  memcpy (&digest, fhdr, sizeof (digest));
  digest.filename[0] = 'G';

  if ((currmode & MODE_DIGEST) || !(currmode & MODE_BOARD))
    return RC_NONE;

  if (fhdr->filemode & FILE_DIGEST)
  {
    int now;
    setbgdir (genbuf2, currboard);
    now = getindex (genbuf2, digest.filename);
    strcpy (currfile, digest.filename);
    delete_file (genbuf2, sizeof (fileheader), now, cmpfilename);
    sprintf (genbuf2, BBSHOME "/boards/%s/%s", currboard, currfile);
    unlink (genbuf2);
    fhdr->filemode = (fhdr->filemode & ~FILE_DIGEST);
    deumoney (fhdr->owner, 500);
  }
  else
  {
    char *ptr, buf[64];
    strcpy (buf, direct);
    ptr = strrchr (buf, '/') + 1;
    ptr[0] = '\0';
    sprintf (genbuf, "%s%s", buf, digest.filename);
    if (!dashf (genbuf))
    {
      digest.savemode = digest.filemode = 0;
      sprintf (genbuf2, "%s%s", buf, fhdr->filename);
      f_cp (genbuf2, genbuf, O_TRUNC);
      strcpy (ptr, fn_mandex);
      rec_add (buf, &digest, sizeof (digest));
    }
    fhdr->filemode = (fhdr->filemode & ~FILE_MARKED) | FILE_DIGEST;
    inumoney (fhdr->owner, 500);
  }
  substitute_record (direct, fhdr, sizeof (*fhdr), ent);
  if (currmode & MODE_SELECT)
  {
    int now;
    char genbuf[100];
    setbdir (genbuf, currboard);
    now = getindex (genbuf, fhdr->filename, sizeof (fileheader));
    substitute_record (genbuf, fhdr, sizeof (*fhdr), now);
  }
  return RC_DRAW;
}

/* ----------------------------------------------------- */
/* ???O?\????                                            */
/* ----------------------------------------------------- */
//extern int b_vote ();
//extern int b_results ();
/* extern int b_vote_maintain (); */

struct one_key read_comms[] =
{
  KEY_TAB, board_digest,
  'b', b_notes,
  'c', cite,
  'r', read_post,
  'z', man,
  'D', del_range,
/* woju
   Ctrl('D'), del_one,
*/
  Ctrl ('S'), save_mail,
  'E', edit_post,
  'T', edit_title,
  's', do_select,
//  'R', b_results,
//  'V', b_vote,
//  'M', b_vote_maintain,
  'B', bh_title_edit,
  'i', bh_passwd_edit,
  'W', b_notes_edit,
  't', add_tag,
//  'w', b_list_edit,
  'o', b_list_edit,
  Ctrl ('D'), del_tag,
  'x', cross_post,
  'g', good_post,
  'y', reply_post,
  'd', del_post,
  'm', mark,
  Ctrl ('P'), do_post,
  'C', gem_tag,
  Ctrl ('C'), Cite_posts,
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
  if (board_visit_time < board_note_time)
  {
    setbfile (buf, currboard, fn_notes);
    more (buf, YEA);
  }
  bid = getbnum (currboard);
  currutmp->brc_id = bid;
  setbdir (buf, currboard);
  curredit &= ~EDIT_MAIL;
  i_read (READING, buf, readtitle, doent, read_comms, &(brdshm->total[bid - 1]));

  log_board (currboard, time (0) - startime);
  log_board2 (currboard, time (0) - startime);
//  inexp(rpguser.race == 2 ? (time(0)-startime)*rpguser.level : time(0)-startime); 
  brc_update ();

  currutmp->brc_id = 0;
  currutmp->mode = mode0;
  currstat = stat0;
//  return;
}


void
ReadSelect ()
{
/*  
  int mode0 = currutmp->mode;
  int stat0 = currstat;
  char genbuf[200];

  currstat = XMODE;
*/
  if (do_select (0, 0, NULL) == RC_NEWDIR)
    Read ();
/*
  currutmp->mode = mode0;
  currstat = stat0;
*/
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

void
cancel_post(fhdr, fpath)
  fileheader *fhdr;
  char *fpath;
{
  int fd;

  if ((fhdr->savemode == 'S') &&/* ?~???H?? */
    ((fd = open(fpath, O_RDONLY)) >= 0))
  {
#define	NICK_LEN	80

    char *ptr, *left, *right, nick[NICK_LEN];
    FILE *fout;
    int ch;

    ptr = nick;
    ch = read(fd, ptr, NICK_LEN);
    close(fd);
    ptr[ch] = '\0';
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
          log_board3("DEL", currboard, 1);

	  if (fout = fopen(BBSHOME"/innd/cancel.bntp", "a"))
	  {
	    fprintf(fout, "%s\t%s\t%s\t%s\t%s\n",
	      currboard, fhdr->filename, fhdr->owner	/* cuser.userid */
	      ,left, fhdr->title);
	    fclose(fout);
	  }
	}
      }
    }
#undef	NICK_LEN
  }
}

/* ----------------------------------------------------- */
/* ???} BBS ??                                           */
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
    outs("\n???d?? (???h?T??)?A??[Enter]????");
    for (i = 0; (i < 3) &&
      getdata(16 + i, 0, "?G", myitem.buf[i], 78, DOECHO,0); i++);
    getdata(b_lines - 1, 0, "(S)?x?s (E)???s???L (Q)?????H[S] ", buf, 3, LCECHO,"S");
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

  fputs("[1m                             "COLOR1" [33m?? [37m?? ?? ?d ?? ?O [33m?? [m \n\n",fp);
  collect = 1;

  while (total)
  {
    sprintf(buf, "[1;32m???? [33m%s[37m(%s)",
      myitem.userid, myitem.username);
    len = strlen(buf);
    strcat(buf," [32m" + (len&1));

    for (i = len >> 1; i < 36; i++)
      strcat(buf, "??");
    sprintf(buf2, "??[33m %.14s [32m????[m\n",
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
    prints("%16s   %-16s?v?d????\n", "?s??", "???? ID"/*, msg_seperator*/);

    for (i = 0; i < j; i++)
    {
      prints("%15d.   [1;%dm%-16s%s[0m\n",
        i + 1, 31 + i % 7, sysoplist[i].userid, sysoplist[i].duty);
    }
    prints("%-14s0.   [1;%dm???}[0m", "", 31 + j % 7);
    getdata(b_lines - 1, 0, "                   ?????J?N?X[0]?G", genbuf, 4, DOECHO,"1");
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
  extern void movie();
  char genbuf[100];

  getdata(b_lines, 0, "?z?T?w?n???}?q" BOARDNAME "?r??(Y/N)?H[N] ",
    genbuf, 3, LCECHO,0);

  if (*genbuf != 'y')
    return 0;
  movie(999);
  if (cuser.userlevel)
  {
    getdata(b_lines, 0, "(G)???????h (M)???????? (N)???d???O [G] ",
      genbuf, 3, LCECHO,0);
    if (genbuf[0] == 'm')
      m_sysop();
    else if (genbuf[0] == 'n')
      note();
  }

  t_display();
  clear();
  prints("[1;31m???R?? [31m%s([37m%s)[31m?A?O???F?A?????{"COLOR1
    " %s [40;33m?I\n\n?H?U?O?z?b?????????U????:[m\n",
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
