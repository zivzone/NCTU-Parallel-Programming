typedef struct tag_RGB
{
    uchar R; 
    uchar G;
    uchar B;
    uchar align;
} RGB;

// kernel function
__kernel void histogram(__global RGB *data, int s, __global int *o_r, __global int *o_g, __global int *o_b) {
    int t_id = get_global_id(0);
    if (t_id < s) {
        RGB p = data[t_id];
        atomic_inc(&o_r[p.R]);
        atomic_inc(&o_g[p.G]);
        atomic_inc(&o_b[p.B]);
    }
}