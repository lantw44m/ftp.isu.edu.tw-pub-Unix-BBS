/*-------------------------------------------------------*/
/* bbsd.c	( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* author : opus.bbs@bbs.cs.nthu.edu.tw		 	 */
/* target : BBS daemon/main/login/top-menu routines 	 */
/* create : 95/03/29				 	 */
/* update : 96/10/10				 	 */
/*-------------------------------------------------------*/


#define	_MAIN_C_


#include "bbs.h"
#include "dns.h"


#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/telnet.h>
#include <sys/resource.h>


#define	QLEN		3
#define	PID_FILE	"run/bbs.pid"
#define	LOG_FILE	"run/bbs.log"
#undef	SERVER_USAGE


#define MAXPORTS        3
static int myports[MAXPORTS] = {23  , 3456,8000/* 3001, 3002, 3003 */ };


extern UCACHE *ushm;


static char rusername[40];
static int mport;
static u_long tn_addr;


#ifdef CHAT_SECURE
char passbuf[9];
#endif


#ifdef MODE_STAT
extern UMODELOG modelog;
extern time_t mode_lastchange;
#endif


/* ----------------------------------------------------- */
/* ���} BBS �{��					 */
/* ----------------------------------------------------- */


void
blog(mode, msg)
  char *mode, *msg;
{
  char buf[512], data[256];
  time_t now;

  time(&now);
  if (!msg)
  {
    msg = data;
    sprintf(data, "Stay: %d (%d)", (now - ap_start) / 60, currpid);
  }

  sprintf(buf, "%s %s %-13s%s\n", Etime(&now), mode, cuser.userid, msg);
  f_cat(FN_USIES, buf);
}


#ifdef MODE_STAT
void
log_modes()
{
  time(&modelog.logtime);
  rec_add(FN_MODE_CUR, &modelog, sizeof(UMODELOG));
}
#endif


void
u_exit(mode)
  char *mode;
{
  int fd, delta;
  ACCT tuser;
  char fpath[80];

#if 0
  if(!cutmp)   /* Thor.980806: ���t�m�N��|��login, �����x�s���� */
    return;    /* Thor.980806: �� bbstate�N���F */
#endif

  utmp_free();			/* ���� UTMP shm */
  blog(mode, NULL);

  if (cuser.userlevel)
  {
    ve_backup();		/* �s�边�۰ʳƥ� */
    brh_save();			/* �x�s�\Ū�O���� */
  }

#ifndef BMW_SAVE        /* lkchu.981201: �b config.h �]�w */
  /* Thor.980405: �����ɥ��w�R�����T */

  usr_fpath(fpath, cuser.userid, FN_BMW);
  unlink(fpath);
#endif

#ifdef MODE_STAT
  log_modes();
#endif

  usr_fpath(fpath, cuser.userid, fn_acct);
  fd = open(fpath, O_RDWR);
  if (fd >= 0)
  {
    if (read(fd, &tuser, sizeof(ACCT)) == sizeof(ACCT))
    {
      delta = time(&cuser.lastlogin) - ap_start;
      cuser.staytime += delta;
       /* Thor.980727:����, �b���W���W�L�T���������p�⦸��  */
       /* lkchu.981201: ���ӭn�� delta, �C���W�����n�W�L�T�����~�� */
      if (delta /* cuser.staytime */ > 3 * 60) 
	cuser.numlogins++;
      cuser.userlevel = tuser.userlevel;
      cuser.tvalid = tuser.tvalid;
      cuser.vtime = tuser.vtime;
      strcpy(cuser.justify, tuser.justify);
      strcpy(cuser.vmail, tuser.vmail);
      lseek(fd, (off_t) 0, SEEK_SET);
      write(fd, &cuser, sizeof(ACCT));
    }
    close(fd);
  }
}


void
abort_bbs()
{
  if (bbstate)
    u_exit("AXXED");
  exit(0);
}


static void
login_abort(msg)
  char *msg;
{
  outs(msg);
  refresh();
  blog("LOGIN", currtitle);
  exit(0);
}


/* Thor.980903: lkchu patch: ���ϥΤW���ӽбb����, �h�U�C function������ */

#ifdef LOGINASNEW

/* ----------------------------------------------------- */
/* �ˬd user ���U���p					 */
/* ----------------------------------------------------- */


