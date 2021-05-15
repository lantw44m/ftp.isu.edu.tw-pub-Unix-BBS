/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: main.c,v 1.26 2002/09/13 01:50:34 edwardc Exp $
*/

#include "bbs.h"

#define DOTLOGINSBAD    "logins.bad"
#define BADLOGINFILE    "log/logins.bad"
int     ERROR_READ_SYSTEM_FILE = NA;
int     RMSG = YEA;
int     msg_num = 0;
int     nettyNN = 0;
int     count_friends = 0, count_users = 0;
int     iscolor = 1;
int     mailXX = 0;
char   *getenv();
int     friend_login_wall();
char   *sysconf_str();
char   *Ctime();
struct user_info *t_search();
void    r_msg();
void    count_msg();
void    c_recover();
void    tlog_recover();
int     listmode;
int     numofsig = 0;
jmp_buf byebye;
extern struct BCACHE *brdshm;
FILE   *ufp;
int     talkrequest = NA;
/* int ntalkrequest = NA ; */
int     enter_uflags;
time_t  lastnote;
extern int dumb_term;
int	ScrFlag = NA;
int	ISLOGIN = 1;

struct user_info uinfo;

char    netty_path[256];

#ifndef BBSD
char    tty_name[20];
#endif
char	fromhost[60];

char    BoardName[STRLEN];
char    ULIST[STRLEN];
int     utmpent = -1;
time_t  login_start_time;
int     showansi = 1;

void
log_usies(mode, mesg)
char   *mode, *mesg;
{
	time_t  now;
	char    buf[256], *fmt;
	now = time(0);
	fmt = currentuser.userid[0] ? "%s %s %-12s %s\n" : "%s %s %s%s\n";
	sprintf(buf, fmt, Ctime(&now), mode, currentuser.userid, mesg);
	file_append("usies", buf);
}

void
u_enter()
{
	char	buf[80], host[60], fhost[60];
	int	i, space;
	FILE	*fp;
	
	enter_uflags = currentuser.flags[0];
	memset(&uinfo, 0, sizeof(uinfo));
	uinfo.active = YEA;
	uinfo.pid = getpid();
	if (HAS_PERM(PERM_LOGINCLOAK) && (currentuser.flags[0] & CLOAK_FLAG))
		uinfo.invisible = YEA;
	uinfo.mode = LOGIN;
#ifdef BBSD
	uinfo.idle_time = time(0);
#endif

	if (strcmp(currentuser.userid, "guest")) {
		if (DEFINE(DEF_FRIENDCALL)) {
			uinfo.pager |= FRIEND_PAGER;
		}
		if (currentuser.flags[0] & PAGER_FLAG) {
			uinfo.pager |= ALL_PAGER;
			uinfo.pager |= FRIEND_PAGER;
		}

		if (DEFINE(DEF_FRIENDMSG)) {
			uinfo.pager |= FRIENDMSG_PAGER;
		}
		if (DEFINE(DEF_ALLMSG)) {
			uinfo.pager |= ALLMSG_PAGER;
			uinfo.pager |= FRIENDMSG_PAGER;
		}
	}
	
	uinfo.uid = usernum;
	strncpy(uinfo.from, fromhost, 60);

#ifdef FROM_TRANSLATE

	/* chinsan.20011231: ����W���ӷ���� port from bbs.ilc.edu.tw */
	strncpy(fhost, fromhost, 60);
	i = 0;
	while (i < strlen(fhost)) {
		fhost[i]= tolower(fhost[i]);
		i++;
	}
	i = 0;
	if ( (fp = fopen("etc/hosts","r"))!= NULL ) {
		while ( fgets(buf, 80, fp) ) {
			space = 0;
			while (buf[space] != ' ' && space < strlen(buf)) {
				space++;
			}
			memset(host, 0 , 60);
			strncpy(host, buf, space);
			/* chinsan.20010112: convert host to lower case (by kllau) */
			while (i < strlen(host)) {
				host[i] = tolower(host[i]);
				i++;
			}
			i = 0;
			if (strstr(fhost,host)) {
				while (buf[space] == ' ' && space < strlen(buf)) {
					space++;
				}
				memset(uinfo.from, 0, 60);
				strncpy(uinfo.from, buf+space, strlen(buf) - space -1 );
				break;
			}
		}
		fclose(fp);
	}

#endif

#if !defined(BBSD) && defined(SHOW_IDLE_TIME)
	strncpy(uinfo.tty, tty_name, 20);
#endif	
	iscolor = (DEFINE(DEF_COLOR)) ? 1 : 0;
	strncpy(uinfo.userid, currentuser.userid, 20);
	strncpy(uinfo.realname, currentuser.realname, 20);
	strncpy(uinfo.username, currentuser.username, 40);
	getfriendstr();
	getrejectstr();
	if (HAS_PERM(PERM_EXT_IDLE))
		uinfo.ext_idle = YEA;
		
	listmode = 0;	/* �ɥΤ@�U, �ΨӬ����쩳 utmpent �d�쥢�ѴX�� */
	
	while ( 1 ) {
		utmpent = getnewutmpent(&uinfo);
		if ( utmpent >= 0 || utmpent == -1 )
			break;
		if ( utmpent == -2 && listmode <= 100 ) {
			listmode++;
			usleep(250);		/* �𮧥|�����@��A���b�y */
			continue;	
		}
		if ( listmode > 100 ) {	/* ���a */
			sprintf(genbuf, "getnewutmpent(): too much times, give up.");
			report(genbuf);
			prints("getnewutmpent(): ���ѤӦh��, ���. �Ц^������.\n");
			sleep(3);
			exit(0);
		}
	}
	
	if (utmpent < 0) {
		sprintf(genbuf, "Fault: No utmpent slot for %s\n", uinfo.userid);
		report(genbuf);
	}
	listmode = 0;
	digestmode = NA;
}

