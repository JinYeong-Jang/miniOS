#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;
} Process;

///RoundRobin
void roundRobin(Process processes[], int n, int quantum) {
    int time = 0, i;
    bool done = false;

    while (!done) {
        done = true;
        for (i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0) {
                done = false;
                if (processes[i].remaining_time > quantum) {
                    time += quantum;
                    processes[i].remaining_time -= quantum;
                } else {
                    time += processes[i].remaining_time;
                    printf("Process %d finishes at time %d\n", processes[i].pid, time);
                    processes[i].remaining_time = 0;
                }
            }
        }
    }
}

///multilevel Queue
void executeProcess(Process p) {
    printf("Process %d with priority %d executes for %d units\n", p.pid, p.priority, p.burst_time);
}

void multilevelQueue(Process processes[], int n) {
    // 우선순위 별로 정렬되어있다고 가정함
    for (int i = 0; i < n; i++) {
        executeProcess(processes[i]);
    }
}

///SRTF
int findNextProcess(Process processes[], int n, int current_time) {
    int min_time = INT_MAX, index = -1;
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
            if (processes[i].remaining_time < min_time) {
                min_time = processes[i].remaining_time;
                index = i;
            }
        }
    }
    return index;
}

void SRTF(Process processes[], int n) {
    int completed = 0, current_time = 0, i;

    while (completed != n) {
        int index = findNextProcess(processes, n, current_time);
        if (index != -1) {
            processes[index].remaining_time--;
            if (processes[index].remaining_time == 0) {
                completed++;
                printf("Process %d finishes at time %d\n", processes[index].pid, current_time + 1);
            }
            current_time++;
        } else {
            current_time++;
        }
    }
}
