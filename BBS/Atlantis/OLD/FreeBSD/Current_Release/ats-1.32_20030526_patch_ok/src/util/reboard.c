/*
�N�Ҧ� bad_board_id �M�� (�]�A�����)�A�o�N�� .BOARDS ���b�ǡC

�n�b share memory �٨S���J�ɰ���A�̦n�b�}���ɰ���A�_�h�|�y
����ƿ��ê��{�H�A�Y�n�^�_��ק�e�����A�A�i�H�b�����J
share memory ��:
cp ~bbs/BOARDS ~bbs/.BOARDS

�i�ϥ� ipcs �o�� share memory �O�_�w�g���J�A�Ϊ̰��� repasswd
�e���� ipcrm �� share memory �񱼡Afor FreeBSD:
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
