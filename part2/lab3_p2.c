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

static void rt_tl1(int t) {
	// lock sem
	// led on
	// wait/busy_wait
	// turn led off
	// unlock sem
}

static void rt_tl2(int t) {
	// lock sem
	// led on
	// wait/busy_wait
	// turn led off
	// unlock sem
}

static void rt_tp(int t) {
	// lock sem
	// led on
	// wait/busy_wait
	// turn led off
	// unlock sem
}

MODULE_LICENSE("GPL");

#define NUM_PERIODS 1000 
SEM sem;
RTIME period;
RT_TASK t1,t2,tp;
unsigned long *BasePtr, *PBDR, *PBDDR;	// pointers for port B DR/DDR

//static void rt_process(int t) {
//	while(1)
//	{
//		// Turn on yellow light, turn off green
//		*PBDR |= 0x40;
//		*PBDR &= ~(0x80);
//		rt_sleep(NUM_PERIODS*period);
//		// Turn off yellow light, turn on green
//		*PBDR &= ~(0x40);
//		*PBDR |= 0x80;
//		rt_sleep(NUM_PERIODS*period);
//		// Check for button press
//		if(1 == check_button())
//		{	// Button pressed
//			// Yellow and green off
//			*PBDR &= ~(0x40);
//			*PBDR &= ~(0x80);
//			// Turn on red
//			*PBDR |= 0x20;
//			rt_sleep(NUM_PERIODS*period);
//			// Turn off
//			*PBDR &= ~(0x20);
//			// Clear button status
//			clear_button();
//		}
//
//	}
//}

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
	rtai_typed_sem_init(&sem, 0, BIN_SEM);

	// Initialize rt tasks
	// 3rd to last arg is priority 0 = highest
	// Equal priorities
	rt_task_init(&t1, rt_tl1, 0, 256, 0, 0, 0);
	rt_task_resume(&t1);

	rt_task_init(&t2, rt_tl2, 0, 256, 0, 0, 0);
	rt_task_resume(&t2);

	rt_task_init(&tp, rt_tp, 0, 256, 0, 0, 0);
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

