#include "DBManager.h"
//----------------------------------------------------------------
void do_DBManager(int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int DBSharedMemoryIdReceived, int receivedLoggerMsgQId, int receivedLoggerId)
{
    initializeDBManager(clientDBManagerMsgQIdReceived, sharedMemoryIdReceived, DBSharedMemoryIdReceived, receivedLoggerMsgQId, receivedLoggerId);
    while (TRUE_)
    {
        printf("DB manager is still alive \n");
        receivedMessage.requestedAction = -1;
        receivedMessage.queryType = -1;
        msgrcv(messageQueueID, &receivedMessage, (sizeof(receivedMessage) - sizeof(receivedMessage.mtype)), getpid(), !IPC_NOWAIT);
        int messageType = receivedMessage.requestedAction;
        if (messageType == MESSAGE_TYPE_ADD)
        {
            lastKey = respondToAdd(receivedMessage.name, receivedMessage.salary, lastKey);
            pointersOfWaitingQueuesForRecordKeys[lastKey] = createWaitingQueue();
            logDBManagerAdd(receivedMessage.name, receivedMessage.salary);
        }
        else if (messageType == MESSAGE_TYPE_MODIFY)
        {
            respondToModify(receivedMessage.key, receivedMessage.modification);
            logDBManagerModify(receivedMessage.callingProcessID, receivedMessage.key, receivedMessage.modification);
        }
        else if (messageType == MESSAGE_TYPE_ACQUIRE)
        {
            int callingProcessWaitingBoolean = respondToAcquire(receivedMessage.key, receivedMessage.callingProcessID, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
            logDBManagerAcquire(receivedMessage.callingProcessID, callingProcessWaitingBoolean, receivedMessage.key);
        }
        else if (messageType == MESSAGE_TYPE_RELEASE)
        {
            int releasedProcess = respondToRelease(receivedMessage.key, receivedMessage.callingProcessID, pointersOfWaitingQueuesForRecordKeys[receivedMessage.key]);
            logDBManagerRelease(receivedMessage.key, releasedProcess);
        }
        else if (messageType == MESSAGE_TYPE_QUERY)
        {
            respondToQuery(receivedMessage.queryType, receivedMessage.searchedSalary, receivedMessage.searchedString, receivedMessage.callingProcessID);
            logDBManagerQuery(receivedMessage.searchedString, receivedMessage.searchedSalary, receivedMessage.queryType);
        }
    }
}
int respondToAdd(char *name, int salary, int lastKey)
{
    struct DBrecord newRecord;
    strcpy(newRecord.name, name);
    newRecord.salary = salary;
    newRecord.key = lastKey + 1;
    DBtable[lastKey + 1] = newRecord;
    return lastKey + 1;
}
void respondToModify(int keyOfTheRecordToBeModified, int modificationValue)
{
    int currentIndex = 0;
    for (; DBtable[currentIndex].key < keyOfTheRecordToBeModified; currentIndex++)
    {
        /*checks that the index to be modified exists*/
    }
    if (currentIndex == keyOfTheRecordToBeModified)
    {
        DBtable[currentIndex].salary += modificationValue;
        if (DBtable[currentIndex].salary < SALARY_LOWER_LIMIT)
        {
            DBtable[currentIndex].salary = SALARY_LOWER_LIMIT;
        }
    }
}
int respondToAcquire(int requiredRecordKey, int CallingProccessPID, struct waitingQueue *waitingQueueOfThePassedKey)
{
    struct message messageSentToClient;
    messageSentToClient.mtype = CallingProccessPID;
    int callingProcessWaitingBoolean;
    if (DBsemaphores[requiredRecordKey] == SEMAPHORE_OCCUPIED)
    {
        callingProcessWaitingBoolean = ADDED_TO_WAITING_QUEUE;
        addToWaitingQueue(waitingQueueOfThePassedKey, CallingProccessPID);
    }
    else
    {
        DBsemaphores[requiredRecordKey] = SEMAPHORE_OCCUPIED;
        callingProcessWaitingBoolean = GRANTED_ACCESS;
        kill(CallingProccessPID, SIGUSR1);
        kill(CallingProccessPID, SIGCONT);
    }
    return callingProcessWaitingBoolean;
}
int respondToRelease(int releasedRecordKey, int CallingProccessPID, struct waitingQueue *waitingQueueOfThePassedKey)
{
    int releasedProcessPID = removeFromWaitingQueue(waitingQueueOfThePassedKey);
    if (releasedProcessPID != NO_PROCESS_WAITING)
    {
        kill(releasedProcessPID, SIGCONT);
    }
    else
    {
        DBsemaphores[releasedRecordKey] = SEMAPHORE_AVAILABLE;
    }
    return releasedProcessPID;
}

void respondToQuery(int queryType, int searchedSalary, char *searchedName, int callingProcessID)
{
    if (lastKey + 1 > NO_ELEMENTS)
    {
        struct message sendSearchResult;
        int checkedRecordIndex = KEY_START;
        int returnedKeyIndex = KEY_START;
        memset(sendSearchResult.queryKeys, NOT_RETURNED, lastKey + 1);
        if (queryType == QUERY_BY_FULL_TABLE)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
            }
        }
        else if (queryType == QUERY_BY_EXACT_NAME)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if (strcmp(DBtable[checkedRecordIndex].name, searchedName) == IDENTICAL_NAMES)
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
            }
        }
        else if (queryType == QUERY_BY_PART_OF_NAME)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                int characterIndex = FIRST_CHARACTER;
                while (DBtable[checkedRecordIndex].name[characterIndex] == searchedName[characterIndex])
                {
                    characterIndex++;
                }
                if (characterIndex != NOT_FOUND && characterIndex == strlen(searchedName))
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
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
            }
        }
        else if (queryType == QUERY_BY_EXACT_NAME_AND_SALARY_EXACT_HYBRID)
        {
            for (; checkedRecordIndex <= lastKey; checkedRecordIndex++)
            {
                if ((DBtable[checkedRecordIndex].salary == searchedSalary) && (strcmp(DBtable[checkedRecordIndex].name, searchedName) == IDENTICAL_NAMES))
                {
                    sendSearchResult.queryKeys[returnedKeyIndex++] = checkedRecordIndex;
                }
            }
        }
        sendSearchResult.mtype = callingProcessID;
        sendSearchResult.queryKeys[returnedKeyIndex++] = NOT_RETURNED;
        sendSearchResult.mtype = callingProcessID;
        msgsnd(messageQueueID, &sendSearchResult, sizeof(sendSearchResult) - sizeof(sendSearchResult.mtype), !IPC_NOWAIT);
    }
}
void initializeDBManager(int messageQueueIdReceived, int sharedMemoryIdReceived, int DBSharedMemoryIdReceived, int receivedLoggerMsgQId, int receivedLoggerId)
{
    messageQueueID = messageQueueIdReceived;
    DBManagerPID = getpid();
    sharedMemoryId = sharedMemoryIdReceived;
    memset(DBsemaphores, SEMAPHORE_AVAILABLE, MAX_NUMBER_OF_RECORDS);
    DBtable = (struct DBrecord *)shmat(DBSharedMemoryIdReceived, (void *)0, 0);
    loggerMsgQIdDBManager = receivedLoggerMsgQId;
    signal(SIGUSR1, handlingSIGUSR1_and_IgnoringSigStop);
    DBManagerLogger = (struct Log *)shmat(sharedMemoryId, (void *)0, 0);
    LoggerId = receivedLoggerId;
}

