#include "local.h"



main(int argc, char *argv[])
{
    static struct  MEMORY memory;
    struct MEMORY *memptr;
    static ushort  start_val[2] = {N_SLOTS, 0};
    int            semid, shmid, croaker;
    char          *shmptr;
    union semun    arg;
    pid_t          pid=getpid();
    char line[1024];
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
    char array[maxWords][1024];
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
    sleep(0.5);
    }
    exit(0);
}