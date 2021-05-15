
#include"bbs.h"

int
instr(char *S, char c)
{
   int i;
   
   for(i=0;i<strlen(S);i++)
      if(S[i] == c)
         return (i+1);
         
   return 0;
}

int
pal_print(PAL *pal)
{
  printf("%-12s, %2d, %5s, %5s,%2d, %s\n",
    pal->userid, pal->savemode,
    pal->owner, pal->date, pal->ftype,
    pal->desc
    );
  return 1;
}    

int
pal_fix(PAL *pal, char *op)
{
   time_t now = time(NULL);
   struct tm *ptime = localtime(&now);
   if((pal->ftype) > 7 || (pal->ftype) < 0)
      pal->ftype = 0;
   if((pal->savemode>7) || (pal->savemode<0))
      pal->savemode = 0;
   if((*pal).date[2] != '/')
      sprintf(pal->date, "%02d/%02d",  ptime->tm_mon + 1, ptime->tm_mday);
   if(!(*pal).desc[0])
      sprintf(pal->desc, "?");
   if( (!(*pal).owner[0]) || instr(op,'r'))
      sprintf(pal->owner, "?");
   if(!(*pal).userid[0])
      sprintf(pal->userid, "?");
   return 1;
}

int
main(int argc, char *argv[])
{
  int fd, fd2, i;
  PAL pal;
  char *fname=FN_PAL, fname2[32];
  char *op1="";
      
  if(argc<2)
  {
    printf("usage: showpal filename\n");
    return 0;
  }
  
  fname=argv[1];
  if(argc>2)
     op1=argv[2];
     
  if(instr(op1,'h'))
  {
    printf("help--\n");
    printf("h : help\n");
    printf("c : count\n");
    printf("f : fix\n");
    printf("m : move\n");
    printf("r : recover\n");
    printf("p : print\n");
    return 0;
  }                
  
  if(instr(op1,'f'))
  {
     strcpy(fname2, fname);
     strcat(fname2, ".new");

     if ((fd2 = open(fname2, O_WRONLY | O_CREAT, 0600)) < 0)
     {
        printf("can not write to new file.\n");
        return 0;
     }
  }
    
  if ((fd = open(fname, O_RDONLY)) >= 0)
  {
    if(instr(op1,'p'))
      printf("   userid  savemode  owner   date  ftype  desc\n");
    i=0;
    while (read(fd, &pal, sizeof(pal)) )
    {
       i++;
       if(instr(op1,'p'))
       {
         printf("%2d ",i);
         pal_print(&pal);
       }
       if(instr(op1,'f'))
       {
          pal_fix(&pal,op1);
          write(fd2, &pal, sizeof(pal));
       }
       
    }
    if(instr(op1,'c'))
      printf("%d\n",i);
                  
    close(fd);
    
    if(instr(op1,'f'))
    {
       close(fd2);
       fd = open(fname2, O_RDONLY);
       if(instr(op1,'p'))
         printf("   userid  savemode  owner   date  ftype  desc\n");
       i=0;
       while (read(fd, &pal, sizeof(pal)) )
       {
          i++;
          if(instr(op1,'p'))
          {
            printf("%2d ",i);
            pal_print(&pal);
          }
       }
       close(fd);       
    }
  }
  else
  {
     printf("can not open the file.\n");
     return 0;
  }
  
  if(instr(op1,'m') && instr(op1,'f'))
  {
     f_rm(fname);
     f_mv(fname2, fname);
  }
  return 1;
}
  