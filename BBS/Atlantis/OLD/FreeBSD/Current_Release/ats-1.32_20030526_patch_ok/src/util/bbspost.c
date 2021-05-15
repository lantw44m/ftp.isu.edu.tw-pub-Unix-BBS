#include "bbs.h"
#include "cache.c"
#include "record.c"


test_main()
{
   boardheader *bptr;
   int i;

   resolve_boards();
   bptr = bcache;
   for (i = 0; i < numboards; i++)
      printf("%s\n", bptr[i].brdname);
}



void
strip_ansi(buf, str)
  char *buf, *str;
{
  register int ch, ansi;

  for (ansi = 0; ch = *str; str++)
  {
    if (ch == '\n')
    {
      break;
    }
    else if (ch == 27)
    {
      ansi = 1;
    }
    else if (ansi)
    {
      if (!strchr("[01234567;", ch))
        ansi = 0;
    }
    else
    {
      *buf++ = ch;
    }
  }
  *buf = '\0';
}



main(int argc, char** argv)
{
   char buf[200], out[200], title[200], sender[200], date[200], owner[200];
   FILE *fout;
   char *ip, *ptr;
   fileheader fhdr;
   char fpath[200];
   char brdname[IDLEN + 1];
   int bnum;
   FILE* fin = stdin;

/*
   fin = fopen(BBSHOME "/mailspool", "r");
*/
   if (argc < 2) {
      puts("Usage: bbspost <Board>");
      exit(1);
   }
   if (!(bnum = getbnum(argv[1]))) {
      printf("No such board: %s\n", argv[1]);
      exit(1);
   }
   strcpy(brdname, bcache[--bnum].brdname);

   *date = *sender = 0;
   while (fgets(buf, sizeof(buf), fin)) {
      if (!strncmp(buf, "From", 4)) {
         if ((ip = strchr(buf, '<')) && (ptr = strrchr(ip, '>'))) {
            if (ip[-1] == ' ')
               ip[-1] = '\0';

            if (strchr(++ip, '@'))
               *ptr = '\0';
            else                                    /* 由 local host 寄信 */
               strcpy(ptr, "@localhost");

            ptr = (char *) strchr(buf, ' ');
            while (*++ptr == ' ');
            sprintf(sender, "%s (%s)", ip, ptr);
         }
         else {
           strtok(buf, " \t\n\r");
           strcpy(sender, (char *) strtok(NULL, " \t\n\r"));

           if (strchr(sender, '@') == NULL)        /* 由 local host 寄信 */
              strcat(sender, "@localhost");
         }
         continue;
      }
      if (!strncmp(buf, "Date: ", 6)) {
         strcpy(date, buf + 6);
         continue;
      }
      if (!strncmp(buf, "Subject: ", 9)) {
         strip_ansi(title, buf + 9);
         continue;
      }
      if (buf[0] == '\n')
         break;
   }

   if (!*sender) {
      puts("No sender");
      exit(1);
   }
   if (!*date) {
      time_t now = time(NULL);
      strcpy(date, ctime(&now));
   }
   if (!*title)
      sprintf(title, "來自 %.64s", sender);
   strcpy(owner, sender);
   owner[IDLEN] = 0;
   title[TTLEN] = 0;
   strtok(owner, ".@");
   strcat(owner + strlen(owner) - 1, ".");
   sprintf(fpath, BBSHOME "/boards/%s", brdname);
   stampfile(fpath, &fhdr);
   if (fout = fopen(fpath, "w")) {
      fprintf(fout, "作者: %s 站內: %s\n標題: %s\n時間: %s\n",
         sender, brdname, title, date);
      while (fgets(buf, sizeof(buf), fin))
         fputs(buf, fout);
      fclose(fout);
      fhdr.savemode = 'L';
      fhdr.filemode = FILE_LOCAL;
      strcpy(fhdr.owner, owner);
      strcpy(fhdr.title, title);
      sprintf(fpath, BBSHOME "/boards/%s/.DIR", brdname);
      return append_record(fpath, &fhdr, sizeof(fhdr));
   }
   printf("fopen error: %s\n", fpath);
   return 1;
}
