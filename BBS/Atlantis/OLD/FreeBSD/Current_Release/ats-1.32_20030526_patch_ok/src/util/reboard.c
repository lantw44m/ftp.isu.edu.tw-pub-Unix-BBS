/*
將所有 bad_board_id 清掉 (包括空欄位)，這將使 .BOARDS 乾淨些。

要在 share memory 還沒載入時執行，最好在開機時執行，否則會造
成資料錯亂的現象，若要回復到修改前的狀態，可以在未載入
share memory 時:
cp ~bbs/BOARDS ~bbs/.BOARDS

可使用 ipcs 得知 share memory 是否已經載入，或者執行 repasswd
前先用 ipcrm 把 share memory 放掉，for FreeBSD:
ftp://freebsd.ee.ntu.edu.tw/sob-version/bin/shutdownbbs
*/

#include <stdio.h>
#include "bbs.h"

#define DOTPASSWDS BBSHOME "/.BOARDS"
#define PASSWDSBAK BBSHOME "/BOARDS"
#define TMPFILE    BBSHOME "/tmpfile"

struct boardheader bh;

main()
{
    FILE *foo1, *foo2;
    int cnum,i,match;

    setgid(BBSGID);
    setuid(BBSUID);
    if( ((foo1=fopen(DOTPASSWDS, "r")) == NULL)
                || ((foo2=fopen(TMPFILE,"w"))== NULL) ){
        puts("file opening error");
        exit(1);
    }

    while( (cnum=fread( &bh, sizeof(bh), 1, foo1))>0 ) {
       if (bad_user_id(bh.brdname))
          continue;
       fwrite( &bh, sizeof(bh), 1, foo2);
    }
    fclose(foo1);
    fclose(foo2);

    if(rename(DOTPASSWDS,PASSWDSBAK)==-1){
        puts("replace fails");
        exit(1);
    }
    unlink(DOTPASSWDS);
    rename(TMPFILE,DOTPASSWDS);
    unlink("tmpfile");

    return 0;
}


bad_user_id(userid)
  char *userid;
{
  register char ch;

  if (!isalnum(*userid))
    return 1;

  while (ch = *(++userid))
  {
    if (!isalnum(ch) && ch != '_' && ch != '-' && ch != '.')
      return 1;
  }
  return 0;
}
