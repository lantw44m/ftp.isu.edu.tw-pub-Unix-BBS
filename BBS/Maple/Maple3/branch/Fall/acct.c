/*-------------------------------------------------------*/
/* acct.c	( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* target : account / administration routines	 	 */
/* create : 95/03/29				 	 */
/* update : 96/04/05				 	 */
/*-------------------------------------------------------*/


#define	_ADMIN_C_


#include "bbs.h"


extern BCACHE *bshm;


#define STR_PERM      "bctpjm#x--------PTCM--L*B#--ACBS"


/* ----------------------------------------------------- */
/* (.ACCT) �ϥΪ̱b�� (account) subroutines		 */
/* ----------------------------------------------------- */


int
acct_load(acct, userid)
  ACCT *acct;
  char *userid;
{
  int fd;

  usr_fpath((char *) acct, userid, fn_acct);
  fd = open((char *) acct, O_RDONLY);
  if (fd >= 0)
  {
    read(fd, acct, sizeof(ACCT));
    close(fd);
  }
  return fd;
}


void
acct_save(acct)
  ACCT *acct;
{
  int fd;
  char fpath[80];

  usr_fpath(fpath, acct->userid, fn_acct);
  fd = open(fpath, O_WRONLY, 0600);	/* fpath �����w�g�s�b */
  if (fd >= 0)
  {
    write(fd, acct, sizeof(ACCT));
    close(fd);
  }
}


int
acct_userno(userid)
  char *userid;
{
  int fd;
  int userno;
  char fpath[80];

  usr_fpath(fpath, userid, fn_acct);
  fd = open(fpath, O_RDONLY);
  if (fd >= 0)
  {
    read(fd, &userno, sizeof(userno));
    close(fd);
    return userno;
  }
  return 0;
}


/* ----------------------------------------------------- */
/* name complete for user ID				 */
/* ----------------------------------------------------- */
/* return value :					 */
/* 0 : �ϥΪ����� enter ==> cancel			 */
/* -1 : bad user id					 */
/* ow.: �Ǧ^�� userid �� userno				 */
/* ----------------------------------------------------- */


int
acct_get(msg, acct)
  char *msg;
  ACCT *acct;
{
  if (!vget(1, 0, msg, acct->userid, IDLEN + 1, GET_USER))
    return 0;

  if (acct_load(acct, acct->userid) >= 0)
    return acct->userno;

  vmsg(err_uid);
  return -1;
}


/* ----------------------------------------------------- */
/* �]�w�t���ɮ�						 */
/* ----------------------------------------------------- */


static void
x_file(mode, xlist, flist)
  int mode;			/* M_XFILES / M_UFILES */
  char *xlist[];		/* description list */
char *flist[];			/* filename list */
{
  int n, i;
  char *fpath, *desc;
  char buf[80];

  move(12, 0);
  clrtobot();
  n = 0;
  while (desc = xlist[n])
  {
    n++;
    move(n + 12, 17);
    prints("(\033[1;36m%d\033[m) %s", n, desc);
    if (mode == M_XFILES)
    {
      move(n + 12, 50); /* Thor.980806: ����: �L�X�ɦW */
      outs(flist[n - 1]);
    }
  }

  vget(b_lines, 0, "�п���ɮ׽s���A�Ϋ� [0] �����G", buf, 3, DOECHO);
  i = atoi(buf);
  if (i <= 0 || i > n)
    return;

  n = vget(b_lines, 36, "(D)�R�� (E)�s�� [Q]�����H", buf, 3, LCECHO);
  if (n != 'd' && n != 'e')
    return;

  fpath = flist[--i];
  if (mode == M_UFILES)
    usr_fpath(buf, cuser.userid, fpath);
  else				/* M_XFILES */
    sprintf(buf, "etc/%s", fpath);

  if (n == 'd')
  {
    unlink(buf);
  }
  else
  {
    vmsg(vedit(buf, NA) ? "��ʤ���" : "��s����");
  } /* Thor.981020: �`�N�Qtalk�����D  */
}


int
m_xfile()
{
  static char *desc[] =
  {
    "�ݪO����",
    "�����W��",
    "���U���ܵe��",
    "�s��W������",
    "�����{�ҫH��",
    "�����{�Ҫ���k",
    "E-mail post ²��",
    "�s�i/�U���H�W��",          /* lkchu.981201: �u�W�s�� mail.acl, 
                                                 �������] bmtad �~���s */
    NULL
  };

  static char *path[] =
  {
    "expire.conf",
    "badid",
    "register",
    "newuser",
    "valid",
    "justify",
    "emailpost",
    "mail.acl"
  };

  x_file(M_XFILES, desc, path);
  return 0;
}


/* ----------------------------------------------------- */
/* bit-wise display and setup				 */
/* ----------------------------------------------------- */


static void
bitmsg(msg, str, level)
  char *msg, *str;
  int level;
{
  int cc;

  outs(msg);
  while (cc = *str)
  {
    outc((level & 1) ? cc : '-');
    level >>= 1;
    str++;
  }

  outc('\n');
}


