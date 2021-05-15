#include "bbs.h"

#ifdef HAVE_INFO
int x_program() {
  char genbuf[80], update[80];
  FILE *fp;

  more("etc/version", NA);
  fp = fopen("etc/update", "r");
  if(fp == NULL) strcpy(update, "None Update Data");
  else {
  fgets(update, 60, fp);
  fclose(fp);
  }

  sprintf(genbuf, "ª©¥»¡G%s  §ó·s¡G%s", MYVERSION, update);
  out2line(1, b_lines-1, genbuf);
  pressanykey(NULL);

  return 0;
  }
#endif
