#ifndef LOCAL_H
#define LOCAL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MaxLength 200

#define SHM_SIZE 200
#define MaxWidth 500


typedef struct
{
  char message[MaxWidth];
} SharedMemory;

union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

void wait_semaphore(int sem_id);
void signal_semaphore(int sem_id);
void wait_semaphore(int sem_id)
{
  struct sembuf operation;
  operation.sem_num = 0;
  operation.sem_op = -1;
  operation.sem_flg = SEM_UNDO;
  semop(sem_id, &operation, 1);
}

void signal_semaphore(int sem_id)
{
  struct sembuf operation;
  operation.sem_num = 0;
  operation.sem_op = 1;
  operation.sem_flg = SEM_UNDO;
  semop(sem_id, &operation, 1);
}
#endif /* LOCAL_H */
