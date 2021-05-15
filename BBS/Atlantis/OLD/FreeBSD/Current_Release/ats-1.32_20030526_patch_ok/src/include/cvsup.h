#include "bbs.h"

#define SRC_DIR  BBSHOME  "/src/cvsup"
#define WORK_DIR BBSHOME  "/CVS"
#define CVS_CUR  WORK_DIR "/Version"
#define CVS_FILE WORK_DIR "/version"
#define CVS_TEMP WORK_DIR "/version.work"
#define CVS_WORK WORK_DIR "/temp"
#define CVS_LOG  WORK_DIR "/cvsup.log"
#define OFFICAL_URL "ftp://ftp.ats.idv.tw/pub/Atlantis/cvs"

#define ERROR_COUNT 3

typedef struct {
  int files;
  char ffpath[20][256];
  char fname[20][32];
} tarball;

typedef struct {
  unsigned int crc1;
  unsigned int crc2;
  char filename[247];
  char mode;
} cksum;

typedef struct {
  char cvs[64];
  usint cvsid;
  time_t date;
  cksum checksum;
} cvscode;

typedef struct {
  int lock;
  char global_cvs[32];
  cvscode code[32];
} cvsindex;

typedef struct {
  char filename[255];
  char mode;
} patchs;

typedef struct {
  patchs list[64];
  char tarname[256];
} lists;
