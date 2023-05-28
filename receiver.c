#include "local.h"




main(int argc, char *argv[])
{
    static char    local_buffer[SLOT_LEN];
    int            i, sleep_limit, semid, shmid;
    pid_t          ppid = getppid();
    char          *shmptr;
    struct MEMORY *memptr;

    if ( (shmid = shmget((int) ppid, 0, 0)) != -1 ) {
    if ( (shmptr = (char *) shmat(shmid, (char *)0, 0)) 
	 == (char *) -1 ) {
      perror("shmat -- receiver -- attach");
      exit(1);
    }
    memptr = (struct MEMORY *) shmptr;
    }
    else {
        perror("shmget -- receiver -- access");
        exit(2);
    }
  
  /*
   * Access the semaphore set
   */
    if ( (semid = semget((int) ppid, 2, 0)) == -1 ) {
        perror("semget -- receiver -- access");
        exit(3);
    }

    while ( 1 ) {
    acquire.sem_num = TO_CONSUME;
    if ( semop(semid, &acquire, 1) == -1 ) {
      perror("semop -- receiver -- acquire");
      exit(4);
    }
    memset(local_buffer, '\0', sizeof(local_buffer));
    strcpy(local_buffer, memptr->buffer[memptr->head]);
    printf("C: [%d] %s.\n", memptr->head, local_buffer);
    memptr->head = (memptr->head + 1) % N_SLOTS;
    release.sem_num = AVAIL_SLOTS;
    if ( semop(semid, &release, 1) == -1 ) {
      perror("semop -- receiver -- release");
      exit(5);
    }
    sleep(2);
  }
    exit(0);
}