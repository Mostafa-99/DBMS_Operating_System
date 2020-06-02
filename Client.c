#include "Client.h"


void do_client(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived,int DBSharedMemoryIdReceived)
{
    initializeClient(DBManagerIdReceived, QueryLoggerIdReceived, sharedMemoryIdReceived, clientDBManagerMsgQIdReceived, clientNumberReceived,DBSharedMemoryIdReceived);
    openConfigurationFile();
    char * readWordFromFile="1";
    while(readWordFromFile!=NULL)
    {
        int acquireTypeFlag=0;
        readWordFromFile=readConfigurationFile();
        if(readWordFromFile==NULL)
        {
            break;
        }
        
        if(strcmp ( readWordFromFile, "add")==0)
        {
            char  name[MAXCHAR];
            strcpy(name,readConfigurationFile());
            char salary[MAXCHAR];
            strcpy(salary,readConfigurationFile());
            requestToAdd(name, atoi(salary));
        }
        else if(strcmp ( readWordFromFile, "modify")==0)
        {
            char key[MAXCHAR];
            strcpy(key,readConfigurationFile());
            char modification[MAXCHAR];
            strcpy(modification,readConfigurationFile());
            int sign= strcmp ( modification, "+")==0 ? ADD_MODIFICATION : SUBTRACT_MODIFICATION;
            char value[MAXCHAR];
            strcpy(value,readConfigurationFile());
            requestToModify(atoi(key), sign, atoi(value));
        }
        else if(strcmp ( readWordFromFile, "acquire")==0)
        {
            char key[MAXCHAR];
            strcpy(key,readConfigurationFile());
            requestToAcquire(atoi(key));
            acquireTypeFlag=1;
        }
        else if(strcmp ( readWordFromFile, "release")==0)
        {
            char key[MAXCHAR];
            strcpy(key,readConfigurationFile());
            requestToRelease(atoi(key));
        }
        else if(strcmp ( readWordFromFile, "query")==0)
        {
          //  requestToQuery(int queryType, int searchedSalary, char* searchedString)
        }
        if(acquireTypeFlag)
        {
            handlingSIGUSR1_and_IgnoringSigStop();
            raise(SIGSTOP);
            signal(SIGSTOP, SIG_DFL);
        }
    }
    fclose(fp);
}
void initializeClient(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived,int DBSharedMemoryIdReceived)
{
    DBManagerId=DBManagerIdReceived;
    QueryLoggerId=QueryLoggerIdReceived;
    sharedMemoryId=sharedMemoryIdReceived;
    clientDBManagerMsgQId=clientDBManagerMsgQIdReceived;
    clientNumber=clientNumberReceived;
    messageClient.mtype=DBManagerId;
    signal (SIGUSR1, handlingSIGUSR1_and_IgnoringSigStop);		
    DBtableView = (struct DBrecord *)shmat(DBSharedMemoryIdReceived, (void *)0, SHM_RDONLY);
}
void openConfigurationFile()
{
    char buffer[50]; 
   	sprintf(buffer, "%d",clientNumber); 
    filename =strcat(buffer,".txt");
    fp = fopen(filename, "r");
}
char * readConfigurationFile()
{
    //read config
    if (fp ==NULL){
        printf("Could not open file %s\n",filename);
        return 0;
    }
    
    if(fscanf(fp,"%s",word) != EOF)
    {
        return word;
    }
    else
    {
        return NULL;
    }
}
void requestToAdd(char* name, int salary)
{
    printf("I am child: %d and I am client number: %d, I require to add %s with salary %d \n",getpid(),clientNumber,name,salary);
    messageClient.destinationProcess=MESSAGE_TYPE_ADD;
    strcpy(messageClient.name,name);
    messageClient.salary=salary;
    messageClient.callingProcessID=getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient)-sizeof(messageClient.mtype), !IPC_NOWAIT);
    printf("I am child I want to send %d  %d  size %d\n",clientDBManagerMsgQId, &messageClient, sizeof(messageClient)-sizeof(messageClient.mtype));
    if(send_val==-1)
    {
        perror("error in send msg");
    }

}
void requestToModify(int key, int modification, int value)
{
    printf("I am child: %d and I am client number: %d, I require to modify key: %d, modification: %d, with value %d \n",getpid(),clientNumber,key,modification,value);
    messageClient.destinationProcess=MESSAGE_TYPE_MODIFY;
    messageClient.key=key;
    messageClient.modification=value*modification;
    messageClient.callingProcessID=getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient)-sizeof(messageClient.mtype), !IPC_NOWAIT);
    if(send_val==-1)
    {
        perror("error in send msg");
    }

}
void requestToAcquire(int key)
{
    printf("I am child: %d and I am client number: %d, I acquire key %d \n",getpid(),clientNumber,key);
    messageClient.destinationProcess=MESSAGE_TYPE_ACQUIRE;
    messageClient.key=key;
    messageClient.callingProcessID=getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient)-sizeof(messageClient.mtype), !IPC_NOWAIT);
    if(send_val==-1)
    {
        perror("error in send msg");
    }

}
void requestToRelease(int key)
{
    printf("I am child: %d and I am client number: %d, I release key %d \n",getpid(),clientNumber,key);
    messageClient.destinationProcess=MESSAGE_TYPE_RELEASE;
    messageClient.key=key;
    messageClient.callingProcessID=getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient)-sizeof(messageClient.mtype), !IPC_NOWAIT);
    if(send_val==-1)
    {
        perror("error in send msg");
    }

}
void requestToQuery(int queryType, int searchedSalary, char* searchedString)
{
    messageClient.destinationProcess=MESSAGE_TYPE_QUERY;
    messageClient.queryType=queryType;
    messageClient.searchedSalary=searchedSalary;
    messageClient.searchedString=searchedString;
    messageClient.callingProcessID=getpid();
    send_val = msgsnd(clientDBManagerMsgQId, &messageClient, sizeof(messageClient)-sizeof(messageClient.mtype), !IPC_NOWAIT);
    if(send_val==-1)
    {
        perror("error in send msg");
    }

}


void handlingSIGUSR1_and_IgnoringSigStop()
{
    signal(SIGSTOP, SIG_IGN);
}