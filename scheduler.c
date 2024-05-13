#include "scheduler.h"

int main() {
    Process processes[] = {{1, 0, 10, 10}, {2, 2, 5, 5}, {3, 3, 8, 8}};
    int n = sizeof(processes)/sizeof(processes[0]);
    int quantum = 3;

    roundRobin(processes, n, quantum);
    SRTF(processes, n);
    multilevelQueue(processes, n);
    return 0;
}