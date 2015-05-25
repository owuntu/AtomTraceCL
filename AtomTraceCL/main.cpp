#include <cstdlib>
#include <glfw3.h>

#include "Ray.h"
#include "SimpleImage.h"
#include "Sphere.h"

using namespace AtomMathCL;

static SimpleImage* gs_pImage;

static void error_callback(int error, const char* description);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
    Sphere s0(1.0f, Vector3::ZERO);

    const unsigned int IMAGE_WIDTH = 512;
    const unsigned int IMAGE_HEIGHT = 512;

    SimpleImage image(IMAGE_WIDTH, IMAGE_HEIGHT);

    gs_pImage = &image;

    for (unsigned int i = 0; i < IMAGE_WIDTH; ++i)
    {
        for (unsigned int j = 0; j < IMAGE_HEIGHT; ++j)
        {
            float xPer = static_cast<float>(i) / static_cast<float>(IMAGE_WIDTH  - 1);
            float yPer = static_cast<float>(j) / static_cast<float>(IMAGE_HEIGHT - 1);

            Ray ray;
            ray.m_orig.X(-1.0f + 2.0f * xPer);
            ray.m_orig.Y( 1.0f - 2.0f * yPer);
            ray.m_orig.Z(0.0f);

            ray.m_dir = Vector3::UNIT_Z * -1.0f;

            if (s0.Intersect(ray))
            {
                unsigned int pixel = j * IMAGE_WIDTH + i;
                image.SetChannelAt(pixel, 0, 255 * xPer);
                image.SetChannelAt(pixel, 1, 255 * yPer);
                image.SetChannelAt(pixel, 2, 255);
            }
        }
    }

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        std::cerr << "Error in main(): error in glfwInit() call.\n";
        return 1;
    }
    window = glfwCreateWindow(IMAGE_WIDTH, IMAGE_HEIGHT, "AtaomTraceCL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window))
    {
        glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image.GetRawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    image.SavePPM("image.ppm");

    glfwTerminate();

    std::cout << "All done\n";

    return 0;
}


void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}