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
$Id: vote.c,v 1.5 2002/09/05 06:04:10 edwardc Exp $
*/

#include "bbs.h"
#include "vote.h"

extern  cmpbnames();
extern int page, range;
extern struct shortfile *bcache;
extern struct BCACHE *brdshm;
static char *vote_type[] = {"�O�D", "���", "�ƿ�", "�Ʀr", "�ݵ�"};
struct votebal currvote;
char    controlfile[STRLEN];
unsigned int result[33];
int     vnum;
int     voted_flag;
FILE   *sug;

int
cmpvuid(userid, uv)
char   *userid;
struct ballot *uv;
{
	return !strcmp(userid, uv->uid);
}

int
setvoteflag(bname, flag)
char   *bname;
int     flag;
{
	int     pos;
	struct boardheader fh;
	pos = search_record(BOARDS, &fh, sizeof(fh), cmpbnames, bname);
	if (flag == 0)
		fh.flag = fh.flag & ~VOTE_FLAG;
	else
		fh.flag = fh.flag | VOTE_FLAG;
	if (substitute_record(BOARDS, &fh, sizeof(fh), pos) == -1)
		prints("Error updating BOARDS file...\n");
}

void
b_report(str)
char   *str;
{
	char    buf[STRLEN];
	sprintf(buf, "%s %s", currboard, str);
	report(buf);
}

void
makevdir(bname)
char   *bname;
{
	struct stat st;
	char    buf[STRLEN];
	sprintf(buf, "vote/%s", bname);
	if (stat(buf, &st) != 0)
		mkdir(buf, 0755);
}

void
setvfile(buf, bname, filename)
char   *buf, *bname, *filename;
{
	sprintf(buf, "vote/%s/%s", bname, filename);
}

void
setcontrolfile()
{
	setvfile(controlfile, currboard, "control");
}
int
b_notes_edit()
{
	char    buf[STRLEN], buf2[STRLEN];
	char    ans[4];
	int     aborted;
	int     notetype;
	if (!chk_currBM(currBM)) {
		return 0;
	}
	clear();
	move(1, 0);
	prints("�s��/�R���Ƨѿ�");
	while (1) {
		getdata(3, 0, "�s��ΧR�����Q�װϪ� (0) ���}  (1) �@��Ƨѿ�  (2) ���K�Ƨѿ�? [1] ", ans, 2, DOECHO, YEA);
		if (ans[0] == '0')
			return FULLUPDATE;
		if (ans[0] == '\0')
			strcpy(ans, "1");
		if (ans[0] == '1' || ans[0] == '2')
			break;
	}
	makevdir(currboard);
	if (ans[0] == '2') {
		setvfile(buf, currboard, "secnotes");
		notetype = 2;
	} else {
		setvfile(buf, currboard, "notes");
		notetype = 1;
	}
	sprintf(buf2, "(E)�s�� (D)�R�� %4s�Ƨѿ�? [E]: ", (notetype == 1) ? "�@��" : "���K");
	getdata(5, 0, buf2, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd') {
		move(6, 0);
		sprintf(buf2, "�u���n�R��%4s�Ƨѿ�", (notetype == 1) ? "�@��" : "���K");
		if (askyn(buf2, NA, NA)) {
			move(7, 0);
			prints("�Ƨѿ��w�g�R��...\n");
			pressanykey();
			unlink(buf);
			aborted = 1;
		} else
			aborted = -1;
	} else
		aborted = vedit(buf, NA);
	if (aborted == -1) {
		pressreturn();
	} else {
		if (notetype == 1)
			setvfile(buf, currboard, "noterec");
		else
			setvfile(buf, currboard, "notespasswd");
		unlink(buf);
	}

	return FULLUPDATE;
}

