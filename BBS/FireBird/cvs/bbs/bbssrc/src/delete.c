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
$Id: delete.c,v 1.1 2000/01/15 01:45:28 edwardc Exp $
*/

#include "bbs.h"
#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif

int
offline()
{
	char    buf[STRLEN];
	modify_user_mode(OFFLINE);
	clear();
	if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_BOARDS) || HAS_PERM(PERM_ADMINMENU)
		|| HAS_PERM(PERM_SEEULEVELS)) {
		move(1, 0);
		prints("\n\n�z�������b��, �����H�K�۱���!!\n");
		pressreturn();
		clear();
		return;
	}
	if (currentuser.stay < 86400) {
		move(1, 0);
		prints("\n\n�藍�_, �z�٥��������榹�R�O!!\n");
		prints("�� mail �� SYSOP �����۱���], ���¡C\n");
		pressreturn();
		clear();
		return;
	}
	getdata(1, 0, "�п�J�A���K�X: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf)) {
		prints("\n\n�ܩ�p, �z��J���K�X�����T�C\n");
		pressreturn();
		clear();
		return;
	}
	getdata(3, 0, "�аݧA�s����W�r? ", buf, NAMELEN, DOECHO, YEA);
	if (*buf == '\0' || strcmp(buf, currentuser.realname)) {
		prints("\n\n�ܩ�p, �ڨä��{�ѧA�C\n");
		pressreturn();
		clear();
		return;
	}
	clear();
	move(1, 0);
	prints("[1;5;31mĵ�i[0;1;31m�G �۱���, �z�N�L�k�A�Φ��b���i�J�����I�I");
	prints("\n\n\n[1;32m���b���n�b 30 �ѫ�~�|�R���C�n���L�� :( .....[m\n\n\n");
	if (askyn("�A�T�w�n���}�o�Ӥj�a�x", NA, NA) == 1) {
		clear();
		currentuser.userlevel = 0;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec), usernum);
		mail_info();
		modify_user_mode(OFFLINE);
		kick_user(&uinfo);
		exit(0);
	}
}
getuinfo(fn)
FILE   *fn;
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n�L���N��     : %s\n", currentuser.userid);
	fprintf(fn, "�L���ʺ�     : %s\n", currentuser.username);
	fprintf(fn, "�u��m�W     : %s\n", currentuser.realname);
	fprintf(fn, "�~���}     : %s\n", currentuser.address);
	fprintf(fn, "�q�l�l��H�c : %s\n", currentuser.email);
	fprintf(fn, "�u�� E-mail  : %s\n", currentuser.reginfo);
	fprintf(fn, "Ident ���   : %s\n", currentuser.ident);
	fprintf(fn, "�b���إߤ�� : %s", ctime(&currentuser.firstlogin));
	fprintf(fn, "�̪���{��� : %s", ctime(&currentuser.lastlogin));
	fprintf(fn, "�̪���{���� : %s\n", currentuser.lasthost);
	fprintf(fn, "�W������     : %d ��\n", currentuser.numlogins);
	fprintf(fn, "�峹�ƥ�     : %d\n", currentuser.numposts);
	fprintf(fn, "�W���`�ɼ�   : %d �p�� %d ����\n",
		currentuser.stay / 3600, (currentuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(currentuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, "�ϥΪ��v��   : %s\n\n", buf);
}
mail_info()
{
	FILE   *fn;
	time_t  now;
	char    filename[STRLEN];
	now = time(0);
	sprintf(filename, "tmp/suicide.%s", currentuser.userid);
	if ((fn = fopen(filename, "w")) != NULL) {
		fprintf(fn, "[1m%s[m �w�g�b [1m%24.24s[m �n�O�۱��F�A�H�U�O�L����ơA�ЫO�d...", currentuser.userid
			,ctime(&now));
		getuinfo(fn);
		fclose(fn);
		postfile(filename, "syssecurity", "�n�O�۱��q��(30�ѫ�ͮ�)...", 2);
		unlink(filename);
	}
	if ((fn = fopen(filename, "w")) != NULL) {
		fprintf(fn, "�j�a�n,\n\n");
		fprintf(fn, "�ڬO %s (%s)�C�ڤv�g�n�O�b30�ѫ����}�o�̤F�C\n\n",
			currentuser.userid, currentuser.username);
		fprintf(fn, "�ڤ��|�󤣥i��ѰO�� %s�H�Ӧb���� %d �� login ���`�@ %d �����r�d�������I�I�w�w�C\n",
			ctime(&currentuser.firstlogin), currentuser.numlogins, currentuser.stay / 60);
		fprintf(fn, "�Чڪ��n�ͧ� %s �q�A�̪��n�ͦW�椤�����a�C�]���ڤv�g�S���v���A�W���F!\n\n",
			currentuser.userid);
		fprintf(fn, "�γ\\���¤@��ڷ|�^�Ӫ��C �í�!! �A��!!\n\n\n");
		fprintf(fn, "%s �� %24.24s �d.\n\n", currentuser.userid, ctime(&now));
		fclose(fn);
		postfile(filename, "notepad", "�n�O�۱��d��...", 2);
		unlink(filename);
	}
}
