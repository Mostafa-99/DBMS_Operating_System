#include "includes.h"

struct Log *loggerSharedMemory = NULL;
int loggerSemaphore = SEMAPHORE_AVAILABLE;
int processesLoggerMsgQId;
struct waitingQueue waitingQueueForLoggerSharedMemory;
struct loggerMsgQ loggerMsgQLogger;
void initializeLogger(int sharedMemoryIdReceived, int processesLoggerMsgQIdReceived);
void do_logger(int sharedMemoryIdReceived, int processesLoggerMsgQIdReceived);
void respondToReleaseLogger(struct waitingQueue *waitingQueueForLoggerSharedMemory);
void respondToAcquireLogger(int PID);