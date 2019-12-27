#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <ios>
#include <CL/cl.h>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
typedef struct tag_RGB
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t align;
} RGB;

typedef struct tag_Image
{
    bool type;
    uint32_t size;
    uint32_t height;
    uint32_t weight;
    RGB *data;
} Image;

Image *readbmp(const char *filename)
{
    std::ifstream bmp(filename, std::ios::binary);
    char header[54];
    bmp.read(header, 54);
    uint32_t size = *(int *)&header[2];
    uint32_t offset = *(int *)&header[10];
    uint32_t w = *(int *)&header[18];
    uint32_t h = *(int *)&header[22];
    uint16_t depth = *(uint16_t *)&header[28];
    if (depth != 24 && depth != 32)
    {
        printf("we don't suppot depth with %d\n", depth);
        exit(0);
    }
    bmp.seekg(offset, bmp.beg);

    Image *ret = new Image();
    ret->type = 1;
    ret->height = h;
    ret->weight = w;
    ret->size = w * h;
    //ret->data = new RGB[w * h]{};
    ret->data = new RGB[w * h];
    for (int i = 0; i < ret->size; i++)
    {
        bmp.read((char *)&ret->data[i], depth / 8);
    }
    return ret;
}

int writebmp(const char *filename, Image *img)
{

    uint8_t header[54] = {
        0x42,        // identity : B
        0x4d,        // identity : M
        0, 0, 0, 0,  // file size
        0, 0,        // reserved1
        0, 0,        // reserved2
        54, 0, 0, 0, // RGB data offset
        40, 0, 0, 0, // struct BITMAPINFOHEADER size
        0, 0, 0, 0,  // bmp width
        0, 0, 0, 0,  // bmp height
        1, 0,        // planes
        32, 0,       // bit per pixel
        0, 0, 0, 0,  // compression
        0, 0, 0, 0,  // data size
        0, 0, 0, 0,  // h resolution
        0, 0, 0, 0,  // v resolution
        0, 0, 0, 0,  // used colors
        0, 0, 0, 0   // important colors
    };

    // file size
    uint32_t file_size = img->size * 4 + 54;
    header[2] = (unsigned char)(file_size & 0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;

    // width
    uint32_t width = img->weight;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) & 0x000000ff;
    header[20] = (width >> 16) & 0x000000ff;
    header[21] = (width >> 24) & 0x000000ff;

    // height
    uint32_t height = img->height;
    header[22] = height & 0x000000ff;
    header[23] = (height >> 8) & 0x000000ff;
    header[24] = (height >> 16) & 0x000000ff;
    header[25] = (height >> 24) & 0x000000ff;

    std::ofstream fout;
    fout.open(filename, std::ios::binary);
    fout.write((char *)header, 54);
    fout.write((char *)img->data, img->size * 4);
    fout.close();
}

/*void histogram(Image *img,uint32_t R[256],uint32_t G[256],uint32_t B[256]){
    std::fill(R, R+256, 0);
    std::fill(G, G+256, 0);
    std::fill(B, B+256, 0);

    for (int i = 0; i < img->size; i++){
        RGB &pixel = img->data[i];
        R[pixel.R]++;
        G[pixel.G]++;
        B[pixel.B]++;
    }
}*/

