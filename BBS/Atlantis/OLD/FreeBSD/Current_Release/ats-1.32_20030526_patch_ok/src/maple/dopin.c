/*-------------------------------------------------------*/
/* sob/dopin.c                (SOB 0.22 ATS Verion 1.31) */
/*-------------------------------------------------------*/
/* target : ATSVersion 由 Dopin 新增的幾個專用函式       */
/* create : 02/09/04 by Dopin                            */
/* update : 02/09/11                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

extern char station_list[][13];

#ifdef USE_SQ_CURSOR
char msg[CURS_STR_NUM] = {0};
#endif

void get_tm_time(int *year, char *month, char *day, char *hour, char *min) {
   time_t now = time(NULL);
   struct tm *ptime;

   ptime = localtime(&now);

   if(year) *year = (int)ptime->tm_year+1900;
   if(month) *month = (char)ptime->tm_mon+1;
   if(day) *day = (char)ptime->tm_mday;
   if(hour) *hour = (char)ptime->tm_hour;
   if(min) *min = (char)ptime->tm_min;
}

int check_mmdd(char *date_str, char month, char day) {
   char buf[10], m, d;

   if(strlen(date_str) > 5) return -2;
   strcpy(buf, date_str);
   buf[2] = buf[5] = 0;
   m = (char)atoi(buf);
   d = (char)atoi(&buf[3]);

   if(m < 1 || m > 12 || d < 1 || d > 31) return -2;
   if(month < 1 || month > 12 || day < 1 || day > 31) return -2;

   if(m > month)
      if(m > 8 && month < 4) return 1;  /* 月份起點超過八月 終點小於四月 */
      else return -1;                   /* 當作時間未到解讀 */

   if(m < month)
      if(m < 4 && month > 8) return -1;
      else return 1;

   if(d > day)  return -1;
   if(d == day) return 0;

   return 1;
}

/* Dopin 08/02/02 */
int mail2user(char *userid, char *owner, char *title, char *buf, char mode) {
   char cbuf[100], genbuf[100];
   fileheader mymail;

   sethomepath(cbuf, userid);
   stampfile(cbuf, &mymail);

   mymail.savemode = mode;        /* hold-mail flag */
   mymail.filemode = 0;
   strcpy(mymail.owner, owner);
   sprintf(mymail.title, "[1;37;41m%s[m", title);
   sethomedir(genbuf, userid);

   append_record(genbuf, &mymail, sizeof(mymail));
   Rename(buf, cbuf);

   return FULLUPDATE;
}

/* Dopin 09/12/02 */
int remove_file_line(char *fpath, char *key, char mode) {
   char buf[256], kbuf[(IDLEN+1)*2];
   int i;
   FILE *fs, *ft;

   if(mode != 'B') return 1;

   if((fs = fopen(fpath, "r")) == NULL) return -1;
   sprintf(buf, BBSHOME "/home/%s/r_temp", cuser.userid);
   if((ft = fopen(buf, "w+")) == NULL) {
      fclose(fs);
      return -2;
   }

   while(fgets(buf, 256, fs)) {
      for(i = 0 ; i < 256 && (unsigned)buf[i] > '\r' ; i++);
      buf[i] = 0;

      if(mode == 'B') {
         for(i = 0 ; i < ((IDLEN+1)*2-1) && (unsigned)buf[i] > ' ' ; i++)
            kbuf[i] = buf[i];
         kbuf[i] = 0;
      }
      if(strcmp(key, kbuf)) fprintf(ft, "%s\n", buf);
   }

   fclose(fs);
   fclose(ft);

   sprintf(buf, BBSHOME "/home/%s/r_temp", cuser.userid);
   Rename(buf, fpath);

   return 0;
}

