#include "bbs.h"

static char STR_bv_results[] = "results";

int vote_results(char *bname) {
  char buf[STRLEN];

  setbfile(buf, bname, STR_bv_results);
  if(more(buf, YEA) == -1) {
    move(3, 0);
    clrtobot();
    outs("\n目前沒有任何投票的結果。");
    pressanykey(NULL);
  }

  return FULLUPDATE;
}

int b_results() {
  return vote_results(currboard);
}
