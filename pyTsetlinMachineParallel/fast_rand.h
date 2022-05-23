//
// Created by Yasser Phoulady on 2019-04-01.
//

#ifndef C_BITWISE_TSETLIN_MACHINE_FAST_RAND_H
#define C_BITWISE_TSETLIN_MACHINE_FAST_RAND_H

#include <stdint.h>

#define SARAM_RAND_MAX UINT32_MAX
#define THREADS 10 
#define TOTAL_RANDOM_NUMBERS 50000

static uint64_t const multiplier = 6364136223846793005u;
static uint64_t       mcg_state  = 0xcafef00dd15ea5e5u;

inline static uint32_t pcg32_fast() {
    uint64_t x = mcg_state;
    unsigned int count = (unsigned int) (x >> 61);	// 61 = 64 - 3

    mcg_state = x * multiplier;
    return (uint32_t) ((x ^ x >> 22) >> (22 + count));	// 22 = 32 - 3 - 7
}

static void pcg32_fast_init(uint64_t seed) {
   mcg_state = 2 * seed + 1;
   pcg32_fast();
}

#define FAST_RAND_MAX UINT32_MAX
#define fast_rand() pcg32_fast()

// Box–Muller transform
inline static int normal(double mean, double variance) {
    double u1 = (double) (fast_rand() + 1) / ((double) FAST_RAND_MAX + 1), u2 = (double) fast_rand() / FAST_RAND_MAX; // u1 in (0, 1] and u2 in [0, 1]
    double n1 = sqrt(-2 * log(u1)) * sin(8 * atan(1) * u2);
    return (int) round(mean + sqrt(variance) * n1);
}

inline static int binomial(int n, double p) {
    return normal(n * p, n * p * (1 - p));
}

// Knuth's random Poisson-distributed number
inline static int poisson(double lambda) {
    int k = 0;
    double l = exp(-lambda), p = 1;
    while (p > l) {
        ++k;
        p *= (double) fast_rand() / FAST_RAND_MAX;
    }
    return k - 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


int accessCount[THREADS] = {0,0,0,0,0,0,0,0,0,0};
uint64_t mcg_states_Array_threads[THREADS];
int RandomNumberArray[THREADS][TOTAL_RANDOM_NUMBERS];
int accessCount[THREADS];
int usedCounter;


void initRandomNumArray()
{   
    clock_t start = clock();

    // printf("Initializing num array ...\n");
    // FILE *fptr;
    // fptr = fopen("generatedRandomNumbers.txt","w");
    // if(fptr == NULL)
    // {
    //     printf("Error!");   
    //     exit(1);             
    // }
    // // srand(time(NULL));

    printf("Threads in Rand: %d\n", THREADS);
    printf("Threads in system: %d\n", omp_get_max_threads());
    

    // usedCounter = 0;

    for (int thread_number=0; thread_number<THREADS; thread_number++){
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int usec = tv.tv_usec;


        pcg32_fast_init(usec + thread_number);

        for (int rand_no_index =0; rand_no_index<TOTAL_RANDOM_NUMBERS; rand_no_index++){
            RandomNumberArray[thread_number][rand_no_index] = fast_rand();
        }
    }


    // clock_t file_start = clock();
    // for (int thread_number=0; thread_number<THREADS; thread_number++){
    //     fprintf(fptr,"THREAD NUMBER: %d\n", thread_number);
    //     for (int rand_no_index =0; rand_no_index<TOTAL_RANDOM_NUMBERS; rand_no_index++){
    //         fprintf(fptr,"%d\n",X[thread_number][rand_no_index]);
    //     }
    // }
    //     clock_t file_end = clock();
    //    fclose(fptr);   
    //    clock_t end = clock();
    //    printf("File input time: %lf s \n", (double) (file_end-file_start)/CLOCKS_PER_SEC);

    //    printf("Total time taken to initialise: %lf s \n", (double) (end-start)/CLOCKS_PER_SEC);
}

void shuffle(int *array, size_t n){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

    if (n>1){
        size_t i;
        for(i = n -1;i>0; i--){
            size_t j = (unsigned int)(drand48()*(i+1));
            int t = array[i];
            array[j] = array[i];
            array[i] = t;
        }
    }
}


uint32_t getRandomNumber()
{
    int thread_id = omp_get_thread_num();
    accessCount[thread_id]++;
    
    if (accessCount[thread_id] == (TOTAL_RANDOM_NUMBERS-1)){ 
        accessCount[thread_id]=1;
    }

    // printf("Thread %d Random Number %d \n", thread_id,(RandomNumberArray[thread_id][accessCount[thread_id] -1]));    

    return ((RandomNumberArray[thread_id][accessCount[thread_id] -1]));
}


void returnStates(){
    for (int i= 0; i < 10 ; i++){
        printf("Thread %d Random Number: %d\n", i , getRandomNumber());
    }
    printf("\n");    
}


#endif //C_BITWISE_TSETLIN_MACHINE_FAST_RAND_H
