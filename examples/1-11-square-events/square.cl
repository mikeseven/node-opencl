__kernel void square(
    __global uint* input,
   __global uint* output,
   unsigned int count)
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i] * input[i];
}
