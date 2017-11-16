#ifndef ATOMTRACECL_CAMERA_H_
#define ATOMTRACECL_CAMERA_H_

#include "Vector3.h"

namespace AtomTraceCL
{
    class Camera
    {
    public:
        AtomMathCL::Vector3 m_position;
        AtomMathCL::Vector3 m_upLeftPix;
        AtomMathCL::Vector3 m_dx;
        AtomMathCL::Vector3 m_dy;

    public:
        // lookat and up vector should be normalized
        // fov is horizontal fov
        bool Init(const AtomMathCL::Vector3& pos,
                  const AtomMathCL::Vector3& lookat,
                  float fov, int imgWidth, int imgHeight,
                  float focalDist = 1.0f, const AtomMathCL::Vector3& up = AtomMathCL::Vector3::UNIT_Z);
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_CAMERA_H_
