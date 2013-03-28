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
        if(write(STDOUT_FILENO, &dict[i],sizeof(dict[i])) != 1)
        {
            return WRITE_ERROR;
        }    
        // debug code.  Prints nicely
        //printchar(charFreq[i].character);
        //fprintf(stdout,"\n",sizeof(char));
    }

    // now we go to the actual run-length coding algorithm

    // seek stdin back to beginning.
    if(lseek(STDIN_FILENO, 0, SEEK_SET) < 0) return SEEK_ERROR;

    // read from standard in until we can't
    while((readcheck = read(STDIN_FILENO, &currChar,1)) == 1)
    {
        int prefix;

        // index in dictionary, this will be the frequent character code also
        int dictIndex = charInDict(currChar,dict);
        
        // character is in dictionary
        if (dictIndex <= 0)
        {
            int runLength = 0;
            unsigned char checkChar = currChar;

            // increment run length until we:
            //  - reach the end of the file
            //  - reach a different character
            //  - exceed the maximum run length
            while(read(STDIN_FILENO, &currChar, 1) == 1 &&
                  checkChar == currChar &&
                  runLength < MAX_RUN_LENGTH)
            {
                runLength++;
            }

            // now we create the encoded character, frequent with a zero prepended
            prefix = 0;
            runLength <<= 4;
            unsigned int encodedChar = prefix | runLength | dictIndex;

            // we have the bits to write, now we write them
            int errcode = writeBits(encodedChar,9);
            if (errcode)
            {
                return errcode;
            }

        }

        // character is not in dictionary (ie: nonfrequent character)
        else
        {
            // character is nonfrequent, therefore gets a 1 prepended
            prefix = 1;
            prefix <<= 8;
            unsigned int encodedChar = prefix | currChar;

            // we have the bits to write, now we write them
            int errcode = writeBits(encodedChar,9);
            if (errcode)
            {
                return errcode;
            }

            // loop continues, read the next byte;
        }
    }
    if (readcheck < 0)
    {
        // PROBLEM
        return READ_ERROR;
    }

    // make sure the buffer is empty and we've zero-padded appropriately, etc
    return flushBits();
}


int main()
{
    int retval;
    retval = czy();

    // lets handle some errors!
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
        fprintf(stderr,"Usage: ./czy < inputfile > outputfile\n");
        exit(retval);
    }
}
