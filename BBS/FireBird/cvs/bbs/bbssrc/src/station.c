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
$Id: station.c,v 1.3 2000/07/24 23:05:59 edwardc Exp $
*/

#include "bbs.h"
#include "chat.h"
#include <sys/ioctl.h>
#ifdef lint
#include <sys/uio.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#ifdef LINUX
#include <unistd.h>
#endif

#ifdef AIX
#include <sys/select.h>
#endif

#if !RELIABLE_SELECT_FOR_WRITE
#include <fcntl.h>
#endif

#if USES_SYS_SELECT_H
#include <sys/select.h>
#endif

#if NO_SETPGID
#define setpgid setpgrp
#endif

#ifndef L_XTND
#define L_XTND          2	/* relative to end of file */
#endif

#define RESTRICTED(u)   (users[(u)].flags == 0)	/* guest */
#define SYSOP(u)        (users[(u)].flags & PERM_SYSOP)
#define CLOAK(u)        (users[(u)].flags & PERM_CHATCLOAK)
#define ROOMOP(u)       (users[(u)].flags & PERM_CHATROOM)
#define OUTSIDER(u)     (users[(u)].flags & PERM_DENYPOST)

#define ROOM_LOCKED     0x1
#define ROOM_SECRET     0x2

#define LOCKED(r)       (rooms[(r)].flags & ROOM_LOCKED)
#define SECRET(r)       (rooms[(r)].flags & ROOM_SECRET)

#define ROOM_ALL        (-2)
#define PERM_CHATROOM PERM_CHAT

char   *CHATROOM_TOPIC[4] = {
	"���ѧڭ̭n�Q�ת��O...",
	"���ڤ@�M Kopi \"O\" �[�B...",
	"�ӪM���@���d���_�էa!",
"�S��s�s�E������l�F..."};

struct chatuser {
	int     sockfd;		/* socket to bbs server */
	int     utent;		/* utable entry for this user */
	int     room;		/* room: -1 means none, 0 means main */
	int     flags;
	char    cloak;
	char    userid[IDLEN + 2];	/* real userid */
	char    chatid[CHAT_IDLEN];	/* chat id */
	char    ibuf[128];	/* buffer for sending/receiving */
	int     ibufsize;	/* current size of ibuf */
	char    host[30];
}       users[MAXACTIVE];


struct chatroom {
	char    name[IDLEN];	/* name of room; room 0 is "main" */
	short   occupants;	/* number of users in room */
	short   flags;		/* ROOM_LOCKED, ROOM_SECRET */
	char    invites[MAXACTIVE];	/* Keep track of invites to rooms */
	char    topic[48];	/* Let the room op to define room topic */
}       rooms[MAXROOM];

struct chatcmd {
	char   *cmdstr;
	void    (*cmdfunc) ();
	int     exact;
};


int     chatroom, chatport;
int     sock = -1;		/* the socket for listening */
int     nfds;			/* number of sockets to select on */
int     num_conns;		/* current number of connections */
fd_set  allfds;			/* fd set for selecting */
struct timeval zerotv;		/* timeval for selecting */
char    chatbuf[256];		/* general purpose buffer */
char    chatname[19];
/* name of the main room (always exists) */

char    mainroom[] = "main";
char   *maintopic = "���ѡA�ڭ̭n�Q�ת��O.....";

char   *msg_not_op = "[1;37m��[32m�z���O�o�[�Ъ��Ѥj[37m ��[m";
char   *msg_no_such_id = "[1;37m��[32m [[36m%s[32m] ���b�o���[�и�[37m ��[m";
char   *msg_not_here = "[1;37m�� [32m[[36m%s[32m] �èS���e�ӥ��|ĳ�U[37m ��[m";


#define HAVE_REPORT

#ifdef  HAVE_REPORT
report(s)
char   *s;
{
	static int disable = NA;

	if (disable)
		return;
		
	file_append("trace.chatd", s);

	disable = YEA;
	return;
}
#else
#define report(s)       ;
#endif


is_valid_chatid(id)
char   *id;
{
	int     i;
	if (*id == '\0')
		return 0;

	for (i = 0; i < CHAT_IDLEN && *id; i++, id++)
		if (strchr(BADCIDCHARS, *id))
			return 0;

	return 1;
}

int
Isspace(ch)
char    ch;
{
	return (int) strchr(" \t\n\r", ch);
}

char   *
nextword(str)
char  **str;
{
	char   *head, *tail;
	int     ch;
	head = *str;
	for (;;) {
		ch = *head;
		if (!ch) {
			*str = head;
			return head;
		}
		if (!Isspace(ch))
			break;
		head++;
	}

	tail = head + 1;
	while (ch = *tail) {
		if (Isspace(ch)) {
			*tail++ = '\0';
			break;
		}
		tail++;
	}
	*str = tail;

	return head;
}

int
chatid_to_indx(unum, chatid)
int     unum;
char   *chatid;
{
	register int i;
	for (i = 0; i < MAXACTIVE; i++) {
		if (users[i].sockfd == -1)
			continue;
		if (!strcasecmp(chatid, users[i].chatid)) {
			if (users[i].cloak == 0 || !CLOAK(unum))
				return i;
		}
	}
	return -1;
}


int
fuzzy_chatid_to_indx(unum, chatid)
int     unum;
char   *chatid;
{
	register int i, indx = -1;
	unsigned int len = strlen(chatid);
	for (i = 0; i < MAXACTIVE; i++) {
		if (users[i].sockfd == -1)
			continue;
		if (!strncasecmp(chatid, users[i].chatid, len) ||
			!strncasecmp(chatid, users[i].userid, len)) {
			if (len == strlen(users[i].chatid) || len == strlen(users[i].userid)) {
				indx = i;
				break;
			}
			if (indx == -1)
				indx = i;
			else
				return -2;
		}
	}
	if (users[indx].cloak == 0 || CLOAK(unum))
		return indx;
	else
		return -1;
}


int
roomid_to_indx(roomid)
char   *roomid;
{
	register int i;
	for (i = 0; i < MAXROOM; i++) {
		if (i && rooms[i].occupants == 0)
			continue;
		report(roomid);
		report(rooms[i].name);
		if (!strcasecmp(roomid, rooms[i].name))
			return i;
	}
	return -1;
}


