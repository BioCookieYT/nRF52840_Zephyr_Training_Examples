/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "ncs_version.h"
#if NCS_VERSION_NUMBER < 0x20600
#include <zephyr/random/rand32.h>
#else
#include <zephyr/random/random.h>
#endif
#include <string.h>

#define THREAD0_STACKSIZE 1024
#define THREAD1_STACKSIZE 1024

#define THREAD0_PRIORITY 4
#define THREAD1_PRIORITY 4

#define COMBINED_TOTAL 40
int32_t increment_count = 0;
int32_t decrement_count = COMBINED_TOTAL;

void thread0(void)
{
    printk("Thread 0 started\n");
    while (1)
    {
        shared_code_section();
    }
}
void thread1(void)
{
    printk("Thread 1 started\n");
    while (1)
    {
         shared_code_section();
    }
}
K_MUTEX_DEFINE(test_mutex);

// Shared code run by both threads
void shared_code_section(void)
{
    /* STEP 12.1 - Lock the mutex */
    k_mutex_lock(&test_mutex, K_FOREVER);


    increment_count += 1;
    increment_count = increment_count % COMBINED_TOTAL;
    decrement_count -= 1;
    if (decrement_count == 0)
    {
        decrement_count = COMBINED_TOTAL;
    }

    /* STEP 12.2 - Unlock the mutex */
    k_mutex_unlock(&test_mutex); 
    if (increment_count + decrement_count != COMBINED_TOTAL)
    {
        printk("Race condition happend!\n");
        printk("Increment_count (%d) + Decrement_count (%d) = %d \n",
               increment_count, decrement_count, (increment_count + decrement_count));
        k_msleep(400 + sys_rand32_get() % 10);
    }
}


// Define and initialize threads
K_THREAD_DEFINE(thread0_id, THREAD0_STACKSIZE, thread0, NULL, NULL, NULL, THREAD0_PRIORITY, 0,
                5000);

K_THREAD_DEFINE(thread1_id, THREAD1_STACKSIZE, thread1, NULL, NULL, NULL, THREAD1_PRIORITY, 0,
                5000);