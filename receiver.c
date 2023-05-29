
#include "local.h"

int isStringInteger(char *str)      //////// check if the string contain an integer///////////
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

// Function to split a string into words based on a delimiter/////////
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

// Decode Function to decode special words/////////
void replaceStrings(char columns[MaxLength][MaxWidth], int colLength)
{
    int j;
    for (j = 1; j < colLength; j++)
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

// Decode Function to decode noraml words/////////
void decodeMessage(char columns[MaxLength][MaxWidth], int colLength)
{
    int col = atoi(columns[0]);
    int row,i;
    for (row = 1; row < colLength; row++)
    {
        char *word = columns[row];
        int len = strlen(word);
        if (isStringInteger(word) == 1)
            continue;
        for (i = 0; i < len; i++)
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

void sendsignal(int);

int num_of_colomns;
char columns[MaxLength][MaxWidth];

int flag;
int *isRecived;
int shmid;
SharedMemory *shared_memory;
int sem_id;

int main(int argc, char *argv[])
{
    if (signal(SIGUSR1, sendsignal) == SIG_ERR) // SIG_ERR = -1
    {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR1);
    }

    printf("im in receiver\n");

    num_of_colomns = atoi(argv[1]);

    flag = 0;

    srand((unsigned)getpid());

    isRecived = (int *)calloc(num_of_colomns, sizeof(int)); ////to check if we have recived all the colomns//////////
    if (isRecived == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    memset(isRecived, 0, num_of_colomns * sizeof(int));

    pid_t ppid = getppid();

    shmid = shmget((int)ppid, SHM_SIZE, 0);
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

    sem_id = semget((int)ppid, 1, 0);
    if (sem_id < 0)
    {
        perror("semget");
        exit(1);
    }

    while (1);

    return 0;
}

void sendsignal(int the_signal)
{
    flag = 0;
    while (flag == 0)
    {
        wait_semaphore(sem_id);
        int j;
        int random_index = rand() % num_of_colomns;

        if (strlen(shared_memory[random_index].message) != 0) /////read from the shared memory at a random index//////
        {

            char Tstr[MaxWidth];
            strncpy(Tstr, shared_memory[random_index].message, MaxWidth - 1);

            char *snum;
            int num;
            snum = strtok(Tstr, "/");
            if (snum != NULL)
            {
                num = atoi(snum);
                isRecived[num - 1] = 1;
                int i = num - 1;
                printf("~~receiver Process: I got Message %d at index %d~~\n", i + 1, random_index + 1);
                strncpy(columns[i], shared_memory[random_index].message, MaxWidth - 1);
            }
        }
        else
        {
            printf("~~receiver Process: Message at index %d is empty~~\n", random_index + 1);
        }
        signal_semaphore(sem_id);
        int i;
        for (i = 0; i < num_of_colomns; i++) /////check if we have recived all the colomns//////
        {
            if (isRecived[i] == 0)
            {
                flag = 0;
                break;
            }
            else
            {
                flag = 1;
            }
        }
        usleep(500);
    }
    printf("++++++++++++++Receiver Got all the colomns+++++++++++++++\n");

    /////decode the colomns//////////////////////////////////////////////////////
    int colLength = 0;
    char tempString[MaxWidth];
    strncpy(tempString, columns[0], MaxWidth);

    char *token = strtok(tempString, "/"); // find the colomn length////////////
    while (token != NULL)
    {
        colLength++;
        token = strtok(NULL, "/");
    }
    char allwords[num_of_colomns][colLength][MaxWidth]; // split the colomn string inside an array////////////
    int i,j,r,row,col;
    for ( i = 0; i < num_of_colomns; i++)
    {
        int wordCount = 0;
        char tempString[MaxWidth];
        strncpy(tempString, columns[i], MaxWidth);

        char **words = splitString(tempString, "/", &wordCount);

        for (j = 0; j < wordCount; j++)
        {
            strncpy(allwords[i][j], words[j], MaxWidth);
        }

        free(words);
    }

    for (r = 0; r < num_of_colomns; r++)
    {
        replaceStrings(allwords[r], colLength);
        decodeMessage(allwords[r], colLength);
    }
    FILE *file = fopen("receiver.txt", "w"); // Open the file in write mode
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        exit(4);
    }
    for (row = 1; row < num_of_colomns; row++)
    {
        for (col = 0; col < colLength; col++)
        {
            fprintf(file, "%s ", allwords[col][row]); // Write to the file
        }
        fprintf(file, "\n");
    }

    fclose(file); // Close the file
    for ( i = 0; i < num_of_colomns; i++) /////// erase the previous results //////////
    {
        isRecived[i] = 0;
        strncpy(columns[i], "", MaxWidth);
    }
    kill(getppid(), SIGUSR1); ///////send signal to parent that decoding is done /////////////////////
}