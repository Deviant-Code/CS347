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

struct dirent * ptr;
DIR *d;
struct dirent *dir;
struct stat fileInfo;
char directory[NAME_MAX + 1];
char filePath[NAME_MAX + 1];

struct dirent **storeFiles;

void initializeDirectories();
void sortFiles();
int compareSize();
void printFiles();
void printPerm();

int main(int argc, char* argv[]){

	initializeDirectories();
	sortFiles();
	printFiles();
	closedir(d);
	return 0;
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
	int index = 0;
	while(storeFiles[index] != NULL){
		filePath[0] = '\0';
		strcat(filePath, directory);
		strcat(filePath, "/");
		strcat(filePath, storeFiles[index]->d_name);
		if(!stat(filePath, &fileInfo)){
			if((S_ISREG(fileInfo.st_mode) || S_ISDIR(fileInfo.st_mode)) &&
				storeFiles[index]->d_name[0] != '.'){
				printPerm();
				printf("%s\n",storeFiles[index]->d_name);
			}
		}
		index++;
	}

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
