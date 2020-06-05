#include "includes.h"
#include "Semaphores.h"

//----------------------------

long int DBManagerPID;
struct message receivedMessage;
struct message sentMessageToClient;

int sharedMemoryId;
int lastKey=-1;
struct waitingQueue *pointersOfWaitingQueuesForRecordKeys[MAX_NUMBER_OF_RECORDS];

struct DBrecord *DBtable;
int LoggerId;
int loggerMsgQIdDBManager;

struct Log *DBManagerLogger;

struct loggerMsgQ messageLoggerDBManager;
//----------------------------
/*Semaphores*/
int DBsemaphores[MAX_NUMBER_OF_RECORDS];

//----------------------------
//----------------------------
/*DB Manager functions*/
void respondToRequests();
int respondToAdd(char* name, int salary, int lastKey); //implemented
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue); //implemented
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
void respondToRelease(int releasedRecordKey, int CallingProccessPID,struct waitingQueue* waitingQueueOfThePassedKey);
void respondToQuery(int queryType, int searchedSalary, char* searchedName, int CallingProccessPID);
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived,int DBSharedMemoryIdReceived,int loggerMsgQIdReceived,int LoggerIdReceived);
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived,int DBSharedMemoryIdReceived,int loggerMsgQIdReceived,int LoggerIdReceived);

void logDBManagerAdd(char *name, int salary);
//----------------------------
void handlingSIGUSR1_and_IgnoringSigStop(); //implemented
