#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

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
#define BUFFER_SIZE 1000

#define KNRM "\x1B[0m"
#define KRED "\x1B[1;31m"
#define KGRN "\x1B[1;32m"
#define KBLU "\x1B[1;34m"

//functions header
void prompt();
void clearVar();
void whit_pipe_execute();
static int with_space_execute(char *cmd_exec, int input, int first, int last);
void change_directory();
void sigintHandler(int sig_num);
void takeInput(char* str);
void history_file();
//...........

int main(){
    sleep(1);
    char ch[2] = "\n";

    signal(SIGINT, sigintHandler);
    system("clear");
    while(true){
        clearVar();
        prompt();
        //fgets(input_buffer, 1024, stdin);
        takeInput(input_buffer);
        history_file();
        if(strcmp(input_buffer, ch)==0)
        {
            continue;
        }
        if(strcmp(input_buffer, "exit") == 0)
        {
            flag = 1;
            break;
        }
        whit_pipe_execute();
    }
    if(flag == 1){
        printf("%sBye...%s\n",KGRN, KNRM);
        sleep(1);
        system("clear");
        //exit(0);
        return 0;
    }
    return 0;
}
//echo hello >> h.txt
void history_file(){
    char his[1000];
    strcpy(his, "echo ");
    strcat(his, input_buffer);
    strcat(his, ">> h.txt");
    system(his);
}
void takeInput(char* str){
    char* buf;
    buf = readline("\n>");
    if (strlen(buf) != 0) {
		add_history(buf);
		strcpy(str, buf);
	}
}

void sigintHandler(int sig_num){
    signal(SIGINT, sigintHandler);
    fflush(stdout);
    //prompt();
    //takeInput(input_buffer);
}

void whit_pipe_execute(){
    int n = 1 , input = 0, first = 1, i;
    cmd_exec[0] = strtok(input_buffer, "|");

    while((cmd_exec[n] = strtok(NULL , "|"))) n++;

    cmd_exec[n] = NULL;
    pipe_count = n-1;
    for(i = 0; i < n-1; i++){
        input = with_space_execute(cmd_exec[i], input, first, 0);
        first=0;
    }
    input = with_space_execute(cmd_exec[i], input, first, 1);
    input=0;
    return;
}

static int with_space_execute(char *cmd_exec, int input, int first, int last){
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
            if(strcmp(args[0], "a") == 0){
                char buf[100] = "cut -d ' ' -f1 ";
                strcat(buf,args[1]);
                system(buf);
                //printf("\n");
                //exit(1);
            }
            else if(strcmp(args[0], "b") == 0){
                char buf[100] = "tr -cs A-Za-z '\n' < ";
                strcat(buf, args[1]);
                strcat(buf, " | tr A-Z a-z | sort | uniq -c | sort -rn | head -1 | tr -d ' ' | tr -d '[:digit:]'");
                system(buf);
                //exit(1);
            }
            else if(strcmp(args[0], "c") == 0){
                char buf[100] = "a=$(tr -d \"[:space:]\" < ";
                strcat(buf, args[1]);
                strcat(buf, ")\n echo $a > ");
                strcat(buf, args[1]);
                strcat(buf, "\n cat ");
                strcat(buf, args[1]);
                system(buf);
                //exit(1);
            }
            else if(strcmp(args[0], "d") == 0){
                char buf[100] = "grep -v '^#' ";
                strcat(buf, args[1]);
                system(buf);
            }
            else if(strcmp(args[0], "f") == 0){
                char buf[100] = "wc -l < ";
                strcat(buf, args[1]);
                system(buf);
            }
            else if(strcmp(args[0], "g") == 0){
                char buf[100] = "head -10 ";
                strcat(buf, args[1]);
                system(buf);
            }
            else{
                //bakhsh marboot be execvp
                status = execvp(args[0], args);
                if(status == -1){
                    fprintf(stderr,"%s: %scommand not found%s\n", args[0], KRED, KNRM);
                    //exit(1);
                }
            }
            exit(1);
        }
        wait(NULL);
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
            printf("bash: cd: %s: %sNo such file or directory%s\n", args[1], KRED, KNRM);
        }
    }

}

void prompt(){
    char shell[1000];
    if(getcwd(pwd, sizeof(pwd)) != NULL){
        strcpy(shell , "saeed_and_behnam shell:");
        //printf("%ssaeed@behnam_shell:%s",KGRN,KNRM);
        strcat(shell, pwd);
        //strcat(shell, "$ ");
        printf("%s%s%s$", KBLU, shell, KNRM);
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
