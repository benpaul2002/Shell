#ifndef FUNC_H
#define FUNC_H

#include "user.h"

void cd_func(char *path, char *arg);
char *pwd_func(char *path);
void echo_func(char **token, int numtokens, int starttokennum);
void ls_func(char *path, char **args, int num_of_strings);
void pinfo_func(char *path, char *arg);
int repeat_func(char **token, int repeat_flag, char **cmd, int n);
int other_func(char **token, int numtokens, int starttokennum, int *bg_flag);
void pipe_func(char *cmds[][20], int n);
void jobs_func(char *arg);
void sig_func(int a, int b);
void fg_func(int a);
void bg_func(int a);

void cd_func(char *path, char *arg) {
    char temp_prev_path[400];
    if(arg==NULL || strcmp(arg, "~")==0) {
        strcpy(temp_prev_path, path);
        strcpy(path, "~");
    }
    else if(strcmp(arg, "-")==0) {
        strcpy(temp_prev_path, path);
        strcpy(path, prevpath);
        printf("%s\n", path);           //printing because cd - also prints
    }
    else {
        strcpy(temp_prev_path, path);
        char *path_chunk;
        path_chunk = strtok(arg, "/");
        while(path_chunk!=NULL) {
            if(strcmp(path_chunk, ".")==0) {
                path_chunk = strtok(NULL, "/");
                continue;
            }
            else if(strcmp(path_chunk, "..")==0) {
                //remove last dir from path
                char dummy[100];                         //this variable will not be used anywhere; just for the function
                char *fpath = getfullpath(path);
                fpath = removefilename(fpath, dummy);
                strcpy(path, fpath);
            }
            else {
                //append to path
                strcat(path, "/");
                strcat(path, path_chunk);
            }
            path_chunk = strtok(NULL, "/");
        }
    }
    char *fpath = getfullpath(path);
    DIR *dr = opendir(fpath);
    if (dr == NULL) {
        perror("cd");
        strcpy(path, temp_prev_path);
        return;
    }
    closedir(dr);
    if(strcmp(fpath, progpath)==0) {
        strcpy(path, "~");
    }
    strcpy(prevpath, temp_prev_path);
}

char *pwd_func(char *path) {
    char *ans = (char *) malloc(400*sizeof(char));
    if(path[0]!='~') {
        return path;
    }
    char cwd[400];
    if(getcwd(cwd, sizeof(cwd))==NULL) {
        perror("getcwd");
    }
    strcpy(ans, cwd);
    if(strcmp(path, "~")!=0) {
        if(path[0]=='~') {
            char temp[400];
            int i;
            for(i=1; i<strlen(path); i++) {
                temp[i] = path[i];
            }
            temp[i] = '\0';
            strcat(ans, temp);
        }
        else strcat(ans, path);
    } 
    return ans;
}

void echo_func(char **token, int numtokens, int starttokennum) {
    for(int i=1+starttokennum; i<numtokens; i++) {
        printf("%s ", token[i]);
    }
    printf("\n");
}