int
b_notes_passwd()
{
	FILE   *pass;
	char    passbuf[20], prepass[20];
	char    buf[STRLEN];
	if (!chk_currBM(currBM)) {
		return 0;
	}
	clear();
	move(1, 0);
	prints("�]�w/���u���K�Ƨѿ��v�K�X...");
	setvfile(buf, currboard, "secnotes");
	if (!dashf(buf)) {
		move(3, 0);
		prints("���Q�װϩ|�L�u���K�Ƨѿ��v�C\n\n");
		prints("�Х��� W �s�n�u���K�Ƨѿ��v�A�ӳ]�w�K�X...");
		pressanykey();
		return FULLUPDATE;
	}
	if (!check_notespasswd())
		return FULLUPDATE;
	getdata(3, 0, "�п�J�s�����K�Ƨѿ��K�X: ", passbuf, 19, NOECHO, YEA);
	getdata(4, 0, "�T�{�s�����K�Ƨѿ��K�X: ", prepass, 19, NOECHO, YEA);
	if (strcmp(passbuf, prepass)) {
		prints("\n�K�X���۲�, �L�k�]�w�Χ��....");
		pressanykey();
		return FULLUPDATE;
	}
	setvfile(buf, currboard, "notespasswd");
	if ((pass = fopen(buf, "w")) == NULL) {
		move(5, 0);
		prints("�Ƨѿ��K�X�L�k�]�w....");
		pressanykey();
		return FULLUPDATE;
	}
	fprintf(pass, "%s\n", genpasswd(passbuf));
	fclose(pass);
	move(5, 0);
	prints("���K�Ƨѿ��K�X�]�w����....");
	sprintf(genbuf, "change notes passwd of %s", currboard);
	report(genbuf);
	pressanykey();
	return FULLUPDATE;
}

int
b_suckinfile(fp, fname)
FILE   *fp;
char   *fname;
{
	char    inbuf[256];
	FILE   *sfp;
	if ((sfp = fopen(fname, "r")) == NULL)
		return -1;
	while (fgets(inbuf, sizeof(inbuf), sfp) != NULL)
		fputs(inbuf, fp);
	fclose(sfp);
	return 0;
}
/*Add by SmallPig*/
int
catnotepad(fp, fname)
FILE   *fp;
char   *fname;
{
	char    inbuf[256];
	FILE   *sfp;
	int     count;
	count = 0;
	if ((sfp = fopen(fname, "r")) == NULL) {
		fprintf(fp, "[1;34m  ��[44m__________________________________________________________________________[m \n\n");
		return -1;
	}
	while (fgets(inbuf, sizeof(inbuf), sfp) != NULL) {
		if (count != 0)
			fputs(inbuf, fp);
		else
			count++;
	}
	fclose(sfp);
	return 0;
}

int
b_closepolls()
{
	char    buf[80];
	time_t  now, nextpoll;
	int     i, end;
	now = time(NULL);
	resolve_boards();

	if (now < brdshm->pollvote) {
		return;
	}
	move(t_lines - 1, 0);
	prints("�藍�_�A�t�������벼���A�еy��...");
	refresh();

	nextpoll = now + 7 * 3600;

	strcpy(buf, currboard);
	for (i = 0; i < brdshm->number; i++) {
		strcpy(currboard, (&bcache[i])->filename);
		setcontrolfile();
		end = get_num_records(controlfile, sizeof(currvote));
		for (vnum = end; vnum >= 1; vnum--) {
			time_t  closetime;
			get_record(controlfile, &currvote, sizeof(currvote), vnum);
			closetime = currvote.opendate + currvote.maxdays * 86400;
			if (now > closetime)
				mk_result(vnum);
			else if (nextpoll > closetime)
				nextpoll = closetime + 300;
		}
	}
	strcpy(currboard, buf);

	brdshm->pollvote = nextpoll;
	return 0;
}

