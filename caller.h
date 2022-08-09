#ifndef CALLER_H
#define CALLER_H

#include "func.h"

int cd_caller(char **token, int numtokens, int starttokennum) {
    char *arg;
    if(numtokens==1) {
        arg = NULL;
    }
    else arg = token[1+starttokennum];
    if(numtokens>2+starttokennum) {
        printf("cd: Too many arguments\n");
        return 1;
    }
    cd_func(path, arg);
    return 0;
}

int pwd_caller(char **token, int numtokens, int starttokennum) {
    if(numtokens>1+starttokennum) {
        printf("pwd: Too many arguments\n");
        return 1;
    }
    printf("%s\n", pwd_func(path));
    return 0;
}

void ls_caller(char **token, int numtokens, int starttokennum) {
    char *args[3];
    int i;
    for(i=1; i<numtokens-starttokennum; i++) {
        args[i-1] = token[i+starttokennum];
    }    
    ls_func(path, args, i-1);
}

int pinfo_caller(char **token, int numtokens, int starttokennum) {
    char *arg;
    if(numtokens==1+starttokennum) {
        arg = NULL;
    }
    else arg = token[1+starttokennum];
    if(numtokens>2+starttokennum) {
        printf("pinfo: Too many arguments\n");
        return 1;
    }
    pinfo_func(path, arg);
    return 0;
}

int jobs_caller(char **token, int numtokens, int starttokennum) {
    char *arg;
    if(numtokens==1+starttokennum) {
        arg = NULL;
    }
    else arg = token[1+starttokennum];
    if(numtokens>2+starttokennum) {
        printf("jobs: Too many arguments\n");
        return 1;
    }
    jobs_func(arg);
    return 0;
}

int sig_caller(char **token, int numtokens, int starttokennum) {
    int a = 0;
    int b = 0;
    if(numtokens!=3+starttokennum) {
        printf("sig: Invalid number of arguments\n");
        return 1;
    }
    int p = pow(10, strlen(token[1])-1);
    for(int i=0; i<strlen(token[1]); i++) {
        if(token[1][i]<48 || token[1][i]>57) {
            printf("sig: invalid arguments\n");
            return 1;
        }
        int temp = (int) (token[1][i]-'0');
        a += p*temp;
        p /= 10;
    }
    p = pow(10, strlen(token[2])-1);
    for(int i=0; i<strlen(token[2]); i++) {
        if(token[2][i]<48 || token[2][i]>57) {
            printf("sig: invalid arguments\n");
            return 1;
        }
        int temp = (int) (token[2][i]-'0');
        b += p*temp;
        p /= 10;
    }
    int valid = 0;
    for(int i=0; i<bgnum; i++) {
        if(bg[i].num==a && bg[i].status==1) {
            valid = 1;
            break;
        }
    }
    if(valid==0) {
        printf("sig: job with job number %d doesn't exist\n", a);
        return 1;
    }
    sig_func(a, b);
    return 0;
}

int fg_caller(char **token, int numtokens, int starttokennum) {
    int a;
    if(numtokens!=2+starttokennum) {
        printf("fg: invalid number or arguments\n");
        return 1;
    }
    int p = pow(10, strlen(token[1])-1);
    for(int i=0; i<strlen(token[1]); i++) {
        if(token[1][i]<48 || token[1][i]>57) {
            printf("fg: invalid arguments\n");
            return 1;
        }
        int temp = (int) (token[1][i]-'0');
        a += p*temp;
        p /= 10;
    }
    int valid = 0;
    for(int i=0; i<bgnum; i++) {
        if(bg[i].num==a && bg[i].status==1) {
            valid = 1;
            break;
        }
    }
    if(valid==0) {
        printf("fg: job with job number %d doesn't exist\n", a);
        return 1;
    }
    fg_func(a);
    return 0;
}

int bg_caller(char **token, int numtokens, int starttokennum) {
    int a;
    if(numtokens!=2+starttokennum) {
        printf("bg: invalid number or arguments\n");
        return 1;
    }
    int p = pow(10, strlen(token[1])-1);
    for(int i=0; i<strlen(token[1]); i++) {
        if(token[1][i]<48 || token[1][i]>57) {
            printf("bg: invalid arguments\n");
            return 1;
        }
        int temp = (int) (token[1][i]-'0');
        a += p*temp;
        p /= 10;
    }
    int valid = 0;
    for(int i=0; i<bgnum; i++) {
        if(bg[i].num==a && bg[i].status==1) {
            valid = 1;
            break;
        }
    }
    if(valid==0) {
        printf("bg: job with job number %d doesn't exist\n", a);
        return 1;
    }
    bg_func(a);
    return 0;
}

#endif