void logDBManagerAdd(char *name, int salary)
{
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_ACQUIRE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);

    msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);

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
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_RELEASE;
    messageLoggerDBManager.mtype = LoggerId;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}
void logDBManagerModify(int callingProcessPID, int recordKey, int modificationValue)
{
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_ACQUIRE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);

    msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);

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

    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_RELEASE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}

void logDBManagerAcquire(int callingProcessPID, int callingProcessWaitingBoolean, int recordKey)
{
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_ACQUIRE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);

    msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);

    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char requestingProcessPID[MAXCHAR];
    sprintf(requestingProcessPID, "%d", callingProcessPID);
    char keyOfSemaphore[4];
    sprintf(keyOfSemaphore, "%d", recordKey);
    if (callingProcessWaitingBoolean == ADDED_TO_WAITING_QUEUE)
    {

        char messageForSharedMemory[MAXCHAR] = "I am DBManager I added ";
        strcat(messageForSharedMemory, requestingProcessPID);
        strcat(messageForSharedMemory, " to the waiting queue of key: ");
        strcat(messageForSharedMemory, keyOfSemaphore);
        strcpy(DBManagerLogger->message, messageForSharedMemory);
    }
    else if (callingProcessWaitingBoolean == GRANTED_ACCESS)
    {
        char messageForSharedMemory[MAXCHAR] = "I am DBManager I granted access to ";
        strcat(messageForSharedMemory, requestingProcessPID);
        strcat(messageForSharedMemory, " to record of key: ");
        strcat(messageForSharedMemory, keyOfSemaphore);
        strcpy(DBManagerLogger->message, messageForSharedMemory);
    }
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_RELEASE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}

void logDBManagerRelease(int recordKey, int releasedProcess)
{

    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_ACQUIRE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);

    msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);

    char DBManagerNumberString[5];
    sprintf(DBManagerNumberString, "%d", 0);
    strcpy(DBManagerLogger->number, DBManagerNumberString);

    char keyOfSemaphore[4];
    sprintf(keyOfSemaphore, "%d", recordKey);
    char messageForSharedMemory[MAXCHAR] = "I am DBManager I released record of key: ";
    strcat(messageForSharedMemory, keyOfSemaphore);
    if (releasedProcess != NO_PROCESS_WAITING)
    {
        char releasedProcessID[MAXCHAR];
        sprintf(releasedProcessID, "%d", releasedProcess);
        strcat(messageForSharedMemory, " and process ");
        strcat(messageForSharedMemory, releasedProcessID);
        strcat(messageForSharedMemory, " is released from waiting queue");
    }
    else
    {
        strcat(messageForSharedMemory, " and waiting queue of this key is now empty");
    }
    strcpy(DBManagerLogger->message, messageForSharedMemory);

    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_RELEASE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}
void logDBManagerQuery(char *name, int salary, int queryType)
{

    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.PID = getpid();
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_ACQUIRE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);

    msgrcv(loggerMsgQIdDBManager, &messageLoggerDBManager, (sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype)), getpid(), !IPC_NOWAIT);

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
        strcat(messageForSharedMemory, "exact name: ");
        strcat(messageForSharedMemory, name);
    }
    else if (queryType == QUERY_BY_PART_OF_NAME)
    {
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
    messageLoggerDBManager.mtype = LoggerId;
    messageLoggerDBManager.requestedAction = MESSAGE_TYPE_RELEASE;
    msgsnd(loggerMsgQIdDBManager, &messageLoggerDBManager, sizeof(messageLoggerDBManager) - sizeof(messageLoggerDBManager.mtype), !IPC_NOWAIT);
}