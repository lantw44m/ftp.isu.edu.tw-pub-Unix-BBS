/*-------------------------------------------------------*/
/* util/toprpg.c        ( Wind & Dust BBS 1998.6 )       */
/*-------------------------------------------------------*/
/* target : 使用者 RPG 排行榜                            */
/*-------------------------------------------------------*/

#include "bbs.h"

rpgrec RPG;

int
main(argc, argv)
  int argc;
  char **argv;
{
  int fdw,fdr,i=0;
  char buf[256];

  if (argc < 3)
  {
    printf("Usage: %s <username> <out-file>\n", argv[0]);
    exit(1);
  }

     sprintf(buf, BBSHOME"/home/%s/.RPG",argv[1]);
     fdr=open(buf , O_RDONLY);
     fdw=open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
     
     read(fdr,&RPG,sizeof(rpgrec));
     
     if (RPG.race > 6)
         RPG.race = 3;
         
     write(fdw,&RPG,sizeof(rpgrec));
     close(fdr);
     close(fdw);
}
