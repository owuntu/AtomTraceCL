#ifndef ATOMTRACECL_TRIANGLES_HCL_
#define ATOMTRACECL_TRIANGLES_HCL_

#include "InfoDef.hcl"

const float3 BaryCentricHelp(const float3 v01, const float3 v02, const float3 v0p, const float3 faceN)
{
    float a0 = dot(cross(v01, v02), faceN);// * 0.5f;
    float a1 = dot(cross(v01, v0p), faceN);// * 0.5f;
    float a2 = dot(cross(v0p, v02), faceN);// * 0.5f;

    float3 bc;
    bc[0] = a1 / a0;
    bc[1] = a2 / a0;
    bc[2] = 1 - bc[0] - bc[1];
    return bc;
}

bool IntersectTriangle(const Ray* pRAY, __constant float3* pVertices, __constant float3* pNormal,
    __constant uint3* pFaces, __constant uint3* pFaceN, uint faceID, HitInfoGeo* pInfoGeo, float* pt)
{
    uint3 face = *(pFaces + faceID);
    float3 v0 = *(pVertices + face.x);
    // TODO: Intersect triangle.

    float3 v01;
    float3 v02;
    {
        float3 v1 = *(pVertices + face.y);
        float3 v2 = *(pVertices + face.z);
        v01 = v1 - v0;
        v02 = v2 - v0;
    }

    float3 vN = cross(v01, v02);

    float3 vn0 = *(pNormal + (*(pFaceN + faceID)).x);
    if (dot(vn0, vN) < 0.f)
        vN = -vN;

    vN = normalize(vN);

    float nDd = dot(vN, pRAY->dir);
    float t = dot(vN, v0 - pRAY->orig) / nDd;
    if (t < 0.f || t > *pt)
        return false;

    float3 hp = pRAY->orig + pRAY->dir * t;
    float3 v0p = hp - v0;
    float3 bc = BaryCentricHelp(v01, v02, v0p, vN);
    if (bc.x < 0.f || bc.y < 0.f || bc.z < 0.f)
        return false;

    *pt = t;
    float3 vn1 = *(pNormal + (*(pFaceN + faceID)).y);
    float3 vn2 = *(pNormal + (*(pFaceN + faceID)).z);
    pInfoGeo->hitP = hp;
    pInfoGeo->hitN = vn0 * bc[2] + vn1 * bc[1] + vn2 * bc[0];
    return true;
}

#endif // ATOMTRACECL_TRIANGLES_HCL_
