#include "bbs.h"

rpgrec RPG;
userec USER;

main()
{
  int fdu,fdr,fdw,i=0;
  char buf[256];

  fdu=open(BBSHOME"/.PASSWDS",O_RDONLY);
  fdw=open(BBSHOME"/.RPGS.NEW",O_WRONLY | O_CREAT | O_TRUNC, 0644);

  while(read(fdu,&USER,sizeof(userec))==sizeof(userec))
  {
    sprintf(buf, BBSHOME"/home/%s/.RPG",USER.userid);
    fdr=open(buf , O_RDONLY);
    read(fdr,&RPG,sizeof(rpgrec));
    strcpy(RPG.userid,USER.userid);
    write(fdw,&RPG,sizeof(rpgrec));
    close(fdr);
    printf("userid=%s,",RPG.userid);
  }
  close(fdu);
  close(fdw);
}
