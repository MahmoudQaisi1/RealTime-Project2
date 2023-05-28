
#include "local.h"

int compareFiles(const char *file1, const char *file2) // to compare between two files entires
{
  FILE *fptr1 = fopen(file1, "r");
  FILE *fptr2 = fopen(file2, "r");

  if (fptr1 == NULL || fptr2 == NULL)
  {
    printf("Failed to open one or both files.\n");
    return -1;
  }

  int char1, char2;
  int difference = 0;
  int position = 0;

  while ((char1 = fgetc(fptr1)) != EOF && (char2 = fgetc(fptr2)) != EOF)
  {
    position++;

    if (char1 != char2)
    {
      difference = 1;
      printf("Files differ at position %d: '%c' vs '%c'\n", position, char1, char2);
      break;
    }
  }

  if (!difference)
    printf("Files are identical.\n");

  fclose(fptr1);
  fclose(fptr2);

  return difference;
}

void create_messages(SharedMemory *shared_memory)
{
  char messages[10][MaxWidth] = {
      {"1/bdf/egi/hjl/kmo"},
      {"2/oru/rux/uxa/xad"},
      {"3/bf/200000/12/2"},
      {"4/b200/999999/alright/hana"},
      {"5/alright/4/5/6"},
      {"6/oru/rux/uxa/xad"},
      {"7/alright/4/5/6"}};

  for (int i = 0; i < 7; i++)
  {
    strncpy(shared_memory[i].message, messages[i], MaxWidth - 1);
    shared_memory[i].message[MaxWidth - 1] = '\0';
    printf("String %d: %s\n", i + 1, shared_memory[i].message);
  }
}

int success_counter = 0;
int failing_counter = 0;

int success_threshold;
int failing_threshold;
pid_t receiver_pid;
pid_t masterSpy_pid;
pid_t spyr_pid_array[MaxLength];
pid_t helper_pid_array[MaxLength];
int user_spy;
int user_helper;
int num_of_colomns;
char num_of_colomns_str[10];
int spy = 0;
pid_t spy_pid_array[MaxLength];
int helper = 0;
pid_t helper_pid_array[MaxLength];
SharedMemory *shared_memory;
int sem_id;
int shmid;

void success(int);
void failed(int);

int main(int argc, char *argv[])
{
  // Adjsting the signal.////////////////////////////////////////////////////////////////////////
  if (signal(SIGUSR1, success) == SIG_ERR) // SIG_ERR = -1
  {
    perror("Sigset can not set SIGUSR1");
    exit(SIGUSR1);
  }

  if (signal(SIGUSR2, failed) == SIG_ERR) // SIG_ERR = -1
  {
    perror("Sigset can not set SIGUSR1");
    exit(SIGUSR2);
  }

  user_spy = 1;
  user_helper = 1;

  success_threshold = 2;
  failing_threshold = 2;

  num_of_colomns = 7;

  snprintf(num_of_colomns_str, 10, "%d", num_of_colomns);

  // init Shared Memory///////////////////////////////////////////////////////////////////////

  char masterPid[10];
  pid_t pid = getpid();


  shmid = shmget((int)pid, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid < 0)
  {
    perror("shmget");
    exit(1);
  }

  shared_memory = (SharedMemory *)shmat(shmid, NULL, 0);
  if (shared_memory == (SharedMemory *)-1)
  {
    perror("shmat");
    exit(1);
  }

  // init binary semaphore///////////////////////////////////////////////////////////////////////

  sem_id = semget((int)pid, 1, IPC_CREAT | 0666);
  if (sem_id < 0)
  {
    perror("semget");
    exit(1);
  }
  union semun sem_union;
  sem_union.val = 1;
  semctl(sem_id, 0, SETVAL, sem_union);


  create_messages(shared_memory);

  // fork master spy///////////////////////////////////////////////////////////////////////

  masterSpy_pid = fork();
  if (masterSpy_pid == 0)
  {
    execlp("./masterSpy", "masterSpy", num_of_colomns_str, NULL);
    printf("Error!! generating masterSpy");
  }

  // fork  spies///////////////////////////////////////////////////////////////////////

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

  // fork  helpers///////////////////////////////////////////////////////////////////////

  for (int i = 0; i < user_helper; i++)
  {
    helper++;
    helper_pid_array[helper - 1] = fork();
    if (helper_pid_array[helper - 1] == 0)
    {
      execlp("./helper", "helper", num_of_colomns_str, NULL);
      printf("Error!! generating helper %d", helper);
    }
  }

  // fork  reciver///////////////////////////////////////////////////////////////////////

  receiver_pid = fork();
  if (receiver_pid == 0)
  {
    execlp("./receiver", "receiver", num_of_colomns_str, NULL);
    printf("Error!! generating receiver");
  }

  if (receiver_pid > 0)
  {
    ///// start  reciver and master Spy//////////////////
    usleep(10000);

    kill(receiver_pid, SIGUSR1);
    kill(masterSpy_pid, SIGUSR2);

    while (1);
  }
  else
  {
    perror("fork receiver_pid");
    exit(1);
  }
  return 0;
}

