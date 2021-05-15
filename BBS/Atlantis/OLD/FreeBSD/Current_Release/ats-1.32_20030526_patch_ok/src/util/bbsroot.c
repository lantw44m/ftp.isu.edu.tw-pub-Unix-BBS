#include <unistd.h>
#include <pwd.h>
#include "bbs.h"

main(int argc, char** argv)
{
   pid_t pid;
   struct passwd* pwd;

   if (argc < 2) {
      puts("Usage: bbsroot <cmd> <arg1> <arg2> ...");
      exit(1);
   }
   if (!(pwd = getpwnam("nobody"))) {
      perror("getpwnam");
      exit(1);
   }
   if (chroot(BBSHOME "/bin")) {
      perror("chroot");
      exit(1);
   }
   setgid(pwd->pw_gid);
   initgroups("nobody", pwd->pw_gid);
   setuid(pwd->pw_uid);

   if (execv(argv[1], argv + 1) == -1)
      perror("execv");
}
