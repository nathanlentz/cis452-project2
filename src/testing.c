#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>


int main(int argc, char *argv[]){
	unsigned char buffer; // 1 byte
	fread(&buffer, 1, 1, argv[1]);
	printf("Read %c", buffer);
}



