#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int clientProcesses;
int iooperations;

pthread_mutex_t lock;
pthread_t scheduler;
pthread_t deviceDriver;

pthread_t *clientThread;

int initThreads();
int initializeVal(int, char**);
int randSleepTime();
int initCond();
void *run_scheduler();
void *run_deviceDriver();
void *run_client();



// Sleep time range for client threads
int MIN_SLEEP = 5;
int SLEEP_RANGE = 10;
int MAX_CYLINDERS = 16000;

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
	iooperations = atoi(argv[2]);

	if(clientProcesses == 0 || iooperations == 0){
		printf("Requires positive integers for input\n");
		return -1;
	}

	return 0;
}

int initThreads(){

	scheduler = (pthread_t) malloc(sizeof(pthread_t));
	deviceDriver = (pthread_t) malloc(sizeof(pthread_t));
	clientThread = (pthread_t *) malloc(clientProcesses * sizeof(pthread_t));

	if(pthread_create(&scheduler, NULL, run_scheduler, NULL)){
		printf("Error creating thread: Scheduler \n");
		return -1;
	}

	if(pthread_create(&deviceDriver, NULL, run_deviceDriver, NULL)){
		printf("Error creating thread: DeviceDriver \n");
		return -1;
	}

	for(int i = 0; i < clientProcesses; i++){
		if(pthread_create(&(clientThread[i]), NULL, run_client, NULL)){
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

void *run_client(){

	//Start timer before request and end after for variance
	printf("I am the client \n");

	//for(int i = 0; i < iooperations; i++){

	printf("%d\n",randSleepTime());
	pthread_barrier_wait(&barrier);

	printf("We waited!!!!\n");
	//}

	return 0;
}

//Generates a random cylinder number to simulate I/O request randomization

int randomCylinderRequest(){
	int randNum = rand();
	randNum %= MAX_CYLINDERS;
	return randNum;
}

// Returns a random sleep time for client threads.
// Range and min sleep time declared in constants
int randSleepTime(){
	srand (time(NULL));
	return rand() % (SLEEP_RANGE + 1) + MIN_SLEEP;
}