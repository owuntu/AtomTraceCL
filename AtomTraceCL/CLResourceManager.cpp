#include <iostream>
#include <string>
#include <vector>
#include <CL\cl2.hpp>
#include "CLResourceManager.h"

static bool CheckError(cl_int error, const std::string& msg)
{
    bool ret = true;
    if (CL_SUCCESS != error)
    {
        std::cerr << msg << " failed,\t";
        std::cerr << "error code: " << error << "\n";
        ret = false;
    }
    return ret;
}

CLResourceManager::CLResourceManager()
{}

CLResourceManager::~CLResourceManager()
{}


bool CLResourceManager::Init()
{
    cl_int errCode = CL_SUCCESS;
    // pick a platform
    std::vector<cl::Platform> platforms;
    errCode = cl::Platform::get(&platforms);

    if (platforms.size() == 0 || !CheckError(errCode, "Get platforms"))
    {
        std::cerr << "No OpenCL platform available, exit." << std::endl;
        return false;
    }

    if (platforms.size() == 1)
    {
        m_platform = platforms[0];
    }
    else
    {
        unsigned int id = platforms.size();

        while (id >= platforms.size())
        {
            std::cout << "Available OpenCL platforms:\n";
            for (std::size_t i = 0; i < platforms.size(); ++i)
            {
                std::string msg = platforms[i].getInfo<CL_PLATFORM_NAME>(&errCode);
                if (CheckError(errCode, "Query platform name"))
                {
                    std::cout << "\t" << i << ": " << msg << "\n";
                }
            }

            std::cout << "Input selection: ";
            std::cin >> id;
            if (id >= platforms.size())
            {
                std::cout << "Input out of range, please input again.\n";
            }
        }

        m_platform = platforms[id];
    }

    std::cout << "Using OpenCL platform: " << m_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    // pick a device
    std::vector<cl::Device> devices;
    errCode = m_platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    bool bPick = true;

    std::cout << "Finding available OpenCL GPU devices.\n";
    if (!CheckError(errCode, "Get GPU devices") || devices.size() == 0)
    {
        std::cout << "No available OpenCL GPU device available.\n";
        errCode = m_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        if (!CheckError(errCode, "Get all devices") || devices.size() == 0)
        {
            std::cerr << "No available OpenCL device on this platform available.\n";
            return false;
        }
    }

    if (devices.size() == 1)
    {
        m_device = devices[0];
        bPick = false;
    }

    if (bPick)
    {
        // pick a OpenCL device
        unsigned int id = devices.size();
        while (id >= devices.size())
        {
            std::cout << "Available OpenCL devices: \n";
            for (std::size_t i = 0; i < devices.size(); ++i)
            {
                std::cout << "\t" << i << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << "\n";
            }
            std::cout << "Input selection: ";
            std::cin >> id;
            if (id >= devices.size())
            {
                std::cout << "Input out of range, input again.\n";
            }
        }
        m_device = devices[id];
    }

    std::cout << "Using OpenCL device: " << m_device.getInfo<CL_DEVICE_NAME>() << std::endl;

    // Create OpenCL context
    m_context = cl::Context(m_device, nullptr, nullptr, nullptr, &errCode);
    if (!CheckError(errCode, "Create context"))
    {
        return false;
    }

    // Create command queue
    m_cq = cl::CommandQueue(m_context, m_device, 0, &errCode);
    if (!CheckError(errCode, "Create command queue"))
    {
        return false;
    }

    return true;
}

const cl::Device& CLResourceManager::Device() const
{
    return m_device;
}

const cl::Context& CLResourceManager::Context() const
{
    return m_context;
}

const cl::CommandQueue& CLResourceManager::CommandQueue() const
{
    return m_cq;
}
