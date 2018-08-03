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

bool checkOperator(char);
void calculate(int, char, int);
#define MAX_LEN 50

int main(int argc, char* argv[]){
    char equation[MAX_LEN];
    while(fgets(equation, MAX_LEN, stdin) != NULL){
        int num1, num2, result;
        char operator;
        if(sscanf(equation, " %d %c %d ", &num1, &operator, &num2) == 3){
            if(checkOperator(operator)){
                calculate(num1, operator, num2);
            } else {
                printf("operator '%c' not recognized\n", operator);
            }
        }
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
