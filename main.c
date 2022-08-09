#include "caller.h"

int main() {
    progpid = getpid();
    strcpy(path, "~");
    if(getcwd(progpath, sizeof(progpath))==NULL) {
        perror("getcwd");
    }
    char *usr_sysname = getusr_sysname();
    int fds[2] = {-1};
    for(int i=0; i<400; i++) {
        bg[i].pid = -1;
        strcpy(bg[i].cmd, "");
        bg[i].status = -1;
        bg[i].num = -1;
    }
    while(1) {
        if(fds[0]!=-1) {
            if(dup2(fds[0], STDIN_FILENO) < 0) {
                perror("Unable to duplicate file descriptor");
                break;
            }
            fds[0] = -1;
            fds[1] = -1;
        }
        else {
            for(int i=0; i<400; i++) {
                int status;
                if(bg[i].status!=-1 && waitpid(bg[i].pid, &status, WNOHANG)){
                    if(WIFEXITED(status)) {
                        printf("%s with pid %d exited normally\n", bg[i].cmd, bg[i].pid);
                    }
                    else if(WIFSIGNALED(status)) {
                        printf("%s with pid %d exited with exit status %d\n", bg[i].cmd, bg[i].pid, status);
                    }
                    bg[i].status = -1;
                    strcpy(bg[i].cmd, "");
                    continue;
                }   
            } 
            printf("%s", usr_sysname);
            printf("%s> ", path);
        }
        char *input = (char *) malloc(400*sizeof(char));
        fflush(stdin);
        if(fgets(input, 400, stdin)==NULL) {
            printf("\n");
            break;
        }
        char *inputcmds[10];
        strtok(input, ";\n");
        int numcmds=0;
        while(input!=NULL) {
            inputcmds[numcmds] = input;
            numcmds++;
            input = strtok(NULL, ";\n");
        }
        int repeat_flag = 0;
        char **repeat_token = (char **) malloc(10*sizeof(char *));
        char *parse;
        int i=0;
        char *token[20];
        int j;
        int temp_stdout, temp_stdin;
        while(i<numcmds || repeat_flag>0) {
            int pipe_flag[20] = {0};
            int pipe_num = 0;
            if(repeat_flag==0) {
                parse = strtok(inputcmds[i], " \t");
                j=0;
                temp_stdout = dup(STDOUT_FILENO);
                temp_stdin = dup(STDIN_FILENO);
            }
            if(parse==NULL && repeat_flag<=0) {
                break;
            }
            while(parse!=NULL) {
                if(strcmp(parse, ">")==0 || strcmp(parse, ">>")==0) {
                    int flag;
                    if(strcmp(parse, ">")==0) {
                        flag = 1;
                    }
                    else flag = 2;
                    parse = strtok(NULL, "\t[ ]*");
                    int fd;
                    if(flag==1) {
                        fd = open(parse, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    }
                    else fd = open(parse, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if(fd < 0) {
                        perror("Failed to open file");
                        break;
                    }
                    if(dup2(fd, STDOUT_FILENO) < 0) {
                        perror("Unable to duplicate file descriptor");
                        break;
                    }
                    close(fd);
                    parse = strtok(NULL, "\t[ ]*");
                    continue;
                }
                else if(strcmp(parse, "<")==0) {
                    parse = strtok(NULL, "\t[ ]*");
                    int fd = open(parse, O_RDONLY);
                    if(dup2(fd, STDIN_FILENO) < 0) {
                        perror("Unable to duplicate file descriptor");
                        break;
                    }
                    close(fd);
                    parse = strtok(NULL, "\t[ ]*");
                    continue;
                }
                else if(strcmp(parse, "|")==0) {
                    pipe_flag[pipe_num] = j;
                    pipe_num++;
                    parse = strtok(NULL, "\t[ ]*");
                    continue;
                }
                token[j] = parse;
                parse = strtok(NULL, "\t[ ]*");
                j++;
            }
            if(pipe_num!=0) {
                char *cmds[20][20];
                int i1 = 0;
                for(int p=0; p<=pipe_num; p++) {
                    int start=0;
                    if(p>0) {
                        start = pipe_flag[p-1];
                    }
                    int x=start;
                    for( ; x<j; x++) {
                        if(p!=pipe_num && x==pipe_flag[p]) {
                            break;
                        }
                        cmds[i1][x-start] = token[x];
                    }
                    cmds[i1][x-start] = NULL;
                    i1++;
                }
                pipe_func(cmds, i1);
                i++;
                if(dup2(temp_stdout, STDOUT_FILENO) < 0) {
                    perror("Unable to restore file descriptor");
                    break;
                }
                close(temp_stdout);
                if(dup2(temp_stdin, STDIN_FILENO) < 0) {
                    perror("Unable to restore file descriptor");
                    break;
                }
                close(temp_stdin);
                break;
            }
            char cmd[10];
            int starttokennum = 0;
            if(repeat_flag>0) {
                repeat_flag--;
                strcpy(cmd, repeat_token[2]);
                starttokennum = 2;
            }
            else strcpy(cmd, token[0]);

            if(strcmp(cmd, "cd")==0) {
                if(cd_caller(token, j, starttokennum)==1) {
                    break;
                }
            }
            else if(strcmp(cmd, "pwd")==0) {
                if(pwd_caller(token, j, starttokennum)==1) {
                    break;
                }
            }
            else if(strcmp(cmd, "echo")==0) {
                echo_func(token, j, starttokennum);
            }
            else if(strcmp(cmd, "ls")==0) {
                ls_caller(token, j, starttokennum);
            }
            else if(strcmp(cmd, "pinfo")==0) {
                if(pinfo_caller(token, j, starttokennum)==1) {
                    break;
                }
            }
            else if(strcmp(cmd, "repeat")==0) {
                repeat_flag = repeat_func(token, repeat_flag, repeat_token, j);
                if(repeat_flag==-1) {
                    repeat_flag = 0;
                    break;
                }
            }
            else if(strcmp(cmd, "jobs")==0) {
                if(jobs_caller(token, j, starttokennum)==1) {
                    break;
                }
            }
            else if(strcmp(cmd, "sig")==0) {
                if(sig_caller(token, j, starttokennum)==1) {
                    break;
                }
            }
            else if(strcmp(cmd, "fg")==0) {
                if(fg_caller(token, j, starttokennum)==1) {
                    break;
                }
            }
            else if(strcmp(cmd, "bg")==0) {
                if(bg_caller(token, j, starttokennum)==1) {
                    break;
                }
            }
            else {
                int bg_flag = 0;
                int num = other_func(token, j, starttokennum, &bg_flag);
                if(num==1) {
                    printf("%s: command not found\n", token[0+starttokennum]);
                    break;
                }
                else if(num==2) {
                    break;
                }
                else if(num>0 && bg_flag!=0) {
                    if(bgnum<400) {
                        bg[bgnum].pid = num;
                        strcpy(bg[bgnum].cmd, cmd);
                        bg[bgnum].status = 1;
                        bg[bgnum].num = bgnum+1;
                        bgnum++;
                    }
                }
            }
            if(repeat_flag<=0) {
                i++;
                if(dup2(temp_stdout, STDOUT_FILENO) < 0) {
                    perror("Unable to restore file descriptor");
                    break;
                }
                close(temp_stdout);
                if(dup2(temp_stdin, STDIN_FILENO) < 0) {
                    perror("Unable to restore file descriptor");
                    break;
                }
                close(temp_stdin);
            }
        }
    }
    for(int i=0; i<400; i++) {
        if(bg[i].status!=-1) {
            printf("%s with pid %d killed\n", bg[i].cmd, bg[i].pid);
            kill(bg[i].pid, SIGKILL);
        }   
    } 
    return 0;
}