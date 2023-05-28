#include "local.h"



main(int argc, char *argv[])
{
    int semid, shmid;
    char          *shmptr;
    struct MEMORY *memptr;
    int i,column, wordChar = 1;
    pid_t parent_pid = (pid_t) atoi(argv[1]);
    char message[1024];
    strcpy(message,argv[2]);

    if ( (shmid = shmget((int) parent_pid, 0, 0)) != -1 ) {
    if ( (shmptr = (char *) shmat(shmid, (char *)0, 0)) 
	 == (char *) -1 ) {
      perror("shmat -- consumer -- attach");
      exit(1);
    }
    memptr = (struct MEMORY *) shmptr;
  }
  else {
    perror("shmget -- consumer -- access");
    exit(2);
  }
  
  /*
   * Access the semaphore set
   */
  if ( (semid = semget((int) parent_pid, 2, 0)) == -1 ) {
    perror("semget -- consumer -- access");
    exit(3);
  }
    
    int startIndex,finishIndex, number;
    char *token = strtok(message, "/");
    column = atoi(token);
    strcpy(message,argv[2]);
    printf("this is message: %s\n",message);
    for (i = 2; i < 1024; i++)
    {
        switch (message[i])
        {
        case 'A'...'Z':
            message[i] =  ((message[i] - 'A' + (wordChar * column)) % 26) + 'A';
            wordChar++;
            break;
        case 'a' ... 'z':
            message[i] =  ((message[i] - 'a' + (wordChar * column)) % 26) + 'a';
            wordChar++;
            break;
        case '0'...'9':
            startIndex = i;
            while (i < strlen(message) && message[i + 1] >= '0' && message[i + 1] <= '9') {
                    i++;
            } 
            finishIndex = i;
            
            int num = atoi(&message[startIndex]);
            char newNum[10];
            snprintf(newNum, sizeof(newNum), "%d", 1000000 - num);
            int newNumLen = strlen(newNum);
            memmove(&message[startIndex + newNumLen], &message[finishIndex + 1], strlen(&message[finishIndex + 1]) + 1);
            memcpy(&message[startIndex], newNum, newNumLen);
            i = startIndex + newNumLen - 1;   
            break;
        case '!':
            message[i] = '1';
            break;
        case '?':
            message[i] = '2';
            break;
        case ',':
            message[i] = '3';
            break;
        case ';':
            message[i] = '4';
            break;
        case ':':
            message[i] = '5';
            break;
        case '%':
            message[i] = '6';
            break;
        case '/':
            wordChar = 1;
            break;
        default:
            break;
        }
    }
    
    srand((unsigned)getpid());
    sleep(rand()%10 +1);
    acquire.sem_num = AVAIL_SLOTS;
    
    if ( semop(semid, &acquire, 1) == -1 ) {
      perror("semop -- child -- acquire");
      exit(4);
    }
    strcpy(memptr->buffer[memptr->tail], message);
    printf("coded: [%d] %s.\n", memptr->tail, memptr->buffer[memptr->tail]);
    
    memptr->tail = (memptr->tail + 1) % N_SLOTS;

    release.sem_num = TO_CONSUME;
    
    if ( semop(semid, &release, 1) == -1 ) {
      perror("semop -- child -- release");
      exit(5);
    }

    exit(0);
}

