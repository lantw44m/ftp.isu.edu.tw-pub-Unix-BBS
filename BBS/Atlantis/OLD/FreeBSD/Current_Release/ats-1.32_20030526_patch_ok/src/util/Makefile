# ------------------------------------------------------- #
#  util/Makefile	( NTHU CS MapleBBS Ver 2.36 )	  #
# ------------------------------------------------------- #
#  target : Makefile for 寄信、統計、備份、系統維護工具	  #
#  create : 95/03/29					  #
#  update : 96/08/26					  #
# ------------------------------------------------------- #
BBSUSER = bbsrs

# --------------------------------------------------------
# For BSD
# --------------------------------------------------------
CFLAGS	= -O6 -pipe -s -fomit-frame-pointer -I../maple -I../include\
	-DBSD44 -D_BBS_UTIL_C_
LIBS 	= -lcrypt

# --------------------------------------------------------
# For Linux
# --------------------------------------------------------
#CFLAGS	= -O6 -pipe -s -fomit-frame-pointer -I../maple -DLINUX -D_BBS_UTIL_C_
#LIBS	= -ltermcap -lbsd

# --------------------------------------------------------
# For SunOS
# --------------------------------------------------------
#CFLAGS	= -O6 -pipe -s -fomit-frame-pointer -I../maple -DSunOS -D_BBS_UTIL_C_
#LIBS 	= 

CC	= gcc

ALL =	mailpost poststat repasswd ognizeboard account bbsmail buildir clrdir\
	clrhome expire showboard showuser topusr mob setuflag bpop3d reboard\
	setuser setboard rmtusr bgopherd backup mandex bfinger randgen bbsroot\
	bbs2html bbs2html_cgi bbspost birth chkpasswd remove_put h_mandex horo\
	makeexpire check_agree updateboard check_person single_multi_st\
	check_size host2db repasswd_online trans_distinct trans_station_ascii\
	trans_help rm_spam_mail
	 
#killbbss

all: $(ALL)

mailpost: mailpost.c
	$(CC) -o $@ $(CFLAGS) $@.c $(LIBS)

mandex: mandex.c
	$(CC) -o $@ $(CFLAGS) $@.c

bgopherd: bgopherd.c
	$(CC) -o $@ $(CFLAGS) $@.c $(LIBS)

bfinger: bfinger.c
	$(CC) -o $@ $(CFLAGS) $@.c

bpop3d: bpop3d.c
	$(CC) -o $@ $(CFLAGS) $@.c $(LIBS)

poststat: poststat.c
	$(CC) -o $@ $(CFLAGS) $@.c

account: account.c
	$(CC) -o $@ $(CFLAGS) $@.c

bbsmail: bbsmail.c
	$(CC) -o $@ $(CFLAGS) $@.c

buildir: buildir.c
	$(CC) -o $@ $(CFLAGS) $@.c

clrdir: clrdir.c
	$(CC) -o $@ $(CFLAGS) $@.c

clrhome: clrhome.c
	$(CC) -o $@ $(CFLAGS) $@.c

expire: expire.c
	$(CC) -o $@ $(CFLAGS) $@.c

showboard: showboard.c
	$(CC) -o $@ $(CFLAGS) $@.c

showuser: showuser.c
	$(CC) -o $@ $(CFLAGS) $@.c

topusr: topusr.c
	$(CC) -o $@ $(CFLAGS) $@.c

mob: mob.c
	$(CC) -o $@ $(CFLAGS) $@.c

setuflag: setuflag.c
	$(CC) -o $@ $(CFLAGS) $@.c

reboard: reboard.c
	$(CC) -o $@ $(CFLAGS) $@.c

repasswd: repasswd.c
	$(CC) -o $@ $(CFLAGS) $@.c

setboard: setboard.c
	$(CC) -o $@ $(CFLAGS) $@.c

rmtusr: rmtusr.c
	$(CC) -o $@ $(CFLAGS) $@.c

randgen: randgen.o
	$(CC) -o $@ $(CFLAGS) $@.c

bbsroot: bbsroot.o
	$(CC) -o $@ $(CFLAGS) $@.c

bbs2html: bbs2html.o
	$(CC) -o $@ $(CFLAGS) $@.c

bbs2html_cgi: bbs2html_cgi.o
	$(CC) -o $@ $(CFLAGS) $@.c

bbspost: bbspost.c
	$(CC) -o $@ $(CFLAGS) $@.c

#killbbs: killbbs.o
	$(CC) -o $@ $(CFLAGS) $@.c -lkvm

update: $(ALL) linkbin

linkbin:
	sh -c "cd /home/$(BBSUSER)/bin ; rm -f $(ALL)"
	sh -c "./lnbin $(BBSUSER) $(ALL)"

clean:
	rm -f $(ALL) *.o
