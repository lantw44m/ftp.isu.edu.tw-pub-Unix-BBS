/*-------------------------------------------------------*/
/* maple/agree.h              (SOB 0.22 ATS Verion 1.31) */
/*-------------------------------------------------------*/
/* target : 連署系統程式之標頭檔 常數 共用函式定義       */
/* create : 02/09/09 by Dopin                            */
/* update : 02/09/10                                     */
/*-------------------------------------------------------*/
/* syntax :                                              */
/*-------------------------------------------------------*/

#define DIR BBSHOME "/agree"        /* 工作目錄 */
#define TARGET DIR "/agree.ags"     /* 記錄檔所在絕對路徑 */
#define LOCKFILE DIR "/lock"        /* 系統鎖定檔 */
#define TEMPFILE DIR "/temp"        /* 系統暫存檔 */
#define BACKUP   DIR "/backup.ags"  /* 系統備份檔 */

#define MINLIMIT "10"  /* 系統預設最小連署門檻人數 (為字串 "1" ~ "65535") */
#define MAXAGREES 128  /* 最高請勿超過 254 筆資料 如需擴充 請更改相關變數 */

#define POSTBOARD "SYSOP"                   /* 預設公告之看板名 */
#define POSTTO BBSHOME "/boards/" POSTBOARD /* 預設公告之看板完整路徑 */

char *err_msg[] = { "目前無任何系統連署", "無此筆連署資料", "空", "資料錯誤" };
char *action[] = { "修改", "刪除", "開放連署", "停止連署", "公告" };
char *limit[] = {
"通過認證", "信箱無上限", "保留帳號", "板主權限", "站務人員", "站長", "無限制"
};

struct ags {
   char   flag;
   char   title[50];
   char   fpath[70];
   char   author[IDLEN+1];
   char   start[6];
   char   end[6];
   char   postboard[(IDLEN+1)*2];
   ushort count;
   ushort minlimit;
   char anonymous;
   usint  perm;
};
typedef struct ags ags;

ags work[MAXAGREES];

void set_agree_file(char *buf, uschar number) {
   sprintf(buf, DIR "/%d.agree", number);
}

int check_agree(ags *check) {
   int i;
   char month, day, data_m, data_d;

   if(!check->flag) return 0;

   get_tm_time(NULL, &month, &day, NULL, NULL);
   if(!month || !day) return 0;

   i = check_mmdd(check->start, month, day);
   if(i == -2) return 0;
   if(i == -1) return 0;

   if(!strcmp(check->end, "--/--")) return 1;
   i = check_mmdd(check->end, month, day);
   if(i == -2) return 0;
   if(i == 1) return 0;

   return 1;
}

int perm_acs(int mode, usint *perm, char key_v) {
   if(!mode) {
      if(!perm) return 1;
      if(HAS_PERM(*perm)) return 1;
      else return 0;
   }

   if(mode == 1) {
      if(*perm < PERM_LOGINOK) return 7;
      if(*perm & PERM_LOGINOK) return 1;
      if(*perm & PERM_MAILLIMIT) return 2;
      if(*perm & PERM_XEMPT) return 3;
      if(*perm & PERM_BM) return 4;
      if(*perm & PERM_ACCOUNTS || *perm & PERM_BOARD) return 5;
      if(*perm & PERM_SYSOP) return 6;
   }

   if(mode == 2) {
      if(key_v == '1') *perm = PERM_LOGINOK;
      if(key_v == '2') *perm = PERM_MAILLIMIT;
      if(key_v == '3') *perm = PERM_XEMPT;
      if(key_v == '4') *perm = PERM_BM;
      if(key_v == '5') *perm = PERM_ACCOUNTS || PERM_BOARD;
      if(key_v == '6') *perm = PERM_SYSOP;
      if(key_v == '7') *perm = 0;
   }
}
