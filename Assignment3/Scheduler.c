#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Scheduler.h"


//Scheduler and Device Driver Threads:
pthread_t scheduler;
pthread_t deviceDriver;


int clientProcesses;
int iorequests;
int randomCylinderRequest();

// Sleep time range for client threads
int MIN_SLEEP = 5;
int SLEEP_RANGE = 10;
int MAX_CYLINDERS = 16000;
int MAX_QUEUE = 2048;

/* ************************************************************/

typedef struct{
	pthread_t			threadID;
	int 				threadNum;
	clock_t				startTime;
	clock_t				endTime;
	double	 			netTime;
	pthread_cond_t cond;
	pthread_mutex_t lock;
} ClientThread;

ClientThread *cThreadArgs;

struct threadCountdown {
	pthread_mutex_t lock;
	pthread_cond_t condition;
	pthread_cond_t condition2;
	int count;
} countdown;

typedef struct node {
	struct node *next;
	int cylinder;
	pthread_mutex_t lock;
	pthread_cond_t cond;
} Node;

typedef struct Queue {
	Node *front;
	Node *rear;
	int length;
	pthread_mutex_t lock;
} Queue;

Queue *schedulingQueue;

typedef struct deviceDriver {
	int cylinderRequest;
	pthread_mutex_t lock;
	pthread_cond_t cond;
} DeviceDriver;

DeviceDriver *dd;
DeviceDriver *sched;

pthread_cond_t ddCond;
pthread_mutex_t ddMut;

Node *createNode();


int main(int argc, char *argv[]){
	int clientProcesses = atoi(argv[1]);
	if(initializeVal(argc, argv)){
		return -1;
	}
	initQueue();
	initDriver();
	initCond();
	initThreads();
	pthread_cond_wait(&countdown.condition, &countdown.lock);
	pthread_cond_wait(&countdown.condition2, &countdown.lock);
	printTimes(clientProcesses);
	return 0;
}

void initQueue(){
	schedulingQueue = malloc(MAX_QUEUE);
	schedulingQueue->front = NULL;
	schedulingQueue->rear = NULL;
	schedulingQueue->length == 0;
}

void printTimes(int tot){
	double meanTime = 0;
	for(int i = 0; i < tot; i++){
		meanTime += cThreadArgs[i].netTime;
	}
	printf("Average time amongst all threads: %lf\n", meanTime/(double)tot);

}

int initCond(){

	if(pthread_mutex_init(&(countdown.lock), NULL)){
		printf("Error initiating mutex\n");
		exit(1);
	}

	if(pthread_cond_init(&(countdown.condition), NULL)) {
		printf("cannot initialize condition variable\n");
		exit(1);
	}

	if(pthread_cond_init(&(countdown.condition2), NULL)) {
		printf("cannot initialize condition variable\n");
		exit(1);
	}
}

