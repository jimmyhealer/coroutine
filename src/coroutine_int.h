#ifndef __COROUTINE_INT_H__
#define __COROUTINE_INT_H__

#include "context.h"

typedef int (*job_t)(struct context *__context, void *args);

/*
 * task_struct maintain the coroutine or task object.
 */
struct task_struct {
    /* job information */
    struct cr *cr;
    int tfd; /* task fd */
    job_t job;
    void *args;
    struct context context; /* defined at context.h */
};

#define container_of(ptr, type, member)                                        \
    __extension__({                                                            \
        const __typeof__(((type *)0)->member) *__mptr = (ptr);                 \
        (type *)((char *)__mptr - offsetof(type, member));                     \
    })

#define task_of(__context) container_of(__context, struct task_struct, context)

/* runqueue */

// Need to be power of two
#define RINGBUFFER_SIZE 16

struct rq {
    unsigned int out, in; /* dequeue at out, enqueue  at in*/
    unsigned int mask; /* the size is power of two, so mask will be size - 1 */
    struct task_struct *r[RINGBUFFER_SIZE];
};

void rq_init(struct rq *rq);
int rq_enqueue(struct rq *rq, struct task_struct *task);
struct task_struct *rq_dequeue(struct rq *rq);

/* main data structure */

#define MAX_CR_TABLE_SIZE 10

struct cr {
    unsigned long size; /* number of the task in this scheduler */
    int crfd; /* coroutine fd number */
    int flag; /* Which type of scheduler, FIFO or CFS */

    // scheduler - chose by the flag
    struct rq rq;
    struct task_struct *current;

    /* sched operations */
    struct task_struct *(*pick_next_task)(struct cr *cr);
    int (*put_prev_task)(struct cr *cr, struct task_struct *prev);
    int (*job_to_proc)(struct cr *cr, struct task_struct *p); // return PID
};

struct cr_struct {
    int size;
    struct cr *table[MAX_CR_TABLE_SIZE];
};

void sched_init(struct cr *cr);
// call by coroutine_add
int schedule(struct cr *cr, job_t func, void *args);

#endif /* __COROUTINE_INT_H__ */