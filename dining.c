#include "dining.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct dining {
  // TODO: Add your variables here
  int currentStudentCount;
  int capacity;
  bool cleaning;
  pthread_mutex_t lock;
  pthread_cond_t capacityCond;
  pthread_cond_t cleaningCond;
} dining_t;

dining_t *dining_init(int capacity) {
  // TODO: Initialize necessary variables
  dining_t *dining = malloc(sizeof(dining_t));
  dining->capacity = capacity;
  dining->currentStudentCount = 0;
  dining->cleaning = false;
  pthread_mutex_init(&dining->lock, NULL);
  pthread_cond_init(&dining->capacityCond, NULL);
  pthread_cond_init(&dining->cleaningCond, NULL);

  return dining;
}
void dining_destroy(dining_t **dining) {
  // TODO: Free dynamically allocated memory
  pthread_mutex_destroy(&(*dining)->lock);
  pthread_cond_destroy(&(*dining)->capacityCond);
  pthread_cond_destroy(&(*dining)->cleaningCond);
  free(*dining);
  *dining = NULL;
}

void dining_student_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);

  while (dining->currentStudentCount == dining->capacity || dining->cleaning) {
    pthread_cond_wait(&dining->capacityCond, &dining->lock);
  }

  dining->currentStudentCount++;
  pthread_mutex_unlock(&dining->lock);
}

void dining_student_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);

  dining->currentStudentCount--;

  pthread_cond_signal(&dining->capacityCond);

  if (dining->currentStudentCount == 0) {
    pthread_cond_signal(&dining->cleaningCond);
  }

  pthread_mutex_unlock(&dining->lock);
}

void dining_cleaning_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);

  while (dining->currentStudentCount != 0 || dining->cleaning) {
    pthread_cond_wait(&dining->cleaningCond, &dining->lock);
  }

  dining->cleaning = true;

  pthread_mutex_unlock(&dining->lock);
}

void dining_cleaning_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);

  dining->cleaning = false;

  pthread_cond_broadcast(&dining->capacityCond);
  pthread_cond_signal(&dining->cleaningCond);

  pthread_mutex_unlock(&dining->lock);
}
