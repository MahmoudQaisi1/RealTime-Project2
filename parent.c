
#include "local.h"




main(int argc, char *argv[])
{
  static struct  MEMORY memory;
  static ushort  start_val[2] = {N_SLOTS, 0};
  int            semid, shmid, croaker;
  char          *shmptr;
  union semun    arg;
  pid_t          sender_id, receiver_id,master_spy_id, pid = getpid();
  int            i,numOfHelpers,numOfSpies;
  int            success_threshold = 2;
  int            failing_threshold = 2;
  int            success_counter = 0;
  int            failing_counter = 0;


  if ( argc != 3 ) {
    fprintf(stderr, "%s Number_of_helpers number_of_spies \n", argv[0]);
    exit(-1);
  }

  numOfHelpers = atoi(argv[1]);
  numOfSpies = atoi(argv[2]);

  pid_t           helpers_id[numOfHelpers];
  pid_t           spies_id[numOfSpies];


  //Shared memory creatuin and attachment//////////////////////////////////////////////////////
  if ( (shmid = shmget((int) pid, sizeof(memory),
		       IPC_CREAT | 0600)) != -1 ) {
    
    if ( (shmptr = (char *) shmat(shmid, 0, 0)) == (char *) -1 ) {
      perror("shmptr -- parent -- attach");
      exit(1);
    }
    memcpy(shmptr, (char *) &memory, sizeof(memory));
  }
  else {
    perror("shmid -- parent -- creation");
    exit(2);
  }

  //Initializing the semaphore/////////////////////////////////////////////////////////////////
  if ( (semid = semget((int) pid, 2,
		       IPC_CREAT | 0666)) != -1 ) {
    arg.array = start_val;
    
    if ( semctl(semid, 0, SETALL, arg) == -1 ) {
      perror("semctl -- parent -- initialization");
      exit(3);
    }
  }
  else {
    perror("semget -- parent -- creation");
    exit(4);
  }

  // forking the needed processes//////////////////////////////////////////////////////////////
  //sender////////
  if ( (sender_id = fork()) == -1 ) {
    perror("fork -- sender");
    exit(5); 
  }
  else if ( sender_id == 0 ) {
    execl("./sender", "./sender", argv[1], (char *) 0);
    perror("execl -- sender");
    exit(6);
  }
  sleep(0.5);



  //receiver//////
  if ( (receiver_id = fork()) == -1 ) {
    perror("fork -- receiver");
    exit(5); 
  }
  else if ( receiver_id == 0 ) {
    execl("./receiver", "./receiver", argv[1], (char *) 0);
    perror("execl -- receiver");
    exit(6);
  }
  sleep(0.5);

  // //master Spy/////
  // if ( (master_spy_id = fork()) == -1 ) {
  //   perror("fork -- master spy");
  //   exit(5); 
  // }
  // else if ( master_spy_id == 0 ) {
  //   execl("./master_spy", "./master_spy", argv[1], (char *) 0);
  //   perror("execl -- master spy");
  //   exit(6);
  // }
  // sleep(0.5);

  // //helpers////////////////////
  // for ( i = 0; i < numOfHelpers; i++)
  // {
  //   if ( (helpers_id[i] = fork()) == -1 ) {
  //     perror("fork -- helper");
  //     exit(5); 
  //   }
  //   else if ( helpers_id[i] == 0 ) {
  //     execl("./helper", "./helper", (char *) 0);
  //     perror("execl -- helper");
  //     exit(6);
  //   }
  //   sleep(0.5);
  // }

  // //spies///////////////////
  // for ( i = 0; i < numOfSpies; i++)
  // {
  //   if ( (spies_id[i] = fork()) == -1 ) {
  //     perror("fork -- spy");
  //     exit(5); 
  //   }
  //   else if ( spies_id[i] == 0 ) {
  //     execl("./spy", "./spy", argv[1], (char *) 0);
  //     perror("execl -- spy");
  //     exit(6);
  //   }
  //   sleep(0.5);
  // }

  //Clean House////////////////////////////////////////////////////////////////////////////////
  
  sleep(5);
  croaker = (int) wait((int *) 0); /* wait for 1 to die */
  kill( receiver_id,SIGKILL);
  //remove the shm/////////
  shmdt(shmptr);
  shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0); /* remove */
  semctl(semid, 0, IPC_RMID, 0);

  exit(0);
}