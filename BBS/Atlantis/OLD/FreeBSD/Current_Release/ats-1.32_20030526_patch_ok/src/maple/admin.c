/*-------------------------------------------------------*/
/* admin.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : administration routines                      */
/* create : 95/03/29                                     */
/* update : 02/09/11 (Dopin)                             */
/*-------------------------------------------------------*/

#define _ADMIN_C_

#include "bbs.h"

extern char *globalboard[];
extern char global_board_cn;
extern char station_list[][13];

/* ----------------------------------------------------- */
/* ¬ÝªOºÞ²z                                              */
/* ----------------------------------------------------- */

int is_ulock(char ch) {
   int flag = 0;
   char ci;
   char ulock_key[3] = { 'E', 'R', 'S' };

   for(ci = 0 ; ci < 3 ; ci++)
      if(ch == ulock_key[ci])
         flag = 1;

   return flag;
}

unsigned setperms(usint pbits, char *userid) {
  char chi, chg=0, genbuf[3], *chc[] = { "¡¼", "¡½" };
  int i;
  usint tperm;

  tperm = pbits;
#ifndef NO_USE_MULTI_STATION
  if(cuser.extra_mode[4] == 1) access_mask('l', &tperm, userid);
#endif

  while(1) {
     clear();
     out2line(1, 1, "    ¡¼ ªí¥ÜÃö³¬Åv­­   ¡½ ªí¥Ü¶}±ÒÅv­­");
     for(chi = 0 ; chi < 31 ; ) {
        move(3 + chi/2, 0);
        for(i = 0 ; i < 2 ; i++) {
           prints(" %2d %s  %s%-31s[m",
           chi+1, &chc[(power(2 ,(int)chi) & tperm) / power(2 ,(int)chi)][0],
           (!HAS_PERM(PERM_SYSOP)
#ifdef USE_ACCOUNTS_PERM
           && !HAS_PERM(PERM_ACCOUNTS) /* ¬ÝªOÁ`ºÞ´N¤£ºÞ¤F §_«h¤Ó½ÆÂø QQ */
#endif
           && chi <= 16) ? "[0;37;40m" : "",
           permstrings[chi]);
           chi++;
        if(chi >= 31) break;
        }
     }

     out2line(1, b_lines, "");
     getdata(b_lines - 3, 20, "½Ð¿ï¾Ü­nÅÜ§óªºÅv­­¸¹½X (Q/Enter Â÷¶}) : ",
             genbuf, 3, LCECHO, 0);
     if(genbuf[0] == 'q' || !genbuf[0]) break;

     i = atoi(genbuf);
     if(i >= 1 && i <= 31) {
        if(HAS_PERM(PERM_SYSOP)) goto P_CHECK;

#ifdef USE_ACCOUNTS_PERM
        if(HAS_PERM(PERM_ACCOUNTS)) {
           if(i >= 12 && i <= 16) continue;
           goto P_CHECK;
           /* ±b¸¹Á`ºÞ¤£¯à°Ê¨t²Î»P¯¸°È¬ÛÃöÅv­­ */
        }
#endif

#ifdef USE_BOARDS_MANAGER
        if(HAS_PERM(PERM_EXTRA1)) {
           if(!cuser.extra_mode[7]) {
              if(i != 12 && i <= 17) continue;
           }
           else if(i != 11) continue;
           goto P_CHECK;
       /* ¬ÝªOÁ`ºÞ¤£¯à°Ê¨t²Î»P¯¸°È¬ÛÃöÅv­­ (¦pªG¨â­Ó³£¶} ³o¨â¬q¥i¥H¦X°_¨Ó) */
       /* ¤S ¦pªG¥u¬O°Ê¨ì¨Ï¥ÎªÌÅv­­ ¨º´N¥u¯à°ÊªO¥DÅv­­ */
        }
#endif

        if(!HAS_PERM(PERM_SYSOP)) {
           if(i <= 17) continue;
        }

P_CHECK:
        if(power(2, i-1) & tperm) bitwr(&tperm, i-1, '-');
        else bitwr(&tperm, i-1, '+');
        chg = 1;
     }
  }
  pbits = tperm;

  return (pbits);
}

int is_global_board(char *boardname) {
   char flag = 0;
   char ci;

   for(ci = 0 ; ci < global_board_cn ; ci++)
      if(!strcmp(globalboard[ci], boardname)) {
         flag = 1;
         break;
      }

   return (int)flag;
}

