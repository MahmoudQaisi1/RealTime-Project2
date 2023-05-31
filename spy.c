#include "local.h"

int main(int argc, char *argv[])
{
    sleep(5);

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
    // call first shared memory////////////

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

    // call second shared memory to comminucate with master spy////////////

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

    int sem_id2 = semget(masterPid, 2, 0);
    if (sem_id2 < 0)
    {
        perror("semget2");
        exit(1);
    }

    while (1)
    {
        int random_index = rand() % num_of_colomns +1;
        wait_semaphore(sem_id);
        wait_semaphore(sem_id2);

        if (strlen(shared_memory[random_index].message) == 0)
        {
            printf("**spy Process:  Message at index %d is Empty**\n", random_index + 1);
        }
        else
        {
            char Tstr[MaxWidth];
            strncpy(Tstr, shared_memory[random_index].message, MaxWidth - 1);

            char *snum;
            int colomn_num;
            snum = strtok(Tstr, "/");
            if (snum != NULL)
            {
                colomn_num = atoi(snum);
                int i = colomn_num - 1;
                strncpy(shared_memory2[i].message, shared_memory[random_index].message, MaxWidth - 1);
                printf("**spy Process: I got Message %d at index %d**\n", i + 1, random_index + 1);
            }
        }

        signal_semaphore(sem_id2);
        signal_semaphore(sem_id);
        sleep(rand()%5+2);
    }

    return 0;
}