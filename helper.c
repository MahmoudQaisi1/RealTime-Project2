

#include "local.h"

int main(int argc, char *argv[])
{
    usleep(1000);

    printf("im in helper\n");
    int flag = 0;
    int num_of_colomns = atoi(argv[1]);

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
    int index1 = rand() % num_of_colomns;
    int index2 = rand() % num_of_colomns;
    while (1)
    {

        wait_semaphore(sem_id);

        char temp_str[MaxWidth];
        char temp_str2[MaxWidth];

        strncpy(temp_str, shared_memory[index1].message, MaxWidth - 1);
        strncpy(temp_str2, shared_memory[index2].message, MaxWidth - 1);

        strncpy(shared_memory[index1].message, temp_str2, MaxWidth - 1);
        strncpy(shared_memory[index2].message, temp_str, MaxWidth - 1);

        printf("@@helper Process: I swaped Message %d and Message %d @@\n", index1 + 1, index2 + 1);

        signal_semaphore(sem_id);
        usleep(2000);
    }

    return 0;
}