void
do_send(writefds, str)
fd_set *writefds;
char   *str;
{
	register int i;
	int     len = strlen(str);
	if (select(nfds, NULL, writefds, NULL, &zerotv) > 0) {
		for (i = 0; i < nfds; i++)
			if (FD_ISSET(i, writefds))
				send(i, str, len + 1, 0);
	}
}


void
send_to_room(room, str)
int     room;
char   *str;
{
	int     i;
	fd_set  writefds;
	FD_ZERO(&writefds);
	for (i = 0; i < MAXROOM; i++) {
		if (users[i].sockfd == -1)
			continue;
		if (room == ROOM_ALL || room == users[i].room)
			/* write(users[i].sockfd, str, strlen(str) + 1); */
			FD_SET(users[i].sockfd, &writefds);
	}
	do_send(&writefds, str);
}


void
send_to_unum(unum, str)
int     unum;
char   *str;
{
	fd_set  writefds;
	FD_ZERO(&writefds);
	FD_SET(users[unum].sockfd, &writefds);
	do_send(&writefds, str);
}


void
exit_room(unum, disp, msg)
int     unum;
int     disp;
char   *msg;
{
	int     oldrnum = users[unum].room;
	if (oldrnum != -1) {
		if (--rooms[oldrnum].occupants) {
			switch (disp) {
			case EXIT_LOGOUT:
				sprintf(chatbuf, "[1;37m�� [32m[[36m%s[32m] �C�C���}�F [37m��[m", users[unum].chatid);
				if (msg && *msg) {
					strcat(chatbuf, ": ");
					strncat(chatbuf, msg, 80);
				}
				break;

			case EXIT_LOSTCONN:
				sprintf(chatbuf, "[1;37m�� [32m[[36m%s[32m] ���_�F�u������ ... [37m��[m", users[unum].chatid);
				break;

			case EXIT_KICK:
				sprintf(chatbuf, "[1;37m�� [32m[[36m%s[32m] �Q�Ѥj���X�h�F [37m��[m", users[unum].chatid);
				break;
			}
			if (users[unum].cloak == 0)
				send_to_room(oldrnum, chatbuf);
		}
	}
	users[unum].flags &= ~PERM_CHATROOM;
	users[unum].room = -1;
}


void
chat_topic(unum, msg)
int     unum;
char   *msg;
{
	int     rnum;
	rnum = users[unum].room;

	if (!ROOMOP(unum) && !SYSOP(unum)) {
		send_to_unum(unum, msg_not_op);
		return;
	}
	if (*msg == '\0') {
		send_to_unum(unum, "[1;31m�� [37m�Ы��w���D [31m��[m");
		return;
	}
	strncpy(rooms[rnum].topic, msg, 48);
	rooms[rnum].topic[47] = '\0';
	sprintf(chatbuf, "/t%.47s", msg);
	send_to_room(rnum, chatbuf);
	sprintf(chatbuf, "[1;37m�� [32m[[36m%s[32m] �N���D�אּ [1;33m%s [37m��[m", users[unum].chatid, msg);
	send_to_room(rnum, chatbuf);
}


enter_room(unum, room, msg)
int     unum;
char   *room;
char   *msg;
{
	int     rnum;
	int     op = 0;
	register int i;
	rnum = roomid_to_indx(room);
	if (rnum == -1) {
		/* new room */
		if (OUTSIDER(unum)) {
			send_to_unum(unum, "[1;31m�� [37m��p�A�z����}�s�]�[ [31m��[m");
			return 0;
		}
		for (i = 1; i < MAXROOM; i++) {
			if (rooms[i].occupants == 0) {
				report("new room");
				rnum = i;
				memset(rooms[rnum].invites, 0, MAXACTIVE);
				strcpy(rooms[rnum].topic, maintopic);
				strncpy(rooms[rnum].name, room, IDLEN - 1);
				rooms[rnum].name[IDLEN - 1] = '\0';
				rooms[rnum].flags = 0;
				op++;
				break;
			}
		}
		if (rnum == -1) {
			send_to_unum(unum, "[1;31m�� [37m�ڭ̪��ж����F�� [31m��[m");
			return 0;
		}
	}
	if (!SYSOP(unum))
		if (LOCKED(rnum) && rooms[rnum].invites[unum] == 0) {
			send_to_unum(unum, "[1;31m�� [37m���аӰQ���K���A�D�ФŤJ [31m��[m");
			return 0;
		}
	exit_room(unum, EXIT_LOGOUT, msg);
	users[unum].room = rnum;
	if (op)
		users[unum].flags |= PERM_CHATROOM;
	rooms[rnum].occupants++;
	rooms[rnum].invites[unum] = 0;
	if (users[unum].cloak == 0) {
		sprintf(chatbuf, "[1;31m�� [37m[[36;1m%s[37m] �i�J [35m%s [37m�]�[ [31m��[m",
			users[unum].chatid, rooms[rnum].name);
		send_to_room(rnum, chatbuf);
	}
	sprintf(chatbuf, "/r%s", room);
	send_to_unum(unum, chatbuf);
	sprintf(chatbuf, "/t%s", rooms[rnum].topic);
	send_to_unum(unum, chatbuf);
	return 0;
}


