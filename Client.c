#include "Client.h"


void do_client(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived)
{
    initializeClient(DBManagerIdReceived, QueryLoggerIdReceived, sharedMemoryIdReceived, clientDBManagerMsgQIdReceived, clientNumberReceived);
    openConfigurationFile();
    char * readWordFromFile="1";
    while(readWordFromFile!=NULL)
    {
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
        send_val = msgsnd(clientDBManagerMsgQId, &message, sizeof(message)-sizeof(message.mtype), !IPC_NOWAIT);
        if(send_val==-1)
        {
            perror("error in send msg");
        }
    }
    fclose(fp);
}
void initializeClient(int DBManagerIdReceived, int QueryLoggerIdReceived, int sharedMemoryIdReceived, int clientDBManagerMsgQIdReceived, int clientNumberReceived)
{
    /*Note: add the message queue ID*/
    DBManagerId=DBManagerIdReceived;
    QueryLoggerId=QueryLoggerIdReceived;
    sharedMemoryId=sharedMemoryIdReceived;
    clientDBManagerMsgQId=clientDBManagerMsgQIdReceived;
    clientNumber=clientNumberReceived;
    message.mtype=DBManagerId;
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

/*Note: send the message in all the request functions*/
void requestToAdd(char* name, int salary)
{
    printf("I am child: %d and I am client number: %d, I require to add %s with salary %d \n",getpid(),clientNumber,name,salary);
    message.destinationProcess=MESSAGE_TYPE_ADD;
    message.name=name;
    message.salary=salary;
}
void requestToModify(int key, int modification, int value)
{
    printf("I am child: %d and I am client number: %d, I require to modify key: %d, modification: %d, with value %d \n",getpid(),clientNumber,key,modification,value);
    message.destinationProcess=MESSAGE_TYPE_MODIFY;
    message.key=key;
    message.modification=modification;
    message.salary=value;
}
void requestToAcquire(int key)
{
    printf("I am child: %d and I am client number: %d, I acquire key %d \n",getpid(),clientNumber,key);
    message.destinationProcess=MESSAGE_TYPE_ACQUIRE;
    message.key=key;
}
void requestToRelease(int key)
{
    printf("I am child: %d and I am client number: %d, I release key %d \n",getpid(),clientNumber,key);
    message.destinationProcess=MESSAGE_TYPE_RELEASE;
    message.key=key;
}
void requestToQuery(int queryType, int searchedSalary, char* searchedString)
{
    message.destinationProcess=MESSAGE_TYPE_QUERY;
    message.queryType=queryType;
    message.searchedSalary=searchedSalary;
    message.searchedString=searchedString;
}