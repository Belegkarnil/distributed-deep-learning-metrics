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
#include "gpu.hpp"

GPU::GPU(){
    this->temperature           = NULL;
    this->power                 = NULL;
    this->powerLimit            = NULL;
    this->gpuClock              = NULL;
    this->gpuClockLimit         = NULL;
    this->vramClock             = NULL;
    this->memory_used           = NULL;
    this->memory_total          = NULL;
    this->utilization_gpu       = NULL;
    this->utilization_memory    = NULL;

    nvmlReturn_t nvmlResult = nvmlInit();
	if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

	nvmlResult = nvmlDeviceGetCount(&(this->gpuDeviceCount));
	if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

	this->gpuDevices = (nvmlDevice_t *) calloc(this->gpuDeviceCount,sizeof(nvmlDevice_t));
	if(gpuDevices == NULL) throw std::system_error(ENOMEM,strerror(errno));

	// Get available devices
	for(int i=0; i<this->gpuDeviceCount; i++){
		nvmlResult = nvmlDeviceGetHandleByIndex(i, &(gpuDevices[i]));
		if(nvmlResult != NVML_SUCCESS)  throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );
		this->initDevice(i);
	}
	/*
	// Get the device name
	char name[NVML_DEVICE_NAME_BUFFER_SIZE];
	nvmlResult = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
	if (nvmlResult != NVML_SUCCESS) std::cerr << "Failed to get device name: " << nvmlErrorString(nvmlResult) << std::endl;
	*/


    this->temperature           = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
    this->power                 = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
    this->powerLimit            = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
    this->gpuClock              = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
    this->gpuClockLimit         = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
    this->vramClock             = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
    this->memory_used           = (unsigned long long *)malloc(sizeof(unsigned long long)*this->gpuDeviceCount);
    this->memory_total          = (unsigned long long *)malloc(sizeof(unsigned long long)*this->gpuDeviceCount);
    this->utilization_gpu       = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
    this->utilization_memory    = (unsigned int *)malloc(sizeof(unsigned int)*this->gpuDeviceCount);
}

GPU::~GPU(){
    free(this->temperature);
    free(this->power);
    free(this->powerLimit);
    free(this->gpuClock);
    free(this->gpuClockLimit);
    free(this->vramClock);
    free(this->memory_used);
    free(this->memory_total);
    free(this->utilization_gpu);
    free(this->utilization_memory);

	// Shutdown NVML library
    free(gpuDevices);
    nvmlShutdown();
}

void GPU::initDevice(const int deviceID){

    nvmlReturn_t nvmlResult;

	// Get the power limit of the device
	nvmlResult = nvmlDeviceGetPowerManagementLimit(this->device[deviceID], &(this->powerLimit[deviceID]));
	if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

	// Get the maximum clock speeds of GPU
	nvmlResult = nvmlDeviceGetMaxClockInfo(this->device[deviceID], NVML_CLOCK_GRAPHICS, &(this->gpuClockLimit[deviceID]));
	if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

	/*
	// Get the power management mode of the device
	nvmlPstates_t pState;
	nvmlResult = nvmlDeviceGetPowerState(*device, &pState);
	if(nvmlResult != NVML_SUCCESS) std::cerr << "Failed to get power state: " << nvmlErrorString(nvmlResult) << std::endl;
	std::cout << "GPU power management mode: " << (pState == NVML_PSTATE_0 ? "Max performance" : "Adaptive") << std::endl;
	*/
	nvmlResult = nvmlDeviceGetMemoryInfo(this->device[deviceID], &(this->nvmlMemory));
	if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );
	this->memory_total[deviceID] = this->nvmlMemory.total;
}


