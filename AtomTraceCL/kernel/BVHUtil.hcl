#ifndef ATOMTRACE_CL_BVH_UTIL_HCL_
#define ATOMTRACE_CL_BVH_UTIL_HCL_

#include "..\cyBVHMacro.h"

// Axis aligned box
typedef struct
{
    float b[6];
}Box;

typedef struct
{
    Box box;
    uint data;
}BVHNode;

bool IntersectBox(const Ray* pRAY, const Box* pBox, float t)
{
    float tmin = -FLT_MAX;
    float tmax = FLT_MAX;

    float dir[3] = { pRAY->m_dir.x, pRAY->m_dir.y, pRAY->m_dir.z };
    float orig[3] = { pRAY->m_orig.x, pRAY->m_orig.y, pRAY->m_orig.z };

    for (int i = 0; i < 3; ++i)
    {
        // Check parallel
        if (dir[i] != 0.f)
        {
            float t1 = (pBox->b[i] - orig[i]) / dir[i];
            float t2 = (pBox->b[i + 3] - orig[i]) / dir[i];
            tmin = max(tmin, min(t1, t2));
            tmax = min(tmax, max(t1, t2));
        }
    }

    tmin = max(tmin, 0.f);

    if (tmin > tmax || tmin > t)
        return false;
    return true;
}

bool IsBoxEmpty(const Box* pBox)
{
    return pBox->b[0] > pBox->b[3]
        || pBox->b[1] > pBox->b[4]
        || pBox->b[2] > pBox->b[5];
}

bool BVHisLeafNode(__constant const BVHNode* pNode)
{
    return ((pNode->data)&_CY_BVH_LEAF_BIT_MASK) > 0;
}

uint BVHgetNodeElementCount(__constant const BVHNode* pNode)
{
    return (((pNode->data) >> _CY_BVH_ELEMENT_OFFSET_BITS) & _CY_BVH_ELEMENT_COUNT_MASK) + 1;
}

uint BVHgetNodeElementsOffset(__constant const BVHNode* pNode)
{
    return ((pNode->data)&_CY_BVH_ELEMENT_OFFSET_MASK);
}

uint BVHgetNodeFirstChildIndex(__constant const BVHNode* pNode)
{
    return ((pNode->data)&_CY_BVH_CHILD_INDEX_MASK);
}

void BVHgetNodeChildrenIndex(__constant const BVHNode* pNode, uint* pChild1, uint* pChild2)
{
    uint child1 = BVHgetNodeFirstChildIndex(pNode);
    *pChild1 = child1;
    *pChild2 = child1 + 1;
}

#endif // ATOMTRACE_CL_BVH_UTIL_HCL_
