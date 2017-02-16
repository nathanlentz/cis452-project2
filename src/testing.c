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
    // int i;
    // bool carry;
    // char num1[10];
    // char num2[10];
    // char sum[10];
    FILE* vectorA;
    int numberLen = atoi(argv[1]);
     if((vectorA = fopen(argv[3], "r")) == NULL){
        perror(argv[1]);
        exit(1);
    }
    int valueLen = atoi(argv[2]);
    char vectorAbuffer[numberLen+2];

    int count = 1;
    //while(count <= valueLen){
    while(fgets(vectorAbuffer, sizeof(vectorAbuffer), vectorA) != NULL){
        //fgets(vectorAbuffer, sizeof(vectorAbuffer), vectorA);
        //vectorAbuffer[numberLen]='\0';
        //printf("Size: %i\n", strlen(vectorAbuffer));
        printf("Read: %s", vectorAbuffer);
        //printf("Read in %s", vectorAbuffer);
        count++;
    }

    printf("Finished reading\n");

    fclose(vectorA);

    // printf("Enter binary num 1: ");
    // fgets(num1, 11, stdin);
    // printf("Enter binary num 2: ");
    // fgets(num2, 11, stdin);

    // printf("Adding %s and %s\n", num1, num2);
    // carry = false;
    //     // Loop through string and perform 'compliment'
    // for(i = numberLen-1; i >= 0; i--){
    //     if((num1[i] == '0' && num2[i] == '0') && !carry){
    //         printf("hit 1\n");
    //         sum[i] = '0';
    //         carry = false;
    //     }
    //     else if((num1[i] == '0' && num2[i] == '0') && carry){
    //         sum[i] = '1';
    //         printf("hit 2\n");
    //         carry = false;
    //     }
    //     else if(((num1[i] == '0' && num2[i] == '1') || (num1[i] == '1' && num2[i] == '0')) && !carry){
    //         printf("hit 3\n");
    //         sum[i] = '1';
    //         carry = false;
    //     }
    //     else if(((num1[i] == '0' && num2[i] == '1') || (num1[i] == '1' && num2[i] == '0')) && carry){
    //         printf("hit 4\n");
    //         sum[i] = '0';
    //         carry = true;;
    //     }
    //     else if((num1[i] == '1' && num2[i] == '1') && !carry){
    //         printf("hit 5\n");
    //         sum[i] = '0';
    //         carry = true;
    //     }
    //     else { // Both are 1s and there is a carry
    //         printf("hit 6\n");
    //         sum[i] = '1';
    //         carry = true;
    //     }
    // }
    // printf("Sum is %s", sum);

    return 0;
}