void ls_func(char *path, char **args, int num_of_strings) {
    int flag_a = 0;
    int flag_l = 0;
    char lspath[10][400];
    char dirname_frompath[10][100];
    int path_num=0;
    for(int i=0; i<num_of_strings; i++) {
        if(strcmp(args[i], "-a")==0) {
            flag_a = 1;
        }
        else if(strcmp(args[i], "-l")==0) {
            flag_l = 1;
        }
        else if(strcmp(args[i], "-al")==0 || strcmp(args[i], "-la")==0) {
            flag_a = 1;
            flag_l = 1;
        }
        else {
            strcpy(dirname_frompath[path_num], args[i]);
            strcpy(lspath[path_num], path);
            if(args[i][0]!='/') {
                if(args[i][0]=='~' && strlen(args[i])<2) {
                    strcpy(lspath[path_num], progpath);
                }
                else {
                    strcat(lspath[path_num], "/");
                    strcat(lspath[path_num], args[i]);
                }
            }
            else strcpy(lspath[path_num], args[i]);
            path_num++;
        }
    }
    if(path_num==0) {
        strcpy(lspath[path_num], path);
        path_num++;
    }
    for(int i=0; i<path_num; i++) {
        if(path_num>1) {
            printf("%s:\n", dirname_frompath[i]);
        }
        struct dirent *de;
        char *fpath = getfullpath(lspath[i]);
        struct stat st;
        stat(fpath, &st);
        int written = 0;                    //flag to check whether output was written; used to give error if file doesn't exist
        char filename[100];             //will only be used if the command is ls <filename>
        int file_flag = 0;
        if(S_ISDIR(st.st_mode)!=1) {
            //file
            fpath = removefilename(fpath, filename);
            file_flag = 1;
        }
        DIR *dr = opendir(fpath);
        DIR *dr2 = opendir(fpath);
        if (dr == NULL) {
            perror("dr");
            return;
        }
        long total = 0;

        //printing total blocks for -l flag
        while(flag_l==1 && file_flag!=1 && (de = readdir(dr)) != NULL) {
            char statpath[400];
            strcpy(statpath, fpath);
            strcat(statpath, "/");
            strcat(statpath, de->d_name);
            stat(statpath, &st);
            if(flag_a!=1 && (strcmp(de->d_name, ".")==0 || strcmp(de->d_name, "..")==0)) {
                continue;
            }
            if(flag_l==1) {
                total += st.st_blocks/2;
            }
        }
        if(flag_l==1 && file_flag!=1) {
            printf("total %ld\n", total);
        }

        while((de = readdir(dr2)) != NULL) {
            if(flag_a!=1 && de->d_name[0]=='.') {
                continue;
            }
            if(file_flag==1 && strcmp(de->d_name, filename)!=0) {
                continue;
            }
            if(flag_l==1) {
                char statpath[400];
                strcpy(statpath, fpath);
                strcat(statpath, "/");
                if(file_flag==1) {
                    strcat(statpath, filename);
                }
                else strcat(statpath, de->d_name);
                if(stat(statpath, &st)==-1) {
                    perror("stat");
                }
                printf((S_ISDIR(st.st_mode)) ? "d" : "-");
                printf((st.st_mode & S_IRUSR) ? "r" : "-");
                printf((st.st_mode & S_IWUSR) ? "w" : "-");
                printf((st.st_mode & S_IXUSR) ? "x" : "-");
                printf((st.st_mode & S_IRGRP) ? "r" : "-");
                printf((st.st_mode & S_IWGRP) ? "w" : "-");
                printf((st.st_mode & S_IXGRP) ? "x" : "-");
                printf((st.st_mode & S_IROTH) ? "r" : "-");
                printf((st.st_mode & S_IWOTH) ? "w" : "-");
                printf((st.st_mode & S_IXOTH) ? "x" : "-");
                printf(" %ld", st.st_nlink);
                struct passwd *p;
                if((p = getpwuid(st.st_uid))==NULL) {
                    perror("getpwuid");
                }
                printf(" %s", p->pw_name);
                if((p = getpwuid(st.st_gid))==NULL) {
                    perror("getpwuid");
                }
                printf(" %s", p->pw_name);
                printf(" %ld", st.st_size);
                char timestr[20]; 
                struct tm *time;
                time = localtime(&(st.st_mtime)); 
                strftime(timestr, 20, "%b %d %H:%M", time); 
                printf(" %s",timestr);
                printf(" %s\n", de->d_name);
            }
            else printf("%s\n", de->d_name);
            written = 1;
        }
        if(written==0 && file_flag==1) {
            printf("ls: cannot access %s: No such file\n", filename);
        }
        if(i!=path_num-1) {
            printf("\n");
        }
        closedir(dr);
    }
}