void
setflags(mask, value)
int     mask, value;
{
	if (((currentuser.flags[0] & mask) && 1) != value) {
		if (value)
			currentuser.flags[0] |= mask;
		else
			currentuser.flags[0] &= ~mask;
	}
}

void
u_exit()
{
	setflags(PAGER_FLAG, (uinfo.pager & ALL_PAGER));
	if (HAS_PERM(PERM_LOGINCLOAK))
		setflags(CLOAK_FLAG, uinfo.invisible);

	if (currentuser.flags[0] != enter_uflags && !ERROR_READ_SYSTEM_FILE) {
		set_safe_record();
		substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	}
	uinfo.active = NA;
	uinfo.pid = 0;
	uinfo.invisible = YEA;
	uinfo.sockactive = NA;
	uinfo.sockaddr = 0;
	uinfo.destuid = 0;
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)
	strcpy(uinfo.tty, "NoTTY");
#endif
	update_utmp();
}

int
cmpuids(uid, up)
char   *uid;
struct userec *up;
{
	return !ci_strncmp(uid, up->userid, sizeof(up->userid));
}

int
dosearchuser(userid)
char   *userid;
{
	int     id;
	if ((id = getuser(userid)) != 0) {
		if (cmpuids(userid, &lookupuser)) {
			memcpy(&currentuser, &lookupuser, sizeof(currentuser));
			return usernum = id;
		}
	}
	memset(&currentuser, 0, sizeof(currentuser));
	return usernum = 0;
}

int     started = 0;

void
talk_request()
{
	signal(SIGUSR1, talk_request);
	talkrequest = YEA;
	bell();
	bell();
	bell();
	sleep(1);
	bell();
	bell();
	bell();
	bell();
	bell();
	return;
}

void
abort_bbs()
{
	time_t  stay;


	if (uinfo.mode == POSTING || uinfo.mode == SMAIL || uinfo.mode == EDIT
		|| uinfo.mode == EDITUFILE || uinfo.mode == EDITSFILE || uinfo.mode == EDITANN)
		keep_fail_post();
	if (started) {
		stay = time(0) - login_start_time;
		sprintf(genbuf, "Stay: %3ld (%s)", stay / 60, currentuser.username);
		log_usies("AXXED", genbuf);
		u_exit();
	}
	shm_deatch();
	exit(0);
}

int
cmpuids2(unum, urec)
int     unum;
struct user_info *urec;
{
	return (unum == urec->uid);
}

int
count_multi(uentp)
struct user_info *uentp;
{
	static int count;
	if (uentp == NULL) {
		int     num = count;
		count = 0;
		return num;
	}
	if (!uentp->active || !uentp->pid)
		return 0;
	if (uentp->uid == usernum)
		count++;
	return 1;
}

int
count_user()
{
	count_multi(NULL);
	apply_ulist(count_multi);
	return count_multi(NULL);
}

void
multi_user_check()
{
	struct user_info uin;
	char    buffer[40];
	int     logins;
	

	if ( HAS_PERM(PERM_SYSOP) && !trusted_host(fromhost) ) {
		sprintf(genbuf,"�������\\���D�� %s �ϥ� SYSOP �v���b��",fromhost);
		securityreport(genbuf);
		prints("�ܩ�p!! �֦������v�����H�бq�Q���\\���D���s�u......\n");
		pressreturn();
		oflush();
		exit(1);
	}
                                        
	if (HAS_PERM(PERM_MULTILOG))
		return;		/* don't check sysops */

	/* allow multiple guest user */
	logins = count_user();

	if (heavyload() && logins) {
		prints("[1;33m��p, �ثe�t�έt���L��, �Фŭ��� Login�C[m\n");
		oflush();
		sleep(3);
		exit(1);
	}
	if (!strcasecmp("guest", currentuser.userid)) {
		if (logins > MAXGUEST) {
			prints("[1;33m��p, �ثe�w���Ӧh [1;36mguest[33m, �еy��A�աC[m\n");
			oflush();
			sleep(3);
			exit(1);
		}
		return;
	} else if (logins >= MULTI_LOGINS) {
		prints("[1;33m�ܩ�p, �z�w Login �ۦP�b�� %d ��, ���T�O�L�H�W���v�q,\n ���s�u�N�Q�����C[m\n", MULTI_LOGINS);
		oflush();
		sleep(3);
		exit(1);
	}
	if (!search_ulist(&uin, cmpuids2, usernum))
		return;		/* user isn't logged in */

	if (!uin.active || (kill(uin.pid, 0) == -1))
		return;		/* stale entry in utmp file */
	if (askyn("[1;37m�z�Q�R�����ƪ� login ��", NA, YEA) == YEA) {
		kill(uin.pid, 9);
		sprintf(buffer, "kicked (multi-login)");
		report(buffer);
		log_usies("KICK ", currentuser.username);
	}
}

