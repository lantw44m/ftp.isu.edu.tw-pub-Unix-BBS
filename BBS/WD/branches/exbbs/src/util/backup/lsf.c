/*
  lsf: Line String Filter
  Usage: lsf <column> <string>
*/

#include <stdio.h>

main(int argc, char** argv)
{
   FILE* fp = stdin;
   char buffer[501];
   int column = 0;

   if (argc < 3 || !sscanf(argv[1], "%d", &column) 
       || column < 0 || column > 499) {
      puts("Usage: lsf <column:0-499> <string>");
      exit(1);
   }

   while (fgets(buffer, 501, fp))
      if (column <= strlen(buffer) 
          && !strncmp(buffer + column, argv[2], strlen(argv[2])))
         printf(buffer);
}
      

