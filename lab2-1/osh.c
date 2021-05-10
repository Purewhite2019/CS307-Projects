#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_LINE 80
// #define AUTO_CURSOR_ENABLE

enum Execution_Type {
    Normal, Redirected_Left, Redirected_Right, Piped
};

char* get_token(const char *s, int *iter);
void execute(const char *in);

int main(void){
    char *input = NULL;
    char *history = NULL;
    int should_run = 1;
    size_t len = 0;

    while(should_run){
        #ifdef AUTO_CURSOR_ENABLE
        if(fork() == 0){
            if(execlp("stty", "stty", "echo", NULL) == -1){
                perror("Warning: Cursor Enable Failed.");
                exit(EXIT_FAILURE);
            }
        }
        #endif
        printf("%d osh>", getpid());
        fflush(stdout);
        if(getline(&input, &len, stdin) != -1){
            // printf("input %lu: %s", strlen(input), input);
            // fflush(stdout);
        }
        else{
            printf("Command read failed.\n");
            fflush(stdout);
            continue;
        }
        if(strcmp(input, "exit\n") == 0){
            should_run = 0;
            printf("Terminated.\n");
        }
        else if(strcmp(input, "!!\n") == 0){
            if(history == NULL)
                printf("No commands in history.\n");
            else
                execute(history);
            continue;
        }
        else{
            execute(input);
            if(history != NULL)
                free(history);
            history = input;
            input = NULL;
        }
    }

    return 0;
}

char* get_token(const char *s, int *iter){
    char *ret = NULL;
    int end = *iter;

    while(s[end] != ' ' && s[end] != '\n' && s[end] != '>' && s[end] != '<' && s[end] != '|')
        ++end;
    if(end == *iter)
            end = *iter + 1;

    if((ret = malloc((end - *iter + 1) * sizeof(char))) == NULL){
        perror("Fatal error: malloc() failed.");
        exit(EXIT_FAILURE);
    }
    memset(ret, 0, (end - *iter + 1) * sizeof(char));
    strncpy(ret, s+*iter, end-*iter);
    *iter = end;
    // printf("token get: | %s | \n", ret);
    fflush(stdout);
    return ret;
}

