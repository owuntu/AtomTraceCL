#include <cstdlib>
#include <iostream>

//#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL\cl2.hpp> // main OpenCL include file 

#include "lodepng.h"

#include "Vector3.h"
#include "Camera.h"
#include "CLResourceManager.h"
#include "RenderImage.h"
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

    const int IMAGE_WIDTH = 800;
    const int IMAGE_HEIGHT = 600;
    size_t worksize = IMAGE_WIDTH * IMAGE_HEIGHT;
    //unsigned char* pImg = new unsigned char[worksize * 3];
    RenderImage image(IMAGE_WIDTH, IMAGE_HEIGHT);

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
                      worksize * 3 * sizeof(unsigned char), image.GetRawData(), &error);
    if (!CheckError(error))
    {
        std::cerr << "create mem1 fail\n";
    }

    cl::Buffer clCam;
    AtomTraceCL::Camera cam;
    cam.Init(AtomMathCL::Vector3::UNIT_Z, AtomMathCL::Vector3::ZERO, 30.f, IMAGE_WIDTH, IMAGE_HEIGHT);
    clCam = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(AtomTraceCL::Camera), &cam, &error);
    if (!CheckError(error))
    {
        std::cerr << "create clCam fail\n";
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

    error = kernel.setArg(1, IMAGE_WIDTH);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args1 fail\n";
    }

    error = kernel.setArg(2, IMAGE_HEIGHT);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args2 fail\n";
    }

    error = kernel.setArg(3, clCam);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args3 fail\n";
    }

    // Tell the device, through the command queue, to execute queue Kernel
    error = cq.enqueueNDRangeKernel(kernel, 0, worksize, 256);
    if (!CheckError(error))
    {
        std::cerr << "enqueue NDRange kernel fail\n";
    }

    // Read the result back into pImg
    // the "CL_TRUE" flag blocks the read operation until all work items have finished their computation
    error = cq.enqueueReadBuffer(mem1, CL_TRUE, 0, worksize*3, image.GetRawData());
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

    image.SavePNG("image.png");
    image.Release();

    system("pause");

    return 0;
}