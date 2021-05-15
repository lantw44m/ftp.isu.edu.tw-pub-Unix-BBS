#include "bbs.h"
#include "bbs_util.h"

int main( int argc, char *argv[])
{
   if( argc < 2 )
   {
      exit(1);
   }
   
   printf("%d\n",dashf(argv[1]));

  return 1;
}


