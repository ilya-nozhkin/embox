#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kernel/task.h>
#include <kernel/time/timer.h>
#include <time.h>

struct timespec ts;

static void *task1(void *data) {
	for (int i = 0; i < 1000; i++) {
		clock_gettime(0, &ts);
		printk("1 %llu\n", (uint64_t) ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
	}
}

static void *task2(void *data) {
	for (int i = 0; i < 1000; i++) {
		clock_gettime(0, &ts);
		printk("2\n", (uint64_t) ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
	}
}

int main(int argc, char **argv) {
	int pid1 = new_task("task1", task1, NULL);
	int pid2 = new_task("task2", task2, NULL);
	task_set_priority(task_table_get(pid1), 21);
	task_set_priority(task_table_get(pid2), 200);
	
	printk("%d %d", pid1, pid2);
	
	task_waitpid(pid1);
	task_waitpid(pid2);

	return 0;
}
