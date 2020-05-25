#include "declarations.c"

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