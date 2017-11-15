#include <cstdlib>
#include <iostream>

//#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL\cl2.hpp> // main OpenCL include file 

#include "lodepng.h"

#include "CLResourceManager.h"
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

    int width = 800;
    int height = 600;
    size_t worksize = width * height;
    unsigned char* pImg = new unsigned char[worksize * 3];

    cl_int error;
    CLResourceManager myOpenCL;
    myOpenCL.Init();

    cl::Device device = myOpenCL.Device();
    cl::Context context = myOpenCL.Context();
    cl::CommandQueue cq = myOpenCL.CommandQueue();
    
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