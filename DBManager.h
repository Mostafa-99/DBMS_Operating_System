#include "includes.h"
//----------------------------
#define MAX_NUMBER_OF_RECORDS 1000
//----------------------------
#define SEMAPHORE_OCCUPIED 0
#define SEMAPHORE_AVAILABLE 1
//----------------------------
long int DBManagerPID;
struct message receivedMessage;
int sharedMemoryId;
int lastKey=-1;
struct waitingQueue *pointersOfWaitingQueuesForRecordKeys[MAX_NUMBER_OF_RECORDS];
struct DBrecord
{
    int key;
    char* name;
    int salary;
};
struct DBrecord DBtable[MAX_NUMBER_OF_RECORDS];
//----------------------------
/*Semaphores*/
int DBsemaphores[MAX_NUMBER_OF_RECORDS];
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
void handlingSIGUSR1_and_IgnoringSigStop(); //implemented
//----------------------------
//----------------------------
/*DB Manager functions*/
void respondToRequests();
void respondToAdd(char* name, int salary, int lastKey); //implemented
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue); //implemented
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
void respondToRelease(int releasedRecordKey, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
void respondToQuery(); /*will be implemented during the online meeting*/
void mainFunctionDBManager();
//----------------------------