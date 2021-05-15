#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


int
main(int argc, char *argv[])
{
  char buf[256],fname[128];
  FILE *fi;
  struct stat st;
  time_t ftime;
  
  if(argc>1)
     strcpy(fname,argv[1]);
  else
     return 0;
  
  if( (fi=fopen( fname,"rt")) == NULL )   
     return 0;
     
  lstat( fname, &st );
  ftime = st.st_mtime;
//  sprintf(buf,"ÀÉ®×: %lu",ftime);
//  strcat(buf,"%0D%0A\n");
//  fputs(buf,stdout);

  while (fgets(buf, sizeof(buf), fi))
  {
     buf[strlen(buf)-1]='\0';
     strcat(buf,"%0D%0A\n");
     fputs(buf,stdout);
  }
  fclose(fi);
  return 1;
}