void execute(const char* in){
    char *arg[MAX_LINE/2 + 1];
    int arg_cnt = 0, cur_arg_cnt = 0, special_pos = 0, isSpacing = 0, iter = 0, shouldWait = 1;
    int type = Normal, pid;
    memset(arg, 0, sizeof(arg));
    while(1){
        while((in[iter] == ' ' || in[iter] == '\n') && (in[iter] != '\0'))
            ++iter;
        if(in[iter] == '\0') break;
        arg[arg_cnt] = get_token(in, &iter);
        switch(arg[arg_cnt][0]){
            case '|':
                special_pos = arg_cnt;
                if(type == Normal)
                    type = Piped;
                else{
                    printf("Invalid command: multiple pipes or mixed usage of pipe and redirection.\n");
                    fflush(stdout);
                    return;
                }
                break;
            case '<':
                special_pos = arg_cnt;
                if(type == Normal)
                    type = Redirected_Left;
                else{
                    printf("Invalid command: multiple redirections or mixed usage of pipe and redirection.\n");
                    fflush(stdout);
                    return;
                }
                break;
            case '>':
                special_pos = arg_cnt;
                if(type == Normal)
                    type = Redirected_Right;
                else{
                    printf("Invalid command: multiple redirections or mixed usage of pipe and redirection.\n");
                    fflush(stdout);
                    return;
                }
                break;
        }
        if(arg[arg_cnt] != NULL)
            ++arg_cnt;
    }
    if(arg_cnt == 0)
        return;
    // for(int j = 0; j < arg_cnt; ++j)
    //     printf("%d: %s\n", j, arg[j]);
    // fflush(stdout);
    if(strcmp(arg[arg_cnt-1], "&") == '\0'){
        shouldWait = 0;
        free(arg[arg_cnt-1]);
        arg[arg_cnt-1] = NULL;
    }
    if(type != Normal && (special_pos == 0 || special_pos == arg_cnt-1)){
        printf("Error: special operator without lhs or rhs.\n");
        return;
    }
    switch(type){
        case Normal:
            if((pid = fork()) == 0){
                if(execvp(arg[0], arg) == -1){
                    printf("Error: \"");
                    for(int i = 0; arg[i] != NULL; ++i)
                        printf("%s ", arg[i]);
                    printf("\" failed.\n");
                }
                exit(EXIT_FAILURE);
            }
            else if(pid == -1){
                perror("Fatal error: fork() failed.");
                exit(EXIT_FAILURE);
            }
            else if(shouldWait){
                waitpid(pid, NULL, 0);
                fflush(stdout);
                // printf("parent wait ok.\n");
            }
            break;
        case Redirected_Left:
            free(arg[special_pos]);
            arg[special_pos] = NULL;

            if((pid = fork()) == 0){
                FILE* fd;
                if((fd = fopen(arg[special_pos+1], "r")) == NULL){
                    perror("Fatal error: fopen() failed.");
                    exit(EXIT_FAILURE);
                }
                if(dup2(fileno(fd), STDIN_FILENO) == -1){
                    perror("Fatal error: dup2() failed.");
                    exit(EXIT_FAILURE);
                }
                if(execvp(arg[0], arg) == -1){
                    printf("Error: \"");
                    for(int i = 0; arg[i] != NULL; ++i)
                        printf("%s ", arg[i]);
                    printf("\" failed.\n");
                }
                exit(EXIT_FAILURE);
            }
            else if(pid == -1){
                perror("Fatal error: fork() failed.");
                exit(EXIT_FAILURE);
            }
            else if(shouldWait){
                waitpid(pid, NULL, 0);
                fflush(stdout);
                // printf("parent wait ok.\n");
            }
            break;
        case Redirected_Right:
            free(arg[special_pos]);
            arg[special_pos] = NULL;

            if((pid = fork()) == 0){
                FILE* fd;
                if((fd = fopen(arg[special_pos+1], "w")) == NULL){
                    perror("Fatal error: fopen() failed.");
                    exit(EXIT_FAILURE);
                }
                if(dup2(fileno(fd), STDOUT_FILENO) == -1){
                    perror("Fatal error: dup2() failed.");
                    exit(EXIT_FAILURE);
                }
                if(execvp(arg[0], arg) == -1){
                    printf("Error: \"");
                    for(int i = 0; arg[i] != NULL; ++i)
                        printf("%s ", arg[i]);
                    printf("\" failed.\n");
                }
                exit(EXIT_FAILURE);
            }
            else if(pid == -1){
                perror("Fatal error: fork() failed.");
                exit(EXIT_FAILURE);
            }
            else if(shouldWait){
                waitpid(pid, NULL, 0);
                fflush(stdout);
                // printf("parent wait ok.\n");
            }
            break;
        case Piped:
            free(arg[special_pos]);
            arg[special_pos] = NULL;

            int pipefd[2], pid1, pid2;
            if(pipe(pipefd) == -1){
                perror("Fatal error: pipe() failed.");
                exit(EXIT_FAILURE);
            }
            if((pid1 = fork()) == -1 || (pid2 = fork()) == -1){
                perror("Fatal error: fork() failed.");
                exit(EXIT_FAILURE);
            }
            if(pid1 == 0){
                if(dup2(pipefd[1], STDOUT_FILENO) == -1){
                    perror("Fatal error: dup2() failed.");
                    exit(EXIT_FAILURE);
                }
                if(execvp(arg[0], arg) == -1){
                    printf("Error: \"");
                    for(int i = 0; arg[i] != NULL; ++i)
                        printf("%s ", arg[i]);
                    printf("\" failed.\n");
                }
                exit(EXIT_FAILURE);
            }
            else if(pid2 == 0){
                if(dup2(pipefd[0], STDIN_FILENO) == -1){
                    perror("Fatal error: dup2() failed.");
                    exit(EXIT_FAILURE);
                }
                if(execvp(arg[special_pos + 1], arg+special_pos+1) == -1){
                    printf("Error: \"");
                    for(int i = 0; arg[i] != NULL; ++i)
                        printf("%s ", arg[i]);
                    printf("\" failed.\n");
                }
                exit(EXIT_FAILURE);
            }
            else if(shouldWait){
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
                fflush(stdout);
                // printf("parent wait ok.\n");
            }
            break;
    }
    // printf("execution type: %d\n", type);
    for(int i = 0; i <= arg_cnt; ++i){
        if(arg[i] == NULL)
            continue;
        free(arg[i]);
        arg[i] = NULL;
    }
}

// Known features and solutions: sometimes the cursor will be hidden after some programs. We can use "stty echo" to solve it.