#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "system.h"



void print_minios(char* str);

int main() {
    print_minios("\n[MiniOS SSU] Hello, World!\n");

    char *input;

    while(1) {
        printf("현재 사용가능한 커맨드는 다음과 같습니다.\n\tminisystem | mynum | fork | IPC | Pi | RR\n");
        // readline을 사용하여 입력 받기
        input = readline("커맨드를 입력하세요(종료:exit) : ");
        printf("\n");
        if (strcmp(input,"exit") == 0) {
            break;
        }
        else if (strcmp(input,"minisystem") == 0){
            minisystem();
        }
		else if (strcmp(input, "mynum") == 0){
			mynum();
		}
        else if(strcmp(input, "fork") == 0){
            mosfork();
        }else if(strcmp(input, "IPC") == 0){
            IPC();
        }else if(strcmp(input, "Pi") == 0){
            Monte_Carlo();
        }else if(strcmp(input, "RR") == 0){
            Round_Robin();
        }
        else system(input);
    }

    // 메모리 해제
    free(input);
    print_minios("[MiniOS SSU] MiniOS Shutdown........");

    return 0;
}

void print_minios(char* str) {
        printf("%s\n",str);
}