usint
bitset(pbits, count, maxon, msg, perms)
  usint pbits;
  int count;			/* �@���X�ӿﶵ */
  int maxon;			/* �̦h�i�H enable �X�� */
  char *msg;
  char *perms[];
{
  int i, j, on;

  extern char radix32[32];

  move(2, 0);
  clrtobot();
  outs(msg);

  for (i = on = 0, j = 1; i < count; i++)
  {
    msg = "��";
    if (pbits & j)
    {
      on++;
      msg = "��";
    }
    move(5 + (i & 15), (i < 16 ? 0 : 40));
    prints("%c %s %s", radix32[i], msg, perms[i]);
    j <<= 1;
  }

  while (i = vans("�Ы�������]�w�A�Ϋ� [Return] �����G"))
  {
    i -= '0';
    if (i >= 10)
      i -= 'a' - '0' - 10;

    if (i >= 0 && i < count)
    {
      j = 1 << i;
      if (pbits & j)
      {
	on--;
	msg = "��";
      }
      else
      {
	if (on >= maxon)
	  continue;
	on++;
	msg = "��";
      }

      pbits ^= j;
      move(5 + (i & 15), (i < 16 ? 2 : 42));
      outs(msg);
    }
  }
  return (pbits);
}


static usint
setperm(level)
  usint level;
{
  if (cuser.userlevel & PERM_SYSOP)
    return bitset(level, NUMPERMS, NUMPERMS, MSG_USERPERM, perm_tbl);

  /* [�b���޲z��] ����� SYSOP */

  if (level & PERM_SYSOP)
    return level;

  return bitset(level, NUMPERMS - 4, NUMPERMS - 4, MSG_USERPERM, perm_tbl);
}


/* ----------------------------------------------------- */
/* �b���޲z						 */
/* ----------------------------------------------------- */


static void
bm_list(userid)			/* ��� userid �O���ǪO���O�D */
  char *userid;
{
  int len, ch;
  BRD *bhdr, *tail;
  char *list;
  extern BCACHE *bshm;

  len = strlen(userid);
  outs("����O�D�G");

  bhdr = bshm->bcache;
  tail = bhdr + bshm->number;

  do
  {
    list = bhdr->BM;
    ch = *list;
    if ((ch > ' ') && (ch < 128))
    {
      do
      {
	if (!str_ncmp(list, userid, len))
	{
	  ch = list[len];
	  if ((ch == 0) || (ch == '/'))
	  {
	    outs(bhdr->brdname);
	    outc(' ');
	    break;
	  }
	}
	while (ch = *list++)
	{
	  if (ch == '/')
	    break;
	}
      } while (ch);
    }
  } while (++bhdr < tail);

  outc('\n');
}


void
acct_show(u, adm)
  ACCT *u;
  int adm;			/* 1: admin 2: reg-form */
{
  int diff;
  usint ulevel;
  char *uid, buf[80];

  clrtobot();

  uid = u->userid;
  if (adm != 2)
    prints("\n�N    ���G%s", uid);

  prints(
    "\n��    �١G%s\n"
    "�u��m�W�G%s\n"
    "�~���}�G%s\n"
    "�l��H�c�G%s\n",
    u->username,
    u->realname,
    u->address,
    u->email);

  prints("���U����G%s", ctime(&u->firstlogin));

  prints("���{����G%s", ctime(&u->lastlogin));

  diff = u->staytime / 60;
  prints("�W�����ơG%d �� (�@ %d �� %d ��)\n",
    u->numlogins, diff / 60, diff % 60);

  prints("�峹�ƥءG%d �g     ", u->numposts);


  usr_fpath(buf, uid, fn_dir);
  prints("�ӤH�H��G%d ��\n", rec_num(buf, sizeof(HDR)));

#ifdef COINAGE
/* mat 981206: �W�[���A�m�O�A�ͤ骺��� */
  if(!(u->sex==0))
     prints("��    �O�G%s      ",((u->sex==1) ? "�k�_�_" :  "�k�_�_"));
   
  prints("�͡@�@��G19%02d/%02d/%02d\n",u->year,u->month,u->day);
    
/*  prints("��    ���G%-9d  ",u->gold); */
  prints("�{    ���G%-9d\n",u->money);
     
  prints("�����s�ڡG%-9d   �ȹ��s�ڡG%-9d\n",u->bankgold,u->banksilver);
      
#endif

  ulevel = u->userlevel;  
  outs("�����{�ҡG\033[32m");
  if (ulevel & PERM_VALID)
  {
    outs(u->tvalid ? Ctime(&u->tvalid) : "���Ĵ����w�L�A�Э��s�{��");
  }
  else
  {
    outs("�аѦҥ������G��i��T�{�A�H���@�v��");
  }
  outs("\033[m\n");

  if (adm)
  {
    prints("�{�Ҹ�ơG%s\n", u->justify);
    prints("�{�Ҧa�}�G%s\n", u->vmail);
    prints("RFC 931 �G%s\n", u->ident);
    prints("�W���a�I�G%s (%d)\n", u->lasthost, u->numemail);
    bitmsg(MSG_USERPERM, STR_PERM, ulevel);
    bitmsg("�X    �СG", "amnsEPQFAC", u->ufo);
  }
  else
  {
    diff = (time(0) - ap_start) / 60;
    prints("���d�����G%d �p�� %d ��\n", diff / 60, diff % 60);
  }

  if (adm == 2)
    return;

  /* Thor: �Q�ݬݳo�� user �O���Ǫ������D */

  if (ulevel & PERM_BM)
    bm_list(uid);

#ifdef	NEWUSER_LIMIT
  if (u->lastlogin - u->firstlogin < 3 * 86400)
    outs("\n�s��W���G�T�ѫ�}���v��");
#endif
}