static int
belong(flist, key)
  char *flist;
  char *key;
{
  int fd, rc;

  rc = 0;
  fd = open(flist, O_RDONLY);
  if (fd >= 0)
  {
    mgets(-1);

    while (flist = mgets(fd))
    {
      str_lower(flist, flist);
      if (str_str(key, flist))
      {
	rc = 1;
	break;
      }
    }

    close(fd);
  }
  return rc;
}


static int
is_badid(userid)
  char *userid;
{
  int ch;
  char *str;

  if (strlen(userid) < 2)
    return 1;

  if (!is_alpha(*userid))
    return 1;

  if (!str_cmp(userid, "new"))
    return 1;

  str = userid;
  while (ch = *(++str))
  {
    if (!is_alnum(ch))
      return 1;
  }
  return (belong("etc/badid", userid));
}


static int
uniq_userno(fd)
  int fd;
{
  char buf[4096];
  int userno, size;
  SCHEMA *sp;			/* record length 16 �i�㰣 4096 */

  userno = 1;

  while ((size = read(fd, buf, sizeof(buf))) > 0)
  {
    sp = (SCHEMA *) buf;
    do
    {
      if (sp->userid[0] == '\0')
      {
	lseek(fd, -size, SEEK_CUR);
	return userno;
      }
      userno++;
      size -= sizeof(SCHEMA);
      sp++;
    } while (size);
  }

  return userno;
}


static void
acct_apply()
{
  SCHEMA slot;
  char buf[80];
  char *userid;
  int try, fd;

  film_out(FILM_APPLY, 0);

  memset(&cuser, 0, sizeof(cuser));
  userid = cuser.userid;
  try = 0;
  for (;;)
  {
    if (!vget(19, 0, msg_uid, userid, IDLEN + 1, DOECHO))
      login_abort("\n�A�� ...");

    if (is_badid(userid))
    {
      vmsg("�L�k�����o�ӥN���A�Шϥέ^��r���A�åB���n�]�t�Ů�");
    }
    else
    {
      usr_fpath(buf, userid, NULL);
      if (dashd(buf))
	vmsg("���N���w�g���H�ϥ�");
      else
	break;
    }

    if (++try >= 10)
      login_abort("\n�z���տ��~����J�Ӧh�A�ФU���A�ӧa");
  }

  for (;;)
  {
    vget(20, 0, "�г]�w�K�X�G", buf, 9, NOECHO);
    if ((strlen(buf) < 3) || !strcmp(buf, userid))
    {
      vmsg("�K�X��²��A���D�J�I�A�ܤ֭n 4 �Ӧr�A�Э��s��J");
      continue;
    }

    vget(21, 0, "���ˬd�K�X�G", buf + 10, 9, NOECHO);
    if (!strcmp(buf, buf + 10))
      break;

    vmsg("�K�X��J���~, �Э��s��J�K�X");
  }

  str_ncpy(cuser.passwd, genpasswd(buf), PASSLEN);

  clear();
  do
  {
    vget(1, 0, "��    �١G", cuser.username, sizeof(cuser.username), DOECHO);
  } while (strlen(cuser.username) < 2);

  do
  {
    vget(2, 0, "�u��m�W�G", cuser.realname, sizeof(cuser.realname), DOECHO);
  } while (strlen(cuser.realname) < 4);

  do
  {
    vget(3, 0, "�~��a�}�G", cuser.address, sizeof(cuser.address), DOECHO);
  } while (strlen(cuser.address) < 12);

/* mat 981206 */
#ifdef COINAGE
  vget(4, 0, "�m    �O(1.�k 2.�k)�G",buf,sizeof(cuser.sex)+1 ,DOECHO);
  cuser.sex=buf[0]-'0';
    
  vget(5, 0, "��    ��G(���+11/��/��)",buf,9,DOECHO);
  cuser.year =  (buf[0] - '0') * 10 + (buf[1] - '0');
  cuser.month = (buf[3] - '0') * 10 + (buf[4] - '0');
  cuser.day =   (buf[6] - '0') * 10 + (buf[7] - '0');

  cuser.money = 1000;    
#endif
            

  cuser.userlevel = PERM_DEFAULT;
  cuser.ufo = UFO_COLOR | UFO_MOVIE | UFO_BNOTE;
  /* Thor.980805: ����, �w�]�X��ufo */
  cuser.numlogins = 1;

  /* dispatch unique userno */

  cuser.firstlogin = cuser.lastlogin = cuser.tcheck = slot.uptime = ap_start;
  memcpy(slot.userid, userid, IDLEN);

  fd = open(FN_SCHEMA, O_RDWR | O_CREAT, 0600);
  {

    /* flock(fd, LOCK_EX); */
    /* Thor.981205: �� fcntl ���Nflock, POSIX�зǥΪk */
    f_exlock(fd);

    cuser.userno = try = uniq_userno(fd);
    write(fd, &slot, sizeof(slot));
    /* flock(fd, LOCK_UN); */
    /* Thor.981205: �� fcntl ���Nflock, POSIX�зǥΪk */
    f_unlock(fd);
  }
  close(fd);

  /* create directory */

  usr_fpath(buf, userid, NULL);
  mkdir(buf, 0755);
  strcat(buf, "/@");
  mkdir(buf, 0755);

  usr_fpath(buf, userid, fn_acct);
  fd = open(buf, O_WRONLY | O_CREAT, 0600);
  write(fd, &cuser, sizeof(cuser));
  close(fd);

  sprintf(buf, "%d", try);
  blog("APPLY", buf);
}

