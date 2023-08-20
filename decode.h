#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define MAX_OUTPUT_FILE 20

typedef struct _DecodeInfo
{
    /* stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    long extn_size;
    long secret_size;

    /* output file info */
    char output_fname[MAX_OUTPUT_FILE];
    char *output_file_fname;
    FILE *fptr_output_file;

} DecodeInfo;


/* Decoding function prototype */


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* get Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/*get secret file extn size*/
Status decode_secret_file_extnsize(DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* decode function, which does the real decoding */
Status decode_data_from_image(char *data, long size, FILE *fptr_stego_image);

/* decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/*decode a size from LSB of str array */
Status decode_size_from_lsb (long *data, char *image_buffer);

#endif
