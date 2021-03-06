/*-------------------------------------------------------*/
/* util/account.c       ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : bbsadm login 會做的事                        */
/* create : 96/12/04                                     */
/* update : 95/12/04                                     */
/*-------------------------------------------------------*/

#include <time.h>

#include "bbs.h"

#include "record.c"

#define MAX_LINE        16
#define ADJUST_M        6       /* adjust back 5 minutes */


void
keeplog(fpath, board, title)
char *fpath;
char *board;
char *title;
{
  fileheader fhdr;
  char genbuf[256];

  if(!board)
    board = "Record";

  sprintf(genbuf, "boards/%s", board);
  stampfile(genbuf, &fhdr);
  rename(fpath, genbuf);

  strcpy(fhdr.title, title);
  strcpy(fhdr.owner, "[歷史軌跡]");
  sprintf(genbuf, "boards/%s/.DIR", board);
  rec_add(genbuf, &fhdr, sizeof(fhdr));
}


void
outs(fp, buf, mode)
  FILE *fp;
  char buf[], mode;
{
  static char state = '0';

  if (state != mode)
    fprintf(fp, "[3%cm", state = mode);
  if (buf[0])
  {
    fprintf(fp, buf);
    buf[0] = 0;
  }
}

main()
{
  char buf1[256];
  FILE *fp1;

  time_t now;
  struct tm *ptime;

  ptime = localtime(&now);

  if(fp1=fopen("tmp/temp","r"))
  {
   fclose(fp1);
   sprintf(buf1,"[公安監控] 系統管理員監控 [%02d/%02d:%02d]"
          ,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_hour);
   keeplog ("tmp/temp","Security",buf1);
   system ("rm -f ../tmp/temp");
  }
  else {
        printf ("can't open file");
  }
  fclose(fp1);
}
