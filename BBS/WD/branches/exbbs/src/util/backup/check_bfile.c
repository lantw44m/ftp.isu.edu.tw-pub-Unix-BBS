#include "bbs.h"

int
check_bfile(char *direct, char *fn)
{
  fileheader head;
  int fi;
  
  if ((fi = open(direct, O_RDONLY, 0)) > 0)
  {
    while (read(fi, &head, sizeof head) == sizeof head)
    {
      if(strcmp(head.filename, fn)==0)
      {
         printf("match");
         break;
      }
    }
    close(fi);
  }
  return 0;
}

int
main(argc, argv)
  int argc;
  char *argv[];
{
  char fn[64],dr[64];

  if (argc<3)  
  {
    printf("error\n");
    return 0;
  }
    strcpy(dr,argv[1]);
    strcpy(fn,argv[2]);
   
  return check_bfile(dr,fn);
}
