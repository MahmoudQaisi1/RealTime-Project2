
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

  SharedMemory  *shared_memory;
  int            sem_id, shmid, croaker;
  char          *shmptr;
  union semun    arg;
  pid_t          sender_id, receiver_id,master_spy_id,pid;
  int            i,numOfHelpers,numOfSpies,columns,
                 success_threshold = 2, failing_threshold = 2, 
                 success_counter = 0,failing_counter = 0;
  char           masterPid[10],columnsStr[10];
  pid_t           helpers_id[MaxLength];
  pid_t           spies_id[MaxLength];

  void resume(){
    printf("Parent awake...\n");
  }

main(int argc, char *argv[])
{
  
  pid = getpid();

  if ( argc != 3 ) {
    fprintf(stderr, "%s Number_of_helpers number_of_spies \n", argv[0]);
    exit(-1);
  }

  numOfHelpers = atoi(argv[1]);
  numOfSpies = atoi(argv[2]);

  if (signal(SIGCONT, resume) == SIG_ERR) // SIG_ERR = -1
  {
    perror("Sigset can not set SIGUSR1");
    exit(SIGUSR1);
  }

  //Shared memory creatuin and attachment//////////////////////////////////////////////////////
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

  //Initializing the semaphore/////////////////////////////////////////////////////////////////
  sem_id = semget((int)pid, 1, IPC_CREAT | 0666);
  if (sem_id < 0)
  {
    perror("semget");
    exit(1);
  }
  union semun sem_union;
  sem_union.val = 1;
  semctl(sem_id, 0, SETVAL, sem_union);

  // forking the needed processes//////////////////////////////////////////////////////////////
  //sender////////
  if ( (sender_id = fork()) == -1 ) {
    perror("fork -- sender");
    exit(5); 
  }
  else if ( sender_id == 0 ) {
    execl("./sender", "./sender", (char *) 0);
    perror("execl -- sender");
    exit(6);
  }
  sleep(0.5);
  printf("Pausing....\n");
  pause();
  

  if(shared_memory[0].message != NULL){
      columns = atoi(shared_memory[0].message);
  }else{
    printf("error --reading columns");
  }

  sprintf(columnsStr,"%d",columns);

  printf("Columns: %s\n",columnsStr);

  sleep(0.5);
  //receiver//////
  if ( (receiver_id = fork()) == -1 ) {
    perror("fork -- receiver");
    exit(5); 
  }
  else if ( receiver_id == 0 ) {
    execl("./receiver", "./receiver",columnsStr, (char *) 0);
    perror("execl -- receiver");
    exit(6);
  }
  sleep(0.5);

  //master Spy/////
  
  if ( (master_spy_id = fork()) == -1 ) {
    perror("fork -- master spy");
    exit(5); 
  }
  else if ( master_spy_id == 0 ) {
    execl("./master_spy", "./master_spy", columnsStr, (char *) 0);
    perror("execl -- master spy");
    exit(6);
  }
  sleep(0.5);

  //helpers////////////////////
  for ( i = 0; i < numOfHelpers; i++)
  {
    if ( (helpers_id[i] = fork()) == -1 ) {
      perror("fork -- helper");
      exit(5); 
    }
    else if ( helpers_id[i] == 0 ) {
      execl("./helper", "./helper",columnsStr,(char *) 0);
      perror("execl -- helper");
      exit(6);
    }
    sleep(0.5);
  }

  //spies///////////////////
  char master[10];
  sprintf(master,"%d",(int)master_spy_id);
  for ( i = 0; i < numOfSpies; i++)
  {
    if ( (spies_id[i] = fork()) == -1 ) {
      perror("fork -- spy");
      exit(5); 
    }
    else if ( spies_id[i] == 0 ) {
      execl("./spy", "./spy", master,columnsStr, (char *) 0);
      perror("execl -- spy");
      exit(6);
    }
    sleep(0.5);
  }

  kill(receiver_id, SIGUSR1);
  kill(master_spy_id, SIGUSR2);
  
  while (1);
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

    kill(receiver_id, SIGUSR1);
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

    for (i = 0; i < numOfSpies; i++)
    {
      kill(spies_id[i], SIGKILL);
    }
    for (i = 0; i < numOfHelpers; i++)
    {
      kill(helpers_id[i], SIGKILL);
    }
    kill(receiver_id, SIGKILL);
    kill(master_spy_id, SIGKILL);

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
    kill(master_spy_id, SIGUSR2);
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
    for (i = 0; i < numOfSpies; i++)
    {
      kill(spies_id[i], SIGKILL);
    }
    for (i = 0; i < numOfHelpers; i++)
    {
      kill(helpers_id[i], SIGKILL);
    }
    kill(receiver_id, SIGKILL);
    kill(master_spy_id, SIGKILL);

    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0); /* remove */
    semctl(sem_id, 0, IPC_RMID, 0);
    printf("Processes are terminated\n");

    exit(0);
  }
} 