void GPU::measure(){
    nvmlReturn_t nvmlResult;

    for(int deviceID=0; deviceID < this->gpuDeviceCount; deviceID++){
        // Get the temperature of the device
        nvmlResult = nvmlDeviceGetTemperature(this->device[deviceID], NVML_TEMPERATURE_GPU, &(this->temperature[deviceID]));
        if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

        // Get the power usage of the device
        nvmlResult = nvmlDeviceGetPowerUsage(this->device[deviceID], &(this->power[deviceID]));
        if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

        // Get the memory usage of the device
        nvmlResult = nvmlDeviceGetMemoryInfo(this->device[deviceID], &(this->nvmlMemory));
        if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );
        this->memory_used[deviceID] = nvmlMemory.total;

        // Get the current clock speeds of GPU
        nvmlResult = nvmlDeviceGetClockInfo(this->device[deviceID], NVML_CLOCK_GRAPHICS, &(this->gpuClock[deviceID]));
        if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

        // Get the memory clock rate of the device
        nvmlResult = nvmlDeviceGetClockInfo(this->device[deviceID], NVML_CLOCK_MEM, &(this->vramClock[deviceID]));
        if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );

        /*
        // Get the process and application running on the device
        nvmlProcessInfo_t infos[1024];
        unsigned int count;
        nvmlResult = nvmlDeviceGetComputeRunningProcesses(*device, &count, infos);
        if(nvmlResult != NVML_SUCCESS) std::cerr << "Failed to get running processes: " << nvmlErrorString(nvmlResult) << std::endl;
        std::cout << "GPU running processes:" << std::endl;
        for (unsigned int i = 0; i < count; ++i) {
        std::cout << "PID: " << infos[i].pid << ", used GPU memory: " << infos[i].usedGpuMemory / 1024.0 / 1024.0 << " MB" << std::endl;
        std::cout << "\tgpuInstanceId: " << infos[i].gpuInstanceId << "computeInstanceId:" << infos[i].computeInstanceId << std::endl;
        }
        */

        // Get the utilization of the device
        nvmlResult = nvmlDeviceGetUtilizationRates(this->device[deviceID], &(this->nvmlUtilization));
        if(nvmlResult != NVML_SUCCESS) throw std::system_error(EALREADY, nvmlErrorString(nvmlResult) );
        this->utilization_gpu[deviceID]		= this->nvmlUtilization.gpu;
        this->utilization_memory[deviceID]	= this->nvmlUtilization.memory;
    }
}

void GPU::outputHeader(FILE *restrict stream){
    printf("gpu%1$d_temp,gpu%1$d_power,gpu%1$d_powerlimit,gpu%1$d_clock,gpu%1$d_maxclock,gpu%1$d_clockrate,gpu%1$d_mem,gpu%1$d_maxmem,gpu%1$d_util,gpu%1$d_memutil",1);
	for(int i=2; i <= this->gpuDeviceCount; i++)
		printf(",gpu%1$d_temp,gpu%1$d_power,gpu%1$d_powerlimit,gpu%1$d_clock,gpu%1$d_maxclock,gpu%1$d_clockrate,gpu%1$d_mem,gpu%1$d_maxmem,gpu%1$d_util,gpu%1$d_memutil",i);
}

void GPU::output(FILE *restrict stream){
    printf("%u,%lf,%lf,%u,%u,%u,%lf,%lf,%u,%u",// 10 fields per GPU
        this->temperature[0],
        this->power[0]         / 1000.0,
        this->powerLimit[0]    / 1000.0,
        this->gpuClock[0]      / 1000,
        this->gpuClockLimit[0] / 1000,
        this->vramClock[0]     / 1000,
        this->memory_used[0]   / 1024.0 / 1024.0,
        this->memory_total[0]  / 1024.0 / 1024.0,
        this->utilization_gpu[0],
        this->utilization_memory[0]
    );
    for(int i=1; i < this->gpuDeviceCount; i++){
    	printf(",%u,%lf,%lf,%u,%u,%u,%lf,%lf,%u,%u",// 10 fields per GPU
    		this->temperature[i],
    		this->power[i]         / 1000.0,
    		this->powerLimit[i]    / 1000.0,
    		this->gpuClock[i]      / 1000,
    		this->gpuClockLimit[i] / 1000,
    		this->vramClock[i]     / 1000,
    		this->memory_used[i]   / 1024.0 / 1024.0,
    		this->memory_total[i]  / 1024.0 / 1024.0,
    		this->utilization_gpu[i],
    		this->utilization_memory[i]
    	);
    }
}


