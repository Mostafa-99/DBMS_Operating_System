#include "Parent.h"
//#include "Client.h"

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
 // printf("msgqid = %d\n", msgqid);
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
  //  printf("I'm parent my new child: %d\n",processesIds[i]);
  }
  return pid;
}
void initializeSharedRecources()
{   
    LoggerSharedMemoryId = shmget(IPC_PRIVATE,LOGGER_SHAREDMEMORY_SIZE,0666|IPC_CREAT);
    DBSharedMemoryId = shmget(IPC_PRIVATE,DBMANAGER_SHAREDMEMORY_SIZE,0666|IPC_CREAT);
   // printf("I'm parent I created the resources: %d  and  %d\n",LoggerSharedMemoryId,DBSharedMemoryId);
}
void roleIdentification()
{
    int send_val;
    key_t clientDBManager_msgqid = msgget(IPC_PRIVATE, 0644); // or msgget(12613, IPC_CREATE | 0644)
    int clientNumber=1;
    for(int i=0;i<NUMBER_OF_PROCESS;i++)
    {     
        struct parentMsgBuff message;
        message.mtype=processesIds[i];
        if(i==DBMANAGER_INDEX)//BDManager
        {
            message.role="DBManager";
            message.sharedMemoryId=LoggerSharedMemoryId;
            message.clientDBManagerMsgQId=clientDBManager_msgqid;
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
            message.clientDBManagerMsgQId=clientDBManager_msgqid;
            message.clientNumber=clientNumber;
            clientNumber++;
        }
        send_val = msgsnd(msgqid, &message, sizeof(message)-sizeof(message.mtype), !IPC_NOWAIT);
    }
}
/*void do_child()
{
    int rec_val;
    struct parentMsgBuff message; */
    /* receive all types of messages */
  /*  rec_val = msgrcv(msgqid, &message, sizeof(message)-sizeof(message.mtype), getpid(), !IPC_NOWAIT);  
    
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
            do_Client(message.DBManagerId,message.QueryLoggerId,message.sharedMemoryId,message.clientDBManagerMsgQId, message.clientNumber);
        } 
    }
}*/
//----------------------------------------------------------------