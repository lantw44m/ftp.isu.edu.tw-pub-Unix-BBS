/*
作者: tmg (海邊漂來的..海嘯) 看板: 386BSD
標題: 亂數密碼產生器
時間: Sun Oct 20 02:16:03 1996

最近大家滿常討論網路安全的問題,
其中, 一個重要的地方是 password 很容易就被 decrypt 出來.....
這裡提供一個做法供大家參考,
就是利用亂數產生 password.......
如此, cracker 根本無法用字典去組合出密碼........
下面這個程式可以產生 NUM 組亂數組合的 password.....
*/

#define NUM 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  int i, j;
  int num;

  srand(time(NULL));
  for (i = 0; i < NUM; i++) {
    for (j = 0; j < 8; j++) {
      num = 94. * rand() / (RAND_MAX + 1.);
      printf("%c", (char)num+' '+1);
    }
    if (i % 8 < 7)
      printf("  ");
    else
      puts("");
  }
  putchar('\n');

  return 0;
}
