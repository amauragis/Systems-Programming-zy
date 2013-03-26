#include <stdio.h>
#include <unistd.h>
#include "zy.h"

// comparator function used by qsort.  will function such that greater
// frequencies are placed before lesser frequencies.
int charfreqCompare(const void* a, const void* b)
{
    return ((charfreq_t*)b)->frequency - ((charfreq_t*)a)->frequency;
}

static void printchar(unsigned char theChar)
{

    switch (theChar) {

        case '\n':
            fprintf(stdout,"\\n",sizeof(char));
            break;
        case '\r':
            fprintf(stdout,"\\r",sizeof(char));
            break;
        case '\t':
            fprintf(stdout,"\\t",sizeof(char));
            break;
        default:
            if ((theChar < 0x20) || (theChar > 0x7f)) {
                fprintf(stdout,"\\%03o", (unsigned char)theChar,sizeof(char));
            } else {
                fprintf(stdout,"%c", theChar,sizeof(char));
            }
        break;

   }
}