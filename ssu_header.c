#include "ssu_header.h"

char *commandata[10] = {
        "backup",
        "remove",
        "recover",
        "list",
        "help"
};

//경로기본설정
void Init() {
    int idx;
    int numOfSlash = 0;
    getcwd(exePATH, PATHMAX);

    for (idx = 0; idx < strlen(exePATH) - 1; idx++) {
        if (numOfSlash == 2) {
            break;
        }
        if (exePATH[idx] == '/') {
            numOfSlash++;
        }
    }

    //getenv로 HOME에 대한 환경변수를 찾고 "/backup"을 붙여 backupPATH에 저장
    strncpy(homePATH, exePATH, idx);
    homePATH[idx] = '\0';
    strcpy(backupPATH, homePATH);
    strcat(backupPATH, "backup/");

    //F_OK 는 access와 같이 사용되는 기호상수, 'F_OK'는 구체적으로 파일이나 디렉터리가 존재하는지 확인하는 데 사용됨
    //존재하면 access는 0, 없으면 -1 반환
    if (access(backupPATH, F_OK)) {
        mkdir(backupPATH, 0777);
    }

    //로그파일이 없다면 생성
    strcpy(backuplogPATH, backupPATH);
    strcat(backuplogPATH, "ssubak.log");

    if (access(backuplogPATH, F_OK)) {
        if ((fd = open(backuplogPATH, O_CREAT | O_WRONLY, 0666)) == -1) {
            fprintf(stderr, "can't make ssubak.log file");
            exit(1);
        }
        close(fd);
    }
    strcpy(backupDirPATH, backupPATH);
    strcat(backupDirPATH, getDate());
    if (access(backupDirPATH, F_OK)) {
        mkdir(backupDirPATH, 0777);
    }

    //프로그램 실행 후 백업디렉토리를 돌면서 링크드리스트 형성
    MakeMainLinkedList(backupPATH);
}

//help 전체 출력
void help() {
    printf("Usage:\n");
    printf("  > backup <PATH> [OPTION]... : backup file if <PATH> is file\n");
    printf("    -d : backup files in directory if <PATH> is directory\n");
    printf("    -r : backup files in directory recursive if <PATH> is directory\n");
    printf("    -y : backup file although already backuped\n");
    printf("  > remove <PATH> [OPTION]... : remove backuped file if <PATH> is file\n");
    printf("    -d : remove backuped files in directory if <PATH> is directory\n");
    printf("    -r : remove backuped files in directory recursive if <PATH> is directory\n");
    printf("    -a : remove all backuped files\n");
    printf("  > recover <PATH> [OPTION]... : recover backuped file if <PATH> is file\n");
    printf("    -d : recover backuped files in directory if <PATH> is directory\n");
    printf("    -n : recover backuped files in directory recursive if <PATH> is directory\n");
    printf("    -l : recover latest backuped file\n");
    printf("    -n : <NEW_PATH> : recover backuped file with new path\n");
    printf("  > list [PATH] : show backup list by directory structure\n");
    printf("    >> rm <INDEX> [OPTION]... : remove backuped files of [INDEX] with [OPTION]\n");
    printf("    >> rc <INDEX> [OPTION]... : recover backuped files of [INDEX] with [OPTION]\n");
    printf("    >> vi(m) <INDEX> : edit original file of [INDEX]\n");
    printf("    >> exit : exit program\n");
    printf("  > help [COMMAND] : show commands for program\n");
}

//recover 부분만 출력
void helprecover() {
    printf("Usage: recover <PATH> [OPTION]... : recover backuped file if <PATH> is file\n");
    printf("    -d : recover backuped files in directory if <PATH> is directory\n");
    printf("    -n : recover backuped files in directory recursive if <PATH> is directory\n");
    printf("    -l : recover latest backuped file\n");
    printf("    -n : <NEW_PATH> : recover backuped file with new path\n");
}

//backup 부분만 출력
void helpbackup() {
    printf("Usage: backup <PATH> [OPTION]... : backup file if <PATH> is file\n");
    printf("    -d : backup files in directory if <PATH> is directory\n");
    printf("    -r : backup files in directory recursive if <PATH> is directory\n");
    printf("    -y : backup file although already backuped\n");
}

//recmove 부분만 출력
void helpremove() {
    printf("Usage: remove <PATH> [OPTION]... : remove backuped file if <PATH> is file\n");
    printf("    -d : remove backuped files in directory if <PATH> is directory\n");
    printf("    -r : remove backuped files in directory recursive if <PATH> is directory\n");
    printf("    -a : remove all backuped files\n");

}

//list 부분만 출력
void helplist() {
    printf("Usage: list [PATH] : show backup list by directory structure\n");
    printf("    >> rm <INDEX> [OPTION]... : remove backuped files of [INDEX] with [OPTION]\n");
    printf("    >> rc <INDEX> [OPTION]... : recover backuped files of [INDEX] with [OPTION]\n");
    printf("    >> vi(m) <INDEX> : edit original file of [INDEX]\n");
    printf("    >> exit : exit program\n");
}

//help 부분만 출력
void helphelp() {
    printf("  > help [COMMAND] : show commands for program\n");
}

//parameter 초기화
void ParameterInit(command_parameter *parameter) {
    parameter->filename = (char *) malloc(sizeof(char *) * PATH_MAX);
    parameter->dirname = (char *) malloc(sizeof(char *) * PATH_MAX);
    parameter->commandopt = 0;
    parameter->majorcommand = 0;
    parameter->nfilename = (char *) malloc(sizeof(char *) * PATH_MAX);

}