void
acct_setup(u, adm)
  ACCT *u;
  int adm;
{
  ACCT x;
  int i, num;
  char *str, buf[80], pass[PASSLEN];
#ifdef COINAGE
  unsigned long int longnum;
#endif
 

  acct_show(u, adm);
  memcpy(&x, u, sizeof(ACCT));

  if (adm)
  {
    adm = vans("�]�w 1)��� 2)�v�� Q)���� [Q] ");
    if (adm == '2')
      goto set_perm;

    if (adm != '1')
      return;
  }
  else
  {
    if (vans("�ק���(Y/N)?[N] ") != 'y')
      return;
  }

  move(i = 3, 0);
  clrtobot();

  if (adm)
  {
    str = x.userid;
    for (;;)
    {
      vget(i, 0, "�ϥΪ̥N��(����Ы� Enter)�G", str, IDLEN + 1, GCARRY);
      if (!str_cmp(str, u->userid) || !acct_userno(str))
	break;
      vmsg("���~�I�w���ۦP ID ���ϥΪ�");
    }
  }
  else
  {
    vget(i, 0, "�нT�{�K�X�G", buf, PASSLEN, NOECHO);
    if (chkpasswd(u->passwd, buf))
    {
      vmsg("�K�X���~");
      return;
    }
  }

  i++;
  for (;;)
  {
    if (!vget(++i, 0, "�]�w�s�K�X(����Ы� Enter)�G", buf, PASSLEN, NOECHO))
      break;

    strcpy(pass, buf);
    vget(i + 1, 0, "�ˬd�s�K�X�G", buf, PASSLEN, NOECHO);
    if (!strcmp(buf, pass))
    {
      buf[8] = '\0';
      str_ncpy(x.passwd, genpasswd(buf), PASSLEN);
      i++;
      break;
    }
  }

  i++;
  str = x.username;
  do
  {
    vget(i, 0, "��    �١G", str, sizeof(x.username), GCARRY);
  } while (str_len(str) < 1);

  i++;
  str = x.realname;
  do
  {
    vget(i, 0, "�u��m�W�G", str, sizeof(x.realname), GCARRY);
  } while (str_len(str) < 4);

  i++;
  str = x.address;
  do
  {
    /* Thor.980730:lkchu patch: �Y�O i++ �h�|�@�����U�L */
    vget(i, 0, "�~��a�}�G", str, sizeof(x.address), GCARRY);
  } while (str_len(str) < 8);

#ifdef COINAGE

  sprintf(buf, "%d", u->sex);
  vget(++i, 0, "��    �O�G", buf, 2,GCARRY);
  if ((num = atoi(buf)) >= 0)
    x.sex = num;
    
  sprintf(buf, "%02d/%02d/%02d",u->year,u->month,u->day);
  vget(++i, 0, "��    ��G", buf, 9,GCARRY);
  x.year =  (buf[0] - '0') * 10 + (buf[1] - '0');
  x.month = (buf[3] - '0') * 10 + (buf[4] - '0');
  x.day =   (buf[6] - '0') * 10 + (buf[7] - '0');

#endif        
  if (adm)
  {
    vget(++i, 0, "�{�Ҹ�ơG", x.justify, 44, GCARRY);

    sprintf(buf, "%d", u->numlogins);
    vget(++i, 0, "�W�u���ơG", buf, 10, GCARRY);
    if ((num = atoi(buf)) >= 0)
      x.numlogins = num;

    sprintf(buf, "%d", u->numposts);
    vget(++i, 0, "�峹�g�ơG", buf, 10, GCARRY);
    if ((num = atoi(buf)) >= 0)
      x.numposts = num;
    
#ifdef COINAGE

#if 0
    sprintf(buf, "%d", u->gold);
    vget(++i, 0, "��    ���G", buf,10 ,GCARRY);
    if (((longnum = atol(buf))>=0)  && (longnum <= MAXMONEY))
      x.gold=longnum;
#endif    
    
    sprintf(buf, "%d", u->money);
    vget(++i, 0, "�{    ���G", buf,10 ,GCARRY);
    if (((longnum = atol(buf))>=0)  &&(longnum <= MAXMONEY))
      x.money=longnum; 
           
    sprintf(buf, "%d", u->bankgold);
    vget(++i, 0, "�����s�ڡG", buf,10 ,GCARRY);
    if (((longnum = atol(buf))>=0)  &&(longnum <= MAXMONEY))
      x.bankgold=longnum;

    sprintf(buf, "%d", u->banksilver);
    vget(++i, 0, "�ȹ��s�ڡG", buf,10 ,GCARRY);
    if (((longnum = atol(buf))>=0)  &&(longnum <= MAXMONEY))
      x.banksilver=longnum;
                                    
#endif
                                                                  
    /* lkchu.981201: �S��γ~ :p */
    vget(++i, 0, "�{�Ҧa�}�G", x.vmail, 44, GCARRY);
    vget(++i, 0, "�W���a�I�G", x.lasthost, 30, GCARRY);
    vget(++i, 0, "RFC 931 �G", x.ident, 44, GCARRY);

    if (vans("�]�w�v��(Y/N)?[N] ") == 'y')
    {
set_perm:

      i = setperm(num = x.userlevel);

      if (i == num)
      {
	vmsg("�����ק�");
	if (adm == '2')
	  return;
      }
      else
      {
	x.userlevel = i;
      }
    }
  }

  if (vans(msg_sure_ny) != 'y')
    return;

  if (adm)
  {
    char buf[256];
    time_t now;
  
    if (str_cmp(u->userid, x.userid))
    { /* Thor: 980806: �S�O�`�N�p�G usr�C�Ӧr�����b�P�@partition���ܷ|�����D */
      char dst[80];

      usr_fpath(buf, u->userid, NULL);
      usr_fpath(dst, x.userid, NULL);
      rename(buf, dst);
    }
    /* lkchu.981201: security log */
    time(&now);
    sprintf(buf, "[%s] %12.12s ��� %12.12s ���v�� %x --> %x\n", Etime(&now),
             cuser.userid, x.userid, u->userlevel, x.userlevel);
    f_cat(FN_SECURITY, buf);
                             
  }
  else
  {
    /* Thor: �o�˧Y�Ϧb�u�W, �]�i�H�� userlevel */

    if (acct_load(u, x.userid) >= 0)
      x.userlevel = u->userlevel;
  }

  memcpy(u, &x, sizeof(x));
  acct_save(u);
}


