#include <limits.h>

char* commands[] = {"cd", "listf", "exit", "pwd", "calc"}

bool type(char* string){
	int i = 0;
	while(commands[i] != NULL){
		if(strcmp(command[i], string) == 0){
			return true;
		}
	}
	return false;
}