void
logout_user(unum)
int     unum;
{
	int     i, sockfd = users[unum].sockfd;
	close(sockfd);
	FD_CLR(sockfd, &allfds);
	memset(&users[unum], 0, sizeof(users[unum]));
	users[unum].sockfd = users[unum].utent = users[unum].room = -1;
	for (i = 0; i < MAXROOM; i++) {
		if (rooms[i].invites != NULL)
			rooms[i].invites[unum] = 0;
	}
	num_conns--;
}
print_user_counts(unum)
int     unum;
{
	int     i, c, userc = 0, suserc = 0, roomc = 0;
	for (i = 0; i < MAXROOM; i++) {
		c = rooms[i].occupants;
		if (i > 0 && c > 0) {
			if (!SECRET(i) || SYSOP(unum))
				roomc++;
		}
		c = users[i].room;
		if (users[i].sockfd != -1 && c != -1 && users[i].cloak == 0) {
			if (SECRET(c) && !SYSOP(unum))
				suserc++;
			else
				userc++;
		}
	}
	sprintf(chatbuf, "[1;31m��[37m �w����{�y[32m%s[37m�z���i[36m%s[37m�j[31m��[m", MY_BBS_NAME, chatname);
	send_to_unum(unum, chatbuf);
	sprintf(chatbuf,
		"[1;31m��[37m �ثe�w�g�� [1;33m%d [37m���|ĳ�Ǧ��ȤH [31m��[m",
		roomc + 1);
	send_to_unum(unum, chatbuf);
	sprintf(chatbuf, "[1;31m�� [37m���|ĳ�U���@�� [36m%d[37m �H ", userc + 1);
	if (suserc)
		sprintf(chatbuf + strlen(chatbuf), "[[36m%d[37m �H�b�����K�Q�׫�]", suserc);
	sprintf(chatbuf + strlen(chatbuf), "[31m��[m");
	send_to_unum(unum, chatbuf);
	return 0;
}
login_user(unum, msg)
int     unum;
char   *msg;
{
	int     i, utent;	/* , fd = users[unum].sockfd; */
	char   *utentstr;
	char   *level;
	char   *userid;
	char   *chatid;
	char   *cloak;
	utentstr = nextword(&msg);
	level = nextword(&msg);
	userid = nextword(&msg);
	chatid = nextword(&msg);
	cloak = nextword(&msg);

	utent = atoi(utentstr);
	for (i = 0; i < MAXACTIVE; i++) {
		if (users[i].sockfd != -1 && users[i].utent == utent) {
			send_to_unum(unum, CHAT_LOGIN_BOGUS);
			return -1;
		}
	}
	if (!is_valid_chatid(chatid)) {
		send_to_unum(unum, CHAT_LOGIN_INVALID);
		return 0;
	}
	if (chatid_to_indx(0, chatid) != -1) {
		/* userid in use */
		send_to_unum(unum, CHAT_LOGIN_EXISTS);
		return 0;
	}
	if (!strncasecmp("localhost" /* MY_BBS_DOMAIN */ , users[unum].host, 30)) {
		users[unum].flags = atoi(level) & ~(PERM_DENYPOST);
		users[unum].cloak = atoi(cloak);
	} else {
		if (chatport != CHATPORT1) {	/* only CHAT1 allows remote
						 * user */
			send_to_unum(unum, CHAT_LOGIN_BOGUS);
			return -1;
		} else {
			if (!(atoi(level) & PERM_LOGINOK) && !strncasecmp(chatid, "guest", 8)) {
				send_to_unum(unum, CHAT_LOGIN_INVALID);
				return 0;
			}
			users[unum].flags = PERM_DENYPOST;
			users[unum].cloak = 0;
		}
	}
	report(level);
	report(users[unum].host);

	users[unum].utent = utent;
	strcpy(users[unum].userid, userid);
	strncpy(users[unum].chatid, chatid, CHAT_IDLEN - 1);
	users[unum].chatid[CHAT_IDLEN] = '\0';
	send_to_unum(unum, CHAT_LOGIN_OK);
	print_user_counts(unum);
	enter_room(unum, mainroom, (char *) NULL);
	return 0;
}

void
chat_list_rooms(unum, msg)
int     unum;
char   *msg;
{
	int     i, occupants;
	if (RESTRICTED(unum)) {
		send_to_unum(unum, "[1;31m�� [37m��p�I�Ѥj�����A�ݦ����ǩж����ȤH [31m��[m");
		return;
	}
	send_to_unum(unum, "[1;33;44m �ͤѫǦW��  �x�H�Ƣx���D        [m");
	for (i = 0; i < MAXROOM; i++) {
		occupants = rooms[i].occupants;
		if (occupants > 0) {
			if (!SYSOP(unum))
				if ((rooms[i].flags & ROOM_SECRET) && (users[unum].room != i))
					continue;
			sprintf(chatbuf, " [1;32m%-12s[37m�x[36m%4d[37m�x[33m%s[m", rooms[i].name, occupants, rooms[i].topic);
			if (rooms[i].flags & ROOM_LOCKED)
				strcat(chatbuf, " [���]");
			if (rooms[i].flags & ROOM_SECRET)
				strcat(chatbuf, " [���K]");
			send_to_unum(unum, chatbuf);
		}
	}
}


chat_do_user_list(unum, msg, whichroom)
int     unum;
char   *msg;
int     whichroom;
{
	int     start, stop, curr = 0;
	int     i, rnum, myroom = users[unum].room;
	while (*msg && Isspace(*msg))
		msg++;
	start = atoi(msg);
	while (*msg && isdigit(*msg))
		msg++;
	while (*msg && !isdigit(*msg))
		msg++;
	stop = atoi(msg);
	send_to_unum(unum, "[1;33;44m ��ѥN���x�ϥΪ̥N��  �x��    ��    �ǡ�Op���Ӧ�                          [m");
	for (i = 0; i < MAXROOM; i++) {
		rnum = users[i].room;
		if (users[i].sockfd != -1 && rnum != -1 && !(users[i].cloak == 1 && !CLOAK(unum))) {
			if (whichroom != -1 && whichroom != rnum)
				continue;
			if (myroom != rnum) {
				if (RESTRICTED(unum))
					continue;
				if ((rooms[rnum].flags & ROOM_SECRET) && !SYSOP(unum))
					continue;
			}
			curr++;
			if (curr < start)
				continue;
			else if (stop && (curr > stop))
				break;
			sprintf(chatbuf, "[1;5m%c[0;1;37m%-8s�x[31m%s%-12s[37m�x[32m%-14s[37m��[34m%-2s[37m��[35m%-30s[m",
				(users[i].cloak == 1) ? 'C' : ' ', users[i].chatid, OUTSIDER(i) ? "[1;35m" : "[1;36m",
				users[i].userid, rooms[rnum].name, ROOMOP(i) ? "�O" : "�_", users[i].host);
			send_to_unum(unum, chatbuf);
		}
	}
	return 0;
}


