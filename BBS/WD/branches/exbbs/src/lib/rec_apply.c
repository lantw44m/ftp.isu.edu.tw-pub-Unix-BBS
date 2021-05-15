#include "dao.h"
#include <fcntl.h>
#include <unistd.h>

#if 0
int
rec_apply(fpath, fptr, size)
  char *fpath;
  int (*fptr) ();
  int size;
{
  char buf[REC_SIZ];
  int fd;

  if ((fd = open(fpath, O_RDONLY)) == -1)
    return -1;

  while (read(fd, buf, size) == size)
  {
    if ((*fptr) (buf))
    {
      close(fd);
      return -2;
    }
  }
  close(fd);
  return 0;
}
#endif

int
rec_apply(fpath, fptr, size)
  char *fpath;
  int (*fptr) ();
int size;
{
  char abuf[REC_SIZ];
  FILE* fp;

  if (!(fp = fopen(fpath, "r")))
    return -1;

  while (fread(abuf, 1, size, fp) == size)
     if ((*fptr) (abuf) == -2) {
        fclose(fp);
        return -2;
     }
  fclose(fp);
  return 0;
}
