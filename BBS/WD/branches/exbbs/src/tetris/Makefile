#	@(#)Makefile	8.1 (Berkeley) 5/31/93

PROG=	tetris
SRCS=	input.c screen.c shapes.c tetris.c scores.c
MAN6=	tetris.6
DPADD=	${LIBTERMCAP}
LDADD=	-ltermcap
HIDEGAME=hidegame

.include <bsd.prog.mk>
