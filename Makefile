CC = gcc
EXEC = czy dzy
CCFLAGS = -Wall -g -pedantic
OBJS = czy.o dzy.o

all: ${EXEC}

#${EXEC}: ${OBJS}
#	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}
czy: czy.o
	${CC} ${CCFLAGS} -lm -o  czy czy.o
dzy: dzy.o
	${CC} ${CCFLAGS} -lm -o  dzy dzy.o
    
clean:
	rm -f ${EXEC} ${OBJS}