int
count_result(ptr)
struct ballot *ptr;
{
	int     i;
	if (ptr == NULL) {
		if (sug != NULL)
			fclose(sug);
		return 0;
	}
	if (ptr->msg[0][0] != '\0') {
		if (currvote.type == VOTE_ASKING) {
			fprintf(sug, "[1m%s [m���@���p�U�G\n", ptr->uid);
		} else
			fprintf(sug, "[1m%s [m����ĳ�p�U�G\n", ptr->uid);
		for (i = 0; i < 3; i++)
			fprintf(sug, "%s\n", ptr->msg[i]);
	}
	result[32]++;
	if (currvote.type == VOTE_ASKING) {
		return 0;
	}
	if (currvote.type != VOTE_VALUE) {
		for (i = 0; i < 32; i++) {
			if ((ptr->voted >> i) & 1)
				(result[i])++;
		}

	} else {
		result[31] += ptr->voted;
		result[(ptr->voted * 10) / (currvote.maxtkt + 1)]++;
	}
	return 0;
}
get_result_title()
{
	char    buf[STRLEN];
	fprintf(sug, "�� �벼�}�ҩ�G[1m%.24s[m  ���O�G[1m%s[m\n", ctime(&currvote.opendate)
		,vote_type[currvote.type - 1]);
	fprintf(sug, "�� �D�D�G[1m%s[m\n", currvote.title);
	if (currvote.type == VOTE_VALUE)
		fprintf(sug, "�� �����벼���Ȥ��i�W�L�G[1m%d[m\n\n", currvote.maxtkt);
	fprintf(sug, "�� �����D�شy�z�G\n\n");
	sprintf(buf, "vote/%s/desc.%d", currboard, currvote.opendate);
	b_suckinfile(sug, buf);
}

int
mk_result(num)
int     num;
{
	char    fname[STRLEN], nname[STRLEN];
	char    sugname[STRLEN];
	char    title[STRLEN];
	int     i;
	unsigned int total = 0;
	setcontrolfile();
	sprintf(fname, "vote/%s/flag.%d", currboard, currvote.opendate);
	count_result(NULL);
	sprintf(sugname, "vote/%s/tmp.%d", currboard, uinfo.pid);
	if ((sug = fopen(sugname, "w")) == NULL) {
		report("open vote tmp file error");
		prints("Error: �����벼���~...\n");
		pressanykey();
	}
	(void) memset(result, 0, sizeof(result));
	if (apply_record(fname, count_result, sizeof(struct ballot)) == -1) {
		report("Vote apply flag error");
	}
	fprintf(sug, "[1;44;36m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t�ϥΪ�%s�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w[m\n\n\n", (currvote.type != VOTE_ASKING) ? "��ĳ�ηN��" : "�������@��");
	fclose(sug);
	sprintf(nname, "vote/%s/results", currboard);
	if ((sug = fopen(nname, "w")) == NULL) {
		report("open vote newresult file error");
		prints("Error: �����벼���~...\n");
	}
	get_result_title(sug);

	fprintf(sug, "** �벼���G:\n\n");
	if (currvote.type == VOTE_VALUE) {
		total = result[32];
		for (i = 0; i < 10; i++) {
			fprintf(sug, "[1m  %4d[m �� [1m%4d[m ������ [1m%4d[m ��  ���� [1m%d%%[m\n",
				(i * currvote.maxtkt) / 10 + ((i == 0) ? 0 : 1), ((i + 1) * currvote.maxtkt) / 10, result[i]
				,(result[i] * 100) / ((total <= 0) ? 1 : total));
		}
		fprintf(sug, "�����벼���G�����ȬO: [1m%d[m\n", result[31] / ((total <= 0) ? 1 : total));
	} else if (currvote.type == VOTE_ASKING) {
		total = result[32];
	} else {
		for (i = 0; i < currvote.totalitems; i++) {
			total += result[i];
		}
		for (i = 0; i < currvote.totalitems; i++) {
			fprintf(sug, "(%c) %-40s  %4d ��  ���� [1m%d%%[m\n", 'A' + i, currvote.items[i], result[i], (result[i] * 100) / ((total <= 0) ? 1 : total));
		}
	}
	fprintf(sug, "\n�벼�`�H�� = [1m%d[m �H\n", result[32]);
	fprintf(sug, "�벼�`���� =[1m %d[m ��\n\n", total);
	fprintf(sug, "[1;44;36m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t�ϥΪ�%s�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w[m\n\n\n", (currvote.type != VOTE_ASKING) ? "��ĳ�ηN��" : "�������@��");
	b_suckinfile(sug, sugname);
	unlink(sugname);
	fclose(sug);

	sprintf(title, "[���i] %s �O���벼���G", currboard);
	postfile(nname, "vote", title, 1);
	if (currboard != "vote")
		postfile(nname, currboard, title, 1);
	dele_vote(num);
	return;
}

