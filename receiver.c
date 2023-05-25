#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "local.h"

int main()
{
    int memSize = 5;
    int flag = 0;

    srand((unsigned)getpid());
    int isRecived[5] = {0, 0, 0, 0, 0};

    key_t key = ftok(".", 'S');

    int shmid = shmget(key, SHM_SIZE, 0);
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

    int sem_id = semget(key, 1, 0);
    if (sem_id < 0)
    {
        perror("semget");
        exit(1);
    }
    while (flag == 0)
    {
        wait_semaphore(sem_id);
        printf("receiver Process: Messages in shared memory:\n");
        int j;
        int random_index = rand() % 5;
        char snum[150];
        strcpy(snum, shared_memory[random_index].message[0]);
        int num;
        num = atoi(snum);
        isRecived[num - 1] = 1;

        printf("Message %d:\n", random_index + 1);
        for (j = 1; j < 5; j++)
        {
            printf("  String %d: %s\n", j, shared_memory[random_index].message[j]);
        }
        signal_semaphore(sem_id);
        for (int i = 0; i < memSize; i++)
        {
            if (isRecived[i] == 0)
            {
                flag = 0;
                break;
            }
            else
                flag = 1;
        }
        sleep(0.3);
    }
    shmdt(shared_memory);
    exit(0); // Terminate the child process

    return 0;
}
