/* mail2bbs.h for mail2bbs configuring */
/* (c)Copyleft 2000 by Firebird BBS Project */
/* $Id: mail2bbs.h,v 1.3 2000/07/21 04:12:55 edwardc Exp $ */

#ifndef _MAIL2BBS_H_
#define _MAIL2BBS_H_

/* mail2bbs �S��\�� */

/* �����ͥ���h�H: �D���n����ĳ�ϥ�, �]���N�X�G�L�k debug .. */

#define DONT_RETURN_ANYTHING	

/* �N�H�U�����Ѳ����H�}�� mailing list to bbs �\�� */

//#define MAILLIST_MODULE			
//#define MAILLIST_RECEIVER	"maillist"		/* ���H�� XXXXXX.bbs@YOUR.BBS.HOST */
//#define MAILLIST_RETURNBRD "junk"			/* �h�H������ */

/* mail2bbs anti-spam �\�� (by: rexchen) */

#define RULE  3    /* rule 2 .... 10 (����ĳ�W�L 10 ) */
#define SPAM_SHMKEY		(31000L)
#define SPAMTABLE		2048

struct SPAM
{
	int spam_a;
	int spam_b;
	int spam_c;
	int spam_times;
};
                                
struct SPAM_MAIL
{
	struct SPAM mail_flag[SPAMTABLE];
	time_t update_time;
};
                                                
#endif/*_MAIL2BBS_H_*/