int
get_vitems(bal)
struct votebal *bal;
{
	int     num;
	char    buf[STRLEN];
	move(3, 0);
	prints("�Ш̧ǿ�J�i��ܶ�, �� ENTER �����]�w.\n");
	num = 0;
	for (num = 0; num < 32; num++) {
		sprintf(buf, "%c) ", num + 'A');
		getdata((num % 16) + 4, (num / 16) * 40, buf, bal->items[num], 36, DOECHO, YEA);
		if (strlen(bal->items[num]) == 0) {
			if (num != 0)
				break;
			num = -1;
		}
	}
	bal->totalitems = num;
	return num;
}


int
vote_maintain(bname)
char   *bname;
{
	char    buf[STRLEN];
	struct votebal *ball = &currvote;
	int     aborted;
	setcontrolfile();
	if (!HAS_PERM(PERM_OVOTE))
		if (!chk_currBM(currBM)) {
			return 0;
		}
	stand_title("�}�ҧ벼�c");
	makevdir(bname);
	for (;;) {
		getdata(2, 0, "(1)�O�D, (2)���, (3)�ƿ�, (4)�ƭ� (5)�ݵ� (6)���� ? : "
			,genbuf, 2, DOECHO, YEA);
		genbuf[0] -= '0';

		if (genbuf[0] < 1 || genbuf[0] > 5) {
			prints("���������벼\n");
			return FULLUPDATE;
		}
		ball->type = (int) genbuf[0];
		break;
	}
	ball->opendate = time(NULL);
	prints("�Ы�������}�l�s�覹�� [�벼���y�z]: \n");
	igetkey();
	setvfile(genbuf, bname, "desc");
	sprintf(buf, "%s.%d", genbuf, ball->opendate);
	aborted = vedit(buf, NA);
	if (aborted) {
		clear();
		prints("���������벼\n");
		pressreturn();
		return FULLUPDATE;
	}
	clear();
	getdata(0, 0, "�����벼�Ҷ��Ѽ� (���i����): ", buf, 3, DOECHO, YEA);

	if (*buf == '\n' || atoi(buf) == 0 || *buf == '\0')
		strcpy(buf, "1");

	ball->maxdays = atoi(buf);
	for (;;) {
		getdata(1, 0, "�벼�c�����D: ", ball->title, 40, DOECHO, YEA);
		if (strlen(ball->title) > 0)
			break;
		bell();
	}
	switch (ball->type) {
	case VOTE_YN:
		ball->maxtkt = 0;
		strcpy(ball->items[0], "�٦�  �]�O���^");
		strcpy(ball->items[1], "���٦��]���O�^");
		strcpy(ball->items[2], "�S�N���]���M���^");
		ball->maxtkt = 1;
		ball->totalitems = 3;
		break;
	case VOTE_SINGLE:
		get_vitems(ball);
		ball->maxtkt = 1;
		break;
	case VOTE_MULTI:
		get_vitems(ball);
		for (;;) {
			getdata(21, 0, "�@�ӤH�̦h�X��? [1]: ", buf, 5, DOECHO, YEA);
			ball->maxtkt = atoi(buf);
			if (ball->maxtkt <= 0)
				ball->maxtkt = 1;
			if (ball->maxtkt > ball->totalitems)
				continue;
			break;
		}
		break;
	case VOTE_VALUE:
		for (;;) {
			getdata(3, 0, "��J�ƭȳ̤j���o�W�L [100] : ", buf, 4, DOECHO, YEA);
			ball->maxtkt = atoi(buf);
			if (ball->maxtkt <= 0)
				ball->maxtkt = 100;
			break;
		}
		break;
	case VOTE_ASKING:
/*                    getdata(3,0,"���ݵ��D�@����Ƥ����� :",buf,3,DOECHO,YEA) ;
                    ball->maxtkt = atof(buf) ;
                    if(ball->maxtkt <= 0) ball->maxtkt = 10;*/
		ball->maxtkt = 0;
		currvote.totalitems = 0;
		break;
	default:
		ball->maxtkt = 1;
		break;
	}
	setvoteflag(currboard, 1);
	clear();
	strcpy(ball->userid, currentuser.userid);
	if (append_record(controlfile, ball, sizeof(*ball)) == -1) {
		prints("�o���Y�������~�A�L�k�}�ҧ벼�A�гq�i����");
		b_report("Append Control file Error!!");
	} else {
		char    votename[STRLEN];
		int     i;
		b_report("OPEN");
		prints("�벼�c�}�ҤF�I\n");
		range++;;
		sprintf(votename, "tmp/votetmp.%s.%05d", currentuser.userid, uinfo.pid);
		if ((sug = fopen(votename, "w")) != NULL) {
			sprintf(buf, "[�q��] %s �|��벼�G%s", currboard, ball->title);
			get_result_title(sug);
			if (ball->type != VOTE_ASKING && ball->type != VOTE_VALUE) {
				fprintf(sug, "\n�i[1m�ﶵ�p�U[m�j\n");
				for (i = 0; i < ball->totalitems; i++) {
					fprintf(sug, "([1m%c[m) %-40s\n", 'A' + i, ball->items[i]);
				}
			}
			fclose(sug);
			postfile(votename, "vote", buf, 1);
			unlink(votename);
		}
	}
	pressreturn();
	return FULLUPDATE;
}


