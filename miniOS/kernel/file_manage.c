#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

// 디렉토리를 생성합니다.
Directory* createDirectory(const char* dirName, Directory* parent) {
    Directory* newDir = (Directory*)malloc(sizeof(Directory));
    if (newDir == NULL) {
        perror("Failed to create directory");
        exit(EXIT_FAILURE);
    }
    strncpy(newDir->dirName, dirName, 255);
    newDir->dirName[255] = '\0';
    newDir->files = NULL;
    newDir->parent = parent;
    newDir->next = NULL;
    newDir->subDirs = NULL;
    return newDir;
}

// 사용자 입력에 따라 동작을 수행합니다.
void file_system() {
    char command[256];
    char fileName[256];
    char dirName[256];

    FILE* file = fopen("directory_structure.txt", "r");
    if (file != NULL) {
        rootDir = loadDirectoryStructure(file, NULL);
        fclose(file);

        // 클립보드 디렉토리를 로드된 구조에서 찾아 설정
        Directory* temp = rootDir;
        Directory* prev = NULL;
        while (temp != NULL) {
            if (strcmp(temp->dirName, "clipboard") == 0) {
                clipboardDir = temp;
                // 클립보드 디렉토리가 루트 아래 있는지 확인
                if (temp->parent != rootDir) {
                    // 클립보드 디렉토리를 루트 아래로 이동
                    if (prev != NULL) {
                        prev->next = temp->next;
                    }
                    temp->next = rootDir->subDirs;
                    rootDir->subDirs = temp;
                    temp->parent = rootDir;
                }
                break;
            }
            prev = temp;
            temp = temp->subDirs;
        }
        if (clipboardDir == NULL) {
            clipboardDir = createDirectory("clipboard", rootDir);
            clipboardDir->next = rootDir->subDirs;
            rootDir->subDirs = clipboardDir;
        }
    }
    else {
        rootDir = createDirectory("root", NULL);
        clipboardDir = createDirectory("clipboard", rootDir);
        clipboardDir->next = rootDir->subDirs;
        rootDir->subDirs = clipboardDir;
    }
    currentDir = rootDir;

    while (1) {
        system("clear");
        printDirectoryStructure(currentDir);
        printf("\nEnter command (newfile, copy, paste, remove, mkdir, cd, exit): ");
        scanf("%s", command);

        if (strcmp(command, "newfile") == 0) {
            printf("Enter file name to create: ");
            scanf("%s", fileName);
            newfile(fileName);
        }
        else if (strcmp(command, "copy") == 0) {
            printf("Enter file name to copy: ");
            scanf("%s", fileName);
            copy(fileName);
        }
        else if (strcmp(command, "paste") == 0) {
            paste();
        }
        else if (strcmp(command, "remove") == 0) {
            printf("Enter file name to remove: ");
            scanf("%s", fileName);
            removeFile(fileName);
        }
        else if (strcmp(command, "mkdir") == 0) {
            printf("Enter directory name to create: ");
            scanf("%s", dirName);
            addSubDirectory(dirName);
        }
        else if (strcmp(command, "cd") == 0) {
            printf("Enter directory name to move into: ");
            scanf("%s", dirName);
            if (strcmp(dirName, "..") == 0) {
                moveToParentDirectory();
            }
            else {
                moveToDirectory(dirName);
            }
        }
        else if (strcmp(command, "exit") == 0) {
            // 프로그램 종료 시 디렉토리 구조 저장
            FILE* file = fopen("directory_structure.txt", "w");
            if (file == NULL) {
                perror("Failed to open file for writing");
                exit(EXIT_FAILURE);
            }
            saveDirectoryStructure(rootDir, file);
            fclose(file);
            break;
        }
        else {
            printf("Invalid command.\n");
        }
    }
}
