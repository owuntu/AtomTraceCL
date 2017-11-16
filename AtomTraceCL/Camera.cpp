#include "Camera.h"
#include "Vector3.h"
#include "MathConstant.h"

using namespace AtomMathCL;

namespace AtomTraceCL
{
    bool Camera::Init(const Vector3& pos, const Vector3& lookat, float fov, int imgWidth, int imgHeight,
                      float focalDist /* = 1.0f */, const Vector3& up /* = Vector3::UNIT_Z */)
    {
        m_position = pos;
        Vector3 upV = up;
        Vector3 rightV = Vector3::UNIT_X;
        if (fabs(lookat.Dot(upV) - 1.0f) < 0.01f)
        {
            // Use UNITX to help generate up vector
            if (fabs(lookat.Dot(Vector3::UNIT_X) - 1.0f) > 0.01f)
            {
                upV = lookat.Cross(Vector3::UNIT_X);
            }
            else
            {
                // Use UNIT_Y to help generate up vector
                upV = lookat.Cross(Vector3::UNIT_Y);
            }
            rightV = lookat.Cross(upV);
            upV.Normalized();
            rightV.Normalized();
        }
        else
        {
            rightV = lookat.Cross(upV);
            upV = lookat.Cross(rightV);
            rightV.Normalized();
            upV.Normalized();
        }

        float invAsp = static_cast<float>(imgHeight) / static_cast<float>(imgWidth);
        float hu = tan(fov * 0.5f * M_PI_F / 180.0f) * focalDist;
        float hv = hu * invAsp;

        float dx = hu * 2.0f / static_cast<float>(imgWidth);
        //float dy = hv * 2.0f / static_cast<float>(imgHeight);
        
        m_dx = rightV * dx;
        m_dy = upV * dx;

        m_upLeftPix = pos + lookat - m_dx * hu + m_dy * hv;

        return true;
    }

} // namespace AtomTraceCL