/*
 *  Copyright 2024 Belegkarnil
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *  associated documentation files (the “Software”), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or substantial
 *  portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 *  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __PROCESS_HPP__
#define	__PROCESS_HPP__

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <cmath>
#include <cstring>

const size_t nchar_pid = (int) ceilf(log10f(sizeof(pid_t)));

int nprocess,nthreads;

struct thread{
	pid_t tid;
	size_t nchildren;
	struct process * children;
};

struct process{
	pid_t pid;
	size_t ntasks;
	struct thread * tasks;
};

struct stack_node{
	pid_t pid;
	pid_t tid;
	struct stack_node * previous;
};

struct process_stack{
	struct stack_node * top;
	size_t counter;
};

struct cpu_info{
	//cpu  16336386 16253 2288078 9094598202 364774 0 72480 0 0 0
	// 10 numeric fields
	unsigned long long int user;		// (1) Time spent in user mode.
	unsigned long long int nice;		// (2) Time spent in user mode with low priority (nice).
	unsigned long long int system;	// (3) Time spent in system mode.
	unsigned long long int sidle;		// (4) Time spent in the idle task.
	unsigned long long int siowait;	// (5) Time waiting for I/O to complete.
	unsigned long long int sirq;		// (6) Time servicing interrupts.
	unsigned long long int ssoftirq;	// (7) Time servicing softirqs.
/*
	unsigned long long int steal;		//(8) time spent in other operating systems (virtualized environment)
	unsigned long long int guest;		//(9) Time spent running a virtual CPU for guest operating systems
	unsigned long long int guest_nice; //(10)
*/
};

struct thread_info{
	unsigned long int minflt;								// %lu  9(10-1) The number of minor faults the process has made which have not required loading a memory page from disk.
	unsigned long int cminflt;								// %lu 10(11-1) The number of minor faults that the process's waited-for children have made.
	unsigned long int majflt;								// %lu 11(12-1) The number of major faults the process has made which have required loading a memory page from disk.
	unsigned long int cmajflt;								// %lu 12(13-1) The number of major faults that the process's waited-for children have made.
	unsigned long int utime;								// %lu (14) Amount of time that this process has been scheduled in user mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)). This includes guest time, guest_time (time spent running a virtual CPU, see below), so that applications that are not aware of the guest time field do not lose that time from their calculations.
	unsigned long int stime;								// %lu (15) Amount of time that this process has been scheduled in kernel mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
	long int cutime;											// %ld (16) Amount of time that this process's waited-for children have been scheduled in user mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)). (See also times(2).) This includes guest time, cguest_time (time spent running a virtual CPU, see below).
	long int cstime;											// %ld (17) Amount of time that this process's waited-for children have been scheduled in kernel mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
	unsigned long int vsize;								// %lu (23) Virtual memory size in bytes.
	long int rss;												// %ld (24) Resident Set Size: number of pages the process has in real memory. This is just the pages which count toward text, data, or stack space. This does not include pages which have not been demand-loaded in, or which are swapped out.
	unsigned long long int delayacct_blkio_ticks;	// %llu (42) Aggregated block I/O delays, measured in clock ticks (centiseconds).
	unsigned long int guest_time;							// %lu (43) Guest time of the process (time spent running a virtual CPU for a guest operating system), measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
	long int cguest_time;									// %ld (44) Guest time of the process's children, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
};

typedef void (*pid_function)(pid_t pid);

int task_filter(const struct dirent *);
void init_process(struct process *);
void free_process(struct process *);
size_t count_pid(char *);
struct stack_node stack_pop(struct process_stack * stack);
void stack_push(struct process_stack * stack, pid_t pid, pid_t tid);
void flatten_process(struct process * root, struct process_stack * stack);
void count_process_threads(struct process * root, int * nprocess, int * nthreads, pid_function process_fn, pid_function thread_fn);
void parse_stat(char * content, struct thread_info * info);
struct cpu_info global_cpu_info();
void global_fs_info(struct fs_info * fs,size_t len);

#endif
