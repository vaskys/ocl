#include <OpenCL/OpenCL.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/cl.h>
#include <string.h>

#include "ccl.h"

cl_platform_id platform_id;
cl_device_id device_id;
cl_context context;
cl_command_queue commands;
cl_program program;
cl_kernel kernel;


size_t global;
size_t local;

char * ccl_load_from_from_file(const char *path) {
    FILE *file = fopen(path,"r");
    if(file) {
        long length;

        fseek (file, 0, SEEK_END);
        length = ftell (file);
        fseek (file, 0, SEEK_SET);
        char *file_buffer = malloc (length);

        fread (file_buffer, 1, length, file);
        fclose(file);
       //printf("%s \n",file_buffer);
        return file_buffer;

    } else {
        printf("file error %s \n",path);
    }
    exit(1);
}


int ccl_platform() {
    int err = clGetPlatformIDs(1, &platform_id, NULL);
    if (err != CL_SUCCESS) {
        printf("Plaftorm error \n");
        exit(1);
    }

    size_t len;
    char buffer[2048];
    clGetPlatformInfo(platform_id,CL_PLATFORM_VERSION, sizeof(buffer), buffer, &len);
    printf("%s \n",buffer);

    return 0;

}
int ccl_device(int type)  {
    int error = clGetDeviceIDs(platform_id, type ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    if (error != CL_SUCCESS) {
        printf("Device error ");
        type? printf("GPU \n") : printf("CPU \n");
        exit(1);
    }
    return 0;
}


int ccl_context() {
    int error;
    context = clCreateContext(0,1,&device_id,NULL,NULL,&error);
    if (error != CL_SUCCESS) {
        printf("Context error \n");
        exit(1);
    }
    return 0;
}

int ccl_command_q() {
    int error;
    commands = clCreateCommandQueue(context,device_id,0,&error);
    if (error != CL_SUCCESS) {
        printf("Command q error \n");
        exit(1);
    }
    return 0;
}

int ccl_create_program(const char *path) {
    int error;
    char *file_buffer = ccl_load_from_from_file(path);

    program = clCreateProgramWithSource(context,1,(const char **) & file_buffer,NULL,&error);
    if (error != CL_SUCCESS) {
        printf("Program error \n");
        exit(1);
    }

    free(file_buffer);

    error = clBuildProgram(program,0,NULL,NULL,NULL,NULL);
    if (error != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        printf("Program Build Error \n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }

    return 0;
}

int ccl_create_kernel(const char *name) {

    int error;
    kernel = clCreateKernel(program,name,&error);
    if (error != CL_SUCCESS) {
        printf("Kernel error \n");
        exit(1);
    }
    return 0;
}

cl_mem ccl_create_buffer(size_t size) {
    int error;
    cl_mem buffer;
    buffer = clCreateBuffer(context,CL_MEM_READ_WRITE,size, NULL, &error);

    if (error != CL_SUCCESS) {
        printf("create buffer error \n");
        exit(1);
    }
    return buffer;
}

int ccl_write_data_to_buffer(cl_mem buffer,size_t size, void * data) {
    int error;
    error = clEnqueueWriteBuffer(commands,buffer,CL_TRUE,0,size,data,0,NULL,NULL);
    if (error != CL_SUCCESS) {
        printf("write to buffer error \n");
        exit(1);
    }

    return 0;
}

int ccl_set_kernel_argument(size_t size, int index, void *data) {
    int error = 0;
    error = clSetKernelArg(kernel, index, size, data);
    if (error != CL_SUCCESS) {
        printf("kernel argument error \n");
        exit(1);
    }

    return 0;
}


size_t ccl_get_kernel_wg_info() {
    int error = clGetKernelWorkGroupInfo(kernel,device_id,CL_KERNEL_WORK_GROUP_SIZE,sizeof(local),&local,NULL);
    if(error) {
        printf("Error kernel group info \n");
        exit(1);
    }
    return local;
}


int ccl_enqueue_nd_rangekernel(size_t local_i,size_t global_i) {
    global = global_i;
    local = local_i;

    int error = clEnqueueNDRangeKernel(commands,kernel,1,NULL,&global,&local,0,NULL,NULL);
    if(error) {
        printf("Error execute kernel \n");
        exit(1);
    }
    return 0;
}



int ccl_enqueue_nd_rangekernel_2d(size_t local_i[2],size_t global_i[2]) {
    int error = clEnqueueNDRangeKernel(commands,kernel,2,NULL,global_i,local_i,0,NULL,NULL);
    if(error) {
        printf("Error execute kernel \n");
        exit(1);
    }
    return 0;
}

void ccl_finish() {
    clFinish(commands);
}



int ccl_read_buffer(cl_mem buffer,size_t size, void *data) {
    int error;
    error = clEnqueueReadBuffer(commands,buffer,CL_TRUE,0,size,data,0,NULL,NULL);

    if (error != CL_SUCCESS) {
        printf("read buffer error \n");
        exit(1);
    }

    return 0;
}


int ccl_init(int type,const char* file_path,const char* kernel_name) {
    int error;
    error = ccl_platform();
    error = ccl_device(type);
    error = ccl_context();
    error = ccl_command_q();
    error = ccl_create_program(file_path);
    error = ccl_create_kernel(kernel_name);

    return error;
}

void ccl_clear_buffer(cl_mem buffer) {
    clReleaseMemObject(buffer);
}

void ccl_clear() {
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);


}



