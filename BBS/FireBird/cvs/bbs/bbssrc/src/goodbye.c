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
$Id: goodbye.c,v 1.1 2000/01/15 01:45:28 edwardc Exp $
*/

#include "bbs.h"

extern char BoardName[];
typedef struct {
	char   *match;
	char   *replace;
}
        logout;

int
countlogouts(filename)
char    filename[STRLEN];
{
	FILE   *fp;
	char    buf[256];
	int     count = 0;
	if ((fp = fopen(filename, "r")) == NULL)
		return 0;

	while (fgets(buf, 255, fp) != NULL) {
		if (strstr(buf, "@logout@") || strstr(buf, "@login@"))
			count++;
	}
	return count + 1;
}


user_display(filename, number, mode)
char   *filename;
int     number, mode;
{
	FILE   *fp;
	char    buf[256];
	int     count = 1;
	clear();
	move(1, 0);
	if ((fp = fopen(filename, "r")) == NULL)
		return;

	while (fgets(buf, 255, fp) != NULL) {
		if (strstr(buf, "@logout@") || strstr(buf, "@login@")) {
			count++;
			continue;
		}
		if (count == number) {
			if (mode == YEA)
				showstuff(buf, 0);
			else {
				prints("%s", buf);
			}
		} else if (count > number)
			break;
		else
			continue;
	}
	refresh();
	fclose(fp);
	return;
}


char   *
cexp(exp)
int     exp;
{
	int     expbase = 0;
	if (exp == -9999)
		return "�S����";
	if (exp <= 100 + expbase)
		return "�s��W��";
	if (exp > 100 + expbase && exp <= 450 + expbase)
		return "�@�미��";
	if (exp > 450 + expbase && exp <= 850 + expbase)
		return "���ů���";
	if (exp > 850 + expbase && exp <= 1500 + expbase)
		return "���ů���";
	if (exp > 1500 + expbase && exp <= 2500 + expbase)
		return "�ѯ���";
	if (exp > 2500 + expbase && exp <= 3000 + expbase)
		return "���ѯ�";
	if (exp > 3000 + expbase && exp <= 5000 + expbase)
		return "��������";
	if (exp > 5000 + expbase)
		return "�}��j��";

}

char   *
cperf(perf)
int     perf;
{

	if (perf == -9999)
		return "�S����";
	if (perf <= 5)
		return "���֥[�o";
	if (perf > 5 && perf <= 12)
		return "�V�O��";
	if (perf > 12 && perf <= 35)
		return "�٤���";
	if (perf > 35 && perf <= 50)
		return "�ܦn";
	if (perf > 50 && perf <= 90)
		return "�u����";
	if (perf > 90 && perf <= 140)
		return "���u�q�F";
	if (perf > 140 && perf <= 200)
		return "������W";
	if (perf > 200)
		return "�����";

}

int
countexp(udata)
struct userec *udata;
{
	int     exp;
	if (!strcmp(udata->userid, "guest"))
		return -9999;
	exp = udata->numposts +
		udata->numlogins / 5 +
		(time(0) - udata->firstlogin) / 86400 +
		udata->stay / 3600;
	return exp > 0 ? exp : 0;
}

int
countperf(udata)
struct userec *udata;
{
	int     perf;
	int     reg_days;
	if (!strcmp(udata->userid, "guest"))
		return -9999;
	reg_days = (time(0) - udata->firstlogin) / 86400 + 1;
	/*
	 * 990530.edwardc ���U�S���\���٦b���U���H���H�|�ɭP reg_days = 0,
	 * �M��b�U���|���� SIGFPE, ���Ƭ��s�����~ ..
	 */
	if (reg_days <= 0)
		return 0;

	perf = ((float) udata->numposts / (float) udata->numlogins +
		(float) udata->numlogins / (float) reg_days) * 10;
	return perf > 0 ? perf : 0;
}
showstuff(buf, limit)
char    buf[256];
int     limit;
{
	extern time_t login_start_time;
	int     frg, i, matchfrg, strlength, cnt, tmpnum;
	static char numlogins[10], numposts[10], nummails[10], rgtday[30], lasttime[30],
	        lastjustify[30], thistime[30], stay[10], alltime[20], ccperf[20],
	        perf[10], exp[10], ccexp[20], star[5];
	char    buf2[STRLEN], *ptr, *ptr2;
	time_t  now;

	static logout loglst[] =
	{
		"userid", currentuser.userid,
		"username", currentuser.username,
		"realname", currentuser.realname,
		"address", currentuser.address,
		"email", currentuser.email,
		"termtype", currentuser.termtype,
		"realemail", currentuser.reginfo,
		"ident", currentuser.ident,
		"rgtday", rgtday,
		"login", numlogins,
		"post", numposts,
		"mail", nummails,
		"lastlogin", lasttime,
		"lasthost", currentuser.lasthost,
		"lastjustify", lastjustify,
		"now", thistime,
		"bbsname", BoardName,
		"stay", stay,
		"alltime", alltime,
		"exp", exp,
		"cexp", ccexp,
		"perf", perf,
		"cperf", ccperf,
		"star", star,
		"pst", numposts,
		"log", numlogins,
		"bbsip", BBSIP,
		"bbshost", BBSHOST,
		NULL, NULL,
	};
	if (!strchr(buf, '$')) {
		if (!limit)
			prints("%s", buf);
		else
			prints("%.82s", buf);
		return;
	}
	now = time(0);
	/* for ansimore3() */

	if (currentuser.numlogins > 0) {
		tmpnum = countexp(&currentuser);
		sprintf(exp, "%d", tmpnum);
		strcpy(ccexp, cexp(tmpnum));
		tmpnum = countperf(&currentuser);
		sprintf(perf, "%d", tmpnum);
		strcpy(ccperf, cperf(tmpnum));
		sprintf(alltime, "%d�p��%d����", currentuser.stay / 3600, (currentuser.stay / 60) % 60);
		sprintf(rgtday, "%24.24s", ctime(&currentuser.firstlogin));
		sprintf(lasttime, "%24.24s", ctime(&currentuser.lastlogin));
		sprintf(thistime, "%24.24s", ctime(&now));
		sprintf(lastjustify, "%24.24s", ctime(&currentuser.lastjustify));
		sprintf(stay, "%d", (time(0) - login_start_time) / 60);
		sprintf(numlogins, "%d", currentuser.numlogins);
		sprintf(numposts, "%d", currentuser.numposts);
		sprintf(nummails, "%d", currentuser.nummails);
		sprintf(star, "%s", horoscope(currentuser.birthmonth, currentuser.birthday));
	}
	frg = 1;
	ptr2 = buf;
	do {
		if (ptr = strchr(ptr2, '$')) {
			matchfrg = 0;
			*ptr = '\0';
			prints("%s", ptr2);
			ptr += 1;
			for (i = 0; loglst[i].match != NULL; i++) {
				if (strstr(ptr, loglst[i].match) == ptr) {
					strlength = strlen(loglst[i].match);
					ptr2 = ptr + strlength;
					for (cnt = 0; *(ptr2 + cnt) == ' '; cnt++);
					sprintf(buf2, "%-*.*s", cnt ? strlength + cnt : strlength + 1, strlength + cnt, loglst[i].replace);
					prints("%s", buf2);
					ptr2 += (cnt ? (cnt - 1) : cnt);
					matchfrg = 1;
					break;
				}
			}
			if (!matchfrg) {
				prints("$");
				ptr2 = ptr;
			}
		} else {
			if (!limit)
				prints("%s", ptr2);
			else
				prints("%.82s", ptr2);
			frg = 0;
		}
	}
	while (frg);
	return;
}
