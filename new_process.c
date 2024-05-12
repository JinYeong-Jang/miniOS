#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#define MAX_PROCESSES 10

typedef struct {
    int pid;        // 프로세스 ID
    int *memory;    // 할당된 메모리
} Process;

Process processes[MAX_PROCESSES];  // 프로세스 배열
int num_processes = 0;             // 생성된 프로세스 수

// 프로세스 생성 함수
void create_process(int memory_size) {
    if (num_processes < MAX_PROCESSES) {
        // 메모리 할당
        processes[num_processes].memory = (int *)malloc(memory_size * sizeof(int));
        if (processes[num_processes].memory == NULL) {
            printf("메모리 할당에 실패했습니다.\n");
            return;
        }
        processes[num_processes].pid = num_processes + 1; // PID는 1부터 시작
        num_processes++;
        printf("프로세스가 생성되었습니다. PID: %d, 할당된 메모리 크기: %d\n", processes[num_processes-1].pid, memory_size);
    } else {
        printf("더 이상 프로세스를 생성할 수 없습니다. 최대 프로세스 개수를 초과했습니다.\n");
    }
}

// 프로세스 종료 함수
void terminate_process(int pid) {
    int i, found = 0;
    for (i = 0; i < num_processes; i++) { //find all processs , thread
        if (processes[i].pid == pid) {
            printf("프로세스 PID %d 종료됨. 할당된 메모리 해제됨\n", processes[i].pid);
            // 할당된 메모리 해제
            free(processes[i].memory);
            // 프로세스 제거
            for (; i < num_processes - 1; i++) {
                processes[i] = processes[i + 1];
            }
            num_processes--;
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("종료할 프로세스를 찾을 수 없습니다. 유효한 PID를 입력하세요.\n");
    }
}

void show_processes() {
    printf("현재 실행 중인 프로세스:\n");
    if (num_processes == 0) {
        printf("실행 중인 프로세스가 없습니다.\n");
        return;
    }
    for (int i = 0; i < num_processes; i++) {
        printf("PID: %d, 할당된 메모리 크기: %p\n", processes[i].pid, (void *)processes[i].memory);
    }
}

void process_manager() {
  printf("======================================\n");
  show_processes();
  printf("======================================\n");

  char *input;
  int memory_size, pid;

  while(1) {
    input = readline("Process Manager : (종료 : exit / 프로세스 생성 : create / 프로세스 종료 : terminate / 프로세스 목록 : list) : ");

    if (strcmp(input,"exit") == 0)
    {
          break;
    }
      
    else if (strcmp(input, "create") == 0)
    {
    printf("할당할 메모리 크기를 입력하세요: ");
    scanf("%d", &memory_size);
    create_process(memory_size);
    }

    else if (strcmp(input, "terminate") == 0)
    {
    printf("종료할 프로세스의 PID를 입력하세요: ");
    scanf("%d", &pid);
    terminate_process(pid);
    }
      
    else if (strcmp(input), "list" == 0)
    {
      show_processes();
    }
      
    else
    {
    printf("알 수 없는 명령어입니다.\n");
    }
  }
}