#include "includes.h"

//----------------------------
#define ADD_MODIFICATION 1
#define SUBTRACT_MODIFICATION -1
#define ERROR_IN_OPEN 0
//----------------------------
int DBManagerId;
int LoggerId;
int QueryLoggerId;
int sharedMemoryId;
int clientDBManagerMsgQId;
int clientNumber;
int loggerMsgQIdClient;
int queryLoggerMsgQIdClient;

FILE *filePointer;
char *filename;
char word[MAXCHAR];

struct Log *clientLogger;
int send_val;
struct message messageClient;
struct DBrecord *DBtableView;
struct loggerMsgQ messageLoggerClient;
struct queryLoggerMsgQ queryLoggerMsgQClient;
//----------------------------
void do_client(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived, int loggerIdReceived, int queryLoggerMsgQIdReceived);
void initializeClient(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived, int loggerIdReceived, int queryLoggerMsgQIdReceived);
void openConfigurationFile();
char *readConfigurationFile();
void requestToAdd(char *name, int salary);
void requestToModify(int key, int modification, int value);
void requestToAcquire(int key);
void requestToRelease(int key);
void requestToQuery(int queryType, int searchedSalary, char **searchedString);
void sendToQueryLogger(int queryType);

void logAdd(char *name, int salary);
void logModify(int key, int modification, int value);
void logAcquire(int key);
void logRelease(int key);
void logQuery();

void getQueryRequestParameters(int *queryType, int *searchedSalary, char **searchedString);
//----------------------------
