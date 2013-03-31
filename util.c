#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "zy.h"

unsigned char buffer[BUF_SIZE];
unsigned char readBuffer;
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

// writeBits
//  Writes a number of bits to stdout.
//  
//  note: is buffered, must flushBits afterward.
//
//  value:  binary value to be written. Data is shifted so it's lsb matches the
//          variable's lsb
//  size:   number of bits to be written <= 32
int writeBits(unsigned int val, char size)
{
    // Calculate buffer position and bit position in current byte
    int currByte = (bitIndex / 8);
    char remainingBits = (8 - (bitIndex % 8));

    // Check if buffer is full
    if (((BUF_SIZE*8) - bitIndex) < size)
    {
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

// readBits
//  Reads bits from stdin
//
// data:   pointer to a char array to hold data
// length: number of bits to read
int readBits(unsigned char* data, unsigned int length)
{
    unsigned int returnedBits = 0;

    // bits in buffer
    unsigned char remainingBits = (8 - bitIndex);

    // get bits from the buffer

    while (length > remainingBits)
    {

        // move bits from buffer into data
        *data |= (readBuffer << (length - (unsigned char)remainingBits));

        // put a new byte in the buffer
        int readval = read(STDIN_FILENO, &buffer, 1);
        if (readval < 0) return READ_ERROR;
        if (readval == 0) return returnedBits; // EOF, done.

        // update length and number of bits to return and reset buffer
        length -= remainingBits;
        returnedBits += remainingBits;
        bitIndex = 0;
        remainingBits = 8;
    }

    // put rest of bits in data
    unsigned char bitsLeft = remainingBits - length;
    *data |= (readBuffer >> bitsLeft);

    // clear bits that we've written
    readBuffer <<= (8 - bitsLeft);
    readBuffer >>= (8 - bitsLeft);

    returnedBits += length;
    bitIndex += length;


    return returnedBits;
}
