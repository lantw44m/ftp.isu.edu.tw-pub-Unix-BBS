/*-------------------------------------------------------*/
/* vote.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : boards' vote routines                        */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

extern cmpbnames();
extern int numboards;
// extern void friend_edit(int type);
extern cmpfilename();

/* .vch : vote control header, 甅ノ fileheader,  i_read 俱,
	  丁羭︽щ布. :)
struct fileheader
{
  char filename[FNLEN];          V.[closetime].A
  char savemode;                 щ程布计
  char owner[IDLEN + 2];         votetime
  char date[6];                  opentime[xx/xx]
  char title[TTLEN + 1];         vote_title
  uschar filemode;               Vote_way : , ╬, ゴだ, etc.
  					     1     2     3     4
  				VOTE_NORMAL	0x01	
  				VOTE_PRIVATE	0x02	╬
  				VOTE_SCORE	0x04	ゴだ
  				VOTE_PAPER	0x08	拜
};
*/                           

#define FHSZ	sizeof(fileheader)
#define VOTE_NORMAL	0x01
#define VOTE_PRIVATE	0x02
#define VOTE_SCORE	0x04
#define VOTE_PAPER	0x08	/* shakalaca.991126: 痙倒窟.. :p */

static char STR_bv_control[] = ".control";	/* щ布ら戳 匡兜 */
static char STR_bv_ballots[] = ".ballots";	/* щ筁布 */
static char STR_bv_results[] = ".results";	/* щ布挡狦 */
static char STR_bv_flags[] = ".flags";		/* Τ礚щ筁布 */
static char STR_fn_vote_polling[] = ".voting";
static char STR_bv_comments[] = ".comments";	/* щ布種 */
/* static char STR_bv_desc[] = "desc"; */       /* щ布ヘ */
/*  V.[closetime].A  */

boardheader *bcache;

void
b_closepolls()
{
  boardheader fh;
  struct stat st;
  FILE *cfp;
  time_t now;
  int fd, dirty;
  char currboard0[15];

  now = time(0);

  if (stat(STR_fn_vote_polling, &st) == -1/* || st.st_mtime > (now - 3600)*/)
  {
    pressanykey("No Such File : .voting !");
    return;
  }

  if ((cfp = fopen(STR_fn_vote_polling, "w")) == NULL)
  {
    pressanykey("Can't open!");
    return;
  }
  fprintf(cfp, ctime(&now));
  fclose(cfp);

  resolve_boards();
  if ((fd = open(fn_board, O_RDWR)) == -1)
  {
    pressanykey(".BOARDS 秨币岿粇");
    return;
  }

  flock(fd, LOCK_EX);

  sprintf(currmsg, "╰参秨布い, 叫祔...");
  kill(currpid, SIGUSR2);

  strcpy(currboard0, currboard);
  dirty = 0;
  while (read(fd, &fh, sizeof(fh)) == sizeof(fh))
  {
    if (fh.bvote && b_close(&fh))
    {
      lseek(fd, (off_t) sizeof(fh) * (getbnum(fh.brdname) - 1), SEEK_SET);
      if (write(fd, &fh, sizeof(fh)) != sizeof(fh))
      {
        sprintf(currmsg, "\033[1;5;37;41mWarning!\033[m");
        kill(currpid, SIGUSR2);
        igetch();
        break;
      }
      dirty = 1;
    }
  }
  if (dirty) /* vote flag changed */
    touch_boards();

  flock(fd, LOCK_UN);
  close(fd);

  strcpy(currboard, currboard0);
  return;
}                       


time_t
next_closetime(direct, closetime)
  char *direct;
  time_t closetime;
{
  int num;
  fileheader vfh;

  if (!(num = rec_num(direct, FHSZ)))
    return closetime;
  else
  {
    num = open(direct, O_RDONLY);
    if (read(num, &vfh, FHSZ) == FHSZ)
    {
      closetime = atol(vfh.filename + 2);
      while(read(num, &vfh, FHSZ) == FHSZ)
      {
        if (closetime > atol(vfh.filename + 2))
          closetime = atol(vfh.filename + 2);
      }
    }
    close(num);                                              
    return closetime;
  }
}


void
delete_vote_files(ent, fhdr, direct, fpath)
  int ent;
  fileheader *fhdr;
  char *direct, *fpath;
{
  int len;

  setbfile(fpath, currboard, fhdr->filename);
  len = strlen(fpath);

  strcpy(fpath + len, STR_bv_ballots);
  unlink(fpath);
  strcpy(fpath + len, STR_bv_control);
  unlink(fpath);
  strcpy(fpath + len, STR_bv_flags);
  unlink(fpath);
  strcpy(fpath + len, STR_bv_comments);                                        
  unlink(fpath);
  *(fpath + len) = 0;
  unlink(fpath);

  strcpy(currfile, fhdr->filename);
  delete_file(direct, FHSZ, ent, cmpfilename);
}


