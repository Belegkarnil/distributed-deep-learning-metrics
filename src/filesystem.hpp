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
#ifndef __FILESYSTEM_HPP__
#define __FILESYSTEM_HPP__

#include <cstdlib>
#include <cstdio>
#include <cinttypes>

#include "resource.hpp"

class Filesystem : public Resource{
private:
	char deviceName[32];
	unsigned long long int nreads;		// reads completed, field 1
	unsigned long long int nwrites;		// writess completed, field 5
	unsigned long long int write_time;	// milliseconds spent writing, field 8
	unsigned long long int current_io;	// number of I/Os currently in progress, field 9
	unsigned long long int io_time;		// milliseconds spent doing I/Os, field 10
public:
    Filesystem();
    ~Filesystem();
    void measure();
    void outputHeader(FILE *restrict stream=stdout);
    void output(FILE *restrict stream=stdout);
};

printf("datafs_nreads,datafs_nwrites,datafs_writetime,datafs_currentio,datafs_iotime");

#endif