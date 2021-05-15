#include <fcntl.h>

void
f_cat(fpath, msg)
  char *fpath;
  char *msg;
{
  int fd;
  char buf[256];

  if ((fd = open(fpath, O_WRONLY | O_CREAT | O_APPEND, 0600)) >= 0)
  {
    sprintf(buf,"%s\n",msg);
    write(fd, buf, strlen(buf));
    close(fd);
  }
}
