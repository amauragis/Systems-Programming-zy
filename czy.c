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

unsigned char buffer[BUF_SIZE];
unsigned char readBuffer;
int bitIndex;

// writeBits
//  Writes a number of bits to stdout.
//  
//  note: is buffered, must flushBits afterward.
//
//  value:  binary value to be written. Data is shifted so it's lsb matches the
//          variable's lsb
//  size:   number of bits to be written <= 32
int writeBits(unsigned int val, unsigned char size)
{
    // Calculate buffer position and bit position in current byte
    int currByte = (bitIndex / 8);
    char remainingBits = (8 - (bitIndex % 8));
    fprintf(stderr,"currByte: %i\n", currByte);
    fprintf(stderr,"remainingBits: %i\n", remainingBits);

    // Check if buffer is full
    if (((BUF_SIZE*8) - bitIndex) < size)
    {
        fprintf(stderr,"buffer full\n");
        // Write all bytes up to the current one
        if (write(STDOUT_FILENO, &buffer, currByte) != currByte)
        {
            return WRITE_ERROR;
        }

        if (currByte < BUF_SIZE)
        {
            // Update the buffer by moving remaining data down to bottom
            buffer[0] = buffer[currByte];
            
            // Clear remaining buffer
            memset(buffer+1, 0, (BUF_SIZE-1));
        }
        else
        {
            // All bytes were written, clear entire buffer
            memset(buffer, 0, BUF_SIZE);
        }

        // move the current bit index
        bitIndex -= currByte*8;

        // Recalc current byte and remaining bits
        currByte = (bitIndex / 8);
        remainingBits = (8 - (bitIndex % 8));
    }

    // strip off out of bound bits
    val <<= ((sizeof(int)*8) - size);
    val >>= ((sizeof(int)*8) - size);

    // Add val to buffer to fill bytes
    while(size > remainingBits)
    {
        // Calculate number of bits to write
        unsigned char numBits = (size - remainingBits);

        // Write bits to current byte
        buffer[currByte] |= (val >> numBits);

        // Set written bits to zero
        val <<= ((sizeof(int)*8) - numBits);
        val >>= ((sizeof(int)*8) - numBits);

        // Increment current bit
        bitIndex += remainingBits;

        // Decrement size remaining
        size -= remainingBits;

        // Recalc 
        currByte = (bitIndex / 8);
        remainingBits = (8 - (bitIndex % 8));
    }

    // Finish remaining bits
    buffer[currByte] |= (unsigned char)(val << (remainingBits - size));

    // Increment current bit
    bitIndex += size;

    // Write completed successfully
    return 0;
}

// flushBits
//  Writes the remaining bits in the buffer to stdout
//  Note: trailing bits in the final byte will be zeros
int flushBits()
{
    // Calculate the most recently written to byte
    int currByte = ((bitIndex - 1) / 8);

    // Only write if there is data in the buffer
    if(bitIndex != 0)
    {
        // Write all bytes including the current one to output
        if(write(STDOUT_FILENO, &buffer, currByte+1) != (currByte+1))
        {
            return WRITE_ERROR;
        }
    }

    // Write completed successfully
    return 0;
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

    // print dictionary 
    int i;
    unsigned char dict[16];
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
    fprintf(stderr,"dictionary completed\n");
    // now we go to the actual run-length coding algorithm

    // seek stdin back to beginning.
    if(lseek(STDIN_FILENO, 0, SEEK_SET) < 0) return SEEK_ERROR;

    // read from standard in until we can't
    while((readcheck = read(STDIN_FILENO, &currChar,1)) == 1)
    {
        unsigned int prefix;

        // index in dictionary, this will be the frequent character code also
        int dictIndex = charInDict(currChar,dict);
        
        // character is in dictionary
        if (dictIndex >= 0)
        {
            fprintf(stderr, "frequent: %c\n",currChar);
            unsigned int runLength = 0;
            unsigned char checkChar = currChar;

            // increment run length until we:
            //  - reach the end of the file
            //  - reach a different character
            //  - exceed the maximum run length
    
            while(1 == read(STDIN_FILENO, &currChar, 1) &&
                  checkChar == currChar &&
                  runLength < MAX_RUN_LENGTH)
            {
                if (checkChar != currChar)
                {
                    lseek(STDIN_FILENO, -1, SEEK_CUR);
                    break;
                }
                runLength++;
            }
            

            // now we create the encoded character, frequent with a zero prepended
            prefix = 0;
            runLength <<= 4;
            unsigned int encodedChar = 0;
            encodedChar |= prefix | runLength | dictIndex;

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
            fprintf(stderr,"nonfrequent: %c\n",currChar);
            // character is nonfrequent, therefore gets a 1 prepended
            prefix = 1;
            prefix <<= 8;
            unsigned int encodedChar = 0;
            encodedChar |= prefix | currChar;

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
