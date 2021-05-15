/*
Multi OS Banner
*/

#include <stdio.h>


char* line = "tty?";
char* BANNER = "FreeBSD 2.2.5";
char* show_os_list();

main(int argc, char** argv)
{
   if (argc > 1)
      line = argv[1];
   printf(show_os_list());
}

char* show_os_list()
{
  char  buf[100][100];
  FILE        *fp;
  int         i,j=0;
  int	      osget,buflen;
  char        dirbuf[40];
  static char	      totalbuf[100];
              
   gethostname(dirbuf, 40);
    
   sprintf(totalbuf, "%s/etc/OS.list", getenv("HOME"));
    if( (fp=fopen(totalbuf, "r"))!=NULL){
     for (i=0; i<100; i++) {
      if (fgets(buf[i],100,fp) == (char *) NULL) {
         buf[i][0]='\0';
         break;
         }
       j++;
       }
      fclose(fp);
     }
   else {
   sprintf(totalbuf,"\r\n\r%s (%s) %s\r\n\r\n",BANNER,dirbuf,line);
   return totalbuf;
   }
    
   osget=time(NULL)%j; 
   buflen=strlen(buf[osget]);
   buf[osget][buflen-1]='\0';
   buf[osget+1][0]='\0';
  
   sprintf(totalbuf,"\r\n\r%s(%s) %s\r\n\r\n",buf[osget],dirbuf,line);
   return totalbuf;
}   