void
result_post(fhdr, bname, fpath)
  fileheader *fhdr;
  char *bname, *fpath;
{
  char distpath[STRLEN];
  fileheader postfile;
  FILE *fp;

  setbpath(distpath, bname);
  memset(&postfile, 0, FHSZ);
  stampfile(distpath, &postfile);

  postfile.savemode = 'L';                               
  strcpy(postfile.owner,"[皑隔贝]");
  snprintf(postfile.title, TTLEN, "[%s 狾秨布挡狦] <%s>",
    currboard, fhdr->title);

  fp = fopen(distpath, "a");
  b_suckinfile(fp, fpath);
  fclose(fp);

  setbdir(distpath, bname);
  rec_add(distpath, &postfile, FHSZ);
}                        


void
make_results(fhdr, counts, buf, fpath, interrupt)
  fileheader *fhdr;
  int *counts, interrupt;	/* いゎ */
  char *buf, *fpath;
{
  FILE *fout, *fin;
  int len, total = 0, num;
  time_t votetime, closetime;
  int fd, n;
  char inbuf[256];

  setbfile(fpath, currboard, fhdr->filename);
  strcpy(buf, fpath);
  len = strlen(fpath);

  strcpy(fpath + len, STR_bv_results);
  fout = fopen(fpath, "w");

  votetime = (time_t)atol(fhdr->owner);
  if (interrupt)
    time(&closetime);
  else
    closetime = atol(fhdr->filename + 2);
  fprintf(fout, "\n%s\n』 щ布秨﹍: %s", MSG_SEPERATOR, Ctime(&votetime));
  fprintf(fout, "\n』 %s: %s\n』 布匡肈/磞瓃: %-41.40s\n\n",
    interrupt ? "矗Ν秨布":"щ布沧ゎ", Ctime(&closetime), fhdr->title);

  b_suckinfile(fout, buf);	/* 弧 */

  fprintf(fout, "\n』 щ布挡狦:\n\n");

  strcpy(buf + len, STR_bv_ballots);
  total = dashs(buf) / 2;	/* 羆布计 */

  strcpy(buf + len, STR_bv_control);
  if (fin = fopen(buf, "r"))
  {
    int count = 0;
    
    while (fgets(inbuf, STRLEN, fin))
    {
      inbuf[(strlen(inbuf) - 1)] = '\0';
      num = counts[count];
      count++;
      fprintf(fout, "   %-51.50s %3d %s (%-02.2f %%)\n", inbuf + 3, num,
        (fhdr->filemode & VOTE_SCORE) ? "だ" : "布",
        total ? (float) 100 * num/total : 0 );
    }
    fclose(fin);
  }

  if (!(fhdr->filemode & VOTE_SCORE))
    fprintf(fout, "\n』 羆布计 = %d 布", total);

  setbfile(buf, currboard, fhdr->filename);
  strcpy(buf + len, STR_bv_flags);

  if ((fd = open(buf, O_RDONLY)) > 0)
  {
    total = 0;
    while ((n = read(fd, inbuf, 256)) == 256)
    {
      while(n)
      {
        if (inbuf[n---1])
          total++;
      }
    }
    close(fd);
  }
  else
    total = -1;

  fprintf(fout, "\n』 羆计 = %d \n", total);

  fprintf(fout, "\n』 иΤ杠璶弧:\n\n");
  strcpy(buf + len, STR_bv_comments);
  b_suckinfile(fout, buf);
  fprintf(fout, "\n\n");
  fclose(fout);

  result_post(fhdr, currboard, fpath);
  if (strcmp(currboard, "Record"))
    result_post(fhdr, "Record", fpath);

  fout = fopen(fpath, "a");
  setbfile(buf, currboard, STR_bv_results);
  b_suckinfile(fout, buf);
  fclose(fout);
  f_mv(fpath, buf);
}


