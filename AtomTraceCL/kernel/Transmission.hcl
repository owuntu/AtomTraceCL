#ifndef ATOMTRACECL_TRANSMISSION_HCL_
#define ATOMTRACECL_TRANSMISSION_HCL_

typedef struct Transmission
{
    float3 color;
    float etat;
}Transmission;

float FresnelDie(const float cosi, const float cost, const float etai, const float etat)
{
    float Rparl = (etat * cosi - etai * cost) /
                  (etat * cosi + etai * cost);
    float Rperp = (etai * cosi - etat * cost) /
                  (etai * cosi + etat * cost);

    return (Rparl * Rparl + Rperp * Rperp) * 0.5f;
}

float3 TransmissionSampleF(const float u1, const Transmission* pTrm, const float etai, const float3* pN, const float3* pWo, float3* pWi, float* pPdf)
{
    // Figure out whether it is entering or exiting the material
    float3 N = normalize(*pN);
    float cosTi = dot(*pWo, N);
    
    float3 wR = N * 2.f * cosTi - *pWo;// reflection direction

    bool isEntering = cosTi > 0.f;
    float ei = etai, et = pTrm->etat;
    if (!isEntering)
    {
        // swap ei and et
        ei = pTrm->etat;
        et = etai;
        cosTi = -cosTi;
        N = -N;
    }
    float eta = ei / et;

    float sinTi = sqrt(1.f - cosTi * cosTi);
    float sinTt = eta * sinTi;

    float T, R;
    if (sinTt >= 1.f)// Total internal reflections
    {
        T = 0.f;
        R = 1.f;
        *pWi = wR;
    }
    else
    {
        float cosTt = sqrt(1.f - sinTt*sinTt);
        R = FresnelDie(fabs(cosTi), fabs(cosTt), ei, et);
        T = 1.f - R;

        if (u1 < T)
        {
            // Transmission direction
            *pWi = normalize(N * cosTt + (*pWo - N * cosTi) * eta) * -1.f;
        }
        else
        {
            // Reflection direction
            *pWi = wR;
        }
    }

    *pPdf = 1.f;
    return pTrm->color;
}

#endif // ATOMTRACECL_TRANSMISSION_HCL_
