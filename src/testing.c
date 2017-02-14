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
// Find sum of binary numbers
// http://www.sanfoundry.com/c-program-sum-binary-numbers/

/* Global Variables */
typedef enum { false, true } bool;

/* Main Entry Point for Program */
int main(int argc, char *argv[]){

    int numberLen = atoi(argv[3]);
    FILE* vectorB;
    if((vectorB = fopen(argv[2], "r")) == NULL){
        perror(argv[2]);
            exit(1);
    }

    char buffer[numberLen+2];
    int sum; 
    int i;
    bool isFirst;
    bool carry;
    
    while (fgets(buffer, sizeof(buffer), vectorB) != NULL)  {
        buffer[numberLen]='\0'; // Remove \n from input for now

        // Report what read was
        printf("Read: %s\n", buffer);
        isFirst = true;
        carry = false;
        // Loop through string and perform 'compliment'
        for(i = numberLen-1; i >=0; i--){
            if(isFirst){
                if(buffer[i] == '0'){
                    buffer[i] = '1';
                    break;
                }
                if(buffer[i] == '1'){
                    buffer[i] = '0';
                    carry = true;
                    continue;
                }
            }
            else {
                if(buffer[i] == '0' && carry){
                    buffer[i] = '1';
                    carry = false;
                }
                else if(buffer[i] == '1' && carry){
                    buffer[i] = '0';
                    carry = true;
                }
                else {
                    break;
                }
            }
        }
        printf("Incremented by 1 is: %s\n", buffer); 
        
    }

    return 0;
}