int
simplepasswd(str, check)
char   *str;
int     check;
{
	char    ch;
	while ((ch = *str++) != '\0') {
		if (check == 1) {
			if (!(ch >= 'a' && ch <= 'z'))
				return 0;
		} else if (!(ch >= '0' && ch <= '9'))
			return 0;
	}
	return 1;
}
#ifndef BBSD
void
system_init(argc, argv)
int     argc;
char  **argv;
#else
void
system_init()
#endif
{
#ifndef BBSD
	char   *rhost;
#endif

	gethostname(genbuf, 256);
#ifdef SINGLE
	if (strcmp(genbuf, SINGLE)) {
		printf("Not on a valid machine!\n");
		exit(-1);
	}
#endif
	sprintf(ULIST, "%s.%s", ULIST_BASE, genbuf);

#ifndef BBSD
	if (argc >= 3) {
		strncpy(fromhost, argv[2], 60);
	} else {
		fromhost[0] = '\0';
	}
	if ((rhost = getenv("REMOTEHOST")) != NULL)
		strncpy(fromhost, rhost, 60);
	fromhost[59] = '\0';
#if defined(SHOW_IDLE_TIME)
	if (argc >= 4) {
		strncpy(tty_name, argv[3], 20);
	} else {
		tty_name[0] = '\0';
	}
#endif
#endif

#ifdef DOTIMEOUT
	init_alarm();
	uinfo.mode = LOGIN;
	alarm(LOGIN_TIMEOUT);
#else
	signal(SIGALRM, SIG_SIG);
#endif

	signal(SIGHUP, abort_bbs);
	signal(SIGBUS, abort_bbs);
	signal(SIGSEGV, abort_bbs);
	signal(SIGTERM, abort_bbs);

	signal(SIGPIPE, abort_bbs);
	signal(SIGTTOU, count_msg);
	signal(SIGUSR1, talk_request);
	signal(SIGUSR2, r_msg);
}

void
system_abort()
{
	if (started) {
		log_usies("ABORT", currentuser.username);
		u_exit();
	}
	clear();
	refresh();
	printf("���¥��{, �O�o�`�ӳ� !\n");
	exit(0);
}

void
logattempt(uid, frm)
char   *uid, *frm;
{
	char    fname[STRLEN];
	
	login_start_time = time(0);
	sprintf(genbuf, "%-12.12s  %-30s %s\n",
		uid, Ctime(&login_start_time), frm);
	file_append(BADLOGINFILE, genbuf);
	sethomefile(fname, uid, DOTLOGINSBAD);
	file_append(fname, genbuf);
}

