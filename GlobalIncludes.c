#include "GlobalIncludes.h"


//-----------------------Semaphores-------------------------
struct inQueueProcess *createNewNode(int givenPID)
{
    struct inQueueProcess *newProcess = (struct inQueueProcess *)malloc(sizeof(struct inQueueProcess));
    newProcess->PID = givenPID;
    newProcess->nextProcess = NO_NODE;
    return newProcess;
}
struct waitingQueue *createWaitingQueue()
{
    struct waitingQueue *newQueue = (struct waitingQueue *)malloc(sizeof(struct waitingQueue));
    newQueue->front = newQueue->rear = NO_NODE;
    return newQueue;
}
void addToWaitingQueue(struct waitingQueue *givenQueue, int PID)
{
    struct inQueueProcess *newProcess = createNewNode(PID);
    if (givenQueue->rear == NO_NODE)
    {
        givenQueue->front = givenQueue->rear = newProcess;
        return;
    }
    givenQueue->rear->nextProcess = newProcess;
    givenQueue->rear = newProcess;
}
int removeFromWaitingQueue(struct waitingQueue *givenQueue)
{
    if (givenQueue->front == NO_NODE)
        return NO_PROCESS_WAITING;

    struct inQueueProcess *removedProcess = givenQueue->front;

    givenQueue->front = givenQueue->front->nextProcess;

    if (givenQueue->front == NO_NODE)
        givenQueue->rear = NO_NODE;
    int removedProcessPID = removedProcess->PID;
    free(removedProcess);
    return removedProcessPID;
}
void handlingSIGUSR1_and_IgnoringSigStop(int signum)
{
    signal(SIGTSTP, SIG_IGN);
}


//-----------------------Parent--------------------------
pid_t forkAllChildren(int numberOfClients)
{
    pid_t pid;
    msgqid = msgget(IPC_PRIVATE, 0644);
    if (msgqid == ERROR_IN_CREATE)
    {
        perror("Error in create");
        exit(0);
    }
    for (int forkedProcessIndex= 0; forkedProcessIndex< numberOfClients+NUMBER_OF_PROCESS_OF_MAIN_MODULES; forkedProcessIndex++)
    {
        pid = fork();
        if (pid == ERROR_IN_CREATE)
        {
            perror("Error in fork");
            exit(0);
        }
        else if (pid == CHILD_FORK_RETURN)
        {
            break;
        }
        processesIds[forkedProcessIndex] = pid;
    }
    return pid;
}
int readConfigurationFileParent()
{
    char buffer[50];
    FILE *filePointer;
    char *filename;
    char word[MAXCHAR];
    
    filePointer = fopen(PARENT_CONFIGURATION_FILE_NAME, "r");

    if (filePointer == FILE_ERROR)
    {
        printf("Could not open file %s\n", filename);
        return 0;
    }

    if (fscanf(filePointer, "%s", word) != EOF)
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
}
void roleIdentification()
{
    int send_val;

    key_t clientDBManager_msgqid = msgget(IPC_PRIVATE, 0644);      
    key_t processesLogger_msgqid = msgget(IPC_PRIVATE, 0644);      
    key_t processesQueryLogger_msgqid = msgget(IPC_PRIVATE, 0644); 

    int clientNumber = 1;
    for (int forkedProcessIndex = 0; forkedProcessIndex< numberOfClients+NUMBER_OF_PROCESS_OF_MAIN_MODULES; forkedProcessIndex++)
    {
        struct parentMsgBuff message;
        message.mtype = processesIds[forkedProcessIndex];
        if (forkedProcessIndex== DBMANAGER_INDEX) 
        {
            message.role = "DBManager";
            message.sharedMemoryId = LoggerSharedMemoryId;
            message.clientDBManagerMsgQId = clientDBManager_msgqid;
            message.DBSharedMemoryId = DBSharedMemoryId;
            message.processesLoggerMsgQId = processesLogger_msgqid;
            message.LoggerId = processesIds[LOGGER_INDEX];
        }
        else if (forkedProcessIndex== QUERY_LOGGER_INDEX) 
        {
            message.role = "QueryLogger";
            message.queryLoggerMsgQId = processesQueryLogger_msgqid;
        }
        else if (forkedProcessIndex== LOGGER_INDEX)
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