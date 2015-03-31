#include <cstdlib>
#include "GL\glut.h"

#include "Ray.h"
#include "SimpleImage.h"
#include "Sphere.h"

using namespace AtomMathCL;

void GlutDisplay();

static SimpleImage* gs_pImage;

int main()
{
    Sphere s0(1.0f, Vector3::ZERO);

    const unsigned int IMAGE_WIDTH = 128;
    const unsigned int IMAGE_HEIGHT = 128;

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

    image.SavePPM("image.ppm");
#if 0
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (glutGet(GLUT_SCREEN_WIDTH) > 0 && glutGet(GLUT_SCREEN_HEIGHT) > 0){
        glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - IMAGE_WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - IMAGE_HEIGHT) / 2);
    }
    else
        glutInitWindowPosition(50, 50);
    glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);

    glutCreateWindow("AtomTraceCL");

    glutDisplayFunc(GlutDisplay);

    glClearColor(0, 0, 0, 0);

    //glPointSize(3.0);
    //glEnable(GL_CULL_FACE);

    float zero[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);

    glEnable(GL_NORMALIZE);

    glLineWidth(2);


    glutMainLoop();

#endif
    return 0;
}

void GlutDisplay()
{
    glDrawPixels(gs_pImage->Width(), gs_pImage->Height(), GL_RGB, GL_UNSIGNED_BYTE, gs_pImage->GetRawData());

    glutPostRedisplay();
}