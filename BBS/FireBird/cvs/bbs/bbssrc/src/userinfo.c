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
$Id: userinfo.c,v 1.12 2002/06/12 12:39:39 edwardc Exp $
*/

#include "bbs.h"

extern time_t login_start_time;
extern char fromhost[60];
char   *
genpasswd();
char   *sysconf_str();

void
disply_userinfo(u, real)
struct userec *u;
int     real;
{
	int     num, diff;
	int     exp;
#ifdef REG_EXPIRED
	time_t  nextreg, now;
#endif

	move(2, 0);
	clrtobot();
	prints("�z���N��     : %-40s", u->userid);
	if (real)
		prints("      �ʧO : %s", (u->gender == 'M' ? "�k" : "�k"));
	prints("\n�z���ʺ�     : %s\n", u->username);
	prints("�u��m�W     : %-40s", u->realname);
	if (real)
		prints("  �X�ͤ�� : %d/%d/%d", u->birthmonth, u->birthday, u->birthyear + 1900);
	prints("\n�~���}     : %s\n", u->address);
	prints("�q�l�l��H�c : %s\n", u->email);
	if (real) {
		prints("�u�� E-mail  : %s\n", u->reginfo);
		if HAS_PERM
			(PERM_ADMINMENU)
				prints("Ident ���   : %s\n", u->ident);
	}
	prints("�׺ݾ��κA   : %s\n", u->termtype);
	prints("�b���إߤ�� : %s", ctime(&u->firstlogin));
	prints("�̪���{��� : %s", ctime(&u->lastlogin));
	if (real) {

		/* edwardc.990410 ��ܤU�������T�{�ɶ������� ? :p */
#ifndef REG_EXPIRED
		prints("�����T�{��� : %s", (u->lastjustify == 0) ? "�������U\n" : Ctime(&u->lastjustify));
#else
		nextreg = u->lastjustify + REG_EXPIRED * 86400;
		prints("�����T�{     : %s", (u->lastjustify == 0) ? "�������U" : ((nextreg - now) < 0 ) ? "[1;31m���U�L��[0m" : "�w�����A���Ĵ���: ");
		now = time(0);
		sprintf(genbuf, "�A%s %d ��\n", ((nextreg - now) < 0) ? "�W�L" : "�٦�", (nextreg - now) / 86400);
		prints("%s%s", (u->lastjustify == 0) ? "" : (char *) Ctime(&nextreg)
			,(u->lastjustify == 0) ? "\n" : genbuf);
#endif

		prints("�̪���{���� : %s\n", u->lasthost);
		prints("�峹�ƥ�     : %d\n", u->numposts);
		prints("�p�H�H�c     : %d ��\n", u->nummails);
	}
	prints("�W������     : %d ��\n", u->numlogins);
	prints("�W���`�ɼ�   : %d �p�� %d ���� ( %d �~ %d �� %d �� )\n", u->stay / 3600, (u->stay / 60) % 60
	, u->stay / 31536000 , ((u->stay / 2628000) % 2628000) % 12, ((u->stay / 86400 ) % 86400) % 30 );
	/* chinsan.20011229: ��...�o�����ӨS���D�F */
	
	exp = countexp(u);
	prints("�g���       : %d  (%s)\n", exp, cexp(exp));
	exp = countperf(u);
	prints("��{��       : %d  (%s)\n", exp, cperf(exp));
	if (real) {
		strcpy(genbuf, "bTCPRp#@XWBA#VS-DOM-F012345678\0");
		for (num = 0; num < strlen(genbuf) - 1; num++)
			if (!(u->userlevel & (1 << num)))
				genbuf[num] = '-';
		prints("�ϥΪ��v��   : %s\n", genbuf);
	} else {
		diff = (time(0) - login_start_time) / 60;
		prints("���d����     : %d �p�� %02d ��\n", diff / 60, diff % 60);
		prints("�ù��j�p     : %dx%d\n", t_lines, t_columns);
	}
	prints("\n");
	if (u->userlevel & PERM_BOARDS) {
		prints("  �z�O�������O�D, �P�±z���I�X.\n");
	} else if (u->userlevel & PERM_LOGINOK) {
		prints("  �z�����U�{�Ǥw�g����, �w��[�J����.\n");
	} else if (u->lastlogin - u->firstlogin < 3 * 86400) {
		prints("  �s��W��, �о\\Ū Announce �Q�װ�.\n");
	} else {
		prints("  ���U�|�����\\, �аѦҥ����i���e������.\n");
	}
}


