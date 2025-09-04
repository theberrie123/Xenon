#include "sched/task.h"
#include "xenon/memory.h"


struct task *task_list = 0;
struct task *current_task = 0;
uint32_t next_pid = 1;


static void add_task(struct task *task)
{
        if (!task_list) {
                task_list = task;
                task->next = task;
        } else {
                task->next = task_list->next;
                task_list->next = task;
        }
}

struct task *create_task(void (*entry)())
{
        struct task *task = (struct task *)kmalloc(sizeof(struct task));
        task->pid = next_pid++;
        task->state = TASK_READY;

        task->esp = (uint32_t)&task->stack[1024];
        task->ebp = task->esp;

        task->esp -= 4;
        *((uint32_t *)task->esp) = (uint32_t)entry;

        add_task(task);
        return task;
}


void switch_task(uint32_t *old_esp, uint32_t *new_esp)
{
    __asm__ __volatile__ (
        "movl %%esp, (%0)\n"
        "movl %%ebp, 4(%0)\n"

        "movl (%1), %%esp\n"
        "movl 4(%1), %%ebp\n"

        "ret\n"
        :
        : "r"(old_esp), "r"(new_esp)
        : "memory"
    );
}



void schedule()
{
        if (!current_task) {
                return;
        }

        struct task *prev = current_task;
        struct task *next = current_task->next;

        while (next->state != TASK_READY) {
                next = next->next;
        }

        current_task = next;

        __asm__ __volatile__ ("cli");
        switch_task(&prev->esp, &next->esp);
        __asm__ __volatile__("sti");
}

void scheduler_init()
{
        current_task = task_list;
}
