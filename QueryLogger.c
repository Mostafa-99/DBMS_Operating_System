#include "QueryLogger.h"





void do_queryLogger(int queryLoggerMsgQIdReceived)
{
  initializeQueryLogger(queryLoggerMsgQIdReceived);
 
    while (1)
    {
      //  printf("QueryLogger is here  MsgQId: %d  \n");
        //sleep(100);
       int rec=msgrcv(queryLoggerMsgQIdQuery, &queryLoggerMsgQProccess, (sizeof(queryLoggerMsgQProccess) - sizeof(queryLoggerMsgQProccess.mtype)), getpid(), !IPC_NOWAIT);
       // if(rec!=-1)
        printf("QueryLogger is not here  %s\n",queryLoggerMsgQProccess.message);



       //write


    }
}

void  initializeQueryLogger(int queryLoggerMsgQIdReceived)
{
    queryLoggerMsgQIdQuery=queryLoggerMsgQIdReceived;
}
