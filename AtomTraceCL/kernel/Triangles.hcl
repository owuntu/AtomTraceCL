#ifndef ATOMTRACECL_TRIANGLES_HCL_
#define ATOMTRACECL_TRIANGLES_HCL_

#include "BVHUtil.hcl"
#include "ConstantDef.hcl"
#include "InfoDef.hcl"
#include "..\TriMeshHeader.h"
#include "UintStack.hcl"

const float3 BaryCentricHelp(const float3 v01, const float3 v02, const float3 v0p, const float3 faceN)
{
    float a0 = dot(cross(v01, v02), faceN);
    float a1 = dot(cross(v01, v0p), faceN);
    float a2 = dot(cross(v0p, v02), faceN);

    float3 bc;
    bc.x = a1 / a0;
    bc.y = a2 / a0;
    bc.z = 1 - bc.x - bc.y;
    return bc;
}

bool IntersectTriangle(const Ray* pRAY, __constant float3* pVertices, __constant float3* pNormal,
    __constant uint3* pFaces, __constant uint3* pFaceN, uint faceID, HitInfoGeo* pInfoGeo, float* pt)
{
    uint3 face = *(pFaces + faceID);
    float3 v0 = *(pVertices + face.x);

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

    float nDd = dot(vN, pRAY->m_dir);
    float t = dot(vN, v0 - pRAY->m_orig) / nDd;
    if (t < 0.f || t > *pt)
        return false;

    float3 hp = pRAY->m_orig + pRAY->m_dir * t;
    float3 v0p = hp - v0;
    float3 bc = BaryCentricHelp(v01, v02, v0p, vN);
    if (bc.x < 0.f || bc.y < 0.f || bc.z < 0.f)
        return false;

    *pt = t;
    float3 vn1 = *(pNormal + (*(pFaceN + faceID)).y);
    float3 vn2 = *(pNormal + (*(pFaceN + faceID)).z);
    pInfoGeo->hitP = hp;
    pInfoGeo->hitN = vn0 * bc.z + vn1 * bc.y + vn2 * bc.x;
    return true;
}

bool IntersectTriMesh(const Ray* pRAY, __constant char* pGeo, HitInfoGeo* pInfogeo, float* pt)
{
    __constant char* pCurr = pGeo;
    // Copy overhead information
    TriMeshHeader header = *(__constant TriMeshHeader*)pCurr;

    __constant float3* pVertices = (__constant float3*)(pCurr + header.vertices.index);
    __constant float3* pNormal = (__constant float3*)(pCurr + header.vns.index);

    bool bHit = false;
    __constant uint3* pFaces = (__constant uint3*)(pCurr + header.faces.index);
    __constant uint3* pFaceN = (__constant uint3*)(pCurr + header.fns.index);
#if 0 // Brute force iterate all triangles
    for (uint i = 0; i < header.faces.size; ++i)
    {
        bHit |= IntersectTriangle(pRAY, pVertices, pNormal, pFaces, pFaceN, i, pInfogeo, pt);
    }
    return bHit;
#else // Using BVH
    // BVH intersection
    __constant const BVHNode* const pROOT = (__constant BVHNode*)(pCurr + header.nodes.index);
    __constant uint* pElementList = (__constant uint*)(pCurr + header.elements.index);
    __constant const BVHNode* pNode = pROOT;
    uint nodeID = 1; // cyBVH root nodeID start as 1
    Box box;
    UintStack stack;
    UintStackInit(&stack);

    UintStackPush(&stack, nodeID);

    while (!UintStackIsEmpty(&stack))
    {
        nodeID = UintStackTop(&stack);
        UintStackPop(&stack);
        pNode = pROOT + nodeID;
        box = pNode->box;
        // intersect box
        if (IsBoxEmpty(&box))
        {
            return false;
        }

        if (IntersectBox(pRAY, &box, *pt))
        {
            if (BVHisLeafNode(pNode))
            {
                // Intersect triangle.
                // Get all the triangle face ID
                uint offset = BVHgetNodeElementsOffset(pNode);
                uint numEle = BVHgetNodeElementCount(pNode);
                __constant uint* pElements = pElementList + offset;

                // Intersect them
                for (uint i = 0; i < numEle; ++i)
                {
                    uint faceID = pElements[i];
                    bHit |= IntersectTriangle(pRAY, pVertices, pNormal, pFaces, pFaceN, faceID, pInfogeo, pt);
                }
            }
            else
            {
                // Internal node, continue to intersect its children
                uint child1, child2;
                BVHgetNodeChildrenIndex(pNode, &child1, &child2);
                UintStackPush(&stack, child2);
                UintStackPush(&stack, child1);
            }
        }
    }

    return bHit;
#endif // Using BVH
}

#endif // ATOMTRACECL_TRIANGLES_HCL_