void
login_query()
{
	char    uid[IDLEN + 2], passbuf[PASSLEN];
	int     curr_login_num;
	int     attempts;
#ifdef MAGIC_PASS
	int     magic;
	char    buf[PASSLEN];
#endif
	char    genbuf[STRLEN];
#ifndef BBSNAME
	char   *ptr;
#endif
	extern struct UTMPFILE *utmpshm;


	dumb_term = YEA;
	
	curr_login_num = num_active_users();
	if (curr_login_num >= MAXACTIVE) {
		ansimore("etc/loginfull", NA);
		oflush();
		sleep(1);
		exit(1);
	}
#ifdef BBSNAME
	strcpy(BoardName, BBSNAME);
	/* edwardc.990508 copy from defined variable */
#else
	ptr = sysconf_str("BBSNAME");
	if (ptr == NULL)
		ptr = "�|���R�W���կ�";
	strcpy(BoardName, ptr);
#endif

	if (fill_shmfile(1, "etc/issue", "ISSUE_SHMKEY")) {
		show_issue();	/* is anibanner ready, remark this and put
				 * \n\n */
	}
	prints("[1;35m�w����{[1;40;33m�i %s �j[0;1;32m�C �����@�@�i�H�� [1;36m%d[0;1;32m �H���U�ϥΡC[m\n",
		BoardName, MAXUSERS);
	resolve_utmp();
	if (utmpshm->usersum == 0)
		utmpshm->usersum = allusers();
	if (utmpshm->max_login_num < curr_login_num)
		utmpshm->max_login_num = curr_login_num;

#ifndef VERSION_ID
	prints("[1;36m�ثe�W���H��:[1;40;37m [%d/%d] [0;1;32m�C �̰��H�ưO��: [[1;36m%d[1;32m]�C �ثe�w�� [1;36m%d[32m �ӵ��U�b���C[m\n",
		curr_login_num, MAXACTIVE, utmpshm->max_login_num, utmpshm->usersum);
#else
	prints("[1;36m�ثe�W���H��:[1;40;37m [%d/%d] [0;1;32m�C �ثe�w�� [1;36m%d[32m �ӵ��U�b���C �e%s�f[m\n",
		curr_login_num, MAXACTIVE, utmpshm->usersum, VERSION_ID);
#endif

#ifdef MUDCHECK_BEFORELOGIN

	prints("[1;33m������ϥε{���W���A�Ы� [1;36mCTRL + C[m : ");
	
	genbuf[0] = igetkey();
	
	if ( genbuf[0] != Ctrl('C') ) {
		prints("\n�藍�_�A�A�èS�����U CTRL+C ��I\n");
		oflush();
		exit(1);
	} else {
		prints("[CTRL] + [C]\n");
	}

#endif
	
	attempts = 0;
	while (1) {
		if (attempts++ >= LOGINATTEMPTS) {
			ansimore("etc/goodbye", NA);
			oflush();
			sleep(1);
			exit(1);
		}
		getdata(0, 0, "[1;33m�п�J�b��[m(�եνп�J `[1;36mguest[m', ���U�п�J`[1;31mnew[m'): ",
			uid, IDLEN + 1, DOECHO, YEA);
#ifdef Quick_LOGIN
		if( uid[0] == 0 )
			strcpy(uid, "guest");
#endif			
		/* ppfoong */
		if ((strcasecmp(uid, "guest") == 0) && (MAXACTIVE - curr_login_num < 10)) {
			ansimore("etc/loginfull", NA);
			oflush();
			sleep(1);
			exit(1);
		}
		if (strcasecmp(uid, "new") == 0) {
#ifdef LOGINASNEW
			memset(&currentuser, 0, sizeof(currentuser));
			new_register();
			ansimore3("etc/firstlogin", YEA);
			break;
#else
			prints("[1;37m���t�Υثe�L�k�H [36mnew[37m ���U, �Х�[36m guest[37m �i�J...[m\n");
#endif
		} else if (*uid == '\0' || !dosearchuser(uid)) {
			prints("[1;31m�u�r�A%s�S���o�ӤH�C...[m\n\n", BoardName);
		} else if (strcasecmp(uid, "guest") == 0) {
			currentuser.userlevel = 0;
			currentuser.flags[0] = CURSOR_FLAG;
#ifdef	_FreeBSD_
			setproctitle("user: guest (%s)", fromhost);
#endif
			break;
		} else {
			getdata(0, 0, "[1;37m�п�J�K�X: [m", passbuf, PASSLEN, NOECHO, YEA);
			passbuf[8] = '\0';
			if (!checkpasswd(currentuser.passwd, passbuf)) {
				logattempt(currentuser.userid, fromhost);
				prints("[1;31m���r�A�z����K�X���ѤF...[m\n\n");
			} else {
/* ���v�]�v���� guest �@�ˡ^�̵M�i�i�J modify by skyo 19990602*/
				if (simplepasswd(passbuf, 1) || simplepasswd(passbuf, 2)
					|| strstr(passbuf, currentuser.userid)) {
					prints("[1;33m* �K�X�L��²��, �п�ܤ@�ӥH�W���S��r��.[m\n");
					getdata(0, 0, "�� <ENTER> �~��", genbuf, 5, NOECHO, YEA);
				}
#ifdef REFUSE_LESS60SEC
				/* edwardc.990420 committe by Harimau */
				if (time(0) - currentuser.lastlogin < 60 &&
					!HAS_PERM(PERM_SYSOP) &&
					strcasecmp(currentuser.userid, "guest") != 0) {
					prints("���F������`���W�[�t�έt��, �Щ�@������A�W��.\n");
					prints("�Y���ðݽгq�����ȤH��, ����.\n");
					oflush();
					sleep(3);
					exit(1);
				}
#endif

#ifdef MAGIC_PASS
				if (HAS_PERM(PERM_SYSOP)) {
					randomize();
					magic = rand() % 100;
					prints("�S��b��ñ�J�f�O: %d\n", magic * 4);
					getdata(4, 0, "�^���f�O : ", buf, PASSLEN, NOECHO, YEA);
					if (*buf == '\0' || !(atoi(buf) == magic)) {
						sprintf(genbuf, "ñ�J�f�O���~ (%s)", fromhost);
						securityreport(genbuf);
						prints("�f�O���~, ���oñ�J ! !\n");
						oflush();
						sleep(2);
						exit(1);
					}
				}
#endif
				bzero(passbuf, PASSLEN - 1);
#ifdef	_FreeBSD_
				/* edwardc.010831 ���T�w�n���n��o��U�� */
				setproctitle("user: %s", currentuser.userid);
#endif
				break;
			}
		}
	}
	ISLOGIN = 0;
	multi_user_check();

#if 1
   if (!term_init(currentuser.termtype)) {
		prints("Bad terminal type.  Defaulting to 'vt100'\n");
		strcpy(currentuser.termtype, "vt100");
		term_init(currentuser.termtype);
	}
#endif

	sethomepath(genbuf, currentuser.userid);
	mkdir(genbuf, 0755);
	dumb_term = NA;
}

int
valid_ident(ident)
char   *ident;
{
	static char *invalid[] = {"unknown@", "root@", "gopher@", "bbs@",
	"bbsadm@", "guest@", "nobody@", "www@", "?@", NULL};
	int     i;
	if (ident[0] == '@')
		return 0;
	for (i = 0; invalid[i] != NULL; i++)
		if (strstr(ident, invalid[i]) != NULL)
			return 0;
	return 1;
}

void
write_defnotepad()
{
	currentuser.notedate = time(NULL);
	set_safe_record();
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	return;
}

