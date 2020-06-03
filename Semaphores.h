/*
#include "includes.h"
//----------------------------
#define SEMAPHORE_OCCUPIED 0
#define SEMAPHORE_AVAILABLE 1
//----------------------------
struct inQueueProcess 
{ 
    int PID; 
    struct inQueueProcess* nextProcess; 
};
struct waitingQueue 
{ 
    struct inQueueProcess *front, *rear; 
};
struct inQueueProcess* createNewNode(int givenPID); //implemented
struct waitingQueue* createWaitingQueue(); //implemented
void addToWaitingQueue(struct waitingQueue* givenQueue, int PID); //implemented
int removeFromWaitingQueue(struct waitingQueue* givenQueue);  //implemented

*/