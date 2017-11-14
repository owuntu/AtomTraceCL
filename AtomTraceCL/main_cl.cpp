#include <cstdlib>
#include <iostream>

//#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL\cl2.hpp> // main OpenCL include file 

#include "lodepng.h"

#include "Utilities.h"


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
    std::string kernelStr;
    ReadFileToString("kernel\\renderer.cl", kernelStr);
    //std::cout << kernelStr << "\n";

    int width = 800;
    int height = 600;
    size_t worksize = width * height;
    unsigned char* pImg = new unsigned char[worksize * 3];

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
        char msg[1024] = { 0 };
        //program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, msg);
        clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, sizeof(msg), msg, nullptr);
        std::cout << msg << std::endl;
    }

    // Create memory buffers in the Context where the desired Device is. 
    // These will be the pointer parameters on the kernel.
    cl::Buffer mem1;
    mem1 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                      worksize * 3 * sizeof(unsigned char), pImg, &error);
    if (!CheckError(error))
    {
        std::cerr << "create mem1 fail\n";
    }

    // Create a kernel object with the compiled program
    cl::Kernel kernel = cl::Kernel(program, "render", &error);
    if (!CheckError(error))
    {
        std::cerr << "create kernel fail\n";
    }

    // Set the kernel parameters
    error = kernel.setArg(0, mem1);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args0 fail\n";
    }

    error = kernel.setArg(1, width);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args1 fail\n";
    }

    error = kernel.setArg(2, height);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args2 fail\n";
    }

    // Tell the device, through the command queue, to execute queue Kernel
    error = cq.enqueueNDRangeKernel(kernel, 0, worksize, 256);
    if (!CheckError(error))
    {
        std::cerr << "enqueue NDRange kernel fail\n";
    }

    // Read the result back into pImg
    // the "CL_TRUE" flag blocks the read operation until all work items have finished their computation
    error = cq.enqueueReadBuffer(mem1, CL_TRUE, 0, worksize*3, pImg);
    if (!CheckError(error))
    {
        std::cerr << "enqueue read buffer fail\n";
    }

    // Await completion of all the above
    error = cq.finish();
    if (!CheckError(error))
    {
        std::cerr << "cq.finish() fail\n";
    }

    lodepng::encode("image.png", pImg, width, height, LCT_RGB, 8);

    if (nullptr != pImg)
    {
        delete[] pImg;
    }
    system("pause");

    return 0;
}