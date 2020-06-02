#include "includes.h"
//----------------------------
#define NUMBER_OF_PROCESS 6
#define NUMBER_OF_CLIENTS 3
#define DBMANAGER_INDEX 0
#define QUERY_LOGGER_INDEX 1
#define LOGGER_INDEX 2
#define LOGGER_SHAREDMEMORY_SIZE 1024
//#define DBMANAGER_SHAREDMEMORY_SIZE 
#define MAX_NUMBER_OF_RECORDS 1000
//----------------------------
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
};
struct message
{
    long mtype;
    int destinationProcess; //to be changed
    int key;
    char name[MAXCHAR];
    int salary;
    int modification;
    int callingProcessID;
    
    int queryType;
    int searchedSalary;
    char* searchedString;
    char* logString;
    int queryKeys[MAX_NUMBER_OF_RECORDS];
};
//----------------------------
/*Parent functions*/
int* readConfiguartions(); 
pid_t forkAllChildren(int* configurations); //implemented
void initializeSharedRecources(); //implemented
void roleIdentification();  //implemented
//----------------------------