#endif /* LOGINASNEW */


/* ----------------------------------------------------- */
/* bad login						 */
/* ----------------------------------------------------- */


#define	FN_BADLOGIN	"logins.bad"


static void
logattempt(type)
  int type;			/* '-' login failure   ' ' success */
{
  char buf[128], fpath[80];

#if 0
  sprintf(buf, "%c%-12s[%s] %s\n", type, cuser.userid,
    Etime(&ap_start), currtitle);
  f_cat(FN_LOGIN_LOG, buf);
#endif

  str_stamp(fpath, &ap_start);
  /* sprintf(buf, "%s %cBBS\t%s\n", fpath, type, currtitle); */
  sprintf(buf, "%s %cBBS\t%s ip:%x\n", fpath, type, currtitle,tn_addr);
  /* Thor.980803: �l�� ip address */
  usr_fpath(fpath, cuser.userid, "log");
  f_cat(fpath, buf);

  if (type != ' ')
  {
    usr_fpath(fpath, cuser.userid, FN_BADLOGIN);
    sprintf(buf, "[%s] %s\n", Ctime(&ap_start),
      (type == '*' ? currtitle : fromhost));
    f_cat(fpath, buf);
  }
}


/* ----------------------------------------------------- */
/* �n�� BBS �{��					 */
/* ----------------------------------------------------- */


extern void talk_rqst();
extern void bmw_rqst();


static void
utmp_setup(mode)
  int mode;
{
  UTMP utmp;

  cutmp = NULL; /* Thor.980805: pal_cache���| check cutmp  */
  pal_cache();
  /* cutmp = NULL; */
  memset(&utmp, 0, sizeof(utmp));
  utmp.pid = currpid;
  utmp.userno = cuser.userno;
  utmp.mode = bbsmode = mode;
  utmp.in_addr = tn_addr;
  utmp.ufo = cuser.ufo;

  strcpy(utmp.userid, cuser.userid);
  strcpy(utmp.username, cuser.username);
  /* str_ncpy(utmp.from, fromhost, sizeof(utmp.from) - 1); */
  str_ncpy(utmp.from, fromhost, sizeof(utmp.from));
  /* Thor.980921: str_ncpy �t 0 */

  /* Thor: �i�DUser�w�g���F�񤣤U... */

  if (!utmp_new(&utmp))
  {
    login_abort("\n�z���諸��l�w�g�Q�H�������n�F�A�ФU���A�ӧa");
  }
}


/* ----------------------------------------------------- */
/* user login						 */
/* ----------------------------------------------------- */