int
vote_flag(bname, val, mode)
char   *bname, val;
int     mode;
{
	char    buf[STRLEN], flag;
	int     fd, num, size;
	num = usernum - 1;
	switch (mode) {
	case 2:
		sprintf(buf, "Welcome.rec");	/* �i���� Welcome �e�� */
		break;
	case 1:
		setvfile(buf, bname, "noterec");	/* �Q�װϳƧѿ����X�� */
		break;
	default:
		return -1;
	}
	if (num >= MAXUSERS) {
		report("Vote Flag, Out of User Numbers");
		return -1;
	}
	if ((fd = open(buf, O_RDWR | O_CREAT, 0600)) == -1) {
		return -1;
	}
	f_exlock(fd);
	size = (int) lseek(fd, 0, SEEK_END);
	memset(buf, 0, sizeof(buf));
	while (size <= num) {
		write(fd, buf, sizeof(buf));
		size += sizeof(buf);
	}
	lseek(fd, (off_t) num, SEEK_SET);
	read(fd, &flag, 1);
	if ((flag == 0 && val != 0)) {
		lseek(fd, (off_t) num, SEEK_SET);
		write(fd, &val, 1);
	}
	f_unlock(fd);
	close(fd);
	return flag;
}


int
vote_check(bits)
int     bits;
{
	int     i, count;
	for (i = count = 0; i < 32; i++) {
		if ((bits >> i) & 1)
			count++;
	}
	return count;
}

int
showvoteitems(pbits, i, flag)
unsigned int pbits;
int     i, flag;
{
	char    buf[STRLEN];
	int     count;
	if (flag == YEA) {
		count = vote_check(pbits);
		if (count > currvote.maxtkt)
			return NA;
		move(2, 0);
		clrtoeol();
		prints("�z�w�g��F [1m%d[m ��", count);
	}
	sprintf(buf, "%c.%2.2s%-36.36s", 'A' + i,
		((pbits >> i) & 1 ? "��" : "  "), currvote.items[i]);
	move(i + 6 - ((i > 15) ? 16 : 0), 0 + ((i > 15) ? 40 : 0));
	prints(buf);
	refresh();
	return YEA;
}

