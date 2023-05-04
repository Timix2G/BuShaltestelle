#define _XOPEN_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

long long stepCounter;
long long points_in_circle;
int t_threads;

void* generatePoint(void* args){
    double* iters_per_thread = (double*) args;
    while(iters_per_thread[0] > 0){
        double x = drand48();
        double y = drand48();

        if(((x * x) + (y * y)) <= 1){
            points_in_circle++;
        }

        iters_per_thread[0]--;
        stepCounter++;
    }
    pthread_exit(NULL);
    t_threads--;
}

int main(){
    long long n_iters;
    stepCounter = 0;
    points_in_circle = 0;
    t_threads = 0;
    
    //user input for iterations (n) and threads (t)
    printf("How many iterations?: \n");
    scanf("%lld", &n_iters);

    printf("\nHow many threads?: \n");
    scanf("%d", &t_threads);

    if(n_iters > 0 && t_threads > 0){
        printf("n: %lld t: %d\nstarting computation", n_iters, t_threads);
    }else{
        perror("Iterations and/or threads can not be < 1");
        return 1;
    }

    pthread_t threadIDs[t_threads];
    double iters_per_thread = ceil(((long double)n_iters)/ ((long double)t_threads));
    double args = {iters_per_thread};

    //create threads
    for(int i = 0; i < t_threads; i++){
        if(pthread_create(&threadIDs[i], NULL, &generatePoint, &args) != 0){
            perror("Thread creation failed");
            return 1;
        }
    }

    long double pi = 0; 
    while(n_iters > stepCounter){
        pi = 4 * (((long double) points_in_circle) / (long double) stepCounter);
        printf("\n%lld Steps completed.\nCurrently, Pi is: %Lf\n, n_iters: %lld", stepCounter, pi, n_iters);
        nanosleep((const struct timespec[]){{1, 0}}, NULL);
    }
    return 0;
}