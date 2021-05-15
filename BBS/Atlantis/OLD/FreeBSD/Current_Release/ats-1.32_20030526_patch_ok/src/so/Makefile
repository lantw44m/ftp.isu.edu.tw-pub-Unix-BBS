# ------------------------------------------------------ #
#  maple/Makefile	( NTHU CS MapleBBS Ver 2.36.sob) #
# ------------------------------------------------------ #
#  target : Makefile for MapleBBS main programs		 #
#  create : 95/03/29				 	 #
#  update : 96/09/12					 #
# ------------------------------------------------------ #
BBSHOME = /home/bbsrs

# ------------------------------------------------------
# NetBSD, 386BSD, FreeBSD 
# ------------------------------------------------------
OS_DEF   = -DNETBSD -DBSD44 -DTWO_LONGJMP_ARGS 
CC       = gcc 
CFLAGS	 = -Os -I../include
LIBS     = -ltermcap -lcrypt -lcompat -export-dynamic
LIBSYBBSD = -lkvm -lutil -lcrypt

# ------------------------------------------------------
# Linux : please remove -lrpcsvc in 'bbsrf' section
# ------------------------------------------------------
#OS_DEF   = -DLINUX -DTWO_LONGJMP_ARGS 
#CC       = gcc 
#CFLAGS    = -O6 -s -pipe -fomit-frame-pointer -I/usr/include/bsd
#LIBS     = -ltermcap -lbsd

# ------------------------------------------------------
# SunOS 4.1.x
# ------------------------------------------------------
#OS_DEF	= -DSunOS -DTWO_LONGJMP_ARGS
#CC	= gcc
#CFLAGS	= -O6 -s -pipe -fomit-frame-pointer -fstrength-reduce
#LIBS	= -ltermcap 
#LIBSYBBSD = -lkvm

# ------------------------------------------------------ #
# 下列的 make rules 不需修改				 #
# ------------------------------------------------------ #

ALL =   $(SO)

SO =	add_money.so agree.so b_results.so b_vote.so chk_result.so\
	check_expire_time.so dreye.so e_sys_f.so help.so input_tool.so\
	menager.so message.so m_all.so m_group.so m_list.so m_st.so m_vote.so\
	pip_3_ats.so query_rmuser.so record_list.so register.so st_name.so\
	station_set.so ulist.so user_files.so x_program.so zip_dir.so\
	newcomers.so goodbaby.so goodbye.so check_mkey.so

ETC =	Makefile $(HDR)

.SUFFIXES: .o .c .ln .so

.c.o:   ;   $(CC) $(CFLAGS) $(OS_DEF) -c $*.c
.c.ln:  ;   lint -abhi $*.c
.o.so:  ;   ld -s -G $*.o -o $*.so

so: $(SO)

all: $(ALL)

update: $(ALL)
	sh -c "cd $(BBSHOME)/bin; touch $(ALL)"
	cp -f $(SO) $(BBSHOME)/bin
	date > ~/etc/update

clean: /tmp
	rm -fr *.o $(ALL) *~
