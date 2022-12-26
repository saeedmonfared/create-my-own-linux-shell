#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


//global variables
char current_directory[1000];
char pwd[1024];
char input_buffer[1024];
int flag , len;
char *cmd_exec[100];
int pipe_count = 0;
static char* args[512];
pid_t pid;
int history_num = 0;

//define
#define BUFFER_SIZE 1000;

//functions header
void prompt();
void clearVar();
void whit_pipe_execute();
static int split(char *cmd_exec, int input, int first, int last);
void change_directory();
void sigintHandler(int sig_num);
//...........

int main(){
    sleep(1);
    char ch[2] = "\n";
    getcwd(current_directory, sizeof(current_directory));
    //signal(SIGINT, sigintHandler);
    system("clear");
    while(true){
        clearVar();
        prompt();
        fgets(input_buffer, 1024, stdin);
        if(strcmp(input_buffer, ch)==0)
        {
            continue;
        }
        len = strlen(input_buffer);
        input_buffer[len-1] = '\0';
        if(strcmp(input_buffer, "exit") == 0)
        {
            flag = 1;
            break;
        }
        whit_pipe_execute();
    }
    if(flag == 1){
        printf("Bye...\n");
        sleep(1);
        system("clear");
        //exit(0);
        return 0;
    }
    return 0;
}

void sigintHandler(int sig_num){
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}

void whit_pipe_execute(){
    int n = 1 , input = 0, first = 1, i;
    cmd_exec[0] = strtok(input_buffer, "|");

    while((cmd_exec[n] = strtok(NULL , "|"))) n++;

    cmd_exec[n] = NULL;
    pipe_count = n-1;
    for(i = 0; i < n-1; i++){
        input = split(cmd_exec[i], input, first, 0);
        first=0;
    }
    input = split(cmd_exec[i], input, first, 1);
    input=0;
    return;
}

static int split(char *cmd_exec, int input, int first, int last){
    char *new_exec;
    new_exec = strdup(cmd_exec);
    int status;

    int m = 1;
    args[0] = strtok(new_exec, " ");
    while((args[m] = strtok(NULL, " "))) m++;
    args[m] = NULL;
    if(args[0]){
        if(strcmp(args[0], "cd") == 0){
            change_directory();
            return 1;
        }
        else if((pid = fork()) == 0){
            //bakhsh marboot be execvp
            status = execvp(args[0], args);
            if(status == -1){
                fprintf(stderr,"%s: command not found\n", args[0]);
                exit(1);
            }
        }
        wait(NULL);
        if(status != -1){
            FILE *history_file = fopen("/home/saeed/Desktop/history", "w");
            if(history_file == NULL) {
                fprintf(stderr, "error: could not open the file\n");
                return 1;
            }
            else{
                history_num++;
                fprintf(history_file,"%d- %s\n", history_num, cmd_exec);
            }
            fclose(history_file);
        }

    }
    return 1;
}

void change_directory(){
    char *home = "/home";
    if(args[1]==NULL)
        chdir(home);
    else if ((strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0))
        chdir(home);
    else{
        int check = chdir(args[1]);
        if(check == 0){
            system("clear");
        }
        else{
            printf("bash: cd: %s: No such file or directory\n", args[1]);
        }
    }

}

void prompt(){
    char shell[1000];
    if(getcwd(pwd, sizeof(pwd)) != NULL){
        strcpy(shell , "saeed_and_behnam shell:");
        strcat(shell, pwd);
        strcat(shell, "$ ");
        printf("%s", shell);
    }
    else{
        perror("getcwd() error");
    }
}

void clearVar(){
    flag = 0;
    len = 0;
    pwd[0] = '\0';
    input_buffer[0] = '\0';
    pipe_count = 0;
    pid=0;
}
