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

DIR * dir;
struct dirent * ptr;


int main(int argc, char* argv[]){

	DIR *d;
	struct dirent *dir;
	
	char directory[NAME_MAX + 1];
    getcwd(directory, NAME_MAX);
    printf("%s\n", directory);
	
	d = opendir(directory);
	
	if(d){
		while((dir = readdir(d)) != NULL) {
			printf("%s\n", dir->d_name);
		}
		closedir(d);
	}

	return 0;
}