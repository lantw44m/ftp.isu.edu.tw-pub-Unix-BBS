#include "bbs.h"

rpgrec RPG;
userec USER;

main(int argc,char *argv[])
{
  int fdu,fdw,fdp,i=0;
//  char buf[256];
  
//  if(argc != 2)
//  {
//  	exit(-1);
//  }
  
  fdu=open(BBSHOME"/.PASSWDS",O_RDONLY);
//  fdr=open(argv[1],O_RDONLY);
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

  while(read(fdu,&USER,sizeof(userec))==sizeof(userec))
  {
  printf("userid=%s,",USER.userid);
  printf("username=%s,",USER.username);
  printf("realname=%s\n",USER.realname);
  }
  close(fdu);
//  close(fdw);
}     