void pinfo_func(char *path, char *arg) {
    int pid;
    if(arg==NULL) {
        pid = getpid();
    }
    else {
        pid = 0;
        int p = pow(10, strlen(arg)-1);
        for(int i=0; i<strlen(arg); i++) {
            if(arg[i]<48 || arg[i]>57) {
                printf("pinfo: invalid arguments\n");
                return;
            }
            int temp = (int) (arg[i]-'0');
            pid += p*temp;
            p /= 10;
        }
    }
    FILE *procfile = NULL;
    char file[200];
    sprintf(file, "/proc/%d/stat", pid);
    procfile = fopen(file, "r");
    if(procfile==NULL) {
        printf("Process %d doesn't exist\n", pid);
        return;
    }
    printf("pid -- %d\n", pid);
    char pinfo_str[200];
    sprintf(pinfo_str, "/proc/%d/exe", pid);
    char pidpath[400];
    readlink(pinfo_str, pidpath, 400);
    printf("Executable path -- %s\n", pidpath);
    unsigned long long val;
    char temp1[100];
    char temp2[100];
    fscanf(procfile, "%lld %s %s ", &val, temp1, temp2);
    printf("Process Status -- %s", temp2);
    for(int i=4; i<24; i++) {
        fscanf(procfile, "%lld", &val);
        if(i==8 && val==pid) {
            printf("+");
        }
        if(i==23) {
            printf("\nmemory -- %lld {Virtual Memory}\n", val);
        }
    }
    fclose(procfile);
}

int repeat_func(char **token, int repeat_flag, char **cmd, int n) {
    if(n<2) {
        printf("repeat: too few arguments\n");
        return -1;
    }
    int p = pow(10, strlen(token[1])-1);
    for(int i=0; i<strlen(token[1]); i++) {
        if(token[1][i]<48 || token[1][i]>57) {
            printf("repeat: invalid arguments\n");
            return -1;
        }
        int temp = (int) (token[1][i]-'0');
        repeat_flag += p*temp;
        p /= 10;
    }
    int i;
    for(i=0; i<n; i++) {
        cmd[i] = token[i];
    }
    return repeat_flag;
}

void ctrlc_handler(int sig) {
    int pid, status;
    pid = waitpid(-1, &status, WNOHANG);
    kill(pid, SIGINT);
    signal(SIGINT, SIG_DFL);
}

char ctrlz_cmd[400];

void ctrlz_handler(int sig) {
    int pid, status;
    pid = waitpid(-1, &status, WNOHANG);
    kill(pid, SIGTSTP);
    signal(SIGTSTP, SIG_DFL);
}

