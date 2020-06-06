#include "includes.h"

//------------Semaphores----------------
struct inQueueProcess 
{ 
    int PID; 
    struct inQueueProcess* nextProcess; 
};
struct waitingQueue 
{ 
    struct inQueueProcess *front, *rear; 
};
struct inQueueProcess* createNewNode(int givenPID); 
struct waitingQueue* createWaitingQueue(); 
void addToWaitingQueue(struct waitingQueue* givenQueue, int PID); 
int removeFromWaitingQueue(struct waitingQueue* givenQueue);  
void handlingSIGUSR1_and_IgnoringSigStop(int signum);

//--------------Parent-------------
int readConfigurationFileParent();
pid_t forkAllChildren(int numberOfClients); 
void initializeSharedRecources(); 
void roleIdentification();  
void do_child(); 

//------------Global declarations----------------
#define PARENT_CONFIGURATION_FILE_NAME "parentConfig.txt"
#define NUMBER_OF_PROCESS_OF_MAIN_MODULES 3
#define MAX_NUMBER_OF_PROCESS 100
#define ERROR_IN_CREATE -1
#define CHILD_FORK_RETURN 0
#define FILE_ERROR NULL
#define ERROR_IN_RECEIVE -1
#define DBMANAGER_INDEX 0
#define QUERY_LOGGER_INDEX 1
#define LOGGER_INDEX 2
#define LOGGER_SHAREDMEMORY_SIZE 1024
//----------------------------
key_t msgqid;
int LoggerSharedMemoryId=-1;
int DBSharedMemoryId=-1;
int processesIds[MAX_NUMBER_OF_PROCESS]={-1};
int numberOfClients=0;
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
    int requestedAction;
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

struct loggerMsgQ
{    
    long mtype;
    int PID;
    int requestedAction;
};
struct queryLoggerMsgQ
{
    long mtype;
    int PID;
    char message[MAXCHAR];
    int requestedAction;
};
