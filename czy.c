/*
 * czy.c
 * Andrew Mauragis
 * Due: 4/2/2013
 *
 * Takes input stream, compresses it, writes it to output stream.
 * Compression is run-length encoding
 */

#include "zy.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

// comparator function used by qsort.  will function such that greater
// frequencies are placed before lesser frequencies.
int charfreqCompare(const void* a, const void* b)
{
    return ((charfreq_t*)b)->frequency - ((charfreq_t*)a)->frequency;
}

static void printchar(unsigned char theChar) {

    switch (theChar) {

        case '\n':
            printf("\\n");
            break;
        case '\r':
            printf("\\r");
            break;
        case '\t':
            printf("\\t");
            break;
        default:
            if ((theChar < 0x20) || (theChar > 0x7f)) {
                printf("\\%03o", (unsigned char)theChar);
            } else {
                printf("%c", theChar);
            }
        break;

   }
}

int czy()
{
    // first we have to create the "most common character" dictionary
    charfreq_t charFreq[UCHAR_MAX];
    memset(charFreq, 0, UCHAR_MAX*sizeof(charfreq_t));

    unsigned char currChar;
    int readcheck;

    // now we iterate through the entire input stream. For each character
    // we read, we add 1 to its count in the charFreq array.
    while((readcheck = read(STDIN_FILENO, &currChar,1)) == 1)
    {
        charFreq[currChar].character = currChar;
        charFreq[currChar].frequency += 1;
    }
    if (readcheck < 0)
    {
        // PROBLEM
        return READ_ERROR;
    }

    // sort the charFreq array using the charfreqCompare function
    qsort(&charFreq, UCHAR_MAX, sizeof(charfreq_t), charfreqCompare);

    // test print charfreq array 
    int i;
    for(i = 0; i < 16; i++)
    {
        //printf("char: %c\tfreq: %i\n",charFreq[i].character, charFreq[i].frequency);
        printchar(charFreq[i].character);
        printf("\n");
    }

    return 0;
}

int main()
{
    return czy();
}
