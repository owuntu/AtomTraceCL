#include <cstdlib>
#include <iostream>
#include <iomanip>

#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL\cl2.hpp> // main OpenCL include file 

#include <glfw3.h>

#include "lodepng.h"

// AtomMathCL
#include "Vector3.h"

#include "Camera.h"
#include "CLResourceManager.h"
#include "RenderImage.h"
#include "SceneLoader.h"
#include "Timer.h"
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
    error = program.build({ device }, "-I kernel -D OPENCL_COMPILER");
    {
        char* msg = nullptr;
        std::size_t len = 0;
        clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, 0, nullptr, &len);
        if (len > 0)
        {
            msg = new char[len];
            clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, len, msg, nullptr);
            std::cerr << msg << std::endl;
            delete[] msg;
        }
    }
    if (!CheckError(error, "Build program"))
    {
        ShutDownGLFW();
        system("pause");
        return 1;
    }

    // Create memory buffers in the Context where the desired Device is. 
    // These will be the pointer parameters on the kernel.
    cl::Buffer pixelBuffer;
    pixelBuffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                      worksize * 3 * sizeof(unsigned char), image.GetRawData(), &error);
    CheckError(error, "Create mem1");

    cl::Buffer clCam;
    AtomTraceCL::Camera cam;
    cam.Init(AtomMathCL::Vector3(0.f, -5.f, 60.f), AtomMathCL::Vector3(0.f, -10.f, 0.f), 40.f, IMAGE_WIDTH, IMAGE_HEIGHT);
    clCam = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(AtomTraceCL::Camera), &cam, &error);
    CheckError(error, "Create clCam");

    // Create the scene
    // Get maximum memory size that can be allocate on GPU device
    cl_ulong maxMem = 0;
    error = device.getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE, &maxMem);
    CheckError(error, "Get GPU memory allocation size");
    if (0 == maxMem)
    {
        std::cerr << "Fail to get maximum GPU memory allocation size.\n";
    }
    // Allocate memory buffer on CPU side for object list
    ObjectList oList(maxMem / 8);

    std::cout << "Loading scene...\n";
    LoadScene(oList);

    cl::Buffer clScene;
    clScene = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, oList.m_size, oList.m_pData);
    CheckError(error, "Create clScene");

    __int32* pTable = new int[oList.m_indexTable.size()];
    for (std::size_t i = 0; i < oList.m_indexTable.size(); ++i)
    {
        *(pTable + i) = oList.m_indexTable[i];
    }
    cl::Buffer clIndexTable = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, oList.m_indexTable.size() * sizeof(__int32), pTable);
    CheckError(error, "Create clIndexTable");
    delete[] pTable;

    unsigned __int32* pSeeds = new unsigned __int32[worksize * 2];
    for (int i = 0; i < worksize * 2; ++i)
    {
        pSeeds[i] = GetRandomNumber01() * 0xfffffffd + 2;
    }
    cl::Buffer clSeeds = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, worksize * 2 * sizeof(unsigned __int32), pSeeds);
    CheckError(error, "Create clSeeds");


    float *pColor = new float[worksize * 3];
    memset(pColor, 0, worksize * 3 * sizeof(float));
    cl::Buffer clColor = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, worksize * 3 * sizeof(float), pColor);

    // Create a kernel object with the compiled program
    cl::Kernel kernel = cl::Kernel(program, "RenderKernel", &error);
    CheckError(error, "Create kernel");

    cl_uint paramIndex = 0;
    // Setup kernel parameters
    error = kernel.setArg(paramIndex, pixelBuffer);
    CheckError(error, "Set kernel args0 pixel buffer");

    paramIndex++;
    error = kernel.setArg(paramIndex, clScene);
    CheckError(error, "Set clScene");

    paramIndex++;
    error = kernel.setArg(paramIndex, clCam);
    CheckError(error, "Set kernel args cam");


    paramIndex++;
    error = kernel.setArg(paramIndex, clIndexTable);
    CheckError(error, "Set kernel args index table");

    paramIndex++;
    error = kernel.setArg(paramIndex, oList.m_numObj);
    CheckError(error, "Set num obj");

    paramIndex++;
    error = kernel.setArg(paramIndex, IMAGE_WIDTH);
    CheckError(error, "Set kernel args img width");

    paramIndex++;
    error = kernel.setArg(paramIndex, IMAGE_HEIGHT);
    CheckError(error, "Set kernel args img height");
    
    paramIndex++;
    error = kernel.setArg(paramIndex, clSeeds);
    CheckError(error, "Set kernel args7(clSeeds)");

    //paramIndex++;
    //error = kernel.setArg(paramIndex, clColor);
   // CheckError(error, "Set kernel args8(clColor)");

    InitGLFWWindow(IMAGE_WIDTH, IMAGE_HEIGHT);

    std::cout << "\n";

    Timer::Init();

    unsigned int currentSample = 0;
    unsigned int sampleIncresment = 8;

    //paramIndex++;
    //error = kernel.setArg(paramIndex, sampleIncresment);
   // CheckError(error, "Set kernel arg9 (sampleInc)");

    bool isFinished = false;
    Timer::Tick loopStart = Timer::GetCurrentTick();
    // OpenGL viewport loop
    while (!glfwWindowShouldClose(gs_pWindow))
    {
        Timer::Tick tstart = Timer::GetCurrentTick();
        if (currentSample < 0xffffffff)
        {
            //error = kernel.setArg(paramIndex+1, currentSample);
            //CheckError(error, "Set kernel args8 (currentSample)");

            currentSample += sampleIncresment;

            // Tell the device, through the command queue, to execute queue Kernel
            error = cq.enqueueNDRangeKernel(kernel, 0, worksize, 256);
            CheckError(error, "Enqueue NDRange kernel");

            std::cout << "\rCurrent samples: " << currentSample << ".";
        }
        else
        {
            isFinished = true;
        }

        // Read the result back into image
        error = cq.enqueueReadBuffer(pixelBuffer, CL_TRUE, 0, worksize * 3, image.GetRawData());
        CheckError(error, "Enqueue read buffer");

        glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image.GetRawData());

        glfwSwapBuffers(gs_pWindow);
        glfwPollEvents();
        if (!isFinished)
        {
            Timer::Tick tend = Timer::GetCurrentTick();
            float msElp = Timer::GetDifferentTickToMS(tstart, tend);
            float fps = 1000.f / msElp;
            float msSample = msElp / static_cast<float>(sampleIncresment);

            float msLoop = Timer::GetDifferentTickToMS(loopStart, tend);
            float avgSpt = msLoop / static_cast<float>(currentSample);
            std::cout << std::fixed;
            std::cout.precision(2);
            std::cout << "\t frame: " << msElp << "ms. fps: " << fps << ". Sample: " << msSample << "ms. Sample avg: " << avgSpt << "ms. Sample per frame: " << sampleIncresment << " ";
        }
        else
        {
            static bool printOnce = false;
            if (!printOnce)
            {
                printOnce = true;
                std::cout << "\nRender Done.\n";
            }
        }
    }

    std::cout << "\n";

    // Read the FINAL result back into image
    error = cq.enqueueReadBuffer(pixelBuffer, CL_TRUE, 0, worksize * 3, image.GetRawData());
    CheckError(error, "Enqueue read buffer");

    // Await completion of all the above
    error = cq.finish();
    CheckError(error, "cq.finish()");

    ShutDownGLFW();

    image.SavePNG("image.png");
    image.Release();

    if (pSeeds != nullptr)
    {
        delete[] pSeeds;
    }

    if (nullptr != pColor)
    {
        delete[] pColor;
    }

    return 0;
}