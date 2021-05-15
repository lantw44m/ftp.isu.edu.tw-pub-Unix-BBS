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
CFLAGS = -O2 -I../util -I../include

CC =	gcc

EXE =	cvsup

BIN = 	$(EXE)

ALL =	$(BIN)

OBJ =	cvsup.o checksum.o write_log.o update.o

.SUFFIXES: .o .c .ln

.c.o:   ;   $(CC) $(CFLAGS) $(OS_DEF) -c $*.c
.c.ln:  ;   lint -abhi $*.c

cvsup: $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ)

update: $(ALL) linkbin
	date > ~/etc/update

linkbin:
	sh -c "cd /home/$(BBSUSER)/bin ; rm -f $(ALL)"
	sh -c "./lnbin $(BBSUSER) $(ALL)"

clean:
	rm -f $(ALL) *.o *~