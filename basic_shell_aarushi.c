#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <termios.h>
#include <stdio.h>


char *abc[1000];
const int READ = 0;
const int WRITE = 1;

void BREAKONSPACE(char str[])
{
    char x[1000][1000];
    int i,g=0;
    char * token2;
    int lidx=0,xidx=0;
    g=0;
    int ii=0;
    while(str[ii] == ' ')
        ii++;
    lidx = ii;
    bool f = false;
    for(i=ii;i<strlen(str);i++)
    {
        if(str[i] == ' ')
        {

            int j;
            bool f = false;
            for(j=i;j<strlen(str);j++)
            {
                if(str[j] != ' ')
                    break;
                if(j == strlen(str)-1)
                {
                    f = true;
                    break;
                }
            }
            if(f)
                break;

            for(lidx;lidx<i;lidx++)
                x[g][xidx++] = str[lidx];
            x[g][xidx] = '\0';
            g++;
            while(i < strlen(str) && str[i] == ' ')
                i++;
            i--;
            xidx=0;
            lidx = i+1;
        }
    }
    if(!f)
    {
        for(lidx;lidx<i;lidx++)
            x[g][xidx++] = str[lidx];
        x[g][xidx] = '\0';
        g++;
    }
    for(i=0;i<g;i++)
    {
        abc[i] = (char *)malloc(strlen(x[i]));
        strcpy(abc[i],x[i]);
    }
    abc[g] = NULL;
}

static int command(int input, int first, int last)
{
    int pipettes[2];
 
    pid_t pid;
    /* Invoke pipe */
    pipe( pipettes );   
    pid = fork();
 
    /*
     SCHEME:
        STDIN --> O --> O --> O --> STDOUT
    */
 
    if (pid == 0) {
        if (first == 1 && last == 0 && input == 0) {
            // First command
            dup2( pipettes[WRITE], STDOUT_FILENO );
        } else if (first == 0 && last == 0 && input != 0) {
            // Middle command
            dup2(input, STDIN_FILENO);
            dup2(pipettes[WRITE], STDOUT_FILENO);
        } else {
            // Last command
            dup2( input, STDIN_FILENO );
        }
 
        if (execvp( abc[0], abc) == -1)
        {
            fprintf(stderr, "Erroraneous statement\n" );
            _exit(EXIT_FAILURE); // If child fails
        }
    }
 
    if (input != 0) 
        close(input);
 
    // Nothing more needs to be written
    close(pipettes[WRITE]);
 
    // If it's the last command, nothing more needs to be read
    if (last == 1)
        close(pipettes[READ]);
 
    return pipettes[READ];
}

char history[1000][1000];

void x(){}


int lsh_cd()
{
  if (abc[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(abc[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int main()
{
    signal(SIGINT,x);
    int his=0;
    while(1)
    {
        char curr[1000];
        getcwd(curr,sizeof(curr));
        printf("Aarushi@Aarushi-Lapptop:~%s$ ",curr);
        fflush(stdout);
        int i;
        char * token;
        int g=0;
        char input[1000],store[1000][1000];
        if(fgets(input,1000,stdin) != NULL)
        {
            for(i=0;i<strlen(input);i++)
            {
                if(input[i] == '\n')
                {
                    input[i] = '\0';
                    break;
                }
            }

            if(strcmp(input,"help") == 0)
            {
                printf(" cd\n help\n history\n exit\n");
                continue;
            }

            BREAKONSPACE(input);

            if(strcmp(abc[0],"cd") == 0)
            {
                lsh_cd();
                continue;
            }


            if(strcmp(input,"") == 0)
                continue;
            strcpy(history[his++],input);
            if(strcmp(input,"exit") == 0)
            {
                printf("Exiting the shell!\n\n\n");
                exit(0);
            }
            // printf("%s\n",input);
            if(strcmp(input,"history") == 0)
            {
                for(i=0;i<his;i++)
                    printf(" %s\n",history[i]);
                continue;
            }

            // printf("%s\n",input );
            token = strtok (input,"|");
              while (token!= NULL)
              {
                printf ("%s\n",token);
                strcpy(store[g++],token);
                token = strtok (NULL, "|");
              }
        }
        int INPUT=0,first=1,last=0;
        for(i=0;i<g-1;i++)
        {
            BREAKONSPACE(store[i]);
            INPUT = command(INPUT,first,last);
            first = 0;
        }
        last = 1;
        BREAKONSPACE(store[i]);
        INPUT = command(INPUT,first,last);

        for(i=0;i<50;i++)
            wait(NULL);
    }
    return 0;
}
