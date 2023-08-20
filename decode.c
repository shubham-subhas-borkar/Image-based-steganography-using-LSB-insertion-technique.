#include <stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>
#include "common.h"

/*Defining function to get file names and validate
 *Input : stego.bmp file
 *output: data hidden in stego.bmp image is decoded in output.txt file
 *return value:e_success or e_failure, on file errors
 */


//function defination to read and validate arguments for decoding 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] != NULL)
    {
	if (strstr(argv[2],".bmp"))                             //checking 3rd argument is .bmp file or not
	{
	    decInfo->stego_image_fname = argv[2];               //assigning argv[2] to structure member stego_image_fname
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	return e_failure;
    }
    if (argv[3] != NULL)                                        //checking 4th argument is NULL or not
    {                                                    
	strcpy(decInfo->output_fname,argv[3]);                  //if true strcpy argv[3] to output_fname
    }
    else
    {
	strcpy(decInfo->output_fname,"output");                 //else strcpy "output" to output_fname
    }
    return e_success;
}

//function defination to open files for decoding
Status open_decode_files(DecodeInfo *decInfo)
{
    //opeinig stego image file in read mode
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname,"r");

    //validating while opening the file
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");         
	fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->stego_image_fname);
	return e_failure;
    }
    return e_success;
}

//function defination to decode magic string
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    //set the file pointer to 54 in source image
    fseek(decInfo -> fptr_stego_image,54,SEEK_SET);
    char magic_data[(strlen(magic_string) + 1)];   

    //call decode_data_from_image to decode magic string from the stego image
    decode_data_from_image(magic_data,strlen(magic_string),decInfo-> fptr_stego_image);
    magic_data[strlen(magic_string)] = '\0'; 
    if (strcmp(magic_data,MAGIC_STRING) == 0) 
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

//generic function defination to decode data from the stego image
Status decode_data_from_image(char *data, long size, FILE *fptr_stego_image)
{
    int i;
    char str[8];      
    for (i = 0; i < size; i++)
    {
	//read 8 bytes from the stego image and store it in an str
	fread(str,8,1,fptr_stego_image);

	//call the function decode_byte_from_lsb to decode ecah character
	decode_byte_from_lsb(&data[i],str);
    }
}

//generic function defination to decode each chacter from stego image
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    int i;
    char ch = 0x00;                                       //declaring char ch and assign 0x00
    for (i = 0; i < 8; i++)
    {
	ch = ch | ((image_buffer[i] & 1) << i);           //bitwise operation to get each character
    }
    *data = ch;

}

//function defination to decode secret file extn size from the stego image
Status decode_secret_file_extnsize(DecodeInfo *decInfo)
{
    char str[32];  

    //read 32 bytes from stego image and store it in str 
    fread(str,32,1,decInfo -> fptr_stego_image);

    //call function to decode size from the stego image
    decode_size_from_lsb(&decInfo -> extn_size,str);
    return e_success;
}

//generic funstion defination to decode size from the stego image
Status decode_size_from_lsb (long *data, char *image_buffer)
{
    int i;
    int size = 0x00;                                            //declaring int size and assigning 0x00
    for (i = 0; i < 32; i++)
    {
	size = size | ((image_buffer[i] & 1) << i);             //bitwise operation to get size 
    }
    *data = size;
}

//function defination to decode secret file extn from stego image
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char extn_data[decInfo -> extn_size];  

    //call data_from_image to decode secret file extn from the stego image
    decode_data_from_image(extn_data,decInfo->extn_size,decInfo->fptr_stego_image);

    //strcat the output_fname and extn_data and assign it to output_file_fname
    decInfo->output_file_fname = strcat(decInfo->output_fname,extn_data);

    //open the file output_file_fname in write mode
    decInfo->fptr_output_file = fopen(decInfo->output_file_fname,"w");

    //check for file opened or not
    if (decInfo->fptr_output_file == NULL)
    {
	perror("fopen");
	fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->output_fname);
	return e_failure;
    }
    return e_success;
}

//function defination to decode secret file size from the stego image
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char str[32];

    //read 32 bytes from the stego image and store it in str
    fread(str,32,1,decInfo-> fptr_stego_image);

    //call decode_size_from_lsb to decode size from the stego image
    decode_size_from_lsb(&decInfo-> secret_size,str);
    return e_success;
}

//function defination to decode secret data from the stego image
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char secret_data[decInfo-> secret_size];

    //call decode_data_from_image to decode secret data from the stego image
    decode_data_from_image(secret_data,decInfo->secret_size,decInfo->fptr_stego_image);

    //write the secret_data into output file
    fwrite(secret_data,decInfo->secret_size,1,decInfo->fptr_output_file);
    return e_success;
}


//function defination to do decoding
Status do_decoding(DecodeInfo *decInfo)
{
    //checking files opened successfully or not
    if (open_decode_files(decInfo) == e_success)
    {
	printf("open files for decoding success\n");

	//checking magic string decodeed successfully or not
	if (decode_magic_string(MAGIC_STRING,decInfo) == e_success)
	{
	    printf("Decoding magic string success\n");

	    //checking the extn size decodeed successfully or not
	    if (decode_secret_file_extnsize(decInfo) == e_success)
	    {
		printf("Decoding extn size success\n");

		//checking the file extn is decodeed successfully or not
		if (decode_secret_file_extn(decInfo) == e_success)
		{
		    printf("Decoding extn  data success\n");

		    //checking secret file size is decodeed successfully or not
		    if (decode_secret_file_size(decInfo) == e_success)
		    {
			printf("Decoding secret size success\n");

			//checking secret file data is decodeed successfully or not
			if (decode_secret_file_data(decInfo) == e_success)
			{
			    printf("Decoding secret file data success\n");
			}
			else    //else returning failure if any condition get falls
			{
			    printf("Decoding secret file data failure\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("Decoding secret size failure\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("Decoding extn data failure\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("Decoding extn size failure\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Decoding magic string failure\n");
	    return e_failure;
	}
    }
    else
    {
	printf("open files for decoding failure\n");
	return e_failure;
    }
    return e_success;                          //returning success if all conditions are true
}
