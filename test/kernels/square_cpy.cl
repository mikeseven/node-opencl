__kernel void square_cpy(
    __global float* input,
   __global float* output,
   unsigned int count)
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i] * input[i];
}
