#ifndef TASK_H
#define TASK_H


#include "xenon/type.h"


#define TASK_RUNNING 0
#define TASK_READY 1
#define TASK_BLOCKED 2


struct task {
        uint32_t esp;           /* stack pointer */
        uint32_t ebp;           /* base pointer */
        uint32_t eip;           /* instructions pointer */
        uint32_t pid;
        uint8_t state;
        struct task *next; 
        uint32_t stack[1024];
};


void scheduler_init();
void schedule();
struct task *create_task(void (*entry)());

extern struct task *current_task;


#endif /* TASK_H */
