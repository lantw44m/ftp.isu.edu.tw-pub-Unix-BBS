#include "bbs.h"

int
badguy(fname,userid)
  char *fname, *userid;
{
  int fd;
  PAL pal;

  if ((fd = open(fname, O_RDONLY)) >= 0)
  {
    while (read(fd, &pal, sizeof(pal)) == sizeof(pal))
    {
      if (!strcmp(pal.userid, userid))
      {
        if(pal.savemode & M_BAD)
           return 1;
        printf("savemode = %d, ftype = %d\n",pal.savemode,pal.ftype);
      }
    }
    close(fd);
  }
  return 0;
}

int
main(int argc,char *argv[])
{
	if (!badguy(argv[1],argv[2]) )
		printf("Not badguy!\n");
	else
		printf("Yes! badguy\n");
}