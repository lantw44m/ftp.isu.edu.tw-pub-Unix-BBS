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
$Id: bm.c,v 1.3 2002/01/08 04:43:45 chinsan Exp $
*/

#include "bbs.h"
extern  cmpbnames();

int
addtodeny(uident)
char   *uident;
{
	char    buf[50], strtosave[STRLEN];
	setbfile(genbuf, currboard, "deny_users");
	if (seek_in_file(genbuf, uident))
		return -1;
	getdata(2, 0, "��J����: ", buf, 40, DOECHO, YEA);
	sprintf(strtosave, "%-12s %s", uident, buf);
	return addtofile(genbuf, strtosave);
}

int
deldeny(uident)
char   *uident;
{
	char    fn[STRLEN];
	setbfile(fn, currboard, "deny_users");
	return del_from_file(fn, uident);
}

int
deny_user()
{
	char    uident[STRLEN];
	char    ans[8], repbuf[STRLEN];
	int     count;
	if (!chk_currBM(currBM)) {
		return DONOTHING;
	}
	if (check_anonyboard(currboard) == YEA) {
		clear();
		prints("\n�ΦW�����D�����D�o��̨���, �G�������D�ӭ���ֵo��峹\n");
		prints("�]���]�w�L�k Post ���W�椧�v�����}�񵹪��D�ϥ�\n");
		prints("�Y���ݭn�]�w�즹�W��, �ЦV�q��sysop�B�z\n");
		pressreturn();
		if (!HAS_PERM(PERM_SYSOP)) {
			clear();
			return FULLUPDATE;
		}
	}
	while (1) {
		clear();
		prints("�]�w�L�k Post ���W��\n");
		setbfile(genbuf, currboard, "deny_users");
		count = listfilecontent(genbuf);
		if (count)
			getdata(1, 0, "(A)�W�[ (D)�R�� or (E)���} [E]: ", ans, 7, DOECHO, YEA);
		else
			getdata(1, 0, "(A)�W�[ or (E)���} [E]: ", ans, 7, DOECHO, YEA);
		if (*ans == 'A' || *ans == 'a') {
			move(1, 0);
			usercomplete("�W�[�L�k POST ���ϥΪ�: ", uident);
			if (*uident != '\0') {
				if (addtodeny(uident) == 1) {
					sprintf(repbuf, "���� %s �b %s �� POST �v�O",
						uident, currboard);
					securityreport(repbuf);
				}
			}
		} else if ((*ans == 'D' || *ans == 'd') && count) {
			move(1, 0);
			namecomplete("�R���L�k POST ���ϥΪ�: ", uident);
			move(1, 0);
			clrtoeol();
			if (uident[0] != '\0') {
				if (deldeny(uident)) {
					sprintf(repbuf, "��_ %s �b %s �� POST �v�O",
						uident, currboard);
					securityreport(repbuf);
				}
			}
		} else
			break;
	}
	clear();
	return FULLUPDATE;
}