int
u_info()
{
  char *str, username[24]; /* Thor.980727:lkchu patch: username[20] -> 24 */

  move(2, 0);
  strcpy(username, str = cuser.username);
  acct_setup(&cuser, 0);
  if (strcmp(username, str))
    memcpy(cutmp->username, str, sizeof(cuser.username));
  return 0;
}


int
m_user()
{
  int ans;
  ACCT acct;

  while (ans = acct_get(msg_uid, &acct))
  {
    if (ans > 0)
      acct_setup(&acct, 1);
  }
  return 0;
}


/* ----------------------------------------------------- */
/* �]�w E-mail address					 */
/* ----------------------------------------------------- */


static int
ban_addr(addr)
  char *addr;
{
  int i;
  char *host, *str;
  char foo[64]; /* SoC: ��m���ˬd�� email address */

  static char *invalid[] =
  {"@bbs", "bbs@", "root@", "gopher@",
    "guest@", "@ppp", "@slip", "@dial", "unknown@", "@anon.penet.fi",
    "193.64.202.3", NULL
  };

  /* SoC: �O���� email ���j�p�g */
  str_lower(foo, addr);

  for (i = 0; str = invalid[i]; i++)
  {
    if (strstr(foo, str))
      return 1;
  }

  /* check for mail.acl (lower case filter) */

  host = (char *) strchr(foo, '@');
  *host = '\0';
  i = acl_has(MAIL_ACLFILE, foo, host + 1);
  /* *host = '@'; */
  return i > 0;
}


int
u_addr()
{
  char *msg, addr[64];

  more("etc/e-mail", (char *)-1);
		     /* lkchu.981201 */
  if (vget(b_lines - 2, 0, "E-Mail �a�}�G", addr, sizeof(cuser.email), DOECHO))
  {
    if (not_addr(addr) || ban_addr(addr))
    {
      msg = "���X�檺 E-mail address";
    }
    else
    {
      int vtime;

      vtime = bsmtp(NULL, NULL, addr, MQ_JUSTIFY);

      if (vtime < 0)
      {
	msg = "�����{�ҫH��L�k�H�X�A�Х��T��g E-mail address";
      }
      else
      {
	cuser.userlevel &= ~(PERM_VALID /* | PERM_POST | PERM_PAGE */ );
	cuser.vtime = vtime;
	strcpy(cuser.email, addr);
	acct_save(&cuser);

	more("etc/justify", (char *)-1);
			    /* lkchu.981201 */
	prints("\n%s(%s)�z�n�A�ѩ�z��s E-mail address ���]�w�A\n"
	  "�бz���֨� \033[44m%s\033[m �Ҧb���u�@���^�Сy�����{�ҫH��z�C",
	  cuser.userid, cuser.username, addr);
	msg = NULL;
      }
    }
    vmsg(msg);
  }

  return 0;
}


int
u_setup()
{
  int ufo, nflag, len;

  static char *flags[] =
  {
     /* COLOR */ "�m��Ҧ�",
     /* MOVIE */ "�ʵe���",
     /* BRDNEW */ "�s�峹",
     /* BNOTE */ "��ܶi�O�e��",
     /* VEDIT */ "²�ƽs�边",

     /* PAGER */ "�����I�s��",
     /* QUITE */ "��������",
     /* PAL */ "�u��ܦn��",
     /* ALOHA */ "�n�ͤW���q�� -",

     /* MOTD */ "²�ƶi���e��",

     /* SELECT */ "�ֳt�����u�W�ϥΪ�", /* lkchu.981201: ��ܽu�W�ϥΪ̦��� */
    "�O�d",
    "�O�d",
    "�O�d",
    "�O�d",
    "�O�d",
    "�O�d",
    "�O�d",
    "�O�d",

     /* CLOAK */ "�����N",
     /* ACL */ "ACL"
  };

  nflag = cuser.userlevel;
  if (!nflag)
    len = 5;
  else if (nflag & PERM_ADMIN)
    len = 21;  
  /* Thor.980910: �ݪ`�N��PERM_ADMIN���F�i��acl, �ٶ��K�]�i�H�������N�F:P */
  else if (nflag & PERM_CLOAK)
    len = 20;
  else
    len = 11;   /* lkchu.981201: �W�[���حn��� len �j�p */

  ufo = cuser.ufo;
  nflag = bitset(ufo, len, len, "�ާ@�Ҧ��]�w�G", flags);
  if (nflag != ufo)
  {
    /* Thor.980805: �ѨM ufo BIFF���P�B���D */
    nflag = (nflag & ~UFO_UTMP_MASK) | (cutmp->ufo & UFO_UTMP_MASK);

    cutmp->ufo = cuser.ufo = nflag; 
    /* Thor.980805: �n�S�O�`�N cuser.ufo�Mcutmp->ufo��UFO_BIFF���P�B���D,�A�� */

    showansi = nflag & UFO_COLOR;
    outs(str_ransi);
  }
  return 0;
}


