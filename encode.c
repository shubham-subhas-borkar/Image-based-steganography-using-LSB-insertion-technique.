#include<stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
//defining function to get the file size
uint get_file_size(FILE *fptr)
{
    //setting file pointer to the end position
    fseek(fptr,0,SEEK_END);                               //file pointer at the EOF using fseek funcn
    return ftell(fptr);                                   //returning the FOF position size of file by ftell funcn
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "r");
    // Do Error handling
    if (encInfo -> fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "r");
    // Do Error handling
    if (encInfo -> fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo -> fptr_stego_image = fopen(encInfo -> stego_image_fname, "w");
    // Do Error handling
    if (encInfo -> fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

OperationType check_operation_type(char *argv[])                                   //function defination for operation type
{
      if (argv[1] != NULL)                                                           //check 2nd argument 
      {
      if (!(strcmp(argv[1],"-e")))                                               //true then check for operation type
      {
          return e_encode;                                                       //true then returning encoding
      }
      else if (!(strcmp(argv[1],"-d")))                                          //if false,check for decoding
      {
          return e_decode;                                                       //if true, returning decoding
      }
      else
      {
          return e_unsupported;                                                  //if both are false, returning error
      }
      }
      else
      {
      return e_unsupported;                                                      //error if 2nd argument is false
      }
}


/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *ptr;
    if (argv[2] != NULL)                                                    //check 3rd argument 
    {
	if (strstr(argv[2],".bmp"))                                         //check 3rd argument is .bmp file
	{
	    encInfo -> src_image_fname = argv[2];                           //assign argv[2] to structure 
	}
	else
	{
	    return e_failure;                                               //return failure
	}
    }
    else
    {
	return e_failure;                                                   //return failure
    }
    if (argv[3] != NULL)
    {
	if (ptr = strchr(argv[3],'.'))                                      //check that 4th argument is having extension 
	{
	    strcpy(encInfo -> extn_secret_file,ptr);                        //assign extension to structure member
	    encInfo -> secret_fname = argv[3];                              //assign argv[3] to structure member
	}
	else
	{
	    return e_failure;                                               //return failure
	}
    }
    else
    {
	return e_failure;
    }
    if (argv[4] == NULL)                                                    //check that 5th argument is present
    {
	encInfo -> stego_image_fname = "stego.bmp";                         //assigning default name to structure member
    }
    else
    {
	if (strstr(argv[4],".bmp"))                                       
	{
	    encInfo -> stego_image_fname = argv[4];                         //assigning same name to structure member
	}
	else
	{
	    return e_failure;
	}
    }
    return e_success;                                                      //returning success
}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    //optaining image capacity by calling the function 
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

    //optaining size of secret file by calling the function 
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    //check capacity is enough to hold the secret data
    if (encInfo -> image_capacity > (54+(strlen(MAGIC_STRING)*8)+32+(strlen(encInfo -> extn_secret_file)*8)+32+encInfo->size_secret_file*8))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

//function defination to copy bmp header of source file to detination bmp file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];  
    //setting file pointer to 1st character
    fseek(fptr_src_image,0,SEEK_SET);
    //read 54 bytes header from the source bmp image to str 
    fread(str,54,1,fptr_src_image);
    //write 54 byte header from the str to destination bmp image
    fwrite(str,54,1,fptr_dest_image);
    return e_success;
}

//function defination to encode the magic string
Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{
    //setting the file pointer to 54 position in source image
    fseek(encInfo -> fptr_src_image,54,SEEK_SET);

    //each element of magic string encoded one after other by calling function encode_data_to_image
    encode_data_to_image(magic_string,strlen(magic_string),encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    return e_success;
}

//generic function defination to encode data to image
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    int i;
    char image_buf[8];
    for (i = 0; i < size; i++)
    {
	//read the 8 bytes from the source image and store in image_buf
	fread(image_buf,8,1,fptr_src_image);
	//encode byte by byte from the source image by function call encode_byte_to_lsb
	encode_byte_to_lsb(data[i],image_buf);
	//write the 8 bytes from the image_buf to stego_image
	fwrite(image_buf,8,1,fptr_stego_image);
    }
}

