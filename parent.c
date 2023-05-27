#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include "local.h"

void create_messages(SharedMemory *shared_memory)
{
  char messages[5][MaxWidth] = {
      {"1/bdf/egi/hjl/kmo"},
      {"2/oru/rux/uxa/xad"},
      {"3/bf/200000/12/2"},
      {"4/b200/999999/alright/hana"},
      {"5/alright/4/5/6"}
  };


  for (int i = 0; i <5; i++)
  {
        strncpy(shared_memory[i].message, messages[i], MaxWidth - 1);
        shared_memory[i].message[MaxWidth - 1] = '\0';
        printf("String %d: %s\n", i + 1, shared_memory[i].message);
  }
}

int main(int argc, char *argv[])
{
  int user_spy = 1;
  int helpers = 1;
  int success_threshold = 2;
  int failing_threshold = 2;
  int success_counter = 0;
  int failing_counter = 0;

  int num_of_colomns = 5;
  char num_of_colomns_str[10];

  snprintf(num_of_colomns_str, 10, "%d", num_of_colomns);

while(success_counter < success_threshold && failing_counter < failing_threshold){
  int spy = 0;
  pid_t spy_pid_array[user_spy];
  char masterPid[10];
  pid_t pid = getpid();

  int shmid = shmget((int)pid, SHM_SIZE, IPC_CREAT | 0666);
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

  int sem_id = semget((int)pid, 1, IPC_CREAT | 0666);
  if (sem_id < 0)
  {
    perror("semget");
    exit(1);
  }
  union semun sem_union;
  sem_union.val = 1;
  semctl(sem_id, 0, SETVAL, sem_union);

  create_messages(shared_memory);

  pid_t masterSpy_pid = fork();
  if (masterSpy_pid == 0)
  {
    execlp("./masterSpy", "masterSpy", num_of_colomns_str, NULL);
    printf("Error!! generating masterSpy");
  }

  for (int i = 0; i < user_spy; i++)
  {
    spy++;
    snprintf(masterPid, 10, "%d", (int)masterSpy_pid);
    spy_pid_array[spy - 1] = fork();
    if (spy_pid_array[spy - 1] == 0)
    {
      execlp("./spy", "spy", masterPid, num_of_colomns_str, NULL); // path to projet file
      printf("Error!! generating spy %d", spy);
    }
  }

  pid_t receiver_pid = fork();
  if (receiver_pid == 0)
  {
    execlp("./receiver", "receiver", num_of_colomns_str, NULL);
    printf("Error!! generating receiver");
  }

  if (receiver_pid > 0)
  {
    int status;
    int flag = 0;
        // Parent process
/*
            wait_semaphore(sem_id);
            printf("Parent Process: Messages in shared memory:\n");
            int i;
            for (i = 0; i <5; i++) {
                printf("Message %d:\n", i+1);
                    printf("  String %d: %s\n", i+1, shared_memory[i].message);
            }
            signal_semaphore(sem_id);
            */
            
    while (1)
    {

      // Wait for any child to finish
      pid_t finished_child = wait(&status);

      if (finished_child == receiver_pid)
      {
        printf("########### Operation successful: Receiver finished############\n\n\n");
        success_counter++;
        flag = 1;
      }

      if (finished_child == masterSpy_pid)
      {
        printf("########### Operation failed: Spy finished#######################\n\n\n");
        failing_counter++;
        flag = 1;
      }
      if (flag == 1)
      {
        for (int i = 0; i < user_spy; i++)
        {
          kill(spy_pid_array[i], SIGKILL);
          //printf("Process ID %d has terminated\n", spy_pid_array[i]);
        }
        kill(receiver_pid, SIGKILL);
        kill(masterSpy_pid, SIGKILL);
        //printf("Process ID %d has terminated\n", masterSpy_pid);

        shmdt(shared_memory);
        shmctl(shmid, IPC_RMID, (struct shmid_ds *)0); /* remove */
        semctl(sem_id, 0, IPC_RMID, 0);
        break;
      }
    }

    
  }
  else
  {
    perror("fork");
    exit(1);
  }
}
  return 0;
}