int
u_lock()
{
  char buf[PASSLEN];

  blog("LOCK ", "screen");

  do
  {
    vget(b_lines, 0, "�� �п�J�K�X�A�H�Ѱ��ù���w�G",
      buf, PASSLEN, NOECHO);
  } while (chkpasswd(cuser.passwd, buf));

  return XEASY;
}


int
u_xfile()
{
  int i;

  static char *desc[] = {
    "�W���a�I�]�w��",
    "�W����",
    "ñ�W��",
#if 0
    /* Thor.981108: ����ק�, �H�K�d�� */
    "���T�O��",
#endif
    "�Ȧs��.1",
    "�Ȧs��.2",
    "�Ȧs��.3",
    "�Ȧs��.4",
    "�Ȧs��.5",
  NULL};

  static char *path[] = {
    "acl",
    "plans",
    "sign",
#if 0
    /* Thor.981108: ����ק�, �H�K�d�� */
    FN_BMW,
#endif
    "buf.1",
    "buf.2",
    "buf.3",
    "buf.4",
    "buf.5"
  };

  i = (cuser.userlevel & PERM_ADMIN) ? 0 : 1;
  x_file(M_UFILES, &desc[i], &path[i]);
  return 0;
}


/* ----------------------------------------------------- */
/* �ݪO�޲z						 */
/* ----------------------------------------------------- */


static int
valid_brdname(brd)
  char *brd;
{
  int ch;

  if (!is_alnum(*brd))
    return 0;

  while (ch = *++brd)
  {
    if (!is_alnum(ch) && ch != '.' && ch != '-' && ch != '_')
      return 0;
  }
  return 1;
}


static int
m_setbrd(brd)
  BRD *brd;
{
  int i;
  char *data, buf[4], old_brdname[IDLEN + 1];

  data = brd->brdname;
  i = *data ? 11 : 1;
  strcpy(old_brdname, data);

  for (;;)
  {
    if (!vget(i, 0, MSG_BID, data, IDLEN + 1, GCARRY))
    {
      if (i == 1)
	return -1;

      strcpy(data, old_brdname);/* Thor:�Y�O�M�ūh�]����W�� */
      continue;
    }

    if (!strcmp(old_brdname, data) && valid_brdname(data))
    {				/* Thor: �P��W�P�h���L */
      break;
    }

    if (brd_bno(data) >= 0)
    {
      outs("\n���~! �O�W�p�P");
    }
    else if (valid_brdname(data))
    {
      break;
    }
  }

  vget(++i, 0, "�ݪO�D�D�G", brd->title, BTLEN + 1, GCARRY);
  vget(++i, 0, "�O�D�W��G", brd->BM, BMLEN + 1, GCARRY);

  data = buf;

  if (vget(++i, 0, "�\\Ū�v��(Y/N)�H[N] ", data, 3, LCECHO) == 'y')
  {
    brd->readlevel =
      bitset(brd->readlevel, NUMPERMS, NUMPERMS, MSG_READPERM, perm_tbl);
    move(2, 0);
    clrtobot();
    i = 1;
  }

  if (vget(++i, 0, "�o���v��(Y/N)�H[N] ", data, 3, LCECHO) == 'y')
  {
    brd->postlevel =
      bitset(brd->postlevel, NUMPERMS, NUMPERMS, MSG_POSTPERM, perm_tbl);
    move(2, 0);
    clrtobot();
    i = 1;
  }

  if (vget(++i, 0, "�]�w�ݩ�(Y/N)�H[N] ", data, 3, LCECHO) == 'y')
  {
#define NUMATTRS        7       /* lkchu.981201: �W�[�ݩʧO�ѤF�� */

    static char *battrs[NUMATTRS] = {
      "���i Zap",
      "����H",
      "���O���g��",
      "�����������D�έp",
      "�����}���|���G",
      "�ΦW�ݪO",
      "���i��H�峹"            /* lkchu.981201 */
      
    };

    brd->battr = bitset(brd->battr, NUMATTRS, NUMATTRS, MSG_BRDATTR, battrs);
  }
  return 0;
}


