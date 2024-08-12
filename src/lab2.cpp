#include "lab2.h"
#include <cstring>
#include <semaphore.h>

// Thread order: acde -> bcde -> hgde -> hgfe -> hik -> m

#define NUMBER_OF_THREADS 11

// thread identifiers
pthread_t tid[NUMBER_OF_THREADS];

// critical section lock
pthread_mutex_t lock;

// semaphores for sequential threads
sem_t semC, semD, semE, semG, semH, semF;

int err;

unsigned int lab2_thread_graph_id() { return 4; }

const char *lab2_unsynchronized_threads() { return "bcde"; }

const char *lab2_sequential_threads() { return "efgh"; }

void console_log(char c) {
  for (int i = 0; i < 3; ++i) {
    pthread_mutex_lock(&lock);
    std::cout << c << std::flush;
    pthread_mutex_unlock(&lock);
    computation();
  }
}

void *thread_a(void *ptr);
void *thread_b(void *ptr);
void *thread_c(void *ptr);
void *thread_d(void *ptr);
void *thread_e(void *ptr);
void *thread_f(void *ptr);
void *thread_g(void *ptr);
void *thread_h(void *ptr);
void *thread_i(void *ptr);
void *thread_k(void *ptr);
void *thread_m(void *ptr);

void *thread_a(void *ptr) {
  console_log('a');
  return ptr;
}

void *thread_b(void *ptr) {
  console_log('b');
  return ptr;
}

void *thread_c(void *ptr) {
  // 1
  console_log('c');
  pthread_join(tid[0], NULL);
  sem_wait(&semC);
  sem_wait(&semC);

  // 2
  pthread_create(&tid[1], NULL, thread_b, NULL);
  sem_post(&semD);
  sem_post(&semE);
  console_log('c');
  pthread_join(tid[1], NULL);
  sem_post(&semE);

  pthread_join(tid[3], NULL);
  pthread_join(tid[4], NULL);
  return ptr;
}

void *thread_d(void *ptr) {
  // 1
  console_log('d');
  sem_post(&semC);

  // 2
  sem_wait(&semD);
  console_log('d');
  sem_post(&semE);

  // 3
  sem_wait(&semD);
  console_log('d');
  sem_post(&semE);

  return ptr;
}

void *thread_e(void *ptr) {
  // 1
  console_log('e');
  sem_post(&semC);

  // 2
  sem_wait(&semE);
  console_log('e');

  // 3
  sem_wait(&semE);
  sem_wait(&semE);
  sem_post(&semD);
  pthread_create(&tid[6], NULL, thread_g, NULL);
  pthread_create(&tid[7], NULL, thread_h, NULL);
  console_log('e');

  // 4
  sem_wait(&semE);
  sem_wait(&semE);
  pthread_create(&tid[5], NULL, thread_f, NULL);

  for (int i = 0; i < 3; ++i) {
    sem_wait(&semE);
    pthread_mutex_lock(&lock);
    std::cout << 'e' << std::flush;
    pthread_mutex_unlock(&lock);
    sem_post(&semH);
  }

  sem_wait(&semE);
  sem_post(&semH);

  pthread_join(tid[5], NULL);
  pthread_join(tid[6], NULL);
  pthread_join(tid[7], NULL);
  return ptr;
}

void *thread_f(void *ptr) {
  // 4
  for (int i = 0; i < 3; ++i) {
    sem_wait(&semF);
    pthread_mutex_lock(&lock);
    std::cout << 'f' << std::flush;
    pthread_mutex_unlock(&lock);
    sem_post(&semE);
  }
  return ptr;
}

void *thread_g(void *ptr) {
  // 3
  console_log('g');
  sem_post(&semE);

  // 4
  for (int i = 0; i < 3; ++i) {
    sem_wait(&semG);
    pthread_mutex_lock(&lock);
    std::cout << 'g' << std::flush;
    pthread_mutex_unlock(&lock);
    sem_post(&semF);
  }
  return ptr;
}

void *thread_h(void *ptr) {
  // 3
  console_log('h');
  sem_post(&semE);

  // 4
  for (int i = 0; i < 3; ++i) {
    sem_wait(&semH);
    pthread_mutex_lock(&lock);
    std::cout << 'h' << std::flush;
    pthread_mutex_unlock(&lock);
    sem_post(&semG);
  }

  // 5
  sem_wait(&semH);
  pthread_create(&tid[8], NULL, thread_i, NULL);

  pthread_create(&tid[9], NULL, thread_k, NULL);
  console_log('h');

  // 6
  pthread_join(tid[8], NULL);
  pthread_join(tid[9], NULL);
  pthread_create(&tid[10], NULL, thread_m, NULL);

  pthread_join(tid[10], NULL);
  return ptr;
}

void *thread_i(void *ptr) {
  console_log('i');
  return ptr;
}

void *thread_k(void *ptr) {
  console_log('k');
  return ptr;
}

void *thread_m(void *ptr) {
  console_log('m');
  return ptr;
}

int lab2_init() {
  pthread_mutex_init(&lock, NULL);

  sem_init(&semC, 0, 0);
  sem_init(&semD, 0, 0);
  sem_init(&semE, 0, 0);
  sem_init(&semF, 0, 0);
  sem_init(&semG, 0, 0);
  sem_init(&semH, 0, 0);

  // Start thread_c
  pthread_create(&tid[2], NULL, thread_c, NULL);
  pthread_create(&tid[0], NULL, thread_a, NULL);
  pthread_create(&tid[3], NULL, thread_d, NULL);
  pthread_create(&tid[4], NULL, thread_e, NULL);

  pthread_join(tid[2], NULL);

  // free resources
  pthread_mutex_destroy(&lock);
  sem_destroy(&semC);
  sem_destroy(&semE);
  sem_destroy(&semD);
  sem_destroy(&semG);
  sem_destroy(&semH);
  sem_destroy(&semF);

  return 0;
}
