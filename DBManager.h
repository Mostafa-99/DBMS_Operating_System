#include "includes.h"

//----------------------------
#define ADDED_TO_WAITING_QUEUE 0
#define GRANTED_ACCESS 1
#define NO_ELEMENTS 0
#define KEY_START 0
#define NOT_RETURNED -1
#define NOT_FOUND 0
#define IDENTICAL_NAMES 0
#define FIRST_CHARACTER 0
#define SALARY_LOWER_LIMIT 0
long int DBManagerPID;
struct message receivedMessage;
struct message sentMessageToClient;

int sharedMemoryId;
int lastKey= KEY_START -1;
struct waitingQueue *pointersOfWaitingQueuesForRecordKeys[MAX_NUMBER_OF_RECORDS];

struct DBrecord *DBtable;
int LoggerId;
int loggerMsgQIdDBManager;

struct Log *DBManagerLogger;

struct loggerMsgQ messageLoggerDBManager;
int DBsemaphores [MAX_NUMBER_OF_RECORDS];


void respondToRequests();
int respondToAdd(char* name, int salary, int lastKey); //implemented
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue); //implemented
int respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
int respondToRelease(int releasedRecordKey, int CallingProccessPID,struct waitingQueue* waitingQueueOfThePassedKey);
void respondToQuery(int queryType, int searchedSalary, char* searchedName, int CallingProccessPID);
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived,int DBSharedMemoryIdReceived,int loggerMsgQIdReceived,int LoggerIdReceived);
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived,int DBSharedMemoryIdReceived,int loggerMsgQIdReceived,int LoggerIdReceived);

void logDBManagerAdd(char *name, int salary);
void logDBManagerModify(int callingProcessPID, int recordKey, int modificationValue);
void logDBManagerAcquire(int callingProcessPID, int callingProcessWaitingBoolean, int recordKey);
void logDBManagerRelease(int recordKey, int releasedProcess);
void logDBManagerQuery(char *name, int salary,int queryType);
//----------------------------