void
chat_list_by_room(unum, msg)
int     unum;
char   *msg;
{
	int     whichroom;
	char   *roomstr;
	roomstr = nextword(&msg);
	if (*roomstr == '\0')
		whichroom = users[unum].room;
	else {
		if ((whichroom = roomid_to_indx(roomstr)) == -1) {
			sprintf(chatbuf, "[1;31m�� [37m�S %s �o�өж��� [31m��[m", roomstr);
			send_to_unum(unum, chatbuf);
			return;
		}
		if ((rooms[whichroom].flags & ROOM_SECRET) && !SYSOP(unum)) {
			send_to_unum(unum, "[1;31m�� [37m���|ĳ�U���ж��Ҥ��}���A�S�����K [31m��[m");
			return;
		}
	}
	chat_do_user_list(unum, msg, whichroom);
}


void
chat_list_users(unum, msg)
int     unum;
char   *msg;
{
	chat_do_user_list(unum, msg, -1);
}


void
chat_map_chatids(unum, whichroom)
int     unum;
int     whichroom;
{
	int     i, c, myroom, rnum;
	myroom = users[unum].room;
	send_to_unum(unum,
		"[1;33;44m ��ѥN�� �ϥΪ̥N��  �x ��ѥN�� �ϥΪ̥N��  �x ��ѥN�� �ϥΪ̥N�� [m");
	for (i = 0, c = 0; i < MAXROOM; i++) {
		rnum = users[i].room;
		if (users[i].sockfd != -1 && rnum != -1 && !(users[i].cloak == 1 && !CLOAK(unum))) {
			if (whichroom != -1 && whichroom != rnum)
				continue;
			if (myroom != rnum) {
				if (RESTRICTED(unum))
					continue;
				if ((rooms[rnum].flags & ROOM_SECRET) && !SYSOP(unum))
					continue;
			}
			sprintf(chatbuf + (c * 50), "[1;34;5m%c[m[1m%-8s%c%s%-12s%s[m", (users[i].cloak == 1) ? 'C' : ' ',
				users[i].chatid, (ROOMOP(i)) ? '*' : ' ', OUTSIDER(i) ? "[1;35m" : "[1;36m", users[i].userid,
				(c < 2 ? "�x" : "  "));
			if (++c == 3) {
				send_to_unum(unum, chatbuf);
				c = 0;
			}
		}
	}
	if (c > 0)
		send_to_unum(unum, chatbuf);
}


void
chat_map_chatids_thisroom(unum, msg)
int     unum;
char   *msg;
{
	chat_map_chatids(unum, users[unum].room);
}


void
chat_setroom(unum, msg)
int     unum;
char   *msg;
{
	char   *modestr;
	int     rnum = users[unum].room;
	int     sign = 1;
	int     flag;
	char   *fstr;
	modestr = nextword(&msg);
	if (!ROOMOP(unum) && !SYSOP(unum)) {
		send_to_unum(unum, msg_not_op);
		return;
	}
	if (*modestr == '+')
		modestr++;
	else if (*modestr == '-') {
		modestr++;
		sign = 0;
	}
	if (*modestr == '\0') {
		send_to_unum(unum,
			"[1;31m�� [37m�Чi�D�d�x�z�n���ж��O: {[[32m+[37m(�]�w)][[32m-[37m(����)]}{[[32ml[37m(���)][[32ms[37m(���K)]}[m");
		return;
	}
	while (*modestr) {
		flag = 0;
		switch (*modestr) {
		case 'l':
		case 'L':
			flag = ROOM_LOCKED;
			fstr = "���";
			break;
		case 's':
		case 'S':
			flag = ROOM_SECRET;
			fstr = "���K";
			break;
		default:
			sprintf(chatbuf, "[1;31m�� [37m��p�A�ݤ����A���N��G[[36m%c[37m] [31m��[m", *modestr);
			send_to_unum(unum, chatbuf);
			return;
		}
		if (flag && ((rooms[rnum].flags & flag) != sign * flag)) {
			rooms[rnum].flags ^= flag;
			sprintf(chatbuf, "[1;37m��[32m �o�ж��Q %s %s%s���Φ� [37m��[m",
				users[unum].chatid, sign ? "�]�w��" : "����", fstr);
			send_to_room(rnum, chatbuf);
		}
		modestr++;
	}
}


void
chat_nick(unum, msg)
int     unum;
char   *msg;
{
	char   *chatid;
	int     othernum;
	chatid = nextword(&msg);
	chatid[CHAT_IDLEN - 1] = '\0';
	if (!is_valid_chatid(chatid)) {
		send_to_unum(unum, "[1;31m�� [37m�z���W�r�O���O�g���F[31m ��[m");
		return;
	}
	othernum = chatid_to_indx(0, chatid);
	if (othernum != -1 && othernum != unum) {
		send_to_unum(unum, "[1;31m�� [37m��p�I���H��A�P�W�A�ҥH�A����i�� [31m��[m");
		return;
	}
	sprintf(chatbuf, "[1;31m�� [36m%s [0;37m�w�g��W�� [1;33m%s [31m��[m",
		users[unum].chatid, chatid);
	send_to_room(users[unum].room, chatbuf);
	strcpy(users[unum].chatid, chatid);
	sprintf(chatbuf, "/n%s", users[unum].chatid);
	send_to_unum(unum, chatbuf);
}


void
chat_private(unum, msg)
int     unum;
char   *msg;
{
	char   *recipient;
	int     recunum;
	recipient = nextword(&msg);
	recunum = fuzzy_chatid_to_indx(unum, recipient);
	if (recunum < 0) {
		/* no such user, or ambiguous */
		if (recunum == -1)
			sprintf(chatbuf, msg_no_such_id, recipient);
		else
			sprintf(chatbuf, "[1;31m ��[37m ����ѻP�̥s����W�r [31m��[m");
		send_to_unum(unum, chatbuf);
		return;
	}
	if (*msg) {
		sprintf(chatbuf, "[1;32m �� [36m%s [37m�ǯȱ��p���ѨӨ�[m: ", users[unum].chatid);
		strncat(chatbuf, msg, 80);
		send_to_unum(recunum, chatbuf);
		sprintf(chatbuf, "[1;32m �� [37m�ȱ��w�g�浹�F [36m%s[m: ", users[recunum].chatid);
		strncat(chatbuf, msg, 80);
		send_to_unum(unum, chatbuf);
	} else {
		sprintf(chatbuf, "[1;31m ��[37m �A�n���軡�Ǥ���r�H[31m��[m");
		send_to_unum(unum, chatbuf);
	}
}


