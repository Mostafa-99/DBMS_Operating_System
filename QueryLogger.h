
#include "includes.h"

int queryLoggerMsgQIdQuery;
int numberOfCurrentLog;

struct queryLoggerMsgQ queryLoggerMsgQProccess;
struct waitingQueue waitingQueueForQueryLogger;
int queryLoggerSemaphore=SEMAPHORE_AVAILABLE;

void do_queryLogger(int queryLoggerMsgQIdReceived);
void initializeQueryLogger(int queryLoggerMsgQIdReceived);
void respondToReleaseQueryLogger(struct waitingQueue *waitingQueueForQueryLogger);
void respondToAcquireQueryLogger(int PID);