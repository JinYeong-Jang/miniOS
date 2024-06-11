#define OPENSSL_API_COMPAT 0x10100000L
#ifndef SSU_PROJECT_1_SSU_HEADER_H
#define SSU_PROJECT_1_SSU_HEADER_H

#if !defined(_OSD_POSIX) && !defined(__DJGPP__)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     //getenv
#include <sys/stat.h>   //mkdir
#include <string.h>     //strcmp
#include <fcntl.h>      //open
#include <limits.h>  //PATH_MAX
#include <time.h>   //localtime, time
#include <dirent.h> //scandir
#include <pwd.h>    //struct passwd *pw = getpwuid(uid);
#include <openssl/md5.h> //해시값


#define PATHMAX 4096
#define STRMAX 4096
#define BUFFER_SIZE 1024


#define CMD_BACK           0b00001
#define CMD_REM            0b00010
#define CMD_REC            0b00100
#define CMD_LIST           0b01000
#define NOT_CMD            0b00000


#define OPT_A        0b000001
#define OPT_D        0b000010
#define OPT_L        0b000100
#define OPT_N        0b001000
#define OPT_R        0b010000
#define OPT_Y        0b100000
#define NO_OPT       0b000000


char exePATH[PATHMAX];
char homePATH[PATHMAX];
char backupPATH[PATHMAX];
char backupDirPATH[PATHMAX];
char backuplogPATH[PATHMAX * 2];
int fd;
char *commandata[10];


typedef struct fileNode {
    char *path;
    char *filename;

    struct fileNode *next;
    struct fileNode *prev;
} fileNode;

typedef struct dirNode {
    char *path;
    char *dirname;
    int sub;

    fileNode *head;
    fileNode *tail;

    struct dirNode *next;
    struct dirNode *prev;
} dirNode;

//백업 디렉토리를 관리하기 위한 헤더와 트레일러
dirNode *header;
dirNode *trailer;

typedef struct command_parameter {
    int majorcommand;
    char *filename;
    char *dirname;
    int commandopt;
    char *nfilename;
    char *argv[10];
} command_parameter;


void Init();

void MakeMainLinkedList(char *path);

void MakeLinkedList(char *path, char *longpath);

void help(void);

void helprecover();

void helpbackup();

void helpremove();

void helplist();

void helphelp();

void ParameterInit(command_parameter *parameter);

int ParameterProcessing(int argcnt, char **arglist, command_parameter *parameter);

void ConvertPath(char *argpath, char *newpath);

char *getDate();

int BackupFile(command_parameter *parameter, char *date);

void BackupDirectory(command_parameter *parameter, char *path, char *backuppath, char *date);

void CommandRemove(command_parameter *parameter, char *date);

void RemoveFiles(command_parameter *parameter, int num, char *date);

void RemRecDirectory(command_parameter *parameter, char *path, char *backuppath, char *date);

void CommandRecover(command_parameter *parameter, char *date);

void RecoverFiles(command_parameter *parameter, char *filename, int num, char *date);

int reverse_alphasort(const struct dirent **a, const struct dirent **b);

int FindFileLinkedList(command_parameter *parameter, char *filename, char *date);

int Prompt(command_parameter *parameter);

void DeleteEmptyDir();

int CompareHash(char *path1, char *path2);

int CompareName(char *path1, char *path2);

void ConvertHash(char *path, char *result_hash);

#endif
#endif