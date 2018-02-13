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

bool BVHisLeafNode(__constant const BVHNode* pNode)
{
    return ((pNode->data)&_CY_BVH_LEAF_BIT_MASK) > 0;
}

uint BVHgetNodeElementCount(__constant const BVHNode* pNode)
{
    return (((pNode->data) >> _CY_BVH_ELEMENT_OFFSET_BITS) & _CY_BVH_ELEMENT_COUNT_MASK);
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