static void
tn_login()
{
  int fd, attempts;
  usint level, ufo;
  time_t start;
  char fpath[80], uid[IDLEN + 1];

#ifndef CHAT_SECURE
  char passbuf[9];
#endif

  /* �� currtitle �@�� */

  sprintf(currtitle, "%s@%s", rusername, fromhost);

  move(16, 0);
  outs("\033[m���[�αb���Gguest�A�ӽзs�b���Gnew");

  move(b_lines-1,0);
  outs("sorry!!�����ϥΪ̸�ƥ���..�·нЭ��s���U..");   
  move(b_lines, 0);
  outs("�� �L�k�s�u�ɡA�ЧQ�� port 3456 8000 �W��");

  attempts = 0;
  for (;;)
  {
    if (++attempts > LOGINATTEMPTS)
    {
      film_out(FILM_TRYOUT, 0);
      login_abort("\n�A�� ...");
    }

    vget(17, 0, msg_uid, uid, IDLEN + 1, DOECHO);

    if (str_cmp(uid, "new") == 0)
    {

#ifdef LOGINASNEW
      acct_apply(); /* Thor.980917: ����: setup cuser ok */
      level = cuser.userlevel;
      ufo = cuser.ufo;
      /* cuser.userlevel = PERM_DEFAULT; */
      /* cuser.ufo = UFO_COLOR | UFO_MOVIE | UFO_BNOTE; */
      break;
#else
      outs("\n���t�Υثe�Ȱ��u�W���U, �Х� guest �i�J");
      continue;
#endif
    }
    else if (!*uid || (acct_load(&cuser, uid) < 0))
    {
      vmsg(err_uid);
    }
    else if (str_cmp(uid, STR_GUEST))
    {
      if (!vget(19, 0, MSG_PASSWD, passbuf, 9, NOECHO))
      {
	continue;	/* �o�{ userid ��J���~�A�b��J passwd �ɪ������L */
      }

      passbuf[8] = '\0';

      if (chkpasswd(cuser.passwd, passbuf))
      {
	logattempt('-');
	vmsg(ERR_PASSWD);
      }
      else
      {
	/* SYSOP gets all permission bits */

	if (!str_cmp(cuser.userid, str_sysop))
	  cuser.userlevel = ~0 ^ (PERM_DENYMAIL | PERM_DENYTALK |
	    PERM_DENYCHAT | PERM_DENYPOST);
	/* else */  /* Thor.980521: everyone should have level */

        level = cuser.userlevel;

	ufo = cuser.ufo & ~(HAS_PERM(PERM_LOGINCLOAK) ?
#if 0
	  (UFO_BRDNEW | UFO_BIFF | UFO_SOCKET) :
	  (UFO_BRDNEW | UFO_BIFF | UFO_SOCKET | UFO_CLOAK));
#endif
        /* lkchu.981201: �C���W�����n 'c' ��·� :p */
          (UFO_BIFF | UFO_SOCKET) :
          (UFO_BIFF | UFO_SOCKET | UFO_CLOAK));
                            
	if ((level & PERM_ADMIN) && (ufo & UFO_ACL))
	{
	  usr_fpath(fpath, cuser.userid, "acl");
          str_lower(rusername, rusername);      /* lkchu.981201: ���p�g */
          str_lower(fromhost, fromhost);	/* lkchu.981201: ���p�g */
	  if (!acl_has(fpath, rusername, fromhost))
	  {  /* Thor.980728: �`�N acl��, �M rusername,fromhost �n�����p�g */
	    logattempt('*');
	    login_abort("\n�A���W���a�I���ӹ�l�A�Юֹ� [�W���a�I�]�w��]");
	  }
	}

	logattempt(' ');

	/* check for multi-session */

	if (!(level & PERM_SYSOP))
	{
	  UTMP *ui;
	  pid_t pid;

	  if (level & (PERM_DENYLOGIN | PERM_PURGE))
	    login_abort("\n�o�ӱb���Ȱ��A�ȡA�Ա��ЦV�������ߡC");


	  if (!(ui = (UTMP *) utmp_find(cuser.userno)))
	    break;		/* user isn't logged in */


	  pid = ui->pid;
	  if (!pid || (kill(pid, 0) == -1))
	  {
	    memset(ui, 0, sizeof(UTMP));
	    break;		/* stale entry in utmp file */
	  }

	  if (vans("�z�Q�R����L���ƪ� login (Y/N)�ܡH[Y] ") != 'n')
	  {
	    kill(pid, SIGTERM);
	    blog("MULTI", cuser.username);
	    break;
	  }

	  if (utmp_count(cuser.userno, 0) > 2)
	    login_abort("\n�A�� ...");
	}
	break;
      }
    }
    else
    {				/* guest */
      logattempt(' ');
      cuser.userlevel = level = 0; 
      /* Thor.980917: �A�ݬݭn���n�j��g�^ cuser.userlevel */
      /* Thor.981207: �ȤH�ê�, �j��g�^cuser.userlevel */
      ufo = UFO_COLOR | UFO_PAGER | UFO_BNOTE | UFO_MOVIE;
      break; /* Thor.980917: ����: cuser maybe not ok ?*/
             /* Thor.980917: ����: cuser ok! */
    }
  }

  /* --------------------------------------------------- */
  /* �n���t��						 */
  /* --------------------------------------------------- */

  start = ap_start;


  sprintf(fpath, "%s (%d)", currtitle, currpid);


  blog("ENTER", fpath);
  if (rusername[0])
    strcpy(cuser.ident, currtitle);

  /* --------------------------------------------------- */
  /* ��l�� utmp�Bflag�Bmode				 */
  /* --------------------------------------------------- */

  cuser.ufo = ufo;
  utmp_setup(M_LOGIN); /* Thor.980917: ����: cutmp, cutmp-> setup ok */
  bbstate = STAT_STARTED;
  mbox_main();

  film_out(FILM_WELCOME, 0);

