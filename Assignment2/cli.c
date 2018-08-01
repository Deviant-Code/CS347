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
#include <limits.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_CHARACTERS 512 // Max Length for commands. Increasing size will increase memory usage but allow for longer input.
#define MAX_COMMAND 128 // Max length for command
#define MAX_FILE_SIZE 128 // Max file length for allocating space
#define TOT_COMMANDS 5 // total commands for interpreter

bool checkCommand(char*);
void changeDirectory(char*, int argCount);
void pwd();
void listf(char**);
char** buildStatement();
char* commands[] = {"cd", "listf", "exit", "pwd", "calc"};
void handleStatement();
int argCount(char**);

char homeDirectory[NAME_MAX + 1];

int main(int argc, char* argv[]){
	getcwd(homeDirectory,NAME_MAX); // initialize starting directory.
	char* command = "";
    while(strcmp(command, "exit") != 0){
        
        printf("$> ");
        char **statement = buildStatement();
        command = statement[0];

        if(!checkCommand(statement[0])){
        	printf("Command not recognized\n");
        } else {
        	handleStatement(statement);
        }

    }
}

void handleStatement(char** statement){
	char *command = statement[0];
	int args = argCount(statement);

	if(strcmp(command, "cd") == 0){
        changeDirectory(statement[1], args);
    } else if (strcmp(command, "pwd") == 0){
        pwd();
    } else if(strcmp(command, "listf") == 0){
    	listf(statement);
    }
}

//Returns number of tokens in a statement
int argCount(char** statement){
	int index = 0;
	while(statement[index] != NULL){
		index++;
	}
	return index;
}

//Retrieves a line (command) of user input and
//Separates it into tokens, returning array of tokens
char** buildStatement(){
	char *whiteSpace = " \t\n\f\r\v";
	char** buildStatement = malloc(MAX_COMMAND);
	char *token;

	char *statement = malloc(MAX_COMMAND);
	fgets(statement, MAX_COMMAND, stdin);
	int index = 0;
	token = strtok(statement, whiteSpace);
	while(token != NULL){
		buildStatement[index] = token;
		token = strtok(NULL, whiteSpace);
		index++;
	}
	return buildStatement;

}

//Changes directory and outputs the resulting change to stdout
void changeDirectory(char* newDir, int argCount){

	if(argCount > 2){
		printf("too many arguments\n");
		return;
	} else if(argCount == 1 || strcmp(newDir, "~") == 0){
		newDir = getenv("HOME");
	}

	char directory[NAME_MAX + 1];
	getcwd(directory, NAME_MAX);

	//If you are already in the requested directory, no output
	//Else attempt to change and output result
	if(!strcmp(directory, newDir) == 0){
		if(chdir(newDir) == 0){
			getcwd(directory, NAME_MAX);
		    printf("cwd changed to %s\n", directory);
		} else {
		    printf("%s: \"%s\"\n", strerror(errno), newDir);
		}
	}
}


//Displays the absolute path of the current working directory
void pwd(){
    char directory[NAME_MAX + 1];
    getcwd(directory, NAME_MAX);
    printf("%s\n", directory);
}

void listf(char** options){
    pid_t child_pid;
  //  int child_status;

    child_pid = fork();
    if(child_pid == 0){
        //CHILD PROCESS
    	char listfDir[NAME_MAX];
  	 	strcat(listfDir, homeDirectory);
		strcat(listfDir, "./listf"); 
		printf("%s\n", listfDir);
        execv(listfDir,options);
                printf("CHILD PROCESS HERE\n");

        _exit(0);
    } else {
        wait(&child_pid);

        if(WIFEXITED(child_pid)){ // Check if child exited normally

        }
    }
}

bool checkCommand(char* string){
	for(int i = 0; i < TOT_COMMANDS; i++){
		if(strcmp(string, commands[i]) == 0){
			return true;
		}
	}
	return false;
}
