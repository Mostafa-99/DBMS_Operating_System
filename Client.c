#include "Client.h"

void do_client(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived, int LoggerIdReceived, int queryLoggerMsgQIdReceived)
{

    initializeClient(DBManagerIdReceived, QueryLoggerIdReceived, sharedMemoryIdReceived, clientDBManagerMsgQIdReceived, clientNumberReceived, DBSharedMemoryIdReceived, loggerMsgQIdReceived, LoggerIdReceived,queryLoggerMsgQIdReceived);
    openConfigurationFile();
    char *readWordFromFile = "1";
    while (readWordFromFile != NULL)
    {
        readWordFromFile = readConfigurationFile();
        if (readWordFromFile == NULL)
        {
            break;
        }

        if (strcmp(readWordFromFile, "add") == 0)
        {
            char name[MAXCHAR];
            strcpy(name, readConfigurationFile());
            char salary[MAXCHAR];
            strcpy(salary, readConfigurationFile());
            requestToAdd(name, atoi(salary));
        }
        else if (strcmp(readWordFromFile, "modify") == 0)
        {
            char key[MAXCHAR];
            strcpy(key, readConfigurationFile());
            char modification[MAXCHAR];
            strcpy(modification, readConfigurationFile());
            int sign = strcmp(modification, "+") == 0 ? ADD_MODIFICATION : SUBTRACT_MODIFICATION;
            char value[MAXCHAR];
            strcpy(value, readConfigurationFile());
            requestToModify(atoi(key), sign, atoi(value));
        }
        else if (strcmp(readWordFromFile, "acquire") == 0)
        {
            char key[MAXCHAR];
            strcpy(key, readConfigurationFile());
            requestToAcquire(atoi(key));
        }
        else if (strcmp(readWordFromFile, "release") == 0)
        {
            char key[MAXCHAR];
            strcpy(key, readConfigurationFile());
            requestToRelease(atoi(key));
        }
        else if (strcmp(readWordFromFile, "query") == 0)
        {
            int queryType, searchedSalary;
            char *searchedString;
            getQueryRequestParameters(&queryType, &searchedSalary, &searchedString);
           /* printf("Client requesting query %d\n",queryType);
            printf("%d \n",searchedSalary);
            printf("%s \n",searchedString);
            printf("<-- searchedString sent by client\n");*/
            requestToQuery(queryType, searchedSalary, &searchedString);
           // int rec= msgrcv(clientDBManagerMsgQId, &messageClient, (sizeof(messageClient) - sizeof(messageClient.mtype)), getpid(), !IPC_NOWAIT);
          /*  if(rec==-1)
            {

            }*/
        }
        sleep(2);
    }
    fclose(fp);
    exit(1);
}
void getQueryRequestParameters(int *queryType, int *searchedSalary, char **searchedString)
{

    char *queryNameType = readConfigurationFile();
    if (strcmp(queryNameType, "full") == 0)
    {
        *queryType = QUERY_BY_FULL_TABLE;
        *searchedString = NULL;
        *searchedSalary = 0;
    }
    else if (strcmp(queryNameType, "name") == 0)
    {
        queryNameType = readConfigurationFile();
        if (strcmp(queryNameType, "startswith") == 0)
        {
            *queryType = QUERY_BY_PART_OF_NAME;
            *searchedString = readConfigurationFile();
        }
        else if (strcmp(queryNameType, "exact") == 0)
        {
            *queryType = QUERY_BY_EXACT_NAME;
            *searchedString = readConfigurationFile();
        }
        *searchedSalary = 0;
    }
    else if (strcmp(queryNameType, "salary") == 0)
    {
        queryNameType = readConfigurationFile();
        if (strcmp(queryNameType, "exact") == 0)
        {
            *queryType = QUERY_BY_EXACT_SALARY;
        }
        else if (strcmp(queryNameType, ">") == 0)
        {
            *queryType = QUERY_BY_GREATER_THAN_SALARY;
        }
        else if (strcmp(queryNameType, "<") == 0)
        {
            *queryType = QUERY_BY_LESS_THAN_SALARY;
        }
        else if (strcmp(queryNameType, ">=") == 0)
        {
            *queryType = QUERY_BY_GREATER_THAN_OR_EQUAL_SALARY;
        }
        else if (strcmp(queryNameType, "<=") == 0)
        {
            *queryType = QUERY_BY_LESS_THAN_OR_EQUAL_SALARY;
        }

        *searchedSalary = atoi(readConfigurationFile());
    }
    else if (strcmp(queryNameType, "hybrid") == 0)
    {
        *queryType = QUERY_BY_EXACT_NAME_AND_SALARY_EXACT_HYBRID;
        *searchedString = readConfigurationFile();
        *searchedSalary = atoi(readConfigurationFile());
    }
}
void initializeClient(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived, int DBSharedMemoryIdReceived, int loggerMsgQIdReceived, int LoggerIdReceived,int queryLoggerMsgQIdReceived)
{
    DBManagerId = DBManagerIdReceived;
    QueryLoggerId = QueryLoggerIdReceived;
    sharedMemoryId = sharedMemoryIdReceived;
    clientDBManagerMsgQId = clientDBManagerMsgQIdReceived;
    clientNumber = clientNumberReceived;
    messageClient.mtype = DBManagerId;
    signal(SIGUSR1, handlingSIGUSR1_and_IgnoringSigStop);
    DBtableView = (struct DBrecord *)shmat(DBSharedMemoryIdReceived, (void *)0, SHM_RDONLY);
    loggerMsgQIdClient = loggerMsgQIdReceived;
    clientLogger = (struct Log *)shmat(sharedMemoryId, (void *)0, 0);
    LoggerId = LoggerIdReceived;
    queryLoggerMsgQIdClient=queryLoggerMsgQIdReceived;
}
void openConfigurationFile()
{
    char buffer[50];
    sprintf(buffer, "%d", clientNumber);
    filename = strcat(buffer, ".txt");
    fp = fopen(filename, "r");
}
char *readConfigurationFile()
{
    //read config
    if (fp == NULL)
    {
        printf("Could not open file %s\n", filename);
        return 0;
    }

    if (fscanf(fp, "%s", word) != EOF)
    {
        return word;
    }
    else
    {
        return NULL;
    }
}
void requestToAdd(char *name, int salary)
{
    messageClient.destinationProcess = MESSAGE_TYPE_ADD;
    strcpy(messageClient.name, name);
    messageClient.salary = salary;
    messageClient.callingProcessID = getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient) - sizeof(messageClient.mtype), !IPC_NOWAIT);
    if (send_val == -1)
    {
        perror("error in send msg");
    }
    else
    {
        logAdd(name, salary);
    }
}
void logAdd(char *name, int salary)
{
   printf("I am child: %d and I am client number: %d, I require to add %s with salary %d _________________________%d______%d\n", getpid(), clientNumber, name, salary,loggerMsgQIdClient,LoggerId);
    messageLoggerClient.mtype = LoggerId;
    messageLoggerClient.PID = getpid();
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);
    
    
    raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);

    char clientNumberString[5];
    sprintf(clientNumberString, "%d", clientNumber);
    strcpy(clientLogger->number, clientNumberString);

    char msgArray[MAXCHAR] = "I require to add ";
    strcat(msgArray, name);

    char salaryString[MAXCHAR];
    sprintf(salaryString, "%d", salary);

    strcat(msgArray, " with salary ");
    strcat(msgArray, salaryString);
    strcpy(clientLogger->message, msgArray);
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_RELEASE;
    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);
    
    
    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
}
void requestToModify(int key, int modification, int value)
{
    messageClient.destinationProcess = MESSAGE_TYPE_MODIFY;
    messageClient.key = key;
    messageClient.modification = value * modification;
    messageClient.callingProcessID = getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient) - sizeof(messageClient.mtype), !IPC_NOWAIT);
    if (send_val == -1)
    {
        perror("error in send msg");
    }
    else
    {
  //  printf("I am child: %d and I am client number: %d, I require to modify key: %d, modification: %d, with value %d \n", getpid(), clientNumber, key, modification, value);
        logModify(key, modification, value);
    }
}
void logModify(int key, int modification, int value)
{
    messageLoggerClient.mtype = LoggerId;
    messageLoggerClient.PID = getpid();
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
    raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);

    char clientNumberString[5];
    sprintf(clientNumberString, "%d", clientNumber);
    strcpy(clientLogger->number, clientNumberString);
    char msgArray[MAXCHAR] = "I require to modify key: ";
    char keyString[MAXCHAR];
    sprintf(keyString, "%d", key);
    strcat(msgArray, keyString);

    char valueString[MAXCHAR];
    sprintf(valueString, "%d", modification * value);

    strcat(msgArray, " with value ");
    strcat(msgArray, valueString);
    strcpy(clientLogger->message, msgArray);
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_RELEASE;

    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
   /* raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
}
void requestToAcquire(int key)
{
    messageClient.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    messageClient.key = key;
    messageClient.callingProcessID = getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient) - sizeof(messageClient.mtype), !IPC_NOWAIT);
    if (send_val == -1)
    {
        perror("error in send msg");
    }
    else
    {
        //raise(SIGTSTP);
        //signal(SIGTSTP, SIG_DFL);
        struct message messageClient2;

        int rec= msgrcv(clientDBManagerMsgQId, &messageClient2, (sizeof(messageClient2) - sizeof(messageClient2.mtype)), getpid(), !IPC_NOWAIT);

        if(rec!=-1)
        {
      //  printf("Acquire doneeeeeeeeeeeeeeeeeeeeeeee \n");
            logAcquire(key);
        }
    }
}
void logAcquire(int key)
{
    messageLoggerClient.mtype = LoggerId;
    messageLoggerClient.PID = getpid();
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
    raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);

    char clientNumberString[5];
    sprintf(clientNumberString, "%d", clientNumber);
    strcpy(clientLogger->number, clientNumberString);
    char msgArray[MAXCHAR] = "I acquire key: ";
    char keyString[MAXCHAR];
    sprintf(keyString, "%d", key);
    strcat(msgArray, keyString);

    strcpy(clientLogger->message, msgArray);
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_RELEASE;

    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
}
void requestToRelease(int key)
{
    //printf("I am child: %d and I am client number: %d, I release key %d kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk\n", getpid(), &messageClient, clientDBManagerMsgQId);
    messageClient.destinationProcess = MESSAGE_TYPE_RELEASE;
    messageClient.key = key;
    messageClient.callingProcessID = getpid();
    messageClient.mtype=DBManagerId;
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient) - sizeof(messageClient.mtype), !IPC_NOWAIT);
    if (send_val == -1)
    {
        perror("error in send msg");
    }
    else
    {
   // printf("I am child: %d and I am client number: %d, I release key %d llllllllllllllllllllllllllllllllllllllllll\n", getpid(), clientNumber, send_val);
        logRelease(key);
    }
}
void logRelease(int key)
{
    messageLoggerClient.mtype = LoggerId;
    messageLoggerClient.PID = getpid();
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
    raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);

    char clientNumberString[5];
    sprintf(clientNumberString, "%d", clientNumber);
    strcpy(clientLogger->number, clientNumberString);
    char msgArray[MAXCHAR] = "I release key: ";
    char keyString[MAXCHAR];
    sprintf(keyString, "%d", key);
    strcat(msgArray, keyString);

    strcpy(clientLogger->message, msgArray);
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_RELEASE;

    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
   // printf("I am child: %d and I am client number: %d, I release key %d kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk\n", getpid(), &messageClient, clientDBManagerMsgQId);

}
void requestToQuery(int queryType, int searchedSalary, char **searchedString)
{
    messageClient.destinationProcess = MESSAGE_TYPE_QUERY;
    messageClient.queryType = queryType;
    messageClient.searchedSalary = searchedSalary;
    printf("I am the client I request a query with type %d and salary %d searching for: %s\n",queryType, searchedSalary, *searchedString);
   if(*searchedString!=NULL)
   {
    strcpy(messageClient.searchedString, *searchedString);
   }
    messageClient.callingProcessID = getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient) - sizeof(messageClient.mtype), !IPC_NOWAIT);
    
    //struct message messageClient2;
    //printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
    int rec= msgrcv(clientDBManagerMsgQId, &messageClient, (sizeof(messageClient) - sizeof(messageClient.mtype)), getpid(), !IPC_NOWAIT);
    printf("key %d \n",messageClient.queryType);
    if (rec == -1)
    {
        perror("error in send msg");
    }
    else
    {
        logQuery();
        sendToQueryLogger();
    }
}
void logQuery()
{
    messageLoggerClient.mtype = LoggerId;
    messageLoggerClient.PID = getpid();
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_ACQUIRE;
    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
    raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);

    char clientNumberString[5];
    sprintf(clientNumberString, "%d", clientNumber);
    strcpy(clientLogger->number, clientNumberString);
    char msgArray[MAXCHAR] = "I requested a query ";

    strcpy(clientLogger->message, msgArray);
    messageLoggerClient.destinationProcess = MESSAGE_TYPE_RELEASE;

    send_val = msgsnd(loggerMsgQIdClient, &messageLoggerClient, sizeof(messageLoggerClient) - sizeof(messageLoggerClient.mtype), !IPC_NOWAIT);

    
    /*raise(SIGTSTP);
    signal(SIGTSTP, SIG_DFL);*/
}
void sendToQueryLogger()
{
    //dynamic alloc is here
   for(int i=0;i<100;i++)
    {
       if( messageClient.queryKeys[i]==-1)
        {

            break;
        }
        printf("querryyyyyyyyyyyzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz  break  %d  __ %d __ %d\n",messageClient.queryKeys[i],getpid(),i);
    }
     printf("querryyAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa \n");
    
    //set size
   
    char clientNumberString[5];
    char message[MAXCHAR]="I am client number: ";

    sprintf(clientNumberString, "%d", clientNumber);
    strcat(message,clientNumberString);

    char msgArray[MAXCHAR] = " I requested a query ";
    strcat(message, msgArray);
    //need switch case for query type

    strcpy(queryLoggerMsgQClient.message,message);
    queryLoggerMsgQClient.mtype=QueryLoggerId;
    //set point and dynamic alloc

    send_val = msgsnd(queryLoggerMsgQIdClient, &queryLoggerMsgQClient, sizeof(queryLoggerMsgQClient) - sizeof(queryLoggerMsgQClient.mtype), !IPC_NOWAIT);

}
void handlingSIGUSR1_and_IgnoringSigStop()
{
    //printf("Woke me before sleeping\n");

    signal(SIGTSTP, SIG_IGN);
}