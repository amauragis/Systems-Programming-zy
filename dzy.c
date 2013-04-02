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

unsigned char readBuffer;
unsigned int bitIndex;

// readBits
//  Reads bits from stdin
//
// data:   pointer to a char array to hold data
// length: number of bits to read (<8)
int readBits(unsigned char* data, unsigned char length)
{
    fprintf(stderr,"\n\nReadBits Call: %i bits\n----------------\n",length);
    unsigned char returnedBits = 0;
    // bits in buffer
    unsigned char remainingBits = (8 - bitIndex);

    // get bits from the buffer

    while (remainingBits < length)
    {
        fprintf(stderr, "remainingBits: %i\n",remainingBits);
        fprintf(stderr, "data: %x\n", *data);
        fprintf(stderr, "readBuffer: %x\n", readBuffer);
        // move bits from buffer into data
        *data |= (readBuffer << (length - remainingBits));

        // put a new byte in the buffer
        int readval = read(STDIN_FILENO, &readBuffer, 1);
        if (readval < 0) return READ_ERROR;
        if (readval == 0) return returnedBits; // EOF, done.

        // update length and number of bits to return and reset buffer
        length -= remainingBits;
        returnedBits += remainingBits;
        bitIndex = 0;
        remainingBits = 8;
    }
    fprintf(stderr, "remainingBits1: %i\n",remainingBits);
    fprintf(stderr, "data1: %x\n", *data);
    fprintf(stderr, "readBuffer1: %x\n", readBuffer);

    // put rest of bits in data
    unsigned char bitsLeft = remainingBits - length;
    *data |= (readBuffer >> bitsLeft);

    // clear bits that we've written
    readBuffer <<= (8 - bitsLeft);
    readBuffer >>= (8 - bitsLeft);

    returnedBits += length;
    bitIndex += length;

    fprintf(stderr, "remainingBits2: %i\n",remainingBits);
    fprintf(stderr, "data2: %x\n", *data);
    fprintf(stderr, "readBuffer2: %x\n", readBuffer);
    return returnedBits;
}


int dzy()
{
    
    // first we rebuild the dictionary
    unsigned char dict[16];

    int readval;
    int i;
    // i'm so sorry
    for(i = 0; i < 16 && (readval = read(STDIN_FILENO, dict+i,1)) == 1; i++)
    {
       fprintf(stderr,"%i: %c\n",i,dict[i]);
    }

   // printf("dictioary built\n");
    if(readval <= 0) return READ_ERROR;
    
    bitIndex = 0;
    readval = read(STDIN_FILENO, &readBuffer, 1);
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

            fprintf(stderr,"infrequent symbol: %c\n",symbol);

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

            // now we get the dictionary code
            unsigned char dictCode = 0;
            readval = readBits(&dictCode, 4);

            // if its less than 0, we're returning an error code
            if (readval < 0) return readval;

            // if we read less than the 4 bits we expected, EOF, we're done
            if (readval < 4) break;

            // pull the real character from the dictionary
            unsigned char realChar = dict[dictCode];
            fprintf(stderr,"frequent symbol: %c\n",realChar);

            // write it the number of times equal to the run length
            for (i = 0; i <= runLength; i++)
            {
                if (write(STDOUT_FILENO, &realChar, 1) != 1) return WRITE_ERROR;
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
