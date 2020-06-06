#include "Logger.h"
void initializeLogger(int sharedMemoryIdReceived, int processesLoggerMsgQIdReceived)
{
    loggerSharedMemory = (struct Log *)shmat(sharedMemoryIdReceived, (void *)0, 0);
    processesLoggerMsgQId = processesLoggerMsgQIdReceived;
}

void do_logger(int sharedMemoryIdReceived, int processesLoggerMsgQIdReceived)
{
    initializeLogger(sharedMemoryIdReceived, processesLoggerMsgQIdReceived);
    while (1)
    {
        //printf("%d aa\n", loggerSharedMemory->number);

        msgrcv(processesLoggerMsgQId, &loggerMsgQLogger, (sizeof(loggerMsgQLogger) - sizeof(loggerMsgQLogger.mtype)), getpid(), !IPC_NOWAIT);
        if(loggerMsgQLogger.destinationProcess==MESSAGE_TYPE_ACQUIRE)
        {
            respondToAcquireLogger(loggerMsgQLogger.PID);
        }
        else if(loggerMsgQLogger.destinationProcess==MESSAGE_TYPE_RELEASE)
        {
      //  printf("loggerMsgQLogger.destinationProcess: %d aa\n", loggerMsgQLogger.destinationProcess);
            respondToReleaseLogger(&waitingQueueForLoggerSharedMemory);
        }
    }

    //detach from shared memory
    shmdt(loggerSharedMemory);
}
void respondToReleaseLogger(struct waitingQueue *waitingQueueForLoggerSharedMemory)
{
    FILE *fileOpened;
    char fileToOpen[80];
    if(strcmp(loggerSharedMemory->number, "0") == 0)
    {
        strcpy(fileToOpen, "DBLogger.txt");
    }
    else
    {
        strcpy(fileToOpen, loggerSharedMemory->number);
        strcat(fileToOpen,"Logger.txt");
    }
    
    

    fileOpened = fopen(fileToOpen, "a"); //opening file  a
    if (fileOpened == NULL)
    {
        printf("unable to open file\n");
    }
    else
    {
        fprintf(fileOpened, "%lu\t", (unsigned long)time(NULL));
        fprintf(fileOpened, "%s", loggerSharedMemory->message);   //writing data into file
        fprintf(fileOpened, "\n");                                      //writing data into file
    }
    fflush(fileOpened);
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueForLoggerSharedMemory);
   
    if(releasedProcessPID!=-1)
    {
        kill(releasedProcessPID, SIGUSR1);
        kill(releasedProcessPID, SIGCONT);
      /*  loggerMsgQLogger.mtype=releasedProcessPID;
        msgsnd(processesLoggerMsgQId, &loggerMsgQLogger, sizeof(loggerMsgQLogger) - sizeof(loggerMsgQLogger.mtype), !IPC_NOWAIT);*/

    }
    else
    {
        loggerSemaphore = SEMAPHORE_AVAILABLE;
        
    }
    
    
    //printf("Logg done wakeup next\n");
    
      
        
   // if(releasedProcessPID)printf("release logger wake up: %d\n",releasedProcessPID);
}
void respondToAcquireLogger(int PID)
{
    if (loggerSemaphore == SEMAPHORE_AVAILABLE)
    {
        loggerSemaphore = SEMAPHORE_OCCUPIED;
       // printf("Available: I am the logger wake up %d \n",PID);
        kill(PID,SIGUSR1);
        kill(PID, SIGCONT);
    }
    else
    {
      //  printf("Not available: I am the logger keep sleeping %d \n",PID);
        addToWaitingQueue(&waitingQueueForLoggerSharedMemory, loggerMsgQLogger.PID);
    }
}