int check_reject(char *uident) {
   int i, j=1, k;
   FILE *fp, *fps;
   char genbuf[128];
   char bufid[IDLEN+1];

   sprintf(genbuf, BBSHOME "/home/%s/reject", uident);
   if((fp = fopen(genbuf, "r")) == NULL) goto Check_Over;
   sprintf(genbuf, BBSHOME "/home/%s/by_pass", uident);

   if((fps = fopen(genbuf, "r")) != NULL) {
      fclose(fps);
      goto By_Pass_Found;
   }

   strcpy(bufid, cuser.userid);
   for(k=0 ; k<IDLEN && bufid[k] != 0 ; k++)
      if(isupper(bufid[k]))
         bufid[k] += 'a' - 'A';

   while(1) {
      i = do_fgets(fp, genbuf, 1);

      if(i <= 0) break;

      for(k=0 ; k<IDLEN && genbuf[k] != 0; k++)
         if(isupper(genbuf[k]))
            genbuf[k] += 'a' - 'A';

      if(!strcmp(genbuf, bufid)) {
         j = 0;
         break;
      }
   }

By_Pass_Found:
   fclose(fp);
Check_Over:

   return j;
}

#ifdef CHECK_POST_TIME
int check_post_time(void) {
  extern char save_title[];
  time_t now = time(NULL);
  int range = now - cuser.last_post_time;

  if(range < POST_RANGE) {
     pressanykey("距上次發文的時間只有 %d 秒 尚需 %d 秒才可發文", range,
                 POST_RANGE - range);
     return 0;
  }

  range = now - cuser.count_time;
  if(cuser.post_number >= POST_NUMBER) {
     char buf[TTLEN], genbuf[100];
     FILE *fp;

     if(range < POST_LOWER) {
        sprintf(buf, "%s 於 %d 秒內共發了 %d 篇文章", cuser.userid,
                range, cuser.post_number);
        strcpy(genbuf, BBSHOME "/tmp");
        sprintf(genbuf, tempnam(genbuf, "post_"));
        if((fp = fopen(genbuf, "w+")) == NULL) return 0;
#ifndef VEDITOR
        strcpy(save_title, "[快速發文系統記錄]");
        write_header(fp);
#endif
        fputs(buf, fp);
        fclose(fp);
        sprintf(buf, "[系統記錄] 快速發文 : %s", cuser.userid);

        void_log('B', genbuf, "log_post", buf);
        cuser.post_number = 0;
     }

     cuser.count_time = now;
  }

  cuser.last_post_time = now;
  cuser.post_number++;

  return 1;
}
#endif

int getbh(boardheader *bh, char *boardname) {
   int bid = getbnum(boardname);

   if(!bid) return 0;

   if(get_record(fn_board, bh, sizeof(boardheader), bid) == -1) {
      bh->level = 0;
      return 0;
   }
   else return bid;
}

/* Dopin 02/12/17 */
int is_board_manager(char *boardname) {
   char buf[50];
   boardheader *bh;

   if(HAS_PERM(PERM_SYSOP)) return 1;
   if(bh = getbcache(currboard)) {
      sprintf(buf, "run/%s.sysop_name", bh->station);
      return belong(buf, cuser.userid);
   }
   else return 0;
}

/* Dopin 03/04/16 */
int is_member(char *bname, char mode, char *fname, char *path) {
   char buf[MAXPATHLEN] = {0}, tb[(IDLEN+1)*2];
   int ret = 0;
   boardheader *bp = getbcache(bname);

   strcpy(tb, currboard); // Dopin: 先暫存現有看板資料
   if(is_board_manager(bname) || is_BM(bp->BM)) return 1; // Dopin: 板主/台長

   if(mode == 'B' | mode == 'A')  // Dopin: 如果模式是 秘密看板(精華區選擇含)
      if(!(bp->level & PERM_ENTERSC && !(bp->level & PERM_POSTMASK)))
         ret = 1;                 // 檢查是否為 進板秘密 且屬性為讀取

   switch(mode) {
      case 'B':
         sprintf(buf, "boards/%s/%s", bname, fname);
         break;
      case 'A':
         strcpy(buf, path);
         break;
   }

   // Dopin: 如果為 進板秘密/精華區秘密目錄 則檢查成員名單
   if(!ret)
      if(mode == 'B' && (ret = belong(buf, cuser.userid)) == 0)
         strcpy(currboard, tb); // Dopin: 不是選擇看板不把非成員的預設目錄還原

   return ret;
}

