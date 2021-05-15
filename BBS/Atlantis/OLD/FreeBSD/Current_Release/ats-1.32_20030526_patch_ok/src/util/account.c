/*-------------------------------------------------------*/
/* util/account.c       ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : ¤W¯¸¤H¦¸²Î­p¡B¨t²Î¸ê®Æ³Æ¥÷                   */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include <time.h>

#include "record.c"

#define MAX_LINE        16
#define ADJUST_M        6       /* adjust back 5 minutes */
/*
woju
Cross-fs rename()
*/

Rename(char* src, char* dst)
{
   char cmd[200];

   if (rename(src, dst) == 0)
      return 0;

   sprintf(cmd, "/bin/mv %s %s", src, dst);
   return system(cmd);

}



void
keeplog(fpath, board, title)
char *fpath;
char *board;
char *title;
{
  fileheader fhdr;
  char genbuf[256];
  char *flog;

  if(!board)
    board = "status";

  sprintf(genbuf, "boards/%s", board);
  stampfile(genbuf, &fhdr);
  Rename(fpath, genbuf);

  strcpy(fhdr.title, title);
  strcpy(fhdr.owner, "<SYSTEM>");
  sprintf(genbuf, "boards/%s/.DIR", board);
  append_record(genbuf, &fhdr, sizeof(fhdr));
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


void
gzip(source, target, stamp)
  char *source, *target, *stamp;
{
  char cmd[200], dst[128];

  sprintf(dst, "adm/%s%s", target, stamp);
  Rename(source, dst);
  sprintf(cmd, "/usr/bin/gzip -9n %s", dst);
  system(cmd);
}


main()
{
  int hour, max, item, total, i, j;
  char *act_file = ".act";
  char *log_file = "usies";
  char buf[256], *p;
  FILE *fp;

  int act[27];                  /* ¦¸¼Æ/²Ö­p®É¶¡/pointer */
  time_t now;
  struct tm *ptime;

  now = time(NULL) - ADJUST_M * 60;     /* back to ancent */
  ptime = localtime(&now);

  memset(act, 0, sizeof(act));
  if ((ptime->tm_hour != 0) && (fp = fopen(act_file, "r")))
  {
    fread(act, sizeof(act), 1, fp);
    fclose(fp);
  }

  if ((fp = fopen(log_file, "r")) == NULL)
  {
    printf("cann't open usies\n");
    return 1;
  }

  if (act[26])
    fseek(fp, act[26], 0);

  while (fgets(buf, 256, fp))
  {
    hour = atoi(buf + 9);
    if (hour < 0 || hour > 23)
    {
      continue;
    }
    if (!strncmp(buf + 22, "ENTER", 5))
    {
      act[hour]++;
      continue;
    }
    if (p = (char *) strstr(buf + 40, "Stay:"))
    {
      if (hour = atoi(p + 5))
      {
        act[24] += hour;
        act[25]++;
      }
      continue;
    }
  }
  act[26] = ftell(fp);
  fclose(fp);

  for (i = max = total = 0; i < 24; i++)
  {
    total += act[i];
    if (act[i] > max)
      max = act[i];
  }

  item = max / MAX_LINE + 1;

  if (!ptime->tm_hour)
    keeplog("etc/today", NULL, "«_ÀI¤H¦¸²Î­p");

  if ((fp = fopen("etc/today", "w")) == NULL)
  {
    printf("cann't open etc/today\n");
    return 1;
  }

  fprintf(fp, "\t\t\t[1;33;46m ¨C¤p®É«_ÀI¤H¦¸²Î­p [%02d/%02d/%02d] [40m\n\n", ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_year%100);
  for (i = MAX_LINE + 1; i > 0; i--)
  {
    strcpy(buf, "   ");
    for (j = 0; j < 24; j++)
    {
      max = item * i;
      hour = act[j];
      if (hour && (max > hour) && (max - item <= hour))
      {
        outs(fp, buf, '3');
        fprintf(fp, "%-3d", hour);
      }
      else if (max <= hour)
      {
        outs(fp, buf, '1');
        fprintf(fp, "¢i ");
      }
      else
        strcat(buf, "   ");
    }
    fprintf(fp, "\n");
  }

  fprintf(fp, "   [32m"
    "0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23\n\n"
    "\t\t[34mÁ`¦@«_ÀI¤H¦¸¡G[37m%-9d[34m¥­§¡°±¯d®É¶¡¡G[37m%d[40;0m\n"

    ,total, act[24] / act[25] + 1);
  fclose(fp);

  if (fp = fopen(act_file, "w"))
  {
    fwrite(act, sizeof(act), 1, fp);
    fclose(fp);
  }


  /* -------------------------------------------------------------- */

    sprintf(buf, "-%02d%02d%02d",
      ptime->tm_year, ptime->tm_mon + 1, ptime->tm_mday);

  now += ADJUST_M * 60;         /* back to future */
  ptime = localtime(&now);

  if (!ptime->tm_hour)
  {
    keeplog(".note", "status", "«_ÀIªº­^¶¯¨ÆÂÝ");
    system("/bin/cp etc/today etc/yesterday");
    system("rm -rf .note");
    gzip(log_file, "usies", buf);

    if (ptime->tm_wday == 0)
    {
      keeplog("etc/week", NULL, "¥»¶g«_ÀI°O¿ý");

      gzip("bbslog", "bntplink", buf);
      gzip("innd/bbslog", "innbbsd", buf);
      gzip("mailog", "mailog", buf);
      gzip("adm/board.read", "board.read", buf);
    }

    if (ptime->tm_mday == 1) {
      keeplog("etc/month", NULL, "¥»¤ë«_ÀI°O¿ý");
      gzip("logins.bad", "logins.bad", buf);
      gzip("sendmail.log", "sendmail.log", buf);
    }

    if (ptime->tm_yday == 1)
      keeplog("etc/year", NULL, "¦~«×«_ÀI°O¿ý");
  }
  else if (ptime->tm_hour == 3 && ptime->tm_wday == 6)
  {
    char *fn1 = "tmp";
    char *fn2 = "suicide";

    Rename(fn1, fn2);
    mkdir(fn1, 0755);
    sprintf(buf, "/bin/gtar cfz adm/%s-%02d%02d%02d.tgz %s",
      fn2, ptime->tm_year, ptime->tm_mon + 1, ptime->tm_mday, fn2);
    system(buf);
    sprintf(buf, "/bin/rm -fr %s", fn2);
    system(buf);
  }
}
