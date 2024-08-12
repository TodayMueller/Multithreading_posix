#include "lab2.h"
#include <cstring>
#include <semaphore.h>

#define NUMBER_OF_THREADS 5

// thread identifiers
pthread_t tid[NUMBER_OF_THREADS];
// critical section lock
pthread_mutex_t lock;
// semaphores for sequential threads
sem_t semD, semE;

int err;

unsigned int lab2_thread_graph_id() 
{
    return 999;
}

const char* lab2_unsynchronized_threads()
{
    return "bcd";
}

const char* lab2_sequential_threads()
{
    return "de";
}

void *thread_b(void *ptr);
void *thread_c(void *ptr);
void *thread_d(void *ptr);
void *thread_e(void *ptr);


void *thread_a(void *ptr)
{
    for (int i = 0; i < 3; ++i) {
        pthread_mutex_lock(&lock);
        std::cout << "a" << std::flush;
        pthread_mutex_unlock(&lock);
        computation();
    }
    err = pthread_create(&tid[1], NULL, thread_b, NULL);
    if (err != 0)
        std::cerr << "Can't create thread. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[2], NULL, thread_c, NULL);
    if (err != 0)
        std::cerr << "Can't create thread. Error: " << strerror(err) << std::endl;
    err = pthread_create(&tid[3], NULL, thread_d, NULL);
    if (err != 0)
        std::cerr << "Can't create thread. Error: " << strerror(err) << std::endl;
    // wait for thread D to finish
    pthread_join(tid[3], NULL);
    return  ptr;
}

void *thread_b(void *ptr)
{
    for (int i = 0; i < 3; ++i) {
        pthread_mutex_lock(&lock);
        std::cout << "b" << std::flush;
        pthread_mutex_unlock(&lock);
        computation();
    }
    return ptr;
}

void *thread_c(void *ptr)
{
    for (int i = 0; i < 3; ++i) {
        pthread_mutex_lock(&lock);
        std::cout << "c" << std::flush;
        pthread_mutex_unlock(&lock);
        computation();
    }
    // wait for thread B to finish
    pthread_join(tid[1], NULL);
    return ptr;
}


void *thread_d(void *ptr)
{
    // perform computations
    for (int i = 0; i < 3; ++i) {
        pthread_mutex_lock(&lock);
        std::cout << "d" << std::flush;
        pthread_mutex_unlock(&lock);
        computation();
    }
    // wait for thread C to finish
    pthread_join(tid[2], NULL);
    // start thread E
    err = pthread_create(&tid[4], NULL, thread_e, NULL);
    if (err != 0)
        std::cerr << "Can't create thread. Error: " << strerror(err) << std::endl;
    // perform computations
    for (int i = 0; i < 3; ++i) {
        sem_wait(&semD);
        pthread_mutex_lock(&lock);
        std::cout << "d" << std::flush;
        pthread_mutex_unlock(&lock);
        computation();
        sem_post(&semE);
    }
    // wait for thread E to finish
    pthread_join(tid[4], NULL);
    return ptr;
}

void *thread_e(void *ptr)
{
    // perform computations
    for (int i = 0; i < 3; ++i) {
        sem_wait(&semE);
        pthread_mutex_lock(&lock);
        std::cout << "e" << std::flush;
        pthread_mutex_unlock(&lock);
        computation();
        sem_post(&semD);
    }
    return ptr;
}


int lab2_init()
{
    // initilize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        std::cerr << "Mutex init failed" << std::endl;
        return 1;
    }
    // initialize semaphores
    // THIS CODE WILL NOT RUN ON MacOS!
    // MacOS doesn't support unnamed semaphores, so named semaphores should be used instead
    if ( sem_init(&semD, 0, 1) != 0 ) {
        std::cerr << "Semaphore #1 init failed" << std::endl;
        return 1;
    }
    if ( sem_init(&semE, 0, 0) != 0 ) {
        std::cerr << "Semaphore #2 init failed" << std::endl;
        return 1;
    }

    // start the first thread
    err = pthread_create(&tid[0], NULL, thread_a, NULL);
    if (err != 0)
        std::cerr << "Can't create thread. Error: " << strerror(err) << std::endl;
    
    // ... and wait for it to finish
    pthread_join(tid[0], NULL);
    
    // free resources
    pthread_mutex_destroy(&lock);
    sem_destroy(&semD);
    sem_destroy(&semE);
    
    std::cout << std::endl;
    // success
    return 0;
}
