CC = gcc
EXEC = czy dzy
CCFLAGS = -Wall -g -ansi -pedantic
OBJS = czy.o dzy.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}

default:
	${CC} ${CCFLAGS} -c $<

all: ${EXEC}

run: ${EXEC}
	./${EXEC}
    
clean:
	rm -f ${EXEC} ${OBJS}

test: ${EXEC}
	valgrind ./${EXEC}

#ctar.o: ctar.c ctar.h
