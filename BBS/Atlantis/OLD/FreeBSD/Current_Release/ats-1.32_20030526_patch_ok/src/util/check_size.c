/*-----------------------------------------------------*/
/* util/check_size.c          (SOB 0.22 ATS Ver. 1.32) */
/*-----------------------------------------------------*/
/* target: �ˬd�t�Τ��U��Ƶ��c����ڤj�p              */
/* create: 11/14/02 by Dopin                           */
/* update:                                             */
/*-----------------------------------------------------*/

#include "bbs.h"

int main(void) {
   printf("Sizeof User  Reccord is %d\n", sizeof(userec));
   printf("Sizeof Board Header  is %d\n", sizeof(boardheader));
   printf("Sizeof File  Header  is %d\n", sizeof(fileheader));

   return 0;
}