void
show_voteing_title()
{
	time_t  closedate;
	char    buf[STRLEN];
	if (currvote.type != VOTE_VALUE && currvote.type != VOTE_ASKING)
		sprintf(buf, "�i�벼��: [1m%d[m ��", currvote.maxtkt);
	else
		buf[0] = '\0';
	closedate = currvote.opendate + currvote.maxdays * 86400;
	prints("�벼�N������: [1m%24s[m  %s  %s\n",
		ctime(&closedate), buf, (voted_flag) ? "([5;1m�ק�e���벼[m)" : "");
	prints("�벼�D�D�O: [1m%-50s[m����: [1m%s[m \n", currvote.title, vote_type[currvote.type - 1]);
}

int
getsug(uv)
struct ballot *uv;
{
	int     i, line;
	move(0, 0);
	clrtobot();
	if (currvote.type == VOTE_ASKING) {
		show_voteing_title();
		line = 3;
		prints("�ж�J�z���@��(�T��):\n");
	} else {
		line = 1;
		prints("�ж�J�z�_�Q���N��(�T��):\n");
	}
	move(line, 0);
	for (i = 0; i < 3; i++) {
		prints(": %s\n", uv->msg[i]);
	}
	for (i = 0; i < 3; i++) {
		getdata(line + i, 0, ": ", uv->msg[i], STRLEN - 2, DOECHO, NA);
		if (uv->msg[i][0] == '\0')
			break;
	}
	return i;
}


int
multivote(uv)
struct ballot *uv;
{
	unsigned int i;
	i = uv->voted;
	move(0, 0);
	show_voteing_title();
	uv->voted = setperms(uv->voted, "�ﲼ", currvote.totalitems, showvoteitems);
	if (uv->voted == i)
		return -1;
	return 1;
}

int
valuevote(uv)
struct ballot *uv;
{
	unsigned int chs;
	char    buf[10];
	chs = uv->voted;
	move(0, 0);
	show_voteing_title();
	prints("�����@�����Ȥ���W�L [1m%d[m", currvote.maxtkt);
	if (uv->voted != 0)
		sprintf(buf, "%d", uv->voted);
	else
		memset(buf, 0, sizeof(buf));
	do {
		getdata(3, 0, "�п�J�@�ӭ�? [0]: ", buf, 5, DOECHO, NA);
		uv->voted = abs(atoi(buf));
	} while (uv->voted > currvote.maxtkt && buf[0] != '\n' && buf[0] != '\0');
	if (buf[0] == '\n' || buf[0] == '\0' || uv->voted == chs)
		return -1;
	return 1;
}

int
user_vote(num)
int     num;
{
	char    fname[STRLEN], bname[STRLEN];
	char    buf[STRLEN];
	struct ballot uservote, tmpbal;
	int     votevalue;
	int     aborted = NA, pos;
	move(t_lines - 2, 0);
	get_record(controlfile, &currvote, sizeof(struct votebal), num);
	if (currentuser.firstlogin > currvote.opendate) {
		prints("�藍�_, �벼�W�U�W�䤣��z���j�W\n");
		pressanykey();
		return;
	}
	sprintf(fname, "vote/%s/flag.%d", currboard, currvote.opendate);
	if ((pos = search_record(fname, &uservote, sizeof(uservote), cmpvuid,
				currentuser.userid)) <= 0) {
		(void) memset(&uservote, 0, sizeof(uservote));
		voted_flag = NA;
	} else {
		voted_flag = YEA;
	}
	strcpy(uservote.uid, currentuser.userid);
	sprintf(bname, "desc.%d", currvote.opendate);
	setvfile(buf, currboard, bname);
	ansimore(buf, YEA);
	move(0, 0);
	clrtobot();
	switch (currvote.type) {
	case VOTE_SINGLE:
	case VOTE_MULTI:
	case VOTE_YN:
		votevalue = multivote(&uservote);
		if (votevalue == -1)
			aborted = YEA;
		break;
	case VOTE_VALUE:
		votevalue = valuevote(&uservote);
		if (votevalue == -1)
			aborted = YEA;
		break;
	case VOTE_ASKING:
		uservote.voted = 0;
		aborted = !getsug(&uservote);
		break;
	}
	clear();
	if (aborted == YEA) {
		prints("�O�d �i[1m%s[m�j��Ӫ����벼�C\n", currvote.title);
	} else {
		if (currvote.type != VOTE_ASKING)
			getsug(&uservote);
		pos = search_record(fname, &tmpbal, sizeof(tmpbal), cmpvuid,
			currentuser.userid);
		if (pos) {
			substitute_record(fname, &uservote, sizeof(uservote), pos);
		} else if (append_record(fname, &uservote, sizeof(uservote)) == -1) {
			move(2, 0);
			clrtoeol();
			prints("�벼����! �гq�������ѥ[���@�ӿﶵ�벼\n");
			pressreturn();
		}
		prints("\n�w�g���z��J���c��...\n");
	}
	pressanykey();
	return;
}
voteexp()
{
	clrtoeol();
	prints("[1;44m�s�� �}�ҧ벼�c�� �}�Ҥ� %-40s���O �Ѽ� �H��[m\n", "�벼�D�D");
}

