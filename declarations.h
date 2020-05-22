#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
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
#define MESSAGE_TYPE_PRINT 5
#define MESSAGE_TYPE_QUERY 6
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
struct DBrecord
{
    int key;
    char* name;
    int salary;
};
int DBsemaphores[MAX_NUMBER_OF_RECORDS];
int SharedMemoryID;
int ManagerPID;
message DBtable[MAX_NUMBER_OF_RECORDS];
//----------------------------
/*Parent functions*/
int* readConfiguartions(); 
void forkAllChildren(int* configurations);
void initializeSharedRecources();
//----------------------------
/*DB Manager functions*/
void respondToRequests(); //most abstract function
void respondToAdd(char* name, int salary);
void respondToModify(int key, int modification);
void respondToAcquire(int key);
void respondToRelease(int key);
void respondToQuery(int queryType, int searchedSalary, char* searchedString);
//----------------------------
/*DB Client functions*/
void requestToAdd(char* name, int salary);
void requestToModify(int key, int modification);
void requestToAcquire(int key);
void requestToRelease(int key);
void requestToQuery(int queryType, int searchedSalary, char* searchedString);
//----------------------------
/*logging functions*/
void sendLog(int OrderOfSendingProcess, char* logString, int loggerPID);
void writeLog(char* logString);
//----------------------------