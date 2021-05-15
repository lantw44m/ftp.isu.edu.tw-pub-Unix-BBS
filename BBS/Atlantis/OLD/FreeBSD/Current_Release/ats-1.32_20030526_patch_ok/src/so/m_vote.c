#include "bbs.h"

static char STR_bv_control[] = "control";
static char STR_bv_desc[] = "desc";
static char STR_bv_ballots[] = "ballots";
static char STR_bv_results[] = "results";
static char STR_bv_flags[] = "flags";
static char STR_bv_tickets[] = "tickets";

extern int cmpbnames();

int vote_maintain(char *bname) {
  FILE *fp, *tick;
  char inbuf[STRLEN], buf[STRLEN];
  int num = 0, aborted, tickets;
  time_t closetime;
  int pos;
  boardheader fh;
  char genbuf[4];
  char fpath[100];
  FILE *vtfp;

  if(!(currmode & MODE_BOARD)) return 0;

  stand_title("羭快щ布");

  setbfile(buf, bname, STR_bv_control);
  if ((fp = fopen(buf, "r")))
  {
    int counts[31], fd, total, count = 0;
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
        counts[(int) (inchar - 'A')]++;
      close(fd);
    }

    outs("\n』 箇щ布ㄆ:\n\n");
    total = 0;

    while(fgets(inbuf, sizeof(inbuf), fp)) {
      inbuf[(strlen(inbuf) - 1)] = '\0';
      inbuf[43] = '\0';         /* truncate */
      num = counts[inbuf[0] - 'A'];

      count++;
      move(((count-1)%15)+4,((count-1)/15)*40);
      prints("   %-31s%3d 布", inbuf, num);
      total += num;
    }
    fclose(fp);

    move(b_lines-2, 0);

    prints("\n』 ヘ玡羆布计 = %d 布", total);

    getdata(b_lines - 1, 0, "(A)щ布 (B)矗Ν秨布 (C)膥尿[C] ", genbuf,
            4, LCECHO, 0);
    if(genbuf[0] == 'a') {
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

      pos = search_record(fn_board, &fh, sizeof(fh), cmpbnames, (int) bname);
      fh.bvote = 0;
      if(substitute_record(fn_board, &fh, sizeof(fh), pos) == -1)
        outs(err_board_update);
      touch_boards();           /* vote flag changed */
      resolve_boards();
      sprintf(fpath, "boards/%s/vote.sus", currboard);
      remove(fpath);
    }
    else if(genbuf[0] == 'b') {
      char b_newresults[80];

      setbfile(b_newresults, bname, "newresults");
      vtfp = fopen(b_newresults, "w");
      if(!vtfp) return FULLUPDATE;

      fprintf(vtfp, "%s\n』 щ布いゎ: %s\n』 布匡肈ヘ磞瓃:\n\n",
        msg_seperator, ctime(&closetime));

      setbfile(buf, bname, STR_bv_tickets);
      unlink(buf);
      setbfile(buf, bname, STR_bv_flags);
      unlink(buf);
      setbfile(buf, bname, STR_bv_ballots);
      unlink(buf);
      setbfile(buf, bname, STR_bv_desc);
      b_suckinfile(vtfp, buf);
      unlink(buf);

      fprintf(vtfp, "』 щ布挡狦:\n\n");
      total = 0;
      setbfile(buf, bname, STR_bv_control);
      if (fp = fopen(buf, "r"))
      {
        fgets(inbuf, sizeof(inbuf), fp);
        while (fgets(inbuf, sizeof(inbuf), fp))
        {
          inbuf[(strlen(inbuf) - 1)] = '\0';
          inbuf[43] = '\0';     /* truncate */
          num = counts[inbuf[0] - 'A'];
          fprintf(vtfp, "    %-42s%3d 布\n", inbuf + 3, num);
          total += num;
        }
        fclose(fp);
      }
      unlink(buf);

      fprintf(vtfp, "\n』 羆布计 = %d 布\n\n", total);
      add_sus(fpath, NULL, vtfp);                       /* Dopin 04/10/02 */
      setbfile(buf, bname, STR_bv_results);
      b_suckinfile(vtfp, buf);

      fclose(vtfp);
      Rename(b_newresults, buf);
      add_r2_board(NULL);                               /* Dopin 04/10/02 */

      pos = search_record(fn_board, &fh, sizeof(fh), cmpbnames, (int) bname);
      fh.bvote = 2;
      if(substitute_record(fn_board, &fh, sizeof(fh), pos) == -1)
        outs(err_board_update);
      touch_boards();           /* vote flag changed */
      resolve_boards();
    }
    return FULLUPDATE;
  }

  outs("ヴ龄秨﹍絪胯Ω [щ布﹙Ξ]:");
  igetch();
  setbfile(buf, bname, STR_bv_desc);
  aborted = vedit(buf, NA);
  if (aborted)
  {
    clear();
    pressanykey("Ωщ布");
    return FULLUPDATE;
  }
  sprintf(buf, "boards/%s/vote.sus", currboard);
  remove(buf);
  sprintf(buf, "boards/%s/results", currboard);
  remove(buf);
  setbfile(buf, bname, STR_bv_flags);
  unlink(buf);

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

  if(num==0)
  {
    clear();
    setbfile(buf, bname, STR_bv_control);
    unlink(buf);
    pressanykey("Ωщ布");
    return FULLUPDATE;
  }
  else
  {

    pos = search_record(fn_board, &fh, sizeof(fh), cmpbnames, (int) bname);
    fh.bvote = 1;
    fh.vtime = closetime;
    if(substitute_record(fn_board, &fh, sizeof(fh), pos) == -1)
      outs(err_board_update);
    touch_boards();             /* vote flag changed */

#ifdef  HAVE_REPORT
    b_report("OPEN");
#endif

    getdata(b_lines, 2, "琌э匡兜砞﹚郎 y/N ? ", buf, 2, LCECHO, 0);
    if(buf[0] == 'y')
       edit_vote_line(bname);

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
  return FULLUPDATE;
}

int m_vote() {
  return vote_maintain(DEFAULT_BOARD);
}

int b_vote_maintain() {
  return vote_maintain(currboard);
}