int
b_close(bh)
  boardheader *bh;
{
  char buf[STRLEN], fpath[STRLEN], inchar[2];
  int counts[100], fd, ent=1;
  time_t now;
  fileheader vfh;

  now = time(NULL);

  if (bh->bvote == 2)
  {
    if (bh->vtime < now - 7 * 86400) /* 秨布琍戳 */
    {
      bh->bvote = 0;
      return 1;
    }
    else
      return 0;
  }

  if (bh->vtime > now)
  {
    bh->bvote = 1;
    return 0;
  }

  setbfile(buf, bh->brdname, FN_VOTE);
  if ((fd = open(buf, O_RDONLY)) == -1)
    return 0;

  while (read(fd, &vfh, FHSZ) == FHSZ)
  {
    if (bh->vtime == atol(vfh.filename + 2))         /* read vote data(title... */
      break;
    else
      ent++;                            /* count entry */
  }

  close(fd);

  setbfile(fpath, bh->brdname, vfh.filename);
  memset(counts, 0, sizeof(counts));
  strcpy(fpath+strlen(fpath), STR_bv_ballots);
  if ((fd = open(fpath, O_RDONLY)) != -1)
  {
    while (read(fd, &inchar, 2) == 2)
      counts[atoi (inchar) - 1]++;
    close(fd);
  }
  strcpy(currboard, bh->brdname);
  make_results(&vfh, &counts, buf, fpath, 0);
  setbfile(buf, bh->brdname, FN_VOTE);
  delete_vote_files(ent, &vfh, buf, fpath);

  if ((bh->vtime = next_closetime(buf, atol(vfh.filename + 2))) == atol(vfh.filename + 2))
  {
    setbfile(fpath, bh->brdname, "V.*");
    snprintf(buf, STRLEN, "/bin/rm -f %s", fpath);
    system(buf);
    bh->bvote = 2;
  }
  return 1;
}


int
vote_flag(fhdr, val)
  fileheader *fhdr;
  char val;
{
  char buf[256], flag;
  int fd, num, size;

  setbfile(buf, currboard, fhdr->filename);
  strcpy(buf + strlen(buf), STR_bv_flags);

  if ((fd = open(buf, O_RDWR | O_CREAT, 0600)) == -1)
  {
    return -1;
  }
  num = usernum - 1;
  size = lseek(fd, 0, SEEK_END);
  memset(buf, 0, sizeof(buf));
  while (size <= num)
  {
    write(fd, buf, sizeof(buf));
    size += sizeof(buf);
  }
  lseek(fd, num, SEEK_SET);
  read(fd, &flag, 1);

  if (flag == 0 && val != 0)
  {
    lseek(fd, num, SEEK_SET);
    write(fd, &val, 1);
  }
  close(fd);
  return flag;
}


int
vote_results(bname)
  char *bname;
{
  char buf[STRLEN];

  setbfile(buf, bname, STR_bv_results);
  if (more(buf, YEA) == -1)
  {
    pressanykey("ヘ玡⊿Τヴщ布挡狦");
    return RC_DRAW;
  }
  return RC_FULL;
}


int
same(compare, list, num, rep)
  int compare;
  int list[];
  int num;
  int rep;
{
  int n;
  int tmp = 0;

  for (n = 0; n < num; n++)
  {
    if (compare == list[n])
      tmp = 1;
    if (rep && tmp == 1)
      list[n] = list[n+1];
  }
  return tmp;
}                        