//parameter 가공
int ParameterProcessing(int argcnt, char **arglist, command_parameter *parameter) {
    struct stat buf;
    int option;
    int argcforopt = argcnt - 1;

    //옵션 처리를 위한 새로운 배열 arglistforopt
    char *arglistforopt[10];
    int index = 0;
    for (int i = 1; i < argcnt; i++) {
        arglistforopt[index++] = arglist[i];
    }

    //parameter의 majorcommand에 정의해놓은 커맨드값과 대조 후 대입
    if (!strcmp(commandata[0], arglist[0])) {
        parameter->majorcommand = CMD_BACK;
    } else if (!strcmp(commandata[1], arglist[0])) {
        parameter->majorcommand = CMD_REM;
    } else if (!strcmp(commandata[2], arglist[0])) {
        parameter->majorcommand = CMD_REC;
    } else if (!strcmp(commandata[3], arglist[0])) {
        parameter->majorcommand = CMD_LIST;
    }
    switch (parameter->majorcommand) {
        //backup일 경우
        case CMD_BACK: {

            //<2-4 예외처리1> 경로를 입력하지 않을 경우, backup 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
            if (argcnt < 2) {
                helpbackup();
                return -1;

            }
                //인수개수가 2개이면 옵션이 없다는 것이니 NO_OPT
            else if (argcnt == 2) {
                parameter->commandopt |= NO_OPT;

                //<2-4 예외처리3> 인자로 입력받은 경로가 일반 파일이나 디렉토리가 아니거나 해당 경로에 대해 접근권한이 없는 경우, 에러 처리 후
                //프로그램 비정상 종료
                if (lstat(arglist[1], &buf) < 0 || (!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))) {
                    fprintf(stderr, "ERROR: lstat error for %s or not a file/directory\n", arglist[1]);
                    return -1;
                }

                //<2-4 에러처리6> 인자로 입력받은 옵션 중 –d, -r을 사용하지 않았는데, 인자로 입력받은 경로가 디렉토리일 경우, 에러 처리 후 프로그램 비정상 종료
                if (!(parameter->commandopt & OPT_R) && !(parameter->commandopt & OPT_D)) {
                    if (lstat(arglist[1], &buf) >= 0 && S_ISDIR(buf.st_mode)) {
                        fprintf(stderr, "ERROR: %s must not be directory\n", arglist[1]);
                        return -1;
                    }
                }
                //절대경로로 변환
                ConvertPath(arglist[1], parameter->filename);
            }
                //옵션이 있는 경우 처리
            else {
                //<2-4 예외처리3> 인자로 입력받은 경로가 일반 파일이나 디렉토리가 아니거나 해당 경로에 대해 접근권한이 없는 경우, 에러 처리 후
                //프로그램 비정상 종료
                if (lstat(arglist[1], &buf) < 0 || (!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))) {
                    fprintf(stderr, "ERROR: lstat error for %s or not a file/directory\n", arglist[1]);
                    return -1;
                }

                while ((option = getopt(argcforopt, arglistforopt, "dry")) != -1) {
                    //<2-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우, backup 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                    if (option != 'd' && option != 'r' && option != 'y') {
                        helpbackup();
                        return -1;
                    }

                    // parameter->commandopt를 OPT_D로 덮어씌우기
                    if (option == 'd') {
                        //<2-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), backup 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_D) {
                            helpbackup();
                            return -1;
                        }
                        parameter->commandopt |= OPT_D;
                    }
                    // parameter->commandopt를 OPT_R로 덮어씌우기
                    if (option == 'r') {
                        //<2-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), backup 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_R) {
                            helpbackup();
                            return -1;
                        }
                        parameter->commandopt |= OPT_R;
                    }
                    // parameter->commandopt를 OPT_Y로 덮어씌우기
                    if (option == 'y') {
                        //<2-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), backup 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_Y) {
                            helpbackup();
                            return -1;
                        }
                        parameter->commandopt |= OPT_Y;
                    }
                }

                //-r -d 옵션 두개가 공존하면 -r이 우선이기에 -d 옵션 삭제
                if (parameter->commandopt & OPT_R && parameter->commandopt & OPT_D) {
                    parameter->commandopt &= ~OPT_D;
                }

                //<2-4 에러처리7> 인자로 입력받은 옵션 중 –d, -r을 사용하였는데, 인자로 입력받은 경로가 파일일 경우, 에러 처리 후 프로그램 비정상 종료
                if (parameter->commandopt & OPT_R || parameter->commandopt & OPT_D) {
                    if (lstat(arglist[1], &buf) >= 00 && S_ISREG(buf.st_mode)) {
                        fprintf(stderr, "ERROR: %s must be directory\n", arglist[1]);
                        return -1;
                    }
                }
                //절대경로로 변환
                ConvertPath(arglist[1], parameter->filename);
            }
            break;
        }
            //remove일 경우
        case CMD_REM: {
            //절대경로로 변환
            ConvertPath(arglist[1], parameter->filename);

            //<3-4 예외처리1> 경로를 입력하지 않을 경우, remove 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
            if (argcnt < 2) {
                helpremove();
                return -1;
            } else if (argcnt == 2) {
                parameter->commandopt = NO_OPT;

                //<3-4 예외처리3> 인자로 입력받은 경로와 일치하는 백업 파일이나 디렉토리가 없는 경우, 에러 처리 후 프로그램 비정상 종료
                if (lstat(arglist[1], &buf) < 0) {
                    fprintf(stderr, "ERROR: lstat error for %s or no file/directory\n", arglist[1]);
                    return -1;
                }

                //<3-4 에러처리6> 인자로 입력받은 옵션 중 –d, -r을 사용하지 않았는데, 인자로 입력받은 경로가 존재하고 디렉토리일 경우, 에러 처리후 프로그램 비정상 종료
                if (!(parameter->commandopt & OPT_R) && !(parameter->commandopt & OPT_D)) {
                    if (lstat(arglist[1], &buf) >= 0 && S_ISDIR(buf.st_mode)) {
                        fprintf(stderr, "ERROR: %s must not be directory\n", arglist[1]);
                        return -1;
                    }
                }

            } else {
                //<3-4 예외처리3> 인자로 입력받은 경로와 일치하는 백업 파일이나 디렉토리가 없는 경우, 에러 처리 후 프로그램 비정상 종료
                if (lstat(arglist[1], &buf) < 0) {
                    fprintf(stderr, "ERROR: lstat error for %s or no file/directory\n", arglist[1]);
                    return -1;
                }

                while ((option = getopt(argcforopt, arglistforopt, "dra")) != -1) {
                    //<3-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우, remove 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                    if (option != 'd' && option != 'r' && option != 'a') {
                        helpremove();
                        return -1;
                    }

                    // parameter->commandopt를 OPT_D로 덮어씌우기
                    if (option == 'd') {
                        //<3-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), remove 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_D) {
                            helpremove();
                            return -1;
                        }
                        parameter->commandopt |= OPT_D;
                    }
                    // parameter->commandopt를 OPT_R로 덮어씌우기
                    if (option == 'r') {
                        //<3-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), remove 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_R) {
                            helpremove();
                            return -1;
                        }
                        parameter->commandopt |= OPT_R;
                    }
                    // parameter->commandopt를 OPT_Y로 덮어씌우기
                    if (option == 'a') {
                        //<3-4 예외처리5> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), remove 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_A) {
                            helpremove();
                            return -1;
                        }
                        parameter->commandopt |= OPT_A;
                    }
                }

                //-r -d 옵션 두개가 공존하면 -r이 우선이기에 -d 옵션 삭제
                if (parameter->commandopt & OPT_R && parameter->commandopt & OPT_D) {
                    parameter->commandopt &= ~OPT_D;
                }

                //<3-4 에러처리7> 인자로 입력받은 옵션 중 –d, -r을 사용하였는데, 인자로 입력받은 경로가 존재하고 파일일 경우, 에러 처리 후 프로그램 비정상 종료
                if (parameter->commandopt & OPT_R || parameter->commandopt & OPT_D) {
                    if (lstat(arglist[1], &buf) >= 0 && S_ISREG(buf.st_mode)) {
                        fprintf(stderr, "ERROR: %s must be directory\n", arglist[1]);
                        return -1;
                    }
                }
            }
            break;
        }
            //recover일 경우
        case CMD_REC: {
            //절대경로로 변환
            ConvertPath(arglist[1], parameter->filename);

            //<4-4 예외처리1> 경로를 입력하지 않을 경우, recover 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
            if (argcnt < 2) {
                helprecover();
                return -1;
            } else if (argcnt == 2) {
                parameter->commandopt = NO_OPT;

            } else {
                while ((option = getopt(argcforopt, arglistforopt, "drln:")) != -1) {
                    //<4-4 예외처리6> 인자로 입력받은 옵션이 올바르지 않을 경우, recover 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                    if (option != 'd' && option != 'r' && option != 'l' && option != 'n') {
                        helprecover();
                        return -1;
                    }

                    // parameter->commandopt를 OPT_D로 덮어씌우기
                    if (option == 'd') {
                        //<4-4 예외처리6> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), recover 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_D) {
                            helprecover();
                            return -1;
                        }
                        parameter->commandopt |= OPT_D;
                    }

                    // parameter->commandopt를 OPT_R로 덮어씌우기
                    if (option == 'r') {
                        //<4-4 예외처리6> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), recover 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_R) {
                            helprecover();
                            return -1;
                        }
                        parameter->commandopt |= OPT_R;
                    }

                    // parameter->commandopt를 OPT_L로 덮어씌우기
                    if (option == 'l') {
                        //<4-4 예외처리6> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), recover 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_L) {
                            helprecover();
                            return -1;
                        }
                        parameter->commandopt |= OPT_L;
                    }

                    // parameter->commandopt를 OPT_N로 덮어씌우기
                    if (option == 'n') {
                        //<4-4 예외처리6> 인자로 입력받은 옵션이 올바르지 않을 경우(중복된 경우), recover 명령어에 대한 Usage 출력 후 프로그램 비정상 종료
                        if (parameter->commandopt & OPT_N) {
                            helprecover();
                            return -1;
                        }
                        parameter->commandopt |= OPT_N;
                    }
                }

                //-r -d 옵션 두개가 공존하면 -r이 우선이기에 -d 옵션 삭제
                if (parameter->commandopt & OPT_R && parameter->commandopt & OPT_D) {
                    parameter->commandopt &= ~OPT_D;
                }

                //<4-4 에러처리8> 인자로 입력받은 옵션 중 –d, -r을 사용하였는데, 인자로 입력받은 경로가 존재하고 파일일 경우, 에러 처리 후 프로그램 비정상 종료
                if (parameter->commandopt & OPT_R || parameter->commandopt & OPT_D) {
                    if (lstat(arglist[1], &buf) >= 0 && S_ISREG(buf.st_mode)) {
                        fprintf(stderr, "ERROR: %s must be directory\n", arglist[1]);
                        return -1;
                    }
                        //<4-4 에러처리9> 인자로 입력받은 옵션 중 ‘-n’ 뒤에 경로를 입력하지 않을 경우, 에러 처리 후 프로그램 비정상 종료
                    else if (parameter->commandopt & OPT_N) {
                        //절대경로로 변환
                        ConvertPath(arglist[argcnt - 1], parameter->nfilename);
                        if (!strcmp(arglist[argcnt - 1], "-n")) {
                            fprintf(stderr, "ERROR: -n <NEWPATH>");
                            return -1;
                        }
                            //<4-4 에러처리10> 인자로 입력받은 옵션 중 ‘-n’ 뒤에 경로가 올바르지 않을 경우, 에러 처리 후 프로그램 비정상 종료
                        else if (lstat(arglist[argcnt - 1], &buf) < 0) {
                            fprintf(stderr, "ERROR: lstat error for %s or wrong <NEW_PATH> input\n",
                                    arglist[argcnt - 1]);
                            return -1;
                        }
                            //<4-4 에러처리11> 인자로 입력받은 옵션 중 –d, -r을 사용하지 않았는데, ‘-n’ 뒤에 경로가 존재하고 디렉토리일 경우,
                            // 혹은 인자로 입력받은 옵션 중 –d, -r을 사용하였는데, ‘-n’ 뒤에 경로가 존재하고 파일일 경우, 에러 처리 후 프로그램 비정상 종료
                        else if (lstat(arglist[argcnt - 1], &buf) && S_ISREG(buf.st_mode)) {
                            fprintf(stderr, "ERROR: %s must be directory\n", arglist[argcnt - 1]);
                            return -1;
                        }
                    }
                }

                //<4-4 에러처리7> 인자로 입력받은 옵션 중 –d, -r을 사용하지 않았는데, 인자로 입력받은 경로가 존재하고 디렉토리일 경우, 에러 처리후 프로그램 비정상 종료
                if (!(parameter->commandopt & OPT_R) && !(parameter->commandopt & OPT_D)) {
                    //절대경로로 변환
                    ConvertPath(arglist[argcnt - 1], parameter->nfilename);

                    if (lstat(arglist[1], &buf) >= 0 && S_ISDIR(buf.st_mode)) {
                        fprintf(stderr, "ERROR: %s must not be directory\n", arglist[1]);
                        return -1;
                    }
                        //<4-4 에러처리9> 인자로 입력받은 옵션 중 ‘-n’ 뒤에 경로를 입력하지 않을 경우, 에러 처리 후 프로그램 비정상 종료
                    else if (parameter->commandopt & OPT_N) {
                        if (!strcmp(arglist[argcnt - 1], "-n")) {
                            fprintf(stderr, "ERROR: -n <NEWPATH>");
                            return -1;
                        }
                            //<4-4 에러처리10> 인자로 입력받은 옵션 중 ‘-n’ 뒤에 경로가 올바르지 않을 경우, 에러 처리 후 프로그램 비정상 종료
                        else if (lstat(arglist[argcnt - 1], &buf) < 0) {
                            fprintf(stderr, "ERROR: lstat error for %s or wrong <NEW_PATH> input\n",
                                    arglist[argcnt - 1]);
                            return -1;
                        }
                            //<4-4 에러처리11> 인자로 입력받은 옵션 중 –d, -r을 사용하지 않았는데, ‘-n’ 뒤에 경로가 존재하고 디렉토리일 경우,
                            // 혹은 인자로 입력받은 옵션 중 –d, -r을 사용하였는데, ‘-n’ 뒤에 경로가 존재하고 파일일 경우, 에러 처리 후 프로그램 비정상 종료
                        else if (lstat(arglist[argcnt - 1], &buf) && S_ISDIR(buf.st_mode)) {
                            fprintf(stderr, "ERROR: %s must be directory\n", arglist[argcnt - 1]);
                            return -1;
                        }
                    }
                }
            }
            break;
        }
        case CMD_LIST: {
            if (argcnt < 2) {
                helplist();
                return -1;
            } else if (argcnt == 2) {

                parameter->commandopt = NO_OPT;

                //절대경로로 변환
                ConvertPath(arglist[1], parameter->filename);

                //<5-4 예외처리3> 인자로 입력받은 경로와 일치하는 백업 파일이나 디렉토리가 없는 경우, 에러 처리 후 프로그램 비정상 종료
                if (lstat(arglist[1], &buf) < 0) {
                    fprintf(stderr, "ERROR: lstat error for %s or no file/directory\n", arglist[1]);
                    return -1;
                }
            } else {
                fprintf(stderr, "ERROR: too many input\n");
                return -1;
            }
        }
    }
    return 0;
}

