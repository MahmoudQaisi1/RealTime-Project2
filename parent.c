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
  char messages[5][5][150] = {
      {"1", "World", "OpenAI", "ChatGPT", "Shared Memory"},
      {"2", "C", "Python", "Java", "Ruby"},
      {"3", "Windows", "MacOS", "Unix", "BSD"},
      {"4", "Mutex", "Lock", "Thread", "Process"},
      {"5", "TCP", "UDP", "HTTP", "IP"}

  };

  int i, j;
  for (i = 0; i < 5; i++)
  {
    for (j = 0; j < 5; j++)
    {

      strcpy(shared_memory[i].message[j], messages[i][j]);
      printf("  String %d: %s\n", j + 1, shared_memory[i].message[j]);
    }
  }
}


int main()
{

  int user_spy = 1;
  int spy = 0;
  pid_t spy_pid_array[user_spy];
  char spies[10];

  key_t key = ftok(".", 'S');

  int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
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

  int sem_id = semget(key, 1, IPC_CREAT | 0666);
  if (sem_id < 0)
  {
    perror("semget");
    exit(1);
  }

  union semun sem_union;
  sem_union.val = 1;
  semctl(sem_id, 0, SETVAL, sem_union);

  create_messages(shared_memory);

  for (int i = 0; i < user_spy; i++)
  {
    spy++;
    snprintf(spies, 10, "%d", spy);
    spy_pid_array[spy - 1] = fork();
    if (spy_pid_array[spy - 1] == 0)
    {
      execlp("./spy", "spy", spies, NULL); // path to projet file
      printf("Error!! generating player %d", spy);
    }
  }

  pid_t receiver_pid = fork();
  if (receiver_pid == 0)
  {
    execlp("./receiver", "receiver", NULL);
    printf("Error!! generating receiver");
  }

  if (receiver_pid > 0)
  {
    int status;
    int flag = 0;
    while (1)
    {

      // Wait for any child to finish
      pid_t finished_child = wait(&status);

      if (finished_child == receiver_pid)
      {
        printf("Operation successful\n");
        flag = 1;
      }
      if (finished_child == spy_pid_array[0])
      {
        printf("Operation failed\n");
        flag = 1;
      }
      if (flag == 1)
      {
        for (int i = 0; i < user_spy; i++)
        {
          kill(spy_pid_array[i], SIGKILL);
          printf("Process ID %d has terminated\n", spy_pid_array[i]);
        }
        kill(receiver_pid, SIGKILL);
        shmdt(shared_memory);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(sem_id, 0, IPC_RMID, 0);
        break;
      }
    }
    // Parent process
    /*
            wait_semaphore(sem_id);
            printf("Parent Process: Messages in shared memory:\n");
            int i, j;
            for (i = 0; i <5; i++) {
                printf("Message %d:\n", i+1);
                for (j = 0; j < 5; j++) {
                    printf("  String %d: %s\n", j+1, shared_memory[i].message[j]);
                }
            }
            signal_semaphore(sem_id);
    */
  }
  else
  {
    perror("fork");
    exit(1);
  }

  return 0;
}
