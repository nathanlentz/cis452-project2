#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

/*********************************************************** 
* CIS 452 - Project 2: Streamed Vector Processing
* @author: Nathan Lentz
* @date: 2/7/2017 - 2/16/2017
* Demonstrates communication across multiple processes
* to compute bit subtraction in parallel
************************************************************/

/* Global Variables */
struct stat st = {0};
FILE *logger;



/* Prototypes */
int initializeLogger();
int findBinaryLength(FILE*);
int findVectorLength(FILE*);



/********************************************************** 
* Entry point for program
* - could I write size to the pipe?
*
**********************************************************/

int main(int argc, char *argv[])
{
	/* Create Logging Dependencies for this session */
	if(initializeLogger() != 0){
		printf("Unable to begin logging session. Exiting.\n");
		exit(1);
	}
	else {
		fprintf(logger, "-- Logger Initialized\n");
	}

	// Check if arguments are complete
	if(argc != 3){
		fprintf(stderr, "There are not enough arguments\n\n");
		fprintf(logger, "-- Not enough arguments. Exiting\n");
		exit(1);
	}

	printf("Spinning things up. . .\n");
	fprintf(logger, "-- Reading file dimensions\n");

	FILE* vectorA;

	//TODO: only allow parent to see file
	if((vectorA = fopen(argv[1], "r")) == NULL){
		perror(argv[1]);
		exit(1);
	}

	int binaryLen = findBinaryLength(vectorA);
	int vectorLen = findVectorLength(vectorA); 
	

	/*******************************************************
	* Parent - The Complimenter
	*
	********************************************************/
	int fd[2];
    if (pipe (fd) < 0) { 
        perror ("Unable to create pipe 1"); 
        exit(1); 
    } 
	

	/* Child 1 - C1 */


	/* Child 2 - C2 */

	fclose(vectorA);
	fclose(logger);
	return 0;
}


/********************************************************** 
* Sets up logger for current session
**********************************************************/
int initializeLogger()
{
	struct tm *localTime;
	time_t currentTime;

	// Get current time of system
	currentTime = time(NULL);

	// Get current time to local time
	localTime = localtime(&currentTime);

	if(stat("tmp", &st) == -1){
		mkdir("tmp", 0700);
	}

	logger = fopen("tmp/log.txt", "a");
	fprintf(logger, "\n\n\nRun: %s\n", asctime(localTime));
	fprintf(logger,"========================================\n\n");
	
	return 0;
}

/********************************************************** 
* Finds the length of each binary number
**********************************************************/
int findBinaryLength(FILE* vector)
{
	int c;
	int length = 0;
	while((c = fgetc(vector)) != '\n'){
		length++;
	}

	return length-1;
}

/********************************************************** 
* Finds the total number of binary numbers
**********************************************************/
int findVectorLength(FILE* vector)
{
	int c;
	int length = 0;
	while((c = fgetc(vector)) != EOF){
		if(c == '\n'){
			length++;
		}
	}

	// Plus 1 because there is no new line at EOF
	return length+1;
}