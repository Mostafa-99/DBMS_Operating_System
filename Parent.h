#include "includes.h"
//----------------------------
#define NUMBER_OF_PROCESS 6
#define NUMBER_OF_CLIENTS 3
#define DBMANAGER_INDEX 0
#define QUERY_LOGGER_INDEX 1
#define LOGGER_INDEX 2
#define LOGGER_SHAREDMEMORY_SIZE 1024
#define DBMANAGER_SHAREDMEMORY_SIZE 1024
//----------------------------
/*Global Variables*/
key_t msgqid;
int LoggerSharedMemoryId=-1;
int DBSharedMemoryId=-1;
int processesIds[NUMBER_OF_PROCESS]={-1};
//----------------------------
struct parentMsgBuff
{
   long mtype;
   char * role;
   int DBManagerId;
   int LoggerId;
   int QueryLoggerId;
   int sharedMemoryId;
   int clientDBManagerMsgQId;
   int clientNumber;
};
//----------------------------
/*Parent functions*/
int* readConfiguartions(); 
pid_t forkAllChildren(int* configurations); //implemented
void initializeSharedRecources(); //implemented
void roleIdentification();  //implemented
//----------------------------