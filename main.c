#include "GlobalIncludes.c"
#include "DBManager.c"
#include "Client.c"
#include "Logger.c"
#include "QueryLogger.c"




void main()
{   
    numberOfClients=readConfigurationFileParent();
    pid_t pid;
    pid= forkAllChildren(numberOfClients);
    if(pid!=CHILD_FORK_RETURN) //Parent
    {   int stat_loc;
        initializeSharedRecources();
        roleIdentification();
        sleep(10);
        // add wait for exit code
        for(int indexOfClient=0;indexOfClient<numberOfClients;indexOfClient++)
        {
           pid = wait(&stat_loc);
        }
        // add destroy 2 shared memory and msgQ
        shmctl(LoggerSharedMemoryId,IPC_RMID,NULL);
        shmctl(DBSharedMemoryId,IPC_RMID,NULL);
        msgctl(msgqid, IPC_RMID, (struct msqid_ds *) 0);
        killpg(getpgrp(),SIGTERM);
    }
    else
    {
        sleep(1);
        do_child();
        //printf("I'm new child my pid: %d and my parent pid: %d\n",getpid(),getppid());
    }
}

void do_child()
{
    int rec_val;
    struct parentMsgBuff message; 
    /* receive all types of messages */
    rec_val = msgrcv(msgqid, &message, sizeof(message)-sizeof(message.mtype), getpid(), !IPC_NOWAIT);  
    
    if(rec_val == ERROR_IN_RECEIVE)
    {
        perror("Error in receive");
    }
    else
    {
        //printf("Role Message received I am child: %d and I am %s \n",getpid(),message.role);
        if(message.role=="DBManager")
        {
            do_DBManager(message.sharedMemoryId,message.clientDBManagerMsgQId, message.DBSharedMemoryId,message.processesLoggerMsgQId,message.LoggerId);
        }
        else if(message.role=="QueryLogger")
        {
            do_queryLogger(message.queryLoggerMsgQId);
        }
        else if(message.role=="Logger")
        {
            do_logger(message.sharedMemoryId,message.processesLoggerMsgQId);
        }
        else
        {
            //printf("msg q %d \n",message.clientDBManagerMsgQId);
            do_client(message.DBManagerId,message.QueryLoggerId,message.sharedMemoryId,message.clientDBManagerMsgQId, message.clientNumber,message.DBSharedMemoryId,message.processesLoggerMsgQId,message.LoggerId,message.queryLoggerMsgQId);
        } 
    }
}