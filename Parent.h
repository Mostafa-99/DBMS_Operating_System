#include "includes.h"
//----------------------------------
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


//----------------------------
#define NUMBER_OF_PROCESS 7
#define NUMBER_OF_CLIENTS 4
#define DBMANAGER_INDEX 0
#define QUERY_LOGGER_INDEX 1
#define LOGGER_INDEX 2
#define LOGGER_SHAREDMEMORY_SIZE 1024
//#define DBMANAGER_SHAREDMEMORY_SIZE 
#define MAX_NUMBER_OF_RECORDS 1000
//----------------------------

//-----------------------------
/*Global Variables*/
key_t msgqid;
int LoggerSharedMemoryId=-1;
int DBSharedMemoryId=-1;
int processesIds[NUMBER_OF_PROCESS]={-1};
//----------------------------
struct DBrecord
{
    int key;
    char name[MAXCHAR];
    int salary;
};
//---------------------------------------------
struct parentMsgBuff
{
   long mtype;
   char * role;
   int DBManagerId;
   int DBSharedMemoryId;
   int LoggerId;
   int QueryLoggerId;
   int sharedMemoryId;
   int clientDBManagerMsgQId;
   int clientNumber;
   int processesLoggerMsgQId;
   int queryLoggerMsgQId;
};
struct message
{
    long mtype;
    int destinationProcess;
    int key;
    char name[MAXCHAR];
    int salary;
    int modification;
    int callingProcessID;

    int queryType;
    int searchedSalary;
    char searchedString[MAXCHAR];
    char* logString;
    int queryKeys[MAX_NUMBER_OF_RECORDS];
};

struct Log
{
    char message[MAX_MEASSAGE_SIZE];
    char number[MAX_NUMBER_SIZE];
};
//struct log clientLoggerObj;

struct loggerMsgQ
{    
    long mtype;
    int PID;
    int destinationProcess;
};
struct queryLoggerMsgQ
{
    long mtype;
    struct DBrecord * firstRecordAddress;
    int size;
    char message[MAXCHAR];
};
//----------------------------
/*Parent functions*/
int* readConfiguartions(); 
pid_t forkAllChildren(int* configurations); //implemented
void initializeSharedRecources(); //implemented
void roleIdentification();  //implemented
//----------------------------