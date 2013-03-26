/* shared library with error codes and function prototypes */

#ifndef ZY_H
#define ZY_H

// error codes
#define UNKNOWN_ERROR   -1
#define READ_ERROR      -2
#define WRITE_ERROR     -3

typedef struct
{
    unsigned char character;
    int frequency;
} charfreq_t;


//function prototypes
int dzy();
int czy();
static void printchar(unsigned char theChar);
int charfreqCompare(const void* a, const void* b);

#endif