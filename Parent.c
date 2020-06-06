#include "Parent.h"
//#include "Client.h"

/*Parent*/
pid_t forkAllChildren(int numberOfClients)
{
    pid_t pid;
    msgqid = msgget(IPC_PRIVATE, 0644); // or msgget(12613, IPC_CREATE | 0644)
    if (msgqid == 0)
    {
        perror("Error in create");
        exit(0);
    }
    // printf("msgqid = %d\n", msgqid);
    for (int i = 0; i < numberOfClients+NUMBER_OF_PROCESS_OF_MAIN_MODULES; i++)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("Error in fork");
            exit(0);
        }
        else if (pid == 0)
        {
            break;
        }
        processesIds[i] = pid;
        //  printf("I'm parent my new child: %d\n",processesIds[i]);
    }
    return pid;
}
int readConfigurationFileParent()
{
    char buffer[50];
    FILE *fp;
    char *filename;
    char word[MAXCHAR];
    
    fp = fopen(PARENT_CONFIGURATION_FILE_NAME, "r");

    //read config
    if (fp == NULL)
    {
        printf("Could not open file %s\n", filename);
        return 0;
    }

    if (fscanf(fp, "%s", word) != EOF)
    {
        return atoi(word);
    }
    else
    {
        return 0;
    }
}
void initializeSharedRecources()
{
    LoggerSharedMemoryId = shmget(IPC_PRIVATE, sizeof(struct Log), 0666 | IPC_CREAT);
    DBSharedMemoryId = shmget(IPC_PRIVATE, MAX_NUMBER_OF_RECORDS * sizeof(struct DBrecord), 0666 | IPC_CREAT);
    // printf("I'm parent I created the resources: %d  and  %d\n",LoggerSharedMemoryId,DBSharedMemoryId);
}
void roleIdentification()
{
    int send_val;

    key_t clientDBManager_msgqid = msgget(IPC_PRIVATE, 0644);      // or msgget(12613, IPC_CREATE | 0644)
    key_t processesLogger_msgqid = msgget(IPC_PRIVATE, 0644);      // or msgget(12613, IPC_CREATE | 0644)
    key_t processesQueryLogger_msgqid = msgget(IPC_PRIVATE, 0644); // or msgget(12613, IPC_CREATE | 0644)

    int clientNumber = 1;
    for (int i = 0; i < numberOfClients+NUMBER_OF_PROCESS_OF_MAIN_MODULES; i++)
    {
        struct parentMsgBuff message;
        message.mtype = processesIds[i];
        if (i == DBMANAGER_INDEX) //BDManager
        {
            message.role = "DBManager";
            message.sharedMemoryId = LoggerSharedMemoryId;
            message.clientDBManagerMsgQId = clientDBManager_msgqid;
            message.DBSharedMemoryId = DBSharedMemoryId;
            message.processesLoggerMsgQId = processesLogger_msgqid;
            message.LoggerId = processesIds[LOGGER_INDEX];
        }
        else if (i == QUERY_LOGGER_INDEX) //QueryLogger
        {
            message.role = "QueryLogger";
            message.queryLoggerMsgQId = processesQueryLogger_msgqid;
        }
        else if (i == LOGGER_INDEX)
        {
            message.role = "Logger";
            message.sharedMemoryId = LoggerSharedMemoryId;
            message.processesLoggerMsgQId = processesLogger_msgqid;
        }
        else
        {
            message.role = "Client";
            message.DBManagerId = processesIds[DBMANAGER_INDEX];
            message.QueryLoggerId = processesIds[QUERY_LOGGER_INDEX];
            message.sharedMemoryId = LoggerSharedMemoryId;
            message.clientDBManagerMsgQId = clientDBManager_msgqid;
            message.clientNumber = clientNumber;
            message.processesLoggerMsgQId = processesLogger_msgqid;
            message.LoggerId = processesIds[LOGGER_INDEX];
            message.queryLoggerMsgQId = processesQueryLogger_msgqid;
            clientNumber++;
        }
        send_val = msgsnd(msgqid, &message, sizeof(message) - sizeof(message.mtype), !IPC_NOWAIT);
    }
}
//-------------------------------------------------
/*Semaphores*/
struct inQueueProcess *createNewNode(int givenPID)
{
    struct inQueueProcess *newProcess = (struct inQueueProcess *)malloc(sizeof(struct inQueueProcess));
    newProcess->PID = givenPID;
    newProcess->nextProcess = NULL;
    return newProcess;
}
struct waitingQueue *createWaitingQueue()
{
    struct waitingQueue *newQueue = (struct waitingQueue *)malloc(sizeof(struct waitingQueue));
    newQueue->front = newQueue->rear = NULL;
    return newQueue;
}
void addToWaitingQueue(struct waitingQueue *givenQueue, int PID)
{
    struct inQueueProcess *newProcess = createNewNode(PID);
    if (givenQueue->rear == NULL)
    {
        givenQueue->front = givenQueue->rear = newProcess;
        return;
    }
    givenQueue->rear->nextProcess = newProcess;
    givenQueue->rear = newProcess;
}
int removeFromWaitingQueue(struct waitingQueue *givenQueue)
{
    if (givenQueue->front == NULL)
        return -1;

    struct inQueueProcess *removedProcess = givenQueue->front;

    givenQueue->front = givenQueue->front->nextProcess;

    if (givenQueue->front == NULL)
        givenQueue->rear = NULL;
    int removedProcessPID = removedProcess->PID;
    free(removedProcess);
    return removedProcessPID;
}