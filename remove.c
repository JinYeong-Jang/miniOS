#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 파일 노드를 정의합니다.
typedef struct FileNode {
    char fileName[256];
    struct FileNode* next;
} FileNode;

// 디렉토리 구조체를 정의합니다.
typedef struct Directory {
    char dirName[256];
    FileNode* files;
    struct Directory* parent;
    struct Directory* next;
    struct Directory* subDirs;
} Directory;

// 루트 디렉토리와 현재 디렉토리를 정의합니다.
Directory* rootDir = NULL;
Directory* currentDir = NULL;
Directory* clipboardDir = NULL;


// 파일을 제거합니다.
void removeFile(const char* fileName) {
    FileNode* prev = NULL;
    FileNode* current = currentDir->files;

    while (current != NULL && strcmp(current->fileName, fileName) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("File '%s' not found in current directory.\n", fileName);
        return;
    }

    if (prev == NULL) {
        currentDir->files = current->next;
    } else {
        prev->next = current->next;
    }

    free(current);
    printf("File '%s' removed from current directory.\n", fileName);
}