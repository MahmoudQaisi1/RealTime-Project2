#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#include "local.h"

int isStringInteger(char *str)
{
    if (str == NULL || *str == '\0')
    {
        return 0;
    }
    int i = 0;
    if (str[i] == '+' || str[i] == '-')
    {
        i++;
    }
    if (!isdigit(str[i]))
    {
        return 0;
    }
    // Check for remaining digits
    i++;
    while (str[i] != '\0')
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
        i++;
    }
    return 1;
}
// Function to split a string into words based on a delimiter
char **splitString(char *str, const char *delimiter, int *wordCount)
{
    char *token;
    char **words = NULL;
    int count = 0;

    token = strtok(str, delimiter);
    while (token != NULL)
    {
        count++;
        words = realloc(words, sizeof(char *) * count);
        words[count - 1] = token;
        token = strtok(NULL, delimiter);
    }

    *wordCount = count;
    return words;
}
void replaceStrings(char columns[MaxLength][MaxWidth], int colLength)
{
    for (int j = 1; j < colLength; j++)
    {

        if (strcmp(columns[j], "alright") == 0)
        {
            strcpy(columns[j], "");
        }
        else if (isStringInteger(columns[j]) == 1)
        {
            int num;
            char *endptr;
            num = atoi(columns[j]);

            if (num == 1)
                strcpy(columns[j], "!");
            else if (num == 2)
                strcpy(columns[j], "?");
            else if (num == 3)
                strcpy(columns[j], ",");
            else if (num == 4)
                strcpy(columns[j], ";");
            else if (num == 5)
                strcpy(columns[j], ":");
            else if (num == 6)
                strcpy(columns[j], "%");
            else
            {
                num = 1000000 - num;
                char str[150];
                sprintf(str, "%d", num);
                strcpy(columns[j], str);
            }
        }
    }
}

void decodeMessage(char columns[MaxLength][MaxWidth], int colLength)
{
    int col = atoi(columns[0]);
    for (int row = 1; row < colLength; row++)
    {
        char *word = columns[row];
        int len = strlen(word);
        if (isStringInteger(word) == 1)
            continue;
        for (int i = 0; i < len; i++)
        {
            if (word[i] == '1')
                word[i] = '!';
            else if (word[i] == '2')
                word[i] = '?';
            else if (word[i] == '3')
                word[i] = ',';
            else if (word[i] == '4')
                word[i] = ';';
            else if (word[i] == '5')
                word[i] = ':';
            else if (word[i] == '6')
                word[i] = '%';
            else if (isalpha(word[i]))
            {
                word[i] -= (col) * (i + 1);
                if (word[i] < 'a')
                    word[i] += 26;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    printf("im in master\n");

    int num_of_colomns = atoi(argv[1]);
    int flag = 0;
    char columns[num_of_colomns][MaxWidth];

    int *isRecived = (int *)calloc(num_of_colomns, sizeof(int));
    if (isRecived == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    memset(isRecived, 0, num_of_colomns * sizeof(int));

    pid_t pid = getpid();

    int shmid = shmget((int)pid, SHM_SIZE, IPC_CREAT | 0666);
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

    int sem_id = semget((int)pid, 1, IPC_CREAT | 0666);
    if (sem_id < 0)
    {
        perror("semget");
        exit(1);
    }

    union semun sem_union;
    sem_union.val = 1;
    semctl(sem_id, 0, SETVAL, sem_union);

    int i;
    for (i = 0; i < num_of_colomns; i++)
    {
        strncpy(shared_memory[i].message, "-1/", MaxWidth - 1);
    }

    while (flag == 0)
    {

        wait_semaphore(sem_id);

        printf("Master Process: Here what i Recived:\n");
        for (i = 0; i < num_of_colomns; i++)
        {
            char Tstr[MaxWidth];
            strncpy(Tstr, shared_memory[i].message, MaxWidth - 1);
            char *snum;
            int num;
            snum = strtok(Tstr, "/");
            if (snum != NULL)
            {
                num = atoi(snum);
                if (-1 < num && num < num_of_colomns + 1)
                {
                    isRecived[num - 1] = 1;
                    printf("Message %d  ", num);
                    strncpy(columns[i], shared_memory[i].message, MaxWidth - 1);
                }
                
            }
        }
        printf("\n");
        signal_semaphore(sem_id);
        for (int i = 0; i < num_of_colomns; i++)
        {
            if (isRecived[i] == 0)
            {
                flag = 0;
                break;
            }
            else{
                flag = 1;
            }
        }
        sleep(0.5);
    }
    printf("--------------Master Spy Got all the colomns-------------\n");

int colLength = 0;
char tempString[MaxWidth];
strncpy(tempString, columns[0], MaxWidth);

char *token = strtok(tempString, "/"); // Assuming space as the word delimiter
while (token != NULL)
{
    colLength++;
    token = strtok(NULL, "/");
}
char allwords[num_of_colomns][colLength][MaxWidth];

for (int i = 0; i < num_of_colomns; i++)
{
    int wordCount = 0;
    char tempString[MaxWidth];
    strncpy(tempString, columns[i], MaxWidth);

    char **words = splitString(tempString, "/", &wordCount);

    for (int j = 0; j < wordCount; j++)
    {
        strncpy(allwords[i][j], words[j], MaxWidth);
    }

    free(words);
}

for (int r = 0; r < num_of_colomns; r++)
{
    replaceStrings(allwords[r], colLength);
    decodeMessage(allwords[r], colLength);
}

FILE *file = fopen("spy.txt", "w"); // Open the file in write mode
if (file == NULL)
{
    printf("Error opening the file.\n");
    return 1;
}
for (int row = 1; row < num_of_colomns; row++)
{
    for (int col = 0; col < colLength; col++)
    {
        fprintf(file, "%s ", allwords[col][row]); // Write to the file
    }
    fprintf(file, "\n"); // Write a new line to the file
}

fclose(file); // Close the file
free(isRecived);

shmdt(shared_memory);
shmctl(shmid, IPC_RMID, (struct shmid_ds *)0); /* remove */
semctl(sem_id, 0, IPC_RMID, 0);
exit(0); // Terminate the child process

return 0;
}
