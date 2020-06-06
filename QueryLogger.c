#include "QueryLogger.h"

void do_queryLogger(int queryLoggerMsgQIdReceived)
{

  initializeQueryLogger(queryLoggerMsgQIdReceived);

  while (1)
  {
    int rec = msgrcv(queryLoggerMsgQIdQuery, &queryLoggerMsgQProccess, (sizeof(queryLoggerMsgQProccess) - sizeof(queryLoggerMsgQProccess.mtype)), getpid(), !IPC_NOWAIT);
     // printf("***************************************************************************************QueryLogger rec\n");

    if (queryLoggerMsgQProccess.requestedAction == MESSAGE_TYPE_ACQUIRE)
    {
      respondToAcquireQueryLogger(queryLoggerMsgQProccess.PID);
    }
    else if (queryLoggerMsgQProccess.requestedAction == MESSAGE_TYPE_RELEASE)
    {
      respondToReleaseQueryLogger(&waitingQueueForQueryLogger);
    }
  }
}
void initializeQueryLogger(int queryLoggerMsgQIdReceived)
{
  queryLoggerMsgQIdQuery = queryLoggerMsgQIdReceived;
  numberOfCurrentLog = 1;
}
void respondToReleaseQueryLogger(struct waitingQueue *waitingQueueForQueryLogger)
{

  int releasedProcessPID = removeFromWaitingQueue(waitingQueueForQueryLogger);

  if (releasedProcessPID != NO_PROCESS_WAITING)
  {
    queryLoggerMsgQProccess.mtype = releasedProcessPID;
    //  printf("***************************************************************************************QueryLogger wake up from queue\n");
    msgsnd(queryLoggerMsgQIdQuery, &queryLoggerMsgQProccess, sizeof(queryLoggerMsgQProccess) - sizeof(queryLoggerMsgQProccess.mtype), !IPC_NOWAIT);
  }
  else
  {
       // printf("***************************************************************************************QueryLogger semaphore available\n");

    queryLoggerSemaphore = SEMAPHORE_AVAILABLE;
  }
}
void respondToAcquireQueryLogger(int PID)
{
  if (queryLoggerSemaphore == SEMAPHORE_AVAILABLE)
  {
    //printf("***************************************************************************************QueryLogger send first one\n");
    queryLoggerSemaphore = SEMAPHORE_OCCUPIED;
    queryLoggerMsgQProccess.mtype = PID;
    msgsnd(queryLoggerMsgQIdQuery, &queryLoggerMsgQProccess, sizeof(queryLoggerMsgQProccess) - sizeof(queryLoggerMsgQProccess.mtype), !IPC_NOWAIT);
  }
  else
  {
    //printf("***************************************************************************************QueryLogger add to queue\n");
    addToWaitingQueue(&waitingQueueForQueryLogger,PID);
  }
}