put_chatid(unum, str)
int     unum;
char   *str;
{
	int     i;
	char   *chatid = users[unum].chatid;
	memset(str, ' ', 10);
	for (i = 0; chatid[i]; i++)
		str[i] = chatid[i];
	str[i] = ':';
	str[10] = '\0';
}


chat_allmsg(unum, msg)
int     unum;
char   *msg;
{
	if (*msg) {
		put_chatid(unum, chatbuf);
		strcat(chatbuf, msg);
		send_to_room(users[unum].room, chatbuf);
	}
	return 0;
}


void
chat_act(unum, msg)
int     unum;
char   *msg;
{
	if (*msg) {
		sprintf(chatbuf, "[1;36m%s [37m%s[m", users[unum].chatid, msg);
		send_to_room(users[unum].room, chatbuf);
	}
}

void
chat_cloak(unum, msg)
int     unum;
char   *msg;
{
	if (CLOAK(unum)) {
		if (users[unum].cloak == 1)
			users[unum].cloak = 0;
		else
			users[unum].cloak = 1;
		sprintf(chatbuf, "[1;36m%s [37m%s �������A...[m", users[unum].chatid, (users[unum].cloak == 1) ? "�i�J" : "����");
		send_to_unum(unum, chatbuf);
	}
}

void
chat_join(unum, msg)
int     unum;
char   *msg;
{
	char   *roomid;
	roomid = nextword(&msg);
	if (RESTRICTED(unum)) {
		send_to_unum(unum, "[1;31m�� [37m�A�u��b�o�̲�� [31m��[m");
		return;
	}
	if (*roomid == '\0') {
		send_to_unum(unum, "[1;31m�� [37m�аݭ��өж� [31m��[m");
		return;
	}
	enter_room(unum, roomid, msg);
	return;
}


void
chat_kick(unum, msg)
int     unum;
char   *msg;
{
	char   *twit;
	int     rnum = users[unum].room;
	int     recunum;
	twit = nextword(&msg);
	if (!ROOMOP(unum) && !SYSOP(unum)) {
		send_to_unum(unum, msg_not_op);
		return;
	}
	if ((recunum = chatid_to_indx(unum, twit)) == -1) {
		sprintf(chatbuf, msg_no_such_id, twit);
		send_to_unum(unum, chatbuf);
		return;
	}
	if (rnum != users[recunum].room) {
		sprintf(chatbuf, msg_not_here, users[recunum].chatid);
		send_to_unum(unum, chatbuf);
		return;
	}
	exit_room(recunum, EXIT_KICK, (char *) NULL);

	if (rnum == 0)
		logout_user(recunum);
	else
		enter_room(recunum, mainroom, (char *) NULL);
}


void
chat_makeop(unum, msg)
int     unum;
char   *msg;
{
	char   *newop = nextword(&msg);
	int     rnum = users[unum].room;
	int     recunum;
	if (!ROOMOP(unum) && !SYSOP(unum)) {
		send_to_unum(unum, msg_not_op);
		return;
	}
	if ((recunum = chatid_to_indx(unum, newop)) == -1) {
		/* no such user */
		sprintf(chatbuf, msg_no_such_id, newop);
		send_to_unum(unum, chatbuf);
		return;
	}
	if (unum == recunum) {
		sprintf(chatbuf, "[1;37m�� [32m�A�ѤF�A���ӴN�O�Ѥj�� [37m��[m");
		send_to_unum(unum, chatbuf);
		return;
	}
	if (rnum != users[recunum].room) {
		sprintf(chatbuf, msg_not_here, users[recunum].chatid);
		send_to_unum(unum, chatbuf);
		return;
	}
	users[unum].flags &= ~PERM_CHATROOM;
	users[recunum].flags |= PERM_CHATROOM;
	sprintf(chatbuf, "[1;37m�� [36m %s[32m�M�w�� [35m%s [32m��Ѥj [37m��[m", users[unum].chatid,
		users[recunum].chatid);
	send_to_room(rnum, chatbuf);
}


void
chat_invite(unum, msg)
int     unum;
char   *msg;
{
	char   *invitee = nextword(&msg);
	int     rnum = users[unum].room;
	int     recunum;
	if (!ROOMOP(unum) && !SYSOP(unum)) {
		send_to_unum(unum, msg_not_op);
		return;
	}
	if ((recunum = chatid_to_indx(unum, invitee)) == -1) {
		sprintf(chatbuf, msg_not_here, invitee);
		send_to_unum(unum, chatbuf);
		return;
	}
	if (rooms[rnum].invites[recunum] == 1) {
		sprintf(chatbuf, "[1;37m�� [36m%s [32m���@�U�N�� [37m��[m", users[recunum].chatid);
		send_to_unum(unum, chatbuf);
		return;
	}
	rooms[rnum].invites[recunum] = 1;
	sprintf(chatbuf, "[1;37m�� [36m%s [32m�ܽбz�� [[33m%s[32m] �ж����[37m ��[m",
		users[unum].chatid, rooms[rnum].name);
	send_to_unum(recunum, chatbuf);
	sprintf(chatbuf, "[1;37m�� [36m%s [32m���@�U�N�� [37m��[m", users[recunum].chatid);
	send_to_unum(unum, chatbuf);
}


void
chat_broadcast(unum, msg)
int     unum;
char   *msg;
{
	if (!SYSOP(unum)) {
		send_to_unum(unum, "[1;31m�� [37m�A���i�H�b�|ĳ�U���j�n�ټM [31m��[m");
		return;
	}
	if (*msg == '\0') {
		send_to_unum(unum, "[1;37m�� [32m�s�����e�O���� [37m��[m");
		return;
	}
	sprintf(chatbuf, "[1m Ding Dong!! �ǹF�ǳ��i�G [36m%s[37m ���ܹ�j�a�ŧG�G[m",

		users[unum].chatid);
	send_to_room(ROOM_ALL, chatbuf);
	sprintf(chatbuf, "[1;34m�i[33m%s[34m�j[m", msg);
	send_to_room(ROOM_ALL, chatbuf);
}


