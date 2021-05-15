/*
 * functions.h		-- include file for define functions individual
 *	
 * Copyright (c) 1999, Edward Ping-Da Chuang <edwardc@edwardc.dhs.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * CVS: $Id: functions.h,v 1.11 2002/09/11 09:59:00 edwardc Exp $
 */

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#define RNDSIGN
/* �ü�ñ�W�� */

//#define MAGIC_PASS
/* ������ ID �W���߰� Magic Pass */

#define REFUSE_LESS60SEC
/* 60 �����i���� login */

#define TALK_LOG
/* ��Ѭ����\�� */

#define CODE_VALID
/* �t�X�{�� */

//#define MSG_CANCLE_BY_CTRL_C
/* �� ctrl-c �ӧ_���T�� */

#define LOG_MY_MESG
/* �T�������������ۤv�ҵo�X���T�� */

#define COLOR_POST_DATE
/* �峹����C�� */

#define BIGGER_MOVIE
/* �[�j���ʬݪO�Ŷ� (�C��) */

#define ALWAYS_SHOW_BRDNOTE
/* �C���i�O���| show �X�i�O�e�� */

#define BBSD
/* �ϥ� BBS daemon, �����h�i�ϥ� in.zbbsd, telnetd �Ψϥ� bbsrf */

//#define MUDCHECK_BEFORELOGIN
/* �����Y�� MUD server �|�n�D�ϥΪ̵n�J���e����ӽT�w���O�ϥε{���ӳs�u
   Ctrl + C ���ɥi�אּ Ctrl + D, Ctrl + Z .. */

#define DLM
/* .SO ���䴩, �N�\�h���`�Ϊ��\��]�� .so ��, �ϥΰʺA�s���覡�I�s */

#define NO_DELETED_BOARD
/* ���çR���L��������T */

#define DISABLE_RFC931
/* ����n�D identd reponse, �H�K�D WinXP �Ψ�L�����𲣥ͤ��_�s�u */

#define Quick_LOGIN
/* �Y�b��J�b���B, ������ <Enter> �h���������ϥ� guest �W�� */

#define ANTI_HANG_ON_SITE
/* �]�w����� */

#define ANONYBOARD_POST_DEFAULT_ANONY
/* �ΦW���npost���ɭ�, �h�w�]"�n"�ΦW */

//#define Fuzzy_MENU
/* �]�w�i�J���\��ɤ��ݭn�t�~�b���U enter */

#define USE_CHINESE_DATE
/* �ϥΤ����� (buggy!! dont enable!) */

#define FROM_TRANSLATE
/* ����W���ӷ� */

#endif /* _FUNCTIONS_H_ */
