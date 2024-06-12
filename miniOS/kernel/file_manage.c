// 사용자 입력에 따라 동작을 수행합니다.
void handleUserInput() {
    char command[256];
    char fileName[256];
    char dirName[256];
    
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
    
    while (1) {
        system("clear");
        printDirectoryStructure(currentDir);
        printf("\nEnter command (newfile, copy, paste, remove, mkdir, cd, exit): ");
        scanf("%s", command);

        if (strcmp(command, "newfile") == 0) {
            printf("Enter file name to create: ");
            scanf("%s", fileName);
            newfile(fileName);
        } else if (strcmp(command, "copy") == 0) {
            printf("Enter file name to copy: ");
            scanf("%s", fileName);
            copy(fileName);
        } else if (strcmp(command, "paste") == 0) {
            paste();
        } else if (strcmp(command, "remove") == 0) {
            printf("Enter file name to remove: ");
            scanf("%s", fileName);
            removeFile(fileName);
        } else if (strcmp(command, "mkdir") == 0) {
            printf("Enter directory name to create: ");
            scanf("%s", dirName);
            addSubDirectory(dirName);
        } else if (strcmp(command, "cd") == 0) {
            printf("Enter directory name to move into: ");
            scanf("%s", dirName);
            if (strcmp(dirName, "..") == 0) {
                moveToParentDirectory();
            } else {
                moveToDirectory(dirName);
            }
        } else if (strcmp(command, "exit") == 0) {
            // 프로그램 종료 시 디렉토리 구조 저장
            FILE* file = fopen("directory_structure.txt", "w");
            if (file == NULL) {
                perror("Failed to open file for writing");
                exit(EXIT_FAILURE);
            }
            saveDirectoryStructure(rootDir, file);
            fclose(file);
            break;
        } else {
            printf("Invalid command.\n");
        }
    }
}
