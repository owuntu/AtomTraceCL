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
#include "Sphere.h"
#include "ObjectList.h"
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
    using namespace AtomTraceCL;
    std::string kernelStr;
    ReadFileToString("kernel\\renderer.cl", kernelStr);

    const int IMAGE_WIDTH = 800;
    const int IMAGE_HEIGHT = 600;

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
    CheckError(error, "Create program");

    // Compile the kernel code (after this we could extract the compiled version)
    error = program.build({device});
    if (!CheckError(error, "Build program"))
    {
        char msg[1024] = { 0 };
        //program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, msg);
        clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, sizeof(msg), msg, nullptr);
        std::cerr << msg << std::endl;
        ShutDownGLFW();
        system("pause");
        return 1;
    }

    // Create memory buffers in the Context where the desired Device is. 
    // These will be the pointer parameters on the kernel.
    cl::Buffer mem1;
    mem1 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                      worksize * 3 * sizeof(unsigned char), image.GetRawData(), &error);
    CheckError(error, "Create mem1");

    cl::Buffer clCam;
    AtomTraceCL::Camera cam;
    cam.Init(AtomMathCL::Vector3::UNIT_Z, AtomMathCL::Vector3::ZERO, 30.f, IMAGE_WIDTH, IMAGE_HEIGHT);
    clCam = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(AtomTraceCL::Camera), &cam, &error);
    CheckError(error, "Create clCam");

    // Create the scene
    ObjectList oList;
    // Load scene
    {
        Sphere s1(0.1f, AtomMathCL::Vector3(0.f, 0.f, -2.0f));
        s1.SetEmission(AtomMathCL::Vector3(1.0f));
        oList.AddObject(s1);
        Sphere s0(0.1f, AtomMathCL::Vector3(-0.3f, 0.f, -2.0f));
        s0.SetColor(AtomMathCL::Vector3(0.75f, 0.25f, .25f));
        oList.AddObject(s0);
    }
    cl::Buffer clScene;
    clScene = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, oList.m_size, oList.m_pData);

    // Create a kernel object with the compiled program
    cl::Kernel kernel = cl::Kernel(program, "RenderKernel", &error);
    CheckError(error, "Create kernel");

    // Set the kernel parameters
    error = kernel.setArg(0, mem1);
    CheckError(error, "Set kernel args0");

    error = kernel.setArg(1, IMAGE_WIDTH);
    CheckError(error, "Set kernel args1");

    error = kernel.setArg(2, IMAGE_HEIGHT);
    CheckError(error, "Set kernel args2");

    error = kernel.setArg(3, clCam);
    CheckError(error, "Set kernel args3");

    error = kernel.setArg(4, clScene);
    error = kernel.setArg(5, oList.m_size);
    error = kernel.setArg(6, oList.m_numObj);

    InitGLFWWindow(IMAGE_WIDTH, IMAGE_HEIGHT);

    // OpenGL viewport loop
    while (!glfwWindowShouldClose(gs_pWindow))
    {
        // Tell the device, through the command queue, to execute queue Kernel
        error = cq.enqueueNDRangeKernel(kernel, 0, worksize, 256);
        CheckError(error,"Enqueue NDRange kernel");

        // Read the result back into image
        error = cq.enqueueReadBuffer(mem1, CL_FALSE, 0, worksize * 3, image.GetRawData());
        CheckError(error, "Enqueue read buffer");

        glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image.GetRawData());

        glfwSwapBuffers(gs_pWindow);
        glfwPollEvents();
    }

    // Read the FINAL result back into image
    error = cq.enqueueReadBuffer(mem1, CL_TRUE, 0, worksize * 3, image.GetRawData());
    CheckError(error, "Enqueue read buffer");

    // Await completion of all the above
    error = cq.finish();
    CheckError(error, "cq.finish()");

    ShutDownGLFW();

    image.SavePNG("image.png");
    image.Release();

    return 0;
}