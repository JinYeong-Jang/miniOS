#include "ssu_header.h"

int main(int argc, char *argv[]) {
    Init();

    char *arglist[10]; // 10개까지 인수처리 가능
    int argcnt = 0;

    //<1-4 예외처리1> 첫번째 인자를 입력안한 경우 에러처리후 비정상 종료
    if (argc == 1) {
        fprintf(stderr, "input error: no input\n");
        DeleteEmptyDir();
        exit(1);
    }

    //맨 첫번째 프로그램 ./ssu_backup을 제외한 인수들 옮기기
    for (int i = 1; i < argc; i++) {
        arglist[argcnt++] = argv[i];
    }

    //help 명렁어가 들어왔을때 처리
    if (!strcmp(arglist[0], "help")) {
        if (argcnt == 1) {
            help();
        } else if (argcnt == 2) {
            //help backup 이면 helpbackup 출력
            if (!strcmp(arglist[1], "backup")) {
                helpbackup();
            }
                //help remove 이면 helpremove 출력
            else if (!strcmp(arglist[1], "remove")) {
                helpremove();
            }
                //help cover 이면 helpcover 출력
            else if (!strcmp(arglist[1], "recover")) {
                helprecover();
            }
                //help list 이면 helplist 출력
            else if (!strcmp(arglist[1], "list")) {
                helplist();
            }
                //help help 이면 helphelp 출력
            else if (!strcmp(arglist[1], "help")) {
                helphelp();
            }
                //<6-4 예외처리1> 잘못된 내장명령어 입력시 에러처리 후 비정상종료
            else {
                fprintf(stderr, "input error: wrong command <backup | remove | recover | list | help>\n");
                DeleteEmptyDir();
                exit(1);
            }
        }
        //help 명렁어를 제외하고 나머지 명령어가 들어오면 파라미터를 초기화 하고 가공함
    } else if (!strcmp(arglist[0], "backup") || !strcmp(arglist[0], "remove") || !strcmp(arglist[0], "recover") ||
               !strcmp(arglist[0], "list")) {
        command_parameter parameter = {0, (char *) 0, (char *) 0, 0, (char *) 0};
        ParameterInit(&parameter);
        if (ParameterProcessing(argcnt, arglist, &parameter) == -1) {
            DeleteEmptyDir();
            exit(1);
        }

        strcpy(parameter.dirname, parameter.filename);

        //backup 명렁어가 들어오면
        if (parameter.majorcommand & CMD_BACK) {
            if (parameter.commandopt & OPT_R || parameter.commandopt & OPT_D) {
                BackupDirectory(&parameter, parameter.dirname, backupDirPATH, getDate());
            } else {
                BackupFile(&parameter, getDate());
            }
        }
            //remove 명령어가 들어오면
        else if (parameter.majorcommand & CMD_REM) {
            if (parameter.commandopt & OPT_R || parameter.commandopt & OPT_D) {
                RemRecDirectory(&parameter, parameter.dirname, backupDirPATH, getDate());
            } else {
                //기본 remove 명렁어
                CommandRemove(&parameter, getDate());
            }
        } else if (parameter.majorcommand & CMD_REC) {
            if (parameter.commandopt & OPT_D || parameter.commandopt & OPT_R) {
                RemRecDirectory(&parameter, parameter.dirname, backupDirPATH, getDate());
            } else {
                CommandRecover(&parameter, getDate());
            }


        } else if (parameter.majorcommand & CMD_LIST) {
            //list 명렁어 구현 안함
        }

    }
        //<1-4 예외처리2> 첫번째 인자로 잘못된 명령어를 입력학 경우 에러 처리 후 프로그램 비정상 종료
    else {
        fprintf(stderr, "input error : wrong command <backup | remove | recover | list | help>\n");
        DeleteEmptyDir();
        exit(1);
    }

    //프로그램 종료 직전에 빈 디렉토리 지우기
//    MakeMainLinkedList(backupPATH);
    DeleteEmptyDir();
    return 0;
}
