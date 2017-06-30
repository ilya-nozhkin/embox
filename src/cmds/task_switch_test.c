#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kernel/task.h>
#include <kernel/time/timer.h>
#include <time.h>

static void *task1(void *data) {
	struct timespec ts;
	struct timespec prev;
	
	for (int i = 0; i < 1000; i++) {
		clock_gettime(0, &ts);
		printk("1 %llu\n", (uint64_t) ts.tv_nsec - prev.tv_nsec);
		prev = ts;
	}
}

int main(int argc, char **argv) {
	int pid1 = new_task("task1", task1, NULL);
	task_set_priority(task_table_get(pid1), 21);
	task_waitpid(pid1);

	return 0;
}