int
m_newbrd()
{
  BRD newboard;
  int bno;
  char fpath[80];
  HDR hdr;

  vs_bar("�إ߷s�O");
  memset(&newboard, 0, sizeof(newboard));
  if (m_setbrd(&newboard))
    return -1;

  if (vans(msg_sure_ny) != 'y')
    return 0;

  time(&newboard.bstamp);
  if ((bno = brd_bno("")) >= 0)
  {
    rec_put(FN_BRD, &newboard, sizeof(newboard), bno);
  }
  /* Thor.981102: ����W�Lshm�ݪO�Ӽ� */
  else if ( bshm->number >= MAXBOARD)
  {
    vmsg("�W�L�t�Ωү�e�Ǭݪ��ӼơA�нվ�t�ΰѼ�");
    return -1;
  }
  else if (rec_add(FN_BRD, &newboard, sizeof(newboard)) < 0)
  {
    vmsg("�L�k�إ߷s�O");
    return -1;
  }

  sprintf(fpath, "gem/brd/%s", newboard.brdname);
  mak_dirs(fpath);
  mak_dirs(fpath + 4);

  bshm->uptime = 0;		/* force reload of bcache */
  bshm_init();

  /* ���K�[�i NewBoard */

  if (vans("�O�_�[�J [NewBoard] �s��(Y/N)?[Y] ") != 'n')
  {
    brd2gem(&newboard, &hdr);
    rec_add("gem/@/@NewBoard", &hdr, sizeof(HDR));
  }

  vmsg("�s�O����");
  return 0;
}


void
brd_edit(bno)
  int bno;
{
  BRD *bhdr, newbh;
  char buf[80];

  vs_bar("�ݪO�]�w");
  bhdr = bshm->bcache + bno;
  memcpy(&newbh, bhdr, sizeof(BRD));
  prints("�ݪO�W�١G%s\n�ݪO�����G%s\n�O�D�W��G%s\n",
    newbh.brdname, newbh.title, newbh.BM);

  bitmsg(MSG_READPERM, STR_PERM, newbh.readlevel);
  bitmsg(MSG_POSTPERM, STR_PERM, newbh.postlevel);
  bitmsg(MSG_BRDATTR, "zTcs", newbh.battr);

  switch (vget(8, 0, "(D)�R�� (E)�]�w (Q)�����H[Q] ", buf, 3, LCECHO))
  {
  case 'd':

    if (vget(9, 0, msg_sure_ny, buf, 3, LCECHO) != 'y')
    {
      vmsg(MSG_DEL_CANCEL);
    }
    else
    {
      char *bname = bhdr->brdname;
      sprintf(buf, "gem/brd/%s", bname);
      f_rm(buf);
      f_rm(buf + 4);
      memset(&newbh, 0, sizeof(newbh));
      sprintf(newbh.title, "[%s] deleted by %s", bname, cuser.userid);
      memcpy(bhdr, &newbh, sizeof(BRD));
      rec_put(FN_BRD, &newbh, sizeof(newbh), bno);
      blog("Admin", newbh.title);
      vmsg("�R�O����");
    }
    break;

  case 'e':

    move(9, 0);
    outs("������ [Return] ���ק�Ӷ��]�w");

    if (!m_setbrd(&newbh))
    {
      if ((vans(msg_sure_ny) == 'y') &&
	memcmp(&newbh, bhdr, sizeof(newbh)))
      {
	if (strcmp(bhdr->brdname, newbh.brdname))
	{
	  char src[80], dst[80];
          /* Thor.980806: �S�O�`�N�p�G board���b�P�@partition�ت��ܷ|�����D */
	  sprintf(src, "gem/brd/%s", bhdr->brdname);
	  sprintf(dst, "gem/brd/%s", newbh.brdname);
	  rename(src, dst);
	  rename(src + 4, dst + 4);
	}
	memcpy(bhdr, &newbh, sizeof(BRD));
	rec_put(FN_BRD, &newbh, sizeof(BRD), bno);
      }
    }
    vmsg("�]�w����");
    break;
  }
}


#ifdef	HAVE_REGISTER_FORM
/* ----------------------------------------------------- */
/* �ϥΪ̶�g���U���					 */
/* ----------------------------------------------------- */


static void
getfield(line, len, buf, desc, hint)
  int line, len;
  char *hint, *desc, *buf;
{
  move(line, 0);
  prints("%s�G%s", desc, hint);
  vget(line + 1, 10, NULL, buf, len, GCARRY);
}


int
u_register()
{
  FILE *fn;
  int ans;
  RFORM rform;

  if (cuser.userlevel & PERM_VALID)
  {
    vmsg("�z�������T�{�w�g�����A���ݶ�g�ӽЪ�"); /* lkchu.981201: �� vmsg ���� */
    return XEASY;
  }

  if (fn = fopen(fn_rform, "rb"))
  {
    while (fread(&rform, sizeof(RFORM), 1, fn))
    {
      if ((rform.userno == cuser.userno) &&
	!str_cmp(rform.userid, cuser.userid))
      {
	fclose(fn);
	vmsg("�z�����U�ӽг�|�b�B�z���A�Э@�ߵ���"); /* lkchu.981201: �� vmsg ���� :p */
	return XEASY;
      }
    }
    fclose(fn);
  }

  if (vans("�z�T�w�n��g���U���(Y/N)�H[N] ") != 'y')
    return XEASY;

  move(2, 0);
  clrtobot();
  prints("%s(%s) �z�n�A�оڹ��g�H�U����ơG\n(�� [Enter] ������l�]�w)",
    cuser.userid, cuser.username);

  memset(&rform, 0, sizeof(RFORM));
  strcpy(rform.realname, cuser.realname);
  strcpy(rform.address, cuser.address);
  rform.career[0] = rform.phone[0] = '\0';
  for (;;)
  {
    getfield(5, 20, rform.realname, "�u��m�W", "�ХΤ���");
    getfield(8, 50, rform.career, "�A�ȳ��", "�Ǯըt�ũγ��¾��");
    getfield(11, 60, rform.address, "�ثe��}", "�]�A��ǩΪ��P���X");
    getfield(14, 20, rform.phone, "�s���q��", "�]�A���~�����ϰ�X");
    ans = vans("�H�W��ƬO�_���T(Y/N)�H(Q)���� [N] ");
    if (ans == 'q')
      return 0;
    if (ans == 'y')
      break;
  }
  strcpy(cuser.realname, rform.realname);
  strcpy(cuser.address, rform.address);

  rform.userno = cuser.userno;
  strcpy(rform.userid, cuser.userid);
  (void) time(&rform.rtime);
  rec_add(fn_rform, &rform, sizeof(RFORM));
  return 0;
}


