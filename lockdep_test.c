#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static DEFINE_MUTEX(lock_a);
static DEFINE_MUTEX(lock_b);

static struct task_struct *thread1;
static struct task_struct *thread2;

static int thread_fn1(void *data)
{
    pr_info("Thread 1: trying to acquire lock A\n");
    mutex_lock(&lock_a);
    pr_info("Thread 1: acquired lock A\n");

    msleep(500);  // simulate work

    pr_info("Thread 1: trying to acquire lock B\n");
    mutex_lock(&lock_b); // deadlock happens here if Thread 2 holds lock_b
    pr_info("Thread 1: acquired lock B\n");

    mutex_unlock(&lock_b);
    mutex_unlock(&lock_a);
    return 0;
}

static int thread_fn2(void *data)
{
    pr_info("Thread 2: trying to acquire lock B\n");
    mutex_lock(&lock_b);
    pr_info("Thread 2: acquired lock B\n");

    msleep(500);  // simulate work

    pr_info("Thread 2: trying to acquire lock A\n");
    mutex_lock(&lock_a); // deadlock happens here if Thread 1 holds lock_a
    pr_info("Thread 2: acquired lock A\n");

    mutex_unlock(&lock_a);
    mutex_unlock(&lock_b);
    return 0;
}

static int __init deadlock_init(void)
{
    pr_info("Deadlock test module loaded\n");

    thread1 = kthread_run(thread_fn1, NULL, "thread1");
    thread2 = kthread_run(thread_fn2, NULL, "thread2");

    return 0;
}

static void __exit deadlock_exit(void)
{
    pr_info("Deadlock test module unloaded\n");
}

module_init(deadlock_init);
module_exit(deadlock_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Kernel module to test deadlock detection");
