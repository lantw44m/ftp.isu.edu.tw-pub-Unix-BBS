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
   outs("���S������N���n�d���� ? (�ܦh�T��) ");
   getdata(b_lines - 3, 0, "", genbuf, 80, DOECHO, 0);

   if(!genbuf[0])
      return DONOTHING;

   sprintf(fpath, "boards/%s/vote.sus", currboard);
   now = time(NULL);
   fp = fopen(fpath, "a+");
   fprintf(fp, "[1;31;46m%s [1;37;46m�� [1;33;46m%s[1;37;46m ���D : [m\n",
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
    pressanykey("�ثe�èS������벼�|��C");
    return FULLUPDATE;
  }

  if(!HAS_PERM(PERM_BASIC)) {
    fclose(cfp);
    pressanykey("�藍�_! �z�����G�Q��, �٨S���벼�v��!");
    return FULLUPDATE;
  }

  setutmpmode(VOTING);
  setbfile(buf, bname, STR_bv_desc);
  more(buf, YEA);

  stand_title("�벼�c");
  fgets(inbuf, sizeof(inbuf), cfp);
  closetime = (time_t) atol(inbuf);

  prints("�벼�覡�G�T�w�n�z����ܫ�A��J��N�X(A, B, C...)�Y�i�C\n"
    "�����벼�A�i�H�� %1d ���C"
    "[1;30;41m �� 0 �����벼 [m [1;37;42m �� 1 �����벼 [m\n"
    "�����벼�N������G%s \n",
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
    pressanykey("�����벼�A�A�w��L�F�I�@�H�@���A�j�a�����C");
    return FULLUPDATE;
  }

  while(1) {
    vote[0] = vote[1] = '\0';
    move(t_lines-2, 0);
    prints("�A�٥i�H�� %2d ��", tickets-i);
    getdata(t_lines-4, 0, "��J�z�����: ", vote, 3, DOECHO, 0);

    move(t_lines - 2, 0);
    *vote = toupper(*vote);
    if(vote[0] == '0' || (!vote[0] && !i)) {
      clrtoeol();
      outs("[5m�O���A�ӧ��!!      [m");
      refresh();
      break;
    }
    else if(vote[0] == '1' && i) ;
    else if(!vote[0]) continue;
    else if(index(choices, vote[0]) == NULL) continue; /* �L�� */
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

    if(vote_flag(bname, vote[0]) != 0) outs("���Ч벼! �����p���C");
    else {
      setbfile(buf, bname, STR_bv_ballots);
      if((fd = open(buf, O_WRONLY | O_CREAT | O_APPEND, 0600)) == 0)
        outs("�L�k��J���o\n");
      else {
        struct stat statb;

        flock(fd, LOCK_EX);
        write(fd, bufvote, i);
        flock(fd, LOCK_UN);
        fstat(fd, &statb);
        close(fd);
        move(t_lines-2,0);
        prints("�w�����벼�I(�ثe�w�벼��: %d)\n", statb.st_size);
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