/* ----------------------------------------------------- */
/* �B�z Register Form					 */
/* ----------------------------------------------------- */


static int
scan_register_form(fd)
  int fd;
{
  static char logfile[] = FN_RFORM_LOG;
  static char *reason[] = {"��J�u��m�W", "�Թ��g�ӽЪ�",
    "�Զ��}���", "�Զ�s���q��", "�Զ�A�ȳ��B�ξǮըt��",
  "�Τ����g�ӽг�", "�ĥ� E-mail �{��", NULL};

  ACCT muser;
  RFORM rform;
  HDR fhdr;
  FILE *fout;

  int op, n;
  char buf[128], msg[128], *agent, *userid, *str;

  vs_bar("�f�֨ϥΪ̵��U���");
  agent = cuser.userid;

  while (read(fd, &rform, sizeof(RFORM)) == sizeof(RFORM))
  {
    userid = rform.userid;
    move(2, 0);
    prints("�ӽХN��: %s (�ӽЮɶ��G%s)\n", userid, Ctime(&rform.rtime));
    prints("�u��m�W: %s\n", rform.realname);
    prints("�A�ȳ��: %s\n", rform.career);
    prints("�ثe��}: %s\n", rform.address);
    prints("�s���q��: %s\n%s\n", rform.phone, msg_seperator);
    clrtobot();

    if ((acct_load(&muser, userid) < 0) || (muser.userno != rform.userno))
    {
      vmsg("�d�L���H");
      op = 'd';
    }
    else
    {
      acct_show(&muser, 2);
      if (muser.userlevel & PERM_VALID)
      {
	vmsg("���b���w�g�������U");
	op = 'd';
      }
      else
      {
	op = vans("�O�_����(Y/N/Q/Del/Skip)�H[S] ");
      }
    }

    switch (op)
    {
    case 'y':

      muser.userlevel |= PERM_VALID;
      strcpy(muser.realname, rform.realname);
      strcpy(muser.address, rform.address);
      sprintf(msg, "%s:%s:%s", rform.phone, rform.career, agent);
      /* str_ncpy(muser.justify, msg, 59); */
      str_ncpy(muser.justify, msg, sizeof(muser.justify));
      /* Thor.980921: �O�I�_�� */

      /* Thor.981022: ��ʻ{�Ҥ]��{�Үɶ�, �C�b�~�|�A�۰ʻ{�Ҥ@��*/
      time(&muser.tvalid);

      acct_save(&muser);

      usr_fpath(buf, userid, "justify");
      if (fout = fopen(buf, "w"))
      {
	fprintf(fout, "%s\n", msg);
	fclose(fout);
      }

      usr_fpath(buf, userid, fn_dir);
      /* lkchu.981201: �令 etc/justified �P bmtad & mailpost �ۦP */
      hdr_stamp(buf, HDR_LINK, &fhdr, "etc/justified" /* "etc/approved" */);
      strcpy(fhdr.title, "[���U���\\] �z�w�g�q�L�����{�ҤF�I");
      strcpy(fhdr.owner, cuser.userid);
      rec_add(buf, &fhdr, sizeof(fhdr));

      strcpy(rform.agent, agent);
      rec_add(logfile, &rform, sizeof(RFORM));

      break;

    case 'q':			/* �Ӳ֤F�A������ */

      do
      {
	rec_add(fn_rform, &rform, sizeof(RFORM));
      } while (read(fd, &rform, sizeof(RFORM)) == sizeof(RFORM));

    case 'd':
      break;

    case 'n':

      move(9, 0);
      prints("�д��X�h�^�ӽЪ��]�A�� <enter> ����\n\n");
      for (n = 0; str = reason[n]; n++)
	prints("%d) ��%s\n", n, str);
      clrtobot();

      if (op = vget(b_lines, 0, "�h�^��]�G", buf, 60, DOECHO))
      {
	int i;
	char folder[80], fpath[80];
	HDR fhdr;

	i = op - '0';
	if (i >= 0 && i < n)
	  strcpy(buf, reason[i]);

	usr_fpath(folder, muser.userid, fn_dir);
	if (fout = fdopen(hdr_stamp(folder, 0, &fhdr, fpath), "w"))
	{
	  fprintf(fout, "\t�ѩ�z���Ѫ���Ƥ����Թ�A�L�k�T�{�����A"
	    "\n\n\t�Э��s��g���U���G%s�C\n", buf);
	  fclose(fout);

	  strcpy(fhdr.owner, agent);
	  strcpy(fhdr.title, "[�N�ߨϪ�] �бz���s��g���U���");
	  rec_add(folder, &fhdr, sizeof(fhdr));
	}

	strcpy(rform.reply, buf);	/* �z�� */
	strcpy(rform.agent, agent);
	rec_add(logfile, &rform, sizeof(RFORM));

	break;
      }

    default:			/* put back to regfile */

      rec_add(fn_rform, &rform, sizeof(RFORM));
    }
  }
}


