CC = gcc
EXEC = czy dzy
CCFLAGS = -Wall -g -pedantic
OBJS = czy.o dzy.o util.o

all: ${EXEC}

#${EXEC}: ${OBJS}
#	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}

czy: czy.o util.o
	${CC} ${CCFLAGS} -lm -o czy czy.o util.o
dzy: dzy.o util.o
	${CC} ${CCFLAGS} -lm -o dzy dzy.o util.o
    
clean:
	rm -f ${EXEC} ${OBJS}
