#include "includes.h"
//----------------------------
//----------------------------
#define SEMAPHORE_OCCUPIED 0
#define SEMAPHORE_AVAILABLE 1
//----------------------------
long int DBManagerPID;
struct message receivedMessage;
int sharedMemoryId;
int lastKey=-1;
struct waitingQueue *pointersOfWaitingQueuesForRecordKeys[MAX_NUMBER_OF_RECORDS];

struct DBrecord *DBtable;
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
//----------------------------
//----------------------------
/*DB Manager functions*/
void respondToRequests();
int respondToAdd(char* name, int salary, int lastKey); //implemented
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue); //implemented
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
void respondToRelease(int releasedRecordKey, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
void respondToQuery(int queryType, int searchedSalary, char* searchedName);
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived,int DBSharedMemoryIdReceived);
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived,int DBSharedMemoryIdReceived);
//----------------------------