//절대경로로 변환
void ConvertPath(char *argpath, char *resolved) {
    char buf[PATHMAX];
    char *newpaths;

    newpaths = realpath(argpath, buf);

    //만약 절대경로로 변환에 실패하면 파일을 만들고 다시 변환
    if (newpaths == NULL) {
        umask(0);
        fd = open(argpath, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        close(fd);
        newpaths = realpath(argpath, buf);
    }
    strcpy(resolved, newpaths);
}

//backup을 위한 시간 함수
char *getDate() {
    char *date = (char *) malloc(sizeof(char) * 14);
    time_t timer;
    struct tm *t;

    timer = time(NULL);
    t = localtime(&timer);

    sprintf(date, "%02d%02d%02d%02d%02d%02d", t->tm_year % 100, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
            t->tm_sec);

    return date;
}

int BackupFile(command_parameter *parameter, char *date) {
    int len;
    int fd1, fd2, fdlog;
    char *buf = (char *) malloc(sizeof(char *) * STRMAX);
    char *filename = (char *) malloc(sizeof(char *) * PATHMAX);
    char *filepath = (char *) malloc(sizeof(char *) * PATHMAX);
    char *newBackupPath = (char *) malloc(sizeof(char *) * PATHMAX);
    char forlog[STRMAX];
    int idx;

    strcpy(filepath, parameter->filename);
    strcpy(newBackupPath, backupDirPATH);

    //오류처리. 읽기 모드로 열지못하면 -1 반환
    if ((fd1 = open(filepath, O_RDONLY)) < 0) {
        fprintf(stderr, "ERROR: open error for %s\n", filepath);
        DeleteEmptyDir();
        exit(1);
    }

    //백업 폴더에 내용이 같은 백업 파일이 존재한지 확인 존재하면 -1 반환
    if (FindFileLinkedList(parameter, filename, date) == -1) {
        return 0;
    }

    if (strcmp(filepath, parameter->dirname)) {
        //dirname이 나오는 인덱스까지 진행
        for (idx = 0; idx < strlen(filepath) - 1; idx++) {
            if (!strncmp(filepath + idx, parameter->dirname, strlen(parameter->dirname))) {
                break;
            }
        }

        //마지막 '/' 다음 문자열을 filename에 복사
        strcpy(filename, filepath + idx + 1 + strlen(parameter->dirname));
        //filepath는 '/' 과 뒤의 파일이름은 사라지고 끝맺음
        //ex) /home/user/documents
        filepath[idx] = '\0';
    } else {
        for (idx = strlen(filepath) - 1; filepath[idx] != '/'; idx--);
        strcpy(filename, filepath + idx + 1);
        filepath[idx] = '\0';
    }

    strcat(newBackupPath, "/");
    //만든 backup/YYMMDDHHMMSS 뒤에 파일 이름 연결
    strcat(newBackupPath, filename);

    //오류처리. 권한은 777로 설정, 파일이 존재하지 않으면 생성,파일이 존재하면 내용을
    // 모두 삭제, 파일을 쓰기 모드로 열지 못하면 -1 반환
    if ((fd2 = open(newBackupPath, O_CREAT | O_TRUNC | O_WRONLY, 0777)) < 0) {
        fprintf(stderr, "ERROR: open error for %s\n", newBackupPath);
        DeleteEmptyDir();
        exit(1);
    }
    while ((len = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, len);
    }

    //백업이 완료되었으니 완료 메세지 출력
    printf("\"%s\" backuped to \"%s\"\n", parameter->filename, newBackupPath);

    //ssubak.log 파일을 열어 로그 작성
    fdlog = open(backuplogPATH, O_RDWR | O_APPEND);
    sprintf(forlog, "%s : \"%s\" backuped to \"%s\"\n", date, parameter->filename, newBackupPath);
    write(fdlog, forlog, strlen(forlog));
    close(fd1);
    close(fd2);
    close(fdlog);

    //새로운 파일을 백업했으니 링크드 리스트에 추가
    fileNode *new = (fileNode *) malloc(sizeof(fileNode));
    new->filename = strdup(filename);
    new->path = strdup(newBackupPath);
    new->next = NULL;
    new->prev = NULL;

    if (trailer->prev->head == NULL) {
        trailer->prev->head = new;
        trailer->prev->tail = new;
    } else {

        fileNode *curr = trailer->prev->head;

        while (1) {
            if (curr->next == NULL) {
                curr->next = new;
                new->prev = curr;

                trailer->prev->tail = new;
                break;

            } else {
                curr = curr->next;
            }
        }
    }
    return 0;
}

void BackupDirectory(command_parameter *parameter, char *path, char *backuppath, char *date) {
    int cnt;
    struct dirent **namelist;

    //입력받은 경로를 scandir로 파악
    if ((cnt = scandir(path, &namelist, NULL, reverse_alphasort)) == -1) {
        fprintf(stderr, "ERROR: scandir error for %s\n", path);
        DeleteEmptyDir();
        exit(1);
    }

    // namelist를 순회하면서 파일이면 backupfile, 디렉토리이면 재귀함수 호출
    for (int i = 0; i < cnt; ++i) {

        // "." , ".." ,".DS_Store" 같은 파일들은 건너띔
        if (namelist[i]->d_name[0] == '.') {
            continue;
        }

        // 디렉토리인지 확인
        if (namelist[i]->d_type == DT_DIR && parameter->commandopt & OPT_R) {
            char *pathgo = (char *) malloc(sizeof(char *) * PATH_MAX);
            char *dirpathgo = (char *) malloc(sizeof(char *) * PATH_MAX);
            strcpy(pathgo, path);
            strcat(pathgo, "/");
            strcat(pathgo, namelist[i]->d_name);

            strcpy(dirpathgo, backuppath);
            strcat(dirpathgo, "/");
            strcat(dirpathgo, namelist[i]->d_name);

            //디렉토리 생성하고 디렉토리를 링크드리스트에 추가한 후 다시 함수 호출
            if (access(dirpathgo, F_OK)) {
                mkdir(dirpathgo, 0777);

                dirNode *new = (dirNode *) malloc(sizeof(dirNode));

                new->dirname = strdup(namelist[i]->d_name);
                new->path = strdup(dirpathgo);
                new->head = NULL;
                new->tail = NULL;
                new->sub = 1;
                new->next = trailer;
                new->prev = trailer->prev;

                trailer->prev->next = new;
                trailer->prev = new;
            }
            BackupDirectory(parameter, pathgo, dirpathgo, date);
        }
            //파일인 경우 backupfile 함수 호출
        else if (namelist[i]->d_type == DT_REG) {
            strcpy(parameter->filename, path);
            strcat(parameter->filename, "/");
            strcat(parameter->filename, namelist[i]->d_name);
            BackupFile(parameter, date);
            strcpy(parameter->filename, path);
        }
    }
    free(namelist);
}

void CommandRemove(command_parameter *parameter, char *date) {
    char *filename = (char *) malloc(strlen(parameter->filename) + 1);
    int i;
    for (i = strlen(parameter->filename) - 1; parameter->filename[i] != '/'; i--);

    strcpy(filename, parameter->filename + i + 1);

    //링크드 리스트에서 같은 파일 찾기
    if (FindFileLinkedList(parameter, filename, date) == -1) {
        exit(0);
    }
    free(filename);
}

void RemoveFiles(command_parameter *parameter, int num, char *date) {
    int fdlog;
    char forlog[STRMAX];
    int cnt = 1;

    dirNode *currDir = trailer->prev;

    //링크드리스트를 돌면서 같은
    while (currDir != header) {
        fileNode *currFile2 = currDir->head;
        while (currFile2 != NULL) {
            //파일이 같은지 확인
            if (CompareName(currFile2->path, parameter->filename)) {
                if (cnt == num || parameter->commandopt & OPT_A) {
                    //파일 제거 문구 출력
                    printf("\"%s\" removed by \"%s\"\n", currFile2->path, parameter->filename);

                    //ssubak.log 파일을 열어 로그 작성
                    fdlog = open(backuplogPATH, O_RDWR | O_APPEND);
                    sprintf(forlog, "%s : \"%s\" removed by \"%s\"\n", date, currFile2->path, parameter->filename);
                    write(fdlog, forlog, strlen(forlog));
                    close(fdlog);

                    //백업 파일 삭제했으니 링크드 리스트에도 반영
                    if (currFile2 == currDir->head) {
                        currDir->head = currFile2->next;
                        if (currDir->head != NULL) {
                            currDir->head->prev = NULL;
                        }
                    } else if (currFile2 == currDir->tail) {
                        currDir->tail = currFile2->prev;
                        currDir->tail->next = NULL;
                    } else {
                        currFile2->prev->next = currFile2->next;
                        currFile2->next->prev = currFile2->prev;
                    }

                    remove(currFile2->path);

                    free(currFile2->path);
                    free(currFile2->filename);
                    free(currFile2);
                }
                cnt++;
            }
            currFile2 = currFile2->next;
        }
        currDir = currDir->prev;
    }
}

//remove directory 와 recover directory를 동시에 처리함
void RemRecDirectory(command_parameter *parameter, char *path, char *backuppath, char *date) {
    int cnt;
    struct dirent **namelist;

    if ((cnt = scandir(path, &namelist, NULL, reverse_alphasort)) == -1) {
        fprintf(stderr, "ERROR: scandir error for %s\n", path);
        DeleteEmptyDir();
        exit(1);
    }

    // scandir해서 나온 namelist를 돔
    for (int i = 0; i < cnt; ++i) {

        // ".", "..", ".DS_Store" 같은 파일들 무시
        if (namelist[i]->d_name[0] == '.') {
            continue;
        }

        // 디렉토리인지 확인 후 함수 재호출
        if (namelist[i]->d_type == DT_DIR && parameter->commandopt & OPT_R) {
            char *pathgo = (char *) malloc(sizeof(char *) * PATH_MAX);
            char *dirpathgo = (char *) malloc(sizeof(char *) * PATH_MAX);
            strcpy(pathgo, path);
            strcat(pathgo, "/");
            strcat(pathgo, namelist[i]->d_name);

            strcpy(dirpathgo, backuppath);
            strcat(dirpathgo, "/");
            strcat(dirpathgo, namelist[i]->d_name);

            RemRecDirectory(parameter, pathgo, dirpathgo, date);

        }
            //파일이면 FindFileLinkedList에서 찾기
        else if (namelist[i]->d_type == DT_REG) {
            strcpy(parameter->filename, path);
            strcat(parameter->filename, "/");
            strcat(parameter->filename, namelist[i]->d_name);
            if (FindFileLinkedList(parameter, namelist[i]->d_name, date) == -1) {
                DeleteEmptyDir();
                exit(0);
            }
            strcpy(parameter->filename, path);
        }
    }
    free(namelist);
}

void CommandRecover(command_parameter *parameter, char *date) {
    char *filename = (char *) malloc(strlen(parameter->filename) + 1);
    int i;
    for (i = strlen(parameter->filename) - 1; parameter->filename[i] != '/'; i--);

    strcpy(filename, parameter->filename + i + 1);

    //링크드 리스트에서 파일 찾기
    if (FindFileLinkedList(parameter, filename, date) == -1) {
        DeleteEmptyDir();
        exit(0);
    }
    free(filename);
}

//프로그램 실행 후 Init()에서 백업디렉토리에 대해 링크드 리스트 형성
void MakeMainLinkedList(char *path) {
    int cnt;
    int tmpcnt = 0;
    struct dirent **namelist;
    header = (dirNode *) malloc(sizeof(dirNode));
    trailer = (dirNode *) malloc(sizeof(dirNode));

    header->path = NULL;
    header->dirname = NULL;
    header->head = NULL;
    header->tail = NULL;
    header->next = trailer;
    header->prev = NULL;

    trailer->path = NULL;
    trailer->dirname = NULL;
    trailer->head = NULL;
    trailer->tail = NULL;
    trailer->next = NULL;
    trailer->prev = header;

    //백업디렉토리 목록 얻음
    if ((cnt = scandir(path, &namelist, NULL, reverse_alphasort)) == -1) {
        fprintf(stderr, "ERROR: scandir error for %s\n", path);
        DeleteEmptyDir();
        exit(1);
    }

    //백업디렉토리를 링크드리스트로 구현
    //첫번째 디렉토리를 head로 하고 그 디렉 밑으로 접근
    for (int i = 0; i < cnt; i++) {
        if (namelist[i]->d_name[0] != '.' && strcmp("ssubak.log", namelist[i]->d_name)) {
            dirNode *new = (dirNode *) malloc(sizeof(dirNode));

            char tmppath[PATHMAX];
            strcpy(tmppath, path);
            strcat(tmppath, namelist[i]->d_name);

            new->dirname = strdup(namelist[i]->d_name);
            new->path = strdup(tmppath);
            new->head = NULL;
            new->tail = NULL;
            new->next = trailer;
            new->prev = trailer->prev;

            header->next = new;
            trailer->prev = new;

            struct dirent **subnamelist;
            int cnt2;
            //첫번째 디렉토리 밑으로 접근해서 링크드 리스트 구현
            if ((cnt2 = scandir(tmppath, &subnamelist, NULL, reverse_alphasort)) != -1) {
                for (int j = 0; j < cnt2; j++) {
                    if (subnamelist[j]->d_name[0] != '.') {
                        MakeLinkedList(subnamelist[j]->d_name, tmppath);
                    }
                }
            }
            tmpcnt = i;
            break;
        }
    }
    //첫번째 디렉토리 접근 후 두번째부터 마지막 디렉토리 접근
    if (tmpcnt + 1 != cnt) {
        for (int i = tmpcnt + 1; i < cnt; i++) {
            if (namelist[i]->d_name[0] != '.' && strcmp("ssubak.log", namelist[i]->d_name)) {
                dirNode *new = (dirNode *) malloc(sizeof(dirNode));

                char tmppath[PATHMAX];
                strcpy(tmppath, path);
                strcat(tmppath, namelist[i]->d_name);

                new->dirname = strdup(namelist[i]->d_name);
                new->path = strdup(tmppath);
                new->head = NULL;
                new->tail = NULL;
                new->next = trailer;
                new->prev = trailer->prev;

                trailer->prev->next = new;
                trailer->prev = new;

                struct dirent **subnamelist;
                int cnt2;
                //디렉토리 밑으로 접근해서 링크드 리스트 구현
                if ((cnt2 = scandir(tmppath, &subnamelist, NULL, reverse_alphasort)) != -1) {

                    for (int j = 0; j < cnt2; j++) {
                        if (subnamelist[j]->d_name[0] != '.') {
                            MakeLinkedList(subnamelist[j]->d_name, tmppath);
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < cnt; i++) {
        free(namelist[i]);
    }
    free(namelist);
}

//backup/240324xxxxxx/ 밑의 디렉토리와 파일에 접근해서 링크드 리스트 구현
void MakeLinkedList(char *path, char *longpath) {
    struct stat sb;
    struct dirent **namelist;

    //경로 이름 붙이기
    char temppath[PATHMAX];

    strcpy(temppath, longpath);
    if (temppath[strlen(temppath) - 1] != '/') {
        strcat(temppath, "/");
    }
    strcat(temppath, path);

    //디렉토리면 연결하고 다시 함수 호출
    if ((stat(temppath, &sb) != -1) && S_ISDIR(sb.st_mode)) {

        dirNode *new = (dirNode *) malloc(sizeof(dirNode));

        new->dirname = strdup(path);
        new->path = strdup(temppath);
        new->head = NULL;
        new->tail = NULL;
        new->sub = 1;
        new->next = trailer;
        new->prev = trailer->prev;

        trailer->prev->next = new;
        trailer->prev = new;

        int cnt;
        if ((cnt = scandir(temppath, &namelist, NULL, reverse_alphasort)) != -1) {
            for (int i = 0; i < cnt; i++) {
                if (namelist[i]->d_name[0] != '.') {
                    MakeLinkedList(namelist[i]->d_name, temppath);
                }
            }
        }
        for (int i = 0; i < cnt; i++) {
            free(namelist[i]);
        }
        free(namelist);
    }
        //파일인 경우
    else {
        fileNode *new = (fileNode *) malloc(sizeof(fileNode));
        new->filename = strdup(path);
        new->path = strdup(temppath);
        new->next = NULL;
        new->prev = NULL;

        if (trailer->prev->head == NULL) {
            trailer->prev->head = new;
            trailer->prev->tail = new;
        } else {

            fileNode *curr = trailer->prev->head;

            while (1) {
                if (curr->next == NULL) {
                    curr->next = new;
                    new->prev = curr;

                    trailer->prev->tail = new;
                    break;

                } else {
                    curr = curr->next;
                }
            }
        }
    }
}

//scandir에서 alphasort 역순으로 정렬해주는 함수
int reverse_alphasort(const struct dirent **a, const struct dirent **b) {
    return strcmp((*b)->d_name, (*a)->d_name);
}

//해시값을 구하여 비교하는 함수
int CompareHash(char *path1, char *path2) {
    char *hash1 = (char *) malloc(sizeof(char) * STRMAX);
    char *hash2 = (char *) malloc(sizeof(char) * STRMAX);

    ConvertHash(path1, hash1);
    ConvertHash(path2, hash2);

    return !strcmp(hash1, hash2);
}

//해시값을 구해주는 함수
void ConvertHash(char *path, char *result_hash) {
    FILE *fp;
    //MD5_DIGEST_LENGTH==16
    unsigned char hash[MD5_DIGEST_LENGTH];
    //short int(short) 유형의 변수가 보유할 수 있는 최대값으로 SHRT_MAX==32767
    unsigned char buffer[SHRT_MAX];
    int bytes = 0;
    //'MD5_CTX'는 OpenSSL 라이브러리에서 MD5 해시 계산의 내부 상태를 유지하는 데 사용되는 구조
    MD5_CTX md5;

    //오류처리. 파일을 binary모드로 열기
    if ((fp = fopen(path, "rb")) == NULL) {
        printf("ERROR: fopen error for %s\n", path);
    }
    //MD5_CTX md5; 과 MD5_Init(&md5); 로 MD5 context를 만들고 초기화함
    MD5_Init(&md5);

    //읽은 값을 buffer로, fread로 읽은 byte를 bytes로 저장
    while ((bytes = fread(buffer, 1, SHRT_MAX, fp)) != 0)
        //버퍼록
        MD5_Update(&md5, buffer, bytes);

    //해시값 얻음
    MD5_Final(hash, &md5);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(result_hash + (i * 2), "%02x", hash[i]);
    result_hash[32] = 0;

    fclose(fp);
}

//경로를 받아 파일/디렉토리 이름을 비교하는 함수
int CompareName(char *path1, char *path2) {
    char *compare1 = (char *) malloc(sizeof(char *) * PATHMAX);
    char *compare2 = (char *) malloc(sizeof(char *) * PATHMAX);
    int idx1, idx2;

    for (idx1 = strlen(path1) - 1; path1[idx1] != '/'; idx1--);
    strcpy(compare1, path1 + idx1 + 1);
    compare1[idx1] = '\0';

    for (idx2 = strlen(path2) - 1; path2[idx2] != '/'; idx2--);
    strcpy(compare2, path2 + idx2 + 1);
    compare2[idx2] = '\0';

    return !(strcmp(compare2, compare1));

}

//중복된 이름의 파일 찾는 함수
int FindFileLinkedList(command_parameter *parameter, char *filename, char *date) {

    //backup 명령어, 이미 백업된 파일이 있다면(해시함수로 비교) 문구 출력
    //y 옵션도 구현. backup이고 y 옵션이 아니여야 문구 출력 및 -1 반환. y 옵션이면 맨 뒤의 0 반환
    if (parameter->majorcommand & CMD_BACK && !(parameter->commandopt & OPT_Y)) {
        //링크드리스트 탐색
        dirNode *currDir = trailer->prev; // 오름차순을 위해 뒤에서부터 시작
        while (currDir != header) { // 헤더까지 가면 종료
            fileNode *currFile = currDir->head;
            while (currFile != NULL) {

                //<2-4 예외처리8>인자로 입력받은 경로에 대해 기존 경로가 같고 내용이 같은 파일이 이미
                //백업 디렉토리에 존재할 경우, 백업하지 않고 ("원본 경로 already backuped to
                //"백업 경로" 출력
                if (CompareName(currFile->path, parameter->filename)) {
                    //해시 비교
                    if (CompareHash(currFile->path, parameter->filename)) {
                        printf("\"%s\" already backuped to \"%s\"\n", parameter->filename, currFile->path);
                        return -1;
                    }
                }
                currFile = currFile->next;
            }
            currDir = currDir->prev; // 예전 노드로 ㄱㄱ
        }

    }
        //remove 명령어
    else if (parameter->majorcommand & CMD_REM) {

        int cnt = 1;
        int num = 0;

        //링크드 리스트 탐색
        dirNode *currDir = trailer->prev; // 오름차순을 위해 뒤에서부터 시작
        while (currDir != header) { // 헤더까지 가면 종료
            fileNode *currFile = currDir->head;
            while (currFile != NULL) {
                if (CompareName(currFile->path, parameter->filename)) {
                    cnt++;
                }
                currFile = currFile->next;
            }
            currDir = currDir->prev;
        }
        //같은 파일이 한개란 뜻
        if (cnt == 2) {
            num = 1;
            RemoveFiles(parameter, num, date);
        }

            //같은 파일이 2개 이상이란 뜻
        else if (cnt > 2) {
            //초기화후 재탐색하면서 리스트 출력
            cnt = 1;
            dirNode *currDir2 = trailer->prev; // 오름차순을 위해 뒤에서부터 시작
            while (currDir2 != header) { // 헤더까지 가면 종료
                fileNode *currFile2 = currDir2->head;
                while (currFile2 != NULL) {
                    if (CompareName(currFile2->path, parameter->filename)) {
                        if (!(parameter->commandopt & OPT_A)) {
                            if (cnt == 1) {
                                printf("backup files of %s\n", parameter->filename);
                                printf("0. exit\n");

                                char *dirtimename = (char *) malloc(sizeof(char *) * PATHMAX);
                                strcpy(dirtimename, currDir2->dirname);

                                //디렉토리이름(날짜)를 출력해야하는데 아니면 인덱싱해서 출력
                                if (strlen(currDir2->dirname) != 12) {

                                    int start = strlen(backupPATH);
                                    strcpy(dirtimename, currDir2->path + start);
                                    dirtimename[12] = '\0';
                                }
                                printf("%d. %-13s\t%lubytes\n", cnt, dirtimename, sizeof(currFile2->path));
                                free(dirtimename);

                            } else {
                                char *dirtimename = (char *) malloc(sizeof(char *) * PATHMAX);
                                strcpy(dirtimename, currDir2->dirname);
                                if (strlen(currDir2->dirname) != 12) {

                                    int start = strlen(backupPATH);
                                    strcpy(dirtimename, currDir2->path + start);
                                    dirtimename[12] = '\0';
                                }
                                printf("%d. %-13s\t%lubytes\n", cnt, dirtimename, sizeof(currFile2->path));
                                free(dirtimename);
                            }
                        }
                        cnt++;
                    }
                    currFile2 = currFile2->next;
                }
                currDir2 = currDir2->prev;
            }
            //옵션이 -a가 아니면 프롬프트로 삭제할 파일 선택, -a이면 다 삭제
            if (!(parameter->commandopt & OPT_A)) {
                //프롬프트 호출해서 몇번 지울건지 정하고 num으로 반환
                num = Prompt(parameter);

                if (num == -1) {
                    DeleteEmptyDir();
                    exit(0);
                }
            }
            RemoveFiles(parameter, num, date);
        }
            //else(cnt가 1인경우)는 같은 파일이 없다. 즉 백업 디렉토리에 없다는 뜻
        else {
            //<3-4 에러처리3> 인자로 입력받은 경로와 일치하는 백업 파일이나 디렉토리가 없는 경우 에러 처리 후 프로그램 비정상 종료
            fprintf(stderr, "%s file is not in backup directory\n", parameter->filename);
            DeleteEmptyDir();
            exit(1);
        }
    }
        //recover 명령어
    else if (parameter->majorcommand & CMD_REC) {

        int cnt = 1;
        int num = 1;

        //링크드 리스트 탐색
        dirNode *currDir = trailer->prev; // 오름차순을 위해 뒤에서부터 시작
        while (currDir != header) { // 헤더까지 가면 종료
            fileNode *currFile = currDir->head;
            while (currFile != NULL) {
                if (CompareName(currFile->path, parameter->filename)) {
                    cnt++;
                }
                currFile = currFile->next;
            }
            currDir = currDir->prev;
        }
        //같은 파일이 한개란 뜻
        if (cnt == 2) {
            RecoverFiles(parameter, filename, num, date);
        }

            //같은 파일이 2개 이상이란 뜻
        else if (cnt > 2) {
            //초기화후 재탐색하면서 리스트 출력
            cnt = 1;
            dirNode *currDir2 = trailer->prev; // 오름차순을 위해 뒤에서부터 시작
            while (currDir2 != header) { // 헤더까지 가면 종료
                fileNode *currFile2 = currDir2->head;
                while (currFile2 != NULL) {
                    if (CompareName(currFile2->path, parameter->filename)) {

                        //옵션이 -l이면 이 리스트 출력을 안함
                        if (!(parameter->commandopt & OPT_L)) {
                            if (cnt == 1) {
                                printf("backup files of %s\n", parameter->filename);
                                printf("0. exit\n");

                                char *dirtimename = (char *) malloc(sizeof(char *) * PATHMAX);
                                if (strlen(currDir2->dirname) != 12) {

                                    int start = strlen(backupPATH);
                                    strcpy(dirtimename, currDir2->path + start);
                                    dirtimename[12] = '\0';
                                } else {
                                    strcpy(dirtimename, currDir2->dirname);
                                }
                                printf("%d. %-13s\t%lubytes\n", cnt, dirtimename, sizeof(currFile2->path));
                                free(dirtimename);

                            } else {
                                char *dirtimename = (char *) malloc(sizeof(char *) * PATHMAX);
                                if (strlen(currDir2->dirname) != 12) {

                                    int start = strlen(backupPATH);
                                    strcpy(dirtimename, currDir2->path + start);
                                    dirtimename[12] = '\0';
                                } else {
                                    strcpy(dirtimename, currDir2->dirname);
                                }
                                printf("%d. %-13s\t%lubytes\n", cnt, dirtimename, sizeof(currFile2->path));
                                free(dirtimename);
                            }
                        }
                        cnt++;
                    }
                    currFile2 = currFile2->next;
                }
                currDir2 = currDir2->prev;
            }
            num = cnt - 1;

            //프롬프트 호출해서 몇번 지울건지 정하고 num으로 반환, 옵션이 -l이면 num = cnt-1 (가장 최신 것 복구)
            if (!(parameter->commandopt & OPT_L)) {
                num = Prompt(parameter);

                if (num == -1) {
                    DeleteEmptyDir();
                    exit(0);
                }
            }
            RecoverFiles(parameter, filename, num, date);
        }
            //else(cnt가 1인경우)는 같은 파일이 없다 즉 백업 디렉토리에 없다는 뜻
        else {
            //<4-4 에러처리3> 인자로 입력받은 경로와 일치하는 백업 파일이나 디렉토리가 없는 경우 에러 처리 후 프로그램 비정상 종료
            fprintf(stderr, "%s file is not in backup directory\n", parameter->filename);
            DeleteEmptyDir();
            exit(1);
        }
    }
    return 0;
}

void RecoverFiles(command_parameter *parameter, char *filename, int num, char *date) {
    int fd1, fd2, fdlog;
    char forlog[STRMAX];
    int cnt = 1;
    char buf[BUFFER_SIZE];
    int length;

    dirNode *currDir = trailer->prev; // 똑같이 반복하면서 복구할 번호에 도달하면 recover
    while (currDir != header) {
        fileNode *currFile2 = currDir->head;
        while (currFile2 != NULL) {
            if (CompareName(currFile2->path, parameter->filename)) {
                if (cnt == num) {
                    if (CompareHash(currFile2->path, parameter->filename)) {
                        //<4-4 예외처리5> 인자로 입력받은 경로에 대한 백업 파일이 원본 파일과 내용이 같을 경우 복원을 진행하지 않고, (“원본 경로” not
                        //changed with “백업 경로”)를 표준 출력
                        printf("\"%s\" not changed with \"%s\"\n", parameter->filename, currFile2->path);
                        currFile2 = currFile2->next;
                        cnt++;
                        continue;
                    }
                    //파일 복구 문구 출력
                    printf("\"%s\" recovered to \"%s\"\n", currFile2->path, parameter->filename);

                    fd1 = open(currFile2->path, O_RDONLY);
                    fd2 = open(parameter->filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

                    while ((length = read(fd1, buf, BUFFER_SIZE)) > 0) {
                        write(fd2, buf, length);
                    }

                    //ssubak.log 파일을 열어 로그 작성
                    fdlog = open(backuplogPATH, O_RDWR | O_APPEND);
                    sprintf(forlog, "%s : \"%s\" recovered to \"%s\"\n", date, currFile2->path, parameter->filename);
                    write(fdlog, forlog, strlen(forlog));
                    close(fdlog);

                    //복구 후 백업 파일은 삭제하기 때문에 링크드리스트에서 제거
                    if (currFile2 == currDir->head) {
                        currDir->head = currFile2->next;
                        if (currDir->head != NULL) {
                            currDir->head->prev = NULL;
                        }
                    } else if (currFile2 == currDir->tail) {
                        currDir->tail = currFile2->prev;
                        currDir->tail->next = NULL;
                    } else {
                        currFile2->prev->next = currFile2->next;
                        currFile2->next->prev = currFile2->prev;
                    }

                    //백업파일은 삭제
                    remove(currFile2->path);

                    free(currFile2->path);
                    free(currFile2->filename);
                    free(currFile2);
                }
                cnt++;
            }
            currFile2 = currFile2->next;
        }
        currDir = currDir->prev;
    }
}

//프롬프트로 지울, 복구할 파일 선택
int Prompt(command_parameter *parameter) {
    char input[STRMAX];
    int num;

    //remove 명령어
    if (parameter->majorcommand & CMD_REM) {
        while (1) {
            //프롬프트 출력
            printf(">> ");
            fgets(input, sizeof(input), stdin);
            input[strlen(input)] = '\0';
            num = atoi(input);
            if (num == 0) {
                //exit
                return -1;
            } else {
                return num;
            }
        }
    }
        //recover 명령어
    else if (parameter->majorcommand & CMD_REC) {
        while (1) {
            //프롬프트 출력
            printf(">> ");
            fgets(input, sizeof(input), stdin);
            input[strlen(input) - 1] = '\0';
            num = atoi(input);
            if (num == 0) {
                //exit
                //
                remove(parameter->filename);
                return -1;
            } else {
                return num;
            }
        }
    }
        //list 명령어 구현
    else {
        return 0;
    }
}

//프로그램이 끝나기 전 빈 디렉토리를 찾아 삭제하는 코드
//맥북은 자동적으로 .DS_Store 파일이 생기니 이걸 없애주는 코드도 있음 좋음. 하지만 리눅스 기준이기에 패스!
void DeleteEmptyDir() {
    dirNode *currentDir = trailer->prev; // 뒤에서부터 순회
    dirNode *temp;

    while (currentDir != header) {
        if (currentDir->head == NULL) {

            temp = currentDir;
            currentDir = currentDir->prev; // 지우기 전에 미리 뒤로 한칸 이동

            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;

            // 디렉토리 삭제
            if (rmdir(temp->path) == 0) {
                free(temp->path);
                free(temp->dirname);
                free(temp);
            }
        } else {
            currentDir = currentDir->prev;
        }
    }
}

