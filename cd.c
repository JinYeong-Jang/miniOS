// 현재 디렉토리를 상위 디렉토리로 이동합니다.
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

void moveToParentDirectory() {
    if (currentDir->parent != NULL) {
        currentDir = currentDir->parent;
        printf("Moved to parent directory '%s'.\n", currentDir->dirName);
    } else {
        printf("Already in the root directory.\n");
    }
}

// 현재 디렉토리의 서브 디렉토리로 이동합니다.
void moveToDirectory(const char* dirName) {
    Directory* subDir = currentDir->subDirs;
    while (subDir != NULL) {
        if (strcmp(subDir->dirName, dirName) == 0) {
            currentDir = subDir;
            printf("Moved to directory '%s'.\n", dirName);
            return;
        }
        subDir = subDir->next;
    }
    printf("Directory '%s' not found in current directory.\n", dirName);
}