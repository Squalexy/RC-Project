CC		= gcc
FLAGS	= -pthread -D_REENTRANT -Wall -g -O
OBJS	= server.o server_clients.o server_config.o file.o
PROG	= server

all:		${PROG}

clean:
			rm ${OBJS} *~ ${PROG}

${PROG}:	${OBJS}
			${CC} ${FLAGS} ${OBJS} -o $@
		
.c.o:
			${CC} ${FLAGS} $< -c -o $@

server.o: server.c server.h global.h

server_clients.o: server_clients.c global.h

server_config.o: server_config.c global.h

file.o: file.c file.h global.h

server: server.o server_clients.o server_config.o file.o

