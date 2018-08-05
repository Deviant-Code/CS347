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
#include "Listf.h"

struct dirent * ptr;
DIR *d;
struct dirent *dir;
struct stat fileInfo;
char directory[NAME_MAX + 1];
char filePath[NAME_MAX + 1];
struct dirent **storeFiles;

//Holds the longest metadata for files in a directory
//Used to format strings to max length of largest file   
struct maxDigits {
	int bytes;
	int links;
	int user;
	int group;
} maxDig;


//Argument Flags
bool lflag, mflag, aflag, cflag = false;

//For adding supported arguments down the road.
#define MAX_ARGS 4


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

//Initialize current working directory dirent for storing files
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

	largestFile();
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
					printLinks();
					printOwnerName();
					printGroupName();
					printSize();
					if(mflag || !(aflag || cflag)){
						printf("M");
						printTime(fileInfo.st_mtime);
					}

					if(aflag){
						printf("A");
						printTime(fileInfo.st_atime);
					}

					if(cflag){
						printf("C");
						printTime(fileInfo.st_ctime);
					}
					printf(" "); // adds buffer before filename
				}
				printf("%s\n",storeFiles[index]->d_name);
			}
		}
		index++;
	}

}

void printSize(){
	printf("%*ld ", maxDig.bytes, fileInfo.st_size); //bytes -- right justify
}


void printOwnerName(){
	struct passwd *pwd = getpwuid(fileInfo.st_uid);
	printf("%-*s ", maxDig.user, pwd->pw_name); // groupName -- left justify
}

void printGroupName(){
	struct group *grp = getgrgid(fileInfo.st_gid);
	printf("%-*s ", maxDig.group, grp->gr_name); // groupName -- left justify
}

void printLinks(){
	printf("%*d ", maxDig.links, (int)fileInfo.st_nlink); // HardLinks -- right justify
}

void printTime(time_t t){
	struct tm lt;
	localtime_r(&t, &lt);
	char timbuf[80];
	strftime(timbuf, sizeof(timbuf), "%D-%R", &lt);
	printf("%s ", timbuf);
}

//Checks for the largest file metadata and stores the max digits required
//in struct maxDig to be used for formatting the listf func
void largestFile(){

	int index = 0;
	while(storeFiles[index] != NULL){
		filePath[0] = '\0';
		strcat(filePath, directory);
		strcat(filePath, "/");
		strcat(filePath, storeFiles[index]->d_name);
		if(!stat(filePath, &fileInfo)){
			if((S_ISREG(fileInfo.st_mode) || S_ISDIR(fileInfo.st_mode)) &&
				storeFiles[index]->d_name[0] != '.'){

				if((long)fileInfo.st_size > maxDig.bytes){
					maxDig.bytes = fileInfo.st_size;
				}

				if((long)fileInfo.st_nlink > maxDig.links){
					maxDig.links = fileInfo.st_nlink;
				}

				int ownerNameL = strlen(getpwuid(fileInfo.st_uid)->pw_name);
				int groupNameL = strlen(getgrgid(fileInfo.st_gid)->gr_name);

				if(ownerNameL > maxDig.user){
					maxDig.user = ownerNameL;
				}

				if(groupNameL > maxDig.group){
					maxDig.group = groupNameL;
				}
			}
		}
		index++;
	}

	//Calculates the number of digits required to represent largest byte count
	int digitCount = 0;
	while(maxDig.bytes > 0){
		maxDig.bytes /= 10;
		digitCount++;
	}
	maxDig.bytes = digitCount;

	//Calculates the number of digits required to represent largest link count
	digitCount = 0;
	while(maxDig.links > 0){
		maxDig.links /= 10;
		digitCount++;
	}
	maxDig.links = digitCount;

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
