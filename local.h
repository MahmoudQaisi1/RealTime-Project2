#ifndef __LOCAL_H_
#define __LOCAL_H_

/*
 * Common header file: parent, producer and consumer
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <math.h>

#define SHM_SIZE 20
#define MaxLength 200
#define MaxWidth 512


typedef struct
{
  char message[MaxWidth];
} SharedMemory;

typedef struct 
{
  int messages;
  int encoded;
  int spy;
  int receiver;
  int thresh1;
  int thresh2;
  int th1;
  int th2;
} openGL;


int * flags;

union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

int wait_semaphore(int sem_id);
int signal_semaphore(int sem_id);
int wait_semaphore(int sem_id)
{
  struct sembuf operation;
  operation.sem_num = 0;
  operation.sem_op = -1;
  operation.sem_flg = SEM_UNDO;
  return semop(sem_id, &operation, 1);
}

int signal_semaphore(int sem_id)
{
  struct sembuf operation;
  operation.sem_num = 0;
  operation.sem_op = 1;
  operation.sem_flg = SEM_UNDO;
  return semop(sem_id, &operation, 1);
}
#endif


