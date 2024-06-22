//
// Created by Administrator on 2024/6/2.
//

#ifndef TEST_C_BLOCKING_QUEUE_H
#define TEST_C_BLOCKING_QUEUE_H
#define QUEUE_DEFAULT_CAPACITY 8

#include <pthread.h>

typedef struct blocking_queue_element {
    void *data;
    struct blocking_queue_element *next;
} BQE;

typedef struct blocking_queue {
    int cap;
    int size;
    BQE *head;
    BQE *tail;
    pthread_mutex_t lock;
    pthread_cond_t notEmpty;
    pthread_cond_t notFull;
} BlockingQueue;

BlockingQueue *queue_create(int cap);

/**
 * 入队，当队列满时会一直阻塞
 * @param q 队列实例
 * @param e 元素数据
 */
void queue_offer(BlockingQueue *q, void *e);

/**
 * 支持超时的入队
 * @param q 队列实例
 * @param e 元素数据
 * @param seconds 超时时间，单位：s
 * @return 0或者1，0：添加到队列成功，1：在超时时间内未成功添加到队列
 */
int queue_offer_t(BlockingQueue *q, void *e, time_t seconds);

/**
 * 出队，如果队列为空则一直阻塞
 * @param q 队列实例
 * @return 出队的数据指针
 */
void *queue_poll(BlockingQueue *q);

void *queue_poll_t(BlockingQueue *q, time_t seconds);


void queue_destroy(BlockingQueue *q);

#endif //TEST_C_BLOCKING_QUEUE_H
