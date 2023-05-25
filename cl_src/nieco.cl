__kernel void jedna(__global int *input, __global int *output, int count) {
  int i = get_global_id(0);
  if (i < count) 
      output[i] = input[i] * input[i];
}

__kernel void dva(__global int *input, __global int *output, __local int *l_result) {
  int i = get_global_id(0) * 2;
  int jedna = input[i];
  int dva = input[i+1];
  int sum = jedna + dva;

  int local_i = get_local_id(0);
  l_result[local_i] = sum;

  barrier(CLK_LOCAL_MEM_FENCE);

  if (local_i == 0 ) {
    int spolu = 0;
    for(int i = 0; i < get_local_size(0); i++) {
        spolu += l_result[i];
    }
    output[get_group_id(0)] = spolu;
  }

}


__kernel void tri(__global const int* A,__global const int* B,__global int* C) {
    int row = get_global_id(0);
    int col = get_global_id(1);

    int sum = 0;
    for (int k = 0; k < 16; ++k) {
        sum += A[row * 16 + k] * B[k * 16 + col];
    }

    C[row * 16 + col] = sum;
}