int
uinfo_query(u, real, unum)
struct userec *u;
int     real, unum;
{
	struct userec newinfo;
	char    ans[3], buf[STRLEN], genbuf[128];
	char    src[STRLEN], dst[STRLEN];
	int     i, fail = 0, netty_check = 0;
	time_t  now;
	struct tm *tmnow;
	memcpy(&newinfo, u, sizeof(currentuser));
	getdata(t_lines - 1, 0, real ?
		"�п�� (0)���� (1)�ק��� (2)�]�w�K�X (3) �� ID ==> [0]" :
		"�п�� (0)���� (1)�ק��� (2)�]�w�K�X (3) ��ñ�W�� ==> [0]",
		ans, 2, DOECHO, YEA);
	clear();
	refresh();

	now = time(0);
	tmnow = localtime(&now);

	i = 3;
	move(i++, 0);
	if (ans[0] != '3' || real)
		prints("�ϥΪ̥N��: %s\n", u->userid);

	switch (ans[0]) {
	case '1':
		move(1, 0);
		prints("�гv���ק�,������ <ENTER> �N��ϥ� [] ������ơC\n");

		sprintf(genbuf, "�ʺ� [%s]: ", u->username);
		getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, YEA);
		if (buf[0])
			strncpy(newinfo.username, buf, NAMELEN);
		if (!real && buf[0])
			strncpy(uinfo.username, buf, 40);

		if (real) {
			sprintf(genbuf, "�u��m�W [%s]: ", u->realname);
			getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, YEA);
			if (buf[0])
				strncpy(newinfo.realname, buf, NAMELEN);
		}

		sprintf(genbuf, "�~��a�} [%s]: ", u->address);
		getdata(i++, 0, genbuf, buf, STRLEN - 10, DOECHO, YEA);
		if (buf[0])
			strncpy(newinfo.address, buf, NAMELEN);

		sprintf(genbuf, "�q�l�H�c [%s]: ", u->email);
		getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, YEA);
		if (buf[0]) {
			netty_check = 1;
			strncpy(newinfo.email, buf, NAMELEN-12);
		}
		sprintf(genbuf, "�׺ݾ��κA [%s]: ", u->termtype);
		getdata(i++, 0, genbuf, buf, 16, DOECHO, YEA);
		if (buf[0])
			strncpy(newinfo.termtype, buf, 16);

		sprintf(genbuf, "�X�ͦ~ [%d]: ", u->birthyear + 1900);
		getdata(i++, 0, genbuf, buf, 5, DOECHO, YEA);
		if (buf[0] && atoi(buf) > tmnow->tm_year + 1805 && atoi(buf) < tmnow->tm_year + 1897)
			newinfo.birthyear = atoi(buf) - 1900;

		sprintf(genbuf, "�X�ͤ� [%d]: ", u->birthmonth);
		getdata(i++, 0, genbuf, buf, 3, DOECHO, YEA);
		if (buf[0] && atoi(buf) >= 1 && atoi(buf) <= 12)
			newinfo.birthmonth = atoi(buf);

		sprintf(genbuf, "�X�ͤ� [%d]: ", u->birthday);
		getdata(i++, 0, genbuf, buf, 3, DOECHO, YEA);
		if (buf[0] && atoi(buf) >= 1 && atoi(buf) <= 31)
			newinfo.birthday = atoi(buf);

		sprintf(genbuf, "�ʧO [%c](M/F): ", u->gender);
		getdata(i++, 0, genbuf, buf, 2, DOECHO, YEA);
		if (buf[0]) {
			if (strchr("MmFf", buf[0]))
				newinfo.gender = toupper(buf[0]);
		}
		if (real) {
			sprintf(genbuf, "�u��Email[%s]: ", u->reginfo);
			getdata(i++, 0, genbuf, buf, 62, DOECHO, YEA);
			if (buf[0])
				strncpy(newinfo.reginfo, buf, STRLEN - 16);

			sprintf(genbuf, "�W�u���� [%d]: ", u->numlogins);
			getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
			if (atoi(buf) > 0)
				newinfo.numlogins = atoi(buf);

			sprintf(genbuf, "�峹�ƥ� [%d]: ", u->numposts);
			getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
			if (atoi(buf) > 0)
				newinfo.numposts = atoi(buf);
			
			sprintf(genbuf, "�W����� [%d]: ", u->stay);
			getdata(i++, 0, genbuf, buf, 16, DOECHO, YEA);
			if (atoi(buf) > 0)
				newinfo.stay = atoi(buf);
		}
		break;
	case '2':
		if (!real) {
			getdata(i++, 0, "�п�J��K�X: ", buf, PASSLEN, NOECHO, YEA);
			if (*buf == '\0' || !checkpasswd(u->passwd, buf)) {
				prints("\n\n�ܩ�p, �z��J���K�X�����T�C\n");
				fail++;
				break;
			}
		}
		getdata(i++, 0, "�г]�w�s�K�X: ", buf, PASSLEN, NOECHO, YEA);
		if (buf[0] == '\0') {
			prints("\n\n�K�X�]�w����, �~��ϥ��±K�X\n");
			fail++;
			break;
		}
		strncpy(genbuf, buf, PASSLEN);

		getdata(i++, 0, "�Э��s��J�s�K�X: ", buf, PASSLEN, NOECHO, YEA);
		if (strncmp(buf, genbuf, PASSLEN)) {
			prints("\n\n�s�K�X�T�{����, �L�k�]�w�s�K�X�C\n");
			fail++;
			break;
		}
		buf[8] = '\0';
		strncpy(newinfo.passwd, genpasswd(buf), ENCPASSLEN);
		break;
	case '3':
		if (!real) {
			sprintf(genbuf, "�ثe�ϥ�ñ�W�� [%d]: ", u->signature);
			getdata(i++, 0, genbuf, buf, 16, DOECHO, YEA);
			if (atoi(buf) > 0)
				newinfo.signature = atoi(buf);
		} else {
			getdata(i++, 0, "�s���ϥΪ̥N��: ", genbuf, IDLEN + 1, DOECHO, YEA);
			if (*genbuf != '\0') {
				if (getuser(genbuf)) {
					prints("\n���~! �w�g���P�� ID ���ϥΪ�\n");
					fail++;
				} else {
					strncpy(newinfo.userid, genbuf, IDLEN + 2);
				}
			}
		}
		break;
	default:
		clear();
		return 0;
	}
	if (fail != 0) {
		pressreturn();
		clear();
		return 0;
	}
	if (askyn("�T�w�n���ܶ�", NA, YEA) == YEA) {
		if (real) {
			char    secu[STRLEN];
			sprintf(secu, "�ק� %s ���򥻸�ƩαK�X�C", u->userid);
			securityreport(secu);
		}
		if (strcmp(u->userid, newinfo.userid)) {

			sprintf(src, "mail/%c/%s", toupper(u->userid[0]), u->userid);
			sprintf(dst, "mail/%c/%s", toupper(newinfo.userid[0]), newinfo.userid);
			rename(src, dst);
			sethomepath(src, u->userid);
			sethomepath(dst, newinfo.userid);
			rename(src, dst);
			sethomefile(src, u->userid, "register");
			unlink(src);
			sethomefile(src, u->userid, "register.old");
			unlink(src);
			setuserid(unum, newinfo.userid);
		}
		if ((netty_check == 1)) {
			sprintf(genbuf, "%s", BBSHOST);
			if ((sysconf_str("EMAILFILE") != NULL) &&
				(!strstr(newinfo.email, genbuf)) &&
				(!invalidaddr(newinfo.email)) &&
				(!invalid_email(newinfo.email))) {
				strcpy(u->email, newinfo.email);
				send_regmail(u);
			} else {
				if (sysconf_str("EMAILFILE") != NULL) {
					move(t_lines - 5, 0);
					prints("\n�z�Ҷ񪺹q�l�l��a�} �i[1;33m%s[m�j\n", newinfo.email);
					prints("�����������ӻ{�A�t�Τ��|�뻼���U�H�A�Ч⥦�ץ��n...\n");
					pressanykey();
					/*
					 * edwardc.990521 if there's wrong
					 * email, do not update anything.
					 */
					return 0;
				}
			}
		}
		memcpy(u, &newinfo, sizeof(newinfo));
		set_safe_record();
		if (netty_check == 1) {
			newinfo.userlevel &= ~(PERM_LOGINOK | PERM_PAGE);
			sethomefile(src, newinfo.userid, "register");
			sethomefile(dst, newinfo.userid, "register.old");
			rename(src, dst);
		}
		substitute_record(PASSFILE, &newinfo, sizeof(newinfo), unum);
	}
	clear();
	return 0;
}