int is_station_sysop(void) {
   char flag = 0;
   char ci;
   char buf[50];
   FILE *fp;

   if(HAS_PERM(PERM_SYSOP)) {
      flag = 1;
      goto CHECK_SYSOP_OVER;
   }
#ifdef NO_USE_MULTI_STATION
   else goto CHECK_SYSOP_OVER;
#endif

#ifdef SYSOP_ONLY_DEFAULTST              /* ­ì©w¸q©ó config.h              */
   if(!cuser.now_stno)                   /* ¦pµL©w¸q SYSOP_ONLY_DEFAULT¸   */
      goto CHECK_SYSOP_OVER;             /* «h¥u­n«ü©w run/ ¤U¬ÛÃöÀÉ®×     */
                                         /* «D¨ã¦³ SYSOP Åv­­ªº¥ç¥i¬°¯¸ªø  */
#endif                                   /* «ØÄ³©w¸q ¥H§Kµo¥Í¦w¥þ°ÝÃD      */

   /* ¨S¦³¤À¯¸ªøÅv­­ ¸õÂ÷ */
   if(!HAS_PERM(PERM_BOARD)) goto CHECK_SYSOP_OVER;

   /* ®Ö¹ï run ¤Uªº¯¸ºÞ³]©w */
   sprintf(buf, BBSHOME "/run/%s.sysop_name",
#ifndef NO_USE_MULTI_STATION
   cuser.station
#else
   DEFAULTST
#endif
   );
   if(belong(buf, cuser.userid)) {
      flag = 1;
      goto CHECK_SYSOP_OVER;
   }

CHECK_SYSOP_OVER:
   return (int)flag;
}

#ifdef HAVE_TIN
int post_in_tin(char *username) {
  char buf[256];
  FILE *fh;
  int counter = 0;

  sethomefile(buf, username, ".tin/posted");
  fh = fopen(buf, "r");
  if (fh == NULL)
    return 0;
  else
  {
    while (fgets(buf, 255, fh) != NULL)
    {
      if (buf[9] != 'd' && strncmp(&buf[11], "csie.bbs.test", 13))
        counter++;
      if (buf[9] == 'd')
        counter--;
    }
    fclose(fh);
    return counter;
  }

}
#endif

/* ----------------------------------------------------- */
/* ²M°£¨Ï¥ÎªÌ«H½c                                        */
/* ----------------------------------------------------- */

#ifdef  HAVE_MAILCLEAN
FILE *cleanlog;
char curruser[IDLEN + 2];
extern int delmsgs[];
extern int delcnt;

static int domailclean(fileheader *fhdrp) {
  static int newcnt, savecnt, deleted, idc;
  char buf[STRLEN];

  if (!fhdrp)
  {
    fprintf(cleanlog, "new = %d, saved = %d, deleted = %d\n",
      newcnt, savecnt, deleted);
    newcnt = savecnt = deleted = idc = 0;
    if (delcnt)
    {
      sethomedir(buf, curruser);
      while (delcnt--)
        delete_record(buf, sizeof(fileheader), delmsgs[delcnt]);
    }
    delcnt = 0;
    return 1;
  }
  idc++;
  if (!(fhdrp->filemode & FILE_READ))
    newcnt++;
  else if (fhdrp->filemode & FILE_MARKED)
    savecnt++;
  else
  {
    deleted++;
    sethomefile(buf, curruser, fhdrp->filename);
    unlink(buf);
    delmsgs[delcnt++] = idc;
  }
  return 0;
}

static int cleanmail(userec *urec) {
  struct stat statb;
  char genbuf[200];

  if (urec->userid[0] == '\0' || !strcmp(urec->userid, str_new))
    return;
  sethomedir(genbuf, urec->userid);
  fprintf(cleanlog, "%s¡G", urec->userid);
  if (stat(genbuf, &statb) == -1 || statb.st_size == 0)
    fprintf(cleanlog, "no mail\n");
  else
  {
    strcpy(curruser, urec->userid);
    delcnt = 0;
    apply_record(genbuf, domailclean, sizeof(fileheader));
    domailclean(NULL);
  }
  return 0;
}