void
chat_goodbye(unum, msg)
int     unum;
char   *msg;
{
	exit_room(unum, EXIT_LOGOUT, msg);
}


/* -------------------------------------------- */
/* MUD-like social commands : action             */
/* -------------------------------------------- */


struct action {
	char   *verb;		/* �ʵ� */
	char   *part1_msg;	/* ���� */
	char   *part2_msg;	/* �ʧ@ */
};


struct action party_data[] =
{
	{"aluba", "��", "��h���|��"},
	{"bearhug", "�������֩�", ""},
	{"bless", "����", "�߷Q�Ʀ�"},
	{"boli", "��", "��h���N���F"},
	{"bow", "���`���q���V", "���`"},
	{"caress", "���N", ""},
	{"cringe", "�V", "���`�}���A�n���^��"},
	{"cry", "�V", "�z�ޤj��"},
	{"comfort", "�Ũ��w��", ""},
	{"clap", "�V", "���P���x"},
	{"dance", "�ԤF", "���⽡���_�R"},
	{"dogleg", "��", "���L"},
	{"drivel", "���", "�y�f��"},
	{"farewell", "�t�\\�̨̤��˦a�V", "�D�O"},
	{"giveme5", "��", "�ӭӼ�����Give Me Five�I"},
	{"giggle", "���", "�̶̪��b��"},
	{"grin", "��", "�S�X���c�����e"},
	{"growl", "��", "�H�����w"},
	{"hand", "��", "����"},
	{"hehe", "��", "��h���K�K�F"},
	{"hug", "�����a�֩�", ""},
	{"kick", "��", "�𪺦��h����"},
	{"kiss", "���k", "���y�U"},
	{"laugh", "�j�n�J��", ""},
	{"like", "�̳��w", "�F"},
	{"look", "�C�j�����ݵ�", ""},
	{"nod", "�V", "�I�Y�٬O"},
	{"nudge", "�Τ�y��", "���Ψ{�l"},
	{"pad", "����", "���ӻH"},
	{"pinch", "�ΤO����", "�����«C"},
	{"punch", "�����~�F", "�@�y"},
	{"shrug", "�L�`�a�V", "�q�F�q�ӻH"},
	{"sigh", "��", "�ۤF�@�f��"},
	{"slap", "�԰Ԫ��ڤF", "�@�y�ե�"},
	{"smooch", "�֧k��", ""},
	{"snicker", "�K�K�K..����", "�ѯ�"},
	{"sniff", "��", "�ᤧ�H��"},
	{"spank", "�Τڴx��", "���v��"},
	{"squeeze", "���a�֩��", ""},
	{"tempt", "�Ω�����ۻ�աA�@�y���h���ݵ�", ""},
	{"thank", "�V", "�D��"},
	{"tickle", "�B�T!�B�T!�k", "���o"},
	{"wall", "�N", "��h����A�{�b���H�b����W�A�T��������Q�����F :P"},
	{"wave", "���", "���R���n��"},
	{"welcome", "���P�w��", "�����"},
	{"wink", "��", "�������w�w����"},
	{"zap", "��", "�ƨg������"},
	{NULL, NULL, NULL}
};


int
party_action(unum, cmd, party)
int     unum;
char   *cmd;
char   *party;
{
	int     i;
	for (i = 0; party_data[i].verb; i++) {
		if (!strcmp(cmd, party_data[i].verb)) {
			if (*party == '\0') {
				party = "�j�a";
			} else {
				int     recunum = fuzzy_chatid_to_indx(unum, party);
				if (recunum < 0) {
					/* no such user, or ambiguous */
					if (recunum == -1)
						sprintf(chatbuf, msg_no_such_id, party);
					else
						sprintf(chatbuf, "[1;31m�� [37m�аݭ����ж� [31m��[m");
					send_to_unum(unum, chatbuf);
					return 0;
				}
				party = users[recunum].chatid;
			}
			sprintf(chatbuf, "[1;36m%s [32m%s[33m %s [32m%s[37;0m",
				users[unum].chatid,
				party_data[i].part1_msg, party, party_data[i].part2_msg);
			send_to_room(users[unum].room, chatbuf);
			return 0;
		}
	}
	return 1;
}


/* -------------------------------------------- */
/* MUD-like social commands : speak              */
/* -------------------------------------------- */


struct action speak_data[] =
{
	{"ask", "�߰�", NULL},
	{"chant", "�q�|", NULL},
	{"charm", "�I�G", NULL},
	{"cheer", "�ܪ�", NULL},
	{"chuckle", "����", NULL},
	{"curse", "�G�|", NULL},
	{"demand", "�n�D", NULL},
	{"frown", "�٬�", NULL},
	{"groan", "�D�u", NULL},
	{"grumble", "�o�c��", NULL},
	{"guitar", "��u�ۦN�L�A��۵�", NULL},
	{"hum", "���ۻy", NULL},
	{"moan", "�d��", NULL},
	{"notice", "�`�N", NULL},
	{"order", "�R�O", NULL},
	{"ponder", "�H��", NULL},
	{"pout", "���ۼL��", NULL},
	{"pray", "��ë", NULL},
	{"request", "���D", NULL},
	{"shout", "�j�s", NULL},
	{"sing", "�ۺq", NULL},
	{"smile", "�L��", NULL},
	{"smirk", "����", NULL},
	{"swear", "�o�}", NULL},
	{"tease", "�J��", NULL},
	{"whimper", "��|����", NULL},
	{"yawn", "����s��", NULL},
	{"yell", "�j��", NULL},
	{NULL, NULL, NULL}
};


