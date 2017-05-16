#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kernel/task.h>
#include <kernel/time/timer.h>

static void *task1(void *data) {
	for (int i = 0; i < 1000; i++) {
		printk("task1\n");
		for (volatile int j = 0; j < 100000; j++);
	}
}

static void *task2(void *data) {
	for (int i = 0; i < 1000; i++) {
		printk("task2\n");
		for (volatile int j = 0; j < 100000; j++);
	}
}

static struct sys_timer *my_timer;

static void tick(sys_timer_t *timer, void *param) {
	printk("adsfasdf\n");
}

int main(int argc, char **argv) {
	int pid1 = new_task("task1", task1, NULL);
	int pid2 = new_task("task2", task2, NULL);
	task_set_priority(task_table_get(pid1), 100);
	task_set_priority(task_table_get(pid2), 100);
	
	printk("%d %d", pid1, pid2);
	
	printf("%d\n", timer_set(&my_timer, TIMER_PERIODIC, 100,
			tick, NULL));
	
	task_waitpid(pid1);
	task_waitpid(pid2);

	return 0;
}
