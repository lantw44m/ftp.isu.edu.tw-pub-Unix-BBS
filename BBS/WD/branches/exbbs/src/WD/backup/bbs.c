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

extern long wordsnum;    /* 計算字數 */

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
      strcpy(buf,"標題: ");
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
   char fpath[256], fpath1[STRLEN], poster[]="[系統報告]";

   setbpath (fpath, "BBSpost");
   stampfile (fpath, &mhdr);
   strcpy (mhdr.owner, poster);
   strcat (mhdr.title, string1);
   strcat (mhdr.title, " 檢查 ");
   strcat (mhdr.title, cuser.userid);

   setbdir (fpath1, "BBSpost");

   if (rec_add (fpath1, &mhdr, sizeof (mhdr)) == -1)
   {
     outs (err_uid);
     return;
   }
   
   if ((fp = fopen (fpath, "w")) != NULL)
   {
      fprintf (fp, "作者: %s\n", poster);
      fprintf (fp, "標題: %s\n時間: %s\n", mhdr.title, ctime(&now));

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
  uid = strtok(buf, "/");       /* shakalaca.990721: 找第一個板主 */
  while (uid)
  {
    if (!InNameList(uid) && searchuser(uid))
      AddNameList(uid);
    uid = strtok(0, "/");       /* shakalaca.990721: 指向下一個 */
  }
  return 0;
}

/* shakalaca.990721: 所有 BM 名單 */
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
    brd_title = "徵求中";
  sprintf (currBM, "板主：%s", brd_title);
  brd_title = (bp->bvote == 1 ? "本看板進行投票中" : bp->title + 7);

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
[←]離開 [→]閱\讀 [^P]發表文章 [b]備忘錄 [d]刪除 [z]精華區 [TAB]文摘 [^Z]求助\n\
" COLOR1 "[1m  編號   ");
  if (currmode & MODE_TINLIKE)
    outs ("篇 數");
  else
    outs ("日 期");
  outs ("  作  者       文  章  標  題                                    [m");
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

    /* itoc.001203: 加密的文章前面出現 X 字樣 */
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
  /* shakalaca.990421: 改了一下, 因為 mail 中看起來不好看 */

  if (ent->filemode & FILE_TAGED)
    type = 't';

  title = str_ttl (mark = ent->title);
  if (title == mark)
    {
      color = '6';
      mark = "□";
    }
  else
    {
      color = '3';
      mark = "R:";
    }

  if (title[47])
    strcpy (title + 44, " …");  /* 把多餘的 string 砍掉 */
    
  if(bar==1)
    strcpy(outmsg, "●[1;33;44m");
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
/* 改良 innbbsd 轉出信件、連線砍信之處理程序             */
/* ----------------------------------------------------- */

outgo_post(fh, board)
  fileheader *fh;
  char *board;
{
  char buf[256];
  if(strcmp(fh->owner,cuser.userid))
    sprintf (buf, "%s\t%s\t%s\t%s\t%s", board,
      fh->filename, fh->owner, "轉錄", fh->title);
  else
    sprintf (buf, "%s\t%s\t%s\t%s\t%s", board,
      fh->filename, fh->owner, cuser.username, fh->title);

  f_cat ("innd/out.http", buf);
  f_cat ("innd/out.bntp", buf);
}

/* ----------------------------------------------------- */
/* 發表、回應、編輯、轉錄文章                            */
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
  sprintf (genbuf, "採用原標題《%.60s》嗎？ [Y] ", save_title);
  getdata (row, 0, genbuf, ans, 2, LCECHO, 0);
  if (*ans == 'n')
    getdata (++row, 0, "標題：", save_title, TTLEN, DOECHO, 0);
}


