#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

unsigned long long  number_of_cpu, number_of_tosses, number_in_circle, toss, each_thread_toss;
//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex;
void* dart_toss(){
    unsigned long long number_in_circle_each_pthread = 0;
    double distance_squared, x, y;
    for (toss = 0; toss < number_of_tosses; toss++) {
        // x = random double between -1 and 1;
        x = 2 * (double) rand() / RAND_MAX - 1.0;
        // y = random double between -1 and 1;
        y = 2 * (double) rand() / RAND_MAX - 1.0;
        distance_squared = x*x + y*y;

        if (distance_squared <= 1)
            number_in_circle_each_pthread++;
    }
    pthread_mutex_lock(&mutex);
	number_in_circle += number_in_circle_each_pthread;
	pthread_mutex_unlock(&mutex);
    return NULL;
}


int main(int argc, char **argv)
{
    double pi_estimate, distance_squared, x, y;
    //unsigned long long  number_of_cpu, number_of_tosses, number_in_circle, toss;
    if ( argc < 2) {
        exit(-1);
    }
    number_of_cpu = atoi(argv[1]);
    number_of_tosses = atoi(argv[2]);
    if (( number_of_cpu < 1) || ( number_of_tosses < 0)) {
        exit(-1);
    }
    pthread_attr_t attr;
	pthread_t* threads = malloc(number_of_cpu * sizeof(pthread_t));
    number_in_circle = 0;
    each_thread_toss = number_of_tosses / number_of_cpu;
    
    pthread_attr_init(&attr);
	pthread_mutex_init(&mutex, NULL);

    // Creat pthreads
    for(int i = 0; i < number_of_cpu; i++){
        pthread_create(&threads[i], NULL, dart_toss, NULL);
    }

    for(int i = 0; i < number_of_cpu; i++){
        pthread_join(threads[i], NULL);
    }
    
    free(threads);
	pthread_mutex_destroy(&mutex);

    /*for (toss = 0; toss < number_of_tosses; toss++) {
        // x = random double between -1 and 1;
        x = 2 * (double) rand() / RAND_MAX - 1.0;
        // y = random double between -1 and 1;
        y = 2 * (double) rand() / RAND_MAX - 1.0;
        distance_squared = x*x + y*y;
        if (distance_squared <= 1)
            number_in_circle++;
    }*/
    pi_estimate = 4*number_in_circle/((double) number_of_tosses);

    printf("%f\n",pi_estimate);
    return 0;
}
