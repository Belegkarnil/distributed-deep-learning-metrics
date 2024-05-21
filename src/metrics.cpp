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
#include "metrics.hpp"

int main(int argn, char* args[]){
    Vector<Resource*> resources();
    resources.push_back(new GPU());
    resources.push_back(new Network("eth0"));
    resources.push_back(new Filesystem("sda1"));
    resources.push_back(new Filesystem("nvme0n1p2"));
    {
        Executor * executor = new Executor();
        resources.push_back(new Process(executor->getPID()));
        resources.push_back(executor);
    }


	struct process root = {executor.execute(args+1, argn-1)};

	struct cpu_info cpu;
	struct fs_info fs[2];
	memcpy(fs[0].device_name,"sda1",strlen("sda1")+1); // /data
	memcpy(fs[1].device_name,"nvme0n1p2",strlen("nvme0n1p2")+1);// /

    printf("%s","timestamp");
    for(res = resources.begin() ; res != resources.end(); ++res){
        putc(',', stdout);
        (*res)->outputHeader();
    }
    putc('\n', stdout);

	while(executor.isRunning()){
		init_process(&root);
		nprocess = 0;
		nthreads = 0;
		ram = 0;
		cpu_info cpu = global_cpu_info();
		count_process_threads(&root,&nprocess,&nthreads,ram_used,NULL);
		free_process(&root);

		global_fs_info(fs,2);

        for(res = resources.begin() ; res != resources.end(); ++res){
            (*res)->measure();
        }

        printf("%lu",std::time(null));
        for(res = resources.begin() ; res != resources.end(); ++res){
            putc(',', stdout);
            (*res)->output();
        }
        putc('\n', stdout);

		sleep(1);
	}

	while(! resources.empty()){
	    delete resources.back();
	    resources.pop_back();
	}

	return EXIT_SUCCESS;
}

void output_header(){
	printf("timestamp,");

	gpu->outputHeader();
	putc(',', stdout);
	network->outputHeader();
	putc(',', stdout);

	printf("cpu_user,cpu_nice,cpu_system,cpu_sidle,cpu_siowait,cpu_sirq,cpu_ssoftirq,nprocess,nthreads,datafs_nreads,datafs_nwrites,datafs_writetime,datafs_currentio,datafs_iotime,osfs_nreads,osfs_nwrites,osfs_writetime,osfs_currentio,osfs_iotime,ram,");

	executor->outputHeader();
	putc('\n', stdout);
}

void output(struct cpu_info * cpu ,struct fs_info * fs){
	current_time = std::time(null);

	// timestamp (1 field)
	printf("%lu,",std::time(null));

	// gpu
	gpu->output();
	putc(',', stdout);

	// network
	network->output();
	putc(',', stdout);

	// cpu
	printf(// 9 fields
			"%llu,%llu,%llu,%llu,%llu,%llu,%llu,%d,%d,",
			cpu->user, cpu->nice, cpu->system, cpu->sidle, cpu->siowait, cpu->sirq, cpu->ssoftirq,
			nprocess, nthreads
	);

	//filesystem
	printf(// 10 fields
			"%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,",
			fs[0].nreads, fs[0].nwrites, fs[0].write_time, fs[0].current_io, fs[0].io_time,
			fs[1].nreads, fs[1].nwrites, fs[1].write_time, fs[1].current_io, fs[1].io_time
	);

	//ram (1 field)
	printf("%llu,",ram);

	// monitored process
	// 1 field
	executor->output();
	putc('\n', stdout);
}


