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
