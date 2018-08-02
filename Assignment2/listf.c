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

struct dirent * ptr;
DIR *d;
struct dirent *dir;
struct stat fileInfo;
char directory[NAME_MAX + 1];
char filePath[NAME_MAX + 1];
struct dirent **storeFiles;

//Argument Flags
bool lflag, mflag, aflag, cflag = false;

#define MAX_ARGS 4

void initializeDirectories();
void sortFiles();
int compareSize();
void printFiles();
void printPerm();
void setFlags(char**);
void printOwner(int);
int largestFile();
void printSize();

int main(int argc, char* argv[]){
	setFlags(argv);
	initializeDirectories();
	sortFiles();
	printFiles();
	closedir(d);
	return 0;
}

//checks arguments passed from listf and sets the according flags
void setFlags(char *argv[]){
	int index = 1; //first index is listf
	while(argv[index] != 0){
		int index2 = 1; //first index is always '-' checked before exec
		while(argv[index][index2] != 0){
			if(argv[index][index2] == 'l'){
				lflag = true;
			} else if(argv[index][index2] == 'm'){
				mflag = true;
			} else if(argv[index][index2] == 'a'){
				aflag = true;
			} else if(argv[index][index2] == 'c'){
				cflag =true;
			}
			index2++;
		}
		index++;
	}
}

void initializeDirectories(){
	getcwd(directory, NAME_MAX);
	d = opendir(directory);
	storeFiles = malloc(1024);
}

void sortFiles(){
	if(d){
		int index = 0;
		while((dir = readdir(d)) != NULL) {
			storeFiles[index] = dir;
			index++;
		}
		qsort(storeFiles, index, sizeof(*storeFiles),compareSize);
	}
}

int compareSize(const void *const A, const void *const B) {
    return strcmp((*(struct dirent **) A)->d_name, (*(struct dirent **) B)->d_name);
}

void printFiles(){

	int maxDigits = largestFile();
	int index = 0;
	while(storeFiles[index] != NULL){
		filePath[0] = '\0';
		strcat(filePath, directory);
		strcat(filePath, "/");
		strcat(filePath, storeFiles[index]->d_name);
		if(!stat(filePath, &fileInfo)){
			if((S_ISREG(fileInfo.st_mode) || S_ISDIR(fileInfo.st_mode)) &&
				storeFiles[index]->d_name[0] != '.'){
				if(lflag){
					printPerm();
					printf("%d ",(int)fileInfo.st_nlink); // HardLinks
					printOwner(fileInfo.st_uid);
					printOwner(fileInfo.st_gid);
					printSize(maxDigits);
				}
				
				printf("%s\n",storeFiles[index]->d_name);
			}
		}
		index++;
	}

}

void printSize(int largestFile){
	printf("%*ld ", largestFile, fileInfo.st_size);
}

//Used to find largest file for formatting byte sizes in printFiles()
//Calculates the largest byte size then totals the digits required for formatting
int largestFile(){
	long largestFile = 0;
	int index = 0;
	while(storeFiles[index] != NULL){
		filePath[0] = '\0';
		strcat(filePath, directory);
		strcat(filePath, "/");
		strcat(filePath, storeFiles[index]->d_name);
		if(!stat(filePath, &fileInfo)){
			if((S_ISREG(fileInfo.st_mode) || S_ISDIR(fileInfo.st_mode)) &&
				storeFiles[index]->d_name[0] != '.'){
				if((long)fileInfo.st_size > largestFile){
					largestFile = fileInfo.st_size;
				}
		 	}
		}
		index++;
	}

	int digitCount = 0;
	while(largestFile > 0){
		largestFile /= 10;
		digitCount++;
	}

	return digitCount;


}

//Takes in a user / group id and prints the name of user / group
void printOwner(int id){
	struct passwd *pw = getpwuid(fileInfo.st_uid);
	printf("%s ", pw->pw_name);

}

void printPerm(){
	printf((S_ISDIR(fileInfo.st_mode)) ? "d" : "-");
    printf((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileInfo.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileInfo.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileInfo.st_mode & S_IROTH) ? "r" : "-");
    printf((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileInfo.st_mode & S_IXOTH) ? "x" : "-");
	printf(" ");
}
