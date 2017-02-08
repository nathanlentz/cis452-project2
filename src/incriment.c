#include <stdio.h>


/*************************************************
* Incriment will be executed by C2 as part of
* Streamed Vector Processing
*
* execpv will call it and it should return 
* the incrimented bit
**************************************************/

int main(int argc, char *argv[])
{
	if(argc != 2){
		return -1;
	}

	int bit = argv[1];

	if(bit == 0){
		bit = 1;
	}
	else if(bit == 1){
		bit = 0;
	}
	else{
		return -1;
	} 
	return bit;
}