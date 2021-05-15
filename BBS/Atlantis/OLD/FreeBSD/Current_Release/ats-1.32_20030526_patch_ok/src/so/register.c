#include "bbs.h"

/* ----------------------------------------------------- */
/* 處理 Register Form                                    */
/* ----------------------------------------------------- */

scan_register_form(char *regfile) {
  char genbuf[200];
  static char *logfile = "register.log";
  static char *field[] = {"num", "uid", "name", "career",
  "addr", "phone", "email", NULL};
  static char *finfo[] = {"帳號位置", "申請代號", "真實姓名", "服務單位",
  "目前住址", "連絡電話", "電子郵件信箱", NULL};
  static char *reason[] = {"輸入真實姓名", "詳填學校科系與年級",
    "填寫完整的住址資料", "詳填連絡電話", "確實填寫註冊申請表",
  "用中文填寫申請單", NULL};

  userec muser;
  FILE *fn, *fout, *freg;
  char fdata[7][STRLEN];
  char fname[STRLEN], buf[STRLEN];
  char ans[4], *ptr, *uid;
  int n, unum;

  uid = cuser.userid;
  sprintf(fname, "%s.tmp", regfile);
  move(2, 0);
  if (dashf(fname))
  {
    pressanykey("其他 SYSOP 也在審核註冊申請單");
    return -1;
  }
  Rename(regfile, fname);
  if ((fn = fopen(fname, "r")) == NULL)
  {
    pressanykey("系統錯誤，無法讀取註冊資料檔: %s", fname);
    return -1;
  }

  memset(fdata, 0, sizeof(fdata));
  while (fgets(genbuf, STRLEN, fn))
  {
    if (ptr = (char *) strstr(genbuf, ": "))
    {
      *ptr = '\0';
      for (n = 0; field[n]; n++)
      {
        if (strcmp(genbuf, field[n]) == 0)
        {
          strcpy(fdata[n], ptr + 2);
          if (ptr = (char *) strchr(fdata[n], '\n'))
            *ptr = '\0';
        }
      }
    }
    else if ((unum = getuser(fdata[1])) == 0)
    {
      move(2, 0);
      clrtobot();
      outs("系統錯誤，查無此人\n\n");
      for (n = 0; field[n]; n++)
        prints("%s     : %s\n", finfo[n], fdata[n]);
      pressanykey(NULL);
    }
    else
    {
      memcpy(&muser, &xuser, sizeof(muser));
      move(1, 0);
      prints("帳號位置    ：%d\n", unum);
      user_display(&muser, 1);

      move(14, 0);
      printdash(NULL);
      for (n = 0; field[n]; n++)
        prints("%-12s：%s\n", finfo[n], fdata[n]);
      if (muser.userlevel & PERM_LOGINOK)
      {
        getdata(b_lines - 1, 0, "此帳號已經完成註冊, 更新(Y/N/Skip)？ ",
          ans, 3, LCECHO, "N");
        if (ans[0] != 'y' && ans[0] != 's')
          ans[0] = 'd';
      }
      else
      {
        getdata(b_lines - 1, 0, "是否接受此資料(Y/N/Q/Del/Skip)？ [S]",
          ans, 3, LCECHO, 0);
      }
      move(2, 0);
      clrtobot();

      switch (ans[0])
      {
      case 'y':

        prints("以下使用者資料已經更新:\n");
        muser.userlevel |= (PERM_LOGINOK | PERM_POST);
        strcpy(muser.realname, fdata[2]);
        strcpy(muser.address, fdata[4]);
        strcpy(muser.email, fdata[6]);
        sprintf(genbuf, "%s:%s:%s", fdata[5], fdata[3], uid);
        strncpy(muser.justify, genbuf, REGLEN);
        sethomefile(buf, muser.userid, "justify");

        if (fout = fopen(buf, "a"))
        {
          fprintf(fout, "%s\n", genbuf);
          fclose(fout);
        }
        substitute_record(fn_passwd, &muser, sizeof(muser), unum);

        if (fout = fopen(logfile, "a"))
        {
          for (n = 0; field[n]; n++)
            fprintf(fout, "%s: %s\n", field[n], fdata[n]);
          n = time(NULL);
          fprintf(fout, "Date: %s\n", Cdate(&n));
          fprintf(fout, "Approved: %s\n", uid);
          fprintf(fout, "----\n");
          fclose(fout);
        }
        /* woju */
        {
          fileheader mhdr;
          char title[128], buf1[80];
          FILE* fp;

          sethomepath(buf1, muser.userid);
          stampfile(buf1, &mhdr);
          strcpy(mhdr.owner, cuser.userid);
          strncpy(mhdr.title, "[註冊成功\]", TTLEN);
          mhdr.savemode = 0;
          mhdr.filemode = 0;
          sethomedir(title, muser.userid);
          append_record(title, &mhdr, sizeof(mhdr));
          Link("etc/registered", buf1);
        }

        break;

      case 'q':         /* 太累了，結束休息 */

        if (freg = fopen(regfile, "a"))
        {
          for (n = 0; field[n]; n++)
            fprintf(freg, "%s: %s\n", field[n], fdata[n]);
          fprintf(freg, "----\n");
          while (fgets(genbuf, STRLEN, fn))
            fputs(genbuf, freg);
          fclose(freg);
        }

      case 'd':
        break;

      case 'n':

        for (n = 0; field[n]; n++)
          prints("%s: %s\n", finfo[n], fdata[n]);
        move(9, 0);
        prints("請提出退回申請表原因，按 <enter> 取消\n");
        for (n = 0; reason[n]; n++)
          prints("%d) 請%s\n", n, reason[n]);

        /* woju */
        if (getdata(10 + n, 0, "退回原因：", buf, 60, DOECHO, 0))
        {
          int i;
          fileheader mhdr;
          char title[128], buf1[80];
          FILE* fp;

          i = buf[0] - '0';
          if (i >= 0 && i < n)
            strcpy(buf, reason[i]);
          sprintf(genbuf, "[退回原因] 請%s", buf);

          sethomepath(buf1, muser.userid);
          stampfile(buf1, &mhdr);
          strcpy(mhdr.owner, "站長");
          strncpy(mhdr.title, "[註冊失敗]", TTLEN);
          mhdr.savemode = 0;
          mhdr.filemode = 0;
          sethomedir(title, muser.userid);

          if (append_record(title, &mhdr, sizeof(mhdr)) != -1) {
             fp = fopen(buf1, "w");
             fprintf(fp, "%s\n", genbuf);
             fclose(fp);
          }

          if (fout = fopen(logfile, "a"))
          {
            for (n = 0; field[n]; n++)
              fprintf(fout, "%s: %s\n", field[n], fdata[n]);
            n = time(NULL);
            fprintf(fout, "Date: %s\n", Cdate(&n));
            fprintf(fout, "Rejected: %s [%s]\n----\n", uid, buf);
            fclose(fout);
          }
          break;
        }
        move(10, 0);
        clrtobot();
        prints("取消退回此註冊申請表");

      default:                  /* put back to regfile */

        if (freg = fopen(regfile, "a"))
        {
          for (n = 0; field[n]; n++)
            fprintf(freg, "%s: %s\n", field[n], fdata[n]);
          fprintf(freg, "----\n");
          fclose(freg);
        }
      }
    }
  }
  fclose(fn);
  unlink(fname);
  return (0);
}

int m_register() {
  FILE *fn;
  int x, y, wid, len;
  char ans[4];
  char genbuf[200];

  if ((fn = fopen(fn_register, "r")) == NULL)
  {
    outs("目前並無新註冊資料");
    return XEASY;
  }

  stand_title("審核使用者註冊資料");
  y = 2;
  x = wid = 0;

  while (fgets(genbuf, STRLEN, fn) && x < 65)
  {
    if (strncmp(genbuf, "uid: ", 5) == 0)
    {
      move(y++, x);
      outs(genbuf + 5);
      len = strlen(genbuf + 5);
      if (len > wid)
        wid = len;
      if (y >= t_lines - 3)
      {
        y = 2;
        x += wid + 2;
      }
    }
  }
  fclose(fn);
  getdata(b_lines - 1, 0, "開始審核嗎(Y/N)？[N] ", ans, 3, LCECHO, 0);
  if(ans[0] == 'y') scan_register_form(fn_register);

  return 0;
}
