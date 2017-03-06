#ifndef MODULE 
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_sem.h>
#include "ece4220lab3.h"
// from ece4220lab3 
// int check_button(void); 1 if pressed
// void clear_button(void);

static void rt_tl1(int t); // traffic light 1
static void rt_tl2(int t); // traffic light 2
static void rt_pl(int t);  // pedestrian light

MODULE_LICENSE("GPL");

#define NUM_PERIODS 1000 
SEM sem;
RTIME period;
RT_TASK t1,t2,tp;
unsigned long *BasePtr, *PBDR, *PBDDR;	// pointers for port B DR/DDR

int init_module(void) {
	// Attempt to map file descriptor
	BasePtr = (unsigned long *) __ioremap(0x80840000, 4096, 0);
	if(NULL == BasePtr) 
	{
		printk(KERN_INFO "Unable to map memory space\n");
		return -1;
	}
	
	// Configure PORTB registers
	PBDR = BasePtr + 1;
	PBDDR = BasePtr + 5;

	// Red/B5 output	
	*PBDDR |= 0x20;
	// Yellow/B6 output
	*PBDDR |= 0x40;
	// Green/B7 output
	*PBDDR |= 0x80;
	// Red off 
	*PBDR &= ~(0x20);
	// Yellow off 
	*PBDR &= (0x40);
	// Green off
	*PBDR &= ~(0x80);
        // Set push button as input
	// Pedestrian button is PORTB0
	*PBDDR &= ~(1 << 0);
	//*PBDDR &= ~(1 << 4);
	
	// Start realtime timer
	rt_set_periodic_mode();
	period = start_rt_timer(nano2count(1000000));

	// Initialize semaphore 
	// Initial value is 1 because we want 1 task to do a rt_sem_wait() without blocking.
	rt_sem_init(&sem, 1);

	// Initialize rt tasks
	// 3rd to last arg is priority 0 = highest
	// Equal priorities
	rt_task_init(&t1, rt_tl1, 0, 256, 0, 0, 0);
	rt_task_resume(&t1);

	rt_task_init(&t2, rt_tl2, 0, 256, 0, 0, 0);
	rt_task_resume(&t2);

	rt_task_init(&tp, rt_pl, 0, 256, 0, 0, 0);
	rt_task_resume(&tp);

	printk(KERN_INFO "MODULE INSTALLED\n");
	return 0;
}

void cleanup_module(void)
{
	// Red/B5 output	
	*PBDDR |= 0x20;
	// Yellow/B6 output
	*PBDDR |= 0x40;
	// Green/B7 output
	*PBDDR |= 0x80;
	
	// Red off
	*PBDR &= ~(0x20);
	// Yellow off 
	*PBDR &= ~(0x40);
	// Green off
	*PBDR &= ~(0x80);

	rt_task_delete(&t1);
	rt_task_delete(&t2);
	rt_task_delete(&tp);
	stop_rt_timer();
	
	printk(KERN_INFO "MODULE REMOVED\n");
	return;
}

// Yellow LED
static void rt_tl1(int t) {
	while(1)
	{
		// Attempt to lock sem -- blocks until retrieved
		int ret = rt_sem_wait(&sem);
		// Yellow led on
		*PBDR |= 0x40;
		// wait/busy_wait
		rt_sleep(NUM_PERIODS*period);
		// Yellow LED off
		*PBDR &= ~(0x40);
		// unlock sem
		rt_sem_signal(&sem);
	}
}

// Green LED
static void rt_tl2(int t) {
	while(1)
	{
		// lock sem
		int ret = rt_sem_wait(&sem);
		// Green LED on
		*PBDR |= 0x80;
		// wait/busy_wait
		rt_sleep(NUM_PERIODS*period);
		// turn led off
		*PBDR &= ~(0x80);
		// unlock sem
		rt_sem_signal(&sem);
	}
}

static void rt_pl(int t) {
	while(1)
	{

		if(1 == check_button())
		{

			// lock sem
			int ret = rt_sem_wait(&sem);
			// RED LED on
			*PBDR |= 0x20;
			// wait/busy_wait
			rt_sleep(NUM_PERIODS*period);
			// turn led off
			*PBDR &= ~(0x20);
			// unlock sem
			rt_sem_signal(&sem);
			// Button pressed
			clear_button();
		}
		else
			rt_sleep(NUM_PERIODS*period);
	}
}