void initDriver(){
	dd = malloc(sizeof(DeviceDriver));
	sched = malloc(sizeof(DeviceDriver));
	dd->cylinderRequest = 0;
	if(pthread_cond_init(&(dd->cond), NULL)) {
		printf("cannot initialize condition variable\n");
		exit(1);
	}
	if(pthread_mutex_init(&(dd->lock), NULL)){
		printf("Error initiating mutex\n");
		exit(1);
	}

	if(pthread_cond_init(&(sched->cond), NULL)) {
		printf("cannot initialize condition variable\n");
		exit(1);
	}
	if(pthread_mutex_init(&(sched->lock), NULL)){
		printf("Error initiating mutex\n");
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

	countdown.count = clientProcesses * iorequests;

	return 0;
}

int initThreads(){

	scheduler = (pthread_t) malloc(sizeof(pthread_t));
	deviceDriver = (pthread_t) malloc(sizeof(pthread_t));
	cThreadArgs = calloc(clientProcesses, sizeof(ClientThread));

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
	int currCyl = 0;

	while(1){
		pthread_cond_wait(&dd->cond,&dd->lock);
		int distance = abs(dd->cylinderRequest - currCyl);
		currCyl == dd->cylinderRequest;
		double reqTime = .01 + (.0001*distance);
		sleep(reqTime);
		pthread_cond_signal(&sched->cond);
	}

	return 0;
}



void *run_scheduler(){

	//WAIT ON IO REQ FROM CLIENT AND PASS TO DEVICE DRIVER ! ! ! !
	//executeFifo();
	executeSSTF();
	pthread_cond_signal(&countdown.condition);
	return 0;
}

void executeFifo(){
	//FIFO:
	while(countdown.count != 0){
		if(schedulingQueue->length != 0){
			pthread_mutex_lock(&schedulingQueue->lock);
			Node *n = schedulingQueue->front;
			if(schedulingQueue->length == 1){
				schedulingQueue->front = NULL;
				schedulingQueue->rear = NULL;

			} else {
				schedulingQueue->front = schedulingQueue->front->next;
			}

			schedulingQueue->length--;
			pthread_mutex_unlock(&schedulingQueue->lock);

			pthread_mutex_lock(&dd->lock);
			dd->cylinderRequest = n->cylinder;
			pthread_mutex_unlock(&dd->lock);
			pthread_cond_signal(&dd->cond);
			pthread_cond_wait(&sched->cond, &sched->lock);
			countdown.count--;
			pthread_cond_signal(&n->cond);
		} else {
			sleep(.01);
		}
	}
}

//SSTF Algorithm Handler
void executeSSTF(){
	int cylinder, currCyl, currDiff, nodeNum;
	while(countdown.count != 0){
		if(schedulingQueue->length != 0){
			pthread_mutex_lock(&dd->lock);
			cylinder = dd->cylinderRequest;
			pthread_mutex_unlock(&dd->lock);

			Node *front = schedulingQueue->front;
			Node *curr = front;
			nodeNum = 0;
			currDiff = abs(front->cylinder - cylinder);
			int i = 0;
			while(curr->next != front){
				i++;
				curr = curr->next;
				if(abs(curr->cylinder - cylinder) < currDiff){
					nodeNum = i;
				}
			}

			//Now, loop through queue to closest nodeNum
			Node *n = schedulingQueue->front;
			for(int i = 0; i < nodeNum; i++){
				n = n->next;
			}

			Node *shortestRoute = n->next;
			n->next = n->next->next;

			schedulingQueue->length--;
			pthread_mutex_unlock(&schedulingQueue->lock);

			pthread_mutex_lock(&dd->lock);
			dd->cylinderRequest = shortestRoute->cylinder;
			pthread_mutex_unlock(&dd->lock);
			pthread_cond_signal(&dd->cond);
			pthread_cond_wait(&sched->cond, &sched->lock);
			countdown.count--;
			pthread_cond_signal(&n->cond);

			pthread_mutex_lock(&schedulingQueue->lock);


		} else {
			sleep(.01);
		}



	}


}

void *run_client(ClientThread *myThreadArg){
	clock_t start_t, end_t;
	double net_t;
	int ret;

	myThreadArg->startTime = start_t; // SET START TIME IN STRUCT

	//Sleep for a random period between 5s and 15s between I/O req
	for(int i = 0; i < iorequests; i++){
		sleep(randSleepTime());
		start_t = clock();
		Node *n = createNode(randomCylinderRequest());
		addQueue(n);
		pthread_cond_wait(&n->cond, &n->lock);
		end_t = clock();
		pthread_mutex_lock(&myThreadArg->lock);
		myThreadArg->endTime = end_t;
		myThreadArg->netTime = ((double) (end_t - start_t)) / (double) CLOCKS_PER_SEC;
		printf("ThreadNum %d waited %lf seconds to complete I/O request\n", myThreadArg->threadNum, myThreadArg->netTime);
		pthread_mutex_unlock(&myThreadArg->lock);
	}

	pthread_cond_signal(&countdown.condition2);


	return 0;
}

void addQueue(Node *n){
	pthread_mutex_lock(&schedulingQueue->lock);
	if(schedulingQueue->length == 0){
		//Queue is empty:
		schedulingQueue->front = n;
		schedulingQueue->rear = n;
	}

	schedulingQueue->rear->next = n;
	schedulingQueue->rear = n;
	schedulingQueue->length++;
	pthread_mutex_unlock(&schedulingQueue->lock);
}

Node *createNode(int cylinder){
	Node *n = malloc(sizeof(Node));
	n->cylinder = cylinder;
	n->next = NULL;
	return n;
}


//Generates a random cylinder number to simulate I/O request randomization
//Constant declared for number of cylinders in disk drive.
int randomCylinderRequest(){
	int randNum = rand();
	randNum %= MAX_CYLINDERS;
	return randNum;
}



// Returns a random sleep time for client threads in seconds.
// Range and min sleep time declared in constants
double randSleepTime(){
	int randSec = rand() % (SLEEP_RANGE + 1) + MIN_SLEEP;
	double randMs = (double) randSec / 1000;
	return randMs;
}
