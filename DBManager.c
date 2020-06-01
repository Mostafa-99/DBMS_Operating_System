#include "DBManager.h"
//----------------------------------------------------------------
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
int respondToAdd(char* name, int salary, int lastKey)
{
    struct DBrecord newRecord;
    newRecord.name = name;
    newRecord.salary = salary;
    newRecord.key = lastKey+1;
    return lastKey+1;
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
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived){
    messageQueueID=messageQueueIdReceived;
    DBManagerPID = getpid();
    sharedMemoryId = sharedMemoryIdReceived;
}
void mainFunctionDBManager(){
    msgrcv(messageQueueID, &receivedMessage,(sizeof(struct message) - sizeof(receivedMessage.mtype)),DBManagerPID,!IPC_NOWAIT);
    int messageType = receivedMessage.mtype;
    if(messageType == MESSAGE_TYPE_ADD) {
        lastKey = respondToAdd(receivedMessage.name,receivedMessage.salary,lastKey);
        pointersOfWaitingQueuesForRecordKeys[lastKey] = createWaitingQueue();
    }
    else if (messageType == MESSAGE_TYPE_MODIFY) {
        respondToModify(receivedMessage.key, receivedMessage.modification);
    }
    else if (messageType == MESSAGE_TYPE_ACQUIRE) {
        respondToAcquire(receivedMessage.key, receivedMessage.callingProcessID, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
    }
    else if (messageType == MESSAGE_TYPE_RELEASE) {
        respondToRelease(receivedMessage.key, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
    }
    else if (messageType == MESSAGE_TYPE_QUERY) {
        /*will be implemented during the online meeting*/
    }
}