static int
do_vote(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  FILE *fp;
  char buf[STRLEN], fpath[STRLEN], vote[3], inbuf[STRLEN];
  int i = 0, len, choice = 0, page = 0, count = 0, invote, bufvote[100], score[100];

  setbfile(fpath, currboard, fhdr->filename);
  len = strlen(fpath);

  if (!HAS_PERM(PERM_BASIC))
  {
    pressanykey("癸ぃ癬! 眤临⊿Τщ布舦翅!");
    return RC_DRAW;
  }

  if (cuser.totaltime < 86400/6)
  {
    pressanykey("癸ぃ癬! 璶禬筁 4 щ布翅!");

/*  shakalaca.000108: bug 碞琌. :p
    wildcat : 或穦Τ fclose  open ぇ玡 -_-
    btw,эΘノtotaltime衡 ...
    fclose(fp); 
*/
    return RC_FULL;
  }              

  if (fhdr->filemode & VOTE_PRIVATE)
  {
    setbfile(buf, currboard, FN_CANVOTE);
    if (!belong(buf, cuser.userid))
    {
      pressanykey("癸ぃ癬! 硂琌╬щ布..⊿Τ淋!");
      return RC_FULL;
    }
    else
    {
      pressanykey("尺淋Ω╬щ布....");
    }
  }

  if (vote_flag(fhdr, NULL))
  {
    pressanykey("Ωщ布眤щ筁Ω產キ单");
    return RC_FULL;
  }

  strcpy(fpath + len, STR_bv_control);
  fp = fopen(fpath, "r");
  if (fp == NULL)
  {
    pressanykey("北郎框ア! 叫恨瞶.");
    fclose(fp);
    return RC_FOOT;
  }

  setutmpmode(VOTING);

  *(fpath + len) = 0;
  more(fpath, YEA);

  memset(bufvote, 0, sizeof(bufvote));
  memset(score, 0, sizeof(score));
  while (page < 3)
  {
    stand_title("щ布絚");
    rewind(fp);

    count = 0;
    
    if (fhdr->filemode & VOTE_SCORE)
      prints("\033[1;32m璸だよΑ絋﹚眤匡拒块ㄤ絏(1, 2, 3...)倒だ计\033[0m\n"
      "\033[1;37;44m だ计絛瞅 1~10 だ ,  [F] 挡щ布 \033[0m");
    else
      prints("\033[1;32m璸布よΑ絋﹚眤匡拒块ㄤ絏(1, 2, 3...)\033[0m\n"
      "\033[1;37;44m  [F] 挡щ布 \033[0mΩщ布щ %1d 布", 
      fhdr->savemode);

    while (fgets(inbuf, sizeof(inbuf), fp))
    {
      count++;
      if (count > 34 * page && count <= 34 * (page + 1))
      {
        move(((count - 1 - 34 * page) % 17) + 4,((count-1-34*page)/17)*40);

        if (fhdr->filemode & VOTE_SCORE)
        {
          int n, num = -1;

          inbuf[strlen(inbuf) - 1] = 0;
          for (n = 0; n < i; n++)
          {
            if (count == score[n])
            {
              num = n;
              break;
            }
          }
          
          if (num >= 0)
            prints(" %-31.30s %2d だ", inbuf, bufvote[num]);
          else
            prints(" %-31.30s  0 だ", inbuf);
        }
        else          
        {
          if (same(count, bufvote, i, 0))
            prints( "*%s", strtok(inbuf,"\n\0"));
          else
            prints( " %s", strtok(inbuf,"\n\0"));
        }
      }                                                            
    }

    while(1)
    {
      vote[0] = vote[1] = vote[2] = '\0';
      if (fhdr->filemode & VOTE_SCORE)
        sprintf(buf, "叫块眤匡拒羆 %d %s%s (F)絋粄 (Q)",
              count % 34 ? count/34+1 : count/34,
             (page > 0) ? " (P)" : "", (
             count > (page + 1) * 34) ? " (N)" : "" );
      else
        sprintf(buf, "眤临щ %d 布羆 %d %s%s (F)絋粄 (Q)",
          (int)fhdr->savemode - i, count % 34 ? count/34+1 : count/34,
             (page > 0) ? " (P)" : "", (
             count > (page + 1) * 34) ? " (N)" : "" );
      getdata(t_lines-2, 0, buf, vote, 3, DOECHO, 0);
      move(t_lines - 2, 0);
      invote = atoi(vote);
      *vote = tolower(*vote);
      if (vote[0] == 'q' || (!vote[0] && !i))
      {
        clrtoeol();
        prints("[5m癘眔ㄓщ翅!!      [m");
        refresh();
        page = 3; /* 铬 */
        break;
      }
      else if (vote[0] == 'f' && i)
         ;	/* щЧ */
      else if (vote[0] == 'p' || vote[0] == 'n')
      {
        if (vote[0] == 'p' && page > 0)
          page--;

        if (vote[0] == 'n' && page < (count / 34))
          page++;

        break;
      }
      else if ( invote <= 34 * page || invote > 34 * (page + 1))
         continue;
      else if (invote > 34 * page && invote <= 34 * (page + 1) &&
       invote <= count)
      {
        if (fhdr->filemode & VOTE_SCORE)   /* shakalaca.991127:  bufvote ㄓ讽璸だ,
        				    bufvote[材碭布] 莱赣魁 ぶだ,?,
        				   ┮ score[100] ㄓ癘材碭匡兜 ! */
        {
          char ans[3];
          if (!getdata(t_lines - 2, 0, "だ计: ", ans, 3, DOECHO, 0))
            continue;
          bufvote[i] = atoi(ans);	/* щ材碭布 (i) Τぶだ (ans) */
          if (bufvote[i] < 0 || bufvote[i] > 10)
            bufvote[i] = 0;
          move(((invote-1-page*34)%17)+4, (((invote-1-page*34))/17)*40 + 33);
          prints("%2d", bufvote[i]);
          score[i] = invote;	/* щ材碭布材碭匡兜 */
          i++;
          continue;
        }                          
        else
        {
          if (same(invote, bufvote, i, 1))
          {
            move(((invote-1-page*34)%17)+4, (((invote-1-page*34))/17)*40);
            prints(" ");
            i--;
            continue;
          }
          else
          {
            if (i == fhdr->savemode)
               continue;
            bufvote[i] = invote;
            move(((invote-1-page*34)%17)+4, (((invote-1-page*34))/17)*40);
              prints("*");
            i++;
            continue;
          }
        }
      }
      else
        continue;

      if (vote_flag(fhdr, vote[0]) != 0)
        prints("滦щ布! ぃぉ璸布");
      else
      {
        strcpy(fpath + len, STR_bv_ballots);
        if ((choice = open(fpath, O_WRONLY | O_CREAT | O_APPEND, 0600)) == 0)
          outs("礚猭щ布詏\n");
        else
        {
          struct stat statb;

          flock(choice, LOCK_EX);
          if (fhdr->filemode & VOTE_SCORE)
          {
            for (count = 0; count < i;count ++) /* count⊿ノㄓ璸计 */
            {
              for (i = 0; i < bufvote[count]; i++)	/* shakalaca.991127: ぶだ碞ぶ
              						   匡兜糶 */
              {
                sprintf(buf, "%02d", score[count]);
                write(choice, buf, 2);
              }              
            }
          }
          else
          {
            for (count = 0; count < i;count ++) /* count⊿ノㄓ璸计 */
            {
              sprintf(buf, "%02d", bufvote[count]);
              write(choice, buf, 2);
            }
          }
          flock(choice, LOCK_UN);
          fstat(choice, &statb);
          close(choice);
                      
          fclose(fp);
                                                                   
          if (getdata(t_lines - 4, 0, "Τ杠璶弧盾 ? (︽)\n>", buf, 65, DOECHO, 0))
          {
            strcpy(fpath+len, STR_bv_comments);
            if(fp = fopen(fpath, "a"))
            {
              fprintf(fp, "%-12s: %s\n", cuser.userid, buf);
              fclose(fp);
            }
          }
  
          substitute_record(fn_passwd, &cuser, sizeof(userec), usernum); /* 癘魁 */
 
          ingold(1);
          clrtoeol();
          if (fhdr->filemode & VOTE_SCORE)
            pressanykey("ЧΘщ布ó皑禣1じ刽");
          else            
            pressanykey("ЧΘщ布ó皑禣1じ刽(ヘ玡щ布计: %d)", statb.st_size/2);
        }
      }
      page = 3; /* 铬 */
      break;             
    }
  }

//  fclose(fp);
//  pressanykey(NULL);
  return RC_FULL;
}


