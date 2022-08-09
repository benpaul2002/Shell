#ifndef USER_H
#define USER_H

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<limits.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pwd.h>
#include<time.h>
#include<math.h>
#include<sys/wait.h>
#include<fcntl.h>

char path[400];
char prevpath[400];
char progpath[400];

int progpid;

struct bgprc {
    int pid;
    char cmd[400];
    int status;         //1 for valid
    int num;
};

typedef struct bgprc bgproc;

bgproc bg[400];                             //maintaining list of child pids

int bgnum = 0;

char *getfullpath(char *path) {
    char *fpath = (char *) malloc(400*sizeof(char));
    if(fpath==NULL) {
        perror("malloc");
    }
    if(path[0]=='~') {
        strcpy(fpath, progpath);
        int i;
        char temp[400];
        for(i=1; i<strlen(path); i++) {
            temp[i-1] = path[i];
        }
        temp[i-1] = '\0';
        strcat(fpath, temp);
        return fpath;
    }
    return path;
}

char *getusr_sysname() {
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    if(gethostname(hostname, HOST_NAME_MAX)!=0) {
        perror("gethostname");
    }
    if(getlogin_r(username, LOGIN_NAME_MAX)!=0) {
        perror("getlogin_r");
    }
    char *ans = (char *) malloc((HOST_NAME_MAX+LOGIN_NAME_MAX)*sizeof(char));
    if(ans==NULL) {
        perror("malloc");
    }
    strcpy(ans, username);
    strcat(ans, "@");
    strcat(ans, hostname);
    strcat(ans, ":");
    return ans;
}

char *removefilename(char *path, char *filename) {
    char *ans = (char *) malloc(400*sizeof(char));
    char temp[100];
    int i, j;
    for(i=strlen(path)-1; i>=0; i--) {
        if(path[i]=='/') {
            break;
        }
    }
    for(j=0; j<i; j++) {
        ans[j] = path[j];
    }
    ans[j] = '\0';
    j++;
    i=0;
    while(j<strlen(path)) {
        temp[i] = path[j];
        i++;
        j++;
    }
    temp[i] = '\0';
    strcpy(filename, temp);
    return ans;
}

#endif