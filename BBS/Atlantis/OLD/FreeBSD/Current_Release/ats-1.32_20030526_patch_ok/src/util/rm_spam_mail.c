/*-------------------------------------------------------*/
/* util/rm_spam_mail.c         ( Atlantis BBS Ver 1.32 ) */
/*-------------------------------------------------------*/
/* target : 依照垃圾信集中看板刪除使用者信箱之垃圾信     */
/* create : 03/05/10 (Dopin)                             */
/* update :   /  /                                       */
/*-------------------------------------------------------*/
/* syntex : rm_spam_mail (as bbsrsadm)                   */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"

#define PASSFILE BBSHOME "/.PASSWDS"
#define SPAMBOARD "SPAM_Mail"        /* 垃圾信集中看板名 請依貴站需要修改 */
#define SPAMDIR   BBSHOME "/boards/" SPAMBOARD "/.DIR"
#define MAXRECORD 1024               /* Dopin: 最多讀入 1024 封垃圾信資料 */

#define SAFE_DEL
/* Dopin: 請依貴站狀況修改 如有空格 不可省略 入無的話就改成空字串 "" */
#define TRANS "[轉錄]"

typedef struct {
  char id[IDLEN+1];
  char title[TTLEN];
} condition;

union fc {
  struct {
    fileheader fh;
  } F;

  struct {
    char buf[128];
  } C;
};
typedef union fc fc;

condition cdn[MAXRECORD];
char us[MAXUSERS][IDLEN+1];

int get_condition_by_board(void) {
  int i = 0;
  fileheader fh;
  FILE *fp;

  if((fp = fopen(SPAMDIR, "rb")) == NULL) return -1;
  while(fread(&fh, sizeof(fh), 1, fp) == 1) {
    if(fh.filemode & FILE_MARKED) {  /* Dopin: 有被 mark 起來才當作已處理 */
      char *ptr;

      strncpy(cdn[i].id, fh.owner, IDLEN);
      cdn[i].id[IDLEN] = 0;
      strncpy(cdn[i].title, fh.title, TTLEN-(strlen(TRANS)+1));
      cdn[i].title[TTLEN-strlen(TRANS)] = 0;

      i++;
      if(i == MAXRECORD) break;
    }
  }
  fclose(fp);

  return i;
}

int get_user_by_passwd(void) {
  int i = 0;
  userec u;
  FILE *fp;

  if((fp = fopen(PASSFILE, "rb")) == NULL) return -1;
  while(fread(&u, sizeof(u), 1, fp) == 1) {
    strcpy(us[i], u.userid);
    i++;
    if(i == MAXUSERS) break;
  }
  fclose(fp);

  return i;
}

int check_bad_id(char *id) {
  int i, j = 0;

  /* Dopin: 退認證信件 跳離 (當然你要照砍也可以 QQ Remark 這行即可) */
  if(strcmp(id, "站長")) return 0; /* 站長二字為 ATS 內建 請依貴站情況更改 */

  for(i = 0 ; i < IDLEN ; i++) {
    if((uschar)id[i] <= ' ') break;
    if(!(isalpha(id[i]) || isdigit(id[i]) || id[i] == '.')) {
      j = 1;
      break;
    }
  }

  return j;
}

int check_bad_title(char *title) {
  int i;
  char buf[TTLEN];

  strncpy(buf, title, TTLEN-1);
  buf[TTLEN-1] = 0;

  for(i = 0 ; i < TTLEN ; i++) {
     if((uschar)buf[i] < ' ') break;
     buf[i] = tolower(buf[i]);
  }

  /* Dopin: 這邊可以彈性變更 */
  if(strstr(buf, "g5?"))      return 1;
  if(strstr(buf, "fw:"))      return 1;
  if(strstr(buf, "fw :"))     return 1;
  if(strstr(buf, "big5"))     return 1;
  if(strstr(buf, "so-8859"))  return 1;
  if(strstr(buf, "來自"))     return 1;
  if(strstr(buf, "無碼"))     return 1;
  if(strstr(buf, "誠徵"))     return 1;
  if(strstr(buf, "想成為"))   return 1;
  if(strstr(buf, "工作室"))   return 1;
  if(strstr(buf, "上網賺錢")) return 1;
  if(strstr(buf, "好康告訴")) return 1;
  if(strstr(buf, "★★★"))   return 1;
  if(strstr(buf, "★☆★"))   return 1;

  return 0;
}

