#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>
int main(int argc, char *argv[])                                                //MAIN function to pass CML Arguments
{
    int option = check_operation_type(argv);                                    //check_operation_type functiton() call to check the operation type          
    if (option == e_encode)                                                     //checking condition for encoding operation                          
    {
	printf(".......Selected encoding.......\n");                  
	EncodeInfo encInfo;                                                      //declaring structure variable for encoding                              
	if (read_and_validate_encode_args(argv,&encInfo) == e_success)           //function call for read and validate encoding arguments
	{
	    printf("Read and validate arguments for encoding success\n");
	    if (do_encoding(&encInfo) == e_success)                               //function call to check do_encoding
	    {
		printf("## Encoding Successfull ##\n");                           //if true, printing encoding successful
	    }
	    else
	    {
		printf("Encoding failure\n");                                     //else encoding failure
	    }
	}
	else
	{
	    printf("Read and validate arguments for encoding failure\n"); 
	}
    }
    else if (option == e_decode)                                                 //checking condition for decoding operation
    {
	printf(".......Selected decoding.......\n");
	DecodeInfo decInfo;                                                      //declaring structure variable for decoding operation
	if(read_and_validate_decode_args(argv,&decInfo) == e_success)            //function call for read and validate decoding arguments
	{
	    printf("Read and validate arguments for decoding success\n");       
	    if (do_decoding(&decInfo)==e_success)                                //function call to check do_decoding 
	    {
		printf("## Decoding Successfull ##\n");                          //if true, printing decoding succcessful
	    }
	    else
	    {
		printf("Decoding secret data failure\n");                        //else decoding failure
	    }
	}
	else
	{
	    printf("Read and validate arguments for decoding failure\n");
	}
    }
    else
    {
	printf("Please pass the valid option\n");
    }
    return 0;
}
