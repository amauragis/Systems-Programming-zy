/*
 * dzy.c
 * Andrew Mauragis
 * Due: 4/2/2013
 *
 * Takes input stream, decompresses it, writes it to output stream.
 * Compression is run-length encoding
 */

#include "zy.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int dzy()
{
    // first we rebuild the dictionary
    unsigned char dict[16];

    int readval = 1;
    int i;
    // i'm so sorry
    for(i = 0; i < 16 && (readval = read(STDIN_FILENO, dict+i,1)) == 1; i++);
    if(readval <= 0) return READ_ERROR;
    
    // we need to loop until we reach the end of the file, not sure how best
    // to structure this yet
    while (1)
    {
        // we are at the first encoded char
        unsigned char checkBit = 0;
        readval = readBits(&checkBit, 1);
        
        // if we can't read any bits we hit EOF
        if (readval == 0) break;
        // if its less than 0, we're returning an error code
        if (readval < 0) return readval;

        if (checkBit != 0)
        {
            // this is an infrequenty symbol (ie: checkbit == 1)
            unsigned char symbol = 0;
            // read the next 8 bits
            readval = readBits(&symbol, 8);

            // if its less than 0, we're returning an error code
            if (readval < 0) return readval;

            // if we read less than the 8 bits we expected, EOF, we're done
            if (readval < 8) break;

            // now we print the 8 bit symbol, simple!
            if (write(STDOUT_FILENO, &symbol, 1) != 1) return WRITE_ERROR;

        }
        else
        {
            // frequent symbol (ie checkbit == 0)
            // first 4 bits are run length, next 4 character code
            unsigned char runLength = 0;
            readval = readBits(&runLength, 4);

            // if its less than 0, we're returning an error code
            if (readval < 0) return readval;

            // if we read less than the 4 bits we expected, EOF, we're done
            if (readval < 4) break;

            // now we get the character code from the dictionary
            unsigned char dictCode = 0;
            readval = readBits(&dictCode, 4);

            // if its less than 0, we're returning an error code
            if (readval < 0) return readval;

            // if we read less than the 4 bits we expected, EOF, we're done
            if (readval < 4) break;

            // pull the real character from the dictionary
            unsigned char realChar = dict[dictCode];

            // write it the number of times equal to the run length
            for (i = 0; i <= runLength; i++)
            {
                if (write(STDOUT_FILENO, &realCHar, 1) != 1) return WRITE_ERROR;
            }
        }
    }

    // should be done.
    return 0;
}

int main()
{
    int retval;
    retval = dzy();
    if(retval != 0)
    {
        switch (retval)
        {
            case UNKNOWN_ERROR:
            {
                fprintf(stderr,"Unknown error!\n");
                break;
            }
            case READ_ERROR:
            {
                fprintf(stderr,"Read error!\n");
                break;
            }
            case WRITE_ERROR:
            {
                fprintf(stderr,"Write error!\n");
                break;
            }
            case SEEK_ERROR:
            {
                fprintf(stderr,"Seek error!\n");
                break;
            }
            default:
            {
                fprintf(stderr,"SEVERELY unknown error!\n");
                break;
            }

        }
        fprintf(stderr,"Usage: ./dzy < inputfile > outputfile\n");
        exit(retval);
    }
}
