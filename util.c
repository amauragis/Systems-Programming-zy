#include <stdio.h>
#include <unistd.h>
#include "zy.h"

unsigned char buffer[BUF_SIZE];
int bitIndex;

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

int writeBits(unsigned int value, char size)
{
    int curr_byte = (bit_buf_index / 8);
    char remaining_bits = (8 - (bit_buf_index % 8));
}