static void
do_reply (fhdr)
     fileheader *fhdr;
{
  char genbuf[200];

// Ptt 看板連署系統
  if(!strcmp(currboard,VOTEBOARD))
    DL_func("SO/votebrd.so:va_do_voteboardreply",fhdr);
  else
  {
    getdata (b_lines - 1, 0,
      "▲ 回應至 (F)看板 (M)作者信箱 (B)二者皆是 (Q)取消？[F] ",
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
      strcpy (mhdr.title, "[新]");
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
  {"[公告] ", "[通知] ", "[文件] ", "[問題] ", "[創作] ", "[新聞] ", "[閒聊] ",
   "[測試] ", "[申請] ",NULL};

//  bp = getbcache (currboard);

// 禁聲名單確認
  setbfile (buf, currboard, FN_LIST);
  if (!HAS_PERM (PERM_SYSOP) && brd_bad_guy (buf, cuser.userid))
  {
    pressanykey ("對不起，您被版主列入禁聲名單了...");
    return RC_FULL;
  }

  if ( !HAS_PERM (PERM_POST) || HAS_PERM (PERM_FORBIDPOST) || !(currmode & MODE_POST) || !brdperm (currboard, cuser.userid))
  {
    pressanykey ("對不起，您目前無法在此發表文章！");
    return RC_FULL;
  }

// Ptt 看板連署系統
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
      pressanykey ("對不起,此板只准看板好友才能發表文章,請向板主申請");
      return RC_FULL;
    }
  }
*/
  clear();
  more ("etc/post.note", NA);
  prints ("發表文章於【 %s 】看板\n", currboard);

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
    getdata (20, 0, "請選擇文章類別或自行輸入類別(或按Enter跳過)：", genbuf, 9, DOECHO, 0);
    i = atoi (genbuf);
    if (i > 0 && i <= num)  /* 輸入數字選項 */
      strncpy (save_title, postprefix[i - 1], strlen (postprefix[i - 1]));
    else if (strlen (genbuf) >= 3)  /* 自行輸入 */
      strncpy (save_title, genbuf, strlen (genbuf));
    else      /* 空白跳過 */
      save_title[0] = '\0';

    getdata (21, 0, "標題：", save_title, TTLEN, DOECHO, save_title);
    strip_ansi (save_title, save_title, ONLY_COLOR);
  }
  if (save_title[0] == '\0')
    return RC_FULL;

  curredit &= ~EDIT_MAIL;
  curredit &= ~EDIT_ITEM;
  setutmpmode (POSTING);

  /* 未具備 Internet 權限者，只能在站內發表文章 */

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
              [1;36m【[37m計 算 稿 酬[36m】\n
              [37m 這是您的[33m第 %d 篇[37m文章，\n
              [36m【費時】[33m %d [37m分[33m % d [37m秒，
              [36m【字數】[33m %d [37m字，
              [36m【稿酬】[33m %d [37m銀兩，
              [36m【經驗值】[33m %d [37m 點。",
       ++cuser.numposts, spendtime / 60, spendtime % 60, wordsnum, money, exp);
       cuser.exp+=exp;
       cuser.silvermoney+=money%10000;
       cuser.goldmoney+=money/10000;
//       UPDATE_USEREC;
       substitute_record (fn_passwd, &cuser, sizeof (userec), usernum);

        pressanykey ("文章發表完畢 :)");
        
        if(postfile.filemode & FILE_PROBLEM)
           security_check(postfile, "POST", wordsnum, spendtime, money, exp);
    }
    else
      pressanykey ("本看板文章不列入紀錄，敬請包涵。");

    log_board3("POS", currboard, cuser.numposts);

  /* 回應到原作者信箱 */
    if (curredit & EDIT_BOTH)
    {
      char *str, *msg = "回應至作者信箱";

      if (str = strchr (quote_user, '.'))
      {
        if (bbs_sendmail (fpath, save_title, str + 1) < 0)
          msg = "作者無法收信";
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
    do_all_post(fpath); // 紀錄所有站內的貼文
// wildcat : do_all_post 取代
//    if (currbrdattr & BRD_ANONYMOUS)  /*  反匿名  */
//      do_unanonymous_post (fpath);

// #ifdef POSTNOTIFY    /* 新文章通知 */
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

  /* itoc.001203: 加密的文章不能 edit */
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

  /* itoc.001203: 加密的文章不能轉錄 */
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
  namecomplete ("轉錄本文章於看板：", xboard);
  if (*xboard == '\0' || !haspostperm (xboard))
    return RC_FULL;

  ent = 1;
  if (HAS_PERM (PERM_SYSOP) || !strcmp (fhdr->owner, cuser.userid))
  {
    getdata (2, 0, "(1)原文轉載 (2)舊轉錄格式？[2] ",
    genbuf, 2, DOECHO, 0);
    if (genbuf[0] == '1')
    {
      ent = 0;
      getdata (2, 0, "保留原作者名稱嗎？ [Y] ", inputbuf, 2, DOECHO, 0);
      if (inputbuf[0] != 'n' && inputbuf[0] != 'N') author = 1;
    }
  }

  if (ent)
    sprintf (xtitle, "[轉錄] %.66s", fhdr->title);
  else
    strcpy (xtitle, fhdr->title);

//  sprintf (genbuf, "採用原標題《%.60s》嗎？ [Y] ", xtitle);
//  getdata (2, 0, genbuf, genbuf2, 4, LCECHO, 0);
//  if (*genbuf2 == 'n')
//  {
//    if (getdata (2, 0, "標題：", genbuf, TTLEN, DOECHO, xtitle))
//      strcpy (xtitle, genbuf);
//  }

  getdata (2, 0, "(S)存檔 (L)站內 (Q)取消？ [S] ", genbuf, 2, LCECHO, "S");
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

      fprintf (xptr, "※ [本文轉錄自 %s 看板]\n\n", currboard);

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
    pressanykey ("文章轉錄完成");
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

  /* itoc.001203: 加密的文章只有原作者以及板主能閱讀 */

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
/* 採集精華區                                            */
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
    namecomplete ("輸入看版名稱 (直接 Enter 進入私人信件夾)：", buf);
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
//      sprintf (buf1, "%s 的信件夾", cuser.userid);
//      setutmpmode (ANNOUNCE);
      setutmpmode(RMAIL);
      a_menu ("私人信件夾", buf, 2);
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
   strcpy (item.title, "◆ ");
   strcpy (item.owner, cuser.userid);
   item.filemode = MAIL_ALREADY_READ;

  if (!getdata (b_lines - 1, 1, "[新增目錄] 請輸入標題：",
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
  
  strcpy (title, "◇ ");
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
  
  /* itoc.001203: 加密的文章只有原作者以及板主能閱讀 */
  
  if (fhdr->filemode & FILE_REFUSE &&
      !(HAS_PERM (PERM_BM) && is_BM (currBM + 6)) &&
      strcmp(cuser.userid, fhdr->owner))
      return RC_FULL;                          

  strcpy (title, "◇ ");
  strncpy (title + 3, fhdr->title, TTLEN - 3);
  
  // sprintf (title, "%s%.72s",(currutmp->pager > 1) ? "" : "◇ ", fhdr->title);
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
  
  /* itoc.001203: 加密的文章只有原作者以及板主能閱讀 */
  
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
/* shakalaca.990714: 將檔案獨立
    unlink (newpath); */
    f_cp (fpath, newpath, O_TRUNC);
    strcpy (fh.owner, cuser.userid);
    sprintf (fh.title, "%s%.72s",
    (currutmp->pager > 1) ? "◇ " : "◇ ", fhdr->title);
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

    if (getdata (b_lines - 1, 0, "標題：", genbuf, TTLEN, DOECHO, tmpfhdr.title))
    {

      strip_ansi (genbuf, genbuf, 0);

      strcpy (tmpfhdr.title, genbuf);
      strcpy (save_title, genbuf);
      dirty++;
    }

    if(HAS_PERM (PERM_SYSOP))
    {
      if (getdata (b_lines - 1, 0, "作者：", genbuf, IDLEN + 2, DOECHO, tmpfhdr.owner))
      {
        strcpy (tmpfhdr.owner, genbuf);
        dirty++;
      }

      if (getdata (b_lines - 1, 0, "日期：", genbuf, 6, DOECHO, tmpfhdr.date))
      {
        sprintf (tmpfhdr.date, "%+5s", genbuf);
        dirty++;
      }
    }

    if (getdata (b_lines - 1, 0, "確定(Y/N)?[n] ", genbuf, 3, DOECHO, 0) &&
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
    getdata (1, 0, "確定刪除標記信件(Y/N)? [N]", ans, 2, LCECHO, 0);
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
       
    getdata (1, 0, "確定刪除標記文章(Y/N)? [N]", ans, 3, LCECHO, 0);
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
      pressanykey("刪到自己的文章共有 %d 篇",payback);
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

  load_paste(); //讀入 paste_file 來定位
  if(!*paste_path)
  {
    pressanykey("尚未定位,請進入精華區中你想收錄的目錄按 [P]");
    return RC_FOOT;
  }

  if (currstat == RMAIL)
  {
    getdata (1, 0, "確定收錄標記信件(Y/N)? [Y]", genbuf, 3, LCECHO, 0);
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
    getdata (1, 0, "確定收錄標記文章(Y/N)? [N]", genbuf, 3, LCECHO, 0);
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

/* itoc.001203: 文章加密 */
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
   showtitle("編特別名單", currboard);
   setbfile(buf,currboard,fhdr->filename);
   strcat(buf,".X\0");
   
   getdata(1,0,"編輯(E) 刪除(D) 沒事[Q] ",ans,2,LCECHO,0);
   
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
    getdata (1, 0, "[設定刪除範圍] 起點：", num1, 5, DOECHO, 0);
    inum1 = atoi (num1);
    if (inum1 <= 0)
    {
      outmsg ("起點有誤");
      refresh ();
      sleep (1);
      return RC_FOOT;
    }
    getdata (1, 28, "終點：", num2, 5, DOECHO, 0);
    inum2 = atoi (num2);
    if (inum2 < inum1)
    {
      outmsg ("終點有誤");
      refresh ();
      sleep (1);
      return RC_FULL;
    }
    getdata (1, 48, msg_sure_ny, num1, 3, LCECHO, 0);
    if (*num1 == 'y')
    {
      outmsg ("處理中,請稍後...");
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
//     pressanykey("刪到自己的文章有 %d 篇",i);
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

  /* itoc.001203: 加密的文章不能刪除 */
//  if (fhdr->filemode & (FILE_MARKED | FILE_DIGEST | FILE_REFUSE))
//     return RC_NONE;

  not_owned = strcmp (fhdr->owner, cuser.userid);
  
  // wildcat : 站長可以連線砍信
//  if (HAS_PERM(PERM_SYSOP) && answer("是否要連線砍信(Y/N) [N]") == 'y')
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
          
        pressanykey ("%s，您的文章減為 %d 篇，支付清潔費 %d 金", msg_del_ok,
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
/* 看板備忘錄、文摘、精華區                              */
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
//    "看板 1)買上限 2)改中文名稱 3)看板說明 4)進版畫面 5)可見名單 6)設密碼 %s:",
    "看板 1)改中文名稱 2)進版畫面 3)可見名單 4)設密碼 ");
//    mode ? "5)進階" : "");

    getdata (1, 0, buf, genbuf, 2, DOECHO, 0);
    switch(genbuf[0])
    {
/*
      case '1':
        clrchyiuan(1, 15);
        move(3, 0);
        prints("目前看板的文章上限為 %-5d 篇"
               "保留時間為 %-5d 天\n",
               bp.maxpost,bp.maxtime);
               
        outs("一個單位為[1;32m一百篇文章[m或是[1;32m三十天[m ,"
             "一個單位需 [1;33m3000 金幣[m");
         
    getdata(7, 0, "你要 (1)買文章上限 (2)買保存時間", buf, 2, DOECHO, 0);
    if (buf[0] == '1' || buf[0] == '2')
    {
      int num = 0;

          while (num <= 0)
          {
            getdata(9, 0, "你要買幾個單位", genbuf, 3, DOECHO, 0);
            num = atoi(genbuf);
          }

          if (check_money(num * 3000, GOLD))
            break;

          if (buf[0] == '1')
          {
            if (bp.maxpost >= 99999)
            {
              pressanykey("文章數已達上限");
              break;
            }
            else
            {
              bp.maxpost += num*100;
              sprintf(buf, "%-13.13s 購買看板 %s 文章上限 %d 篇 , %s",
                cuser.userid, bp.brdname,num*100, ctime(&now));
              f_cat(BBSHOME"/log/board_edit", buf);
              log_usies ("BuyPL", currboard);
              pressanykey("看板文章上限增加為 %d 篇", bp.maxpost);
            }
          }
          else
          {
            if (bp.maxtime >= 9999)
            {
              pressanykey("保存時間已達上限");
              break;
            }
            else
            {
              bp.maxtime += num * 30;
              sprintf(buf,"%-13.13s 購買看板 %s 文章保留時間 %d 天 , %s",
                cuser.userid,bp.brdname,num*30,ctime(&now));
              f_cat(BBSHOME"/log/board_edit",buf);
              log_usies ("BuyBT", currboard);
              pressanykey("看板文章保留時間增加為 %d 天",bp.maxtime);
            }
          }
          degold(num*3000);
        }
        break;
*/
      case '1':
        move (1, 0);
        clrtoeol ();
        getdata (1, 0, "請輸入看板新中文敘述:"
          ,genbuf, BTLEN - 16, DOECHO, bp.title + 7);
        if (!genbuf[0]) return 0;
        strip_ansi (genbuf, genbuf, 0);
        if(strcmp(bp.title+7,genbuf))
        {
          sprintf(buf,"%-13.13s 更換看板 %s 敘述 [%s] -> [%s] , %s",
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
        outs("對本看板的描述 (共三行) :");
        getdata (2, 0, ":", bp.desc[0], 79, DOECHO, bp.desc[0]);
        getdata (3, 0, ":", bp.desc[1], 79, DOECHO, bp.desc[1]);
        getdata (4, 0, ":", bp.desc[2], 79, DOECHO, bp.desc[2]);
        sprintf(buf,"%-13.13s 更換看板 %s 說明 , %s",
          cuser.userid,bp.brdname,ctime(&now));
        f_cat(BBSHOME"/log/board_edit",buf);
        log_usies ("SetBoardDesc", currboard);
        break;
*/
      case '2':
        setbfile (buf, currboard, fn_notes);
        if (vedit (buf, NA) == -1)
        {
          getdata(3,0,"需要刪除檔案嗎？ (y/N) ", genbuf, 2, LCECHO, "N");
          
          if(genbuf[0]=='y')
            f_rm(buf);
          pressanykey(NULL);
        }
//        else
//        {
//          int aborted;
//          getdata (3, 0, "請設定有效期限(0 - 9999)天？", buf, 5, DOECHO,"9999");
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
          if(!getdata (1, 0, "請輸入原本的密碼" ,genbuf, PASSLEN, PASS, 0)
             ||
             !chkpasswd(bp.passwd, genbuf))
             {
               pressanykey("密碼錯誤");
               return -1;
             }
        }
        if (!getdata(1, 0, "請設定新密碼：", genbuf, PASSLEN, PASS,0))
        {
          pressanykey("密碼設定取消, 繼續使用舊密碼");
          return -1;
        }
        strncpy(buf, genbuf, PASSLEN);

        getdata(1, 0, "請檢查新密碼：", genbuf, PASSLEN, PASS,0);
        if (strncmp(genbuf, buf, PASSLEN))
        {
          pressanykey("新密碼確認失敗, 無法設定新密碼");
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
          getdata(1, 0, "您要 1)改屬性 2)改類別 3)看今日記錄:", genbuf, 2,DOECHO, 0);
          switch(genbuf[0])
          {
          case '1':
          {
            int oldattr=bp.brdattr;
            ans = answer("看板狀態更改為 (o)開放 (p)私人 (h)隱藏?");
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
              if(answer("是否要讓 guest 看到你的板? (Y/n)") == 'n')
                bp.brdattr &= ~BRD_POSTMASK;
              else
                bp.brdattr |= BRD_POSTMASK;
              bp.brdattr &= ~BRD_HIDE;
            }
            if(bp.brdattr != oldattr)
            {
              sprintf(buf,"%-13.13s 更改看板 [%s] 之屬性為 %s , %s",
                cuser.userid,bp.brdname,
                ans == 'p' ? "私人" : ans == 'h' ? "隱藏" :"開放",ctime(&now));
              f_cat(BBSHOME"/log/board_edit",buf);
              log_usies("ATTR_Board",currboard);
            }
          }
          break;

          case '2':
            move (1, 0);
            clrtoeol ();
            getdata (1, 0, "請輸入看板新類別:",genbuf, 5, DOECHO, bp.title);
            if (!genbuf[0]) return 0;
            strip_ansi (genbuf, genbuf, 0);
            if(strncmp(bp.title,genbuf,4))
            {
              sprintf(buf,"%-13.13s 更換看板 %s 類別 [%-4.4s] -> [%s] ,%s",
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
        pressanykey("放棄修改");
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
       getdata(2,0,"需要刪除檔案嗎？ (y/N) ", ans, 2, LCECHO, "N");
       if(ans[0]=='y')
          f_rm(buf);
       pressanykey(NULL);
    }
//      pressanykey (msg_cancel);
//    else
//    {
//      boardheader fh;
//      int pos;
//      getdata (3, 0, "請設定有效期限(0 - 9999)天？", buf, 5, DOECHO, "9999");
//      aborted = atol (buf);
//      pos = rec_search (fn_board, &fh, sizeof (fh), cmpbnames, (int) currboard);
   /* SiE: 這裡與rec_search的傳入值型態不同..應該是爆調的原因 */
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
    prints ("看板名稱：%-15s 板主名單：%s\n看板說明：%-49s 文章上限：%d %s\n",
       bh.brdname, bh.BM, bh.title, bh.postotal,
       (bh.brdattr & BRD_OUTGOING && bh.brdattr & BRD_INGOING) ? "x2" : "");
    {
      int t = bh.totaltime;
      int day=t/86400, hour=(t/3600)%24, min=(t/60)%60, sec=t%60;
      avg = bh.totalvisit != 0 ? bh.totaltime / bh.totalvisit : 0;
      prints("開板時間：%s\n", Ctime(&bh.opentime));
      prints("拜訪人數：%-15d拜訪總時：%d 天 %d 小時 %d 分 %d 秒 平均停留：%d秒\n",
             bh.totalvisit, day, hour, min, sec, avg);
    }
    prints ("最新訪客：%-15s最新時間：%s\n"
          ,bh.lastvisit, Ctime (&bh.lastime));
//    bperm_msg (&bh);
    prints ("%s設限\n可zap:%s 列入統計:%s "
            "轉出:%s 轉入:%s 群組:%s 私人:%s 隱藏:%s 信箱:%s 優良:%s 個人:%s",
            bh.level ? "有" : "不",
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
             
    getdata (9, 0, "是否更改看板狀態? (y/N) ", genbuf, 3, LCECHO, "y");
    if(genbuf[0]=='y')
    {
      oldattr=bh.brdattr;
      getdata (10, 0, "請輸入看板新中文敘述:"
               ,genbuf, BTLEN - 13, DOECHO, bh.title + 7);

      if (genbuf[0])
      {
        strip_ansi (genbuf, genbuf, 0);
        if(strcmp(bh.title+7,genbuf))
        {
          sprintf(buf,"%s %-13.13s 更換看板 %s 敘述 [%s] -> [%s]",
          ctime(&now),cuser.userid,bh.brdname,bh.title+7,genbuf);
          f_cat(BBSHOME"/log/board_edit.log",buf);
          strcpy (bh.title + 7, genbuf);
        }
      }
      
      if(!(bh.brdattr & BRD_CLASS) && !(bh.brdattr & BRD_GROUPBOARD))
      {
      
         getdata (11, 0, "轉信屬性: 1.站內 2.轉出 3.轉入 4.轉入與轉出 ",genbuf, 2, LCECHO, 0);
         if(genbuf[0])
         switch(genbuf[0])
         {
           case '1':
           bh.brdattr &= ~BRD_OUTGOING;
           bh.brdattr &= ~BRD_INGOING;
        strncpy(bh.title+5,"○",2);
        break;
           case '2':
           bh.brdattr |= BRD_OUTGOING;
           bh.brdattr &= ~BRD_INGOING;
        strncpy(bh.title+5,"☉",2);
        break;
           case '3':
           bh.brdattr |= BRD_INGOING;
           bh.brdattr &= ~BRD_OUTGOING;
        strncpy(bh.title+5,"◎",2);
        break;
           case '4':
           bh.brdattr |= BRD_OUTGOING;
           bh.brdattr |= BRD_INGOING;
        strncpy(bh.title+5,"●",2);
        break;
         }
      }

      if(bh.brdattr & BRD_PERSONAL)
      {
        getdata(12,0,"看板屬性: 1.開放 2.私人 3.隱藏? ",genbuf,2,LCECHO, 0);
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
          sprintf(buf,"%s %-13.13s 更改看板 [%s] 之屬性為 %s .",
            ctime(&now),cuser.userid,bh.brdname,
            genbuf[0] == '1' ? "私人" : genbuf[0] == '2' ? "隱藏" : "開放");
          f_cat(BBSHOME"/log/board_edit.log",buf);
        }
      }
      
      getdata(13, 0, "看板信箱: 1.關閉 2.開放 ", genbuf, 2, LCECHO, 0);
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
      getdata (1, 0, "請輸入看板新類別:",genbuf, 5, DOECHO, bp.title );
      
      if (!genbuf[0]) return 0;
      
      strip_ansi (genbuf, genbuf, 0);
      if(strncmp(bp.title,genbuf,4))
      {
        sprintf(buf,"%s %-13.13s 更換看板 %s 類別 [%s] -> [%s]",
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
    
    getdata (b_lines, 0, "請輸入看板新中文敘述:"
      ,genbuf, BTLEN - 13, DOECHO, bp.title + 7);
      
    if (!genbuf[0]) return 0;
    
    strip_ansi (genbuf, genbuf, 0);
    if(strcmp(bp.title+7,genbuf))

    {
      sprintf(buf,"%s %-13.13s 更換看板 %s 敘述 [%s] -> [%s]",
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
      if(!getdata (1, 0, "請輸入原本的密碼" ,genbuf, PASSLEN, PASS, 0) ||
         !chkpasswd(bp.passwd, genbuf))
         {
           pressanykey("密碼錯誤");
           return -1;
         }
    }
    if (!getdata(1, 0, "請設定新密碼：", genbuf, PASSLEN, PASS,0))
    {
      pressanykey("密碼設定取消, 繼續使用舊密碼");
      return -1;
    }
    strncpy(buf, genbuf, PASSLEN);

    getdata(1, 0, "請檢查新密碼：", genbuf, PASSLEN, PASS,0);
    if (strncmp(genbuf, buf, PASSLEN))
    {
      pressanykey("新密碼確認失敗, 無法設定新密碼");
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
  if (more (buf, YEA) == -1) // 可以看多頁
  {
    clear ();
    pressanykey ("本看板尚無「備忘錄」。");
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

  /* shakalaca.000112: 超過 30min 才將 index 刪除, 作 cache 用 */
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
/* 看板功能表                                            */
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
    pressanykey ("對不起, 此板只准看板好友進入, 請向板主申請入境許\可");
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

// shakalaca.010221: 刪除文章, 判斷是否為轉出而送出 cancel message.
// net 決定是否要送出 message (by sysop), by_BM 決定要送入 jun or delete
// 註: 預設的存檔 Local or Net 得多加上看板的設定判斷:)

int
cancel_post(fh, not_owned, net_del)
  fileheader *fh;
  int not_owned, net_del;
{
  char *brd;
  int fd;
  char fn[256], fpath[256], buf[256];
  fileheader backfile;

  //  shakalaca.010221: 如果是 BM 放入 deleted, 否則放入 junk
  //  brd = not_owned ? "deleted" : "junk";
  
  brd = HAS_PERM(PERM_SYSOP) ? "junk" : 
        (net_del==2) ? "Temp" : "deleted";  
  
  // shakalaca.010221: 設定看板路徑
  setbfile (fn, brd, fh->filename);
  // shakalaca.010221: 設定檔案存在路徑
  setbfile(fpath, currboard, fh->filename);
  // shakalaca.010221: 把原始檔案移到 junk or deleted 下面.
  f_mv (fpath, fn);

  strcat(fpath,".X\0");
  f_rm (fpath);
        
  // shakalaca.010221: 在 fn 之下建立一個 timestamp 檔案, 並將 fn
  // 改為該檔案的路徑 (相對於 BBSHOME)
  //  stampfile (fn, &backfile);

  // shakalaca.010221: 將 touch 的檔案刪除, 否則會有 bug ?
  //  unlink(fn);

  // shakalaca.010221: 將原始檔案的作者, 標題, date 及 filemode 一並存入
  // 作者: IDLEN + 2  標題: TTLEN + 1  date: 6  filemode: 1

  memcpy (&backfile, fh, sizeof(fileheader));

  if(backfile.filemode & FILE_PROBLEM)
     security_check(fh,"DELETE",0,0,0,0);
  
  if(backfile.filemode & FILE_REFUSE)
     strcpy(backfile.owner, cuser.userid);

  log_board3("DEL", currboard, 1);

  // shakalaca.010221: 在該目錄下建立 .DIR 對映
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
/* 離開 BBS 站                                           */
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
    outs("\n請留言 (至多三行)，按[Enter]結束");
    for (i = 0; (i < 3) &&
      getdata(16 + i, 0, "：", myitem.buf[i], 78, DOECHO,0); i++);
    getdata(b_lines - 1, 0, "(S)儲存 (E)重新來過 (Q)取消？[S] ", buf, 3, LCECHO,"S");
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

  fputs("[1m                             "COLOR1" [33m◆ [37m心 情 留 言 板 [33m◆ [0m \n\n",fp);
  collect = 1;

  while (total)
  {
    sprintf(buf, "[1;32m□?? [33m%s[37m(%s)",
      myitem.userid, myitem.username);
    len = strlen(buf);
    strcat(buf," [32m" + (len&1));

    for (i = len >> 1; i < 36; i++)
      strcat(buf, "??");
    sprintf(buf2, "??[33m %.14s [32m?龤?[0m\n",
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
    prints("%16s   %-16s權責劃分\n", "編號", "站長 ID"/*, msg_seperator*/);

    for (i = 0; i < j; i++)
    {
      prints("%15d.   [1;%dm%-16s%s[0m\n",
        i + 1, 31 + i % 7, sysoplist[i].userid, sysoplist[i].duty);
    }
    prints("%-14s0.   [1;%dm離開[0m", "", 31 + j % 7);
    getdata(b_lines - 1, 0, "                   請輸入代碼[0]：", genbuf, 4, DOECHO,"1");
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

  getdata(b_lines, 0, "您確定要離開〈" BOARDNAME "〉嗎(Y/N)？[N] ",
    genbuf, 3, LCECHO,0);

  if (*genbuf != 'y')
    return 0;
//  movie(999);
  if (cuser.userlevel)
  {
//    getdata(b_lines, 0, "(G)悄悄離去 (M)托夢站長 (N)刻留言板 [G] ",
    getdata(b_lines, 0, "(0)寫寫留言板囉 (1)讓我悄悄的走吧 ",
      genbuf, 3, LCECHO,0);
    if (genbuf[0] == 'm')
      m_sysop();
    else if (genbuf[0] == '0' || genbuf[0] == 'n')
      note();
  }

  t_display();
  clear();
  prints("[1;31m親愛的 [31m%s([37m%s)[31m，別忘了再度光臨"COLOR1
    " %s [40;33m！\n\n以下是您在站內的註冊資料:[m\n",
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
