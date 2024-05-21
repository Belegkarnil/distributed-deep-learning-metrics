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
#include "process.hpp"

void init_process(struct process * proc){
	char base_path[nchar_pid + 13]; // /proc/${pid}/task/${tid} 12+ digit(pid_t) + '\0' = 12+nchar_pid+1 =
	sprintf(base_path,"/proc/%d/task/",proc->pid);
	struct dirent * * namelist;
	proc->ntasks = scandir(base_path, &namelist,task_filter,alphasort);
	if(((int)proc->ntasks) <= 0){
		//perror("cannot read proc dir");
		proc->ntasks = 0;
		return;
	}
	proc->tasks = (struct thread*) malloc(sizeof(struct thread) * proc->ntasks);
	for(int i=0; i<proc->ntasks; i++){
		proc->tasks[i].tid			= atoi(namelist[i]->d_name);
		proc->tasks[i].nchildren	= 0;
		free(namelist[i]);
	}
	free(namelist);
	// a thread can be stopped and the file ../children can be inaccessible
	// /proc/[pid]/task/[tid]/children text file with spaces
	char content[256];
	char thread_path[2*nchar_pid + 22]; // len(base_path) + "${tid}/children" = len(base_path) + nchar_pid + 9 = 22 + 2*nchar_pid
	FILE * f;
	for(int i=0; i<proc->ntasks; i++){
		sprintf(thread_path,"/proc/%d/task/%d/children",proc->pid,proc->tasks[i].tid);
		f = fopen(thread_path,"r");
		if(f != NULL){
			if(fgets(content,256,f) != NULL){
				fclose(f);
				proc->tasks[i].nchildren	= count_pid(content);
				proc->tasks[i].children		= (struct process*)malloc(sizeof(struct process)*proc->tasks[i].nchildren);
				char * token;
				for(int j=0, pos=0; j<proc->tasks[i].nchildren; j++){
					token	 = strtok(content+pos," ");
					proc->tasks[i].children[j].pid=atoi(token);
					init_process(&(proc->tasks[i].children[j]));
					pos	+= strlen(token)+1;
				}
			}else{
				fclose(f);
			}
		}
	}
}
size_t count_pid(char * spaced_pid){
	size_t count = 0;
	while(*spaced_pid){
		if(*spaced_pid == ' ') ++count;
		++spaced_pid;
	}
	return count;
}

void free_process(struct process * proc){
	for(int i=0; i<proc->ntasks; i++){
		for(int j=0; j<proc->tasks[i].nchildren; j++){
			free_process(&(proc->tasks[i].children[j]));
		}
		if(proc->tasks[i].nchildren){
			free(proc->tasks[i].children);
		}
	}
	if(proc->ntasks > 0){
		free(proc->tasks);
	}
}

int task_filter(const struct dirent * t){
	return t->d_name[0] != '.';
}



struct stack_node stack_pop(struct process_stack * stack){
	struct stack_node res = *(stack->top);
	stack->counter--;
	free(stack->top);
	stack->top = res.previous;
	return res;
}

void stack_push(struct process_stack * stack, pid_t pid, pid_t tid){
	struct stack_node * node = (struct stack_node *) malloc(sizeof(struct stack_node));
	node->pid = pid;
	node->tid = tid;
	node->previous = stack->top;
	stack->top = node;
	stack->counter++;
}

void flatten_process(struct process * root, struct process_stack * stack){
	for(int i=0; i<root->ntasks; i++){
		for(int j=0; j<root->tasks[i].nchildren; j++){
			flatten_process(&(root->tasks[i].children[j]),stack);
		}
		stack_push(stack, root->pid, root->tasks[i].tid);
	}
}

void count_process_threads(struct process * root, int * nprocess, int * nthreads, pid_function process_fn, pid_function thread_fn){
	*nprocess += 1;
	if(process_fn != NULL) process_fn(root->pid);
	if(root->ntasks > 0) *nthreads += root->ntasks;
	for(int i=0; i<root->ntasks; i++){
		if(thread_fn != NULL) thread_fn(root->tasks[i].tid);
		for(int j=0; j<root->tasks[i].nchildren; j++){
			count_process_threads(&(root->tasks[i].children[j]), nprocess, nthreads, process_fn, thread_fn);
		}
	}
}


void parse_stat(char * content, struct thread_info * info){
	char * token;

	 // skip 9 tokens
	for(int i=0; i<9; i++){ token = strtok(content," "); content += strlen(token)+1; }

	token = strtok(content," "); content += strlen(token)+1;
	info->minflt	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->cminflt	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->majflt	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->cmajflt	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->utime	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->stime	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->cutime	= strtol(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->cstime	= strtol(token, NULL,10);

	// from 17 to 23 => 5 between (18, 19, 20, 21, 22) => skip 5 tokens
	for(int i=0; i<5; i++){ token = strtok(content," "); content += strlen(token)+1; }

	token = strtok(content," "); content += strlen(token)+1;
	info->vsize	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->rss	= strtol(token, NULL,10);

	// from 24 to 42 => 17 between => skip 17 tokens
	for(int i=0; i<17; i++){ token = strtok(content," "); content += strlen(token)+1; }

	token = strtok(content," "); content += strlen(token)+1;
	info->delayacct_blkio_ticks	= strtoull(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->guest_time	= strtoul(token, NULL,10);

	token = strtok(content," "); content += strlen(token)+1;
	info->cguest_time	= strtol(token, NULL,10);
}

struct cpu_info global_cpu_info(){
	FILE * f = fopen("/proc/stat","r");
	char buffer[256];
	char * content;
	struct cpu_info res;
	memset(&res,0,sizeof(struct cpu_info));
	if(f != NULL && fgets(buffer,256,f) != NULL){
		fclose(f);
		char * token;
		content = buffer + 4;//skip "cpu "
		while(*content == ' ') content++; //skip heading spaces (must be 1 space)

		token = strtok(content," "); content += strlen(token)+1;
		res.user = strtoull(token, NULL,10);

		token = strtok(content," "); content += strlen(token)+1;
		res.nice = strtoull(token, NULL,10);

		token = strtok(content," "); content += strlen(token)+1;
		res.system = strtoull(token, NULL,10);

		token = strtok(content," "); content += strlen(token)+1;
		res.sidle = strtoull(token, NULL,10);

		token = strtok(content," "); content += strlen(token)+1;
		res.siowait = strtoull(token, NULL,10);

		token = strtok(content," "); content += strlen(token)+1;
		res.sirq = strtoull(token, NULL,10);

		token = strtok(content," "); content += strlen(token)+1;
		res.ssoftirq = strtoull(token, NULL,10);
	}else if(f != NULL) fclose(f);
	return res;
}

long long int ram;
void ram_used(pid_t pid){
	char stat_path[nchar_pid + 12];
	sprintf(stat_path,"/proc/%d/stat",pid);
	struct thread_info info;
	char content[1024];
	FILE * f = fopen(stat_path,"r");
	if(f != NULL){
		if(fgets(content,1024,f) != NULL){
			parse_stat(content,&info);
			ram += info.rss;
		}
		fclose(f);
	}
}
