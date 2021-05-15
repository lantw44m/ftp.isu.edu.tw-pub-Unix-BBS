#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bbs.h"

#define CNTFILE BBSHOME"/.counter"

main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *fcnt;
  unsigned long visited;
  struct tm *ptime;
  time_t now;
  time(&now);
  ptime = localtime(&now);


   if(argc > 3 || argc == 2 && (strcmp(argv[1],"+") && strcmp(argv[1],"-"))){
    printf("usage: %s [+|-]\n", argv[0]);
    exit(1);
   }

   if (fcnt = fopen(CNTFILE, "r"))
   {
    fscanf(fcnt, "%lu", &visited);
/*    system("touch"); */
    fclose(fcnt);
   }
   if (argc == 1 || argc == 2 && !strcmp(argv[1], "+"))
   printf("\033[1;36m您是本站自 %d 年 %d 月 %d 日起第 %lu 位訪客\033[0m\n",
       ptime->tm_year, ptime->tm_mon+1, ptime->tm_mday ,++visited);

   if (fcnt = fopen(CNTFILE, "w"))
   {
   if (argc == 2 && !strcmp(argv[1], "-"))
      fprintf(fcnt, "%lu", --visited);
   else
      fprintf(fcnt, "%lu", visited);
    fclose(fcnt);
  }
}
