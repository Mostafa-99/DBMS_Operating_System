#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define SEMAPHORE_OCCUPIED 0
#define SEMAPHORE_AVAILABLE 1
#define NO_NODE NULL
#define BLOCK_YOURSELF 0
#define UNBLOCK_YOURSELF 1
#define NO_PROCESS_WAITING -1
//----------------------------
#define MESSAGE_TYPE_ADD 1
#define MESSAGE_TYPE_MODIFY 2
#define MESSAGE_TYPE_ACQUIRE 3
#define MESSAGE_TYPE_RELEASE 4
#define MESSAGE_TYPE_QUERY 5
//----------------------------
#define QUERY_BY_FULL_TABLE 0
#define QUERY_BY_EXACT_NAME 1
#define QUERY_BY_PART_OF_NAME 2
#define QUERY_BY_EXACT_SALARY 3
#define QUERY_BY_GREATER_THAN_SALARY 4
#define QUERY_BY_LESS_THAN_SALARY 5
#define QUERY_BY_GREATER_THAN_OR_EQUAL_SALARY 6
#define QUERY_BY_LESS_THAN_OR_EQUAL_SALARY 7
#define QUERY_BY_EXACT_NAME_AND_SALARY_EXACT_HYBRID 8
//----------------------------
#define TRUE_ 1
#define NO_WORD NULL
#define IDENTICAL_WORDS 0
#define MAX_MEASSAGE_SIZE 420
#define MAX_NUMBER_SIZE 2
int messageQueueID;
#define MAXCHAR 1000
#define MAX_NUMBER_OF_RECORDS 1000