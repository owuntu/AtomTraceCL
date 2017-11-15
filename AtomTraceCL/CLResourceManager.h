#ifndef CL_RESOURCE_MANAGER_H_
#define CL_RESOURCE_MANAGER_H_


class cl::Platform;
class cl::Device;
class cl::Context;
class cl::Program;
class cl::Kernel;
class cl::CommandQueue;

class CLResourceManager
{
public:
    CLResourceManager();
    ~CLResourceManager();

    bool Init();

private:
    cl::Platform m_platform;
    cl::Device m_device;
    cl::Context m_context;
    cl::Program m_program;
    cl::Kernel m_kernel;
    cl::CommandQueue m_cq;
};

#endif // CL_RESOURCE_MANAGER_H_
