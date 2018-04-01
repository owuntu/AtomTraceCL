#ifndef ATOMTRACECL_UTILITIES_HCL_
#define ATOMTRACECL_UTILITIES_HCL_

#include "ConstantDef.hcl"

// This function is copy from: http://davibu.interfree.it/opencl/smallptgpu2/smallptGPU2.html
float GetRandom01(uint *pSeed0, uint *pSeed1) {
    *pSeed0 = 36969 * ((*pSeed0) & 65535) + ((*pSeed0) >> 16);
    *pSeed1 = 18000 * ((*pSeed1) & 65535) + ((*pSeed1) >> 16);

    unsigned int ires = ((*pSeed0) << 16) + (*pSeed1);

    // Convert to float
    union {
        float f;
        unsigned int ui;
    } res;
    res.ui = (ires & 0x007fffff) | 0x40000000;

    return (res.f - 2.f) / 2.f;
}

void GetAxisBaseGivenDir(const float3* pN, float3* pAx, float3* pAy)
{
    float3 ax;
    if (1.f - fabs(dot(UNIT_X, *pN)) < 0.01f)
    {
        ax = cross(*pN, UNIT_Y);
    }
    else
    {
        ax = cross(*pN, UNIT_X);
    }

    float3 ay = cross(ax, *pN);

    *pAx = normalize(ax);
    *pAy = normalize(ay);
}

float Halton(int index, int base)
{
    float r = 0;
    float f = 1.0f / (float)base;
    for (int i = index; i > 0; i /= base)
    {
        r += f * (i%base);
        f /= (float)base;
    }
    return r;
}

void UniformSampleSphere(float u1, float u2, float3* p)
{
    float theta = HALF_PI_F*(1.f - u1 * 2.f); // [-PI/2, PI/2]
    p->z = cos(theta);
    float r = sin(theta);
    float phi = TWO_PI_F * u2;
    p->x = r * cos(phi);
    p->y = r * sin(phi);
}

void SampleHemiSphere(float u1, float u2, const float3* pN, float3* p)
{
    float3 ax;
    if (1.f - fabs(dot(UNIT_X, *pN)) < 0.01f)
    {
        ax = cross(*pN, UNIT_Y);
    }
    else
    {
        ax = cross(*pN, UNIT_X);
    }

    float3 ay = cross(ax, *pN);

    ax = normalize(ax);
    ay = normalize(ay);

    // Importance sampling
    float theta = 0.5f * acos(1.f - 2.f * u1);
    //float theta = HALF_PI_F * u1;
    float phi = TWO_PI_F * u2;
    float r = sin(theta);
    *p = *pN * cos(theta) + ax * r * cos(phi) + ay * r * sin(phi);
}

#endif // ATOMTRACECL_UTILITIES_HCL_
