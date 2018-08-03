#ifndef CLI
#define CLI

bool checkCommand(char*);
void changeDirectory(char*, int argCount);
void pwd();
void listf();
char **buildStatement();
void handleStatement();
int argCount();
void initializeDirectories();
bool listfFormat();
char *buildShellCommand();
void calc();
bool redirectOutput();
void restoreStd();
void joinStatement(char**, int);

#endif