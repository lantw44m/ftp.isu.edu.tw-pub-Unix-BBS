#include "bbs.h"

static char STR_bv_results[] = "results";

int vote_results(char *bname) {
  char buf[STRLEN];

  setbfile(buf, bname, STR_bv_results);
  if(more(buf, YEA) == -1) {
    move(3, 0);
    clrtobot();
    outs("\n�ثe�S������벼�����G�C");
    pressanykey(NULL);
  }

  return FULLUPDATE;
}

int b_results() {
  return vote_results(currboard);
}
