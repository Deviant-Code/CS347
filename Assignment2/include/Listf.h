#ifndef LISTF
#define LISTF

void initializeDirectories();
void sortFiles();
int compareSize();
void printFiles();
void printPerm();
void setFlags(char**);
char* getOwner(int);
void largestFile();
void printSize();
void printTime(time_t t);

void printOwnerName();
void printGroupName();
void printSize();
void printLinks();

#endif