#include "GlobalIncludes.c"
#include "DBManager.c"
#include "Client.c"
#include "Logger.c"
#include "QueryLogger.c"

void main()
{
    numberOfClients = readConfigurationFileParent();
    pid_t pid;
    pid = forkAllChildren(numberOfClients);
    if (pid != CHILD_FORK_RETURN)
    {
        int stat_loc;
        initializeSharedRecources();
        roleIdentification();

        for (int indexOfClient = 0; indexOfClient < numberOfClients; indexOfClient++)
        {
            pid = wait(&stat_loc);
        }

        shmctl(LoggerSharedMemoryId, IPC_RMID, NULL);
        shmctl(DBSharedMemoryId, IPC_RMID, NULL);
        msgctl(msgqid, IPC_RMID, (struct msqid_ds *)0);
        killpg(getpgrp(), SIGTERM);
    }
    else
    {
        sleep(1);
        do_child();
    }
}

void do_child()
{
    int rec_val;
    struct parentMsgBuff message;

    rec_val = msgrcv(msgqid, &message, sizeof(message) - sizeof(message.mtype), getpid(), !IPC_NOWAIT);

    if (rec_val == ERROR_IN_RECEIVE)
    {
        perror("Error in receive");
    }
    else
    {
        if (message.role == "DBManager")
        {
            do_DBManager(message.sharedMemoryId, message.clientDBManagerMsgQId, message.DBSharedMemoryId, message.processesLoggerMsgQId, message.LoggerId);
        }
        else if (message.role == "QueryLogger")
        {
            do_queryLogger(message.queryLoggerMsgQId);
        }
        else if (message.role == "Logger")
        {
            do_logger(message.sharedMemoryId, message.processesLoggerMsgQId);
        }
        else
        {
            do_client(message.DBManagerId, message.QueryLoggerId, message.sharedMemoryId, message.clientDBManagerMsgQId, message.clientNumber, message.DBSharedMemoryId, message.processesLoggerMsgQId, message.LoggerId, message.queryLoggerMsgQId);
        }
    }
}