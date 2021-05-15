/*-------------------------------------------------------*/
/* vote.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : boards' vote routines                        */
/* create : 95/03/29                                     */
/* update : 02/08/19 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"
#include <time.h>

long atol();
extern int cmpbnames();
extern int numboards;

static char STR_bv_control[] = "control";
static char STR_bv_desc[] = "desc";
static char STR_bv_ballots[] = "ballots";
static char STR_bv_results[] = "results";
static char STR_bv_flags[] = "flags";
static char STR_bv_tickets[] = "tickets";

void b_suckinfile(FILE *fp, char *fname) {
  FILE *sfp;

  if(sfp = fopen(fname, "r")) {
    char inbuf[256];

    while (fgets(inbuf, sizeof(inbuf), sfp)) fputs(inbuf, fp);
    fclose(sfp);
  }
}

int b_closepolls() {
  static char *fn_vote_polling = ".voting";
  boardheader fh;
  struct stat st;
  FILE *cfp;
  char buf[80];
  time_t now;
  int fd, dirty;

  now = time(NULL);

  /*
  if(!strcmp(cuser.userid, "Dopin")) {
     move(0, 0);
     prints(".%d. .%d. .%d.", stat(fn_vote_polling, &st),
             st.st_mtime, now);
     igetkey();
  }
  */

  if (stat(fn_vote_polling, &st) == -1 || st.st_mtime > now - 3600)
    return 0;

  if ((cfp = fopen(fn_vote_polling, "w")) == NULL)
    return 0;

//  if(!strcmp(cuser.userid, "Dopin")) { outs("OK"); igetkey(); }

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
      lseek(fd, sizeof(fh) * (getbnum(fh.brdname) - 1), SEEK_SET);
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

void add_r2_board(char *board_name) {                    /* Dopin 04/10/02 */
   fileheader vr;
   char title[80], fpath[80], vbuf[100], genbuf[100], buf[30];

   if(board_name == NULL) strcpy(buf, currboard);
   else strcpy(buf, board_name);

   sprintf(fpath, "boards/%s/results", buf);
   sprintf(title, "boards/%s", buf);

   stampfile(title, &vr);

   vr.savemode = 'M';
   vr.filemode = FILE_READ;
   strcpy(vr.owner, "[SYSTEM]");
   strcpy(vr.title, "== §ë²¼µ²ªG ==");

   sprintf(genbuf, "boards/%s/.DIR", buf);
   append_record(genbuf, &vr, sizeof(vr));

   sprintf(vbuf, "cp %s %s", fpath, title);
   system(vbuf);
}

int add_sus(char *fpath, char *bname, FILE *vtfp) {     /* Dopin 04/08/02 */
   char buf[30];
   FILE *fp;

   if(vtfp == NULL) return -1;

   if(bname == NULL) strcpy(buf, currboard);
   else strcpy(buf, bname);

   sprintf(fpath, "boards/%s/vote.sus", buf);
   fp = fopen(fpath, "r");
   if(!fp) return -1;

   fprintf(vtfp, "¤j®aªº·N¨£ : \n");
   while(fgets(fpath, 80, fp) && !ferror(fp)) {
      if(!feof(fp) && !ferror(fp))
         fprintf(vtfp, "%s", fpath);
      else
         break;
   }
   fclose(fp);

   sprintf(fpath, "boards/%s/vote.sus", buf);
   remove(fpath);

   return 0;
}

int
b_close(fh)
  boardheader *fh;
{
  FILE *cfp, *tfp;
  char *bname = fh->brdname;
  char buf[STRLEN];
  char inchar, inbuf[80];
  int counts[31], fd;
  int total, num ,bid;
  char b_control[64];
  char b_newresults[64];
  time_t now;
  struct stat st;

  now = time(NULL);

    if(!strcmp(cuser.userid, "Dopin")) {  /* Debug */
       move(0, 0);
       prints(".%s.  .%d.  .%d.",
              fh->brdname, fh->vtime, now);
       igetkey();
    }

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
      counts[(int) (inchar - 'A')]++;
    close(fd);
  }
  unlink(buf);

  setbfile(b_newresults, bname, "newresults");
  tfp = fopen(b_newresults, "w");
  if(!tfp) return 1;

  fprintf(tfp, "%s\n¡» §ë²¼¤¤¤î©ó: %s\n¡» ²¼¿ïÃD¥Ø´y­z:\n\n",
    msg_seperator, ctime(&fh->vtime));

  setbfile(buf, bname, STR_bv_desc);
  b_suckinfile(tfp, buf);
  unlink(buf);

  fprintf(tfp, "¡» §ë²¼µ²ªG:\n\n");
  total = 0;
  if (cfp = fopen(b_control, "r"))
  {
    fgets(inbuf, sizeof(inbuf), cfp);
    while (fgets(inbuf, sizeof(inbuf), cfp))
    {
      inbuf[(strlen(inbuf) - 1)] = '\0';
      inbuf[43] = '\0';         /* truncate */
      num = counts[inbuf[0] - 'A'];
      fprintf(tfp, "    %-42s%3d ²¼\n", inbuf + 3, num);
      total += num;
    }
    fclose(cfp);
  }
  unlink(b_control);

  fprintf(tfp, "\n¡» Á`²¼¼Æ = %d ²¼\n\n", total);
  add_sus(inbuf, bname, tfp);                           /* Dopin 04/10/02 */
  setbfile(buf, bname, STR_bv_results);
  b_suckinfile(tfp, buf);

  fclose(tfp);
  Rename(b_newresults, buf);
  add_r2_board(bname);                                  /* Dopin 04/10/02 */

  return 1;
}

int edit_vote_line(char *bname) {
   char buf[100];
   int aborted;

   setbfile(buf, bname, STR_bv_control);
   aborted = vedit(buf, NA);

   return FULLUPDATE;
}

int vote_flag(char *bname, char val) {
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

#ifdef  SYS_VOTE
int x_vote() {
  return user_vote(DEFAULT_BOARD);
}

int x_results() {
  return vote_results(DEFAULT_BOARD);
}
#endif                          /* SYS_VOTE */

#ifdef  HAVE_REPORT
b_report(s)
  char *s;
{
  static int disable = NA;
  int fd;

  if (disable)
    return;
  if ((fd = open("trace.bvote", O_WRONLY, 0664)) != -1)
  {
    char buf[512];
    char timestr[18], *thetime;
    time_t dtime;

    time(&dtime);
    thetime = ctime(&dtime);
    strncpy(timestr, &(thetime[4]), 15);
    timestr[15] = '\0';
    flock(fd, LOCK_EX);
    lseek(fd, 0, L_XTND);
    sprintf(buf, "%s %-12s %-20s %s\n", timestr, cuser.userid, currboard, s);
    write(fd, buf, strlen(buf));
    flock(fd, LOCK_UN);
    close(fd);
    return;
  }
  disable = YEA;
  return;
}
#endif                          /* HAVE_REPORT */
