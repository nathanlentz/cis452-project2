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

/*********************************************************** 
* CIS 452 - Project 2: Streamed Vector Processing
* @author: Nathan Lentz
* @date: 2/7/2017 - 2/16/2017
* Demonstrates communication across multiple processes
* to compute bit subtraction in parallel
************************************************************/

/* Global Variables */
typedef enum { false, true } bool;
int isPaused;
int incrementerId;
int adderId;
bool incrementerHasRead = true;
bool adderHasRead = true;
int TTK = 2;
// ZERO FOR FALSE, ONE FOR TRUE

/* Prototypes */
void pauseProcesses();
void recievedEOFHandler(int signal);
void unpauseHandler(int signal);
void readLineHandler(int signal);
void readNextLineHandler(int signal);

/* Main Entry Point for Program */
int main(int argc, char *argv[])
{
    signal(SIGINT, unpauseHandler);
    /* Check to ensure there are enough arguments */
    // argv[0] - Program 
    // argv[1] - Input File A
    // argv[2] - Input File B
    // argv[3] - Binary Number Length
    // argv[4] - Number of values
    if(argc != 5){
		fprintf(stderr, "There are not enough arguments\n\n");
		exit(1);
	}

    printf("Spinning things up. . .\n");

    int numberLen = atoi(argv[3]);
    int numberOfValues = atoi(argv[4]);
    
    // Create compToIncPipe
    int compToIncPipe[2];
    int incrementerStatus;

    if (pipe (compToIncPipe) < 0) { 
        perror ("Unable to create pipe between Complimentor and Incrementer"); 
        exit(1); 
    }

    // Create Incrimenter Process
    if ((incrementerId = fork()) < 0) { 
        perror ("fork failed"); 
        exit(1); 
    }  

    pauseProcesses();

    /* Complimentor Process */ 
    if(incrementerId > 0) { // Parent
        // Close unnecessary file descriptors
        close(compToIncPipe[READ]);

        // Map Signal Handler 
        signal(SIGUSR1, readLineHandler);
        signal(SIGUSR2, readNextLineHandler);

        FILE* vectorB;
        if((vectorB = fopen(argv[2], "r")) == NULL){
            perror(argv[2]);
             exit(1);
        }
        // Try reading line by line
        char buffer[numberLen+2]; // New line and null terminating
        int i;
        while (fgets(buffer, sizeof(buffer), vectorB) != NULL)  {
            // Report what read was
            while(1){
                if(incrementerHasRead && adderHasRead){
                    break;
                }
            }
            //while(!canComplimenterContinue){ ; } // No op until incrementer has read
            buffer[numberLen-1]='\0'; // Remove \n from input for now

            printf("Complimentor read: %s\t", buffer);
            // Loop through string and perform 'compliment'
            for(i = 0; i < numberLen; i++){
                if(buffer[i] == '1'){
                    buffer[i] = '0';
                }
                else if(buffer[i] == '0'){
                    buffer[i] = '1';
                }
            }
            printf("Writing to Incrementer: %s\n", buffer);
            // Write to pipe
            write(compToIncPipe[WRITE], &buffer, sizeof(buffer));
            incrementerHasRead = false;
            adderHasRead = false;
            
        }
        wait(&incrementerStatus);
        printf("Complimentor done!\n");
    } 
    
    /* Incrementer & Adder Processes */
    else { // Incrementer begins 
        // Close unnecessary file descriptors
        close(compToIncPipe[WRITE]);

        // Create Pipe between Incrimenter and Adder
        int incToAddPipe[2];
        int adderStatus;

        if (pipe (incToAddPipe) < 0) { 
            perror ("Unable to create pipe between Complimentor and Incrementer"); 
            exit(1); 
        }

        if ((adderId = fork()) < 0) { 
            perror ("fork failed"); 
            exit(1); 
        }         

        if(adderId > 0){ // Parent (INCREMENTER)
            // Close unnecessary file descriptors
            close(incToAddPipe[READ]);
            char buffer[numberLen+2];

            // Map Signal Handler for Recieving Signal from Adder
            signal(SIGUSR2, readLineHandler);

            int counter = 1;
            while(counter <= numberOfValues){
                while(!adderHasRead) { ; } // Wait for adder to read
                read(compToIncPipe[READ], &buffer, sizeof(buffer));
                printf("Incrementer read: %s\t", buffer);
                // Perform Incrementation on buffer
                printf("Incrementer writing to Adder: %s\n", buffer);
                write(incToAddPipe[WRITE], &buffer, sizeof(buffer));
                counter++;
                adderHasRead = false;
                kill(getppid(), SIGUSR1);
            }
            wait(&adderStatus);
            printf("Incrementer done!\n");
        
            exit(0);
        }

        /* Adder Process */ 
        else {
            // Close unnecessary file descriptors
            close(compToIncPipe[READ]);
            close(incToAddPipe[WRITE]);

            char buffer[numberLen+2];
            int counter = 1;
            while(counter <= numberOfValues){
                read(incToAddPipe[READ], &buffer, sizeof(buffer));
                printf("Adder read: %s\n", buffer);
                counter++;
                kill(getppid(), SIGUSR2);
            }
            printf("Adder done!\n");
            exit(0);
        }

    }
    
    
    return 0;
}

/****************************************************************
* Unpause processes to begin vector processing
* This handler is triggered by ^C
*****************************************************************/

void unpauseHandler(int signal)
{
	if(signal == SIGINT) {
		isPaused = 1;
	}
    TTK--;
    if(TTK == 0){
        exit(1);
    }
}

/****************************************************************
* Pause processes to give time to evaluate process tree
*****************************************************************/
void pauseProcesses(){
	if(incrementerId > 0){
	    printf("\nAll processes ready to go: Ctrl + C to begin\n");
    }
	fflush(stdout);
    while(1){
		if(isPaused == 1){
            fprintf(stdout, "\n");
			break;
		}
	}
}

/****************************************************************
* Notify parent process that they have read the next line
*****************************************************************/
void readLineHandler(int signal)
{
    // Issue is that the process for Complementer thinks canIncrementer is always false.
    if(signal == SIGUSR1){
        if(incrementerId > 0){ // If Complementer
            incrementerHasRead = true;
        }
    }
    if(signal == SIGUSR2){
        if(adderId > 0){ // If Incrementer
            adderHasRead = true;
            kill(getppid(), SIGUSR2);
        }
    }
}

void readNextLineHandler(int signal){
    if(signal == SIGUSR2){
        if(incrementerId > 0){
            adderHasRead = true;
        }
    }
}