int other_func(char **token, int numtokens, int starttokennum, int *bg_flag) {
    strcpy(ctrlz_cmd, token[starttokennum]);
    int ppid = getpid();
    if(strcmp(token[numtokens-1], "&")==0) {
        *bg_flag = 1;
    }
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    int pid = fork();
    int status;
    if(pid==0) {
        if(*bg_flag==1) {
            setpgid(0, 0);
        }
        else tcsetpgrp(0,ppid);
        signal(SIGINT, ctrlc_handler);
        signal(SIGTSTP, ctrlz_handler);
        char *cmd = token[0+starttokennum];
        char *argv[20];
        int i;
        for(i=0+starttokennum; i<numtokens; i++) {
            if(i==numtokens-1 && strcmp(token[i], "&")==0) {
                break;
            }
            argv[i-starttokennum] = token[i];
        }
        argv[i-starttokennum] = NULL;
        if(execvp(cmd, argv)==-1) {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else {
        if(*bg_flag==0) {
            waitpid(pid, &status, WUNTRACED);
            if(status!=0 && status!=2 && !WIFEXITED(status)) {
                if(bgnum<400) {
                    bg[bgnum].pid = pid;
                    strcpy(bg[bgnum].cmd, ctrlz_cmd);
                    bg[bgnum].status = 1;
                    bg[bgnum].num = bgnum+1;
                    bgnum++;
                }
            }
        }
        if(WIFEXITED(status)) {
            if(WEXITSTATUS(status)!=0) {
                return 1;
            }
        }
        else {
            if(WIFSIGNALED(status)) {
                return pid;
            }
            else {
                return 2;
            }
        }
        return 0;
    }
}

void pipe_func(char *cmds[][20], int n) {
    int prev = 0;
    int fd[2];
    for(int i=0; i<n; i++) {
        int num = 0;
        for(int j=0; cmds[i][j]!=NULL; j++) {
            num++;
        }
        int status;
        pipe(fd);
        int pid = fork();
        if(pid==-1) {
            exit(EXIT_FAILURE);
        }
        else if(pid==0) {
            dup2(prev, 0);
            if(i+1<n) {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            if(strcmp(cmds[i][0], "pwd")==0) {
                if(num>1) {
                    exit(EXIT_FAILURE);           
                }
                printf("%s\n", pwd_func(path));
            }
            else if(strcmp(cmds[i][0], "pinfo")==0) {
                if(num>2) {
                    exit(EXIT_FAILURE);           
                }
                pinfo_func(path, cmds[i][1]);
            }
            else if(strcmp(cmds[i][0], "jobs")==0) {
                if(num>2) {
                    exit(EXIT_FAILURE);           
                }
                jobs_func(cmds[i][1]);
            }
            else if(execvp(cmds[i][0], cmds[i])==-1) {
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        else {
            waitpid(pid, &status, 0);
            if(WIFEXITED(status)) {
                if(status!=0) {
                    printf("%s failed\n", cmds[i][0]);
                    //return 1;
                }
            }
            close(fd[1]);
            prev = fd[0];
        }
    }
}

void jobs_func(char *arg) {
    int flag_r = 0;
    int flag_s = 0;
    if(arg!=NULL) {
        if(strcmp(arg, "-r")==0) {
            flag_r = 1;
        }
        else if(strcmp(arg, "-s")==0) {
            flag_s = 1;
        }
        else {
            printf("jobs: invalid arguments\n");
            return;
        }
    }
    bgproc temp;
    bgproc temp_arr[400];
    for(int i=0; i<bgnum; i++) {
        temp_arr[i].pid = bg[i].pid;
        strcpy(temp_arr[i].cmd, bg[i].cmd);
        temp_arr[i].status = bg[i].status;
        temp_arr[i].num = bg[i].num;
    }

    for(int i=0; i<bgnum; i++) {
        for(int j=i+1; j<bgnum; j++) {
            if(strcmp(temp_arr[i].cmd, temp_arr[j].cmd)>0) {
                temp.pid = temp_arr[i].pid;
                strcpy(temp.cmd, temp_arr[i].cmd);
                temp.status = temp_arr[i].status;
                temp.num = temp_arr[i].num;

                temp_arr[i].pid = temp_arr[j].pid;
                strcpy(temp_arr[i].cmd, temp_arr[j].cmd);
                temp_arr[i].status = temp_arr[j].status;
                temp_arr[i].num = temp_arr[j].num;

                temp_arr[j].pid = temp.pid;
                strcpy(temp_arr[j].cmd, temp.cmd);
                temp_arr[j].status = temp.status;
                temp_arr[j].num = temp.num;
            }
        }
    }

    for(int i=0; i<bgnum; i++) {
        if(temp_arr[i].status!=-1) {
            FILE *procfile = NULL;
            char file[200];
            sprintf(file, "/proc/%d/stat", temp_arr[i].pid);
            procfile = fopen(file, "r");
            unsigned long long val;
            char temp1[100];
            char temp2[100];
            fscanf(procfile, "%lld %s %s ", &val, temp1, temp2);
            if(flag_r==1 && (strcmp(temp2, "R")!=0 && strcmp(temp2, "S")!=0)) {
                continue;
            }
            if(flag_s==1 && (strcmp(temp2, "R")==0 || strcmp(temp2, "S")==0)) {
                continue;
            }
            printf("[%d] ", temp_arr[i].num);
            if(strcmp(temp2, "R")==0 || strcmp(temp2, "S")==0) {
                printf("Running ");
            }
            else printf("Stopped ");

            printf("%s [%d]\n", temp_arr[i].cmd, temp_arr[i].pid);
        }
    }
    return;
}

void sig_func(int a, int b) {
    int pid = bg[a-1].pid;
    kill(pid, b);
}

void fg_func(int a) {
    int pid = bg[a-1].pid;
    int status;
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(0, pid);
    kill(pid, SIGCONT);
    signal(SIGINT, ctrlc_handler);
    waitpid(pid, &status, WUNTRACED);
    tcsetpgrp(0, progpid);
    signal(SIGTTOU, SIG_DFL);
}

void bg_func(int a) {
    int pid = bg[a-1].pid;
    kill(pid, SIGCONT);
}

#endif