int
printvote(ent)
struct votebal *ent;
{
	static int i;
	struct ballot uservote;
	char    buf[STRLEN + 10], *date;
	char    flagname[STRLEN];
	int     num_voted;
	if (ent == NULL) {
		move(2, 0);
		voteexp();
		i = 0;
		return 0;
	}
	i++;
	if (i > page + 19 || i > range)
		return QUIT;
	else if (i <= page)
		return 0;
	sprintf(buf, "flag.%d", ent->opendate);
	setvfile(flagname, currboard, buf);
	if (search_record(flagname, &uservote, sizeof(uservote), cmpvuid,
			currentuser.userid) <= 0) {
		voted_flag = NA;
	} else
		voted_flag = YEA;
	num_voted = get_num_records(flagname, sizeof(struct ballot));
	date = ctime(&ent->opendate) + 4;
	sprintf(buf, " %s%3d %-12.12s %-6.6s %-40.40s%-4.4s %3d  %4d[m\n", (voted_flag == NA) ? "[1m" : "", i, ent->userid,
		date, ent->title, vote_type[ent->type - 1], ent->maxdays, num_voted);
	prints("%s", buf);
}

int
dele_vote(num)
int     num;
{
	char    buf[STRLEN];
	sprintf(buf, "vote/%s/flag.%d", currboard, currvote.opendate);
	unlink(buf);
	sprintf(buf, "vote/%s/desc.%d", currboard, currvote.opendate);
	unlink(buf);
	if (delete_record(controlfile, sizeof(currvote), num) == -1) {
		prints("�o�Ϳ��~�A�гq������....");
		pressanykey();
	}
	range--;
	if (get_num_records(controlfile, sizeof(currvote)) == 0) {
		setvoteflag(currboard, 0);
	}
}

int
vote_results(bname)
char   *bname;
{
	char    buf[STRLEN];
	setvfile(buf, bname, "results");
	if (ansimore(buf, YEA) == -1) {
		move(3, 0);
		prints("�ثe�S������벼�����G�C\n");
		clrtobot();
		pressreturn();
	} else
		clear();
	return FULLUPDATE;
}

int
b_vote_maintain()
{
	return vote_maintain(currboard);
}

void
vote_title()
{

	docmdtitle("[�벼�c�C��]",
		"[[1;32m��[m,[1;32me[m] ���} [[1;32mh[m] �D�U [[1;32m��[m,[1;32mr <cr>[m] �i��벼 [[1;32m��[m,[1;32m��[m] �W,�U��� [1m���G��[m��ܩ|���벼");
	update_endline();
}

