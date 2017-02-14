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
int pid;
int nextLine = 0;


//int numberLength;
//int vectorLength;

/* Prototypes */
int initializeLogger();
int findBinaryLength(FILE*);
int findVectorLength(FILE*);
void pauseProcesses();
void unpauseHandler(int signal);
void killAll(int signal);
void acknowledgeParentHandler(int signal);
void recievingCompleteHandler(int signal);

/********************************************************** 
* Entry point for program
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
	//int fd2[2];
	
	int status;

    if (pipe (fd) < 0) { 
        perror ("Unable to create pipe 1"); 
        exit(1); 
    }

    // Create First Child Process
    if ((pid = fork()) < 0) { 
        perror ("fork failed"); 
        exit(1); 
    }  

    /* Second Process - C1 */
    if (pid == 0){

    	// Close write from pipe
    	close(fd[WRITE]);
    	//fclose(stdout);
    	//fclose(stdin);

		FILE* output;
		if((output = fopen("tmp/output.txt", "w+")) == NULL){
			perror("Cannot open output");
			exit(1);
		}
		fflush(stdout);
		fflush(stdin);

		FILE* vectorA;
		if((vectorA = fopen(argv[1], "r")) == NULL){
			perror(argv[1]);
			exit(1);
		}

		int bit = 0;
		int carry = 0;
		int isFirstBit = 0;
    	/* Loop for handling read/write */
		while(1){
	    	do{
	    		read(fd[READ], &bit, sizeof(int));
	    		printf("Child received %i\n", bit);
		    	if(bit == 3){
					fprintf(logger, "-- C1: Recieved end of file. Sending signal to parent\n");
					fclose(output);
					kill(getppid(), SIGUSR2);
		    		exit(0);
					//wait(&status2);
		    		// Notify Parent we've gotten all characters
		    	}
		    	else if(bit == 2){
					isFirstBit = 0;
					carry = 0;
		    		fprintf(logger, "-- C1: Recieved end of line. Sending signal to parent\n");
		    		kill(getppid(), SIGUSR1);
					fprintf(output, "\n");
		    	}
		    	else {
					if(isFirstBit == 1 && carry == 1){
						if(bit == 1){
							bit = 0;
							carry = 1;
						}
						if(bit == 0){
							bit = 1;
							carry = 0;
						}
					}
					else if(isFirstBit == 0){
						if(bit == 1){
							bit = 0;
							carry = 1;
						}
						else if(bit == 0){
							bit = 0;
							carry = 0;
						}
					}
					fprintf(logger, "-- C1: Writing %i to output\n", bit);
					fprintf(output, "%i", bit);
					isFirstBit = 1;
				}
		    	// TODO Perform Incriment

		    	// Write to C2
		    	//write(fd2[WRITE], &bit, sizeof(int));
	    	} while(bit != 2);
		    
		}
	    exit(0);
    }
	
	/*******************************************************
	* Parent - The Complimenter. Read from Vector B
	* and do compliment for each bit
	********************************************************/
	else {
		//int logId = 0;
		// Close read from pipe
		close(fd[READ]);
		//fclose(stdout);
		//fclose(stdin);
		signal(SIGUSR1, acknowledgeParentHandler);
		signal(SIGUSR2, recievingCompleteHandler);
		pauseProcesses();
		signal(SIGINT, killAll);

		fprintf(logger, "-- Opening VectorB for ereading\n");

		FILE* vectorB;

		// Open file B
		if((vectorB = fopen(argv[2], "r")) == NULL){
			perror(argv[2]);
			exit(1);
		}
		
		// while((c = fgetc(vectorB)) != EOF){
		// 	if(bit != 2 || bit != 3){
		// 		bitA = c - '0';
		// 		if(bit = 0 )
		// 	}
		// }

		// 2 Might mean end of binary number
		// 3 might mean EOF
		int bit = 0;
		int c;
		while((c = fgetc(vectorB)) != EOF){
			if(c == '\n'){
				nextLine = 1;
				bit = 2;
			}
			else {

				bit = c - '0';
				if(bit == 0){
					bit = 1;
				} else {
					bit = 0;
				}
			}
			fprintf(logger, "-- P: Sending bit %i to child\n", bit);
			write(fd[WRITE], &bit, sizeof(int));
		}

		if(c == EOF){
			fprintf(logger, "-- P: Reached EOF, sending 3 to child\n");
			bit = 3;
			write(fd[WRITE], &bit, sizeof(int));
		}

    	fclose(vectorB);
    	wait(&status);

	}



	/* Child 2 - C2 */
	fprintf(logger, "-- Terminating Program\n");
	fclose(logger);
	return 0;
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
	if(pid > 0){
		fprintf(logger, "Child recieved end of a line\n");
		nextLine = 0;
	}	
}

void recievingCompleteHandler(int signal){
	if(pid > 0){
		printf("\nChild Finished Recieving");
		fprintf(logger, "Child finished recieving\n");
		kill(pid, SIGKILL);
		exit(0);
	}
}

void killAll(int signal)
{
	if(pid > 0){
		fprintf(logger, "Recieved instruction to kill\n");
		kill(pid, SIGKILL);
		exit(0);
	}
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

