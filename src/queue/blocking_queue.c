//
// Created by Administrator on 2024/6/2.
//
#include <malloc.h>
#include <sys/unistd.h>
#include <sys/errno.h>
#include "blocking_queue.h"

BQE *queue_e_create(void *data);

BlockingQueue *queue_create(int cap) {
    BlockingQueue *q = (BlockingQueue *) malloc(sizeof(BlockingQueue));
    if (cap <= 0) {
        cap = QUEUE_DEFAULT_CAPACITY;
    }
    q->cap = cap;
    q->size = 0;
    q->head = q->tail = queue_e_create(NULL);
    q->lock = NULL;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->notEmpty, NULL);
    pthread_cond_init(&q->notFull, NULL);
    return q;
}

void queue_offer(BlockingQueue *q, void *e) {
    pthread_mutex_t lock = q->lock;

    pthread_mutex_lock(&lock);
    //sleep(2);
    while (q->size >= q->cap) {
        pthread_cond_wait(&q->notFull, &lock);
    }
    BQE *E = queue_e_create(e);
    q->tail->next = E;
    q->tail = E;
    q->size++;
    pthread_cond_signal(&q->notEmpty);

    pthread_mutex_unlock(&lock);
}

int queue_offer_t(BlockingQueue *q, void *e, time_t seconds) {
    struct timespec timespec;
    clock_gettime(CLOCK_REALTIME, &timespec);
    timespec.tv_sec += seconds; // time unit: s * 1000 = ms * 1000 = us * 1000 = ns, 1 s = 1000 000 000 ns

    pthread_mutex_t lock = q->lock;
    pthread_mutex_lock(&lock);
    while (q->size >= q->cap) {
        int res = pthread_cond_timedwait(&q->notFull, &lock, &timespec);
        if (res == ETIMEDOUT) {
            pthread_mutex_unlock(&lock);
            return 1;
        }
    }
    BQE *E = queue_e_create(e);
    q->tail->next = E;
    q->tail = E;
    q->size++;
    pthread_cond_signal(&q->notEmpty);

    pthread_mutex_unlock(&lock);
    return 0;
}

void *queue_poll(BlockingQueue *q) {
    void *data;
    pthread_mutex_t lock = q->lock;
    pthread_mutex_lock(&lock);
    while (q->size <= 0) {
        pthread_cond_wait(&q->notEmpty, &lock);
    }
    BQE *e = q->head->next;
    data = e->data;
    q->head->next = e->next;
    free(e);
    q->size--;
    if (q->size == 0) {
        q->tail = q->head;
    }
    pthread_cond_signal(&q->notFull);

    pthread_mutex_unlock(&lock);

    return data;
}

void *queue_poll_t(BlockingQueue *q, time_t seconds) {
    void *data;

    struct timespec timespec;
    clock_gettime(CLOCK_REALTIME, &timespec);
    timespec.tv_sec += seconds;

    pthread_mutex_t lock = q->lock;
    pthread_mutex_lock(&lock);

    while (q->size <= 0) {
        int res = pthread_cond_timedwait(&q->notEmpty, &lock, &timespec);
        if (res == ETIMEDOUT) {
            pthread_mutex_unlock(&lock);
            return NULL;
        }
    }
    BQE *e = q->head->next;
    data = e->data;
    q->head->next = e->next;
    free(e);
    q->size--;
    if (q->size == 0) {
        q->tail = q->head;
    }
    pthread_cond_signal(&q->notFull);

    pthread_mutex_unlock(&lock);

    return data;
}

void queue_destroy(BlockingQueue *q) {
    pthread_mutex_t lock = q->lock;

    pthread_mutex_lock(&lock);

    while (q->head) {
        BQE *e = q->head;
        if (e->data) {
            free(e->data);
        }
        q->head = q->head->next;
        free(e);
    }
    pthread_cond_destroy(&q->notFull);
    pthread_cond_destroy(&q->notEmpty);
    free(q);

    pthread_mutex_unlock(&lock);

    pthread_mutex_destroy(&lock);
}

BQE *queue_e_create(void *data) {
    BQE *e = (BQE *) malloc(sizeof(BQE));
    e->next = NULL;
    e->data = data;
    return e;
}