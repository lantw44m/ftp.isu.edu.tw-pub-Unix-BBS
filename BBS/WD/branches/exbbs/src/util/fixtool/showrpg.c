#include "bbs.h"

rpgrec RPG;

main(int argc,char *argv[])
{
  int fdr,fdw,fdp,i=0;
  char buf[256];
  
  if(argc != 2)
  {
  	exit(-1);
  }
  
  fdr=open(argv[1],O_RDONLY);
//  fdr=open(BBSHOME"/.RPGS",O_RDONLY);
//  fdw=open(BBSHOME"/PASSWDS.NEW",O_WRONLY | O_CREAT | O_TRUNC, 0644);

//  while(read(fdr,&RPG,sizeof(rpgrec))==sizeof(rpgrec))
//  {
//    sprintf(buf, BBSHOME"/home/%s/.RPG",RPG.userid);
//    fdw=open(buf ,O_WRONLY | O_CREAT | O_TRUNC, 0600);
//    write(fdw,&RPG,sizeof(rpgrec));
//    close(fdw);
//    printf("%-10d. -- %-14.14s --\n",++i, RPG.userid);
//  }

  while(read(fdr,&RPG,sizeof(rpgrec))==sizeof(rpgrec))
  {
  printf("userid=%s,",RPG.userid);
  printf("age=%d,",RPG.age);
  printf("race=%d\n",RPG.race);
  }
  close(fdr);
//  close(fdw);
}     
