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
CFLAGS    = -Os -I../include
ICONVFLAGS = -I/usr/local/include -L/usr/local/lib
LIBS     = -ltermcap -lcrypt -lcompat -export-dynamic -liconv
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

EXE =	bbs mbbsd

BIN =	$(EXE) bbsrf xchatd xchatd3

ALL =   $(BIN) zbbsd

SRC =	admin.c announce.c bbs.c board.c cache.c chat.c dopin.c edit.c\
	io.c kaede.c mail.c main.c menu.c more.c name.c read.c record.c\
	register.c screen.c stuff.c talk.c term.c user.c vote.c xyz.c\
	leeym.c dl_lib.c
           
OBJ =	admin.o announce.o bbs.o board.o cache.o chat.o edit.o dopin.o\
	io.o kaede.o mail.o main.o menu.o more.o name.o read.o\
	record.o register.o screen.o stuff.o talk.o term.o user.o vote.o\
	xyz.o leeym.o dl_lib.o str_decode_m3.o

MOBJ =	visio.o admin.o announce.o bbs.o board.o cache.o chat.o dopin.o\
	edit.o kaede.o mail.o menu.o more.o name.o read.o record.o register.o\
	stuff.o talk.o user.o vote.o xyz.o mbbsd.o leeym.o dl_lib.o\
	str_decode_m3.o

SO =

ETC =	Makefile $(HDR)

.SUFFIXES: .o .c .ln .so

.c.o:   ;   $(CC) $(CFLAGS) $(OS_DEF) $(ICONVFLAGS) -c $*.c
.c.ln:  ;   lint -abhi $*.c
.o.so:  ;   ld -s -G $*.o -o $*.so

so: $(SO)

all: $(ALL)

bbs: $(OBJ) $(ETC)
	$(CC) -o $@ $(CFLAGS) $(OBJ) $(ICONVFLAGS) $(LIBS)

lint: $(LNFILES)
	lint -abh $(LNFILES)

xchatd: xchatd.c
	$(CC) -o xchatd $(OS_DEF) $(CFLAGS) xchatd.c

xchatd3: xchatd3.c
	$(CC) -o xchatd3 $(OS_DEF) $(CFLAGS) xchatd3.c

bbsrf: bbsrf.c 
	$(CC) $(CFLAGS) -o bbsrf $(OS_DEF) bbsrf.c

zbbsd: zbbsd.c
	$(CC) $(CFLAGS) -o zbbsd $(OS_DEF) zbbsd.c $(LIBSYBBSD)

mbbsd: $(MOBJ) $(ETC)
	$(CC) -o $@ $(CFLAGS) $(MOBJ) $(ICONVFLAGS) $(LIBS)

newbbs: $(EXE)
	-sh -c "cd $(BBSHOME)/bin; touch $(EXE)"
	mv -f $(BBSHOME)/bin/bbs $(BBSHOME)/bin/bbs.old
	mv -f $(BBSHOME)/bin/mbbsd $(BBSHOME)/bin/mbbsd.old
	cp $(EXE) $(BBSHOME)/bin
	date > ~/etc/update

update: $(ALL)
	sh -c "cd $(BBSHOME)/bin; touch $(ALL)"
	mv -f $(BBSHOME)/bin/bbs $(BBSHOME)/bin/bbs.old
	mv -f $(BBSHOME)/bin/bbsrf $(BBSHOME)/bin/bbsrf.old
	mv -f $(BBSHOME)/bin/xchatd $(BBSHOME)/bin/xchatd.old
	mv -f $(BBSHOME)/bin/xchatd3 $(BBSHOME)/bin/xchatd3.old
	mv -f $(BBSHOME)/bin/zbbsd $(BBSHOME)/bin/zbbsd.old
	mv -f $(BBSHOME)/bin/mbbsd $(BBSHOME)/bin/mbbsd.old
	cp $(ALL) $(BBSHOME)/bin
	date > ~/etc/update

clean: /tmp
	rm -fr $(GARBAGE) *.o $(ALL) $(LNFILES) *~