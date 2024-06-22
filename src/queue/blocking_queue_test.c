//
// Created by Administrator on 2024/6/22.
//
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <malloc.h>
#include "blocking_queue.h"

int *producer(void *q) {
    pthread_t t = pthread_self();
    printf("tid: %lu 0x%lx\n", (unsigned long) t, (unsigned long) t);
    int n = 200;
    while (n--) {
        int *e = (int *) malloc(sizeof(int));
        *e = 200 - n;
        queue_offer(q, e);
        printf("offer：%d\n", *e);
    }
    return NULL;
}

int *producer_timed_wait(void *q) {
    pthread_t t = pthread_self();
    printf("tid: %lu 0x%lx\n", (unsigned long) t, (unsigned long) t);
    int n = 200;
    while (n--) {
        int *e = (int *) malloc(sizeof(int));
        *e = 200 - n;
        int res = queue_offer_t(q, e, 1);
        if (res == 1) {
            break;
        } else {
            printf("offer-timed_wait：%d\n", *e);
        }
    }
    return NULL;
}

int *consumer(void *q) {
    pthread_t t = pthread_self();
    printf("tid: %lu 0x%lx\n", (unsigned long) t, (unsigned long) t);
    int n = 200;
    while (n--) {
        int *e = queue_poll(q);
        printf("poll：%d\n", *e);
        free(e);
    }
    return NULL;
}

int *consumer_timed_wait(void *q) {
    pthread_t t = pthread_self();
    printf("tid: %lu 0x%lx\n", (unsigned long) t, (unsigned long) t);
    while (1) {
        int *e = queue_poll_t(q, 2);
        if (!e) {
            break;
        }
        printf("poll-timed_wait：%d\n", *e);
        free(e);
    }
    return NULL;
}

void queue_test() {
    BlockingQueue *q = queue_create(10);

    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, NULL, (void *(*)(void *)) producer, q);
    pthread_create(&t2, NULL, (void *(*)(void *)) consumer, q);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    queue_destroy(q);
}

void queue_test_timed_wait() {
    BlockingQueue *q = queue_create(10);

    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, NULL, (void *(*)(void *)) producer_timed_wait, q);
    pthread_create(&t2, NULL, (void *(*)(void *)) consumer_timed_wait, q);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    queue_destroy(q);
}