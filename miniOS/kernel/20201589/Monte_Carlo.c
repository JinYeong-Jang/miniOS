#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define NUM_POINTS 100000
#define NUM_THREADS 4

int count_inside_circle = 0;
pthread_mutex_t mutex;

void* monte_carlo_func(void* arg){
    int num_points = *((int*)arg);
    unsigned int seed = (unsigned int)(time(NULL) ^ (uintptr_t)pthread_self());
    int local_count = 0;

    for(int i = 0; i < num_points; i++){
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if(sqrt(x * x + y * y) <= 1){
            local_count++;
        }
    }

    pthread_mutex_lock(&mutex);
    count_inside_circle += local_count;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void Monte_Carlo(){
    pthread_t threads[NUM_THREADS];
    int num_points_per_thread = NUM_POINTS / NUM_THREADS;

    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_create(&threads[i], NULL, monte_carlo_func, &num_points_per_thread);
    }

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    double pi_estimate = 4.0 * count_inside_circle / (NUM_THREADS * num_points_per_thread);
    printf("Estimated Value of Pi: %lf\n", pi_estimate);
}

