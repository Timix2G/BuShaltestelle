#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

long long stepCounter = 0;

//idfk which/any return type 
void generatePoint(long long *k, long long n){

    while(n > 0){
        double x = drand48();
        double y = drand48();

        if(((x * x) + (y * y)) <= 1){
            k++;
        }

        n--;
        stepCounter++;
    }
    pthread_exit(NULL);
}

int main(){
    long long n;
    long long t;
    long long k;
    
    //user input for iterations (n) nad threads (t)
    printf("How many iterations?: \n");
    scanf("%lld", &n);
    printf("\nHow many threads?: \n");
    scanf("%lld", &t);

    if(n > 0 && t > 0){
        printf("n: %d t: %d\nstarting computation", n, t);
    }else{
        perror("Iterations and/or threads can not be < 1");
        return 1;
    }

    pthread_t threadIDs[t];
    long long *args = {k, n/t};

    //create threads
    for(int i = 0; i < t; i++){
        if(pthread_create(threadIDs[i], NULL, &generatePoint, *args) != 0){
            perror("Thread creation failed");
            return 1;
        }
    }

    long double pi = 0; 
    while(/*idfk*/){
        pi = 4 * (((long double) k) / (long double) n);
        print("%lld Steps completed.\nCurrently, Pi is: %lf\n", stepCounter, pi);
        nanosleep((const struct timespec[]){{1, 0}}, NULL);
    }

    return 0;
}