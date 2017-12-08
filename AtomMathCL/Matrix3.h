#ifndef ATOMMATHCL_MATRIX3_H_
#define ATOMMATHCL_MATRIX3_H_

namespace AtomMathCL
{
    class Vector3;

    class Matrix3
    {
    public:
        Matrix3();

        const Matrix3 operator*(const Matrix3& rhs) const;
        const Matrix3 operator+(const Matrix3& rhs) const;
        const Matrix3 operator-(const Matrix3& rhs) const;

        const Vector3 operator*(const Vector3& vec) const;

        const Matrix3& operator/=(const float);

        float& operator[](int index);
        const float operator[](int index) const;

        void Transpose();
        Matrix3 GetTranspose() const;

        // Referencing cyCodeBase https://github.com/cemyuksel/cyCodeBase/blob/master/cyMatrix.h?ts=4
        void Invert(); // Invert this matrix
        void GetInverse(Matrix3 &inverse) const; // Get the inverse of this matrix
        
        Matrix3 GetInverse() const;

        void Zero();

        void SetIdentity();

        void SetScale(const Vector3& scale);
        void SetScale(float scale);

        void Matrix3::SetRotation(const Vector3& axis, float cosTheta, float sinTheta);
        void SetRotation(float theta, const Vector3& axis);
        void SetRotationX(float theta);
        void SetRotationY(float theta);
        void SetRotationZ(float theta);

    private:
        float m_data[9];
        float m_padding[3]; // For OpenCL alignment
    };
} // namespace AtomMathCL

#endif
