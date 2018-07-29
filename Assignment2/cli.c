/*

Jesse Ericksen
Assignment 2
CSCI 347, Summer 2018

*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHARACTERS 512 // Max Length for commands. Increasing size will increase memory usage but allow for longer input.

int main(int argc, char* argv[]){

    char* command = malloc(MAX_CHARACTERS);
    while(strcmp(command, "exit") != 0){
        printf("$> ");
        fgets(command, MAX_CHARACTERS, stdin);

        printf("%s", command);
        if(strcmp(command, "cd")){
        //    changeDirectory()
        }
    }

    
}
