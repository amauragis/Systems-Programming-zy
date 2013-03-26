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

    // print dictionary 
    int i;
    char dict[16];
    for(i = 0; i < 16; i++)
    {
        dict[i] = charFreq[i].character;
        write(STDOUT_FILENO, &dict[i],sizeof(dict[i]));
                
        // debug code.  Prints nicely
        //printchar(charFreq[i].character);
        //fprintf(stdout,"\n",sizeof(char));
    }

    return 0;
}

int main()
{
    return czy();
}
