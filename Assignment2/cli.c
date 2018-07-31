/*

Jesse Ericksen
Assignment 2
CSCI 347, Summer 2018

This program provides a command line interface with support for multiple commands listed below:
    - cd : cd <path> // Changes directory to the path inputted
    -


*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAX_CHARACTERS 512 // Max Length for commands. Increasing size will increase memory usage but allow for longer input.
#define MAX_COMMAND 128 // Max length for command
#define MAX_FILE_SIZE 128 // Max file length for allocating space

void changeDirectory(char*);
void pwd();
void listf(char*);

int main(int argc, char* argv[]){

    char* command = malloc(MAX_COMMAND); // Holds just the first argument (command) of an entry
    char* statement = malloc(MAX_CHARACTERS); // Holds items after the command!!

    while(strcmp(command, "exit") != 0){
        printf("$> ");
        scanf("%s", command);

        fgets(statement, MAX_CHARACTERS, stdin); // retrieve statement after command
        strtok(statement, "\n"); //remove new line break at end of input
        statement++; //remove leading whitespace

        if(strcmp(command, "cd") == 0){
            changeDirectory(statement);
        } else if (strcmp(command, "pwd") == 0){
            pwd();
        }
    }
}

//Changes directory and outputs the resulting change to stdout
void changeDirectory(char* statement){
    //add option for ~ input and for cleaning up input
    char* cwd = malloc(MAX_FILE_SIZE);
    if(chdir(statement) == 0){
        cwd = getcwd(cwd, MAX_FILE_SIZE);
        printf("cwd changed to %s\n", cwd);
    } else {
        printf("Error in changeDirectory: %s: \"%s\"\n", strerror(errno), statement);
    }
}

//Displays the absolute path of the current working directory
void pwd(){
    char* cwd = malloc(MAX_FILE_SIZE);
    cwd = getcwd(cwd, MAX_FILE_SIZE);
    printf("%s\n", cwd);
}

void listf(char* statement){
    pid_t child_pid, wpid;
    int child_status;

    child_pid = fork();
    if(child_pid == 0){
        //CHILD PROCESS
    } else {
        wait()
    }
}
