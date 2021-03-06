/*-------------------------------------------------------*/
/* register.c   ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : user register routines                       */
/* create : 95/03/29                                     */
/* update : 02/07/10                                     */
/*-------------------------------------------------------*/

#undef VACATION  /* Dopin: 放假時才定義 但其實無多大作用 */

#include "bbs.h"

/* ------------------- */
/* password encryption */
/* ------------------- */

static char pwbuf[14];

char *genpasswd(char *pw) {
  if(pw[0]) {
    char saltc[2], c;
    int i;

    i = 9 * getpid();
    saltc[0] = i & 077;
    saltc[1] = (i >> 6) & 077;

    for(i = 0; i < 2; i++) {
      c = saltc[i] + '.';
      if(c > '9') c += 7;
      if(c > 'Z') c += 6;
      saltc[i] = c;
    }
    strcpy(pwbuf, pw);
    return crypt(pwbuf, saltc);
  }
  return "";
}

int checkpasswd(char *passwd, char *test) {
  char *pw;

  strncpy(pwbuf, test, 14);
  pw = crypt(pwbuf, passwd);
  return (!strncmp(pw, passwd, 14));
}

/* ------------------ */
/* 檢查 user 註冊情況 */
/* ------------------ */

static int bad_user_id(char *userid) {
  register char ch;

  if (strlen(userid) < 2)
    return 1;

  if(not_alpha(*userid)) return 1;

  if(!ci_strcmp(userid, str_new)) return 1;

  while(ch = *(++userid)) if(not_alnum(ch)) return 1;

  return 0;
}

/* -------------------------------- */
/* New policy for allocate new user */
/* (a) is the worst user currently  */
/* (b) is the object to be compared */
/* -------------------------------- */

static int compute_user_value(userec *urec, time_t clock) {
  int value;

  /* if (urec) has XEMPT permission, don't kick it */
  if((urec->userid[0] == '\0') || (urec->userlevel & PERM_XEMPT)) return 9999;

  value = (clock - urec->lastlogin) / 60;       /* minutes */

  /* new user should register in 30 mins */
  if(strcmp(urec->userid, str_new) == 0) return (30 - value);

#ifdef  VACATION
  return 120 * 24 * 60 - value; /* 寒暑假保存帳號 120 天 */
#else
  /* 未 login 成功者，不保留 */
  if(!urec->numlogins)  return -1;
  /* #login 少於三者，保留 10 天 */
  if(urec->numlogins <= 3) return 10 * 24 * 60 - value;

  /* 未完成註冊者，保留 30 天 / 一般情況，保留 60 天 */
  return (urec->userlevel & PERM_LOGINOK ? 60 : 30) * 24 * 60 - value;
#endif
}

static int getnewuserid(char *newuserid) {
  static char *fn_fresh = ".fresh";
  extern struct UCACHE *uidshm;
  userec utmp, zerorec;
  time_t clock;
  struct stat st;
  int fd, val, i;
  char genbuf[200];
  char genbuf2[200];

  memset(&zerorec, 0, sizeof(zerorec));
  clock = time(NULL);

  /* woju: reload shm before searchnewuser ("resolve"_ucache) */
  reload_ucache();

  /* -------------------------------------- */
  /* Lazy method : 先找尋已經清除的過期帳號 */
  /* -------------------------------------- */

  if((i = searchnewuser(0)) == 0) {
    /* ------------------------------- */
    /* 每 1 個小時，清理 user 帳號一次 */
    /* ------------------------------- */

    if((stat(fn_fresh, &st) == -1) || (st.st_mtime < clock - 3600)) {
      if((fd = open(fn_fresh, O_RDWR | O_CREAT, 0600)) == -1) return -1;

      write(fd, ctime(&clock), 25);
      close(fd);
      log_usies("CLEAN", "dated users");

      outs("尋找新帳號中, 請稍待片刻...\n\r");
      i = 0;

      while(i < MAXUSERS) {
        i++;

        if(read(fd, &utmp, sizeof(userec)) != sizeof(userec)) break;
        /* woju */
        if((val = compute_user_value(&utmp, clock)) < 0 &&
           !belong("etc/anonymous", utmp.userid)) {
           sprintf(genbuf, "#%d %-12s %15.15s %d %d %d",
             i, utmp.userid, ctime(&(utmp.lastlogin)) + 4,
             utmp.numlogins, utmp.numposts, val);

           /* Dopin 下面是超級蟲蟲 Remark */
           /*
           if (val > -1 * 60 * 24 * 365)
           {
             log_usies("CLEAN", genbuf);
             sprintf(genbuf, "home/%s", utmp.userid);
             sprintf(genbuf2, "tmp/%s", utmp.userid);
             if (dashd(genbuf) && Rename(genbuf, genbuf2))
             {
               sprintf(genbuf, "/bin/rm -fr home/%s", utmp.userid);
               system(genbuf);
             }
             if (lseek(fd, (i - 1) * sizeof(userec), SEEK_SET) != -1)
                write(fd, &zerorec, sizeof(utmp));
           }
           else
              log_usies("DATED", genbuf);
           */
        }
      }
      close(fd);
      time(&(uidshm->touchtime));
    }
  }
  if((fd = open(fn_passwd, O_RDWR | O_CREAT, 0600)) == -1) return -1;
  flock(fd, LOCK_EX);

  i = searchnewuser(1);
  if((i <= 0) || (i > MAXUSERS)) {
    flock(fd, LOCK_UN);
    close(fd);
    if(more("etc/user_full", NA) == -1)
       printf("抱歉，使用者帳號已經滿了，無法註冊新的帳號\n\r");

    val = (st.st_mtime - clock + 3660) / 60;
    printf("請等待 %d 分鐘後再試一次，祝你好運\n\r", val);
    sleep(2);
    exit(1);
  }

  sprintf(genbuf, "uid %d, %s", i, newuserid);
  log_usies("APPLY", genbuf);
  sprintf(genbuf, "home/%s", newuserid);
  if(stat(genbuf, &st) != -1) {
    sprintf(genbuf, "uid %d, %s", i, newuserid);
    log_usies("COLLISION", genbuf);
    exit(1);
  }

  strcpy(zerorec.userid, str_new);
  zerorec.lastlogin = clock;
  if(lseek(fd, sizeof(zerorec) * (i - 1), SEEK_SET) == -1) {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }
  write(fd, &zerorec, sizeof(zerorec));
  setuserid(i, zerorec.userid);
  flock(fd, LOCK_UN);
  close(fd);
  return i;
}