int main(int argc, char *argv[])
{
    cl_int err;
    cl_platform_id p_id;
    cl_device_id d_id;
    cl_context cx;
    cl_command_queue cq;
    cl_program pg;
    cl_kernel k;
    cl_mem input;
    cl_mem o_r;
    cl_mem o_g;
    cl_mem o_b;
    int fr[256];
	int fg[256];
	int fb[256];
    size_t ws_l[2] = {64, 64};
    size_t ws_g[2];
	
	clGetPlatformIDs(1, &p_id, NULL);
	err = clGetDeviceIDs(p_id, CL_DEVICE_TYPE_GPU, 1, &d_id, NULL);
	cx = clCreateContext(0, 1, &d_id, NULL, NULL, &err);
	cq = clCreateCommandQueue(cx, d_id, 0, &err);
	std::ifstream infile("histogram.cl", std::ios_base::in);
    infile.seekg(0, std::ios_base::end);
    std::vector<char> d;
	int l;
    l = infile.tellg();
    infile.seekg(0, std::ios_base::beg);
    d = std::vector<char>(l + 1);
    infile.read(&d[0], l);
    d[l] = 0;
    const char *s = &d[0];
	pg = clCreateProgramWithSource(cx, 1, &s, 0, 0);
	infile.close();
	err = clBuildProgram(pg, 0, NULL, NULL, NULL, NULL); 
	k = clCreateKernel(pg, "histogram", &err);
    char *filename;
    if (argc >= 2)
    {
        int many_img = argc - 1;
        for (int i = 0; i < many_img; i++)
        {
            filename = argv[i + 1];
            Image *img = readbmp(filename);

            std::cout << img->weight << ":" << img->height << "\n";
            /*
            uint32_t R[256];
            uint32_t G[256];
            uint32_t B[256];

            histogram(img,R,G,B);
            */
	        input = clCreateBuffer(cx, CL_MEM_READ_ONLY, img->size * sizeof(RGB), NULL, NULL);
	        o_r = clCreateBuffer(cx, CL_MEM_WRITE_ONLY, 256 * sizeof(int), NULL, NULL);
	        o_g = clCreateBuffer(cx, CL_MEM_WRITE_ONLY, 256 * sizeof(int), NULL, NULL);
	        o_b = clCreateBuffer(cx, CL_MEM_WRITE_ONLY, 256 * sizeof(int), NULL, NULL);
	        err = clEnqueueWriteBuffer(cq, input, CL_TRUE, 0, img->size * sizeof(RGB), img->data, 0, NULL, NULL);
	        err = clSetKernelArg(k, 0, sizeof(cl_mem), &input);
	        err = clSetKernelArg(k, 1, sizeof(int), &img->size);
	        err = clSetKernelArg(k, 2, sizeof(cl_mem), &o_r);
	        err = clSetKernelArg(k, 3, sizeof(cl_mem), &o_g);
	        err = clSetKernelArg(k, 4, sizeof(cl_mem), &o_b);
	        if (img->size % ws_l[0] != 0) {
                int rm = img->size % ws_l[0];
	            ws_g[0] = img->size - rm + ws_l[0];
	        } else {
	            ws_g[0] = img->size;
	        }
	        if (img->size % ws_l[1] != 0) {
                int rm = img->size % ws_l[1];
	            ws_g[1] = img->size - rm + ws_l[1];
	        } else {
	            ws_g[1] = img->size;
	        }
	        err = clEnqueueNDRangeKernel(cq, k, 1, NULL, ws_g, ws_l, 0, NULL, NULL);
	        clFinish(cq);
	        err = clEnqueueReadBuffer(cq, o_r, CL_TRUE, 0, 256 * sizeof(int), fr, 0, NULL, NULL);
	        err = clEnqueueReadBuffer(cq, o_g, CL_TRUE, 0, 256 * sizeof(int), fg, 0, NULL, NULL);
	        err = clEnqueueReadBuffer(cq, o_b, CL_TRUE, 0, 256 * sizeof(int), fb, 0, NULL, NULL);
            int max = 0;
            for(int i=0;i<256;i++){
                max = fr[i] > max ? fr[i] : max;
                max = fg[i] > max ? fg[i] : max;
                max = fb[i] > max ? fb[i] : max;
            }

            Image *ret = new Image();
            ret->type = 1;
            ret->height = 256;
            ret->weight = 256;
            ret->size = 256 * 256;
            ret->data = new RGB[256 * 256]{};

            for(int i=0;i<ret->height;i++){
                for(int j=0;j<256;j++){
                    if(fr[j]*256/max > i)
                        ret->data[256*i+j].R = 255;
                    if(fg[j]*256/max > i)
                        ret->data[256*i+j].G = 255;
                    if(fb[j]*256/max > i)
                        ret->data[256*i+j].B = 255;
                }
            }

            std::string newfile = "hist_" + std::string(filename); 
            writebmp(newfile.c_str(), ret);
            delete [] ret;
        }
    }else{
        printf("Usage: ./hist <img.bmp> [img2.bmp ...]\n");
    }
    clReleaseMemObject(input);
	clReleaseMemObject(o_r);
	clReleaseMemObject(o_g);
    clReleaseMemObject(o_b);
    clReleaseProgram(pg);
    clReleaseKernel(k);
    clReleaseCommandQueue(cq);
    clReleaseContext(cx);
    return 0;
}
