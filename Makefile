CC = gcc
EXEC = czy dzy
CCFLAGS = -Wall -g -ansi -pedantic
OBJS = czy.o dzy.o

all: ${EXEC}

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}
    
clean:
	rm -f ${EXEC} ${OBJS}