void new_register() {
  userec newuser;
  char passbuf[STRLEN];
  int allocid, try;
  char genbuf[200];

#if 0
  time_t now;

  /* ------------------ */
  /* 限定星期幾不准註冊 */
  /* ------------------ */

  now = time(0);
  sprintf(genbuf, "etc/no_register_%3.3s", ctime(&now));
  if(more(genbuf, NA) != -1) {
    pressanykey(NULL);
    exit(1);
  }
#endif

  memset(&newuser, 0, sizeof(newuser));

  more("etc/register", NA);
  try = 0;
  while(1) {
    if(++try >= 6) {
      pressanykey("您嘗試錯誤的輸入太多，請下次再來吧");
      refresh();
      oflush();
      exit(1);
    }
    getdata(0, 0, msg_uid, newuser.userid, IDLEN + 1, DOECHO, 0);

    if(bad_user_id(newuser.userid))
      outs("無法接受這個代號，請使用英文字母，並且不要包含空格\n");
    else if(searchuser(newuser.userid))
      outs("此代號已經有人使用\n");

    /* Prince: 系統損壞事件暫時性的處理 */
    else if(belong("etc/fixlist",newuser.userid)) {
      char ans[4];
      outs("[警告]此ID在本次系統損壞保管名單中!!若非原所有人註冊,站方將有權刪除!!\n");
      getdata(0, 0, "確定要以此ID註冊 (y/N) ?", ans, 4, LCECHO, 0);
      if(*ans == 'y') break;
    }
    /* Prince: 等事件過後可將此段刪除 */
    else break;
  }

  try = 0;
  while(1) {
    if(++try >= 6) {
      pressanykey("您嘗試錯誤的輸入太多，請下次再來吧");
      refresh();
      oflush();
      exit(1);
    }

    if((getdata(0, 0, "請設定密碼：", passbuf, PASSLEN, NOECHO, 0) < 3) ||
       !strcmp(passbuf, newuser.userid)) {
      outs("密碼太簡單，易遭入侵，至少要 4 個字，請重新輸入\n");
      continue;
    }

    strncpy(newuser.passwd, passbuf, PASSLEN);
    getdata(0, 0, "請檢查密碼：", passbuf, PASSLEN, NOECHO, 0);
    if(strncmp(passbuf, newuser.passwd, PASSLEN)) {
      outs("密碼輸入錯誤, 請重新輸入密碼.\n");
      continue;
    }

    passbuf[8] = '\0';
    strncpy(newuser.passwd, genpasswd(passbuf), PASSLEN);
    break;
  }

  newuser.userlevel = PERM_DEFAULT;
  newuser.uflag = COLOR_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
  newuser.firstlogin = newuser.lastlogin = time(NULL);

  allocid = getnewuserid(newuser.userid);

  if(allocid > MAXUSERS || allocid <= 0) {
    fprintf(stderr, "本站人口已達飽和！\n");
    exit(1);
  }

  if(substitute_record(fn_passwd, &newuser, sizeof(newuser), allocid) == -1) {
    fprintf(stderr, "客滿了，再見！\n");
    exit(1);
  }

  setuserid(allocid, newuser.userid);
  if(!dosearchuser(newuser.userid)) {
    fprintf(stderr, "無法建立帳號\n");
    exit(1);
  }
}

