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

int czy()
{
    /* first we have to create the "most common character" dictionary */
    int char_freq[256];
    memset(char_freq, 0, 256*sizeof(int));


    return 0;
}

int main()
{
    return 0;
}