#include "Semaphores.h"
/*Semaphores*/
/*struct inQueueProcess* createNewNode(int givenPID) 
{ 
    struct inQueueProcess* newProcess = (struct inQueueProcess*)malloc(sizeof(struct inQueueProcess)); 
    newProcess->PID = givenPID; 
    newProcess->nextProcess = NULL; 
    return newProcess; 
}
struct waitingQueue* createWaitingQueue() 
{ 
    struct waitingQueue* newQueue = (struct waitingQueue*)malloc(sizeof(struct waitingQueue)); 
    newQueue->front = newQueue->rear = NULL; 
    return newQueue; 
}
void addToWaitingQueue(struct waitingQueue* givenQueue, int PID)
{
    struct inQueueProcess* newProcess = createNewNode(PID); 
    if (givenQueue->rear == NULL) 
    { 
        givenQueue->front = givenQueue->rear = newProcess; 
        return; 
    } 
    givenQueue->rear->nextProcess = newProcess; 
    givenQueue->rear = newProcess; 
}
int removeFromWaitingQueue(struct waitingQueue* givenQueue) 
{ 
    if (givenQueue->front == NULL) return; 
   
    struct inQueueProcess* removedProcess = givenQueue->front; 
  
    givenQueue->front = givenQueue->front->nextProcess; 
  
    if (givenQueue->front == NULL) givenQueue->rear = NULL; 
    int removedProcessPID = removedProcess->PID;
    free(removedProcess); 
    return removedProcessPID;
}*/