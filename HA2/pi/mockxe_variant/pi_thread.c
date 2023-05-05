#define _POSIX_C_SOURCE 199506L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


typedef struct thread_data {
    unsigned int seed; // seed for rand_r()
    int n; // completed iterations
    int m; // total iterations
    int k; // points inside quarter circle
} thread_data;


void* thread_function(void *arg) {
    thread_data *tdata = (thread_data *) arg;
    double x;
    double y;

    for (int i = 0; i < tdata->m; ++i) {
        x = (double) rand_r(&tdata->seed) / RAND_MAX;
        y = (double) rand_r(&tdata->seed) / RAND_MAX;

        if (x*x + y*y <= 1)
            tdata->k++;

        tdata->n++;
    }

    return NULL;
}


int main(void) {

    int n;
    int t;

    printf("enter iteration amount (n): ");
    scanf("%d", &n);

    printf("enter thread amount (t): ");
    scanf("%d", &t);

    if (n < 1 || t < 1) {
        printf("\ninvalid input!\n");
        exit(1);
    }

    printf("\nstarting with %d threads and %d iterations...\n", t, n);


    pthread_t tids[t];
    thread_data tdatas[t];

    int m = n / t; // calculate iterations per thread
    int e = n % t; // calculate remaining (extra) iterations

    for (int i = 0; i < t; ++i) {
        tdatas[i].seed = rand(); // create pseudo random seed
        tdatas[i].k = 0;
        tdatas[i].n = 0;
        tdatas[i].m = m; // set iterations for thread
        if (i < e) // if first e threads...
            tdatas[i].m++; // ...assign one extra iteration

        int ret = pthread_create(&tids[i], NULL, &thread_function, (void *) &tdatas[i]);
        if (ret != 0) {
            printf("thread creation failed with error %d\n", ret);
            exit(1);
        }
    }

    int threads_done;
    do {
        sleep(1);

        threads_done = 0;
        int t_k = 0;
        int t_n = 0;

        for (int i = 0; i < t; ++i) {
            t_k += tdatas[i].k;
            t_n += tdatas[i].n;

            if (tdatas[i].n == tdatas[i].m)
                threads_done++;
        }

        double a = 4 * (double) t_k / t_n;

         printf("threads finished: %d/%d\t"
               "current iterations (n): %d\t"
               "current points inside quarter circle (k): %d\t"
               "pi approximation: %f\n",
               threads_done, t, t_n, t_k, a);

    } while (threads_done < t);

}
