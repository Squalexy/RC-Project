CC		= gcc
FLAGS	= -Wall -g -O
OBJS	= client.o
PROG	= client

all:		${PROG}

clean:
			rm ${OBJS} *~ ${PROG}

${PROG}:	${OBJS}
			${CC} ${FLAGS} ${OBJS} -o $@
		
.c.o:
			${CC} ${FLAGS} $< -c -o $@

client.o: client.c client.h global.h

client: client.o
