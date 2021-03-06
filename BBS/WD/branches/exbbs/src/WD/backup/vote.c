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
extern void friend_edit(int type);

static char STR_bv_control[] = "control";  /* щ布ら戳 匡兜 */
static char STR_bv_desc[] = "desc";        /* щ布ヘ */
static char STR_bv_ballots[] = "ballots";
static char STR_bv_results[] = "results";
static char STR_bv_flags[] = "flags";
static char STR_bv_tickets[] = "tickets";  /* 碭布 */
static char STR_bv_comments[] = "comments"; /* щ布種 */
static char STR_bv_limited[] = "limited"; /* ╬щ布 */

boardheader *bcache;

void
b_suckinfile(fp, fname)
  FILE *fp;
  char *fname;
{
  FILE *sfp;

  if (sfp = fopen(fname, "r"))
  {
    char inbuf[256];

    while (fgets(inbuf, sizeof(inbuf), sfp))
      fputs(inbuf, fp);
    fclose(sfp);
  }
}


int
b_closepolls()
{
  static char *fn_vote_polling = ".voting";
  boardheader fh;
  struct stat st;
  FILE *cfp;
  time_t now;
  int fd, dirty;

  now = time(NULL);

  if (stat(fn_vote_polling, &st) == -1 || st.st_mtime > now - 3600)
    return 0;

  if ((cfp = fopen(fn_vote_polling, "w")) == NULL)
    return 0;
  fprintf(cfp, ctime(&now));
  fclose(cfp);

  resolve_boards();
  if ((fd = open(fn_board, O_RDWR)) == -1)
  {
    outs(ERR_BOARD_OPEN);
    return -1;
  }

  flock(fd, LOCK_EX);
  dirty = 0;
  while (read(fd, &fh, sizeof(fh)) == sizeof(fh))
  {
    if (fh.bvote && b_close(&fh))
    {
      lseek(fd, (off_t)(sizeof(fh) * (getbnum(fh.brdname) - 1)), SEEK_SET);
      if (write(fd, &fh, sizeof(fh)) != sizeof(fh))
      {
        sprintf(currmsg, "[1;5;37;41mWarning![m");
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
  return 0;
}


void
vote_report(bname, fname, fpath)
  char *bname, *fname, *fpath;
{
  register char *ip;
  time_t dtime;
  int fd;
  fileheader header;

  ip = fpath;
  while (*(++ip));
  *ip++ = '/';

  /* get a filename by timestamp */

  dtime = time(0);
  for (;;)
  {
// wildcat : セ钩Τ糶岿 ?
    sprintf(ip, "M.%d.A", ++dtime);
    fd = open(fpath, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd >= 0)
      break;
    dtime++;
  }
  close(fd);
  unlink(fpath);
  link(fname, fpath);

  /* append record to .DIR */

  memset(&header, 0, sizeof(fileheader));
  strcpy(header.owner,"[皑隔贝]");
  sprintf(header.title, "[%s] 狾 匡薄厨旧", bname);
  {
    register struct tm *ptime = localtime(&dtime);
    sprintf(header.date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
  }
  strcpy(header.filename, ip);

  strcpy(ip, ".DIR");
  if ((fd = open(fpath, O_WRONLY | O_CREAT, 0644)) >= 0)
  {
    flock(fd, LOCK_EX);
    lseek(fd, (off_t)0, SEEK_END);
    write(fd, &header, sizeof(fileheader));
    flock(fd, LOCK_UN);
    close(fd);
  }
}


int
b_close(fh)
  boardheader *fh;
{
  FILE *cfp, *tfp, *frp;
  char *bname = fh->brdname;
  char buf[STRLEN];
  char inchar, inbuf[80];
  int counts[31], fd;
  int total=0, num;
  char b_control[64];
  char b_newresults[64];
  char b_report[64];
  time_t now;
  struct stat st;

  now = time(NULL);

  if (fh->bvote == 2)
  {
    if (fh->vtime < now - 7 * 86400)
    {
      fh->bvote = 0;
      return 1;
    }
    else
      return 0;
  }

  if (fh->vtime > now)
    return 0;

  fh->bvote = 2;
  setbfile(b_control, bname, STR_bv_control);
  if (stat(b_control, &st) == -1)
  {
    fh->bvote = 0;
    return 1;
  }

  memset(counts, 0, sizeof(counts));
  setbfile(buf, bname, STR_bv_flags);
  unlink(buf);
  setbfile(buf, bname, STR_bv_tickets);
  unlink(buf);
  setbfile(buf, bname, STR_bv_ballots);
  if ((fd = open(buf, O_RDONLY)) != -1)
  {
    while (read(fd, &inchar, 1) == 1)
    {
      counts[(int) (inchar - 'A')]++;
      total++;
    }
    close(fd);
  }
  unlink(buf);

  setbfile(b_newresults, bname, "newresults");
  if ((tfp = fopen(b_newresults, "w")) == NULL)
    return 1;

  fprintf(tfp, "%s\n』 щ布いゎ: %s\n』 布匡肈ヘ磞瓃:\n\n",
    msg_seperator, ctime(&fh->vtime));

  setbfile(buf, bname, STR_bv_desc);
  b_suckinfile(tfp, buf);
  unlink(buf);

  fprintf(tfp, "\n』 щ布挡狦:\n\n");
  if (cfp = fopen(b_control, "r"))
  {
    fgets(inbuf, sizeof(inbuf), cfp);
    while (fgets(inbuf, sizeof(inbuf), cfp))
    {
      inbuf[(strlen(inbuf) - 1)] = '\0';
      inbuf[43] = '\0';         /* truncate */
      num = counts[inbuf[0] - 'A'];
      fprintf(tfp, "    %-42s%3d 布   (%2.2f%%)\n", inbuf + 3, num,
		 total ?  (float) 100 * num/total : 0);
    }
    fclose(cfp);
  }
  unlink(b_control);

  fprintf(tfp, "\n』 ㄏノ某\n");
  setbfile(buf, bname, STR_bv_comments);
  b_suckinfile(tfp, buf);
  unlink(buf);

  fprintf(tfp, "\n』 羆布计 = %d 布\n\n", total);
  fclose(tfp);

  setbfile(b_report, bname, "report");
  if (frp = fopen(b_report, "w"))
  {
    b_suckinfile(frp, b_newresults);
    fclose(frp);
  }
  
  sprintf(inbuf,BBSHOME"/boards/%s",bname);
  vote_report(bname, b_report, inbuf);
  sprintf(inbuf,BBSHOME"/boards/%s","Record");
  vote_report(bname, b_report, inbuf);
  unlink(b_report);

  tfp = fopen(b_newresults, "a");
  setbfile(buf, bname, STR_bv_results);
  b_suckinfile(tfp, buf);
  fclose(tfp);
  Rename(b_newresults, buf);
  return 1;
}


int
vote_maintain(bname)
  char *bname;
{
  FILE *fp, *tick;
  char inbuf[STRLEN], buf[STRLEN];
  int num = 0, aborted;
  time_t closetime;
  int pos;
  boardheader fh;
  char genbuf[4];

  if (!(currmode & MODE_BOARD))
    return 0;

  stand_title("羭快щ布");

  setbfile(buf, bname, STR_bv_control);
  if ((fp = fopen(buf, "r")))
  {
    int counts[31], fd, total=0, count = 0;
    char inchar;

    fgets(inbuf, sizeof(inbuf), fp);
    closetime = (time_t) atol(inbuf);
    prints("丁ず礚猭羭︽ㄢщ布\nΩщ布箇璸挡: %s",
      ctime(&closetime));

    memset(counts, 0, sizeof(counts));
    setbfile(buf, bname, STR_bv_ballots);
    if ((fd = open(buf, O_RDONLY)) != -1)
    {
      while (read(fd, &inchar, 1) == 1)
      {
        counts[(int) (inchar - 'A')]++;
        total++; 
      }   
      close(fd);
    }
    outs("\n』 箇щ布ㄆ:\n\n");
    while (fgets(inbuf, sizeof(inbuf), fp))
    {
      inbuf[(strlen(inbuf) - 1)] = '\0';
      inbuf[43] = '\0';         /* truncate */
      num = counts[inbuf[0] - 'A'];
      count++;
      move(((count-1)%15)+4,((count-1)/15)*40);
      prints("   %-22s%3d 布 (%2.2f%%)", inbuf + 3, num,
                  total ? (float) 100 * num/total:0);
    }
    fclose(fp);
    move(b_lines-2, 0);
    prints("』 ヘ玡羆布计 = %d 布", total);
    getdata(b_lines - 1, 0, "(A)щ布 (B)矗Ν秨布 (C)膥尿[C] ", genbuf, 4, LCECHO,"C");
    if (genbuf[0] == 'a')
    {
      setbfile(buf, bname, STR_bv_tickets);
      unlink(buf);
      setbfile(buf, bname, STR_bv_control);
      unlink(buf);
      setbfile(buf, bname, STR_bv_flags);
      unlink(buf);
      setbfile(buf, bname, STR_bv_ballots);
      unlink(buf);
      setbfile(buf, bname, STR_bv_desc);
      unlink(buf);
      setbfile(buf, bname, STR_bv_limited);
      unlink(buf);

      pos = rec_search(fn_board, &fh, sizeof(fh), cmpbnames, (int) bname);
      fh.bvote = 0;
      if (substitute_record(fn_board, &fh, sizeof(fh), pos) == -1)
        outs(err_board_update);
      touch_boards();           /* vote flag changed */
      resolve_boards();
    }
    else if (genbuf[0] == 'b')
    {
      rec_search(fn_board, &fh, sizeof(fh), cmpbnames, (int) bname);
      fh.vtime = time(NULL) -1;
      b_close(&fh);
    }
    return RC_FULL;
  }
 if(1)
  {
    FILE *fp;
    char buf[128];
    sprintf(buf,"boards/%s/title",currboard);
    fp = fopen(buf,"w");
    getdata(2, 0, "叫块щ布肈:", buf, 65, DOECHO, 0);
    fputs(buf,fp);                                       
    fclose(fp);                                          
  }

  outs("ヴ龄秨﹍絪胯Ω [щ布弧]:");
  igetch();
  setbfile(buf, bname, STR_bv_desc);
  aborted = vedit(buf, NA);
  if (aborted== -1)
  {
    pressanykey("Ωщ布");
    return RC_FULL;
  }
  aborted = 0;
  setbfile(buf, bname, STR_bv_flags);
  unlink(buf);

  getdata(4, 0, "琌﹚щ布虫\
 (y)絪膟щ布虫 [N]ヴщ布 ", inbuf, 2, LCECHO, "N");
  setbfile(buf, bname, STR_bv_limited);
  if (inbuf[0] == 'y')
  {
    fp = fopen (buf,"w");
    fprintf(fp,"Ωщ布砞");
    fclose(fp);
    friend_edit(FRIEND_CANVOTE);
  }
  else
  {
    if (dashf(buf)) unlink(buf);
  }

  clear();
  getdata(0, 0, "Ωщ布秈︽碭ぱ (ぶぱ)", inbuf, 4, DOECHO, 0);

  if (*inbuf == '\n' || !strcmp(inbuf, "0") || *inbuf == '\0')
    strcpy(inbuf, "1");

  time(&closetime);
  closetime += atol(inbuf) * 86400;
  setbfile(buf, bname, STR_bv_control);
  fp = fopen(buf, "w");
  fprintf(fp, "%lu\n", closetime);
  fflush(fp);

  clear();
  outs("\n叫ㄌ块匡兜,  ENTER ЧΘ砞﹚");
  num = 0;
  while (!aborted)
  {
    sprintf(buf, "%c) ", num + 'A');
    getdata( (num%15) + 2,(num/15)*40, buf, inbuf, 36, DOECHO, 0);
    if (*inbuf)
    {
      fprintf(fp, "%1c) %s\n", (num+'A'), inbuf);
      num++;
    }
    if (*inbuf == '\0' || num == 30)
      aborted = 1;
  }
  fclose(fp);

  if (num == 0)
  {
    clear();
    setbfile(buf, bname, STR_bv_control);
    unlink(buf);
    pressanykey("Ωщ布");
    return RC_FULL;
  }
  else
  {
    pos = rec_search(fn_board, &fh, sizeof(fh), cmpbnames, (int) bname);
    fh.bvote = 1;
    fh.vtime = closetime;
    if (substitute_record(fn_board, &fh, sizeof(fh), pos) == -1)
      outs(err_board_update);
    touch_boards();             /* vote flag changed */
    while(1)
    {
      move( t_lines - 3, 0);
      sprintf(buf,"叫拜щ碭布 (1~%1d): ",num);
      getdata(t_lines-3, 0, buf , inbuf, 3, DOECHO, 0);
      if(atoi(inbuf)<=0 ||atoi(inbuf)>num)
        continue;
      else
      {
        setbfile( buf, bname, "tickets" );
        if((tick= fopen(buf, "w")) == NULL)
          break;
        fprintf(tick,"%s\n",inbuf);
        fclose(tick);
        break;
      }
    }
    move( t_lines - 2, 0);
    outs("秨﹍щ布");
  }
  pressanykey(NULL);
  return RC_FULL;
}


int
vote_flag(bname, val)
  char *bname, val;
{
  char buf[256], flag;
  int fd, num, size;

  num = usernum - 1;
  setbfile(buf, bname, STR_bv_flags);
  if ((fd = open(buf, O_RDWR | O_CREAT, 0600)) == -1)
  {
    return -1;
  }
  size = lseek(fd, 0, SEEK_END);
  memset(buf, 0, sizeof(buf));
  while (size <= num)
  {
    write(fd, buf, sizeof(buf));
    size += sizeof(buf);
  }
  lseek(fd, (off_t)num, SEEK_SET);
  read(fd, &flag, 1);
  if (flag == 0 && val != 0)
  {
    lseek(fd, (off_t)num, SEEK_SET);
    write(fd, &val, 1);
  }
  close(fd);
  return flag;
}


int
same(compare, list, num)
  char compare;
  char list[];
  int num;
{
  int n;
  int rep = 0;

  for(n = 0; n < num; n++)
  {
    if(compare == list[n])
      rep = 1;
    if(rep == 1)
      list[n] = list[n+1];
  }
  return rep;
}


int
user_vote(bname)
  char *bname;
{
  FILE *cfp, *fcm;
  char buf[STRLEN];
  char inbuf[80], choices[31], vote[2], bufvote[31];
  int i = 0, tickets, j;
  int fd, count = 0;
  time_t closetime;

  setbfile( buf, bname, STR_bv_tickets);
  if((cfp= fopen(buf, "r")) == NULL)
    tickets=1;
  else
  {
    fgets( inbuf, sizeof(inbuf), cfp );
    tickets = atoi(inbuf);
    fclose(cfp);
  }


  setbfile(buf, bname, STR_bv_control);
  move(3, 0);
  clrtobot();
  if ((cfp = fopen(buf, "r")) == NULL)
  {
    pressanykey("ヘ玡⊿Τヴщ布羭︽");
    return RC_FULL;
  }

  if (!HAS_PERM(PERM_BASIC))
  {
    fclose(cfp);
    pressanykey("癸ぃ癬! 眤临⊿Τ莉眔щ布舦翅!");
    return RC_FULL;
  }
  setbfile(buf, bname, STR_bv_limited); /* Ptt */
  if (dashf(buf))
  {
    setbfile(buf, bname, FN_CANVOTE);
    if(!belong(buf, cuser.userid))
     {
       fclose(cfp);
       pressanykey("癸ぃ癬! 硂琌╬щ布..⊿Τ淋!");
       return RC_FULL;
     }
    else
     {
       pressanykey("尺淋Ω╬щ布....");
//       pressanykey("尺淋Ω╬щ布....<ヴ種龄浪跌Ω淋虫>");
//       more(buf,YEA);
     }
  }

  setutmpmode(VOTING);
  setbfile(buf, bname, STR_bv_desc);
  more(buf, YEA);

  stand_title("щ布絚");
  fgets(inbuf, sizeof(inbuf), cfp);
  closetime = (time_t) atol(inbuf);

  prints("щ布よΑ絋﹚眤匡拒块ㄤ絏(A, B, C...)\n"
    "Ωщ布щ %1d 布"
    "[1m[30;41m  0 щ布  "COLOR1"  1 ЧΘщ布 [m\n"
    "Ωщ布盢挡%s \n",
    tickets, ctime(&closetime));
  move(5, 0);
  memset(choices, 0, sizeof(choices));
  while (fgets(inbuf, sizeof(inbuf), cfp))
  {
    choices[count++] = inbuf[0];
    move(((count-1)%15)+5,((count-1)/15)*40);
    prints( " %s", strtok(inbuf,"\n\0"));
  }
  fclose(cfp);

  if (vote_flag(bname, '\0'))
  {
    move(21, 0);
    pressanykey("Ωщ布щ筁Ω產キ单");
    return RC_FULL;
  }

  while(1)
  {
    vote[0] = vote[1] = '\0';
    move(t_lines-2, 0);
    prints("临щ %2d 布", tickets-i);
    getdata(t_lines-4, 0, "块眤匡拒: ", vote, 3, DOECHO, 0);
    move(t_lines - 2, 0);
    *vote = toupper(*vote);
    if (vote[0] == '0' || (!vote[0] && !i))
    {
      clrtoeol();
      prints("[5m癘ㄓщ翅!!      [m");
      refresh();
      break;
    }
    else if (vote[0] == '1' && i)
       ;
    else if (!vote[0])
       continue;
    else if (index(choices, vote[0]) == NULL) /* 礚 */
      continue;
    else if (same(vote[0], bufvote, i))
    {
      move(((vote[0]-'A')%15)+5, (((vote[0]-'A'))/15)*40);
      prints(" ");
      i--;
      continue;
    }
    else
    {
      if (i == tickets)
         continue;
      bufvote[i] = vote[0];
      move(((vote[0]-'A')%15)+5, (((vote[0]-'A'))/15)*40);
      prints("*");
      i++;
      continue;
    }

    if (vote_flag(bname, vote[0]) != 0)
      prints("滦щ布! ぃぉ璸布");
    else
    {
      setbfile(buf, bname, STR_bv_ballots);
      if ((fd = open(buf, O_WRONLY | O_CREAT | O_APPEND, 0600)) == 0)
        outs("礚猭щ布詏\n");
      else
      {
        struct stat statb;
        char buf[3], mycomments[3][74], b_comments[80];

        for(j=0; j<3;j++) strcpy(mycomments[j],"\n");

        flock(fd, LOCK_EX);
        write(fd, bufvote, i);
        flock(fd, LOCK_UN);
        fstat(fd, &statb);
        close(fd);
        move(0,0);
        clrtobot();
        getdata(b_lines - 2, 0,"眤癸硂Ωщ布Τぐ或腳禥種ǎ盾(y/n)[N]",
                buf,3,DOECHO,0);
        if(buf[0]=='Y' || buf[0]=='y'){
	do{
        move(5,0);
        outs("叫拜眤癸硂Ωщ布Τぐ或腳禥種ǎ程︽[Enter]挡");
        for (j = 0; (j < 3) &&
                getdata(7 + j, 0, "", mycomments[j], 74, DOECHO, 0); j++);
        for(j = 0; j < 3; j++)
        strip_ansi(mycomments[j], mycomments[j] ,0);
        getdata(b_lines-2,0,"(S)纗 (E)穝ㄓ筁 (Q)[S]",buf,3,LCECHO,0);
	if (buf[0] == 'q' || j == 0 && *buf != 'e')
	return;                             
	}while (buf[0] == 'e');                    
        setbfile(b_comments, bname, STR_bv_comments);
        if(mycomments[0])
        if (fcm = fopen(b_comments, "a")){
        fprintf(fcm, "〕ㄏノ %s 某\n", cuser.userid);
        for(j = 0; j < 3; j++)
        fprintf(fcm, "    %s\n", mycomments[j]);
        fprintf(fcm, "\n");
        fclose(fcm);
          }
        }
        move(b_lines -1 ,0);
        substitute_record(fn_passwd, &cuser, sizeof(userec), usernum); /* 癘魁 */

	ingold(1);
        prints("ЧΘщ布ó皑禣1じ刽(ヘ玡щ布计: %d)\n", statb.st_size);
      }
    }
    break;
  }
  pressanykey(NULL);
  return RC_FULL;
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
  }
  return RC_FULL;
}


int
b_vote_maintain()
{
  return vote_maintain(currboard);
}


int
b_vote()
{
  return user_vote(currboard);
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
  FILE *fp;
  struct vdata
  {
    char pointer[5];
    char bname[20];
    char vtitle[50];
    char mode[25];
  } vdata[MAXBOARD];

  all = 0;
  pass = 1;
  resolve_boards();

  for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
    if (bhdr->bvote == 1 && Ben_Perm(bhdr))
    {
      strcpy(vdata[all].bname, bhdr->brdname);
      sprintf(buf, "boards/%s/title", bhdr->brdname);
      if (fp = fopen(buf, "r"))
      {
        fgets(vdata[all].vtitle, 50, fp);
        fclose(fp);
      }
      else
        strcpy(vdata[all].vtitle, "タщ布い.....");
      strcpy(vdata[all].pointer,"");
      setbfile(buf, vdata[all].bname, "limited");
      if (dashf(buf))
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

        show_file(BBSHOME"/etc/votetitle",1,6,ONLY_COLOR);
        move(7,0);
        prints(COLOR1
"[1m  絪腹  嘿         щ  布    肈                        щ布摸  笆  [m"
);
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
        prints("[0m%s [37m%2d.  %-14s   %-36.36s %s %s[0m ",
        vdata[i+pagenum*pageboard].pointer,
        i+pagenum*pageboard + 1,
        vdata[i+pagenum*pageboard].bname,
        vdata[i+pagenum*pageboard].vtitle,
        vdata[i+pagenum*pageboard].mode,
        vote_flag(vdata[i+pagenum*pageboard].bname,'\0')?"[1;36mщ[0m":"[1;31mゼщ[0m");
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
