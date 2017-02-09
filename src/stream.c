#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define READ 0 
#define WRITE 1 
#define MAX 1

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
int isPaused = 0;



/* Prototypes */
int initializeLogger();
int findBinaryLength(FILE*);
int findVectorLength(FILE*);
void pauseProcesses();
void unpauseHandler(int signal);
void acknowledgeParentHandler(int signal);



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

	signal(SIGINT, unpauseHandler);
	fprintf(logger, "-- Binding ^C (SIGINT) signal to handler for all processes\n");
	


	int fd[2];
	int fd2[2];
	int secondPid;
	//int status;

    if (pipe (fd) < 0) { 
        perror ("Unable to create pipe 1"); 
        exit(1); 
    }

    // Create First Child Process
    if ((secondPid = fork()) < 0) { 
        perror ("fork failed"); 
        exit(1); 
    }  

    /* Second Process - C1 */
    if (secondPid == 0){
    	// Close write from pipe
    	//int logId = 1;

    	close(fd[WRITE]);
    	//fclose(stdout);
    	fclose(stdin);
    	

		int thirdPid;
		int status2;
		if(pipe(fd2) < 0){
			perror("Unable to create pipe 2");
			fprintf(logger, "Failed to create pipe 2\n");
			exit(1);
		}

		if((thirdPid = fork()) < 0){
			perror("fork failed");
			fprintf(logger, "Failed to create second fork");
			exit(1);
		}

		

		/***************************************************
		* Third Process (C2). Read VectorA and perform 
		* addition on A + (-B). B is read from pipe
		***************************************************/

		if(thirdPid == 0){
			close(fd[READ]);
			close(fd2[WRITE]);
			pauseProcesses();

		}

		else {
	    	int bit = 0;
    
	    	while(bit != 2){
		    	read(fd[READ], &bit, sizeof(int));
		    	printf("Child received %i\n", bit);
	    	}

		}

    	// try to read from pipe
    




    }
	
	/*******************************************************
	* Parent - The Complimenter. Read from Vector B
	* and do compliment for each bit
	********************************************************/
	else {
		//int logId = 0;
		// Close read from pipe
		close(fd[READ]);
		fclose(stdout);
		fclose(stdin);

		pauseProcesses();

		fprintf(logger, "-- Reading file dimensions\n");

		FILE* vectorB;

		// Open file B
		if((vectorB = fopen(argv[2], "r")) == NULL){
			perror(argv[2]);
			exit(1);
		}
		// Turn char into int
		

		// Find file dimensions and send through pipe
		// char dimensions[1];
		// dimensions[0] = findBinaryLength(vectorB);
		// dimensions[1] = findVectorLength(vectorB); 
		
		int bit = 0;
		int c;

		while((c = fgetc(vectorB)) != '\n'){
			bit = c - '0';
			if(bit == 0){
				bit = 1;
			} else {
				bit = 0;
			}
			write(fd[WRITE], &bit, sizeof(int));
		}
		// 2 Might mean end of binary number
		// 3 might mean EOF
		if(c == '\n'){
			bit = 2;
			write(fd[WRITE], &bit, sizeof(int));
		}

    	fclose(vectorB);

	}



	/* Child 2 - C2 */

	
	fclose(logger);
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

void pauseProcesses(){
	
	printf("\nAll processes ready to go: Ctrl + C to begin\n");
	fprintf(logger, "** Processes and pipes created. Waiting for signal to begin\n");
	fflush(stdout);
    while(1){
		if(isPaused == 1){
			fprintf(stdout, " recieved! Beginning\n");
			fprintf(logger, "--^C Received from user, beginning streaming\n");
			break;
		}
	}

}


/***********************************************************
* Signal Handler designed to begin execution of 
* vector processing from paused state. Triggered from ^C
*
*
************************************************************/
void unpauseHandler(int signal)
{
		if(signal == SIGINT) {
			isPaused = 1;
		}
}


/*************************************************************
* Signal Hanlder for letting parent of a process know that
* they have received all bits for a binary number on one line
**************************************************************/

void acknowledgeParentHandler(int signal)
{
	// Send this signal to parent to let them know they are done

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