int
speak_action(unum, cmd, msg)
int     unum;
char   *cmd;
char   *msg;
{
	int     i;
	for (i = 0; speak_data[i].verb; i++) {
		if (!strcmp(cmd, speak_data[i].verb)) {
			sprintf(chatbuf, "[1;36m%s [32m%s�G[33m %s[37;0m",
				users[unum].chatid, speak_data[i].part1_msg, msg);
			send_to_room(users[unum].room, chatbuf);
			return 0;
		}
	}
	return 1;
}


/* -------------------------------------------- */
/* MUD-like social commands : condition          */
/* -------------------------------------------- */


struct action condition_data[] =
{
	{"applaud", "�԰԰԰԰԰԰�.... (���x�n)", NULL},
	{"beautiful", "�̺}�G�F!!!!", NULL},
	{"blush", "�y�����F", NULL},
	{"cough", "�y�F�X�n", NULL},
	{"faint", "�w�ˤF.....", NULL},
	{"god", "�U�઺�ѯ�,�н絹�گ��_���O�q..", NULL},
	{"handsome", "�̫ӤF!!!!", NULL},
	{"happy", "r-o-O-m....ť�F�u�n�I", NULL},
	{"hurt", "Wow!�n�h�n�h�I", NULL},
	{"luck", "�z�I�֮�աI", NULL},
	{"puke", "�u���ߡA��ť�F���Q�R", NULL},
	{"shake", "�n�F�n�Y", NULL},
	{"sleep", "Zzzzzzzzzz�A�u�L��A���ֺεۤF", NULL},
	{"so", "�N��l!!", NULL},
	{"strut", "�j�n�j�\\�a��", NULL},
	{"tongue", "�R�F�R���Y", NULL},
	{"think", "�n���Y�Q�F�@�U", NULL},
	{"walay", "�z�����I", NULL},
	{"wowu", "�ڹ�A�������ʷʦ��p����,�@�o���i���B ... :Q", NULL},
	{"yeh", "�C���", NULL},
	{NULL, NULL, NULL}
};


int
condition_action(unum, cmd)
int     unum;
char   *cmd;
{
	int     i;
	for (i = 0; condition_data[i].verb; i++) {
		if (!strcmp(cmd, condition_data[i].verb)) {
			sprintf(chatbuf, "[1;36m%s [33m%s[37;0m",
				users[unum].chatid, condition_data[i].part1_msg);
			send_to_room(users[unum].room, chatbuf);
			return 1;
		}
	}
	return 0;
}


/* -------------------------------------------- */
/* MUD-like social commands : help               */
/* -------------------------------------------- */

char   *dscrb[] = {
	"[1m�i Verb + Nick�G   �ʵ� + ���W�r �j[36m   �ҡG//kick piggy[m",
	"[1m�i Verb + Message�G�ʵ� + �n������ �j[36m   �ҡG//sing �ѤѤ���[m",
"[1m�i Verb�G�ʵ� �j    �����G�¸ܭ���[m", NULL};
struct action *verbs[] = {party_data, speak_data, condition_data, NULL};


#define SCREEN_WIDTH    80
#define MAX_VERB_LEN    10
#define VERB_NO         8

void
view_action_verb(unum)
int     unum;
{
	int     i, j;
	char   *p;
	send_to_unum(unum, "/c");
	for (i = 0; dscrb[i]; i++) {
		send_to_unum(unum, dscrb[i]);
		chatbuf[0] = '\0';
		j = 0;
		while (p = verbs[i][j++].verb) {
			strcat(chatbuf, p);
			if ((j % VERB_NO) == 0) {
				send_to_unum(unum, chatbuf);
				chatbuf[0] = '\0';
			} else {
				strncat(chatbuf, "        ", MAX_VERB_LEN - strlen(p));
			}
		}
		if (j % VERB_NO)
			send_to_unum(unum, chatbuf);
		send_to_unum(unum, " ");
	}
}


struct chatcmd chatcmdlist[] =
{
	"act", chat_act, 0,
	"bye", chat_goodbye, 0,
	"flags", chat_setroom, 0,
	"invite", chat_invite, 0,
	"join", chat_join, 0,
	"kick", chat_kick, 0,
	"msg", chat_private, 0,
	"nick", chat_nick, 0,
	"operator", chat_makeop, 0,
	"rooms", chat_list_rooms, 0,
	"whoin", chat_list_by_room, 1,
	"wall", chat_broadcast, 1,
	"cloak", chat_cloak, 1,
	"who", chat_map_chatids_thisroom, 0,
	"list", chat_list_users, 0,
	"topic", chat_topic, 0,

	NULL, NULL, 0
};


int
command_execute(unum)
int     unum;
{
	char   *msg = users[unum].ibuf;
	char   *cmd;
	struct chatcmd *cmdrec;
	int     match = 0;
	/* Validation routine */
	if (users[unum].room == -1) {
		/* MUST give special /! command if not in the room yet */
		if (msg[0] != '/' || msg[1] != '!')
			return -1;
		else
			return (login_user(unum, msg + 2));
	}
	/* If not a /-command, it goes to the room. */
	if (msg[0] != '/') {
		chat_allmsg(unum, msg);
		return 0;
	}
	msg++;
	cmd = nextword(&msg);

	if (cmd[0] == '/') {

		if (!strcmp(cmd + 1, "help") || (cmd[1] == '\0')) {
			view_action_verb(unum);
			match = 1;
		} else if (party_action(unum, cmd + 1, msg) == 0)
			match = 1;
		else if (speak_action(unum, cmd + 1, msg) == 0)
			match = 1;
		else
			match = condition_action(unum, cmd + 1);
	} else {
		for (cmdrec = chatcmdlist; !match && cmdrec->cmdstr; cmdrec++) {
			if (cmdrec->exact)
				match = !strcasecmp(cmd, cmdrec->cmdstr);
			else
				match = !strncasecmp(cmd, cmdrec->cmdstr, strlen(cmd));
			if (match)
				cmdrec->cmdfunc(unum, msg);
		}
	}

	if (!match) {
		sprintf(chatbuf, "[1;31m �� [37m��p�A�ݤ����A���N��G[36m/%s [31m��[m", cmd);
		send_to_unum(unum, chatbuf);
	}
	memset(users[unum].ibuf, 0, sizeof(users[unum].ibuf));
	return 0;
}


