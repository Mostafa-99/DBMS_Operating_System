
#include "includes.h"

int queryLoggerMsgQIdQuery;
int numberOfCurrentLog;

struct queryLoggerMsgQ queryLoggerMsgQProccess;

void do_queryLogger(int queryLoggerMsgQIdReceived);
void  initializeQueryLogger(int queryLoggerMsgQIdReceived);
