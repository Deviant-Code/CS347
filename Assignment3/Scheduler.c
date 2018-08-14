#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int clientProcesses;
int iorequests;

pthread_mutex_t lock;
pthread_t scheduler;
pthread_t deviceDriver;

int initThreads();
int initializeVal(int, char**);
double randSleepTime();
int initCond();
void *run_scheduler();
void *run_deviceDriver();
void *run_client();

// Sleep time range for client threads
int MIN_SLEEP = 5;
int SLEEP_RANGE = 10;
/* ************************************************************/

typedef struct{
	pthread_t			threadID;
	int 				threadNum;
	clock_t				startTime;
	clock_t				endTime;
	double	 			netTime;
} ClientThread;

ClientThread *cThreadArgs;

struct threadCountdown {
	int unfinished;
	pthread_mutex_t lock;
	pthread_cond_t condition;
} countdown;

pthread_barrier_t barrier;


int main(int argc, char *argv[]){
	if(initializeVal(argc, argv)){
		return -1;
	}

	initCond();
	initThreads();

	pthread_barrier_wait(&barrier);
	return 0;

}

int initCond(){

	if(pthread_barrier_init(&barrier, NULL, clientProcesses + 1)){
		printf("Error initializing barrier\n");
	}

	if(pthread_mutex_init(&(countdown.lock), NULL)){
		printf("Error initiating mutex\n");
		exit(1);
	}

	if(pthread_cond_init(&(countdown.condition), NULL)) {
		printf("cannot initialize condition variable\n");
		exit(1);
	}

}

int initializeVal(int argc, char *argv[]){

	if(argc != 3){
		printf("Please run program with 2 arguments.\n");
		printf("Arg1 : # of client processes.\n");
		printf("Arg2 : # of I/O operations to perform. \n");
		return -1;
	}

	clientProcesses = atoi(argv[1]);
	iorequests = atoi(argv[2]);

	if(clientProcesses == 0 || iorequests == 0){
		printf("Requires positive integers for input\n");
		return -1;
	}



	return 0;
}

int initThreads(){

	scheduler = (pthread_t) malloc(sizeof(pthread_t));
	deviceDriver = (pthread_t) malloc(sizeof(pthread_t));
	ClientThread *cThreadArgs = calloc(clientProcesses, sizeof(ClientThread));

	if(pthread_create(&scheduler, NULL, run_scheduler, NULL)){
		printf("Error creating thread: Scheduler \n");
		return -1;
	}

	if(pthread_create(&deviceDriver, NULL, run_deviceDriver, NULL)){
		printf("Error creating thread: DeviceDriver \n");
		return -1;
	}

	for(int i = 0; i < clientProcesses; i++){
		cThreadArgs[i].threadNum = (i+1);
		if(pthread_create(&(cThreadArgs[i].threadID), NULL, run_client, &cThreadArgs[i])){
			printf("Error creating client thread # %d \n", (i+1));
			return -1;
		}
	}

	return 0;

}

void *run_deviceDriver(){

	printf("I am the device Driver, im cul\n");
	return 0;
}


void *run_scheduler(){

	printf("I am the scheduler, hear me roar!\n");
	return 0;


}

void *run_client(void *arg){
	ClientThread* myThread;
	clock_t start_t, end_t;
	double net_t;


	start_t = clock();
	myThread = (ClientThread*) arg;

	printf("I am the client # %d\n", myThread->threadNum); // TEST
	myThread->startTime = start_t; // SET START TIME IN STRUCT

	//Sleep for a random period between 5s and 15s between I/O req
	for(int i = 0; i < iorequests; i++){
		sleep(randSleepTime());
	}

	pthread_barrier_wait(&barrier);
	end_t = clock();
	myThread->endTime = end_t;
	net_t = ((double) (end_t - start_t)) / CLOCKS_PER_SEC;
	printf("We waited %f seconds!!!!\n", net_t);


	return 0;
}



// Returns a random sleep time for client threads in seconds.
// Range and min sleep time declared in constants
double randSleepTime(){
	int randSec = rand() % (SLEEP_RANGE + 1) + MIN_SLEEP;
	double randMs = (double) randSec / 1000;
	return randMs;
}