void
notepad_init()
{
	FILE   *check;
	char    notetitle[STRLEN];
	char    tmp[STRLEN * 2];
	char   *fname, *bname, *ntitle;
	long int maxsec;
	time_t  now;
	maxsec = 86400;
	lastnote = 0;
	if ((check = fopen("etc/checknotepad", "r")) != NULL) {
		fgets(tmp, sizeof(tmp), check);
		lastnote = atol(tmp);
		fclose(check);
	} else
		lastnote = 0;
	if (lastnote == 0) {
		lastnote = time(NULL) - (time(NULL) % maxsec);
		check = fopen("etc/checknotepad", "w");
		fprintf(check, "%d", lastnote);
		fclose(check);
		sprintf(tmp, "�d���O�b %s Login �}�ҡA���w�}�Үɶ��� %s"
			,currentuser.userid, Ctime(&lastnote));
		report(tmp);
	}
	if ((time(NULL) - lastnote) >= maxsec) {
		move(t_lines - 1, 0);
		prints("�藍�_�A�t�Φ۰ʵo�H�A�еy��.....");
		refresh();
		now = time(0);
		check = fopen("etc/checknotepad", "w");
		lastnote = time(NULL) - (time(NULL) % maxsec);
		fprintf(check, "%d", lastnote);
		fclose(check);
		if ((check = fopen("etc/autopost", "r")) != NULL) {
			while (fgets(tmp, STRLEN, check) != NULL) {
				fname = strtok(tmp, " \n\t:@");
				bname = strtok(NULL, " \n\t:@");
				ntitle = strtok(NULL, " \n\t:@");
				if (fname == NULL || bname == NULL || ntitle == NULL)
					continue;
				else {
					sprintf(notetitle, "%.10s�n%s", Ctime(&now) + 11, ntitle);
					if (dashf(fname)) {
						postfile(fname, bname, notetitle, 1);
						sprintf(tmp, "%s �۰ʱi�K", ntitle);
						report(tmp);
					}
				}
			}
			fclose(check);
		}
		sprintf(notetitle, "%.15s �d���O�O��", Ctime(&now));
		if (dashf("etc/notepad", "r")) {
			postfile("etc/notepad", "notepad", notetitle, 1);
			unlink("etc/notepad");
		}
		report("�۰ʵo�H�ɶ����");
	}
	return;
}


void
user_login()
{
	char    fname[STRLEN];
	char   *ruser;

	if (strcmp(currentuser.userid, "SYSOP") == 0) {
		currentuser.userlevel = ~0;	/* SYSOP gets all permission
						 * bits */
		substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	}
	ruser = getenv("REMOTEUSER");
	sprintf(genbuf, "%s@%s", ruser ? ruser : "?", fromhost);
	log_usies("ENTER", genbuf);
	u_enter();
	if (ruser != NULL) {
		sprintf(genbuf, "%s@%s", ruser, fromhost);
		if (valid_ident(genbuf)) {
			strncpy(currentuser.ident, genbuf, NAMELEN);
			currentuser.ident[NAMELEN - 1] = '\0';
		}
	}
	sprintf(genbuf,"Enter - %s",fromhost);
	report(genbuf);
	started = 1;
	initscr();
	scrint = 1;
	if (USE_NOTEPAD == 1)
		notepad_init();
	if (strcasecmp(currentuser.userid, "guest") != 0 && USE_NOTEPAD == 1) {
		if (DEFINE(DEF_NOTEPAD)&& currentuser.numlogins != 0) {
			int     noteln;
			if (lastnote > currentuser.notedate)
				currentuser.noteline = 0;
			noteln = countln("etc/notepad");
			if (lastnote > currentuser.notedate || currentuser.noteline == 0) {
				shownotepad();
				currentuser.noteline = noteln;
				write_defnotepad();
			} else if ((noteln - currentuser.noteline) > 0) {
				move(0, 0);
				ansimore2("etc/notepad", NA, 0, noteln - currentuser.noteline + 1);
				igetkey();
				currentuser.noteline = noteln;
				write_defnotepad();
				clear();
			}
		}
	}
	if (show_statshm("0Announce/bbslist/countusr", 0) && DEFINE(DEF_GRAPH)) {
		refresh();
		pressanykey();
	}
	if ((vote_flag(NULL, '\0', 2 /* �ˬdŪ�L�s��Welcome �S */ ) == 0)) {
		if (dashf("Welcome")) {
			ansimore("Welcome", YEA);
			vote_flag(NULL, 'R', 2 /* �g�JŪ�L�s��Welcome */ );
		}
	} else {
		if (fill_shmfile(3, "Welcome2", "WELCOME_SHMKEY"))
			show_welcomeshm();
	}
	show_statshm("etc/posts/day", 1);
	refresh();
	move(t_lines - 2, 0);
	clrtoeol();
	prints("[1;36m�� �o�O�z�� [33m%d[36m �����X�����A�W���z�O�q [33m%s[36m �s�������C\n",
		currentuser.numlogins + 1, currentuser.lasthost);
	prints("�� �W���s�u�ɶ��� [33m%s[m ", ctime(&currentuser.lastlogin));
	igetkey();
	setuserfile(fname, DOTLOGINSBAD);
	if (ansimore(fname, NA) != -1) {
		if (askyn("�z�n�R���H�W�K�X��J���~���O����", YEA, YEA) == YEA)
			unlink(fname);
	}
	if (currentuser.gender == 'X' && currentuser.numlogins > 1)
		check_gender();

	strncpy(currentuser.lasthost, fromhost, 16);
	currentuser.lasthost[15] = '\0';	/* dumb mistake on my part */
	currentuser.lastlogin = time(0);
	login_start_time = time(0);

	set_safe_record();
	if (HAS_PERM(PERM_LOGINOK) &&
/*	skyo.0507 modify �֦� SYSOP �v���̡A���ݧ�s��� */
/*  edwardc.990624 ���M���ݭn��s���, ���O�n�۰ʩ������U���Įɶ� */
/*	!HAS_PERM(PERM_SYSOP) && */
		strcasecmp(currentuser.userid, "SYSOP") &&
		strcasecmp(currentuser.userid, "guest") &&
		(time(0) - currentuser.lastjustify >= REG_EXPIRED * 86400)) {
		if (HAS_PERM(PERM_SYSOP))
			currentuser.lastjustify = time(0);
		else {
			strcpy(currentuser.email, "");
			strcpy(currentuser.address, "");
			currentuser.userlevel &= ~(PERM_LOGINOK | PERM_PAGE);
			mail_file("etc/expired", currentuser.userid, "��s�ӤH��ƻ����C");
		}
	}
	
	if(HAS_PERM(PERM_BOARDS) && !HAS_PERM(PERM_OBOARDS)) {
		if ( !check_BM() ) {
			currentuser.userlevel &= ~(PERM_BOARDS);
			securityreport( "�w�����O�D�A�۰ʮ����O�D�v��" );
		}
	}
	
	currentuser.numlogins++;
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	if (currentuser.firstlogin == 0) {
		currentuser.firstlogin = login_start_time - 7 * 86400;
	}
	check_register_info();
}


