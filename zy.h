// shared library with error codes, system constants, and function prototypes

#ifndef ZY_H
#define ZY_H

// error codes
#define UNKNOWN_ERROR   -1
#define READ_ERROR      -2
#define WRITE_ERROR     -3
#define SEEK_ERROR  	-4

// system constants
#define BUF_SIZE		64
#define MAX_RUN_LENGTH	16

// struct for dictionary sorting
typedef struct
{
    unsigned char character;
    int frequency;
} charfreq_t;

//global variables
extern unsigned char buffer[BUF_SIZE];
extern unsigned char readBuffer;
extern int bitIndex;

//function prototypes
int dzy();
int czy();
void printchar(unsigned char theChar);
int charfreqCompare(const void* a, const void* b);
int writeBits(unsigned int value, char size);
int flushBits();
int charInDict(char currChar, char dict[16]);
int readBits(unsigned char* data, unsigned int length);

#endif
