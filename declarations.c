#include "declarations.h"
/*Semaphores*/
struct inQueueProcess* createNewNode(int givenPID) 
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
}
void handlingSIGUSR1_and_IgnoringSigStop()
{
    signal(SIGSTOP, SIG_IGN);
}
//----------------------------------------------------------------
/*DB Manager*/
void respondToAdd(char* name, int salary, int lastKey)
{
    struct DBrecord newRecord;
    newRecord.name = name;
    newRecord.salary = salary;
    newRecord.key = lastKey+1;
}
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue)
{
    int lowerLimit=0;
    int currentIndex=0;
    for(;DBtable[currentIndex].key < keyOfTheRecordToBeModified;currentIndex++);
    if(currentIndex == keyOfTheRecordToBeModified)
    {
        DBtable[currentIndex].salary += modificationValue;
        if(DBtable[currentIndex].salary < lowerLimit) DBtable[currentIndex].salary = lowerLimit;
    }
}
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey)
{
    if(DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED)
    {
        addToWaitingQueue(waitingQueueOfThePassedKey, CallingProccessPID);
    }
    else
    {
        DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED;
        kill(CallingProccessPID,SIGUSR1);
        kill(CallingProccessPID,SIGCONT);
    }
}
void respondToRelease(int releasedRecordKey, struct waitingQueue* waitingQueueOfThePassedKey)
{
    DBsemaphores[releasedRecordKey] == SEMAPHORE_AVAILABLE;
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueOfThePassedKey);
    kill(releasedProcessPID,SIGCONT);
}
//----------------------------------------------------------------