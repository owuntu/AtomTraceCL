#include <cstdlib>
#include <iostream>

#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL\cl2.hpp> // main OpenCL include file 

#include <glfw3.h>

#include "lodepng.h"

#include "Vector3.h"
#include "Camera.h"
#include "CLResourceManager.h"
#include "RenderImage.h"
#include "Utilities.h"


static GLFWwindow* gs_pWindow;

namespace GLFWFunc
{
    void error_callback(int error, const char* description)
    {
        fputs(description, stderr);
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
} // namespace GLFWFunc

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

bool InitGLFWWindow(int width, int height)
{
    using namespace GLFWFunc;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        std::cerr << "Error in glfwInit() call.\n";
        return false;
    }

    gs_pWindow = glfwCreateWindow(width, height, "AtomTraceCL", nullptr, nullptr);
    if (nullptr == gs_pWindow)
    {
        std::cerr << "Error in glfwCreateWindow().\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(gs_pWindow);
    glfwSwapInterval(1);
    glfwSetKeyCallback(gs_pWindow, key_callback);
    return true;
}

void ShutDownGLFW()
{
    if (nullptr != gs_pWindow)
    {
        glfwDestroyWindow(gs_pWindow);
        glfwTerminate();
        gs_pWindow = nullptr;
    }
}

int main(int argc, char** argv)
{
    std::string kernelStr;
    ReadFileToString("kernel\\renderer.cl", kernelStr);

    const int IMAGE_WIDTH = 800;
    const int IMAGE_HEIGHT = 600;

    InitGLFWWindow(IMAGE_WIDTH, IMAGE_HEIGHT);

    size_t worksize = IMAGE_WIDTH * IMAGE_HEIGHT;
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

    cl::Buffer clCounter;
    float counter = 0.f;
    clCounter = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float), &counter, &error);
    if (!CheckError(error))
    {
        std::cerr << "create clCounter fail\n";
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

    error = kernel.setArg(4, clCounter);
    if (!CheckError(error))
    {
        std::cerr << "set kernel args4 fail\n";
    }

    while (!glfwWindowShouldClose(gs_pWindow))
    {
        error = cq.enqueueWriteBuffer(clCounter, CL_TRUE, 0, sizeof(int), &counter);
        if (!CheckError(error))
        {
            std::cerr << "enqueue write buffer fail\n";
        }

        // Tell the device, through the command queue, to execute queue Kernel
        error = cq.enqueueNDRangeKernel(kernel, 0, worksize, 256);
        if (!CheckError(error))
        {
            std::cerr << "enqueue NDRange kernel fail\n";
        }

        // Read the result back into image
        error = cq.enqueueReadBuffer(mem1, CL_FALSE, 0, worksize * 3, image.GetRawData());
        if (!CheckError(error))
        {
            std::cerr << "enqueue read buffer fail\n";
        }

        glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image.GetRawData());

        glfwSwapBuffers(gs_pWindow);
        glfwPollEvents();

        counter+=0.01f;
    }

    // Read the FINAL result back into image
    error = cq.enqueueReadBuffer(mem1, CL_TRUE, 0, worksize * 3, image.GetRawData());
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
    ShutDownGLFW();

    image.SavePNG("image.png");
    image.Release();

    return 0;
}