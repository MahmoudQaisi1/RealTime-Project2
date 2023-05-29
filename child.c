#include "local.h"



main(int argc, char *argv[])
{
    int semid, shmid;
    SharedMemory *shared_memory;
    int i, column, wordChar = 1;
    pid_t parent_pid = (pid_t) atoi(argv[1]);
    char message[1024];
    strcpy(message,argv[2]);
    
    int startIndex,finishIndex, number;
    char *token = strtok(message, "/");
    column = atoi(token);
    strcpy(message,argv[2]);
    int nullChar;


    shmid = shmget((int)parent_pid, SHM_SIZE, 0);
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

    int sem_id = semget((int)parent_pid, 1, 0);
    if (sem_id < 0)
    {
        perror("semget");
        exit(1);
    }
    for (i = 2; i < 1024; i++)
    {
        if(message[i] == '\0'){
            nullChar = i;
            break;
        }
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

    wait_semaphore(sem_id);
    strncpy(shared_memory[column].message,message,nullChar);
    printf("%s\n",shared_memory[column].message);
    signal_semaphore(sem_id);


    return 0;
}