#ifdef MODE_STAT
  memset(&modelog, 0, sizeof(UMODELOG));
  mode_lastchange = ap_start;
#endif

  if (level)			/* not guest */
  {
    move(b_lines - 2, 0);
    prints("     �w��z�� [1;33m%d[m �׫��X�����A\
�W���z�Ӧ� [1;33m%s[m�A\n�ڰO�o���ѬO [1;33m%s[m�C",
      cuser.numlogins, cuser.lasthost, Ctime(&cuser.lastlogin));
    vmsg(NULL);

    usr_fpath(fpath, cuser.userid, FN_BADLOGIN);
    if (!more(fpath, (char *)-1) && vans("�z�n�R���W�z�O����(Y/N)?[Y]") != 'n')
                     /* lkchu.981201 */
      unlink(fpath);

    /*
     * Thor: �������ܹL userlevel, �ҥH substitute_record �����n�i�g�J
     * userlevel. �u���b userlogin ��, ���i�H�� userlevel, ��L�ɭԧ��i�H.
     */

    /* ------------------------------------------------- */
    /* �ֹ� user level					 */
    /* ------------------------------------------------- */

#ifdef JUSTIFY_PERIODICAL
    /* if (level & PERM_VALID) */
    if ((level & PERM_VALID) && !(level & PERM_SYSOP))
    {  /* Thor.980819: �������@�w�������{��, ex. SysOp  */
      fd = cuser.tvalid;

      if (fd + VALID_PERIOD < start)
      {
	level ^= PERM_VALID;
      }
      else
      {
	/* ���Įɶ��O�� 10 �ѫe���Xĵ�i */

	if (fd + VALID_PERIOD - 10 * 86400 < start)
	{
	  bell();
	  more("etc/re-reg", NULL);
	}
      }
    }
#endif

    if (!(level & PERM_VALID))
    {
      bell();
      more("etc/notify", NULL);
    }

    if (cuser.numemail > 5000 && cuser.firstlogin < 892112972 - 365 * 86400)
      cuser.numemail = 300;

    level |= (PERM_POST | PERM_PAGE | PERM_CHAT);


    if (!(level & PERM_SYSOP))
    {
#ifdef NEWUSER_LIMIT
      /* Thor.980825: lkchu patch: �J�M�� NEWUSER_LIMIT, �٬O�[�@�U�n�F, 
                                   �_�h�@���^�� FAQ ���֪�. :) */
      if (start - cuser.firstlogin < 3 * 86400)
      {
	/* if (!(level & PERM_VALID)) */
	level &= ~PERM_POST;	/* �Y�Ϥw�g�q�L�{�ҡA�٬O�n���ߤT�� */
	more("etc/newuser", NULL);
      }
#endif

      /* Thor.980629: ���g�����{��, �T�� chat/talk/write */
      if(!(level&PERM_VALID))
      {
        level &= ~(PERM_CHAT | PERM_PAGE);
      }


      if (level & PERM_DENYPOST)
	level &= ~PERM_POST;

      if (level & PERM_DENYTALK)
	level &= ~PERM_PAGE;

      if (level & PERM_DENYCHAT)
	level &= ~PERM_CHAT;

      if ((cuser.numemail >> 4) > (cuser.numlogins + cuser.numposts))
	level |= PERM_DENYMAIL;
    }

    cuser.userlevel = level;

    /* ------------------------------------------------- */
    /* �n�ͦW��P�B�B�M�z�L���H��			 */
    /* ------------------------------------------------- */

    if (start > cuser.tcheck + CHECK_PERIOD)
    {
      pal_sync(NULL);

#if 0
      vote_sync(NULL);		/* Thor: �M�����s�b���O�� */
#endif

      ufo |= m_quota(); /* Thor.980804: ����, ��ƾ�z�]�֦��]�t BIFF check */
      cuser.ufo = ufo;
      cuser.tcheck = start;
    }
    else
    {
      ufo = (ufo & ~UFO_MQUOTA) | m_query(cuser.userid);
    }

    cutmp->ufo = cuser.ufo = ufo; /* Thor.980805: �ѨM ufo �P�B���D */
#if 0
    cuser.ufo = ufo;
    if (ufo & UFO_BIFF)
    {
      cutmp->ufo = ufo;
    }
#endif

    /* ------------------------------------------------- */
    /* �N .ACCT �g�^					 */
    /* ------------------------------------------------- */

    cuser.lastlogin = start;
    str_ncpy(cuser.lasthost, fromhost, sizeof(cuser.lasthost));
    usr_fpath(fpath, cuser.userid, fn_acct);
    fd = open(fpath, O_WRONLY);
    write(fd, &cuser, sizeof(ACCT));
    close(fd);

    if (ufo & UFO_MQUOTA)
    {
      bell();
      more("etc/mquota", NULL);
    }

    ve_recover();

#ifdef LOGIN_NOTIFY
    if (!(ufo & UFO_CLOAK))     /* lkchu.981201: �ۤv�����N���� notify */
    {
      void loginNotify();
      loginNotify();
    }
    else                        /* lkchu.981201: �M�� benz, ���q���F */
    {
      usr_fpath(fpath, cuser.userid, FN_BENZ);
      unlink(fpath);
    }    
#endif

#ifdef  HAVE_ALOHA
    if (ufo & UFO_ALOHA)          /* lkchu.981201: �n�ͤW���q���]�w */
      aloha_setup();
#endif
  }
  else
  {
    vmsg(NULL);			/* Thor: for guest look welcome */
  }

  /* Thor.980917: ����: �즹�������ӳ]�w�n cuser. level? ufo? rusername bbstate cutmp cutmp-> */

  if (!(ufo & UFO_MOTD))
  {
    more("gem/@/@-day", NULL);	/* ����������D */
    pad_view();
  }

  showansi = ufo & UFO_COLOR;
}


