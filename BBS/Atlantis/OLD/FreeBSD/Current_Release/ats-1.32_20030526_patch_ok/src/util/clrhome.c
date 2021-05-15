/*-------------------------------------------------------*/
/* util/clrhome.c       ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : Maple home (.DIR) ²M²z¡BºûÅ@                 */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/
/* syntax : clrhome                                      */
/*-------------------------------------------------------*/


#include "bbs.h"

#include "record.c"
#include "cache.c"



main(argc, argv)
  int argc;
  char *argv[];
{
  struct dirent *de;
  DIR *dirp;
  char buf[80], *ptr;
  char t_dir[80];
  char cmd[200];

  strcpy(buf, BBSHOME "/home");
  if ((dirp = opendir(buf)) == NULL)
  {
    printf(":Err: unable to open %s\n", buf);
    return;
  }

  ptr = buf + strlen(buf);
  *ptr++ = '/';
  getcwd(t_dir, 29);

  while (de = readdir(dirp))
  {
    if (de->d_name[0] != '.')
    {
      strcpy(ptr, de->d_name);
      if (!searchuser(de->d_name)) {
         sprintf(cmd, "mv '%s' %s", buf, BBSHOME "/tmp");
         puts(cmd);
         system(cmd);
      }
    }
  }
  closedir(dirp);
  chdir(t_dir);
}
