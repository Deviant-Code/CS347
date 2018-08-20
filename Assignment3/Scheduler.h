#ifndef SCHED
#define SCHED

int initThreads();
int initializeVal(int, char**);
double randSleepTime();
int initCond();
void *run_scheduler();
void *run_deviceDriver();
void *run_client();
void addQueue();
void initQueue();
void initDriver();
void printTimes();
void executeFifo();
void executeSSTF();

#endif