int
vote_key(ch, allnum, pagenum)
int     ch;
int     allnum, pagenum;
{
	int     deal = 0, ans;
	char    buf[STRLEN];
	switch (ch) {
	case 'v':
	case 'V':
	case '\n':
	case '\r':
	case 'r':
	case KEY_RIGHT:
		user_vote(allnum + 1);
		deal = 1;
		break;
	case 'R':
		vote_results(currboard);
		deal = 1;
		break;
	case 'H':
	case 'h':
		show_help(F_HELP_VOTE);
		deal = 1;
		break;
	case 'A':
	case 'a':
		if ((!HAS_PERM(PERM_OVOTE)) && !chk_currBM(currBM))
			return YEA;
		vote_maintain(currboard);
		deal = 1;
		break;
	case 'O':
	case 'o':
		if ((!HAS_PERM(PERM_OVOTE)) &&!chk_currBM(currBM))
			return YEA;
		clear();
		deal = 1;
		get_record(controlfile, &currvote, sizeof(struct votebal), allnum + 1);
		prints("[5;1;31mĵ�i!![m\n");
		prints("�벼�c���D�G[1m%s[m\n", currvote.title);
		ans = askyn("�A�T�w�n���������o�ӧ벼��", NA, NA);

		if (ans != 1) {
			move(2, 0);
			prints("�����R�����\n");
			pressreturn();
			clear();
			break;
		}
		mk_result(allnum + 1);
		sprintf(buf, "���������벼 %s", currvote.title);
		securityreport(buf);
		break;
	case 'D':
	case 'd':
		if (!HAS_PERM(PERM_OVOTE))
			if (!chk_currBM(currBM)) {
				return 1;
			}
		deal = 1;
		get_record(controlfile, &currvote, sizeof(struct votebal), allnum + 1);
		clear();
		prints("[5;1;31mĵ�i!![m\n");
		prints("�벼�c���D�G[1m%s[m\n", currvote.title);
		ans = askyn("�z�T�w�n�j�������o�ӧ벼��", NA, NA);

		if (ans != 1) {
			move(2, 0);
			prints("�����R�����\n");
			pressreturn();
			clear();
			break;
		}
		sprintf(buf, "�j�������벼 %s", currvote.title);
		securityreport(buf);
		dele_vote(allnum + 1);
		break;
	default:
		return 0;
	}
	if (deal) {
		Show_Votes();
		vote_title();
	}
	return 1;
}
Show_Votes()
{

	move(3, 0);
	clrtobot();
	printvote(NULL);
	setcontrolfile();
	if (apply_record(controlfile, printvote, sizeof(struct votebal)) == -1) {
		prints("���~�A�S���벼�c�}��....");
		pressreturn();
		return 0;
	}
	clrtobot();
	return 0;
}

int
b_vote()
{
	int     num_of_vote;
	int     voting;
	if (!HAS_PERM(PERM_VOTE) || (currentuser.stay < 1800)) {
		return;
	}
	setcontrolfile();
	num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
	if (num_of_vote == 0) {
		move(3, 0);
		clrtobot();
		prints("��p, �ثe�èS������벼�|��C\n");
		pressreturn();
		setvoteflag(currboard, 0);
		return FULLUPDATE;
	}
	setlistrange(num_of_vote);
	clear();
	voting = choose(NA, 0, vote_title, vote_key, Show_Votes, user_vote);
	clear();
	return /* user_vote( currboard ) */ FULLUPDATE;
}

int
b_results()
{
	return vote_results(currboard);
}

int
m_vote()
{
	char    buf[STRLEN];
	strcpy(buf, currboard);
	strcpy(currboard, DEFAULTBOARD);
	modify_user_mode(ADMIN);
	vote_maintain(DEFAULTBOARD);
	strcpy(currboard, buf);
	return;
}

int
x_vote()
{
	char    buf[STRLEN];
	modify_user_mode(XMENU);
	strcpy(buf, currboard);
	strcpy(currboard, DEFAULTBOARD);
	b_vote();
	strcpy(currboard, buf);
	return;
}

int
x_results()
{
	modify_user_mode(XMENU);
	return vote_results(DEFAULTBOARD);
}