void
set_numofsig()
{
	int     sigln;
	char    signame[STRLEN];
	setuserfile(signame, "signatures");
	sigln = countln(signame);
	numofsig = sigln / MAXSIGLINES;
	if ((sigln % MAXSIGLINES) != 0)
		numofsig += 1;
}
int
chk_friend_book()
{
	FILE   *fp;
	int     idnum, n = 0;
	char    buf[STRLEN], *ptr;
	if ((fp = fopen("friendbook", "r")) == NULL)
		return 0;

	move(5, 0);
	prints("[1m�t�δM�H�W�U�C��:[m\n\n");
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		char    uid[14];
		char    msg[STRLEN];
		struct user_info *uin;
		ptr = strstr(buf, "@");
		if (ptr == NULL)
			continue;
		ptr++;
		strcpy(uid, ptr);
		ptr = strstr(uid, "\n");
		*ptr = '\0';
		idnum = atoi(buf);
		if (idnum != usernum || idnum <= 0)
			continue;
		uin = t_search(uid, NA);
		sprintf(msg, "%s �w�g�W���C", currentuser.userid);
		if (!uinfo.invisible && uin != NULL && !DEFINE(DEF_NOLOGINSEND)
			&& do_sendmsg(uin, msg, 2, uin->pid) == 1) {
			prints("[1m%s[m ��A�A�t�Τw�g�i�D�L�A�W���������C\n", uid);
		} else
			prints("[1m%s[m ��A�A�t�εL�k�p����L�A�ЧA��L�p���C\n", uid);
		n++;
		del_from_file("friendbook", buf);
		if (n > 15) {
			pressanykey();
			move(7, 0);
			clrtobot();
		}
	}
	fclose(fp);
	return n;
}