void check_register() {
  char *ptr;
  char genbuf[200];

  stand_title("請詳細填寫個人資料");

  while(strlen(cuser.username) < 2)
    getdata(2, 0, "綽號暱稱：", cuser.username, 24, DOECHO, 0);

  for(ptr = cuser.username; *ptr; ptr++)
    if(*ptr == 9) *ptr == ' '; /* TAB convert */

  while(strlen(cuser.realname) < 4)
    getdata(4, 0, "真實姓名：", cuser.realname, 20, DOECHO, 0);

  while(strlen(cuser.address) < 8)
    getdata(6, 0, "聯絡地址：", cuser.address, 50, DOECHO, 0);

  /* Dopin: 傳說中的直覺 GOTO 大法 ^O^;;; */
  if(!cuser.year || !cuser.month || !cuser.day) {
    int i;

B_LOOPY:
    out2line(1, 8, "");
    getdata(8, 0, "生日西元年份 [1940-1994]：", genbuf, 5, DOECHO, 0);
    i = atoi(genbuf);
    if(i < 1940 || i > 1994) goto B_LOOPY;
    else {
      cuser.year = i - 1900;
      goto B_OKM;
    }
B_LOOPM:
    move(8, 31); clrtoeol();
B_OKM:
    getdata(8, 31, "幾月 [01-12]：", genbuf, 3, DOECHO, 0);
    i = atoi(genbuf);
    if(i < 1 || i > 12) goto B_LOOPM;
    else {
      cuser.month = i;
      goto B_OKD;
    }
B_LOOPD:
    move(8, 48); clrtoeol();
B_OKD:
    getdata(8, 48, "幾日 [01-31]：", genbuf, 3, DOECHO, 0);
    i = atoi(genbuf);
    if(i < 1 || i > 31) goto B_LOOPD;
    cuser.day = i;
  }

  if(!strchr(cuser.email, '@')) {
    bell();
    move(t_lines - 4, 0);
    prints("\
※ 為了您的權益，請填寫真實的 E-mail address， 以資確認閣下身份，\n\
   格式為 [44muser@domain_name[0m 或 [44muser@\\[ip_number\\][0m。\n\n\
※ 如果您真的沒有 E-mail，請直接按 [return] 即可。");

    do {
      getdata(10, 0, "電子信箱：", cuser.email, 50, DOECHO, 0);
      if (!cuser.email[0])
        sprintf(cuser.email, "%s%s", cuser.userid, str_mail_address);
    } while (!strchr(cuser.email, '@'));

#ifdef  EMAIL_JUSTIFY
    mail_justify(cuser);
#endif
  }

  cuser.userlevel |= PERM_DEFAULT;
  if(!HAS_PERM(PERM_SYSOP) && !(cuser.userlevel & PERM_LOGINOK)) {
    /* 回覆過身份認證信函，或曾經 E-mail post 過 */

    setuserfile(genbuf, "email");
    if(dashf(genbuf)) {
      /* Leeym /
      cuser.userlevel |= ( PERM_POST | PERM_LOGINOK );
      /* 關於權限各站請依自己規定.蔽站採嚴格認證.未確認不得 post */
/*
comment out by woju (already done by mailpost)
      strcpy(cuser.justify, cuser.email);
      改進原先 emailreply 確認後確認資料仍空白
*/
      unlink(genbuf);
      /* 身份確認成功後發一封信告知已完成確認 */
      {
        fileheader mhdr;
        char title[128], buf1[80];
        FILE* fp;

        sethomepath(buf1, cuser.userid);
        stampfile(buf1, &mhdr);
        strcpy(mhdr.owner, "站長");
        strncpy(mhdr.title, "[註冊成功\]", TTLEN);
        mhdr.savemode = mhdr.filemode = 0;

        sethomedir(title, cuser.userid);
        append_record(title, &mhdr, sizeof(mhdr));
        Link("etc/registered", buf1);

        cuser.userlevel |= PERM_LOGINOK;
      }
    }

#ifdef  STRICT
    else {
      cuser.userlevel &= ~PERM_POST;
      more("etc/justify", YEA);
    }
#endif
  }

#ifndef MIN_USEREC_STRUCT
  while(cuser.classsex < 1 || cuser.classsex > 3) {
     getdata(12, 0, "性別 (1)男 (2)女 (3)不詳 : ", genbuf, 2, DOECHO, 0);
     cuser.classsex = atoi(genbuf);
  }
#endif

#ifdef  NEWUSER_LIMIT
  if(!(cuser.userlevel & PERM_LOGINOK) && !HAS_PERM(PERM_SYSOP)) {
    if(cuser.lastlogin - cuser.firstlogin < 3 * 86400)
      cuser.userlevel &= ~PERM_POST;
    more("etc/newuser", YEA);
  }
#endif

  if(HAS_PERM(PERM_DENYPOST) && !HAS_PERM(PERM_SYSOP))
     cuser.userlevel &= ~PERM_POST;
}
