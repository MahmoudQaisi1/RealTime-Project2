#include "local.h"



main(int argc, char *argv[])
{
    pid_t          pid=getpid(),ppid = getppid();
    char line[512];
    int maxWords = 0;
    int currentWords = 0;
    int numLines = 0;
    int i,j;

    FILE *file = fopen("sender.txt", "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

//get the number of columns and the number of lines///////////////////////////////////////////////////////////////
    while (fgets(line, sizeof(line), file) != NULL) {
        numLines++;
        currentWords = 0;
        char *token = strtok(line, " ");
        while (token != NULL) {
            currentWords++;
            token = strtok(NULL, " ");
        }
        if (currentWords > maxWords) {
            maxWords = currentWords;
        }
    }
    fclose(file);
    
//Spilt into columns///////////////////////////////////////////////////////////////
    file = fopen("sender.txt", "r");
    char array[maxWords][512];
    int colmnIndex = 0;

    for ( i = 0; i < maxWords; i++)
    {
        sprintf(array[i],"%d/",i+1);
    }
    // Read the file line by line
    while (fgets(line, 1024, file) != NULL) {
        colmnIndex = 0;
        line[strcspn(line, "\n")] = '\0'; // Remove the newline character from the line
        // Tokenize the line into words
        char *word = strtok(line, " ");
        while (word != NULL)
        {
            sprintf(array[colmnIndex],"%s%s/",array[colmnIndex],word);
            word = strtok(NULL, " ");
            colmnIndex++;
        }

        while (colmnIndex < maxWords)
        {
            sprintf(array[colmnIndex],"%salright/",array[colmnIndex],word);
            colmnIndex++;
        }
    }
    fclose(file);

    //Accessing the shared memory and semaphores//////////////////////////////////////////////////////////
    int shmid = shmget((int)ppid, SHM_SIZE, 0);
    if (shmid < 0)
    {
        perror("shmget sender...");
        exit(1);
    }

    SharedMemory *shared_memory = (SharedMemory *)shmat(shmid, NULL, 0);
    if (shared_memory == (SharedMemory *)-1)
    {
        perror("shmat sender...");
        exit(1);
    }

    int sem_id = semget((int)ppid, 2, 0);
    if (sem_id < 0)
    {
        perror("semget");
        exit(1);
    }


    //Accessing the shared memory and semaphores for openGL//////////////////////////////////////////////////////////
    int shmid2 = shmget(1, sizeof(openGL), 0);
    if (shmid2 < 0)
    {
        perror("shmget sender... openGL");
        exit(1);
    }

    openGL *open_gl = (openGL *)shmat(shmid2, NULL, 0);
    if (open_gl == (openGL *)-1)
    {
        perror("shmat sender...");
        exit(1);
    }

    int sem_id2 = semget(1, 2, 0);
    if (sem_id2 < 0)
    {
        perror("semget");
        exit(1);
    }
    // forking the needed children processes//////////////////////////////////////////////////////////////
    pid_t child[maxWords];
    char arg1[10];
    sprintf(arg1,"%d",(int)getppid());
    for (i = 0; i < maxWords; i++)
    {     
        if ( (child[i] = fork()) == -1 ) {
            perror("fork -- child");
            exit(5); 
        }
        else if ( child[i] == 0 ) {
            execl("./child", "./child",arg1,array[i], (char *) 0);
            perror("execl -- child");
            exit(6);
        }
        wait_semaphore(sem_id2);
        open_gl->messages++;
        signal_semaphore(sem_id2);
    sleep(2);
    }
    sleep(2);
    //Waking up parent and sending the number of columns needed///////////////////////////////////////////
    char columnStr[10];
    sprintf(columnStr,"%d",maxWords);
    strcpy(shared_memory[0].message,columnStr);
    printf("Done coding the message...\n");
    kill((pid_t) ppid,SIGCONT);
    while (1);//waiting for more instructions...
    
}