int
check_maxmail()
{
	extern char currmaildir[STRLEN];
	/* chinsan.20011231: �W�[���l���ڤj�p����, 
	 * �åB�� "��{��" �u�ʹ�W���W�[��{�Ȫ��@�b, 
	 * ��KB�P���ĵ�ٳ��i�h�O �̫�[�W��{�u�ʼW�[�Ȥ��W�� + 100
	 * �t�~, �ϥΪ̭Y�W�L�u�ʤW�� + 10 ����, �h�|�b�ù��̤W�����"�l��L�q"
	 * port from bbs.ilc.edu.tw 
	 */
	int max_num, max_sum, mail_num, mail_sum;
	max_num = (HAS_PERM(PERM_SYSOP)) ?
		MAX_SYSOPMAIL_HOLD : (HAS_PERM(PERM_BOARDS)) ?
		MAX_BMMAIL_HOLD : MAX_MAIL_HOLD;
	max_sum = (HAS_PERM(PERM_SYSOP)) ?
		MAX_MAIL_SYSOP : (HAS_PERM(PERM_BOARDS)) ?
		MAX_MAIL_BM : MAX_MAIL_SUM;
		
	set_safe_record();
	mail_num = get_num_records(currmaildir, sizeof(struct fileheader));
	mail_sum = get_sum_records(currmaildir, sizeof(struct fileheader));
	
	max_num += (countperf(&currentuser)) / 2;
	max_sum += (countperf(&currentuser)) / 2;
	
	//currentuser.nummails = get_num_records(currmaildir, sizeof(struct fileheader));
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	if (mail_num > max_num) {
		prints("\n\n�z���p�H�H�󰪹F %d ��, �ЧR���L���H��, ���q�����b %d �ʥH�U�C\n", mail_num, max_num);
		prints("��H��W�L %d �ʮ�, �A�N�L�k�ϥΥ������e�H�\\��C\n", max_num + 10);
		if (mail_num > max_num + 100) {
			sprintf(genbuf, "�p�H�H���ƨϥιL��: %d ��", mail_num);
			securityreport(genbuf);
		}
	}
	if (mail_sum > max_sum) {
		prints("\n\n�z���p�H�H�󰪹F %d KB, �ЧR���L���H��, ���q�����b %d KB�H�U�C\n", mail_sum, max_sum);
		prints("��H��W�L %d �ʮ�, �A�N�L�k�ϥΥ������e�H�\\��C\n", max_sum + 10);
		if (mail_sum > max_sum + 100) {
			sprintf(genbuf, "�p�H�H��e�q�ϥιL��: %d KB", mail_sum);
			securityreport(genbuf);
		}
	}
		
	if ((mail_num > max_num + 10) || (mail_sum > max_sum + 10)) {
		mailXX = 1;
		return (1);
	} else {
		mailXX = 0;
		return (0);
	}
}
#ifndef BBSD
int
main(argc, argv)
int     argc;
char  **argv;
#else
void
start_client()
#endif
{
#ifdef BBS_INFOD
	if (strstr(argv[0], "bbsinfo") != NULL) {
		load_sysconf();
		bbsinfod_main(argc, argv);
		exit(0);
	}
#endif
	load_sysconf();

#ifndef BBSD
	if (argc < 2 || *argv[1] != 'h') {
		printf("You cannot execute this program directly.\n");
		exit(-1);
	}
	system_init(argc, argv);
#else
	system_init();
#endif

	if (setjmp(byebye)) {
		system_abort();
	}
#ifndef BBSD
	get_tty();
	init_tty();
#endif

	login_query();
	user_login();
	m_init();
	RMSG = NA;
	clear();
	c_recover();
#ifdef TALK_LOG
	tlog_recover();		/* 990713.edwardc for talk_log recover */
#endif

	if (strcasecmp(currentuser.userid, "guest")) {
		if (HAS_PERM(PERM_ACCOUNTS) && dashf("new_register")) {
			prints("[1;33m���s�ϥΪ̥��b���z�q�L���U��ơC[m");
		}
		check_maxmail();
		if (chk_friend_book())
			pressanykey();
		move(7, 0);
		clrtobot();
		if (!DEFINE(DEF_NOLOGINSEND))
			if (!uinfo.invisible)
				apply_ulist(friend_login_wall);
		pressanykey();
		clear();
		set_numofsig();
	}
	memset(netty_path, 0, sizeof(netty_path));
	nettyNN = NNread_init();

	fill_date();		/* �ˬd������ */
	b_closepolls();		/* �����벼 */

	num_alcounter();
	if (count_friends > 0 && DEFINE(DEF_LOGFRIEND))
		t_friends();
	while (1) {
		if (DEFINE(DEF_NORMALSCR))
			domenu("TOPMENU");
		else
			domenu("TOPMENU2");
		Goodbye();
	}
}

int     refscreen = NA;

int
egetch()
{
	int     rval;
	check_calltime();
	if (talkrequest) {
		talkreply();
		refscreen = YEA;
		return -1;
	}
	while (1) {
		rval = igetkey();
		if (talkrequest) {
			talkreply();
			refscreen = YEA;
			return -1;
		}
		if (rval != Ctrl('L'))
			break;
		redoscr();
	}
	refscreen = NA;
	return rval;
}

char   *
boardmargin()
{
	static char buf[STRLEN];
	if (selboard)
		sprintf(buf, "�Q�װ� [%s]", currboard);
	else {
		brc_initial(DEFAULTBOARD);
		if (getbnum(currboard)) {
			selboard = 1;
			sprintf(buf, "�Q�װ� [%s]", currboard);
		} else
			sprintf(buf, "�ثe�èS���]�w�Q�װ�");
	}
	return buf;
}
/*Add by SmallPig*/
void
update_endline()
{
	char    buf[STRLEN];
	time_t  now;
	int     allstay;
	if (!DEFINE(DEF_ENDLINE)) {
		move(t_lines - 1, 0);
		clrtoeol();
		return;
	}
	now = time(0);
	allstay = (now - login_start_time) / 60;
	move(t_lines - 1, 0);
	clrtoeol();
	sprintf(buf, "[[36m%.12s[33m]", currentuser.userid);
	num_alcounter();
	resolve_boards();
	if (strlen(brdshm->date) == 0) {
		strcpy(brdshm->date, "  �Sԣ�j��l  ");
	}
	prints("[1;44;33m�ɶ�:[[36m%16s[37m%14s[44;33m] �`�H��/�n��:[[36m%4d[33m/[1;36m%3d[33m] �b��:%-22s[m", 
		Ctime(&now) + 5, (is_birth(currentuser)) ? "  �ͤ�n�Ы�  " : brdshm->date, count_users, count_friends, buf);

}


