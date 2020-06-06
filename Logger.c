#include "Logger.h"
void initializeLogger(int sharedMemoryIdReceived, int processesLoggerMsgQIdReceived)
{
    loggerSharedMemory = (struct Log *)shmat(sharedMemoryIdReceived, (void *)0, 0);
    processesLoggerMsgQId = processesLoggerMsgQIdReceived;
}

void do_logger(int sharedMemoryIdReceived, int processesLoggerMsgQIdReceived)
{
    initializeLogger(sharedMemoryIdReceived, processesLoggerMsgQIdReceived);
    while (TRUE_)
    {
        msgrcv(processesLoggerMsgQId, &loggerMsgQLogger, (sizeof(loggerMsgQLogger) - sizeof(loggerMsgQLogger.mtype)), getpid(), !IPC_NOWAIT);
        if (loggerMsgQLogger.requestedAction == MESSAGE_TYPE_ACQUIRE)
        {
            respondToAcquireLogger(loggerMsgQLogger.PID);
        }
        else if (loggerMsgQLogger.requestedAction == MESSAGE_TYPE_RELEASE)
        {
            respondToReleaseLogger(&waitingQueueForLoggerSharedMemory);
        }
    }
    shmdt(loggerSharedMemory);
}
void respondToReleaseLogger(struct waitingQueue *waitingQueueForLoggerSharedMemory)
{
    FILE *fileOpened;
    char fileToOpen[80];
    if (strcmp(loggerSharedMemory->number, "0") == IDENTICAL_WORDS)
    {
        strcpy(fileToOpen, "DBLogger.txt");
    }
    else
    {
        strcpy(fileToOpen, loggerSharedMemory->number);
        strcat(fileToOpen, "Logger.txt");
    }

    fileOpened = fopen(fileToOpen, "a");
    if (fileOpened == NULL)
    {
        printf("unable to open file\n");
    }
    else
    {
        fprintf(fileOpened, "%lu\t", (unsigned long)time(NULL));
        fprintf(fileOpened, "%s", loggerSharedMemory->message); 
        fprintf(fileOpened, "\n");                            
    }
    fflush(fileOpened);
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueForLoggerSharedMemory);

    if (releasedProcessPID != NO_PROCESS_WAITING)
    {
        kill(releasedProcessPID, SIGUSR1);
        kill(releasedProcessPID, SIGCONT);
    }
    else
    {
        loggerSemaphore = SEMAPHORE_AVAILABLE;
    }
}
void respondToAcquireLogger(int PID)
{
    if (loggerSemaphore == SEMAPHORE_AVAILABLE)
    {
        loggerSemaphore = SEMAPHORE_OCCUPIED;

        kill(PID, SIGUSR1);
        kill(PID, SIGCONT);
    }
    else
    {
        addToWaitingQueue(&waitingQueueForLoggerSharedMemory, loggerMsgQLogger.PID);
    }
}