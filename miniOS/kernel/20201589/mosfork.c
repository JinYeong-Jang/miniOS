#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
void mosfork(){
    int status;
    int pid = fork();
    if(pid == 0){
        printf("PID of Chile : %d\n\n", pid);
        exit(1);
    }
    if(pid > 0){
        wait(&status);
        printf("PID of Parent : %d\n\n", pid);
    }
}