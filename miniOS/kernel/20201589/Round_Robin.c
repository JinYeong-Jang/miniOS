#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define TIME_QUANTUM 4
#define MAX_PROCESSES 5

typedef struct Process{
    int id;
    int burst_time;
    int arrival_time;
    int executed_time;
} Process;

typedef struct Node{
    Process* process;
    struct Node* next;
} Node;

typedef struct{
    Node* front;
    Node* rear;
} Queue;

void init_queue(Queue* q){
    q->front = q->rear = NULL;
}

bool is_empty(Queue* q){
    return q->front == NULL;
}

void enqueue(Queue* q, Process* p){
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->process = p;
    new_node -> next = NULL;
    if(is_empty(q)){
        q->front = q->rear = new_node;
    }else{
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

Process* dequeue(Queue* q){
    if(is_empty(q)) return NULL;
    Node* temp = q->front;
    Process* p = temp->process;
    q->front = q->front->next;
    if(q->front == NULL){
        q->rear = NULL;
    }
    free(temp);
    return p;
}
void Scheduling(Process processes[], int n) {
    Queue q;
    init_queue(&q);

    int current_time = 0;
    int processed = 0;
    
    while (processed < n){
        for(int i = 0; i < n; i++){
            if(processes[i].arrival_time <= current_time && processes[i].executed_time == 0){
                enqueue(&q, &processes[i]);
                processes[i].executed_time = -1;
            }
        }
        if(!is_empty(&q)){
            Process* p = dequeue(&q);
            if(p->burst_time > TIME_QUANTUM){
                current_time += TIME_QUANTUM;
                p->burst_time -= TIME_QUANTUM;
                enqueue(&q, p);
            }else{
                current_time += p->burst_time;
                p->burst_time = 0;
                processed++;
                printf("Process %d finished at time %d\n", p->id, current_time);
            }
        }else{
            current_time++;
        }
    }
}

void Round_Robin(){
    Process processes[MAX_PROCESSES] = {
        {1, 10, 0, 0},
        {2, 4, 1, 0},
        {3, 5, 2, 0},
        {4, 3, 3, 0},
        {5, 7, 4, 0}
    };

    printf("Starting Round Robin Scheduling..\n");
    Scheduling(processes, MAX_PROCESSES);
}

