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

#define PRODUCER_STACKSIZE 1024
#define CONSUMER_STACKSIZE 1024

#define PRODUCER_PRIORITY 5
#define CONSUMER_PRIORITY 4

K_SEM_DEFINE(instance_monitor_sem, 10, 10);

volatile uint32_t available_instance_count = 10;

// Function for getting access of resource
void get_access(void)
{
    k_sem_take(&instance_monitor_sem, K_FOREVER);

    available_instance_count--;
    printk("Resource taken and available_instance_count = %d\n", available_instance_count);
}

// Function for releasing access of resource
void release_access(void)
{
    available_instance_count++;
    printk("Resource given and available_instance_count = %d\n", available_instance_count);
    k_sem_give(&instance_monitor_sem);
}

void producer(void)
{
    printk("Producer thread started\n");
    while (1)
    {
        release_access();
        // Assume the resource instance access is released at this point
#if NCS_VERSION_NUMBER < 0x20600
        k_msleep(500 + sys_rand32_get() % 10);
#else
        uint32_t rand_val;
        sys_rand_get(&rand_val, sizeof(rand_val));
        k_msleep(500 + rand_val % 10);
#endif
    }
}

void consumer(void)
{
    printk("Consumer thread started\n");
    while (1)
    {
        get_access();
        // Assume the resource instance access is released at this point
#if NCS_VERSION_NUMBER < 0x20600
        k_msleep(sys_rand32_get() % 10);
#else
        uint32_t rand_val;
        sys_rand_get(&rand_val, sizeof(rand_val));
        k_msleep(rand_val % 10);
#endif
    }
}

// Define and initialize threads
K_THREAD_DEFINE(producer_id, PRODUCER_STACKSIZE, producer, NULL, NULL, NULL, PRODUCER_PRIORITY, 0,
                0);

K_THREAD_DEFINE(consumer_id, CONSUMER_STACKSIZE, consumer, NULL, NULL, NULL, CONSUMER_PRIORITY, 0,
                0);