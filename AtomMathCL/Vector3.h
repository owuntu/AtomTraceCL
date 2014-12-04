/// \file: Vector3.h
/// \Description: Vector3 class wrap on top of openCL

#ifndef __ATOMMATH_AM_VECTOR3_H_
#define __ATOMMATH_AM_VECTOR3_H_

#include <cstddef>
#include <cl/cl_platform.h>

namespace AtomMathCL
{
	class Vector3
	{
    private:
        cl_float m_x;
        cl_float m_y;
        cl_float m_z;
    public:
        static const Vector3 ZERO;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;

        inline Vector3(void);
        inline Vector3(cl_float in_x, cl_float in_y, cl_float in_z);
        inline Vector3(cl_float s);
        inline Vector3(const Vector3& v);
        inline ~Vector3(void);

        inline const cl_float Length() const;
        inline const cl_float LengthSq() const; // Faster since no square root computation
        inline const Vector3 GetNormalize() const;
        inline void Normalized();

        inline const Vector3& operator=(const Vector3&); //Assignment operator
        inline const Vector3& operator+=(const Vector3&);
        inline const Vector3& operator+=(const cl_float);
        inline const Vector3& operator-=(const Vector3&);
        inline const Vector3& operator-=(const cl_float);
        inline const Vector3& operator*=(const cl_float);
        inline const Vector3& operator/=(const cl_float);

        inline bool operator==(const Vector3&) const;

        inline void DebugPrint() const;

        const Vector3 operator+(const Vector3&) const;
        const Vector3 operator+(const cl_float) const;
        const Vector3 operator-(const Vector3&) const;
        const Vector3 operator-(const cl_float) const;
        const Vector3 operator-();
        const cl_float operator*(const Vector3 &) const; //dot product
        const Vector3 operator*(const cl_float) const; //scale
        const Vector3 operator/(const cl_float) const; //scale

        float Dot(const Vector3 &) const;
        const Vector3 Cross(const Vector3 &) const;

        // Accessors
        inline const cl_float X() const;
        inline const cl_float Y() const;
        inline const cl_float Z() const;
        inline const cl_float& operator[](const std::size_t& index) const;
        inline       cl_float& operator[](const std::size_t& index);
        inline void X(const cl_float in_x);
        inline void Y(const cl_float in_y);
        inline void Z(const cl_float in_z);

        inline void Set(const cl_float i_x, const cl_float i_y, const cl_float i_z);

	};
}

#endif // __ATOMMATH_AM_VECTOR3_H_