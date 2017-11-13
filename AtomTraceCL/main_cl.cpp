#include <cstdlib>
#include <iostream>
#include <CL\opencl.h>

#include "Utilities.h"

#define STR(x) #x

static const char* gs_SAMPLE_KERNEL = STR(\
    __kernel void example(__global char* buf,   \n
                          __global char* buf2)  \n
    {                                           \n
        int x = get_global_id(0);               \n
        buf2[x] = buf[x];                       \n
    }                                           \n
);

bool CheckError(cl_int error)
{
    bool ret = true;
    if (CL_SUCCESS != error)
    {
        std::cerr << "Error code: " << error << "\n";
        ret = false;
    }
    return ret;
}

int main(int argc, char** argv)
{
    using namespace std;

    string kernelStr;
    ReadFileToString("kernel\\example.cl", kernelStr);
    cout << kernelStr << "\n";

    // Original sample code from Google code: https://code.google.com/p/simple-opencl/
    char buf[] = "Hello World!";

    size_t srcsize, worksize = strlen(buf);
    cout << "Worksize: " << worksize << "\n";
    cl_int error;
    cl_platform_id* platform = nullptr;
    cl_device_id device;
    cl_uint numPlf, devices;

    // Fetch number of available OpenCL platform
    error = clGetPlatformIDs(0, nullptr, &numPlf);
    if (!CheckError(error))
    {
        std::cerr << "GetPlatformID number of platform fail\n";
    }

    if (0 != numPlf)
    {
        platform = new cl_platform_id[numPlf];
        // Fetch the Platforms
        error = clGetPlatformIDs(numPlf, platform, &numPlf);
        if (!CheckError(error))
        {
            std::cerr << "GetPlatformID fail\n";
        }
    }

    {
        // Fetch the OpenCL version
        const size_t BUF_SIZE = 256;
        char vBuf[BUF_SIZE] = { 0 };
        size_t actSize = 0;
        error = clGetPlatformInfo(platform[0], CL_PLATFORM_VERSION, BUF_SIZE, vBuf, &actSize);
        cout << "OpenCL platform version:\t" << vBuf << "\n";

        error = clGetPlatformInfo(platform[0], CL_PLATFORM_PROFILE, BUF_SIZE, vBuf, &actSize);
        cout << "OpenCL platform profile: " << vBuf << "\n";

        error = clGetPlatformInfo(platform[0], CL_PLATFORM_VENDOR, BUF_SIZE, vBuf, &actSize);
        cout << "Vendor: " << vBuf << "\n";

        error = clGetPlatformInfo(platform[0], CL_PLATFORM_NAME, BUF_SIZE, vBuf, &actSize);
        cout << "Platform name: " << vBuf << "\n";

        error = clGetPlatformInfo(platform[0], CL_PLATFORM_EXTENSIONS, BUF_SIZE, vBuf, &actSize);
        cout << "Platform extensions: " << vBuf << "\n";
    }

    // Fetch the Devices for this platform
    error = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_ALL, 1, &device, &devices);
    if (!CheckError(error))
    {
        std::cerr << "Get Device ID fail\n";
    }

    {
        // Fetch the device name
        const size_t BUF_SIZE = 256;
        char vBuf[BUF_SIZE] = { 0 };
        size_t actSize = 0;
        error = clGetDeviceInfo(device, CL_DEVICE_NAME, BUF_SIZE, vBuf, &actSize);
        cout << "Device name: " << vBuf << "\n";
        error = clGetDeviceInfo(device, CL_DEVICE_VENDOR, BUF_SIZE, vBuf, &actSize);
        cout << "Device vendor: " << vBuf << "\n";
        error = clGetDeviceInfo(device, CL_DEVICE_VERSION, BUF_SIZE, vBuf, &actSize);
        cout << "Device version: " << vBuf << "\n";
    }

    // Create a memory context for the device we want to use
    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform[0], 0 };

    // Note that nVidia's OpenCL requires the platform property
    cl_context context = clCreateContext(properties, 1, &device, NULL, NULL, &error);
    if (!CheckError(error))
    {
        std::cerr << "Create context fail\n";
    }

    // Create a command queue to communicate with the device
    cl_command_queue cq = clCreateCommandQueueWithProperties(context, device, NULL, &error);
    if (!CheckError(error))
    {
        std::cerr << "create command queue fail\n";
    }

    // Read the source kernel code in sampleKernel
    
    // Submit the source code of the kernel to OpenCL, and create a program object with it
    const char* kernelSrc = kernelStr.c_str();
    // srcsize = strlen(gs_SAMPLE_KERNEL);
    srcsize = kernelStr.size();
    cout << "Kernel source size: " << srcsize << "\n";
    cl_program prog = clCreateProgramWithSource(context, 
                                                1, 
                                                &kernelSrc, 
                                                &srcsize, 
                                                &error);
    if (!CheckError(error))
    {
        std::cerr << "create program fail\n";
    }

    // Compile the kernel code (after this we could extract the compiled version)
    error = clBuildProgram(prog, 1, &device, "", NULL, NULL);
    if (!CheckError(error))
    {
        std::cerr << "build program fail\n";
    }

    // Create memory buffers in the Context where the desired Device is. 
    // These will be the pointer parameters on the kernel.
    cl_mem mem1, mem2;
    mem1 = clCreateBuffer(context, CL_MEM_READ_ONLY, worksize, NULL, &error);
    if (!CheckError(error))
    {
        std::cerr << "create mem1 fail\n";
    }

    mem2 = clCreateBuffer(context, CL_MEM_READ_ONLY, worksize, NULL, &error);
    if (!CheckError(error))
    {
        std::cerr << "create mem2 fail\n";
    }

    // Create a kernel object with the compiled program
    cl_kernel k_example = clCreateKernel(prog, "example", &error);
    if (!CheckError(error))
    {
        std::cerr << "create kernel fail\n";
    }

    // Set the kernel parameters
    error = clSetKernelArg(k_example, 0, sizeof(mem1), &mem1);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args fail\n";
    }

    error = clSetKernelArg(k_example, 1, sizeof(mem2), &mem2);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args2 fail\n";
    }

    // Create a char array in where to store the results of the kernel
    char buf2[sizeof buf];
    buf2[0] = '?';
    buf2[worksize] = 0;

    cout << sizeof(buf2) << "\n";

    // Send input data to OpenCL (async, don't alter the buffer!)
    error = clEnqueueWriteBuffer(cq, mem1, CL_FALSE, 0, worksize, buf, 0, NULL, NULL);
    if (!CheckError(error))
    {
        std::cerr << "enqueue write buffer fail\n";
    }

    // Tell the device, through the command queue, to execute que Kernel
    error = clEnqueueNDRangeKernel(cq, k_example, 1, NULL, &worksize, &worksize, 0, NULL, NULL);
    if (!CheckError(error))
    {
        std::cerr << "enqueue NDRange kernel fail\n";
    }

    // Read the result back into buf2
    error = clEnqueueReadBuffer(cq, mem2, CL_FALSE, 0, worksize, buf2, 0, NULL, NULL);
    if (!CheckError(error))
    {
        std::cerr << "enqueue read buffer fail\n";
    }

    // Await completion of all the above
    error = clFinish(cq);
    if (!CheckError(error))
    {
        std::cerr << "clFinish(cq) fail\n";
    }

    // Finally, ouput the result
    cout << "buf2: ["; 
    for (int i = 0; i < sizeof(buf2); ++i){
        cout << buf2[i];
    }
    cout << "]\n";

    if (nullptr != platform)
    {
        delete[] platform;
        platform = nullptr;
    }

    system("pause");

    return 0;
}