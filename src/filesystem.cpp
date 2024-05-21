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
#include "filesystem.hpp"

Filesystem::Filesystem(const char * name){
    int len = strlen(name);
    if(len > 31) len = 31;
    strncpy(deviceName,name,len);
    deviceName[31] = '\0';
}

Filesystem::~Filesystem(){
}

void Filesystem::measure(){
	FILE * f = fopen("/proc/diskstats","r");
	char buffer[256];
	char * content;
	size_t count = 0;
	int i;
	if(f != NULL){
		char * token;
		while(count < len && fgets(buffer,256,f) != NULL){
			content = buffer;
			while(*content == ' ') content++; //skip spaces
			while(*content != ' ') content++; //skip first set of digits
			while(*content == ' ') content++; //skip spaces
			while(*content != ' ') content++; //skip second set of digits
			while(*content == ' ') content++; //skip spaces

			token = strtok(content," "); content += strlen(token)+1;

			if(!strcmp(token,deviceName)){
				token = strtok(content," "); content += strlen(token)+1;
				this->nreads = strtoull(token, NULL,10);

				token = strtok(content," "); content += strlen(token)+1;
				this->nwrites = strtoull(token, NULL,10);

				token = strtok(content," "); content += strlen(token)+1;
				this->write_time = strtoull(token, NULL,10);

				token = strtok(content," "); content += strlen(token)+1;
				this->current_io = strtoull(token, NULL,10);

				token = strtok(content," "); content += strlen(token)+1;
				this->io_time = strtoull(token, NULL,10);
			}
		}
	}
	if(f != NULL) fclose(f);
}

void Filesystem::outputHeader(FILE *restrict stream){
    fprintf(stream,"%1$s_nreads,%1$s_nwrites,%1$s_writetime,%1$s_currentio,%1$s_iotime",deviceName);
}

void Filesystem::output(FILE *restrict stream){
    fprintf(stream,// 5 fields
			"%llu,%llu,%llu,%llu,%llu"
			nreads, nwrites, write_time, current_io, io_time
	);
}