int check_file_line(char *fpath) {
   int i = 0;
   char buf[256];
   FILE *fp;

   fp = fopen(fpath, "r");
   if(!fp) return 0;

   while(fgets(buf, 256, fp)) i++;
   fclose(fp);

   return i;
}

int fileheader_marked(char *fpath, char filemode, char *key, char type) {
   char fg, shift, buf[128];
   char *ptr, *pkey;
   int len;
   FILE *fs, *ft;
   fileheader fh;

   len = strlen(key);
   if(type == 'U') ptr = fh.owner;       /* 是比對 userid */
   else if(type == 'T') ptr = fh.title;  /* 是比對 title */

   pkey = key;
   if(key[0] == 'R' && key[1] == 'e' && key[2] == ':') {
      pkey += 4; /* 回文標題額外處理 */
      len -= 4;
   }

   sprintf(buf, "%s.TG", fpath);
   fs = fopen(fpath, "rb");
   if(!fs) return -1;
   ft = fopen(buf, "wb+");
   if(!ft) {
      fclose(fs);
      return -2;
   }

   while(fread(&fh, sizeof(fh), 1, fs)) {
      fg = 0;
      if(strcmp(ptr, pkey)) {
         if(type == 'T')
            if(fh.title[0] == 'R' && fh.title[1] == 'e' && fh.title[2] == ':')
               if(!strncmp(ptr+4, pkey, len)) fg = 1;
      }
      else fg = 1;

      if(fg) fh.filemode ^= filemode; /* 設定標記 */
      fwrite(&fh, sizeof(fh), 1, ft);
   }

   fclose(fs);
   fclose(ft);
   Rename(buf, fpath);

   return 0;
}

void Copy(char *src, char *tar) {
 char buf[MAXPATHLEN*2];

 sprintf(buf, "cp %s %s", src, tar);
 system(buf);
}

void void_log(char mode, char *fpath, char *board, char *title) {
   fileheader fhdr = {0};
   char genbuf[256];

   if(mode == 'B') {
      if(!board) board = "SYSOP";
      sprintf(genbuf, "boards/%s", board);
   }
   if(mode == 'S')
#ifdef NO_USE_MULTI_STATION
      strcpy(genbuf, "man/boards/Note/OrderSongs");
#else
      sprintf(genbuf, "man/boards/Note.%s/OrderSongs", cuser.station);
#endif

   stampfile(genbuf, &fhdr);
   if(mode == 'S') Copy(fpath, genbuf);
   else Rename(fpath, genbuf);

   strcpy(fhdr.title, title);
   strcpy(fhdr.owner, "<SYSTEM>");
   if(mode == 'B') sprintf(genbuf, "boards/%s/.DIR", board);

   if(mode == 'S')
#ifdef NO_USE_MULTI_STATION
      strcpy(genbuf, "man/boards/Note/OrderSongs/.DIR");
#else
      sprintf(genbuf, "man/boards/Note.%s/OrderSongs/.DIR",
              cuser.station);
#endif

   append_record(genbuf, &fhdr, sizeof(fhdr));
}

#ifdef  MultiIssue
/* Dopin 11/06/02 */
int check_multi_index(char mode, int st_num, char *genbuf) {
  FILE *fp;
  int ci;

  sprintf(genbuf, BBSHOME "/etc/%s/%s/IssueNum", /* 都用 IssueNum 統一檔名 */
          mode == 'I' ? "issue" : "SN", station_list[st_num]);
  fp = fopen(genbuf, "r");
  if(!fp) return -1;

  fgets(genbuf, 5, fp);
  genbuf[4] = 0;
  fclose(fp);
  ci = (char)atoi(genbuf);
  if(!ci) return -1;

  srandom((unsigned long)time(NULL));
  sprintf(genbuf, BBSHOME "/etc/%s/%s/%d", mode == 'I' ? "issue" : "SN",
          station_list[st_num], random()%ci+1);

  return 0;
}
#endif