void success(int the_signal)    // when reciver send a signal///////////////////////////////////////////////////////////////////////
{
  success_counter++;

  if (success_counter < success_threshold)
  {
    const char *file1 = "sender.txt";
    const char *file2 = "receiver.txt";
    int result = compareFiles(file1, file2);
    if (result == -1)
    {
      printf("Error occurred during file comparison.\n");
    }
    else if (result == 0)
    {
      printf("########### Operation successful: Receiver finished############\n\n\n");
    }
    else
    {
      printf("########### Receiver finished But Decoding Failed############\n\n\n");
    }

    kill(receiver_pid, SIGUSR1);
  }
  else
  {
    const char *file1 = "sender.txt";
    const char *file2 = "receiver.txt";
    int result = compareFiles(file1, file2);
    if (result == -1)
    {
      printf("Error occurred during file comparison.\n");
    }
    else if (result == 0)
    {
      printf("########### Operation successful: Receiver finished############\n\n\n");
    }
    else
    {
      printf("########### Receiver finished But Decoding Failed############\n\n\n");
    }

    for (int i = 0; i < user_spy; i++)
    {
      kill(spy_pid_array[i], SIGKILL);
    }
    for (int i = 0; i < user_helper; i++)
    {
      kill(helper_pid_array[i], SIGKILL);
    }
    kill(receiver_pid, SIGKILL);
    kill(masterSpy_pid, SIGKILL);

    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0); /* remove */
    semctl(sem_id, 0, IPC_RMID, 0);
    printf("Processes are terminated\n");

    exit(0);
  }
}

void failed(int the_signal) // when master spy send a signal///////////////////////////////////////////////////////////////////////
{
  failing_counter++;

  if (failing_counter < failing_threshold)
  {
    const char *file1 = "sender.txt";
    const char *file2 = "spy.txt";
    int result = compareFiles(file1, file2);
    if (result == -1)
    {
      printf("Error occurred during file comparison.\n");
    }
    else if (result == 0)
    {
      printf("########### Operation failed: Spy finished#######################\n\n\n");
    }
    else
    {
      printf("########### Spy finished But Decoding Failed############\n\n\n");
    }
    kill(masterSpy_pid, SIGUSR2);
  }
  else
  {
    const char *file1 = "sender.txt";
    const char *file2 = "spy.txt";
    int result = compareFiles(file1, file2);
    if (result == -1)
    {
      printf("Error occurred during file comparison.\n");
    }
    else if (result == 0)
    {
      printf("########### Operation failed: Spy finished#######################\n\n\n");
    }
    else
    {
      printf("########### Spy finished But Decoding Failed############\n\n\n");
    }
    for (int i = 0; i < user_spy; i++)
    {
      kill(spy_pid_array[i], SIGKILL);
    }
    for (int i = 0; i < user_helper; i++)
    {
      kill(helper_pid_array[i], SIGKILL);
    }
    kill(receiver_pid, SIGKILL);
    kill(masterSpy_pid, SIGKILL);

    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0); /* remove */
    semctl(sem_id, 0, IPC_RMID, 0);
    printf("Processes are terminated\n");

    exit(0);
  }
} 
