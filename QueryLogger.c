#include "QueryLogger.h"





void do_queryLogger(int queryLoggerMsgQIdReceived)
{
  initializeQueryLogger(queryLoggerMsgQIdReceived);
  FILE *fileOpened;
  char fileToOpen[80];
    while (1)
    {
      //  printf("QueryLogger is here  MsgQId: %d  \n");
      //sleep(100);
      int rec=msgrcv(queryLoggerMsgQIdQuery, &queryLoggerMsgQProccess, (sizeof(queryLoggerMsgQProccess) - sizeof(queryLoggerMsgQProccess.mtype)), getpid(), !IPC_NOWAIT);
      // if(rec!=-1)
      printf("QueryLogger is not here  %s\n",queryLoggerMsgQProccess.message);
      fileOpened = fopen("QueryLogger", "a"); //opening file  a
      if (fileOpened == NULL)
      {
          printf("unable to open file\n");
      }
      else
      {
          fprintf(fileOpened, "Query Number %d : ",numberOfCurrentLog);
          fprintf(fileOpened, "%lu\t", (unsigned long)time(NULL));
          fprintf(fileOpened, "%s", queryLoggerMsgQProccess.message);   //writing data into file
          fprintf(fileOpened, "\n");   
          fprintf(fileOpened, "---------------------------------------------------------------------------------------------------------\n"); 
          //replace the comments on next two lines because the size is always zero

          //for (int i = 0; i < queryLoggerMsgQProccess.size; i++)
          for (int i = 0; i < 5; i++)
          {
            //replace the comments on next two (4 lines) to get the real values

            // fprintf(fileOpened, "%d\t",(i+1));
            // fprintf(fileOpened, "%d\t",queryLoggerMsgQProccess.firstRecordAddress[i].key);
            // fprintf(fileOpened, "%s\t",queryLoggerMsgQProccess.firstRecordAddress[i].name);
            // fprintf(fileOpened, "%d\n",queryLoggerMsgQProccess.firstRecordAddress[i].salary);
            fprintf(fileOpened, "%d\t",(i+1));
            fprintf(fileOpened, "%s\t\t","key");
            fprintf(fileOpened, "%s\t","name");
            fprintf(fileOpened, "%s\n","salary");
          }
          fprintf(fileOpened, "-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n");  
          numberOfCurrentLog++; 
      }
      fflush(fileOpened);
    }
}

void  initializeQueryLogger(int queryLoggerMsgQIdReceived)
{
    queryLoggerMsgQIdQuery=queryLoggerMsgQIdReceived;
    numberOfCurrentLog=1;
}
