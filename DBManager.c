#include "DBManager.h"
//----------------------------------------------------------------
int respondToAdd(char* name, int salary, int lastKey)
{
    struct DBrecord newRecord;
    strcpy(newRecord.name,name);
    newRecord.salary = salary;
    newRecord.key = lastKey+1;
    DBtable[lastKey+1]=newRecord;
   // printf("New record added to DB with name: %s and Salary: %d with key: %d \n",DBtable[lastKey+1].name,DBtable[lastKey+1].salary,DBtable[lastKey+1].key);
    return lastKey+1;
}
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue)
{
    int lowerLimit=0;
    int currentIndex=0;
    for(;DBtable[currentIndex].key < keyOfTheRecordToBeModified;currentIndex++);
    if(currentIndex == keyOfTheRecordToBeModified )
    {
      //  printf("New record will be modified from DB with name: %s and Salary: %d with key: %d \n",DBtable[currentIndex].name,DBtable[currentIndex].salary,currentIndex);
        DBtable[currentIndex].salary += modificationValue;
        if(DBtable[currentIndex].salary < lowerLimit) DBtable[currentIndex].salary = lowerLimit;
      //  printf("New record modified succ in DB with name: %s and new Salary: %d with key: %d \n",DBtable[currentIndex].name,DBtable[currentIndex].salary,currentIndex);
        //respondToRelease(keyOfTheRecordToBeModified,pointersOfWaitingQueuesForRecordKeys[keyOfTheRecordToBeModified]);
    }

}
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey)
{
    if(DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED)
    {
       //  printf("Respond Acquire:add to queue DBmanager\n");
        addToWaitingQueue(waitingQueueOfThePassedKey, CallingProccessPID);
    }
    else
    {
        DBsemaphores[requiredRecordKey] = SEMAPHORE_OCCUPIED;
        //kill(CallingProccessPID,SIGUSR1);
       // kill(CallingProccessPID,SIGCONT);
        receivedMessage.mtype=CallingProccessPID;
        msgsnd(messageQueueID, &receivedMessage, sizeof(receivedMessage)-sizeof(receivedMessage.mtype), !IPC_NOWAIT);
      //  printf("Respond Acquire:Semaphore available DBmanager\n");

    }
}
void respondToRelease(int releasedRecordKey, int CallingProccessPID,struct waitingQueue* waitingQueueOfThePassedKey)
{

  //  printf("Semaphore %d is now released\n",releasedRecordKey);
    DBsemaphores[releasedRecordKey] = SEMAPHORE_AVAILABLE;
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueOfThePassedKey);
    if(releasedProcessPID!=-1)
    {
       // kill(releasedProcessPID,SIGCONT);
   //     printf("process %d returned from the wait\n",releasedProcessPID);
          receivedMessage.mtype=releasedProcessPID;
       msgsnd(messageQueueID, &receivedMessage, sizeof(receivedMessage)-sizeof(receivedMessage.mtype), !IPC_NOWAIT);
    }


}
void respondToQuery(int queryType, int searchedSalary, char* searchedName, int callingProcessID) {
    if(lastKey+1>0) {
        printf("Respond to Query DBmanager: %d\n",queryType);
        struct message sendSearchResult;
        int notReturnedKey = -1;
        int checkedRecordIndex=0;
        int returnedKeyIndex=0;
        memset(sendSearchResult.queryKeys, notReturnedKey, lastKey+1);
        if(queryType == QUERY_BY_FULL_TABLE) {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if(queryType == QUERY_BY_EXACT_NAME) {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(strcmp(DBtable[checkedRecordIndex].name,searchedName)==0){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return keyyyy: %d\n",checkedRecordIndex);
                }
            }
        }
        else if(queryType == QUERY_BY_PART_OF_NAME) {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(strstr(DBtable[checkedRecordIndex].name,searchedName)!=NULL){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return key: %d\n",checkedRecordIndex);
                    
                }
            }
        }
        else if(queryType==QUERY_BY_EXACT_SALARY) {

            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(DBtable[checkedRecordIndex].salary == searchedSalary){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return key: %d\n",checkedRecordIndex);
                }
            }
    //printf("I am the Manager I request a query with type  and salary  searching for:  msg to \n");
        }
        else if(queryType==QUERY_BY_LESS_THAN_SALARY) {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(DBtable[checkedRecordIndex].salary < searchedSalary){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return key: %d\n",checkedRecordIndex);
                }
            }
        }
        else if(queryType==QUERY_BY_GREATER_THAN_SALARY) {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(DBtable[checkedRecordIndex].salary > searchedSalary){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return key: %d\n",checkedRecordIndex);
                }
            }
        }
        else if(queryType==QUERY_BY_LESS_THAN_OR_EQUAL_SALARY) {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(DBtable[checkedRecordIndex].salary <= searchedSalary){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return key: %d\n",checkedRecordIndex);
                }
            }
        }
        else if(queryType==QUERY_BY_GREATER_THAN_OR_EQUAL_SALARY) {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(DBtable[checkedRecordIndex].salary >= searchedSalary){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return key: %d\n",checkedRecordIndex);
                }
            }
        }
        else if(queryType==QUERY_BY_EXACT_NAME_AND_SALARY_EXACT_HYBRID)
        {
            for(;checkedRecordIndex<=lastKey;checkedRecordIndex++) {
                if(DBtable[checkedRecordIndex].salary == searchedSalary && strcmp(DBtable[checkedRecordIndex].name, searchedName)==0){
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                    printf("return key: %d\n",checkedRecordIndex);
                }
            }
        }
        if (returnedKeyIndex>0) {
            sendSearchResult.mtype=callingProcessID;

            //to lock the array
            sendSearchResult.queryKeys[returnedKeyIndex++]=-1;

            msgsnd(messageQueueID, &sendSearchResult, sizeof(sendSearchResult)-sizeof(sendSearchResult.mtype), !IPC_NOWAIT);
        }
    }
    
}
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived,int DBSharedMemoryIdReceived, int loggerMsgQIdReceived,int LoggerIdReceived){
    messageQueueID=messageQueueIdReceived;
    DBManagerPID = getpid();
    sharedMemoryId = sharedMemoryIdReceived;
    memset(DBsemaphores, SEMAPHORE_AVAILABLE, MAX_NUMBER_OF_RECORDS);
    DBtable = (struct DBrecord *)shmat(DBSharedMemoryIdReceived, (void *)0, 0);
    loggerMsgQIdDBManager=loggerMsgQIdReceived;
    signal(SIGUSR1, handlingSIGUSR1_and_IgnoringSigStop);

    DBManagerLogger = (struct Log *)shmat(sharedMemoryId, (void *)0, 0);
    LoggerId = LoggerIdReceived;

}
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived,int LoggerIdReceived)
{
    initializeDBManager(clientDBManagerMsgQIdReceived, sharedMemoryIdReceived, DBSharedMemoryIdReceived,loggerMsgQIdReceived,LoggerIdReceived);
    while(1)
    {
        msgrcv(messageQueueID, &receivedMessage,(sizeof(receivedMessage) - sizeof(receivedMessage.mtype)),DBManagerPID,!IPC_NOWAIT);
        int messageType = receivedMessage.destinationProcess;
        if(messageType == MESSAGE_TYPE_ADD) {
            lastKey = respondToAdd(receivedMessage.name,receivedMessage.salary,lastKey);
            pointersOfWaitingQueuesForRecordKeys[lastKey] = createWaitingQueue();
            logDBManagerAdd(receivedMessage.name,receivedMessage.salary);
        }
        else if (messageType == MESSAGE_TYPE_MODIFY) {
            respondToModify(receivedMessage.key, receivedMessage.modification);
        }
        else if (messageType == MESSAGE_TYPE_ACQUIRE) {
            respondToAcquire(receivedMessage.key, receivedMessage.callingProcessID, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
        }
        else if (messageType == MESSAGE_TYPE_RELEASE) {
            respondToRelease(receivedMessage.key, receivedMessage.callingProcessID,pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
        }
        else if (messageType == MESSAGE_TYPE_QUERY) {
            respondToQuery(receivedMessage.queryType, receivedMessage.searchedSalary, receivedMessage.searchedString, receivedMessage.callingProcessID);
        }
    }
}

void logDBManagerAdd(char *name, int salary)
{
    int send_val;
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
      //  printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq  %d____ %d\n",loggerMsgQIdDBManager,LoggerId);
    if(send_val!=-1)
    {
    }

    raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);

    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char msgArray[MAXCHAR] = "I am DBManager I added ";
    strcat(msgArray, name);

    char salaryString[MAXCHAR];
    sprintf(salaryString, "%d", salary);

    strcat(msgArray, " with salary ");
    strcat(msgArray, salaryString);
    strcpy(DBManagerLogger->message, msgArray);
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_RELEASE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    
    
}
