#ifndef ATOMTRACECL_CAMERA_H_
#define ATOMTRACECL_CAMERA_H_

#include "Vector3.h"

namespace AtomTraceCL
{
    class Camera
    {
    public:
        AtomMathCL::Vector3 m_position;
        float padding0; // alignment for OpenCL float3, since float3 and float4 are idendical.
        AtomMathCL::Vector3 m_upLeftPix;
        float padding1;
        AtomMathCL::Vector3 m_dx;
        float padding2;
        AtomMathCL::Vector3 m_dy;
        float padding3;

    public:
        // Using default constructor and destructor should be fine.

        // up vector should be normalized
        // fov is horizontal fov
        bool Init(const AtomMathCL::Vector3& pos,
                  const AtomMathCL::Vector3& target,
                  float fov, int imgWidth, int imgHeight,
                  float focalDist = 1.0f, const AtomMathCL::Vector3& up = AtomMathCL::Vector3::UNIT_Y);
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_CAMERA_H_