#ifdef USE_SQ_CURSOR
char *str_cursor(void) {
   static char ci = -1;

   ci++;
   if(ci >= CURS_STR_NUM/2) ci = 0;

   msg[0] = CURS_STRING[ci*2];
   msg[1] = CURS_STRING[ci*2+1];
   return msg;
}
#endif

#ifdef COUNT_MONEY_WHEN_POST
/* Dopin 10/01/02 */
int dp_filelength(char *fpath) { /* 極端陽春 !!!!! 請照貴站狀況自行更改 QQ */
   char *check_h[] = {
 STR_AUTHOR1, STR_AUTHOR2, STR_POST1, STR_POST1C, STR_POST2, "時間:", "標題:"
   }; /* 作者: 看板: ... 就是那幾個 header 再加上 時間與標題 */
   char buf[256];
   int i = 0, j, k;
   FILE *fp;

   fp = fopen(fpath, "r");
   if(!fp) return -1;

   while(fgets(buf, 256, fp)) {  /* 如果是 Header 跳開 */
      for(j = 0 ; j < 7 ; j++) if(strstr(buf, check_h[j])) goto B_2_here;
      if((buf[0] == ':' || buf[0] == '>') && buf[1] == ' ') continue;
      if(buf[0] == '-' && buf[1] == '-') break; /* 如果是引言 跳開 */
      /* 如果是 -- 分隔 以下不計 */

      for(j = 0, k = 0; j < 256 && buf[j] ; j++) {
         if((uschar)buf[j] == ' ') k++;    /* 計算空白字元數目 */
         if((uschar)buf[j] < ' ') break;   /* 高興連標點都不計也可以改 都好 */
      }
      buf[j] = 0;

      j = strlen(buf);
      if(j < 5) continue;
      i += j - k; /* 長度減去空白字元 */
B_2_here:
   }

   return i;
}

/* Dopin: 請依照您的狀況修改 勿完全照抄 */
int getpostmoney(time_t a, time_t b, int length, char mode) {
   float rate;

   /* pressanykey("%d %d %d %d", a, b, length, b-a); */ /* Dopin: Debug 用 */
   if(mode == 'P') {
      if(b - a < 60) return 0;    /* 發文不滿一分鐘 不計錢 */
      if(length < 60) return 0;   /* 字數不到 60 不計錢 */


      rate = (float)((float)(b-a)/(float)length);
      if(rate < 0.21) return 0;  /* c/p 大法不計錢 每秒五字(含思考)未免太快 */
   }
   /* 其他要加什麼判斷 就隨便 */
   /* 這程式會有缺點 就是自爆文損的錢常會比發文賺的還高 但我覺得合理應該 */

   if(mode == 'D') if(length < 100) return 50;
   return length / 2; /* 給文章字數目一半之錢 */
}
#endif

#ifndef NO_USE_MULTI_STATION
/* Dopin 10/22/02 */
int maskfile_update(char mode, usint *perm, char *userid) {
   char buf[10], filename[40];
   FILE *fp;

   sprintf(buf, "mask.%d", cuser.now_stno);
   sprintf(filename, "home/%s/%s", userid, buf);


   if(mode == 'u' || mode == 'l' && (fp = fopen(filename, "rb")) == NULL) {
      if((fp = fopen(filename, "wb+")) == NULL) return -1;
      else mode = 'u';
   }

   if(mode == 'u') fwrite(perm, sizeof(usint), 1, fp);
   else            fread(perm, sizeof(usint), 1, fp);

   fclose(fp);

   return 0;
}

/* Dopin : MODE 分為 (l)oad/(u)pdate 兩種 */
int access_mask(char mode, usint *perm, char *userid) {
   usint cmask = 0x0001FFFF, umask = 0x00000000, emask = 0xFFF80000;

   if(mode == 'u') umask = *perm & emask;
   if(maskfile_update(mode, &umask, userid)) {
      pressanykey("Error Open Mask Files");
      return -1;
   }

   if(mode == 'l') {
      *perm &= cmask;
      *perm |= umask & emask;
   }

   return 0;
}
#endif
