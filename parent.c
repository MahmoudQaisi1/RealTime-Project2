
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

  
  void resume(){
    printf("Parent awake...\n");
  }

  void success(int);
  void failed(int);

  SharedMemory  *shared_memory;
  openGL         *open_gl;
  int            sem_id, shmid,sem_id2, shmid2;
  union semun    arg;
  pid_t          sender_id, receiver_id,master_spy_id,pid,openGL_pid;
  int            i,numOfHelpers,numOfSpies,columns,
                 success_threshold, failing_threshold, 
                 success_counter = 0,failing_counter = 0;
  char           masterPid[10],columnsStr[10],parameterStr[20];
  pid_t           helpers_id[20];
  pid_t           spies_id[20]; 

main(int argc, char *argv[])
{
  pid = getpid();
  
  FILE *parameters = fopen("parameters.txt","r");
  i=0;
  while (fgets(parameterStr,sizeof(parameterStr),parameters))
  {
    switch (i)
    {
    case 0:
      sscanf(parameterStr, "%*[^=]=%d",&numOfHelpers);
      break;
    case 1:
      sscanf(parameterStr, "%*[^=]=%d",&numOfSpies);
      break;
    case 2:
      sscanf(parameterStr, "%*[^=]=%d",&success_threshold);
      break;
    case 3:
      sscanf(parameterStr, "%*[^=]=%d",&failing_threshold);
    default:
      break;
    }
    i++;
  }
  
  if (signal(SIGCONT, resume) == SIG_ERR) // SIG_ERR = -1
  {
    perror("Sigset can not set SIGUSR1");
    exit(SIGUSR1);
  }

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

  //Shared memory creatuin and attachment//////////////////////////////////////////////////////
  shmid = shmget((int)pid, SHM_SIZE * sizeof(SharedMemory), IPC_CREAT | 0666);
  if (shmid < 0)
  {
    perror("shmget parent...");
    exit(1);
  }

  shared_memory = (SharedMemory *)shmat(shmid, NULL, 0);
  if (shared_memory == (SharedMemory *)-1)
  {
    perror("shmat");
    exit(1);
  }

  //Shared memory for openGL;
  shmid2 = shmget(1,sizeof(openGL), IPC_CREAT | 0666);
  if (shmid2 < 0)
  {
    perror("shmget parent... openGL");
    exit(1);
  }

  open_gl = (openGL *)shmat(shmid2, NULL, 0);
  if (open_gl == (openGL *)-1)
  {
    perror("shmat openGL");
    exit(1);
  }

  //Initializing the semaphore/////////////////////////////////////////////////////////////////
  sem_id = semget((int)pid, 2, IPC_CREAT | 0666);
  if (sem_id < 0)
  {
    perror("semget");
    exit(1);
  }
  union semun sem_union;

  //Semaphore for openGL
  sem_union.val = 1;
  semctl(sem_id, 0, SETVAL, sem_union);

  sem_id2 = semget(1, 2, IPC_CREAT | 0666);
  if (sem_id < 0)
  {
    perror("semget openGL");
    exit(1);
  }
  union semun sem_union2;
  sem_union2.val = 1;
  semctl(sem_id2, 0, SETVAL, sem_union2);

    open_gl->messages = 0;
     open_gl->encoded = 0;
     open_gl->spy = 0;
     open_gl->receiver = 0;
     open_gl->thresh1 = 0;
     open_gl->thresh2 = 0;
     open_gl->th1 = success_threshold;
     open_gl->th1 = failing_threshold;

  // forking the needed processes//////////////////////////////////////////////////////////////
  //forking OpenGL///////////////////////////////////////////////////////////////////////////
  if ( (openGL_pid = fork()) == -1 ) {
    perror("fork -- openGL");
    exit(5); 
  }
  else if ( openGL_pid == 0 ) {
    execl("./openGl", "./openGl", (char *) 0);
    perror("execl -- openGL");
    exit(6);
  }
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
  sleep(1);

  if(shared_memory[0].message != NULL){
      columns = atoi(shared_memory[0].message);
  }else{
    printf("error --reading columns");
  }

  sprintf(columnsStr,"%d",columns);

  printf("Columns: %s\n",columnsStr);
 signal_semaphore(sem_id);

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
  printf("reciver: %d\n",(int) receiver_id);
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

  sleep(3);
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
    wait_semaphore(sem_id2);
     open_gl->receiver = 0;
     open_gl->thresh1++;
    signal_semaphore(sem_id2);
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
    kill(sender_id, SIGKILL);

    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0); 
    semctl(sem_id, 0, IPC_RMID, 0);

    shmdt(open_gl);
    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)0);
    semctl(sem_id2, 0, IPC_RMID, 0);
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

    wait_semaphore(sem_id2);
    open_gl->spy = 0;
    open_gl->thresh2++;
    signal_semaphore(sem_id2);
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
    kill(sender_id, SIGKILL);

    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    semctl(sem_id, 0, IPC_RMID, 0);

    shmdt(open_gl);
    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)0);
    semctl(sem_id2, 0, IPC_RMID, 0);
    printf("Processes are terminated\n");

    exit(0);
  }
} 
