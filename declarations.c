#include "declarations.h"
/*Parent*/
pid_t forkAllChildren(int* configurations)
{
  pid_t pid;
  msgqid = msgget(IPC_PRIVATE, 0644); // or msgget(12613, IPC_CREATE | 0644)
  if(msgqid == 0)
  {	
    perror("Error in create");
    exit(0);
  }
  printf("msgqid = %d\n", msgqid);
  for(int i=0;i<NUMBER_OF_PROCESS;i++)
  {
    pid = fork();
    if(pid==-1)
    { 	
        perror("Error in fork");  	  	
        exit(0);
    }
    else if(pid==0){
      break;
    }
    processesIds[i]=pid;
    printf("I'm parent my new child: %d\n",processesIds[i]);
  }
  return pid;
}
void initializeSharedRecources()
{   
    LoggerSharedMemoryId = shmget(IPC_PRIVATE,LOGGER_SHAREDMEMORY_SIZE,0666|IPC_CREAT);
    DBSharedMemoryId = shmget(IPC_PRIVATE,DBMANAGER_SHAREDMEMORY_SIZE,0666|IPC_CREAT);
    printf("I'm parent I created the resources: %d  and  %d\n",LoggerSharedMemoryId,DBSharedMemoryId);
}
void roleIdentification()
{
    int send_val;
    for(int i=0;i<NUMBER_OF_PROCESS;i++)
    {     
        struct parentMsgBuff message;
        message.mtype=processesIds[i];
        if(i==DBMANAGER_INDEX)//BDManager
        {
            message.role="DBManager";
            message.sharedMemoryId=LoggerSharedMemoryId;
        }
        else if(i==QUERY_LOGGER_INDEX) //QueryLogger
        {
            message.role="QueryLogger";
        }
        else if(i==LOGGER_INDEX)
        {
            message.role="Logger";
            message.sharedMemoryId=LoggerSharedMemoryId;  
        }
        else
        {
            message.role="Client";
            message.DBManagerId=processesIds[DBMANAGER_INDEX];
            message.QueryLoggerId=processesIds[QUERY_LOGGER_INDEX];
            message.sharedMemoryId=LoggerSharedMemoryId;  
        }
        send_val = msgsnd(msgqid, &message, sizeof(message)-sizeof(message.mtype), !IPC_NOWAIT);
    }
}
void do_child()
{
    int rec_val;
    struct parentMsgBuff message; 
    /* receive all types of messages */
    rec_val = msgrcv(msgqid, &message, sizeof(message)-sizeof(message.mtype), getpid(), !IPC_NOWAIT);  
    
    if(rec_val == -1)
    {
        perror("Error in receive");
    }
    else
    {
        printf("Role Message received I am child: %d and I am %s \n",getpid(),message.role);
        if(message.role=="DBManager")
        {
            //do_DBManager();
        }
        else if(message.role=="QueryLogger")
        {
            //do_QueryLogger;
        }
        else if(message.role=="Logger")
        {
            //do_Logger();
        }
        else
        {
            //do_Client();
        } 
    }
}
//----------------------------------------------------------------
/*Semaphores*/
struct inQueueProcess* createNewNode(int givenPID) 
{ 
    struct inQueueProcess* newProcess = (struct inQueueProcess*)malloc(sizeof(struct inQueueProcess)); 
    newProcess->PID = givenPID; 
    newProcess->nextProcess = NULL; 
    return newProcess; 
}
struct waitingQueue* createWaitingQueue() 
{ 
    struct waitingQueue* newQueue = (struct waitingQueue*)malloc(sizeof(struct waitingQueue)); 
    newQueue->front = newQueue->rear = NULL; 
    return newQueue; 
}
void addToWaitingQueue(struct waitingQueue* givenQueue, int PID)
{
    struct inQueueProcess* newProcess = createNewNode(PID); 
    if (givenQueue->rear == NULL) 
    { 
        givenQueue->front = givenQueue->rear = newProcess; 
        return; 
    } 
    givenQueue->rear->nextProcess = newProcess; 
    givenQueue->rear = newProcess; 
}
int removeFromWaitingQueue(struct waitingQueue* givenQueue) 
{ 
    if (givenQueue->front == NULL) return; 
   
    struct inQueueProcess* removedProcess = givenQueue->front; 
  
    givenQueue->front = givenQueue->front->nextProcess; 
  
    if (givenQueue->front == NULL) givenQueue->rear = NULL; 
    int removedProcessPID = removedProcess->PID;
    free(removedProcess); 
}
void handlingSIGUSR1_and_IgnoringSigStop()
{
    signal(SIGSTOP, SIG_IGN);
}
//----------------------------------------------------------------
/*DB Manager*/
void respondToAdd(char* name, int salary, int lastKey)
{
    struct DBrecord newRecord;
    newRecord.name = name;
    newRecord.salary = salary;
    newRecord.key = lastKey+1;
}
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue)
{
    int lowerLimit=0;
    int currentIndex=0;
    for(;DBtable[currentIndex].key < keyOfTheRecordToBeModified;currentIndex++);
    if(currentIndex == keyOfTheRecordToBeModified)
    {
        DBtable[currentIndex].salary += modificationValue;
        if(DBtable[currentIndex].salary < lowerLimit) DBtable[currentIndex].salary = lowerLimit;
    }
}
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey)
{
    if(DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED)
    {
        addToWaitingQueue(waitingQueueOfThePassedKey, CallingProccessPID);
    }
    else
    {
        DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED;
        kill(CallingProccessPID,SIGUSR1);
        kill(CallingProccessPID,SIGCONT);
    }
}
void respondToRelease(int releasedRecordKey, struct waitingQueue* waitingQueueOfThePassedKey)
{
    DBsemaphores[releasedRecordKey] == SEMAPHORE_AVAILABLE;
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueOfThePassedKey);
    kill(releasedProcessPID,SIGCONT);
}
//----------------------------------------------------------------