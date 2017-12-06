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

        float& operator[](int index);
        const float operator[](int index) const;

        void Transpose();
        Matrix3 GetTranspose() const;

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
    };
} // namespace AtomMathCL

#endif
