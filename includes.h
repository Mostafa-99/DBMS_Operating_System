#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<signal.h>
#include <sys/shm.h> 
#include <sys/wait.h>

//----------------------------
#define MESSAGE_TYPE_ADD 1
#define MESSAGE_TYPE_MODIFY 2
#define MESSAGE_TYPE_ACQUIRE 3
#define MESSAGE_TYPE_RELEASE 4
#define MESSAGE_TYPE_QUERY 5
//----------------------------
#define QUERY_BY_NAME 1
#define QUERY_BY_SALARY 2
#define QUERY_HYBRID 3
//----------------------------
/*System info*/
struct message
{
    long mtype;
    int destinationProcess;
    int key;
    char* name;
    int salary;
    int modification;

    int queryType;
    int searchedSalary;
    char* searchedString;
    char* logString;
};