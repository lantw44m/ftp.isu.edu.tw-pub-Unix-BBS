#include "bbs.h"

#ifdef  EMAIL_JUSTIFY
int check_magic_key() {
  char buf[80], key[48], *mk_err[] = { "驗證資料錯誤 請重新執行寄發認證信" };
  int i;
  FILE *fp;

  if(HAS_PERM(PERM_LOGINOK)) {
    pressanykey("您已通過認證 不需執行此功\能");
    return FULLUPDATE;
  }

  if(count_multi() > 1) {
    pressanykey("本功\能只允許\單一登錄時執行");
    return FULLUPDATE;
  }

  setuserfile(buf, "MAGIC_KEY");
  if((fp = fopen(buf, "r")) == NULL) {
    pressanykey(mk_err);
    return FULLUPDATE;
  }

  fgets(buf, 80, fp);
  for(i = 0 ; i < 80 && (uschar)buf[i] > ' ' ; i++) ;

  if(i == 80) {
    pressanykey(mk_err);
    return FULLUPDATE;
  }

  getdata(b_lines, 0, "請輸入認證編碼 (例 12345:12345) ", key, 32, DOECHO, 0);

  if(!strcmp(buf, key)) {
    item_update_passwd('l');
    xuser.userlevel |= PERM_LOGINOK;

    sprintf(xuser.justify, "%-.30s%s", xuser.email, "[編碼]");
    strcpy(cuser.justify, xuser.justify);
    item_update_passwd('u');

    pressanykey("認證成功\ 請重登後即可提升權限");
  }
  else pressanykey("編碼確認錯誤 請重新輸入");

  return FULLUPDATE;
}
/* endif of EMAIL_JUSTIFY */
#endif
