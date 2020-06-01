#include "includes.h"
//----------------------------
#define MAXCHAR 1000
#define ADD_MODIFICATION 1
#define SUBTRACT_MODIFICATION -1
//----------------------------
int DBManagerId;
int QueryLoggerId;
int sharedMemoryId;
int clientDBManagerMsgQId;
int clientNumber;

FILE * fp;
char* filename;
char word [MAXCHAR];

int send_val;
struct message message;
//----------------------------
/*DB Client functions*/
void do_client(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived);
void initializeClient(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived);
void openConfigurationFile();
char * readConfigurationFile();
void requestToAdd(char* name, int salary);
void requestToModify(int key, int modification, int value);
void requestToAcquire(int key);
void requestToRelease(int key);
void requestToQuery(int queryType, int searchedSalary, char* searchedString);
//----------------------------