#include <stdio.h>
#include "cvsup.h"

void write_log(char *str) {
  FILE *fp;
  time_t now = time(NULL);

  if((fp = fopen(CVS_LOG, "a+")) == NULL) return;
  fprintf(fp, "%-56.56s %s\n", str, Cdate(&now));
  fclose(fp);
}
