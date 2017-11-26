/// \file: Vector3.h
/// \Description: Vector3 class wrap on top of openCL

#ifndef __ATOMMATH_AM_VECTOR3_H_
#define __ATOMMATH_AM_VECTOR3_H_

#include <cstddef>
//#include <cl/cl_platform.h>

namespace AtomMathCL
{
	class Vector3
	{
    private:
        float m_x;
        float m_y;
        float m_z;
        float padding; // alignment for OpenCL float3, since float3 and float4 are identical.
    public:
        static const Vector3 ZERO;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;

        inline Vector3(void);
        inline Vector3(float in_x, float in_y, float in_z);
        inline Vector3(float s);
        inline Vector3(const Vector3& v);
        inline ~Vector3(void);

        inline const float Length() const;
        inline const float LengthSq() const; // Faster since no square root computation
        inline const Vector3 GetNormalize() const;
        inline void Normalized();

        inline const Vector3& operator=(const Vector3&); //Assignment operator
        inline const Vector3& operator+=(const Vector3&);
        inline const Vector3& operator+=(const float);
        inline const Vector3& operator-=(const Vector3&);
        inline const Vector3& operator-=(const float);
        inline const Vector3& operator*=(const float);
        inline const Vector3& operator/=(const float);

        inline bool operator==(const Vector3&) const;

        inline void DebugPrint() const;

        const Vector3 operator+(const Vector3&) const;
        const Vector3 operator+(const float) const;
        const Vector3 operator-(const Vector3&) const;
        const Vector3 operator-(const float) const;
        const Vector3 operator-();
        const float operator*(const Vector3 &) const; //dot product
        const Vector3 operator*(const float) const; //scale
        const Vector3 operator/(const float) const; //scale

        float Dot(const Vector3 &) const;
        const Vector3 Cross(const Vector3 &) const;

        // Accessors
        inline const float X() const;
        inline const float Y() const;
        inline const float Z() const;
        inline const float& operator[](const std::size_t& index) const;
        inline       float& operator[](const std::size_t& index);
        inline void X(const float in_x);
        inline void Y(const float in_y);
        inline void Z(const float in_z);

        inline void Set(const float i_x, const float i_y, const float i_z);

	}; // AtomMathCL
}

#include "Vector3.inl"

#endif // __ATOMMATH_AM_VECTOR3_H_