int m_mclean() {
  char ans[4];
  if(!is_station_sysop())
     return FULLUPDATE;

  getdata(b_lines - 1, 0, msg_sure_ny, ans, 4, LCECHO, 0);
  if (ans[0] != 'y')
    return FULLUPDATE;

  cleanlog = fopen("mailclean.log", "w");
  outmsg("This is variable msg_working!");

  move(b_lines - 1, 0);
  if (apply_record(fn_passwd, cleanmail, sizeof(userec)) == -1)
  {
    outs(ERR_PASSWD_OPEN);
  }
  else
  {
    fclose(cleanlog);
    outs("²M°£§¹¦¨! °O¿ýÀÉ mailclean.log.");
  }
  return FULLUPDATE;
}
#endif  HAVE_MAILCLEAN

/* ----------------------------------------------------- */
/* ²£¥Í°lÂÜ°O¿ý¡G«ØÄ³§ï¥Î log_usies()¡BTRACE()           */
/* ----------------------------------------------------- */

#ifdef  HAVE_REPORT
void report(char *s) {
  static int disable = NA;
  int fd;

  if (disable)
    return;

  if ((fd = open("trace", O_WRONLY, 0664)) != -1)
  {
    char buf[256];
    char *thetime;
    time_t dtime;

    time(&dtime);
    thetime = Cdate(&dtime);
    flock(fd, LOCK_EX);
    lseek(fd, 0, L_XTND);
    sprintf(buf, "%s %s %s\n", cuser.userid, thetime, s);
    write(fd, buf, strlen(buf));
    flock(fd, LOCK_UN);
    close(fd);
  }
  else
    disable = YEA;
}

int m_trace() {
  struct stat bstatb, ostatb, cstatb;
  int btflag, otflag, ctflag, done = 0;
  char ans[2];
  char *msg;

  clear();
  move(0, 0);
  outs("Set Trace Options");
  clrtobot();
  while (!done)
  {
    move(2, 0);
    otflag = stat("trace", &ostatb);
    ctflag = stat("trace.chatd", &cstatb);
    btflag = stat("trace.bvote", &bstatb);
    outs("Current Trace Settings:\n");
    if (otflag)
      outs("Normal tracing is OFF\n");
    else
      prints("Normal tracing is ON (size = %d)\n", ostatb.st_size);
    if (ctflag)
      outs("Chatd  tracing is OFF\n");
    else
      prints("Chatd  tracing is ON (size = %d)\n", cstatb.st_size);
    if (btflag)
      outs("BVote  tracing is OFF\n");
    else
      prints("BVote  tracing is ON (size = %d)\n", bstatb.st_size);

    move(8, 0);
    outs("Enter:\n");
    prints("<1> to %s Normal tracing\n", otflag ? "enable " : "disable");
    prints("<2> to %s Chatd  tracing\n", ctflag ? "enable " : "disable");
    prints("<3> to %s BVote  tracing\n", btflag ? "enable " : "disable");
    getdata(12, 0, "Anything else to exit¡G", ans, 2, DOECHO, 0);

    switch (ans[0])
    {
    case '1':
      if (otflag)
      {
        /* woju */
        system("touch trace");
        msg = "BBS   tracing enabled.";
        report("opened report log");
      }
      else
      {
        report("closed report log");
        system("/bin/mv trace trace.old");
        msg = "BBS   tracing disabled; log is in trace.old";
      }
      break;

    case '2':
      if (ctflag)
      {
        system("touch trace.chatd");
        msg = "Chat  tracing enabled.";
        report("chatd trace log opened");
      }
      else
      {
        system("/bin/mv trace.chatd trace.chatd.old");
        msg = "Chat  tracing disabled; log is in trace.chatd.old";
        report("chatd trace log closed");
      }
      break;

    case '3':
      if (btflag)
      {
        system("touch trace.bvote");
        msg = "BVote tracing enabled.";
        report("BVote trace log opened");
      }
      else
      {
        system("/bin/mv trace.bvote trace.bvote.old");
        msg = "BVote tracing disabled; log is in trace.bvote.old";
        report("BoardVote trace log closed");
      }
      break;

    default:
      msg = NULL;
      done = 1;
    }
    move(t_lines - 2, 0);
    if (msg)
      prints("%s\n", msg);
  }
  clear();
}
#endif                          /* HAVE_REPORT */