/* ----------------------------------------------------- */
/* trap signals						 */
/* ----------------------------------------------------- */


static void
tn_signals()
{
  struct sigaction act;

  /* act.sa_mask = 0; */ /* Thor.981105: �зǥΪk */
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  act.sa_handler = (void *)abort_bbs;
  sigaction(SIGBUS, &act, NULL);
  sigaction(SIGSEGV, &act, NULL);
  sigaction(SIGTERM, &act, NULL);
  sigaction(SIGXCPU, &act, NULL);
  sigaction(SIGSYS, &act, NULL);/* bad argument to system call */

  act.sa_handler = (void *)talk_rqst;
  sigaction(SIGUSR1, &act, NULL);

  act.sa_handler = (void *)bmw_rqst;
  sigaction(SIGUSR2, &act, NULL);

  /* sigblock(sigmask(SIGPIPE)); */
  /* Thor.981206: lkchu patch: �Τ@ POSIX �зǥΪk  */
  /* �b���ɥ� sigset_t act.sa_mask */
  sigaddset(&act.sa_mask, SIGPIPE);
  sigprocmask(SIG_BLOCK, &act.sa_mask, NULL);
    
}


static inline void
tn_main()
{
  clear();
  film_out((time(&ap_start) % 77) + FILM_MOVIE + 7, 1);
  prints("\033[1;37mfall.twbbs.org �� �x�W��ޤj�� �� 140.118.236.220\033[0m\n\
\033[1;33m�w����{\033[1;37m �i\033[1;44;36m %s \033[m�j\033[1;37m�ثe�u�W�H�� [\033[1;33m%d\033[1;37m] �H", 
    str_site, ushm->count);
     
  currpid = getpid();

  tn_signals();  /* Thor.980806: ��� tn_login�e, �H�K call in���|�Q�� */
  tn_login();
  /*tn_signals(); */

  prints("1"); 
  board_main();
  prints("2"); 
  gem_main();
  prints("3"); 
  talk_main();
  prints("4"); 

  menu();
  abort_bbs();			/* to make sure it will terminate */
}


/* ----------------------------------------------------- */
/* FSA (finite state automata) for telnet protocol	 */
/* ----------------------------------------------------- */


static void
telnet_init()
{
  static char svr[] = {
    IAC, DO, TELOPT_TTYPE,
    IAC, SB, TELOPT_TTYPE, TELQUAL_SEND, IAC, SE,
    IAC, WILL, TELOPT_ECHO,
    IAC, WILL, TELOPT_SGA
  };

  int n, len;
  char *cmd;
  int rset;
  struct timeval to;
  char buf[64];

  /* --------------------------------------------------- */
  /* init telnet protocol				 */
  /* --------------------------------------------------- */

  to.tv_sec = 1;
  to.tv_usec = 1;
  cmd = svr;

  for (n = 0; n < 4; n++)
  {
    len = (n == 1 ? 6 : 3);
    send(0, cmd, len, 0);
    cmd += len;

    rset = 1;
    if (select(1, (fd_set *) & rset, NULL, NULL, &to) > 0)
      recv(0, buf, sizeof(buf), 0);
  }
}


