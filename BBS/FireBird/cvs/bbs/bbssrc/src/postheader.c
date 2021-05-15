/*
$Id: postheader.c,v 1.4 2002/01/10 12:54:41 chinsan Exp $
*/

#include "bbs.h"

extern int numofsig;
struct shortfile *getbcache();

int
post_header(header)
struct postheader *header;
{
	int     anonyboard = 0;
	char    r_prompt[20], mybuf[256], ans[5];
	char    titlebuf[STRLEN];
	struct shortfile *bp;
#ifdef RNDSIGN
	int     oldset, rnd_sign = 0;
#endif

	if (currentuser.signature > numofsig || currentuser.signature < 0)
		currentuser.signature = 1;

#ifdef RNDSIGN
	if (numofsig > 0) {
		if (DEFINE(DEF_RANDSIGN)) {
			oldset = currentuser.signature;
			srand((unsigned) time(0));
			currentuser.signature = (rand() % numofsig) + 1;
			rnd_sign = 1;
		} else {
			rnd_sign = 0;
		}
	}
#endif

	if (header->reply_mode) {
		strcpy(titlebuf, header->title);
		header->include_mode = 'Y';
	} else
		titlebuf[0] = '\0';
	bp = getbcache(currboard);
	if (header->postboard)
		anonyboard = bp->flag & ANONY_FLAG;
#ifdef ANONYBOARD_POST_DEFAULT_ANONY
	header->chk_anony = (anonyboard) ? 1 : 0;
#else	
	header->chk_anony = 0 ;
#endif	
	while (1) {
		if (header->reply_mode)
			sprintf(r_prompt, "�ި��Ҧ� [[1m%c[m]", header->include_mode);
		move(t_lines - 4, 0);
		clrtobot();
		prints("[m%s [1m%s[m      %s\n",
			(header->postboard) ? "�o��峹��" : "���H�H�G", header->ds,
			(anonyboard) ? (header->chk_anony == 1 ? "[1m�n[m�ϥΰΦW" : "[1m��[m�ϥΰΦW") : "");
		prints("�ϥμ��D: [1m%-50s[m\n", (header->title[0] == '\0') ? "[���b�]�w�D�D]" : header->title);
#ifdef RNDSIGN
		prints("�ϥβ� [1m%d[m ��ñ�W��     %s %s", currentuser.signature
			,(header->reply_mode) ? r_prompt : "", (rnd_sign == 1) ? "[�H��ñ�W��]" : "");
#else
		prints("�ϥβ� [1m%d[m ��ñ�W��     %s", currentuser.signature
			,(header->reply_mode) ? r_prompt : "");
#endif
		if (titlebuf[0] == '\0') {
			move(t_lines - 1, 0);
			if (header->postboard == YEA || strcmp(header->title, "�S�D�D"))
				strcpy(titlebuf, header->title);
			getdata(t_lines - 1, 0, "���D: ", titlebuf, 50, DOECHO, NA);
			if (titlebuf[0] == '\0') {
				if (header->title[0] != '\0') {
					titlebuf[0] = ' ';
					continue;
				} else
					return NA;
			}
			strcpy(header->title, titlebuf);
			continue;
		}
		move(t_lines - 1, 0);
		sprintf(mybuf,
			"[1;32m0[m~[1;32m%d V[m ��/��ñ�W��%s [1;32mX[m �H��ñ�W�ɡA[1;32mT[m ����D%s�A[1;32mQ[m ���: ",
			numofsig, (header->reply_mode) ? "�A[1;32mY[m/[1;32mN[m/[1;32mR[m/[1;32mA[m ��ި��Ҧ�" : "", (anonyboard) ? "�A[1;32mS[m �ΦW" : "");
		getdata(t_lines - 1, 0, mybuf, ans, 3, DOECHO, YEA);
		ans[0] = toupper(ans[0]);
		if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9) {
			if (atoi(ans) <= numofsig)
				currentuser.signature = atoi(ans);
		} else if ( ans[0] == 'Q' || ans[0] == 'q' ) {
			return NA;
		} else if (header->reply_mode &&
			(ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A' || ans[0] == 'R')) {
			header->include_mode = ans[0];
		} else if (ans[0] == 'T') {
			titlebuf[0] = '\0';
		} else if (ans[0] == 'S' && anonyboard) {
			header->chk_anony = (header->chk_anony == 1) ? 0 : 1;
		} else if (ans[0] == 'V') {
			setuserfile(mybuf, "signatures");
			if (askyn("�w�]��ܫe�T��ñ�W��, �n��ܥ�����", NA, YEA) == YEA)
  				ansimore(mybuf);
			else {
				clear();
				ansimore2(mybuf, NA, 0, 18);
			}
#ifdef RNDSIGN
		} else if (ans[0] == 'X') {
			if (rnd_sign == 0 && numofsig != 0) {
				oldset = currentuser.signature;
				srand((unsigned) time(0));
				currentuser.signature = (rand() % numofsig) + 1;
				rnd_sign = 1;
			} else if (rnd_sign == 1 && numofsig != 0) {
				rnd_sign = 0;
				currentuser.signature = oldset;
			}
			ans[0] = ' ';
#endif
		} else {
			if (header->title[0] == '\0')
				return NA;
			else
				return YEA;
		}
	}
}

int 
check_anonyboard(currboard)
{
	struct shortfile *bp;
	int     anonyboard = 0;
	bp = getbcache(currboard);
	anonyboard = bp->flag & ANONY_FLAG;
	if (anonyboard == 8) {
		return YEA;
		//this is because anony_flag = 8 = 100 in binary
	} else {
		return NA;
	}

}
