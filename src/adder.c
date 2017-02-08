#include <stdio.h>
#include <stdlib.h>


/*************************************************
* Adder will take two bits and subtract A - B and
* output the result as a new int, C
**************************************************/

int main(int argc, char *argv[])
{
	if(argc != 3){
		printf("Not enough arguments to add\n");
		return -1;
	}

	int a, b;
	int result;
	// Assuming that A is argv[1] and B is argv[2]

	sscanf(argv[1], "%d", &a);
	sscanf(argv[2], "%d", &b);
	// Bit Addition
	if(a && b == 0){
		result = 0;
	}
	else if((a == 0 && b == 1) || (a == 1 && b == 0)){
		result = 1;
	}
	else if(a && b == 1){
		result = 10;
	}

	return result;

	//Echo $? is where the last program returned value is
}