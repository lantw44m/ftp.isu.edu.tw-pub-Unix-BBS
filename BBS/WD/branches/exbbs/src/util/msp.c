/*
Multi Message Prompt
*/

#include <stdio.h>


char* BANNER = "Starting SunOfBeach BBS System";
char* show_os_list();

main()
{
   printf(show_os_list());
}

char* show_os_list()
{
  char  buf[100][100];
  FILE        *fp;
  int         i,j=0;
  int	      osget,buflen;
  static char	      totalbuf[100];
              
   sprintf(totalbuf, "%s/etc/MSG.list", getenv("HOME")); 
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
   sprintf(totalbuf,"%s.....  ",BANNER);
   return totalbuf;
   }
    
   osget=time(NULL)%j; 
   buflen=strlen(buf[osget]);
   buf[osget][buflen-1]='\0';
   buf[osget+1][0]='\0';
  
   sprintf(totalbuf,"%s.....  ",buf[osget]);
   return totalbuf;
}   
