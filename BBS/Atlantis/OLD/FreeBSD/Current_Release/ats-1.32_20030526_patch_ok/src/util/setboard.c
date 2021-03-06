#include <stdio.h>
#include "bbs.h"

#define DOTPASSWDS BBSHOME "/.BOARDS"
#define PASSWDSBAK BBSHOME "/BOARDS"
#define TMPFILE    BBSHOME "/tmpfile"

unsigned short rev(unsigned short i);

boardheader board;

main(argc, argv)
int argc;
char **argv;
{
    FILE *foo1, *foo2;
    int cnum,i,match;
    int VF;

    if( argc < 3 || !(*argv[1] == '0' || *argv[1] == '1' || *argv[1] == '2')) {
        puts("Usage: setboard VoteFlag[012] board1 [board2...]");
        exit(1);
    }
    if( ((foo1=fopen(DOTPASSWDS, "r")) == NULL)
                || ((foo2=fopen(TMPFILE,"w"))== NULL) ){
        puts("file opening error");
        exit(1);
    }

    while( (cnum=fread( &board, sizeof(board), 1, foo1))>0 ) {
       for (match =0, i = 2; i < argc; i++)
           if(!strcmp(board.brdname, argv[i])) {
              match = 1;
              break;
           }
       if (match)
          board.bvote = *argv[1] - '0';
       fwrite( &board, sizeof(board), 1, foo2);
    }
    fclose(foo1);
    fclose(foo2);

    if(rename(DOTPASSWDS, PASSWDSBAK)==-1){
        puts("replace fails");
        exit(1);
    }
    unlink(DOTPASSWDS);
    rename(TMPFILE,DOTPASSWDS);
    unlink("tmpfile");

    exit(0);
}

