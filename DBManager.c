#include "DBManager.h"
//----------------------------------------------------------------
int respondToAdd(char *name, int salary, int lastKey)
{
    struct DBrecord newRecord;
    strcpy(newRecord.name, name);
    newRecord.salary = salary;
    newRecord.key = lastKey + 1;
    DBtable[lastKey + 1] = newRecord;
    // printf("New record added to DB with name: %s and Salary: %d with key: %d \n",DBtable[lastKey+1].name,DBtable[lastKey+1].salary,DBtable[lastKey+1].key);
    return lastKey + 1;
}
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue)
{
    int lowerLimit = 0;
    int currentIndex = 0;
    for (; DBtable[currentIndex].key < keyOfTheRecordToBeModified; currentIndex++)
        ;
    if (currentIndex == keyOfTheRecordToBeModified)
    {
        //  printf("New record will be modified from DB with name: %s and Salary: %d with key: %d \n",DBtable[currentIndex].name,DBtable[currentIndex].salary,currentIndex);
        DBtable[currentIndex].salary += modificationValue;
        if (DBtable[currentIndex].salary < lowerLimit)
            DBtable[currentIndex].salary = lowerLimit;
        //  printf("New record modified succ in DB with name: %s and new Salary: %d with key: %d \n",DBtable[currentIndex].name,DBtable[currentIndex].salary,currentIndex);
        //respondToRelease(keyOfTheRecordToBeModified,pointersOfWaitingQueuesForRecordKeys[keyOfTheRecordToBeModified]);
    }
}
int respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue *waitingQueueOfThePassedKey)
{
    int waitingBoolean = -1;
    if (DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED)
    {
        //  printf("Respond Acquire:add to queue DBmanager\n");
        addToWaitingQueue(waitingQueueOfThePassedKey, CallingProccessPID);
        waitingBoolean = ADDED_TO_WAITING_QUEUE;
    }
    else
    {
        DBsemaphores[requiredRecordKey] = SEMAPHORE_OCCUPIED;
        waitingBoolean = GRANTED_ACCESS;
        //kill(CallingProccessPID,SIGUSR1);
        // kill(CallingProccessPID,SIGCONT);
        receivedMessage.mtype = CallingProccessPID;

          printf("Respond Acquire:Semaphore available DBmanager\n");
        msgsnd(messageQueueID, &receivedMessage, sizeof(receivedMessage) - sizeof(receivedMessage.mtype), !IPC_NOWAIT);
    }
    return waitingBoolean;
}
void respondToRelease(int releasedRecordKey, int CallingProccessPID, struct waitingQueue *waitingQueueOfThePassedKey)
{

    //  printf("Semaphore %d is now released\n",releasedRecordKey);
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueOfThePassedKey);
    if (releasedProcessPID != -1)
    {
        // kill(releasedProcessPID,SIGCONT);
        //     printf("process %d returned from the wait\n",releasedProcessPID);
        receivedMessage.mtype = releasedProcessPID;
        msgsnd(messageQueueID, &receivedMessage, sizeof(receivedMessage) - sizeof(receivedMessage.mtype), !IPC_NOWAIT);
    }
    else{

        DBsemaphores[releasedRecordKey] = SEMAPHORE_AVAILABLE;
    }
}
void respondToQuery(int queryType, int searchedSalary, char *searchedName, int callingProcessID)
{
      //  printf("Respond to Query DBmanager: %d\n",queryType);
    if (lastKey + 1 > 0)
    {
        struct message sendSearchResult;
        int notReturnedKey = -1;
        int checkedRecordIndex = 0;
        int returnedKeyIndex = 0;
        memset(sendSearchResult.queryKeys, notReturnedKey, lastKey + 1);
        if (queryType == QUERY_BY_FULL_TABLE)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
              //  printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_EXACT_NAME)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (strcmp(DBtable[checkedRecordIndex].name, searchedName) == 0)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                 //   printf("return keyyyy: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_PART_OF_NAME)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (strstr(DBtable[checkedRecordIndex].name, searchedName) != NULL)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                 //   printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_EXACT_SALARY)
        {

            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (DBtable[checkedRecordIndex].salary == searchedSalary)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                  //  printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_LESS_THAN_SALARY)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (DBtable[checkedRecordIndex].salary < searchedSalary)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                  //  printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_GREATER_THAN_SALARY)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (DBtable[checkedRecordIndex].salary > searchedSalary)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                  //  printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_LESS_THAN_OR_EQUAL_SALARY)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (DBtable[checkedRecordIndex].salary <= searchedSalary)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                 //   printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_GREATER_THAN_OR_EQUAL_SALARY)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (DBtable[checkedRecordIndex].salary >= searchedSalary)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                  //  printf("return key: %d\n",checkedRecordIndex);
            }
        }
        else if (queryType == QUERY_BY_EXACT_NAME_AND_SALARY_EXACT_HYBRID)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (DBtable[checkedRecordIndex].salary == searchedSalary && strcmp(DBtable[checkedRecordIndex].name, searchedName) == 0)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
                   // printf("return key: %d\n",checkedRecordIndex);
            }
        }

        if (returnedKeyIndex > 0)
        {
            sendSearchResult.mtype = callingProcessID;

            //to lock the array
           // for (int i = 0; i < returnedKeyIndex; i++)
              //  printf("returned values of key: %d\n", sendSearchResult.queryKeys[i]);
        }

            sendSearchResult.queryKeys[returnedKeyIndex++] = -1;
            sendSearchResult.mtype=callingProcessID;
            msgsnd(messageQueueID, &sendSearchResult, sizeof(sendSearchResult) - sizeof(sendSearchResult.mtype), !IPC_NOWAIT);
          //  printf("Respond to Query DBmanagerqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa: %d\n",queryType);
    }
}
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived, int LoggerIdReceived)
{
    messageQueueID = messageQueueIdReceived;
    DBManagerPID = getpid();
    sharedMemoryId = sharedMemoryIdReceived;
    memset(DBsemaphores, SEMAPHORE_AVAILABLE, MAX_NUMBER_OF_RECORDS);
    DBtable = (struct DBrecord *)shmat(DBSharedMemoryIdReceived, (void *)0, 0);
    loggerMsgQIdDBManager = loggerMsgQIdReceived;
    signal(SIGUSR1, handlingSIGUSR1_and_IgnoringSigStop);

    DBManagerLogger = (struct Log *)shmat(sharedMemoryId, (void *)0, 0);
    LoggerId = LoggerIdReceived;
}
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived, int LoggerIdReceived)
{
    initializeDBManager(clientDBManagerMsgQIdReceived, sharedMemoryIdReceived, DBSharedMemoryIdReceived, loggerMsgQIdReceived, LoggerIdReceived);
    while (1)
    {
        receivedMessage.destinationProcess=-1;
        receivedMessage.queryType=-1;
        int rec_val=-1;
      //  while (rec_val==-1)
        {
            rec_val= msgrcv(messageQueueID, &receivedMessage, (sizeof(receivedMessage) - sizeof(receivedMessage.mtype)), getpid(), !IPC_NOWAIT);

        }
           // printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy %d\n",receivedMessage.destinationProcess);
        int messageType = receivedMessage.destinationProcess;
        int notAvailableForThisRequest = -1;
        char *noAvailableNameForThisRequest = NULL;
        if (messageType == MESSAGE_TYPE_ADD)
        {
            lastKey = respondToAdd(receivedMessage.name, receivedMessage.salary, lastKey);
            pointersOfWaitingQueuesForRecordKeys[lastKey] = createWaitingQueue();
            //printf("DBmanager added %s\n", receivedMessage.name);
            //logDBManagerAdd(receivedMessage.name,receivedMessage.salary);
            logDBManagerAdd(messageType, receivedMessage.name, receivedMessage.salary, notAvailableForThisRequest, notAvailableForThisRequest, notAvailableForThisRequest, notAvailableForThisRequest, notAvailableForThisRequest);
        }
        else if (messageType == MESSAGE_TYPE_MODIFY)
        {
            respondToModify(receivedMessage.key, receivedMessage.modification);
            logDBManagerModify(messageType, noAvailableNameForThisRequest, notAvailableForThisRequest, receivedMessage.callingProcessID, notAvailableForThisRequest, receivedMessage.key, receivedMessage.modification, notAvailableForThisRequest);
        }
        else if (messageType == MESSAGE_TYPE_ACQUIRE)
        {
            int waitBoolean = respondToAcquire(receivedMessage.key, receivedMessage.callingProcessID, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
             logDBManagerAcquire(messageType, noAvailableNameForThisRequest, notAvailableForThisRequest, receivedMessage.callingProcessID, waitBoolean, receivedMessage.key, notAvailableForThisRequest, notAvailableForThisRequest);
        }
        else if (messageType == MESSAGE_TYPE_RELEASE)
        {
            respondToRelease(receivedMessage.key, receivedMessage.callingProcessID, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
            logDBManagerRelease(messageType, noAvailableNameForThisRequest, notAvailableForThisRequest, notAvailableForThisRequest, notAvailableForThisRequest, receivedMessage.key, notAvailableForThisRequest, notAvailableForThisRequest);
        }
        else if (messageType == MESSAGE_TYPE_QUERY)
        {
            respondToQuery(receivedMessage.queryType, receivedMessage.searchedSalary, receivedMessage.searchedString, receivedMessage.callingProcessID);
            logDBManagerQuery(messageType, receivedMessage.searchedString, receivedMessage.searchedSalary, notAvailableForThisRequest, notAvailableForThisRequest, notAvailableForThisRequest, notAvailableForThisRequest, receivedMessage.queryType);
        }
    }
}

void logDBManagerAdd(int requestedType, char *name, int salary, int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType)
{
    int send_val;
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    //  printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq  %d____ %d\n",loggerMsgQIdDBManager,LoggerId);
    if (send_val != -1)
    {
    }

    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
    int rec= msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);


    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char messageForSharedMemory[MAXCHAR] = "I am DBManager I added ";
    strcat(messageForSharedMemory, name);

    char salaryString[MAXCHAR];
    sprintf(salaryString, "%d", salary);

    strcat(messageForSharedMemory, " with salary ");
    strcat(messageForSharedMemory, salaryString);
    strcpy(DBManagerLogger->message, messageForSharedMemory);
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_RELEASE;
    messageLoggerDBManager.mtype = LoggerId;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}
void logDBManagerModify(int requestedType, char *name, int salary, int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType)
{
    int send_val;
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    //  printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq  %d____ %d\n",loggerMsgQIdDBManager,LoggerId);
    if (send_val != -1)
    {
    }

    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
    int rec= msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);


    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char modification[MAXCHAR];
    sprintf(modification, "%d", modificationValue);
    char modifiedRecordKey[4];
    sprintf(modifiedRecordKey, "%d", recordKey);
    char messageForSharedMemory[MAXCHAR] = "I am DBManager I modified record of key: ";
    strcat(messageForSharedMemory, modifiedRecordKey);
    strcat(messageForSharedMemory, " with a value of: ");
    strcat(messageForSharedMemory, modification);
    strcpy(DBManagerLogger->message, messageForSharedMemory);
    //printf("%s\n", messageForSharedMemory);
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_RELEASE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}
void logDBManagerAcquire(int requestedType, char *name, int salary, int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType)
{
    int send_val;
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    //  printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq  %d____ %d\n",loggerMsgQIdDBManager,LoggerId);
    if (send_val != -1)
    {
    }

    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
    int rec= msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);


    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char modification[MAXCHAR];
    sprintf(modification, "%d", modificationValue);
    char modifiedRecordKey[4];
    sprintf(modifiedRecordKey, "%d", recordKey);
    char messageForSharedMemory[MAXCHAR] = "I am DBManager I modified record of key: ";
    strcat(messageForSharedMemory, modifiedRecordKey);
    strcat(messageForSharedMemory, " with a value of: ");
    strcat(messageForSharedMemory, modification);
    strcpy(DBManagerLogger->message, messageForSharedMemory);
    //printf("%s\n", messageForSharedMemory);
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_RELEASE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}
void logDBManagerRelease(int requestedType, char *name, int salary, int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType)
{
    int send_val;
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    //  printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq  %d____ %d\n",loggerMsgQIdDBManager,LoggerId);
    if (send_val != -1)
    {
    }

    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
    int rec= msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);


    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char keyOfSemaphore[4];
    sprintf(keyOfSemaphore, "%d", recordKey);
    char messageForSharedMemory[MAXCHAR] = "I am DBManager I released record of key: ";
    strcat(messageForSharedMemory, keyOfSemaphore);
    strcpy(DBManagerLogger->message, messageForSharedMemory);
   // printf("%s\n", messageForSharedMemory);
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_RELEASE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}
void logDBManagerQuery(int requestedType, char *name, int salary, int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType)
{
    int send_val;
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    if (send_val != -1)
    {
    }

   /* raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
    int rec= msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);


    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char messageForSharedMemory[MAXCHAR] = "I am DBManager I returned a query by ";
    if (queryType == QUERY_BY_FULL_TABLE)
    {
        strcat(messageForSharedMemory, "full table");
    }
    else if (queryType == QUERY_BY_EXACT_NAME)
    {
       // printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq  %s____ %d\n","Exact name",LoggerId);
        strcat(messageForSharedMemory, "exact name: ");
        strcat(messageForSharedMemory, name);
    }
    else if (queryType == QUERY_BY_PART_OF_NAME)
    {
   // printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq  %s____ %d\n","Part of name",LoggerId);
        strcat(messageForSharedMemory, "part of name: ");
        strcat(messageForSharedMemory, name);
    }
    else if (queryType == QUERY_BY_EXACT_SALARY)
    {
        strcat(messageForSharedMemory, "exact salary: ");
        char salaryString[MAXCHAR];
        sprintf(salaryString, "%d", salary);
        strcat(messageForSharedMemory, salaryString);
    }
    else if (queryType == QUERY_BY_EXACT_NAME_AND_SALARY_EXACT_HYBRID)
    {
        strcat(messageForSharedMemory, "exact name: ");
        strcat(messageForSharedMemory, name);
        strcat(messageForSharedMemory, " and exact salary: ");
        char salaryString[MAXCHAR];
        sprintf(salaryString, "%d", salary);
        strcat(messageForSharedMemory, salaryString);
    }
    else if (queryType == QUERY_BY_GREATER_THAN_SALARY)
    {
        strcat(messageForSharedMemory, "greater than salary: ");
        char salaryString[MAXCHAR];
        sprintf(salaryString, "%d", salary);
        strcat(messageForSharedMemory, salaryString);
    }
    else if (queryType == QUERY_BY_GREATER_THAN_OR_EQUAL_SALARY)
    {
        strcat(messageForSharedMemory, "greater than or equal salary: ");
        char salaryString[MAXCHAR];
        sprintf(salaryString, "%d", salary);
        strcat(messageForSharedMemory, salaryString);
    }
    else if (queryType == QUERY_BY_LESS_THAN_SALARY)
    {
        strcat(messageForSharedMemory, "less than salary: ");
        char salaryString[MAXCHAR];
        sprintf(salaryString, "%d", salary);
        strcat(messageForSharedMemory, salaryString);
    }
    else if (queryType == QUERY_BY_LESS_THAN_OR_EQUAL_SALARY)
    {
        strcat(messageForSharedMemory, "less than or equal salary: ");
        char salaryString[MAXCHAR];
        sprintf(salaryString, "%d", salary);
        strcat(messageForSharedMemory, salaryString);
    }
    strcpy(DBManagerLogger->message, messageForSharedMemory);
    //printf("%s\n", messageForSharedMemory);
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_RELEASE;
    send_val = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}

/*void logDBManager(int requestedType, char* name, int salary,int callingProcessPID, int waitBoolean, int recordKey, int modificationValue, int queryType)
{
    //printf("logDBmanager\n");
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    int checkSend = msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    if(checkSend!=-1)
    {
        //printf("logDB 0\n");
        raise(SIGTSTP);
        signal(SIGTSTP, SIG_DFL);
        //printf("logDB 1\n");
        char DBManagerNumberString[5];
        sprintf(DBManagerNumberString, "%d", 0);
        strcpy(DBManagerLogger->number, DBManagerNumberString);
        //printf("logDB 2\n");
        if(requestedType == MESSAGE_TYPE_ADD)
        {
            //printf("DBmanager logged add\n");
            char messageForSharedMemory[MAXCHAR] = "I am DBManager I added ";
            strcat(messageForSharedMemory, name);

            char salaryString[MAXCHAR];
            sprintf(salaryString, "%d", salary);

            strcat(messageForSharedMemory, " with salary ");
            strcat(messageForSharedMemory, salaryString);
            strcpy(DBManagerLogger->message, messageForSharedMemory);
            printf("%s\n",messageForSharedMemory);
        }
        else if(requestedType == MESSAGE_TYPE_ACQUIRE)
        {
            //printf("DBmanager logged acquire\n");
            char requestingProcessPID [MAXCHAR];
            sprintf(requestingProcessPID, "%d", callingProcessPID);
            char keyOfSemaphore [4];
            sprintf(keyOfSemaphore, "%d", recordKey);
            if(waitBoolean == ADDED_TO_WAITING_QUEUE) {
                
                char messageForSharedMemory[MAXCHAR] = "I am DBManager I added ";
                strcat(messageForSharedMemory, requestingProcessPID);
                strcat(messageForSharedMemory, " to the waiting queue of key: ");
                strcat(messageForSharedMemory, keyOfSemaphore);
                strcpy(DBManagerLogger->message, messageForSharedMemory);
                printf("%s\n",messageForSharedMemory);
            }
            else if (waitBoolean == GRANTED_ACCESS) {
                char messageForSharedMemory[MAXCHAR] = "I am DBManager I granted access to ";
                strcat(messageForSharedMemory, requestingProcessPID);
                strcat(messageForSharedMemory, " to record of key: ");
                strcat(messageForSharedMemory, keyOfSemaphore);
                strcpy(DBManagerLogger->message, messageForSharedMemory);
                printf("%s\n",messageForSharedMemory);
            }
        }
        else if(requestedType == MESSAGE_TYPE_MODIFY)
        {
            //printf("DBmanager logged modify\n");
            char modification [MAXCHAR];
            sprintf(modification, "%d", modificationValue);
            char modifiedRecordKey [4];
            sprintf(modifiedRecordKey, "%d", recordKey);
            char messageForSharedMemory[MAXCHAR] = "I am DBManager I modified record of key: ";
            strcat(messageForSharedMemory, modifiedRecordKey);
            strcat(messageForSharedMemory," with a value of: ");
            strcat(messageForSharedMemory, modification);
            strcpy(DBManagerLogger->message, messageForSharedMemory);
            printf("%s\n",messageForSharedMemory);
        }
        else if(requestedType == MESSAGE_TYPE_RELEASE) {
            //printf("DBmanager logged release\n");
            char keyOfSemaphore [4];
            sprintf(keyOfSemaphore, "%d", recordKey);
            char messageForSharedMemory[MAXCHAR] = "I am DBManager I released record of key: ";
            strcat(messageForSharedMemory, keyOfSemaphore);
            strcpy(DBManagerLogger->message, messageForSharedMemory);
            printf("%s\n",messageForSharedMemory);
        }
        else if(requestedType == MESSAGE_TYPE_QUERY) {
            //printf("DBmanager logged query\n");
            char messageForSharedMemory[MAXCHAR] = "I am DBManager I returned a query by ";
            if(queryType == QUERY_BY_FULL_TABLE)
            {
                strcat(messageForSharedMemory,"full table");
            }
            else if(queryType == QUERY_BY_EXACT_NAME)
            {
                strcat(messageForSharedMemory,"exact name: ");
                strcat(messageForSharedMemory,name);
            }
            else if(queryType == QUERY_BY_PART_OF_NAME)
            {
                strcat(messageForSharedMemory,"part of name: ");
                strcat(messageForSharedMemory,name);
            }
            else if(queryType == QUERY_BY_EXACT_SALARY)
            {
                strcat(messageForSharedMemory,"exact salary: ");
                char salaryString[MAXCHAR];
                sprintf(salaryString, "%d", salary);
                strcat(messageForSharedMemory,salaryString);
            }
            else if(queryType == QUERY_BY_EXACT_NAME_AND_SALARY_EXACT_HYBRID)
            {
                strcat(messageForSharedMemory,"exact name: ");
                strcat(messageForSharedMemory,name);
                strcat(messageForSharedMemory," and exact salary: ");
                char salaryString[MAXCHAR];
                sprintf(salaryString, "%d", salary);
                strcat(messageForSharedMemory,salaryString);
            }
            else if(queryType == QUERY_BY_GREATER_THAN_SALARY)
            {
                strcat(messageForSharedMemory,"greater than salary: ");
                char salaryString[MAXCHAR];
                sprintf(salaryString, "%d", salary);
                strcat(messageForSharedMemory,salaryString);
            }
            else if(queryType == QUERY_BY_GREATER_THAN_OR_EQUAL_SALARY)
            {
                strcat(messageForSharedMemory,"greater than or equal salary: ");
                char salaryString[MAXCHAR];
                sprintf(salaryString, "%d", salary);
                strcat(messageForSharedMemory,salaryString);
            }
            else if(queryType == QUERY_BY_LESS_THAN_SALARY)
            {
                strcat(messageForSharedMemory,"less than salary: ");
                char salaryString[MAXCHAR];
                sprintf(salaryString, "%d", salary);
                strcat(messageForSharedMemory,salaryString);
            }
            else if(queryType == QUERY_BY_LESS_THAN_OR_EQUAL_SALARY)
            {
                strcat(messageForSharedMemory,"less than or equal salary: ");
                char salaryString[MAXCHAR];
                sprintf(salaryString, "%d", salary);
                strcat(messageForSharedMemory,salaryString);
            }
            strcpy(DBManagerLogger->message, messageForSharedMemory);
            printf("%s\n",messageForSharedMemory);
        }
        messageLoggerDBManager.destinationProcess = MESSAGE_TYPE_RELEASE;
        msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
    }
    
}*/