static int
maintain_vote(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  char buf[STRLEN], fpath[STRLEN], inbuf[STRLEN], inchar[2];
  int len, page = 0;
  FILE *fp;

  if (!(currmode & MODE_BOARD))
    return 0;

  while (page < 3)
  {
    stand_title("щ布恨瞶");

    setbfile(fpath, currboard, fhdr->filename);
    len = strlen(fpath);

    strcpy(fpath + len, STR_bv_control);
    if ((fp = fopen(fpath, "r")))
    {
      int counts[100], fd, total, count = 0;

      memset(counts, 0, sizeof(counts));
      strcpy(fpath + len, STR_bv_ballots);

      total = dashs(fpath) / 2;
      if ((fd = open(fpath, O_RDONLY)) != -1)
      {
        while (read(fd, &inchar, 2) == 2)
          counts[atoi (inchar) - 1]++;
        close(fd);
      }

      outs("\n』 箇щ布ㄆ:\n\n");
      while (fgets(inbuf, sizeof(inbuf), fp))
      {
        inbuf[(strlen(inbuf) - 1)] = '\0';
        inbuf[31] = '\0';

        fd = counts[count];
        count++;
        if (count > 34 * page && count <= 34 * (page + 1))
        {
           move(((count-1 - 34 * page)%17)+4,((count-1 - 34 * page)/17)*40);
/*           prints(" %-31.30s %3d 布 (%-02.2f %%)", 
             buf, fd , total ? (float) 100 * fd/total : 0 ); */
           prints(" %-31.30s %3d %s", inbuf, fd, 
            (fhdr->filemode & VOTE_SCORE) ? "だ": "布");
        }
      }
      fclose(fp);

      move(b_lines-2, 0);
      if (!(fhdr->filemode & VOTE_SCORE))
        prints("\n』 ヘ玡羆布计 = %d 布", total);

      fd = 0;

      if (fhdr->filemode & VOTE_PRIVATE)
        buf[0] = answer("(A/B)/矗Νщ秨布 (C/D)絪胯弧/虫 (R)綷\弄痙ē (P/N)/ (E)膥尿[E]");
      else  
        buf[0] = answer("(A)щ布 (B)矗Ν秨布 (C)絪胯弧 (R)綷\弄痙ē (P/N)/ (D)膥尿[D] ");

      if (buf[0] == 'a' || buf[0] == 'A')
      {
        delete_vote_files(ent, fhdr, direct, fpath);
        fd = 1;
      }
      else if(buf[0] == 'b' || buf[0] == 'B')
      {
        make_results(fhdr, &counts, buf, fpath, 1);
        delete_vote_files(ent, fhdr, direct, fpath);
        fd = 2;
      }
      else if(buf[0] == 'c'|| buf[0] == 'C')
      {
        *(fpath + len) = 0;
        if (vedit(fpath, NA) == -1)
        {
          outs("щ布弧[ゼ穝]");
          pressanykey(NULL);
        }
      }
/*
      else if ((buf[0] == 'd' || buf[0] == 'D') && (fhdr->filemode & VOTE_PRIVATE))
      {
        friend_edit(FRIEND_CANVOTE);
      }
*/
      else if (buf[0] == 'r' || buf[0] == 'R')
      {
        strcpy(fpath + len, STR_bv_comments);
        more(fpath);
      }
      else if (buf[0] == 'p' || buf[0] == 'n' || buf[0] == 'P' || buf[0] == 'N')
      {
        if ((buf[0] == 'p' || buf[0] == 'P') && page > 0)
          page--;

        if ((buf[0] == 'n' || buf[0] == 'N') && count > (page + 1) * 34)
          page++;

        continue;
      }
      else
        break;                       

      if (fd)
      {
        boardheader bh;
        int pos;
  
        pos = rec_search(fn_board, &bh, sizeof(bh), cmpbnames, (int) currboard);
        if ((bh.vtime = next_closetime(direct, atol(fhdr->filename + 2))) == atol(fhdr->filename + 2))
          bh.bvote = fd == 1 ? 0 : 2;
        else
          bh.bvote = 1;
        if (substitute_record(fn_board, &bh, sizeof(bh), pos, 0) == -1)
          outs(err_board_update);
        touch_boards();           /* vote flag changed */
        resolve_boards();
      }
    }
    break;
    return RC_FULL;
  }

  return RC_FULL;
}