/* ----------------------------------------------------- */
/* stand-alone daemon					 */
/* ----------------------------------------------------- */


static void
start_daemon(inetd, port)
  int inetd;
  int port; /* Thor.981206: �� 0 �N�� *�S���Ѽ�* */
{
  int n;
  struct linger ld;
  struct sockaddr_in sin;
  struct rlimit rl;
  char buf[80], data[80];
  time_t val;

  /*
   * More idiot speed-hacking --- the first time conversion makes the C
   * library open the files containing the locale definition and time zone.
   * If this hasn't happened in the parent process, it happens in the
   * children, once per connection --- and it does add up.
   */

  time(&val);
  strftime(buf, 80, "%d/%b/%Y %H:%M:%S", localtime(&val));

  /* --------------------------------------------------- */
  /* adjust resource : 16 mega is enough		 */
  /* --------------------------------------------------- */

  rl.rlim_cur = rl.rlim_max = 16 * 1024 * 1024;
  /* setrlimit(RLIMIT_FSIZE, &rl); */
  setrlimit(RLIMIT_DATA, &rl);

#ifdef SOLARIS
#define RLIMIT_RSS RLIMIT_AS
  /* Thor.981206: port for solaris 2.6 */
#endif

  setrlimit(RLIMIT_RSS, &rl);

  rl.rlim_cur = rl.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rl);

  rl.rlim_cur = rl.rlim_max = 60 * 20;
  setrlimit(RLIMIT_CPU, &rl);

  /* --------------------------------------------------- */
  /* speed-hacking DNS resolve				 */
  /* --------------------------------------------------- */

  dns_init();

  /* --------------------------------------------------- */
  /* change directory to bbshome       			 */
  /* --------------------------------------------------- */

  chdir(BBSHOME);
  umask(077);

  /* --------------------------------------------------- */
  /* detach daemon process				 */
  /* --------------------------------------------------- */

  close(1);
  close(2);

  if(inetd) /* Thor.981206: inetd -i */
  {
    /* Give up root privileges: no way back from here	 */
    setgid(BBSGID);
    setuid(BBSUID);
#if 0
    if (getsockname(0, &sin, &n) >= 0)
      mport = port = ntohs(sin.sin_port);
#endif
    mport = port;

    sprintf(data, "%d\t%s\t%d\tinetd -i\n", getpid(), buf, port);
    f_cat(PID_FILE, data);
    return;
  }

  close(0);

  if (fork())
    exit(0);

  setsid();

  if (fork())
    exit(0);

  /* --------------------------------------------------- */
  /* fork daemon process				 */
  /* --------------------------------------------------- */

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;

  if (port <= 0) /* Thor.981206: port 0 �N��S���Ѽ� */
  {
    n = MAXPORTS - 1;
    while (n)
    {
      if (fork() == 0)
	break;

      sleep(1);
      n--;
    }
    port = myports[n];
  }

  n = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  val = 1;
  setsockopt(n, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(val));

#if 0
  setsockopt(n, SOL_SOCKET, SO_KEEPALIVE, (char *) &val, sizeof(val));

  setsockopt(n, IPPROTO_TCP, TCP_NODELAY, (char *) &val, sizeof(val));
#endif

  ld.l_onoff = ld.l_linger = 0;
  setsockopt(n, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));

  mport = port;
  sin.sin_port = htons(port);
  if ((bind(n, (struct sockaddr *)&sin, sizeof(sin)) < 0) || (listen(n, QLEN) < 0))
    exit(1);

  /* --------------------------------------------------- */
  /* Give up root privileges: no way back from here	 */
  /* --------------------------------------------------- */

  setgid(BBSGID);
  setuid(BBSUID);

  sprintf(data, "%d\t%s\t%d\n", getpid(), buf, port);
  f_cat(PID_FILE, data);
}


/* ----------------------------------------------------- */
/* reaper - clean up zombie children			 */
/* ----------------------------------------------------- */


static inline void
reaper()
{
  while (waitpid(-1, NULL, WNOHANG | WUNTRACED) > 0);
}


