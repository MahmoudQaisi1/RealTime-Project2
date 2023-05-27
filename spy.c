#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#include "local.h"

int main(int argc, char *argv[])
{
        sleep(0.8);

      printf("im in spy\n");
    int flag = 0;
    int num_of_colomns = atoi(argv[2]);

    srand((unsigned)getpid());

    pid_t ppid = getppid();
    int shmid = shmget((int)ppid, SHM_SIZE, 0);
    if (shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    SharedMemory *shared_memory = (SharedMemory *)shmat(shmid, NULL, 0);
    if (shared_memory == (SharedMemory *)-1)
    {
        perror("shmat");
        exit(1);
    }

    int sem_id = semget((int)ppid, 1, 0);
    if (sem_id < 0)
    {
        perror("semget");
        exit(1);
    }

    int masterPid = atoi(argv[1]);

    //printf("master Pid = %s \n", argv[1]);

    int shmid2 = shmget(masterPid, SHM_SIZE, 0);
    if (shmid2 < 0)
    {
        perror("shmget2");
        exit(1);
    }

    SharedMemory *shared_memory2 = (SharedMemory *)shmat(shmid2, NULL, 0);
    if (shared_memory2 == (SharedMemory *)-1)
    {
        perror("shmat2");
        exit(1);
    }

    int sem_id2 = semget(masterPid, 1, 0);
    if (sem_id2 < 0)
    {
        perror("semget2");
        exit(1);
    }

    while (1)
    {

        wait_semaphore(sem_id);
        //printf("spy Process: Messages in shared memory:\n");
        int j;
        int random_index = rand() % num_of_colomns;

        wait_semaphore(sem_id2);
        printf("**spy Process: I got Message %d**\n", random_index + 1);
        
        strncpy(shared_memory2[random_index].message, shared_memory[random_index].message,MaxWidth -1);    
        signal_semaphore(sem_id2);

        //    for (j = 1; j < 5; j++)
        //    {
        //        printf("  String %d: %s\n", j, shared_memory[random_index].message[j]);
        //    }
        signal_semaphore(sem_id);
        sleep(0.3);
    }
    shmdt(shared_memory);
    shmdt(shared_memory2);

    exit(0); // Terminate the child process

    return 0;
}