int
m_register()
{
  int num;
  char buf[80];

  num = rec_num(fn_rform, sizeof(RFORM));
  if (num <= 0)
  {
    vmsg("�ثe�õL�s���U���"); /* lkchu.981201: �� vmsg ���� :p */
    return XEASY;
  }

  sprintf(buf, "�@�� %d ����ơA�}�l�f�ֶ�(Y/N)�H[N] ", num);
  num = XEASY;

  if (vans(buf) == 'y')
  {
    sprintf(buf, "%s.tmp", fn_rform);
    if (dashf(buf))
    {
      vmsg("��L SYSOP �]�b�f�ֵ��U�ӽг�");
    }
    else
    {
      int fd;

      rename(fn_rform, buf);
      fd = open(buf, O_RDONLY);
      if (fd >= 0)
      {
	scan_register_form(fd);
	close(fd);
	unlink(buf);
	num = 0;
      }
      else
      {
	vmsg("�L�k�}�ҵ��U��Ƥu�@��");
      }
    }
  }
  return num;
}
#endif


/* ----------------------------------------------------- */
/* ���Ͱl�ܰO���G��ĳ��� log_usies()�BTRACE()		 */
/* ----------------------------------------------------- */


#ifdef	HAVE_REPORT
void
report(s)
  char *s;
{
  static int disable = NA;
  int fd;

  if (disable)
    return;

  if ((fd = open("trace", O_WRONLY, 0600)) != -1)
  {
    char buf[256];
    char *thetime;
    time_t dtime;

    time(&dtime);
    thetime = Etime(&dtime);

    /* flock(fd, LOCK_EX); */
    /* Thor.981205: �� fcntl ���Nflock, POSIX�зǥΪk */
    f_exlock(fd);

    lseek(fd, 0, L_XTND);
    sprintf(buf, "%s %s %s\n", cuser.userid, thetime, s);
    write(fd, buf, strlen(buf));

    /* flock(fd, LOCK_UN); */
    /* Thor.981205: �� fcntl ���Nflock, POSIX�зǥΪk */
    f_unlock(fd);

    close(fd);
  }
  else
    disable = YEA;
}


int
m_trace()
{
  struct stat bstatb, ostatb, cstatb;
  int btflag, otflag, ctflag, done = 0;
  char ans[2];
  char *msg;

  clear();
  move(0, 0);
  outs("Set Trace Options");
  clrtobot();
  while (!done)
  {
    move(2, 0);
    otflag = stat("trace", &ostatb);
    ctflag = stat("trace.chatd", &cstatb);
    btflag = stat("trace.bvote", &bstatb);
    outs("Current Trace Settings:\n");
    if (otflag)
      outs("Normal tracing is OFF\n");
    else
      prints("Normal tracing is ON (size = %d)\n", ostatb.st_size);
    if (ctflag)
      outs("Chatd  tracing is OFF\n");
    else
      prints("Chatd  tracing is ON (size = %d)\n", cstatb.st_size);
    if (btflag)
      outs("BVote  tracing is OFF\n");
    else
      prints("BVote  tracing is ON (size = %d)\n", bstatb.st_size);

    move(8, 0);
    outs("Enter:\n");
    prints("<1> to %s Normal tracing\n", otflag ? "enable " : "disable");
    prints("<2> to %s Chatd  tracing\n", ctflag ? "enable " : "disable");
    prints("<3> to %s BVote  tracing\n", btflag ? "enable " : "disable");
    vget(12, 0, "Anything else to exit�G", ans, 2, DOECHO);

    switch (ans[0])
    {
    case '1':
      if (otflag)
      {
	system("/bin/touch trace");
	msg = "BBS   tracing enabled.";
	report("opened report log");
      }
      else
      {
	report("closed report log");
	system("/bin/mv trace trace.old");
	msg = "BBS   tracing disabled; log is in trace.old";
      }
      break;

    case '2':
      if (ctflag)
      {
	system("/bin/touch trace.chatd");
	msg = "Chat  tracing enabled.";
	report("chatd trace log opened");
      }
      else
      {
	system("/bin/mv trace.chatd trace.chatd.old");
	msg = "Chat  tracing disabled; log is in trace.chatd.old";
	report("chatd trace log closed");
      }
      break;

    case '3':
      if (btflag)
      {
	system("/bin/touch trace.bvote");
	msg = "BVote tracing enabled.";
	report("BVote trace log opened");
      }
      else
      {
	system("/bin/mv trace.bvote trace.bvote.old");
	msg = "BVote tracing disabled; log is in trace.bvote.old";
	report("BoardVote trace log closed");
      }
      break;

    default:
      msg = NULL;
      done = 1;
    }
    move(b_lines - 1, 0);
    if (msg)
      prints("%s\n", msg);
  }
  clear();
}
#endif				/* HAVE_REPORT */
