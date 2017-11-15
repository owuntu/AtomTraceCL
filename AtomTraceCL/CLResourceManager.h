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

    // Accessor
    const cl::Device& Device() const;
    const cl::Context& Context() const;
    const cl::CommandQueue& CommandQueue() const;

private:
    cl::Platform m_platform;
    cl::Device m_device;
    cl::Context m_context;
    cl::CommandQueue m_cq;
};

#endif // CL_RESOURCE_MANAGER_H_
