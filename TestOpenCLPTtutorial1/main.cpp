// Getting started with OpenCL tutorial 
// by Sam Lapere, 2016, http://raytracey.blogspot.com
// Code based on http://simpleopencl.blogspot.com/2013/06/tutorial-simple-start-with-opencl-and-c.html

#include <iostream>
#include <vector>

#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL\cl2.hpp> // main OpenCL include file 

void main()
{
    // Find all available OpenCL platforms (e.g. AMD OpenCL, Nvidia CUDA, Intel OpenCL)
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Show the names of all available OpenCL platforms
    std::cout << "Available OpenCL platforms: \n\n";
    for (unsigned int i = 0; i < platforms.size(); i++)
    {
        std::cout << "\t" << i + 1 << ": " << platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;
        std::cout << "\t" << "   " << platforms[i].getInfo<CL_PLATFORM_VERSION>() << std::endl;
    }

    // Choose and create an OpenCL platform
    std::cout << std::endl << "Enter the number of the OpenCL platform you want to use: ";
    unsigned int input = 0;
    std::cin >> input;
    // Handle incorrect user input
    while (input < 1 || input > platforms.size()) {
        std::cin.clear(); //clear errors/bad flags on std::cin
        std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n'); // ignores exact number of chars in std::cin buffer
        std::cout << "No such platform." << std::endl << "Enter the number of the OpenCL platform you want to use: ";
        std::cin >> input;
    }

    cl::Platform platform = platforms[input - 1];

    // Print the name of chosen OpenCL platform
    std::cout << "Using OpenCL platform: \t" << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

    // Find all available OpenCL devices (e.g. CPU, GPU or integrated GPU)
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    // Print the names of all available OpenCL devices on the chosen platform
    std::cout << "Available OpenCL devices on this platform: " << std::endl << std::endl;
    for (unsigned int i = 0; i < devices.size(); i++)
        std::cout << "\t" << i + 1 << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;

    // Choose an OpenCL device 
    std::cout << std::endl << "Enter the number of the OpenCL device you want to use: ";
    input = 0;
    std::cin >> input;
    // Handle incorrect user input
    while (input < 1 || input > devices.size()) {
        std::cin.clear(); //clear errors/bad flags on std::cin
        std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n'); // ignores exact number of chars in std::cin buffer
        std::cout << "No such device. Enter the number of the OpenCL device you want to use: ";
        std::cin >> input;
    }

    cl::Device device = devices[input - 1];

    // Print the name of the chosen OpenCL device
    std::cout << std::endl << "Using OpenCL device: \t" << device.getInfo<CL_DEVICE_NAME>() << std::endl << std::endl;

    // Create an OpenCL context on that device.
    // the context is the interface between host and device and manages all the OpenCL resources 
    cl::Context context = cl::Context(device);

    ///////////////////
    // OPENCL KERNEL //
    ///////////////////

    // the OpenCL kernel in this tutorial is a simple program that adds two float arrays in parallel  
    // the source code of the OpenCL kernel is passed as a string to the host
    // the "__global" keyword denotes that "global" device memory is used, which can be read and written 
    // to by all work items (threads) and all work groups on the device and can also be read/written by the host (CPU)

    const char* source_string =
        "	__kernel void parallel_add(__global float* x, __global float* y, __global float* z){	"
        "	const int i = get_global_id(0);	" // get a unique number identifying the work item in the global pool
        "	z[i] = y[i] + x[i];				" // add two arrays 
        "}";



    // Create an OpenCL program by performing runtime source compilation
    cl::Program program = cl::Program(context, source_string);

    // Build the program and check for compilation errors 
    cl_int result = program.build({ device }, "");
    if (result) std::cout << "Error during compilation! (" << result << ")" << std::endl;

    // Create a kernel (entry point in the OpenCL program)
    // kernels are the basic units of executable code that run on the OpenCL device
    // the kernel forms the starting point into the OpenCL program, analogous to main() in CPU code
    // kernels can be called from the host (CPU)
    cl::Kernel kernel = cl::Kernel(program, "parallel_add");

    // Create input data arrays on the host (= CPU)
    const int numElements = 10;
    float cpuArrayA[numElements] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
    float cpuArrayB[numElements] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f };
    float cpuOutput[numElements] = {}; // empty array for storing the results of the OpenCL program

                                       // Create buffers (memory objects) on the OpenCL device, allocate memory and copy input data to device.
                                       // Flags indicate how the buffer should be used e.g. read-only, write-only, read-write
    cl::Buffer clBufferA = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, numElements * sizeof(cl_int), cpuArrayA);
    cl::Buffer clBufferB = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, numElements * sizeof(cl_int), cpuArrayB);
    cl::Buffer clOutput = cl::Buffer(context, CL_MEM_WRITE_ONLY, numElements * sizeof(cl_int), NULL);

    // Specify the arguments for the OpenCL kernel
    // (the arguments are __global float* x, __global float* y and __global float* z)
    kernel.setArg(0, clBufferA); // first argument 
    kernel.setArg(1, clBufferB); // second argument 
    kernel.setArg(2, clOutput);  // third argument 

                                 // Create a command queue for the OpenCL device
                                 // the command queue allows kernel execution commands to be sent to the device
    cl::CommandQueue queue = cl::CommandQueue(context, device);

    // Determine the global and local number of "work items"
    // The global work size is the total number of work items (threads) that execute in parallel
    // Work items executing together on the same compute unit are grouped into "work groups"
    // The local work size defines the number of work items in each work group
    // Important: global_work_size must be an integer multiple of local_work_size 
    std::size_t global_work_size = numElements;
    std::size_t local_work_size = 10; // could also be 1, 2 or 5 in this example
                                      // when local_work_size equals 10, all ten number pairs from both arrays will be added together in one go

                                      // Launch the kernel and specify the global and local number of work items (threads)
    queue.enqueueNDRangeKernel(kernel, NULL, global_work_size, local_work_size);

    // Read and copy OpenCL output to CPU 
    // the "CL_TRUE" flag blocks the read operation until all work items have finished their computation
    queue.enqueueReadBuffer(clOutput, CL_TRUE, 0, numElements * sizeof(cl_float), cpuOutput);

    // Print results to console
    for (int i = 0; i < numElements; i++)
        std::cout << cpuArrayA[i] << " + " << cpuArrayB[i] << " = " << cpuOutput[i] << std::endl;

    queue.finish();

    system("PAUSE");
}
