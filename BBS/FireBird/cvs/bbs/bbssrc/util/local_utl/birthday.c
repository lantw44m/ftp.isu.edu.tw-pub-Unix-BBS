/* $Id: birthday.c,v 1.7 2000/07/31 07:23:32 edwardc Exp $ */

#include <time.h>
#include <stdio.h>
#include "bbs.h"

main(argc, argv)
char   *argv[];
{
	FILE   *fp, *fout;
	time_t  now;
	int     i, j = 0, add;
	struct userec aman;
	struct tm *tmnow;
	char    buf[256];
	sprintf(buf, "%s/.PASSWDS", BBSHOME);
	if ((fp = fopen(buf, "rb")) == NULL) {
		printf("Can't open record data file.\n");
		return 1;
	}
	sprintf(buf, "%s/0Announce/bbslist/birthday", BBSHOME);
	if ((fout = fopen(buf, "w")) == NULL) {
		printf("Can't write to birthday file.\n");
		return 1;
	}
	now = time(0);
	now += 86400;		/* ��������Ѥ���ǰ�! +1 ���� */

	tmnow = localtime(&now);
	fprintf(fout, "\n%s����جP�W��\n\n", BBSNAME);
	fprintf(fout, "�H�U�O %d �� %d �骺�جP:\n\n", tmnow->tm_mon+1, tmnow->tm_mday);

	for (i = 0;; i++) {
		if (fread(&aman, sizeof(struct userec), 1, fp) <= 0)
			break;
			
		/* �H�U��  *�W�H* ���ݭn��b�� */
		if (!strcasecmp(aman.userid, "SYSOP") || !strcasecmp(aman.userid, "guest") )
			continue;
			
		if (aman.birthmonth == 0)
			continue;
		if (aman.birthmonth == tmnow->tm_mon + 1 &&
			aman.birthday == tmnow->tm_mday) {
			fprintf(fout, " ** %-15.15s (%s)\n", aman.userid, aman.username);
			j++;
		}
	}
	fprintf(fout, "\n\n�`�@�� %d ��جP�C\n", j);
	fclose(fout);
	fclose(fp);
}
