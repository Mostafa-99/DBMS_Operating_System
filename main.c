//#include "declarations.c"
#include "Parent.c"
#include "DBManager.c"
#include "Client.c"

void do_child(); //base implemented

void main()
{   //readConfiguartions();
    pid_t pid;
    pid= forkAllChildren(NULL);
    if(pid!=0) //Parent
    {   int stat_loc;
        initializeSharedRecources();
        roleIdentification();
        sleep(10);
        // add wait for exit code
        for(int i=0;i<NUMBER_OF_PROCESS;i++)
        {
           pid = wait(&stat_loc);
        }
        // add destroy 2 shared memory and msgQ
        shmctl(LoggerSharedMemoryId,IPC_RMID,NULL);
        shmctl(DBSharedMemoryId,IPC_RMID,NULL);
        msgctl(msgqid, IPC_RMID, (struct msqid_ds *) 0);
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
    
    if(rec_val == -1)
    {
        perror("Error in receive");
    }
    else
    {
        //printf("Role Message received I am child: %d and I am %s \n",getpid(),message.role);
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
            do_client(message.DBManagerId,message.QueryLoggerId,message.sharedMemoryId,message.clientDBManagerMsgQId, message.clientNumber);
        } 
    }
}