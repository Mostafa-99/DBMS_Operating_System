#include "includes.h"
#include "Semaphores.h"

//----------------------------
#define ADDED_TO_WAITING_QUEUE 0
#define GRANTED_ACCESS 1
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
int respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
void respondToRelease(int releasedRecordKey, int CallingProccessPID,struct waitingQueue* waitingQueueOfThePassedKey);
void respondToQuery(int queryType, int searchedSalary, char* searchedName, int CallingProccessPID);
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived,int DBSharedMemoryIdReceived,int loggerMsgQIdReceived,int LoggerIdReceived);
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived,int DBSharedMemoryIdReceived,int loggerMsgQIdReceived,int LoggerIdReceived);

void logDBManagerAdd(int requestedType, char* name, int salary,int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType);
void logDBManagerModify(int requestedType, char* name, int salary,int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType);
void logDBManagerAcquire(int requestedType, char* name, int salary,int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType);
void logDBManagerRelease(int requestedType, char* name, int salary,int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType);
void logDBManagerQuery(int requestedType, char* name, int salary,int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType);

//----------------------------
void handlingSIGUSR1_and_IgnoringSigStop(); //implemented
