
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
    for ( j = 1; j < colLength; j++)
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
    int row,i;
    int col = atoi(columns[0]);
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
int shmid, shmid2;
SharedMemory *shared_memory;
int sem_id,sem_id2;
openGL *open_gl;
int openCounter;

int main(int argc, char *argv[])
{

    if (signal(SIGUSR2, sendsignal) == SIG_ERR) // SIG_ERR = -1
    {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR2);
    }

    printf("im in master\n");

    num_of_colomns = atoi(argv[1]);
    flag = 0;

    isRecived = (int *)calloc(num_of_colomns, sizeof(int)); ////to check if we have recived all the colomns//////////
    if (isRecived == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    pid_t pid = getpid();

    shmid = shmget((int)pid, SHM_SIZE*sizeof(SharedMemory), IPC_CREAT | 0666);
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

    sem_id = semget((int)pid, 2, IPC_CREAT | 0666);
    if (sem_id < 0)
    {
        perror("semget");
        exit(1);
    }
    union semun sem_union;
    sem_union.val = 1;
    semctl(sem_id, 0, SETVAL, sem_union);


    //Accessing the shared memory and semaphores for openGL//////////////////////////////////////////////////////////
    shmid2 = shmget(1, sizeof(openGL), 0);
    if (shmid2 < 0)
    {
        perror("shmget receiver... openGL");
        exit(1);
    }

    open_gl = (openGL *)shmat(shmid2, NULL, 0);
    if (open_gl == (openGL *)-1)
    {
        perror("shmat receiver...");
        exit(1);
    }

    sem_id2 = semget(1, 2, 0);
    if (sem_id2 < 0)
    {
        perror("semget receiver");
        exit(1);
    }

    while (1);

    return 0;
}

void sendsignal(int the_signal)
{
    int i;
    flag = 0;
    while (flag == 0)
    {

        wait_semaphore(sem_id);
        openCounter = 0;
        printf("Master Process: Here what i Recived:\n");
        for (i = 0; i < num_of_colomns; i++)
        {
            if (strlen(shared_memory[i].message) != 0)  /////////check for new messages//////////////////
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
                openCounter++;
            }
        }
        printf("\n");
        signal_semaphore(sem_id);

        wait_semaphore(sem_id2);
        open_gl->spy = openCounter;
        printf("Messages I have: %d\n",open_gl->spy);
        signal_semaphore(sem_id2);
        for ( i = 0; i < num_of_colomns; i++) /////check if we have recived all the colomns//////
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
        sleep(2);
    }
    printf("--------------Master Spy Got all the colomns-------------\n");

    int j,startIndex,finishIndex,wordChar = 1,saveIndex;
    int numberOfLines = 0;
    for (i = 0; i < num_of_colomns; i++)
    {
        for (j = 2; j < MaxWidth; j++)
        {
            switch (columns[i][j])
        {
        case 'A'...'Z':
            columns[i][j] =  ((columns[i][j] - 'A' - (wordChar * (i+1))% 26 + 26) % 26) + 'A';
            wordChar++;
            break;
        case 'a' ... 'z':
            columns[i][j] =  ((columns[i][j] - 'a' - (wordChar *(i+1))% 26 + 26) % 26 ) + 'a';
            wordChar++;
            break;
        case '#':
            startIndex = j;
            do {
                    j++;
            } while (columns[i][j] != '#');
            int num = atoi(&columns[i][startIndex + 1]);
            finishIndex = j + num + 1;
            num = atoi(&columns[i][j+1]);
            char oldNum[15];
            snprintf(oldNum, sizeof(oldNum), "%d", 1000000 - num);
            int oldNumLen = strlen(oldNum);
            memmove(&columns[i][startIndex + oldNumLen], &columns[i][finishIndex + 1], strlen(&columns[i][finishIndex + 1]) + 1);
            memcpy(&columns[i][startIndex], oldNum, oldNumLen);
            j = finishIndex;  
            break;
        case '1':
            columns[i][j] = '!';
            break;
        case '2':
            columns[i][j] = '?';
            break;
        case '3':
            columns[i][j] = ',';
            break;
        case '4':
            columns[i][j] = ';';
            break;
        case '5':
            columns[i][j] = ';';
            break;
        case '6':
           columns[i][j] = '%';
            break;
        case '/':
            wordChar = 1;
            numberOfLines++;
            break;
        default:
            break;
        }
        }
    }
    FILE *file = fopen("spy.txt", "w"); // Open the file in write mode
    char decodedLines[numberOfLines][MaxWidth];
    int currentLine;
    for (i = 0; i <num_of_colomns; i++)
    {
        currentLine = 0;
        char *word = strtok(columns[i], "/");
        word = strtok(NULL, "/");
        while (word != NULL)
        {
            if(i == 0 ){
            strcpy(decodedLines[currentLine],"");
            if(strcmp(word,"alright") != 0)
                sprintf(decodedLines[currentLine],"%s", word);
            }else{
                if(strcmp(word,"alright") != 0)
                    sprintf(decodedLines[currentLine],"%s %s",decodedLines[currentLine],word);
            }
            word = strtok(NULL, "/");
            currentLine++;
        }
    }
    for (i = 0; i < currentLine; i++)
    {
        fprintf(file,"%s\n", decodedLines[i]);
    }    
    fclose(file); // Close the file
    for (i = 0; i < num_of_colomns; i++) /////// erase the previous results //////////
    {
        isRecived[i] = 0;
        strncpy(shared_memory[i].message, "", MaxWidth);
        strncpy(columns[i], "", MaxWidth);
    }
    kill(getppid(), SIGUSR2);///////send signal to parent that decoding is done /////////////////////
}