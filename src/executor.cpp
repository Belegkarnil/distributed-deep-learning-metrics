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
#include "executor.hpp"

Executor::Executor(int maxbuffSize): bufferSize(maxbuffSize){
    running = false;
    pid     =  0;
    fd      = -1;

    data = (char*) malloc(maxbuffSize*sizeof(char));
    data[0] = '\0';
    len  = 0;

	if (signal(SIGTERM, this->signalHandler) == SIG_ERR) throw std::system_error(EACCES, "can't catch SIGTERM");
	//if (signal(SIGKILL, signal_handler) == SIG_ERR) std::cerr << "can't catch SIGINT" << std::endl;
	if (signal(SIGINT, this->signalHandler) == SIG_ERR) throw std::system_error(EACCES, "can't catch SIGINT");
}

Executor::~Executor(){
    free(data);
    if(fd >= 0) close(fd);
}

void Executor::signalHandler(int signum){
   cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here
   // terminate program

   exit(signum);
}

bool Executor::isRunning(){
    if(!running) return running;
    const bool zombie = true;
    int res = kill(python_process,0);
    if(res == -1){
        //std::cerr << "kill child to know if alive is -1 => is not alive: " << strerror(errno) << std::endl;
        running = false; // process does not exists (or no acces but ...)
        return running;
    }
    //assuming that res == 0
    //std::cout << "Still running: " << (res == 0?"true":"false") << std::endl;
    //std::cout << "Pid: " << python_process << std::endl;
    if(!zombie){
        //std::cerr << "not checking zombie and kill gives 0, still alive" << std::endl;
        running = true;
        return running;
    }
    // if need to check zombie...
    int status;
    res = waitpid(python_process,&status,WNOHANG);
    if(res < 0){
        //std::cerr << "waitpid to know if alive is -1 => is not alive: " << strerror(errno) << std::endl;
        running = false; // process does not exists (or no acces but ...)
        return running;
    }
    if(res > 0){
        //std::cerr << "child process status is exited: " << (WIFEXITED(status)?"true":"false") << std::endl;
        running = ! WIFEXITED(status);
        return running;
    }
    // WNOHANG, res=0 ==> no change
    running = true;
    return running;
}

pid_t Executor::getPID(){
    return pid;
}

void Executor::execute(char* cmd[],size_t len){
	int filedes[2];
	if(pipe(filedes) < 0){
		perror("Cannot create pipe");
		exit(-1);
	}
	pid = fork();
	if(pid < 0){
		perror("Cannot run child process");
		exit(-2);
	} else if(pid > 0){
		close(filedes[1]);// close the write pipe
		fd = filedes[0];
		running = true;

		if(fcntl( python_fd, F_SETFL, fcntl(python_fd, F_GETFL) | O_NONBLOCK) == -1)
		    throw std::system_error(EACCES, strerror(errno));

		return;
	}
	// in child
	while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {} // duplicate out pipe to stdout
	close(filedes[1]);
	close(filedes[0]);
	execvp(cmd[0], cmd);
	perror("Cannot execute child process");}
	exit(-1);
}

void Executor::measure(){
    // assume that the buffer is large enough to read one/several lines each second

    len = 0;
    data[0] = '\0';
    int remain = bufferSize;
    ssize_t count;

    do{
        count = read(fd,data+len,remain);

        // hoping that (if count == -1) errno == EINTR
        if(count <= 0){
            data[len] = '\0';
            return;
        }

        // bytes read, need to check if a full line was read (i.e. last character is ASCII 10, \n)
        if(data[count-1] == '\n'){
            data[len]  = '\0';
            len       += count;
            return;
        }

        // need to keep reading (assuming max one line per iteration, if the learning process is faster => issue)
        len    += count;
        remain -= count;
    }while(true);
}

void Executor::outputHeader(FILE *restrict stream){
    fprintf(stream,"%s","childdata");
}

void Executor::output(FILE *restrict stream){
    fprintf(stream,"%s",data);
}