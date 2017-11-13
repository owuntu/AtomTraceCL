#include <cstdlib>
#include <iostream>

//#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL\cl2.hpp> // main OpenCL include file 

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
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;

    // Fetch number of available OpenCL platform
    error = cl::Platform::get(&platforms);
    if (!CheckError(error))
    {
        std::cerr << "GetPlatformID number of platform fail\n";
    }

    for (std::size_t i = 0; i < platforms.size(); i++)
    {
        std::cout << i << ": Platform name:" << platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;
        std::cout << "   OpenCL version: " << platforms[i].getInfo<CL_PLATFORM_VERSION>() << std::endl;
        std::cout << "   Platform profile: " << platforms[i].getInfo<CL_PLATFORM_PROFILE>() << std::endl;
        std::cout << "   Vendor: " << platforms[i].getInfo<CL_PLATFORM_VENDOR>() << std::endl;
        std::cout << "   Extensions: " << platforms[i].getInfo<CL_PLATFORM_EXTENSIONS>() << std::endl;
    }

    cl::Platform platform = platforms[0]; // pick the first platform by default

    // Fetch the Devices for this platform
    error = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (!CheckError(error))
    {
        std::cerr << "Get Device ID fail\n";
    }

    for (std::size_t i = 0; i < devices.size(); ++i)
    {
        std::cout << i << ": Device name: " << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << "   Vendor: " << devices[i].getInfo<CL_DEVICE_VENDOR>() << std::endl;
        std::cout << "   Version: " << devices[i].getInfo<CL_DEVICE_VERSION>() << std::endl;
    }

    cl::Device device = devices[0]; // choose the first device by default

    // Create a memory context for the device we want to use
    //cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform(), 0 };

    cl::Context context = cl::Context(device, nullptr, nullptr, nullptr, &error);
    if (!CheckError(error))
    {
        std::cerr << "Create context fail\n";
    }

    // Create a command queue to communicate with the device
    cl::CommandQueue cq = cl::CommandQueue(context, device, 0, &error);
    if (!CheckError(error))
    {
        std::cerr << "create command queue fail\n";
    }

    // Read the source kernel code in sampleKernel
    
    // Submit the source code of the kernel to OpenCL, and create a program object with it
    cl::Program program = cl::Program(context, kernelStr.c_str(), false, &error);
    if (!CheckError(error))
    {
        std::cerr << "create program fail\n";
    }

    // Compile the kernel code (after this we could extract the compiled version)
    error = program.build({device});
    if (!CheckError(error))
    {
        std::cerr << "build program fail\n";
    }

    // Create memory buffers in the Context where the desired Device is. 
    // These will be the pointer parameters on the kernel.
    cl::Buffer mem1, mem2;
    mem1 = cl::Buffer(context, CL_MEM_READ_ONLY, worksize, nullptr, &error);
    if (!CheckError(error))
    {
        std::cerr << "create mem1 fail\n";
    }

    mem2 = cl::Buffer(context, CL_MEM_READ_ONLY, worksize, nullptr, &error);
    if (!CheckError(error))
    {
        std::cerr << "create mem2 fail\n";
    }

    // Create a kernel object with the compiled program
    cl::Kernel k_example = cl::Kernel(program, "example", &error);
    if (!CheckError(error))
    {
        std::cerr << "create kernel fail\n";
    }

    // Set the kernel parameters
    error = k_example.setArg(0, mem1);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args fail\n";
    }

    error = k_example.setArg(1, mem2);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args2 fail\n";
    }

    // Create a char array in where to store the results of the kernel
    char buf2[sizeof buf] = { 0 };
    buf2[0] = '?';
    buf2[worksize] = 0;

    // Send input data to OpenCL (async, don't alter the buffer!)
    error = cq.enqueueWriteBuffer(mem1, CL_FALSE, 0, worksize, buf);
    if (!CheckError(error))
    {
        std::cerr << "enqueue write buffer fail\n";
    }

    // Tell the device, through the command queue, to execute que Kernel
    error = cq.enqueueNDRangeKernel(k_example, 0, worksize, worksize);
    if (!CheckError(error))
    {
        std::cerr << "enqueue NDRange kernel fail\n";
    }

    // Read the result back into buf2
    error = cq.enqueueReadBuffer(mem2, CL_FALSE, 0, worksize, buf2);
    if (!CheckError(error))
    {
        std::cerr << "enqueue read buffer fail\n";
    }

    // Await completion of all the above
    error = cq.finish();
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

    system("pause");

    return 0;
}