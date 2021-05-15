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
$Id: modetype.c,v 1.1 2000/01/15 01:45:28 edwardc Exp $
*/

#include "modes.h"

char   *
ModeType(mode)
int     mode;
{
	switch (mode) {
	case IDLE:
		return "";
	case NEW:
		return "�s���͵��U";
	case LOGIN:
		return "�i�J����";
	case DIGEST:
		return "�s����ذ�";
	case MMENU:
		return "�D���";
	case ADMIN:
		return "�޲z�̿��";
	case SELECT:
		return "��ܰQ�װ�";
	case READBRD:
		return "���M�ѤU";
	case READNEW:
		return "���s�峹";
	case READING:
		return "�~���峹";
	case POSTING:
		return "�o��峹";
	case MAIL:
		return "�B�z�H��";
	case SMAIL:
		return "�H�y�H�F";
	case RMAIL:
		return "�\\���H��";
	case TMENU:
		return "��ѿ��";
	case LUSERS:
		return "���U�|��";
	case FRIEND:
		return "�M��n��";
	case MONITOR:
		return "��������";
	case QUERY:
		return "�d�ߺ���";
	case TALK:
		return "���";
	case PAGE:
		return "�I�s";
	case CHAT1:
		return "Chat1";
	case CHAT2:
		return "Chat2";
	case CHAT3:
		return "Chat3";
	case CHAT4:
		return "Chat4";
	case IRCCHAT:
		return "�|��IRC";
	case LAUSERS:
		return "��������";
	case XMENU:
		return "�t�θ�T";
	case VOTING:
		return "�벼";
	case BBSNET:
		return "BBSNET";
	case EDITWELC:
		return "�s��Welc";
	case EDITUFILE:
		return "�s��ӤH��";
	case EDITSFILE:
		return "�s�רt����";
	case ZAP:
		return "�q�\\�Q�װ�";
	case GAME:
		return "���O�E��";
	case SYSINFO:
		return "�ˬd�t��";
	case ARCHIE:
		return "ARCHIE";
	case DICT:
		return "½�d�r��";
	case LOCKSCREEN:
		return "�ù���w";
	case NOTEPAD:
		return "�d���O";
	case GMENU:
		return "�u��c";
	case MSG:
		return "�e�T��";
	case USERDEF:
		return "�ۭq�Ѽ�";
	case EDIT:
		return "�ק�峹";
	case OFFLINE:
		return "�۱���..";
	case EDITANN:
		return "�s�׺��";
	case WWW:
		return "�y�C WWW";
	case HYTELNET:
		return "Hytelnet";
	case CCUGOPHER:
		return "�L�����";
	case LOOKMSGS:
		return "��ݰT��";
	case WFRIEND:
		return "�M�H�W�U";
	case WNOTEPAD:
		return "�����ٯd";
	case BBSPAGER:
		return "�����ǩI";
	default:
		return "�h�F����!?";
	}
}
