#include <stdio.h>
#include <unistd.h>

main(int argc, char* argv[])
{
   unsigned int us = 0;

   if (argc == 1 || sscanf(argv[1], "%ud", &us) != 1) {
      fprintf(stderr, "usage: usleep us\n"); 
      return;
   }
   usleep(us);
}
