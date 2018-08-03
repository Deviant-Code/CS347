#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <stdbool.h>
#include "Calc.h"


#define MAX_LEN 50
char equation[MAX_LEN];


int main(int argc, char* argv[]){
	checkInput(argv);
    while(fgets(equation, MAX_LEN, stdin) != NULL){
        parseInput();
    }
}

//Checks input for correct format : int char int
void parseInput(){
	int num1, num2;
        char operator;
        if(sscanf(equation, " %d %c %d ", &num1, &operator, &num2) == 3){
            if(checkOperator(operator)){
                calculate(num1, operator, num2);
            } else {
                printf("operator '%c' not recognized\n", operator);
            }
        }
}
//Allows for user to use calc func by passing in equation from command line
//E.g. 'calc 5 * 5' -- Only supports one command at a time
void checkInput(char *argv[]){
	int index = 1;
	while(argv[index] != NULL && argv[index+1] != NULL && argv[index+2] != NULL){
		char s[MAX_LEN];
		strcat(s, argv[index]);
		strcat(s, " ");
		strcat(s, argv[index + 1]);
		strcat(s, " ");
		strcat(s, argv[index + 2]);
		memcpy(equation,s, MAX_LEN);
		parseInput();
		index += 3;
	}

}


bool checkOperator(char input){
    char supported[] = {'/', '*', '+', '-', '=', '^'};
    int index = 0;
    while(supported[index] != 0){
        if(supported[index] == input){
            return true;
        }
        index++;
    }
    return false;
}

void calculate(int num1, char operator, int num2){
    if(operator == '-'){
        int result = num1 - num2;
        printf("%d %c %d = %d\n", num1, operator, num2, result);
    } else if(operator == '+'){
        int result = num1 + num2;
        printf("%d %c %d = %d\n", num1, operator, num2, result);
    } else if(operator == '*'){
        int result = num1 * num2;
        printf("%d %c %d = %d\n", num1, operator, num2, result);
    } else if(operator == '/'){
        double result = (double) num1 / (double) num2;
        printf("%d %c %d = %lf\n", num1, operator, num2, result);
    } else if(operator == '^'){
        int result = num1^(num2);
        printf("%d %c %d = %d\n", num1, operator, num2, result);
    }
}