int
make_vote()
{
  char buf[STRLEN], fpath[STRLEN];
  int num = 0, len, aborted = 0, page = 0;
  time_t closetime;
  fileheader vfh;
  struct tm *ptime;
  FILE *fp;

  if (!(currmode & MODE_BOARD))
    return 0;

  setbfile(buf, currboard, FN_VOTE);
  if (rec_num(buf,FHSZ) >= 20)
  {
    pressanykey("%s 狾竒Τびщ布!!",currboard);
    return 0;
  }

  stand_title("羭快щ布");
  memset(&vfh, 0, FHSZ);

  getdata(2, 0, "щ布よΑ (1) (2)璸だ [1] ", buf, 4, LCECHO, 0);

  if (atoi(buf) == 2)
    vfh.filemode = VOTE_SCORE;
  else if ((atoi(buf) == 3) && HAS_PERM(PERM_SYSOP))
    vfh.filemode = VOTE_PAPER;
  else 
    vfh.filemode = VOTE_NORMAL;

  getdata(3, 0, "Ωщ布秈︽碭ぱ (ぶぱ)", buf, 4, DOECHO, 0);
  if ((num = atoi(buf)) < 1)
    num = 1;

  time(&closetime);
  ptime = localtime(&closetime);
  sprintf(vfh.date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
  sprintf(vfh.title + 65, "/%02d", ptime->tm_year % 100);
  sprintf(vfh.owner, "%d", closetime);

  getdata(4, 0, "щ布肈", vfh.title, 40, DOECHO, 0);
  if (1)
  {
    FILE *fp;
    char buf[128];
    sprintf(buf,"boards/%s/title",currboard);
    fp = fopen(buf,"w");
    fputs(vfh.title, fp);                                       
    fclose(fp);                                          
  }

  closetime += num * 86400;
  do
  {
    sprintf(vfh.filename, "V.%d.A", ++closetime);
    setbfile(fpath, currboard, vfh.filename);
  }while(dashf(fpath));
  len = strlen(fpath);

  pressanykey("ヴ龄秨﹍絪胯Ω [щ布弧/﹙Ξ]");

  if (vedit(fpath, NA) == -1)
  {
    unlink(fpath);
    pressanykey("Ωщ布");
    return RC_FULL;
  }
/*
  getdata(2, 0, "щ布璶砞盾 (Y/N) [N] ", buf, 4, LCECHO, 0);

  if (*buf == 'y')
  {
    friend_edit(FRIEND_CANVOTE);
    vfh.filemode |= VOTE_PRIVATE;
  }
 wildcat : ぐ或 else 璶р V.xxx.A unlink 奔? 螟┣常⊿弧 :p
  else
  {
    if (dashf(fpath)) 
      unlink(fpath);
  }
*/
  clear();
  strcpy(fpath + len, STR_bv_control);
  fp = fopen(fpath, "w");

  move(0, 0);
  outs("叫ㄌ块匡兜,  ENTER ЧΘ砞﹚");
  num = 0;
  /* 块 99 匡兜 */
  while (!aborted)
  {
    sprintf(buf, "%02d) ", num + 1);
    getdata(((num - page * 34) % 17) + 2, ((num - page * 34) / 17) * 40,
      buf, buf, 33, DOECHO, 0);
    if (*buf)
    {
      fprintf(fp, "%02d) %s\n", num + 1, buf);
      num++;
    }
    
    if (*buf == '\0' || num == 99)
      aborted = 1;
    
    if (num % 34 == 0)
    {
      page++;
      clear();
      outs("叫ㄌ块匡兜,  ENTER ЧΘ砞﹚\n");
    }
  }
  fclose(fp);

  if (num == 0)
  {
    clear();
    unlink(fpath);
    *(fpath + len) = 0;
    unlink(fpath);
    prints( "Ωщ布\n" );
    pressanykey(NULL);
    return RC_FULL;
  }
  else
  {
    boardheader bh;

    if (!(vfh.filemode & VOTE_SCORE))
    {
      while(1)
      {
        move(t_lines - 3, 0);
        sprintf(buf, "叫拜щ碭布 (1-%1d): ",num);
        getdata(t_lines - 3, 0, buf , buf, 3, DOECHO, 0);
       
        if (atoi(buf) <= 0 || atoi(buf) > num)
          continue;
        else
        {
          vfh.savemode = atoi(buf);
          break;
        }
      }
    }
    
    setbfile(fpath, currboard, FN_VOTE);
    if (rec_add(fpath, &vfh, FHSZ) == -1)
    {
      outs("╰参祇ネ岿粇! щ布!");
      setbfile(fpath, currboard, vfh.filename);
      unlink(fpath);
      strcpy(fpath + len, STR_bv_control);
      unlink(fpath);
      pressanykey(NULL);
      return RC_FULL;
    }

    num = rec_search(fn_board, &bh, sizeof(bh), cmpbnames, (int) currboard);
    if (bh.bvote == 1 && bh.vtime < closetime); /* voting & closetime is earlier */
    else
    {
      bh.bvote = 1;
      bh.vtime = closetime;
      if (substitute_record(fn_board, &bh, sizeof(bh), num, 0) == -1)
        outs(err_board_update);
      touch_boards();           /* vote flag changed */
    }
    move(t_lines - 2, 0);
    outs("秨﹍щ布");
  }
  pressanykey(NULL);
  return RC_FULL;

}

struct one_key vote_comms[] = {
  'r', do_vote,
  Ctrl('P'), make_vote,
  'M', maintain_vote,
  '\0', NULL
};


void
votedoent(num, ent, row, bar)
  int num;
  fileheader *ent;
  int row, bar;
{
  char closeday[9];
  struct tm *ptime;
  time_t dtime;
  char buf[128], outmsg[256];

  dtime = atol(ent->filename + 2);
  ptime = localtime(&dtime);
  sprintf(closeday, "%02d/%02d/%02d",
          ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_year % 100);

  if(bar==1)
    sprintf(outmsg, "〈");
  else
    sprintf(outmsg, "  ");

  sprintf(buf, "%4d %c %-5s%-4.3s %-11s %s[%s][m  %s", 
    num, vote_flag(ent, NULL) ? ' ':'+', ent->date, ent->title + 65, closeday,
    (ent->filemode & VOTE_PRIVATE) ? "[1;31m" : "[1;36m",
    (ent->filemode & VOTE_PRIVATE) ? "╬" : "", ent->title);
  strcat(outmsg, buf);
    
  outmsgline(outmsg, row);
  
}


static void
votetitle()
{
  showtitle("щ布┮", BoardName);
  outs("\
[■]瞒秨 [△/ENTER]秨﹍щ布 狾盡ノ龄: [^P]羭快Ωщ布 [M]щ布/矗Ν秨布\n\
" COLOR1 "\033[1m絪腹   秨﹍ら戳   挡ら戳   щ  布    肈                                   \033[0m");
}


int
ShowVote(bname)
  char *bname;
{
  char buf[STRLEN], currboard0[15];

  if (currmode & MODE_DIGEST)
    return RC_NONE;

  strcpy(currboard0, currboard);
  strcpy(currboard, bname);
  setbfile(buf, currboard, FN_VOTE);
  getkeep(buf, 1, 1);

  i_read(VOTING, buf, votetitle, votedoent, vote_comms, NULL);

  strcpy(currboard, currboard0);
  return RC_FULL;
}


int
b_vote()
{
  return ShowVote(currboard);
}


int
b_results()
{
  return vote_results(currboard);
}

/* щ布いみ chyiuan */
int
all_vote()
{
  register int i;
  register boardheader *bhdr;
  int all,pass,ch,tmp;
  int boards;
  int redraw=1;
  int pageboard=15,pagenum=0;
  char buf[256];
//  fileheader vch;
  struct vdata
  {
    char pointer[5];
    char bname[20];
    char btitle[50];
    int num;
  } vdata[MAXBOARD];

  all = 0;
  pass = 1;
  resolve_boards();

  for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
    if (bhdr->bvote == 1 && Ben_Perm(bhdr))
    {
      strcpy(vdata[all].bname, bhdr->brdname);
      strcpy(vdata[all].btitle, bhdr->title);
      sprintf(buf, BBSHOME"/boards/%s/%s", bhdr->brdname, ".VCH");
      vdata[all].num = rec_num(buf, FHSZ);
      strcpy(vdata[all].pointer,"");

/*
      int vnum = 0;
      
      sprintf(buf, BBSHOME"/boards/%s/%s", bhdr->brdname, ".VCH");
      for(vnum = rec_num(buf,FHSZ); vnum > 0 ; vnum--)
      {
        rec_get(buf, &vch , FHSZ , vnum);
        strcpy(vdata[all].bname, bhdr->brdname);
        strcpy(vdata[all].vtitle, vch.title);
        strcpy(vdata[all].pointer,"");
        if(vch.filemode & VOTE_PRIVATE)
        {
          setbfile(buf, vdata[all].bname, FN_CANVOTE);
          if(!belong(buf, cuser.userid))
            strcpy(vdata[all].mode,"[1;33m[╬][mぃ淋");
          else
            strcpy(vdata[all].mode,"[1;33m[╬][m淋い");
        }
        else
          strcpy(vdata[all].mode,"[1;33m[そ][m⊿");
        all++;          
      }
*/
      all++;
    }

    if (!all)
    {
      pressanykey("ヘ玡ず⊿Τヴщ布...");
      return 0;
    }

    pass=0;
    pagenum=0;
    strcpy(vdata[pass].pointer,"〈");
    do
    {
      if(redraw)
      {
        showtitle("羛щ布いみ", BoardName);

        show_file(BBSHOME"/etc/votetitle",1,7,ONLY_COLOR);
        redraw=0;
      }
      pagenum=pass/pageboard;
      if(pagenum<(all/pageboard))
        boards=pageboard;
      else
        boards=all-(all/pageboard)*pageboard;
      clrchyiuan(8,9+pageboard);
      for (i = 0; i <boards; i++)
      {
        move(8+i,0);
        prints("[0m%s [37m%2d.  %-14s   %-44.44s %d[0m ",
        vdata[i+pagenum*pageboard].pointer,
        i+pagenum*pageboard + 1,
        vdata[i+pagenum*pageboard].bname,
        vdata[i+pagenum*pageboard].btitle,
        vdata[i+pagenum*pageboard].num);
//        vote_flag(vdata[i+pagenum*pageboard].bname,'\0')?"[1;36mщ[0m":"[1;31mゼщ[0m");
      }
      move(b_lines,0);
      prints(COLOR1
"[1m щ布匡虫 [33m(◆/□)(p/n)[37m┕/ [33m(PgUp/PgDn)(P/N)[37m/ [33m(△)(r)[37mщ布 [33m(■)(q)[37m瞒秨  [m");
      move(b_lines,79);
      ch=egetch();
      strcpy(vdata[pass].pointer,"");
      switch(ch)
      {
        case 'e':
        case KEY_LEFT:
          ch='q';
        case 'q':
          break;

        case KEY_PGUP:
        case 'P':
        case 'b':
          if(pass==0)
            pass=all-1;
          else
            pass-=pageboard;
          if(pass<0)
            pass=0;
          break;
       case KEY_PGDN:
       case ' ':
       case 'N':
       case Ctrl('F'):
         if(pass==all-1)
           pass=0;
         else
           pass+=pageboard;
         if(pass>=all-1)
           pass=all-1;
         break;
       case KEY_DOWN:
       case 'n':
       case 'j':
         if(pass++ >=all-1)
           pass=0;
         break;

       case KEY_UP:
       case 'p':
       case 'k':
         pass--;
         if(pass<0)
           pass=all-1;
         break;

       case '0':
       case KEY_HOME:
         pass= 0;
         break;
       case '$':
       case KEY_END:
         pass=all-1;
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
         if ((tmp = search_num(ch,all-1)) >= 0)
           pass = tmp;
         break;
       case KEY_RIGHT:
       case '\n':
       case '\r':
       case 'r':
         if (pass <= all-1 && pass >= 0)
         {
           strcpy(buf, currboard);
           strcpy(currboard, vdata[pass].bname);
           b_vote();
           strcpy(currboard, buf);
           redraw=1;
         }
         break;
       }
     strcpy(vdata[pass].pointer,"〈");
   }while(ch!='q' && ch!='Q');
  return 0;
}
