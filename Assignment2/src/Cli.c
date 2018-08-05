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
#include <fcntl.h>
#include <sys/stat.h>
#include "Cli.h"

#define MAX_CHARACTERS 512 // Max Length for commands. Increasing size will increase memory usage but allow for longer input.
#define MAX_COMMAND 128 // Max length for command
#define MAX_FILE_SIZE 128 // Max file length for allocating space
#define TOT_COMMANDS 5 // total commands for interpreter

char* commands[] = {"cd", "listf", "exit", "pwd", "calc"};

char calcDirectory[NAME_MAX +1];
char listfDirectory[NAME_MAX + 1];
char currDir[NAME_MAX + 1];

int saved_stdout;
int saved_stdin;

char **statement;

int main(int argc, char* argv[]){
    initializeDirectories();
	char* command = "";
    printf("$> ");
    while(strcmp(command, "exit") != 0){
        statement = buildStatement();

        if(statement[0] != NULL){
            command = statement[0];
            if(redirectOutput()){
            	handleStatement();
            	restoreStd();
            } else {
            	handleStatement();
            }
            printf("$> ");

        }
    }
    return 0;
}

//Initializes saved directories of executables and saves stdin and stdout for redirection
void initializeDirectories(){
	statement = malloc(MAX_COMMAND);
    getcwd(listfDirectory,NAME_MAX); // initialize starting directory.
    strcat(listfDirectory, "/src/./listf");
    getcwd(calcDirectory,NAME_MAX); // initialize starting directory.
    strcat(calcDirectory, "/src/./calc");

    saved_stdout = dup(1);
    saved_stdin = dup(0);
}

void handleStatement(){
	if(statement[0] != NULL){
		char *command = statement[0];
		int args = argCount(statement);

		if(strcmp(command, "cd") == 0){
		    changeDirectory(statement[1], args);
		} else if (strcmp(command, "pwd") == 0){
		    pwd();
		} else if(strcmp(command, "listf") == 0){
			listf(statement);
		} else if(strcmp(command, "calc") == 0){
		    calc();
		} else {
		    //else try as shell command
		    char* shellCommand = buildShellCommand();
		    system(shellCommand);
		}
	}

}

//Used to restore stdin and stdout to default
void restoreStd(){
	close(1);
	close(0);
	dup2(saved_stdout, 1);
	dup2(saved_stdin, 0);
}

//Returns true if output or input has been redirected
bool redirectOutput(){
	bool flag = false;
	int index = 0;
	while(statement[index] != NULL){
		if(strcmp(statement[index], ">") == 0){
			if(statement[index + 1] != NULL){
				int fd = open(statement[index + 1], O_WRONLY);
				dup2(fd, 1);
				statement[index] = NULL;
				flag = true;
			} else {
				printf("No file specified for redirect\n");
				return false;
			}

		} else if(strcmp(statement[index], "<") == 0){
			if(statement[index + 1] != NULL){
				if(fopen(statement[index + 1], "r")){
					int fd = open(statement[index + 1], O_RDONLY);
					dup2(fd, 0);
					char **appendSt = buildStatement();
					joinStatement(appendSt, index);
				
					flag = true;
				}
			} else {
				printf("No file specified for redirect\n");
				return false;
			}
		}
		index++;
	}
	return flag;
}

//Appends char** type to char**
//Used for appending input redirection onto existing input
void joinStatement(char **append, int index){

	char **newStatement = malloc(MAX_COMMAND);
	
	int statIndex = 0;
	while(statIndex < index){
		newStatement[statIndex] = statement[statIndex];
		statIndex++;
	}


	int appendIndex = 0;
	int newIndex = statIndex;
	statIndex = statIndex +2;
	while(append[appendIndex] != NULL){
		newStatement[newIndex] = append[appendIndex];
		appendIndex++;
		newIndex++;
	}

	while(statement[statIndex] != NULL){
		newStatement[newIndex] = statement[statIndex];
		newIndex++;
		statIndex++;
	}
	statement = newStatement;

}

//Creates child process to run ./Calc for calculator functionality
void calc(){
    pid_t child_pid;
    child_pid = fork();

    if(child_pid<0){
    	perror("Fork failed in calc()\n");
    	exit(-1);
    } else if(child_pid == 0){ // Child process
        execv(calcDirectory,statement);
        _exit(1);
    } else {
        wait(&child_pid);
        if(!WIFEXITED(child_pid)){ // Check if child exited normally
        	perror("Child exited abnormally in calc\n");
        }
    }
}

//Takes in an array of arguments and builds a shell command
char *buildShellCommand(){
    char* shellCommand = malloc(NAME_MAX + 1);
    int index = 0;
    while(statement[index] != NULL){
        strcat(shellCommand, statement[index]);
        strcat(shellCommand, " ");
        index++;
    }
    return shellCommand;
}

//Returns number of tokens in a statement
int argCount(){
	int index = 0;
	while(statement[index] != NULL){
		index++;
	}
	return index;
}

//Retrieves a line (command) of user input and
//Separates it into tokens, returning array of tokens
char **buildStatement(){
	char *whiteSpace = " \t\n\f\r\v";
	char *token;
	char *input = malloc(MAX_COMMAND);
	char **buildStatement = malloc(MAX_COMMAND);
	fgets(input, MAX_COMMAND, stdin);
	int index = 0;
	token = strtok(input, whiteSpace);
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

void listf(){
    if(listfFormat()){
	    pid_t child_pid;
	    child_pid = fork();
	    if(child_pid < 0){
	    	perror("Fork failed in listf\n");
	    	exit(-1);
	    } else if(child_pid == 0){
	        //CHILD PROCESS
	        execv(listfDirectory,statement);
	        _exit(1);
	    } else {
	        wait(&child_pid); // wait on child to finish listf func
	        if(!WIFEXITED(child_pid)){ // Check if child exited normally
				perror("Child exited abnormally in listf\n");	
	        }
	    }
	}
}

//checks format and specifiers before listf command executes
bool listfFormat(){
	int index = 1;
	while(statement[index] != NULL){
		if(statement[index][0] != '-'){
			printf("incorrect format for 'listf'\n");
			return false;
		}

		int index2 = 1;
		while(statement[index][index2] != 0){
			if(!(statement[index][index2] == 'l' || statement[index][index2] == 'm' ||
				statement[index][index2] == 'a' || statement[index][index2] == 'c' )){
				printf("specifier: %c not recognized for 'listf'\n", statement[index][index2]);
				return false;
			}
			index2++;
		}
		index++;
	}
	return true;
}

//Returns true if command parsed is a valid command
bool checkCommand(char* string){
	for(int i = 0; i < TOT_COMMANDS; i++){
		if(strcmp(string, commands[i]) == 0){
			return true;
		}
	}
	return false;
}
