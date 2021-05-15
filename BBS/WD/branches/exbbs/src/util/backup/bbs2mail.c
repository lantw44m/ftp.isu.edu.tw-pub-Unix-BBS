/*-------------------------------------------------------*/
/* util/bbs2mail.c	( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 將 BBS 文章 轉換成 mail 格式                  */
/* create : 95/03/29                                     */
/* update : 99/01/19                                     */
/*-------------------------------------------------------*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <sysexits.h>

#include "bbs.h"

int
main(int argc, char *argv[])
{
  char genbuf[256], title[80], name[80], sender[80], brdname[80], *ip, *ptr;
  time_t tmp_time;
        
  while (fgets(genbuf, 255, stdin))
  {
    if (!strncmp(genbuf, "作者:", 5))
    {
      strtok(genbuf, " \t\n\r");
      strcpy(sender, (char *) strtok(NULL, " \t\n\r"));
      
//      strcpy(name, (char *) strtok(NULL, " \t\n\r"));
      
      ip=strstr(genbuf, "看板:");
      ptr=(char *) strchr(ip, ' ');
      sprintf(brdname, "%s", *ptr+1);
/*            
      if ((ip = strchr(name, '(')) && (ptr = strrchr(ip, ')')))
      {
	*ptr = '\0';
	if (ip[-1] == ' ')
	  ip[-1] = '\0';
	ptr = (char *) strchr(genbuf, ' ');
	while (*++ptr == ' ');
	sprintf(name, "%s", ptr);
      }
      else
        sprintf(name, "%s", sender);
*/
      continue;
    }
                 
    if (!strncmp(genbuf, "標題:", 5))
    {
      strcpy(title, genbuf + 6);
      continue;
    }
    if (genbuf[0] == '\n')
      break;
  }

  if (ptr = strchr(sender, '\n'))
    *ptr = '\0';

  if (ptr = strchr(title, '\n'))
    *ptr = '\0';

  if (strchr(sender, '@') == NULL)	/* 由 local host 寄信 */
  {
    strcat(sender, "@" MYHOSTNAME);
  }

  time(&tmp_time);

  if (!title[0])
    sprintf(title, "來自 %.64s", sender);

  str_decode(title);
  printf("name=%s\nmail=%s\nbrdname=%s\nsubject=%s\npostdate=%s\n\n",
    name, sender, brdname, title, ctime(&tmp_time));

  while (fgets(genbuf, 255, stdin))
  {
    fputs(genbuf, stdout);
  }

  return 1;

}

