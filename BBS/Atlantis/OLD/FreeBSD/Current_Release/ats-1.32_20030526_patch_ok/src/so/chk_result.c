#include "bbs.h"

int check_results(void) {
   char buf[100], buf2[40];

   sprintf(buf2, " USE %s ", currboard);
   sprintf(buf, "more adm/board.read | grep '%s' | sort -r > tmp/read.%s",
   buf2, currboard);
   system(buf);
   sprintf(buf2, "tmp/read.%s", currboard);

   if(more(buf2, YEA) == -1) pressanykey("�{���o�Ϳ��~, �бz�^���� Bug �O");

   sprintf(buf, "rm %s", buf2);
   system(buf);

   return FULLUPDATE;
}
