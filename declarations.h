#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<signal.h>
#include <sys/shm.h> 
#include <sys/wait.h>
//----------------------------
#define NUMBER_OF_PROCESS 6
#define NUMBER_OF_CLIENTS 3
#define DBMANAGER_INDEX 0
#define QUERY_LOGGER_INDEX 1
#define LOGGER_INDEX 2
#define LOGGER_SHAREDMEMORY_SIZE 1024
#define DBMANAGER_SHAREDMEMORY_SIZE 1024
//----------------------------
#define ROLE_DB_MANAGER 1
#define ROLE_DB_CLIENT 2
#define ROLE_LOGGER 3
#define ROLE_QUERY_LOGGER 4
#define ROLE_PHASE2_MODUL 5
//----------------------------
#define MESSAGE_TYPE_ADD 1
#define MESSAGE_TYPE_MODIFY 2
#define MESSAGE_TYPE_ACQUIRE 3
#define MESSAGE_TYPE_RELEASE 4
#define MESSAGE_TYPE_QUERY 5
//----------------------------
#define QUERY_BY_NAME 1
#define QUERY_BY_SALARY 2
#define QUERY_HYBRID 3
//----------------------------
#define MAX_NUMBER_OF_RECORDS 1000
//----------------------------
#define SEMAPHORE_OCCUPIED 0
#define SEMAPHORE_AVAILABLE 1
//----------------------------
/*Global Variables*/
key_t msgqid;
int LoggerSharedMemoryId=-1;
int DBSharedMemoryId=-1;
int processesIds[NUMBER_OF_PROCESS]={-1};
//----------------------------
/*System info*/
struct message
{
    long mtype;
    int destinationProcess;
    int key;
    char* name;
    int salary;
    int modification;
    int queryType;
    int searchedSalary;
    char* searchedString;
    char* logString;
};

struct parentMsgBuff
{
   long mtype;
   char * role;
   int DBManagerId;
   int LoggerId;
   int QueryLoggerId;
   int sharedMemoryId;
   
};

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
/*Parent functions*/
int* readConfiguartions(); 
pid_t forkAllChildren(int* configurations); //implemented
void initializeSharedRecources(); //implemented
void roleIdentification();  //implemented
void do_child(); //base implemented
//----------------------------
/*DB Manager functions*/
void respondToRequests();
void respondToAdd(char* name, int salary, int lastKey); //implemented
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue); //implemented
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
void respondToRelease(int releasedRecordKey, struct waitingQueue* waitingQueueOfThePassedKey); //implemented
//----------------------------
/*DB Client functions*/
void requestToAdd(char* name, int salary);
void requestToModify(int key, int modification);
void requestToAcquire(int key);
void requestToRelease(int key);
void requestToQuery(int queryType, int searchedSalary, char* searchedString);
//----------------------------
/*logging functions*/
//comment for ahmed yehia alter the parameters to suit you logic
void sendQueryLog();
void sendLog(int OrderOfSendingProcess, char* logString, int loggerPID);
void writeLog(char* logString);
//----------------------------