int
process_chat_command(unum)
int     unum;
{
	register int i;
	int     rc, ibufsize;
	if ((rc = recv(users[unum].sockfd, chatbuf, sizeof(chatbuf), 0)) <= 0) {
		/* disconnected */
		exit_room(unum, EXIT_LOSTCONN, (char *) NULL);
		return -1;
	}
	ibufsize = users[unum].ibufsize;
	for (i = 0; i < rc; i++) {
		/* if newline is two characters, throw out the first */
		if (chatbuf[i] == '\r')
			continue;

		/* carriage return signals end of line */
		else if (chatbuf[i] == '\n') {
			users[unum].ibuf[ibufsize] = '\0';
			if (command_execute(unum) == -1)
				return -1;
			ibufsize = 0;
		}
		/* add other chars to input buffer unless size limit exceeded */
		else {
			if (ibufsize < 127)
				users[unum].ibuf[ibufsize++] = chatbuf[i];
		}
	}
	users[unum].ibufsize = ibufsize;

	return 0;
}

int
main(argc, argv)
int     argc;
char   *argv[];
{
	struct sockaddr_in sin;
	register int i;
	int     sr, newsock, sinsize;
	fd_set  readfds;
	struct timeval tv;
	umask(007);
	/* ----------------------------- */
	/* init variable : rooms & users */
	/* ----------------------------- */

	if (argc <= 1) {
		strcpy(chatname, CHATNAME1);
		chatroom = 1;
		chatport = CHATPORT1;
	} else {
		chatroom = atoi(argv[1]);
	}
	switch (chatroom) {
	case 4:
		strcpy(chatname, CHATNAME4);
		chatport = CHATPORT4;
		break;
	case 3:
		strcpy(chatname, CHATNAME3);
		chatport = CHATPORT3;
		break;
	case 2:
		strcpy(chatname, CHATNAME2);
		chatport = CHATPORT2;
		break;
	case 1:
	default:
		strcpy(chatname, CHATNAME1);
		chatport = CHATPORT1;
		break;
	}
	maintopic = CHATROOM_TOPIC[chatroom - 1];
	strcpy(rooms[0].name, mainroom);
	strcpy(rooms[0].topic, maintopic);

	if (chatport <= 1000) {
		strcpy(chatname, CHATNAME1);
		chatroom = 1;
	}
	for (i = 0; i < MAXACTIVE; i++) {
		users[i].chatid[0] = '\0';
		users[i].sockfd = users[i].utent = -1;
	}

	/* ------------------------------ */
	/* bind chat server to port       */
	/* ------------------------------ */

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons(chatport);
	sin.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr *) & sin, sizeof(sin)) < 0) {
		return -1;
	}
	sinsize = sizeof(sin);
	if (getsockname(sock, (struct sockaddr *) & sin, &sinsize) == -1) {
		perror("getsockname");
		exit(1);
	}
	if (listen(sock, 5) == -1) {
		perror("listen");
		exit(1);
	}
	if (fork()) {
		return (0);
	}
	setpgid(0, 0);

	/* ------------------------------ */
	/* trap signals                   */
	/* ------------------------------ */

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGURG, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);


	FD_ZERO(&allfds);
	FD_SET(sock, &allfds);
	nfds = sock + 1;

	while (1) {
		memcpy(&readfds, &allfds, sizeof(readfds));

		tv.tv_sec = 60 * 30;
		tv.tv_usec = 0;
		if ((sr = select(nfds, &readfds, NULL, NULL, &tv)) < 0) {
			if (errno == EINTR)
				sleep(50);
			continue;
		} else if (!sr)
			continue;

#if 0
		if (sr == 0) {
			exit(0);/* normal chat server shutdown */
		}
#endif

		if (FD_ISSET(sock, &readfds)) {
			sinsize = sizeof(sin);
			newsock = accept(sock, (struct sockaddr *) & sin, &sinsize);
			if (newsock == -1) {
				continue;
			}
			for (i = 0; i < MAXACTIVE; i++) {
				if (users[i].sockfd == -1) {
					struct hostent *hp;
					char   *s = users[i].host;
					struct hostent *local;
					//struct in_addr in;
					int     j;
					local = gethostbyname("localhost");


					if (local) {
						for (j = 0; j < local->h_length / sizeof(unsigned int); j++) {
							if (sin.sin_addr.s_addr == (unsigned int) local->h_addr_list[j])
								break;
						}
						if ((j < local->h_length) || (sin.sin_addr.s_addr == 0x100007F))
							strcpy(s, "localhost");
						else {
							hp = gethostbyaddr((char *) &sin.sin_addr, sizeof(struct in_addr),
								sin.sin_family);
							strncpy(s, hp ? hp->h_name : (char *) inet_ntoa(sin.sin_addr), 30);
						}
					} else {
						hp = gethostbyaddr((char *) &sin.sin_addr, sizeof(struct in_addr),
							sin.sin_family);
						strncpy(s, hp ? hp->h_name : (char *) inet_ntoa(sin.sin_addr), 30);
					}
					s[29] = 0;

					users[i].sockfd = newsock;
					users[i].room = -1;
					break;
				}
			}

			if (i >= MAXACTIVE) {
				/* full -- no more chat users */
				close(newsock);
			} else {

#if !RELIABLE_SELECT_FOR_WRITE
				int     flags = fcntl(newsock, F_GETFL, 0);
				flags |= O_NDELAY;
				fcntl(newsock, F_SETFL, flags);
#endif

				FD_SET(newsock, &allfds);
				if (newsock >= nfds)
					nfds = newsock + 1;
				num_conns++;
			}
		}
		for (i = 0; i < MAXACTIVE; i++) {
			/* we are done with newsock, so re-use the variable */
			newsock = users[i].sockfd;
			if (newsock != -1 && FD_ISSET(newsock, &readfds)) {
				if (process_chat_command(i) == -1) {
					logout_user(i);
				}
			}
		}
#if 0
		if (num_conns <= 0) {
			/* one more pass at select, then we go bye-bye */
			tv = zerotv;
		}
#endif
	}
	/* NOTREACHED */
}
