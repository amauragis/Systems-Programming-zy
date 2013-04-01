#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "zy.h"

// comparator function used by qsort.  will function such that greater
// frequencies are placed before lesser frequencies.
int charfreqCompare(const void* a, const void* b)
{
    return ((charfreq_t*)b)->frequency - ((charfreq_t*)a)->frequency;
}

void printchar(unsigned char theChar)
{

    switch (theChar) {

        case '\n':
            fprintf(stdout,"\\n");
            break;
        case '\r':
            fprintf(stdout,"\\r");
            break;
        case '\t':
            fprintf(stdout,"\\t");
            break;
        default:
            if ((theChar < 0x20) || (theChar > 0x7f)) {
                fprintf(stdout,"\\%03o", (unsigned char)theChar);
            } else {
                fprintf(stdout,"%c", theChar);
            }
        break;

   }
}

// check if currChar is part of dict
int charInDict(char currChar, char dict[16])
{
    int i;
    for (i = 0; i < 16; i++)
    {
        if (currChar == dict[i]){
            // it is, return immediately with the index
            return i;
        }
    }
    // its not, return -1
    return -1;
}



