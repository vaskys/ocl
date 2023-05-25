#ifndef CL_H
#define CL_H


#include <OpenCL/OpenCL.h>
#include <stddef.h>
typedef enum {
  CHAR,
  INT,
  FLOAT,
  DOUBLE,
  CL_MEM
} CCL_TYPES;

int ccl_platform();
int ccl_device(int type);
int ccl_context();
int ccl_command_q();
int ccl_create_program(const char *path);
int ccl_create_kernel(const char* name);

cl_mem ccl_create_buffer(size_t size);
int ccl_write_data_to_buffer(cl_mem buffer,size_t size, void * data);
int ccl_read_buffer(cl_mem buffer,size_t size, void *data);


int ccl_set_kernel_argument(size_t size, int index, void *data);


void ccl_clear_buffer(cl_mem buffer);
void ccl_clear();

int ccl_init(int type,const char* file_path,const char* kernel_name);


size_t ccl_get_kernel_wg_info();

int ccl_enqueue_nd_rangekernel(size_t local_i,size_t global_i);
int ccl_enqueue_nd_rangekernel_2d(size_t local_i[2],size_t global_i[2]);

void ccl_finish();




#endif