//generic function defination to  encode byte to lsb
Status encode_byte_to_lsb (char data, char *image_buffer)
{
    int i;

    //each byte of data requires 8 byte to make it as lsb
    for (i = 0; i < 8; i++)
    {
	//operation to perform encoding data 
	image_buffer[i] = image_buffer[i] & 0xFE;
	image_buffer[i] = image_buffer[i] | ((data & (1 << i)) >> i);
    }
}

//function defination to encode secret file extension size
Status encode_secret_file_extnsize(int extn_size, EncodeInfo *encInfo)
{
    char str [32]; 

    //operation to encode secret file extension size from source to destination
    fread (str,32,1,encInfo -> fptr_src_image);
    encode_size_to_lsb(extn_size,str);
    fwrite(str,32,1,encInfo -> fptr_stego_image);
    return e_success;
}

//generic function defination to encode size to lsb
Status encode_size_to_lsb (int data, char *image_buffer)
{
    int i;
    //ecah size requires 32 bytes to amke it as lsb
    for (i = 0; i < 32; i++)
    {
	//operation to perform encoding of size
	image_buffer[i] = image_buffer[i] & 0xFE;
	image_buffer[i] = image_buffer[i] | ((data & (1 << i)) >> i);
    }
}

//defining function encode_secret_file_extn
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    //each byte of secret file extension encoded one after other by calling function encode_data_to_image
    encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

//function defination to encode secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char str[32]; 

    //encoding 32 bytes of data size from source to destination
    fread(str,32,1,encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size,str);
    fwrite(str,32,1,encInfo -> fptr_stego_image);
    return e_success;
}

//function defination to encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //set file pointer of secret file to first chracter
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char str[encInfo->size_secret_file]; 

    //read the secret file data and store it an array str
    fread (str,encInfo->size_secret_file,1,encInfo->fptr_secret);

    //encode secret file data by calling the function encode_data_to_image
    encode_data_to_image(str,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

//function defination to copy remaining img data
Status copy_remaining_img_data (FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    //copying remaining data from source image to destin image byte by byte untill it reaches EOF
    while ((fread(&ch,sizeof(char),1,fptr_src)) > 0)
    {
	fwrite(&ch,sizeof(char),1,fptr_dest);
    }
    return e_success;
}

//function defination to perform encoding
Status do_encoding(EncodeInfo *encInfo)
{
    //checking files opened successfully or not
    if (open_files(encInfo) == e_success)
    {
	printf("opening files for encoding success\n");

	//checking destn image has capacity to hold the secret data
	if (check_capacity(encInfo) == e_success)
	{
	    printf("Capacity checking success\n");

	    //checking source file header copied successfully to destn file or not
	    if (copy_bmp_header(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
	    {
		printf("BMP header copy success\n");

		//checking magic string encoding success or not
		if (encode_magic_string(MAGIC_STRING,encInfo) == e_success)
		{
		    printf("Encode magic string success\n");

		    //checking secret file extnsize encoding success or not
		    if (encode_secret_file_extnsize(strlen(encInfo -> extn_secret_file),encInfo) == e_success)
		    {
			printf("Encode extn size success\n");

			//checking secret file extn encoding success or not
			if (encode_secret_file_extn(encInfo -> extn_secret_file,encInfo) == e_success)
			{
			    printf("Encode extn data success\n");

			    //checking secret file size encoding success or not
			    if (encode_secret_file_size(encInfo -> size_secret_file,encInfo) == e_success)
			    {
				printf("Encode secret file size success\n");

				//checking secret file data encoding success or not
				if (encode_secret_file_data(encInfo) == e_success)
				{
				    printf("Encode secret file data success\n");

				    //checking remaining data is copied to destn image or not
				    if (copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
				    {
					printf("Copy remaining data success\n");
				    }
				    else   //all else conditions returning failure for each function                      
				    {
					printf("Failed to copy remaining data\n");
					return e_failure;
				    }
				}
				else
				{
				    printf("Encode secret file data failure\n");
				    return e_failure;
				}
			    }
			    else
			    {
				printf("Encode secret file size failure\n");
				return e_failure;
			    }
			}
			else
			{
			    printf("Encode extn data failure\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("Encode extn size failure\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("Encode magic string failure\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("BMP header copy failure\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Capacity checking failure\n");
	    return e_failure;
	}
    }
    else
    {
	printf("opening files for encoding failure\n");
	return e_failure;
    }
    //returning success if all conditions get true
    return e_success;                                    
}






