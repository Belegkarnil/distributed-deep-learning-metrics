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
#ifndef __EXECUTOR_HPP__
#define __EXECUTOR_HPP__

#include <cstdlib>
#include <cstdio>
#include <cinttypes>
#include <csignal>
#include <system_error>

#include "resource.hpp"

class Executor: public Resource {
    private:
        bool running;
        pid_t pid;
        int fd;
        void signalHandler(int signum);
        const int bufferSize;
        int len;
        char * data;
    public:
        Executor(int maxbuffSize = 4096);
        ~Executor();
        void execute(char* cmd[],size_t len);
        pid_t getPID();
        bool isRunning();
        void measure();
        void outputHeader(FILE * stream=stdout);
        void output(FILE * stream=stdout);
};

#endif