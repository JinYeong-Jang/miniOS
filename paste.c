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

// 파일 노드를 생성합니다.
FileNode* createFileNode(const char* fileName) {
    FileNode* newNode = (FileNode*)malloc(sizeof(FileNode));
    if (newNode == NULL) {
        perror("Failed to create file node");
        exit(EXIT_FAILURE);
    }
    strncpy(newNode->fileName, fileName, 255);
    newNode->fileName[255] = '\0';
    newNode->next = NULL;
    return newNode;
}

// 클립보드 디렉토리를 비웁니다.
void clearClipboard() {
    FileNode* current = clipboardDir->files;
    while (current != NULL) {
        FileNode* temp = current;
        current = current->next;
        free(temp);
    }
    clipboardDir->files = NULL;
}

// 파일을 현재 디렉토리로 붙여넣습니다.
void paste() {
    if (clipboardDir->files == NULL) {
        printf("Clipboard is empty.\n");
        return;
    }

    FileNode* fileToPaste = clipboardDir->files;

    // 파일을 현재 디렉토리로 이동합니다.
    FileNode* pastedFile = createFileNode(fileToPaste->fileName);
    pastedFile->next = currentDir->files;
    currentDir->files = pastedFile;

    // 클립보드를 비웁니다.
    clearClipboard();

    printf("File '%s' pasted to current directory.\n", pastedFile->fileName);
}