#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

//fd[0] untuk read
//fd[1] untuk write

int main(){
    int fd1[2], fd2[2];
    int status;
    pid_t pid;

    if(pipe(fd1) == -1){
        fprintf(stderr, "Pipe Failed" ); 
		return 1;
    }

    pid = fork();

    if(pid < 0){
        fprintf(stderr, "fork Failed" ); 
		return 1; 
    }

    else if (pid == 0){
        close(fd1[0]);
        dup2(fd1[1], 1);
        char *argv[] = {"ps", "aux", NULL};
        execv("/usr/bin/ps", argv);
    }

    while(wait(&status) > 0);
    close(fd1[1]);
    dup2(fd1[0], 0);

    if(pipe(fd2) == -1){
        fprintf(stderr, "Pipe Failed" ); 
		return 1;
    }

    pid = fork();

    if(pid < 0){
        fprintf(stderr, "fork Failed" ); 
		return 1; 
    }

    else if(pid == 0){
        close(fd2[0]);
        dup2(fd2[1], 1);
        char *argv[] = {"sort", "-nrk", "3,3", NULL};
        execv("/usr/bin/sort", argv);
    }

    while(wait(&status) > 0);
    close(fd2[1]);
    dup2(fd2[0], 0);

    char *argv[] = {"head", "-5", NULL};
    execv("/usr/bin/head", argv);

    close(fd2[0]);
    close(fd1[0]);
}