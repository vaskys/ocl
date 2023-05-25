#include "ex.h"
#include "ccl.h"
#include <OpenCL/cl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


void ex_jedna() {

    const int pocet = 1024;
    int data[pocet];
    int results[pocet];

    int count = pocet;
    int i = 0;
    for(i = 0; i < count; i++) {
        data[i] = rand() % 1000;
    }

    ccl_init(1,"cl_src/nieco.cl","jedna");
    cl_mem input = ccl_create_buffer(sizeof(int) * count);
    cl_mem output = ccl_create_buffer(sizeof(int) * count);

    ccl_write_data_to_buffer(input, sizeof(int) * count, data);

    ccl_set_kernel_argument(sizeof(cl_mem), 0, &input);
    ccl_set_kernel_argument(sizeof(cl_mem), 1, &output);
    ccl_set_kernel_argument(sizeof(int), 2, &count);

    size_t local = ccl_get_kernel_wg_info();

    ccl_enqueue_nd_rangekernel(local,count);
    ccl_finish();

    ccl_read_buffer(output, sizeof(int) * count, results);

    int c = 0;
    for( i =0; i< count; i++) {
        if(results[i] == data[i] * data[i])
            c++;
    }

    printf("%d - %d \n",c,count);

    ccl_clear_buffer(input);
    ccl_clear_buffer(output);
    ccl_clear();
}


void ex_dva() {
    ccl_init(1,"cl_src/nieco.cl","dva");

    const int pocet = 1000;

    const int n = 4;
    const int group_size = pocet / n;
    

    int data[pocet];
    int results[group_size];

    int i = 0;
    for(i = 0; i < pocet; i++) {
        data[i] = rand() % 1000;
    }

    cl_mem input = ccl_create_buffer(sizeof(int) * pocet);
    cl_mem output = ccl_create_buffer(sizeof(int) * group_size);

    ccl_write_data_to_buffer(input,sizeof(int) * pocet, data);

    ccl_set_kernel_argument(sizeof(cl_mem), 0, &input);
    ccl_set_kernel_argument(sizeof(cl_mem), 1, &output);
    ccl_set_kernel_argument(sizeof(int) *n, 2, NULL);

    ccl_enqueue_nd_rangekernel(n,pocet/2);
    ccl_finish();

    ccl_read_buffer(output, sizeof(int) * group_size, results);

    int sum_jedna = 0;
    for(i = 0; i < group_size; i++) {
        sum_jedna += results[i];
    }

    int sum_dva = 0;
    for(i = 0; i < pocet ; i++) {
        sum_dva += data[i];
    }

    printf("%d - %d  \n",sum_jedna, sum_dva);

    ccl_clear_buffer(input);
    ccl_clear_buffer(output);
    ccl_clear();
}


void ex_tri() {
    ccl_init(1,"cl_src/nieco.cl","tri");

    int W = 16;
    int H = 16;
    int size = W*H;

    int *m_a = (int*)malloc(sizeof(int) * size);
    int *m_b = (int*)malloc(sizeof(int) * size);
    int *m_c = (int*)malloc(sizeof(int) * size);

    int i;
    for(i = 0; i < size; i++) {
        m_a[i] = rand() % 100;
        m_b[i] = rand() % 100;
    }

    cl_mem jedna = ccl_create_buffer(sizeof(int) * size);
    cl_mem dva = ccl_create_buffer(sizeof(int) * size);
    cl_mem tri = ccl_create_buffer(sizeof(int) * size);


    ccl_write_data_to_buffer(jedna,sizeof(int)*size,m_a);
    ccl_write_data_to_buffer(dva,sizeof(int)*size,m_b);


    ccl_set_kernel_argument(sizeof(cl_mem), 0, &jedna);
    ccl_set_kernel_argument(sizeof(cl_mem), 1, &dva);
    ccl_set_kernel_argument(sizeof(cl_mem), 2, &tri);

    size_t localWorkSize[2], globalWorkSize[2];
    localWorkSize[0] = 4;
    localWorkSize[1] = 4;
    globalWorkSize[0] = 16;
    globalWorkSize[1] = 16;

    ccl_enqueue_nd_rangekernel_2d(localWorkSize,globalWorkSize);
    ccl_finish();

    ccl_read_buffer(tri, sizeof(int) * size,m_c);
       
    int counter = 0;
    for(i=0; i< size;i++) {
        printf("%d,",m_c[i]);
        counter++;

        if(counter >=16) {
            printf("\n");
            counter = 0;
        }
        
    }

    printf("\n");


    int j;
    int k;
    for(i=0; i < 16; i++) {
        for(j=0; j < 16; j++) {
            int sum = 0;
            for(k = 0; k< 16; k++) {
                sum +=m_a[i * 16 + k] * m_b[k * 16 + j];
            }
            int cislo = sum;
            printf("%d,",cislo);
        }
        printf("\n");
    }


    ccl_clear_buffer(jedna);
    ccl_clear_buffer(dva);
    ccl_clear_buffer(tri);
    ccl_clear();


    free(m_a);
    free(m_b);
    free(m_c);
}
