#include "co.h"
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define STACK_SIZE 1024
#define MAX_SIZE 100

typedef enum co_status {
    CO_NEW = 1, // 新创建，还未执行过
    CO_RUNNING, // 已经执行过
    CO_WAITING, // 在 co_wait 上等待
    CO_DEAD, // 已经结束，但还未释放资源
} co_status;

struct co {
    const char *name;
    void (*func)(void *); // co_start 指定的入口地址和参数
    void *arg;

    enum co_status status; // 协程的状态
    struct co * waiter; // 是否有其他协程在等待当前协程
    jmp_buf context; // 寄存器现场 (setjmp.h)
    uint8_t stack[STACK_SIZE]; // 协程的堆栈
};

typedef struct co co;

co *cur;
co *co_pool[MAX_SIZE];
int co_cnt = 0;

static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg) {
    asm volatile (
#if __x86_64__
        "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
        : : "b"((uintptr_t)sp), "d"(entry), "a"(arg) : "memory"
#else
        "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
        : : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg) : "memory"
#endif
    );
}

co *co_create(const char *name, void (*func)(void *), void *arg) {
    co *newco = (co *)malloc(sizeof(co));
    newco->name = name;
    newco->func = func;
    newco->arg = arg;
    newco->status = CO_NEW;
    newco->waiter = NULL;

    return newco;
}

__attribute__((constructor))
void __cur_init__() {
    cur = co_create("main", NULL, NULL);
    co_pool[co_cnt++] = cur;
}

void co_destroy(co *co) {
    free(co);
}

__attribute__((destructor))
void __cur_dtor__() {
    co_destroy(cur);
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
    co *newco = co_create(name, func, arg);
    co_pool[co_cnt++] = newco;
    return newco;
}

void co_entry(co *co) {
    co->func(co->arg);
    co->status = CO_DEAD;
    if (co->waiter) {
        co->waiter->status = CO_RUNNING;
    }
    co_yield();
}

void co_wait(struct co *co) {
    co->waiter = cur;
    cur->status = CO_WAITING;

    while (co->status != CO_DEAD) {
        if (co->status == CO_NEW) {
            co->status = CO_RUNNING;
            cur = co;
            stack_switch_call(co->stack + STACK_SIZE, co_entry, (uintptr_t)co);
        }
        else if (co->status == CO_RUNNING) {
            cur = co;
            longjmp(co->context, 1);
        }
    }
    co_destroy(co);
}

void co_yield() {
    int val = setjmp(cur->context);
    if (val == 0) {
        co *available[MAX_SIZE];
        int cnt = 0;
        for (int i = 0; i < co_cnt; i++) {
            co *co = co_pool[i];
            if (co->status == CO_NEW || co->status == CO_RUNNING) {
                available[cnt++] = co;
            }
        }
        if (cnt == 0) {
            return;
        }
        int s = rand() % cnt;
        co *co = available[s];
        if (co->status == CO_NEW) {
            co->status = CO_RUNNING;
            cur = co;
            stack_switch_call(co->stack + STACK_SIZE, co_entry, (uintptr_t)co);
        }
        else if (co->status == CO_RUNNING) {
            cur = co;
            longjmp(co->context, 1);
        }
    }
    else {
        return;
    }
}
