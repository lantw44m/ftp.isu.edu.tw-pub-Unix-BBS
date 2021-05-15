/*-------------------------------------------------------*/
/* kaede.c       ( SunOfBeach SOB BBS Ver 0.00 )         */
/*-------------------------------------------------------*/
/* target : Kaede's functions                            */
/* create : 96/03/23                                     */
/* update : 02/08/25 (Dopin kill many unser functions)   */
/*-------------------------------------------------------*/

#include "bbs.h"

char *kaede_prints(char *str) {
  static char strbuf[256];
  char buf[16], *strh = strbuf;
  register int pos;

  while (1)
  {
    if (!(pos = (int)strstr(str, "\033*")))
    {
      strcpy(strh, str);
      strh += strlen(str);
      break;
    }

    if ((pos -= (int)str) > 0)
    {
      strncpy(strh, str, pos);
      strh += pos;
      str += pos;
      continue;
    }

    while (str[pos+2] != 's')
    {
      if (str[pos+2] == 'n')
        str[pos+2] = ' ';
      if (!str[pos+2])
      {
        pos--;
        break;
      }
      pos++;
    }

    buf[0] = '%';
    if (pos > 13)
      pos = 13;
    strncpy(buf + 1, str + 2, pos + 1);
    buf[pos+2] = '\0';
    str += pos + 3;
    sprintf(strh, buf, cuser.userid);
    strh += strlen(strh);
  }
  strh[0] = '\0';
  return strbuf;
}

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

/*
woju
Cross-fs link()
*/

Link(char* src, char* dst) {
   char cmd[200];

   if (link(src, dst) == 0)
      return 0;

   sprintf(cmd, "/bin/cp -R %s %s", src, dst);
   return system(cmd);
}

#ifdef SunOS

#include <syslog.h>

void setenv(const char *name, const char *value, int overwrite) {
  if (overwrite || (getenv(name) == 0))
  {
    char *p;

    if ((p = malloc(strlen(name) + strlen(value) + 2)) == 0)
    {
      syslog(LOG_ERR, "out of memory\n");
      exit(1);
    }
    sprintf(p, "%s=%s", name, value);
    putenv(p);
  }
}

atexit(procp)
void (*procp)();
{
   on_exit(procp, 0);
}

#endif

capture_screen() {
   char fname[200];
   FILE* fp;
   extern screenline *big_picture;
   extern uschar scr_lns;
   int i;

   setuserfile(fname, "buf.0");
   if (fp = fopen(fname, "w")) {
      for (i = 0; i < scr_lns; i++)
         fprintf(fp, "%.*s\n", big_picture[i].len, big_picture[i].data + mbbsd);
      fclose(fp);
   }
}

edit_note() {
   char fname[200];
   int mode0 = currutmp->mode;
   int stat0 = currstat;
   char c0 = *quote_file;

   *quote_file = 0;
   setutmpmode(NOTE);
   setuserfile(fname, "note");
   vedit(fname, 0);
   currutmp->mode = mode0;
   currstat = stat0;
   *quote_file = c0;
}

char *my_ctime(const time_t *t) {
  struct tm *tp;
  static char ans[100];

  tp = localtime(t);
  sprintf(ans, "%02d/%02d/%02d %02d:%02d:%02d", tp->tm_year,
     tp->tm_mon + 1,tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
  return ans;
}

typedef int (*FPTR)();

scr_exec(FPTR fptr)
{
     uschar mode0 = currutmp->mode;
     int stat0 = currstat;
     char c0 = currutmp->chatid[0];
     screenline* screen0 = calloc(t_lines, sizeof(screenline));
     extern screenline* big_picture;
     int y, x;

     getyx(&y, &x);
     currutmp->mode = 0;
     currutmp->chatid[0] = 1;
     memcpy(screen0, big_picture, t_lines * sizeof(screenline));
     fptr();
     currutmp->mode = mode0;
     currutmp->chatid[0] = c0;
     currstat = stat0;
     memcpy(big_picture, screen0, t_lines * sizeof(screenline));
     free(screen0);
     move(y, x);
     redoscr();
}