#ifdef	SERVER_USAGE
static void
servo_usage()
{
  struct rusage ru;
  FILE *fp;

  fp = fopen("run/bbs.usage", "a");

  if (!getrusage(RUSAGE_CHILDREN, &ru))
  {
    fprintf(fp, "\n[Server Usage] %d: %d\n\n"
      "user time: %.6f\n"
      "system time: %.6f\n"
      "maximum resident set size: %lu P\n"
      "integral resident set size: %lu\n"
      "page faults not requiring physical I/O: %d\n"
      "page faults requiring physical I/O: %d\n"
      "swaps: %d\n"
      "block input operations: %d\n"
      "block output operations: %d\n"
      "messages sent: %d\n"
      "messages received: %d\n"
      "signals received: %d\n"
      "voluntary context switches: %d\n"
      "involuntary context switches: %d\n\n",

      getpid(), ap_start,
      (double) ru.ru_utime.tv_sec + (double) ru.ru_utime.tv_usec / 1000000.0,
      (double) ru.ru_stime.tv_sec + (double) ru.ru_stime.tv_usec / 1000000.0,
      ru.ru_maxrss,
      ru.ru_idrss,
      ru.ru_minflt,
      ru.ru_majflt,
      ru.ru_nswap,
      ru.ru_inblock,
      ru.ru_oublock,
      ru.ru_msgsnd,
      ru.ru_msgrcv,
      ru.ru_nsignals,
      ru.ru_nvcsw,
      ru.ru_nivcsw);
  }

  fclose(fp);
}
#endif


static void
main_term()
{
#ifdef	SERVER_USAGE
  servo_usage();
#endif
  exit(0);
}


static inline void
main_signals()
{
  struct sigaction act;

  /* act.sa_mask = 0; */ /* Thor.981105: �зǥΪk */
  sigemptyset(&act.sa_mask);      
  act.sa_flags = 0;

  act.sa_handler = reaper;
  sigaction(SIGCHLD, &act, NULL);

  act.sa_handler = main_term;
  sigaction(SIGTERM, &act, NULL);

#ifdef	SERVER_USAGE
  act.sa_handler = servo_usage;
  sigaction(SIGPROF, &act, NULL);
#endif

  /* sigblock(sigmask(SIGPIPE)); */
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  int csock;			/* socket for Master and Child */
  int *totaluser;
  int value;
  struct sockaddr_in sin;

  /* --------------------------------------------------- */
  /* setup standalone daemon				 */
  /* --------------------------------------------------- */

  /* start_daemon(argc > 1 ? atoi(argv[1]) : 0); */
  /* Thor.981206: �� 0 �N�� *�S���Ѽ�* */
  /* start_daemon(argc > 1 ? atoi(argv[1]) : -1); */

  /* Thor.981207: usage,  bbsd, or bbsd 1234, or bbsd -i 1234 */
  start_daemon(argc > 2, atoi(argv[argc-1]));

  main_signals();

  /* --------------------------------------------------- */
  /* attach shared memory & semaphore			 */
  /* --------------------------------------------------- */

  /* sem_init(); */
  ushm_init();
  bshm_init();
  fshm_init();

  /* --------------------------------------------------- */
  /* main loop						 */
  /* --------------------------------------------------- */

  totaluser = &ushm->count;
  /* avgload = &ushm->avgload; */

  for (;;)
  {
    value = 1;
    if (select(1, (fd_set *) & value, NULL, NULL, NULL) < 0)
      continue;

    value = sizeof(sin);
    csock = accept(0, (struct sockaddr *)&sin, &value);
    if (csock < 0)
    {
      reaper();
      continue;
    }

    ap_start++;
    argc = *totaluser;
    if (argc >= MAXACTIVE - 5 /* || *avgload > THRESHOLD */ )
    {
      sprintf(currtitle,
	"�ثe�u�W�H�� [%d] �H�A�t�ι��M�A�еy��A��\n", argc);
      send(csock, currtitle, strlen(currtitle), 0);
      close(csock);
      continue;
    }

    if (fork())
    {
      close(csock);
      continue;
    }

    dup2(csock, 0);
    close(csock);

    /* ------------------------------------------------- */
    /* ident remote host / user name via RFC931		 */
    /* ------------------------------------------------- */

    /* rfc931(&sin, fromhost, rusername); */

    tn_addr = sin.sin_addr.s_addr;
    dns_ident(mport, &sin, fromhost, rusername);

    telnet_init();
    tn_main();
  }
}
