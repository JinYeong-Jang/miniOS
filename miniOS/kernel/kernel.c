#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

void file_system();

int main() {
    print_minios("\n[MiniOS SSU] Hello, World!\n");
    file_system();
    // 메모리 해제
    free(input);
    print_minios("[MiniOS SSU] MiniOS Shutdown........");

    return 0;
}
