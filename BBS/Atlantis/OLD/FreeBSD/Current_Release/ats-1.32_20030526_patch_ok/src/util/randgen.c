/*
�@��: tmg (����}�Ӫ�..���S) �ݪO: 386BSD
���D: �üƱK�X���;�
�ɶ�: Sun Oct 20 02:16:03 1996

�̪�j�a���`�Q�׺����w�������D,
�䤤, �@�ӭ��n���a��O password �ܮe���N�Q decrypt �X��.....
�o�̴��Ѥ@�Ӱ��k�Ѥj�a�Ѧ�,
�N�O�Q�ζüƲ��� password.......
�p��, cracker �ڥ��L�k�Φr��h�զX�X�K�X........
�U���o�ӵ{���i�H���� NUM �նüƲզX�� password.....
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
