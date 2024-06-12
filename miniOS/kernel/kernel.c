#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "system.h"



void print_minios(char* str);

int main() {
    // 디렉토리 구조를 텍스트 파일에서 읽어와 복원
    FILE* file = fopen("directory_structure.txt", "r");
    if (file != NULL) {
        rootDir = loadDirectoryStructure(file, NULL);
        fclose(file);
    } else {
        rootDir = createDirectory("root", NULL);
        clipboardDir = createDirectory("clipboard", rootDir);
        rootDir->subDirs = clipboardDir;  // 루트 디렉토리의 서브 디렉토리로 추가
    }
    currentDir = rootDir;

    handleUserInput();

    return 0;
}

void print_minios(char* str) {
        printf("%s\n",str);
}