/*ReWrite by SmallPig*/
void
showtitle(title, mid)
char   *title, *mid;
{
	char    buf[STRLEN], *note;
	int     spc1, spc2;
	note = boardmargin();
	spc1 = 39 - strlen(title) - strlen(mid) / 2;
	spc2 = 40 - strlen(note) - strlen(mid) / 2;
	if (spc1 < 2)
		spc1 = 2;
	if (spc2 < 2)
		spc2 = 2;
	move(0, 0);
	clrtoeol();
	sprintf(buf, "%*s", spc1, "");
	if (!strcmp(mid, BoardName))
		prints("[1;44;33m%s%s[37m%s[1;44m", title, buf, mid);
	else if (mid[0] == '[')
		prints("[1;44;33m%s%s[5;36m%s[m[1;44m", title, buf, mid);
	else
		prints("[1;44;33m%s%s[36m%s", title, buf, mid);
	sprintf(buf, "%*s", spc2, "");
	prints("%s[33m%s[m\n", buf, note);
	update_endline();
	move(1, 0);
}

void
docmdtitle(title, prompt)
char   *title, *prompt;
{
	char    middoc[30];
	if (chkmail())
		strcpy(middoc, "[�z���H��]");
	else if (mailXX == 1)
		strcpy(middoc, "[�H��L�q!!!]");
	else
		strcpy(middoc, BoardName);

	showtitle(title, middoc);
	move(1, 0);
	clrtoeol();
	prints("%s", prompt);
	clrtoeol();
}
/* this is standard edwardc's style function :) */
void
c_recover()
{
	char    fname[STRLEN], buf[STRLEN];
	int     a;
	sprintf(fname, "home/%c/%s/%s.deadve", toupper(currentuser.userid[0]), currentuser.userid, currentuser.userid);
	if ( dashf(fname) ) {
		clear();
		strcpy(genbuf, "");
		if (strcasecmp(currentuser.userid, "guest") == 0) {
			/*
			 * edwardc.990630 well, guest �õL�k���H,
			 * �ҥH���_�u�ƥ��H��H�c�O�S�� �ĪG��,
			 * �ҥH�ڭ̧⥦�s��T�w���@�ӼȦs��, �ݭn���H�A�h���X�ϥ�
			 */

			prints("[1;32m�z���@�ӽs��@�~�����`���_, �t�αN�ƥ��ɦs�� �Ȧs�� 1 [m\n");
			prints("(�Цb�s��ɫ� ESC + I + 1 ���X�Ȧs�ɤ��e)\n");
			sprintf(buf, "home/%c/%s/clip_1", toupper(currentuser.userid[0]), currentuser.userid);
			unlink(buf);	/* discard old file */
			rename(fname, buf);
			pressanykey();
			return;	/* bye bye */

		}
		getdata(0, 0, "[1;32m�z���@�ӽs��@�~�����`���_�A(S) �g�J�Ȧs�� (M) �H�^�H�c (Q) ��F�H[Q]�G[m", genbuf, 2, DOECHO, YEA);

		switch (genbuf[0]) {

		case 'M':
		case 'm':
			mail_file(fname, currentuser.userid, "�����`�_�u�ҫO�d������...");
			unlink(fname);
			break;

		case 'S':
		case 's':
			while (1) {
				strcpy(genbuf, "");
				getdata(2, 0, "[1;33m�п�ܼȦs�� [0-7] [0]�G[m", genbuf, 2, DOECHO, YEA);
				if (genbuf[0] == '\0')
					a = 0;
				else
					a = atoi(genbuf);

				if (a >= 0 && a <= 7) {
					sprintf(buf, "home/%c/%s/clip_%d", toupper(currentuser.userid[0]), currentuser.userid, a);
					if (dashf(buf)) {
						getdata(3, 0, "[1;31m�Ȧs�ɤw�s�b�A�л\\�Ϊ��[? (O)�л\\ (A)���[ [O]�G[m", genbuf, 2, DOECHO, YEA);
						switch (genbuf[0]) {
						case 'A':
						case 'a':
							f_cp(fname, buf, O_APPEND);
							uname(fname);
							break;
						default:
							unlink(buf);
							rename(fname, buf);
							break;
						}
					} else {
						rename(fname, buf);
					}
					break;
				}
			}
			break;

		case 'Q':
		case 'q':
		default:
			unlink(fname);
			break;

		}
	}
}

#ifdef TALK_LOG
void
tlog_recover()
{
	char    buf[256];
	sprintf(buf, "home/%c/%s/talk_log", toupper(currentuser.userid[0]), currentuser.userid);

	if (strcasecmp(currentuser.userid, "guest") == 0 || !dashf(buf))
		return;

	clear();
	strcpy(genbuf, "");

	getdata(0, 0, "[1;32m�z���@�Ӥ����`�_�u�үd�U�Ӫ���ѰO��, �z�n .. (M) �H�^�H�c (Q) ��F�H[Q]�G[m", genbuf, 2, DOECHO, YEA);

	if (genbuf[0] == 'M' || genbuf[0] == 'm')
		mail_file(buf, currentuser.userid, "��ѰO��");

	unlink(buf);
	return;

}
#endif

int
check_BM()
{
	int i, numboards;
	struct boardheader fh;
	char *str;
	
	numboards = get_num_records(BOARDS, sizeof(fh));
	
	for (i = 0; i < numboards; i++) {
		get_record(BOARDS, &fh, sizeof(fh), i) ;
		for ( str = strtok(fh.BM, ",: ;|&()\0\n");
			str != NULL;
			str = strtok( NULL, ",: ;|&()\0\n") )
			if ( !strcmp(currentuser.userid, str) )
				return YEA;
	}

	return NA;
}
