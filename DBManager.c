#include "DBManager.h"
//----------------------------------------------------------------
int respondToAdd(char* name, int salary, int lastKey)
{
    struct DBrecord newRecord;
    strcpy(newRecord.name,name);
    newRecord.salary = salary;
    newRecord.key = lastKey+1;
    DBtable[lastKey+1]=newRecord;
    printf("New record added to DB with name: %s and Salary: %d with key: %d \n",DBtable[lastKey+1].name,DBtable[lastKey+1].salary,DBtable[lastKey+1].key);
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
        printf("New record modified succ in DB with name: %s and new Salary: %d with key: %d \n",DBtable[currentIndex].name,DBtable[currentIndex].salary,currentIndex);
        respondToRelease(keyOfTheRecordToBeModified,pointersOfWaitingQueuesForRecordKeys[keyOfTheRecordToBeModified]);
    }

}
void respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue* waitingQueueOfThePassedKey)
{
    if(DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED)
    {
         printf("Respond Acquire:add to queue DBmanager\n");
        addToWaitingQueue(waitingQueueOfThePassedKey, CallingProccessPID);
    }
    else
    {
        printf("Respond Acquire:Semaphore available DBmanager\n");
        DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED;
        kill(CallingProccessPID,SIGUSR1);
        kill(CallingProccessPID,SIGCONT);
    }
}
void respondToRelease(int releasedRecordKey, struct waitingQueue* waitingQueueOfThePassedKey)
{
    printf("Respond to Release DBmanager\n");
    DBsemaphores[releasedRecordKey] == SEMAPHORE_AVAILABLE;
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueOfThePassedKey);
    kill(releasedProcessPID,SIGCONT);
}
void respondToQuery(int queryType, int searchedSalary, char* searchedName, int callingProcessID) {
    if(lastKey+1>0) {
        printf("Respond to Query DBmanager: %s\n",searchedName);
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
                    printf("return key: %d\n",checkedRecordIndex);
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
            msgsnd(messageQueueID, &sendSearchResult, sizeof(sendSearchResult)-sizeof(sendSearchResult.mtype), IPC_NOWAIT);
        }
    }
    
}
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived,int DBSharedMemoryIdReceived, int loggerMsgQIdReceived){
    messageQueueID=messageQueueIdReceived;
    DBManagerPID = getpid();
    sharedMemoryId = sharedMemoryIdReceived;
    memset(DBsemaphores, SEMAPHORE_AVAILABLE, MAX_NUMBER_OF_RECORDS);
    DBtable = (struct DBrecord *)shmat(DBSharedMemoryIdReceived, (void *)0, 0);
    loggerMsgQIdDBManager=loggerMsgQIdReceived;
}
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived)
{
    initializeDBManager(clientDBManagerMsgQIdReceived, sharedMemoryIdReceived, DBSharedMemoryIdReceived,loggerMsgQIdReceived);
    while(1)
    {
        msgrcv(messageQueueID, &receivedMessage,(sizeof(receivedMessage) - sizeof(receivedMessage.mtype)),DBManagerPID,!IPC_NOWAIT);
        int messageType = receivedMessage.destinationProcess;
        if(messageType == MESSAGE_TYPE_ADD) {
            lastKey = respondToAdd(receivedMessage.name,receivedMessage.salary,lastKey);
            pointersOfWaitingQueuesForRecordKeys[lastKey] = createWaitingQueue();
        }
        else if (messageType == MESSAGE_TYPE_MODIFY) {
            respondToModify(receivedMessage.key, receivedMessage.modification);
        }
        else if (messageType == MESSAGE_TYPE_ACQUIRE) {
            respondToAcquire(receivedMessage.key, receivedMessage.callingProcessID, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
        }
        else if (messageType == MESSAGE_TYPE_RELEASE) {
            respondToRelease(receivedMessage.key, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
        }
        else if (messageType == MESSAGE_TYPE_QUERY) {
            respondToQuery(receivedMessage.queryType, receivedMessage.searchedSalary, receivedMessage.searchedString, receivedMessage.callingProcessID);
        }
    }
}