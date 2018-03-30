#ifndef ATOMTRACECL_METAL_HCL_
#define ATOMTRACECL_METAL_HCL_

#include "ConstantDef.hcl"
#include "Utilities.hcl"

typedef struct
{
    float3 eta;
    float3 kk;
    float exp;
}Metal;

// Return Color
float3 FresnelCond(const float cosi, const float3* pEta, const float3* pK)
{
    float3 tmp_f = *pEta * *pEta + *pK * *pK;
    float cosi2 = cosi * cosi;
    float3 tmp = tmp_f * cosi2;
    float3 r = 2.0f * *pEta * cosi;

    float3 Rparl2 = (tmp - r + 1) /
        (tmp + r + 1);
    float3 Rperp2 = (tmp_f - r + cosi2) /
        (tmp_f + r + cosi2);

    return (Rperp2 + Rparl2) * 0.5f;
}

float BlinnMD_D(const float3* pWh, const float3* pN, const float EXP)
{
    return ((EXP + 2.0f) * pow(fabs(dot(*pWh, *pN)), EXP) * INV_PI_F * 0.5f);
}

void BlinnMD_SampleF(const float u1, const float u2, const float EXP, const float3* pN, const float3* pWo, float3* pWi, float* pPdf)
{
    float3 h;
    float costheta = pow(u1, 1.f / (EXP + 1.f));

    float sintheta = sqrt(max(0.f, 1.f - costheta*costheta));
    float phi = u2 * TWO_PI_F;
    float3 ax, ay;
    GetAxisBaseGivenDir(pN, &ax, &ay);
    h =   ax  * sintheta * cos(phi)
        + ay  * sintheta * sin(phi)
        + *pN * costheta;
    h = normalize(h);
    float vh = dot(*pWo, h);
    float blinn_pdf = ((EXP + 1.f) * pow(costheta, EXP))
        / (2.f * PI_F * 4.f * vh);
    if (vh <= 0.f)
        blinn_pdf = 0.f;
    if (vh < 0.f)
    {
        h = -h;
        vh = -vh;
    }
    *pWi = h * 2.0f * min(1.f, vh) - *pWo;
    *pPdf = blinn_pdf;
}

float MicrofacetG(const float3* pWo, const float3* pN, const float3* pWi)
{
    float3 wh = normalize(*pWo + *pWi);
    float NdotH = fabs(dot(wh, *pN));
    float NdotWo = fabs(dot(*pWo, *pN));
    float NdotWi = fabs(dot(*pWi,*pN));
    float HdotWo = fabs(dot(wh, *pWo));

    return fmin(1.f, fmin(2.0f * NdotH * NdotWo / HdotWo,
        2.0f * NdotH * NdotWi / HdotWo));
}

// Return Color
const float3 MetalF(const Metal* pMetal, const float3* pWo, const float3* pN, const float3* pWi)
{
    const float3 ZERO = (float3)(0.f);
    float cosThetaI = dot(*pWi, *pN);
    float cosThetaO = dot(*pWo, *pN);
    if (0.f == cosThetaO || 0.f == cosThetaI)
        return ZERO;

    float3 wh = *pWi + *pWo;
    if (wh.x == 0.f && wh.y == 0.f && wh.z == 0.f)
        return ZERO;

    wh = normalize(wh);
    float cosThetaH = dot(wh, *pWo);
    float3 F = FresnelCond(cosThetaH, &pMetal->eta, &pMetal->kk);
    return /* m_R * */  BlinnMD_D(&wh, pN, pMetal->exp) * MicrofacetG(pWo, pN, pWi) * F /
        (4.f * cosThetaI * cosThetaO);
}

#endif // ATOMTRACECL_METAL_HCL_