void
x_info()
{
	modify_user_mode(GMENU);
	if (!strcmp("guest", currentuser.userid)) {
		disply_userinfo(&currentuser, 0);
		pressreturn();
		return;
	}
	disply_userinfo(&currentuser, 1);
	uinfo_query(&currentuser, 0, usernum);
}

void
getfield(line, info, desc, buf, len)
int     line, len;
char   *info, *desc, *buf;
{
	char    prompt[STRLEN];
	sprintf(genbuf, "  ����]�w: %-46.46s [1;32m(%s)[m",
		(buf[0] == '\0') ? "(���]�w)" : buf, info);
	move(line, 0);
	prints(genbuf);
	sprintf(prompt, "  %s: ", desc);
	getdata(line + 1, 0, prompt, genbuf, len, DOECHO, YEA);
	if (genbuf[0] != '\0') {
		strncpy(buf, genbuf, len);
	}
	move(line, 0);
	clrtoeol();
	prints("  %s: %s\n", desc, buf);
	clrtoeol();
}

void
x_fillform()
{
	char    rname[NAMELEN], addr[STRLEN];
	char    phone[STRLEN], dept[STRLEN], assoc[STRLEN];
	char    ans[5], *mesg, *ptr;
	FILE   *fn;
	time_t  now;
	modify_user_mode(NEW);
	move(3, 0);
	clrtobot();
	if (!strcmp("guest", currentuser.userid)) {
		prints("��p, �Х� new �ӽФ@�ӷs�b����A��ӽЪ�.");
		pressreturn();
		return;
	}
	if (currentuser.userlevel & PERM_LOGINOK) {
		prints("�z�w�g�����������ϥΪ̵��U����, �w��[�J��������C.");
		pressreturn();
		return;
	}
	if ((fn = fopen("new_register", "r")) != NULL) {
		while (fgets(genbuf, STRLEN, fn) != NULL) {
			if ((ptr = strchr(genbuf, '\n')) != NULL)
				*ptr = '\0';
			if (strncmp(genbuf, "userid: ", 8) == 0 &&
				strcmp(genbuf + 8, currentuser.userid) == 0) {
				fclose(fn);
				prints("�����|���B�z�z�����U�ӽг�, �Э@�ߵ���.");
				pressreturn();
				return;
			}
		}
		fclose(fn);
	}
	move(3, 0);
	if (askyn("�z�T�w�n��g���U���", NA, NA) == NA)
		return;
	strncpy(rname, currentuser.realname, NAMELEN);
	strncpy(addr, currentuser.address, STRLEN);
	dept[0] = phone[0] = assoc[0] = '\0';
	while (1) {
		move(3, 0);
		clrtoeol();
		prints("%s �z�n, �оڹ��g�H�U�����:\n", currentuser.userid);
		getfield(6, "�ХΤ���", "�u��m�W", rname, NAMELEN);
		getfield(8, "�Ǯըt�ũΤ��q¾��", "�Ǯըt��", dept, STRLEN);
		getfield(10, "�]�A��ǩΪ��P���X", "�ثe��}", addr, STRLEN);
		getfield(12, "�]�A�i�p���ɶ�", "�p���q��", phone, STRLEN);
#ifdef NEED_ASSOC		/* edwardc.990410 i think it's not general
				 * enough */
		getfield(14, "�դͷ|�β��~�Ǯ�", "�� �� �|", assoc, STRLEN);
#endif
		mesg = "�H�W��ƬO�_���T, �� Q �����U (Y/N/Quit)? [N]: ";
		getdata(t_lines - 1, 0, mesg, ans, 3, DOECHO, YEA);
		if (ans[0] == 'Q' || ans[0] == 'q')
			return;
		if (ans[0] == 'Y' || ans[0] == 'y')
			break;
	}
	strncpy(currentuser.realname, rname, NAMELEN);
	strncpy(currentuser.address, addr, STRLEN);
	if ((fn = fopen("new_register", "a")) != NULL) {
		now = time(NULL);
		fprintf(fn, "usernum: %d, %s\n", usernum, Ctime(&now));	
			/* edwardc.020612 add a newline in case of register is broken. */
		fprintf(fn, "userid: %s\n", currentuser.userid);
		fprintf(fn, "realname: %s\n", rname);
		fprintf(fn, "dept: %s\n", dept);
		fprintf(fn, "addr: %s\n", addr);
		fprintf(fn, "phone: %s\n", phone);
#ifdef NEED_ASSOC
		fprintf(fn, "assoc: %s\n", assoc);
#endif
		fprintf(fn, "----\n");
		fclose(fn);
	}
	setuserfile(genbuf, "mailcheck");
	if ((fn = fopen(genbuf, "w")) == NULL) {
		fclose(fn);
		return;
	}
	fprintf(fn, "usernum: %d\n", usernum);
	fclose(fn);
}
