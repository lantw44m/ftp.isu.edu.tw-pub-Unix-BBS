#include <time.h>
#include <stdio.h>
#include "bbs.h"     // 新舊版本的bbs.h位置不同

struct boardheader aboard;

main(int argc, char **argv)
{
    FILE *foo1, *foo2;
    int cnum;

    if (argc < 3) {
        printf("usage: setboardtime .BOARDS output_file\n");
        exit(1);
    }

    if( ((foo1=fopen(argv[1], "r")) == NULL)
                || ((foo2=fopen(argv[2],"w"))== NULL) ){
        puts("file opening error");
        exit(1);
    }

    while( (cnum=fread( &aboard, sizeof(aboard), 1, foo1))>0 ) {
       aboard.totalvisit = 0;
       aboard.totaltime = 0;
       fwrite( &aboard, sizeof(aboard), 1, foo2);
    }
    fclose(foo1);
    fclose(foo2);

    return 0;
}
