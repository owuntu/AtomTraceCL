#ifndef ATOMTRACECL_METAL_HCL_
#define ATOMTRACECL_METAL_HCL_

#include "ConstantDef.hcl"

typedef struct
{
    float3 eta;
    float3 kk;
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

float BlinnMD_D(const float3* pWh, const float3* pN)
{
    const float EXP = 1000.0f;
    return ((EXP + 2.0f) * pow(dot(*pWh, *pN), EXP) * INV_PI_F * 0.5f);
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
    return /* m_R * */  BlinnMD_D(&wh, pN) * MicrofacetG(pWo, pN, pWi) * F /
        (4.f * cosThetaI * cosThetaO);
}

#endif // ATOMTRACECL_METAL_HCL_