int check_spam(fileheader *pfh, int spam_record, char *userid) {
  int i, j = 0, trans = strlen(TRANS);

#ifdef SAFE_DEL
  /* BBS 備份不砍 */
  if(!strcmp(pfh->owner, "[備.忘.錄]") || !strcmp(pfh->owner, "[SYSTEM]"))
    return 0;
#endif

#ifndef SAFE_DEL
  if(!strcmp(pfh->owner, "RECOVER") return 1;
#endif

  for(i = 0 ; i < spam_record ; i++)
    if(trans ?
#ifdef SAFE_DEL
       !strncmp(&cdn[i].title[strlen(TRANS)], pfh->title, TTLEN-strlen(TRANS))
     : !strncmp(pfh->owner, cdn[i].id, IDLEN) &&
       strncmp(pfh->title, cdn[i].title, TTLEN-1)
#else
       strstr(&cdn[i].title[strlen(TRANS)], pfh->title)
     : !strncmp(pfh->owner, cdn[i].id, IDLEN) ||
        strncmp(pfh->title, cdn[i].title, TTLEN-1)
#endif
     || check_bad_id(pfh->owner) || check_bad_title(pfh->title)) {
      /* Dopin: 顯示出 [使用者信箱] 垃圾信來源 ID 垃圾信來源標題 */
      printf("[%s] %s %s\n", userid, pfh->owner, pfh->title);
      j = 1;
      break;
    }

  return j;
}

int main(int argc, char *argv[]) {
  char src[MAXPATHLEN], tar[MAXPATHLEN], tmp[MAXPATHLEN];
  int user_num, spam_num, i, j, k = 0;
  fc fh;       /* Dopin: 有點多此一舉(直接宣告 fileheader 亦可) 但我高興 :p */
  int fd;      /* Dopin: 當作 file index 所以另外宣告 */
  FILE *ft;

  spam_num = get_condition_by_board();
  user_num = get_user_by_passwd();

  printf("User Records is = %d / Spam Mail Records is %d\n", user_num,
         spam_num);
  if(!user_num || !spam_num) return -1; /* Dopin: 沒找齊資料 不用執行 */

  for(i = 0 ; i < user_num ; i++) {
    sprintf(src, "%s/home/%s/.DIR", BBSHOME, us[i]);
    /* 除錯用 如果跑時停在一個使用者目錄很久很久 請檢查該目錄 .DIR 是否損壞 */
    puts(src);

    sprintf(tar, "%s/home/%s/.DIR.check", BBSHOME, us[i]);
    /* 沒必要的話 我實在不喜歡(因為懶 :p) 利用 open/close 這類東東來維護檔案 */
    if((fd = open(src, O_RDONLY, 0)) == -1) continue;
    if((ft = fopen(tar, "wb+")) == NULL) {
      close(fd);
      continue;
    }

    flock(fd, LOCK_EX);  /* Dopin: 鎖住檔案 以策安全 */
    while(read(fd, &fh.C.buf, sizeof(fh)) == sizeof(fh)) {
      if(!check_spam(&fh.F.fh, spam_num, us[i]))
        fwrite(&fh.F.fh, sizeof(fh), 1, ft);
      else {
        /* 刪除垃圾信件 */
        printf("%s\n", fh.F.fh.title);
        sprintf(tmp, "%s/home/%s/%s", BBSHOME, us[i], fh.F.fh.filename);
        unlink(tmp);
      }
    }
    flock(fd, LOCK_UN);

    close(fd);
    fclose(ft);
    Rename(tar, src);
  }

  return 0;
}
