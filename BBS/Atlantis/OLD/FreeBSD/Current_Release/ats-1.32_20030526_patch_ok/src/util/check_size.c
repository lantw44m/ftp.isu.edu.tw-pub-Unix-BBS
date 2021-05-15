/*-----------------------------------------------------*/
/* util/check_size.c          (SOB 0.22 ATS Ver. 1.32) */
/*-----------------------------------------------------*/
/* target: 檢查系統中各資料結構的實際大小              */
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
