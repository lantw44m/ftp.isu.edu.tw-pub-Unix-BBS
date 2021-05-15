#include "bbs.h"

rpgrec RPG;

main(int argc,char *argv[])
{
  char buf[256];
  int fdr;
  
    sprintf(buf, BBSHOME"/home/%s/.RPG",argv[1]);
    fdr=open(buf , O_RDONLY);
    read(fdr,&RPG,sizeof(rpgrec));
    close(fdr);
    printf("userid=%s\n",RPG.userid);
    printf("race=%d\n",RPG.race);
    printf("level=%d\n",RPG.level);
}
