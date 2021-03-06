#include "bbs.h"

static char STR_bv_control[] = "control";
static char STR_bv_desc[] = "desc";
static char STR_bv_ballots[] = "ballots";
static char STR_bv_tickets[] = "tickets";

int do_suess(void) {
   FILE *fp;
   char genbuf[100], fpath[80];
   time_t now;
   int i, lines = 0;

   move(b_lines - 4, 0);
   outs("有沒有任何意見要留言的 ? (至多三行) ");
   getdata(b_lines - 3, 0, "", genbuf, 80, DOECHO, 0);

   if(!genbuf[0])
      return DONOTHING;

   sprintf(fpath, "boards/%s/vote.sus", currboard);
   now = time(NULL);
   fp = fopen(fpath, "a+");
   fprintf(fp, "[1;31;46m%s [1;37;46m於 [1;33;46m%s[1;37;46m 說道 : [m\n",
           cuser.userid, Cdate(&now));
   fprintf(fp, "%s\n", genbuf);
   if(!genbuf[0])
      goto FINISH_SUS;

   lines++;
   getdata(b_lines - 2, 0, "", genbuf, 80, DOECHO, 0);
   fprintf(fp, "%s\n", genbuf);
   if(!genbuf[0])
      goto FINISH_SUS;

   lines++;
   getdata(b_lines - 1, 0, "", genbuf, 80, DOECHO, 0);
   fprintf(fp, "%s\n", genbuf);
   if(!genbuf[0])
      goto FINISH_SUS;

   lines++;
FINISH_SUS:
   for(i = 3 ; i > lines ; i--)
      fprintf(fp, "%s\n", genbuf);

   fclose(fp);

   return FULLUPDATE;
}

int user_vote(char *bname) {
  FILE *cfp;
  char buf[STRLEN];
  char inbuf[80], choices[31], vote[2], bufvote[31];
  int i = 0, tickets;
  int fd, count = 0;
  time_t closetime;

  setbfile( buf, bname, STR_bv_tickets);
  if((cfp= fopen(buf, "r")) == NULL) tickets=1;
  else {
    fgets(inbuf, sizeof(inbuf), cfp);
    tickets = atoi(inbuf);
    fclose(cfp);
  }

  setbfile(buf, bname, STR_bv_control);
  move(3, 0);
  clrtobot();
  if((cfp = fopen(buf, "r")) == NULL) {
    pressanykey("目前並沒有任何投票舉行。");
    return FULLUPDATE;
  }

  if(!HAS_PERM(PERM_BASIC)) {
    fclose(cfp);
    pressanykey("對不起! 您未滿二十歲, 還沒有投票權喔!");
    return FULLUPDATE;
  }

  setutmpmode(VOTING);
  setbfile(buf, bname, STR_bv_desc);
  more(buf, YEA);

  stand_title("投票箱");
  fgets(inbuf, sizeof(inbuf), cfp);
  closetime = (time_t) atol(inbuf);

  prints("投票方式：確定好您的選擇後，輸入其代碼(A, B, C...)即可。\n"
    "此次投票你可以投 %1d 票。"
    "[1;30;41m 按 0 取消投票 [m [1;37;42m 按 1 完成投票 [m\n"
    "此次投票將結束於：%s \n",
    tickets, ctime(&closetime));
  move(5, 0);
  memset(choices, 0, sizeof(choices));
  while (fgets(inbuf, sizeof(inbuf), cfp)) {
    choices[count++] = inbuf[0];
    move(((count-1)%15)+5,((count-1)/15)*40);
    prints( " %s", strtok(inbuf,"\n\0"));
  }
  fclose(cfp);

  if(vote_flag(bname, '\0')) {
    pressanykey("此次投票，你已投過了！一人一次，大家平等。");
    return FULLUPDATE;
  }

  while(1) {
    vote[0] = vote[1] = '\0';
    move(t_lines-2, 0);
    prints("你還可以投 %2d 票", tickets-i);
    getdata(t_lines-4, 0, "輸入您的選擇: ", vote, 3, DOECHO, 0);

    move(t_lines - 2, 0);
    *vote = toupper(*vote);
    if(vote[0] == '0' || (!vote[0] && !i)) {
      clrtoeol();
      outs("[5m記的再來投喔!!      [m");
      refresh();
      break;
    }
    else if(vote[0] == '1' && i) ;
    else if(!vote[0]) continue;
    else if(index(choices, vote[0]) == NULL) continue; /* 無效 */
    else if(same(vote[0], bufvote, i)) {
      move(((vote[0]-'A')%15)+5, (((vote[0]-'A'))/15)*40);
      prints(" ");
      i--;
      continue;
    }
    else {
      if(i == tickets) continue;
      bufvote[i] = vote[0];
      move(((vote[0]-'A')%15)+5, (((vote[0]-'A'))/15)*40);
      prints("*");
      i++;
      continue;
    }

    if(vote_flag(bname, vote[0]) != 0) outs("重覆投票! 不予計票。");
    else {
      setbfile(buf, bname, STR_bv_ballots);
      if((fd = open(buf, O_WRONLY | O_CREAT | O_APPEND, 0600)) == 0)
        outs("無法投入票匭\n");
      else {
        struct stat statb;

        flock(fd, LOCK_EX);
        write(fd, bufvote, i);
        flock(fd, LOCK_UN);
        fstat(fd, &statb);
        close(fd);
        move(t_lines-2,0);
        prints("已完成投票！(目前已投票數: %d)\n", statb.st_size);
        do_suess();
      }
    }
    break;
  }
  pressanykey(NULL);
  return FULLUPDATE;
}

int b_vote() {
  return user_vote(currboard);
}
