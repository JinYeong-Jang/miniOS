#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1
void IPC(){
    char write_msg[BUFFER_SIZE] = "Greeting";
    char read_msg[BUFFER_SIZE];
    int fd[2];
    pid_t pid;
    if(pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();

    if(pid == 0){
        close(fd[WRITE_END]);
        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("read %s \n", read_msg);
        close(fd[READ_END]);
        exit(EXIT_SUCCESS);
        }
    if(pid > 0){
        close(fd[READ_END]);
        write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);
        close(fd[WRITE_END]);
        wait(NULL);
    }
}