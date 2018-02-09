#ifndef _CY_BVH_MACRO_H_INCLUDED_
#define _CY_BVH_MACRO_H_INCLUDED_

#ifndef CY_BVH_ELEMENT_COUNT_BITS
#define CY_BVH_ELEMENT_COUNT_BITS	3	//!< Determines the number of bits needed to represent the maximum number of elements in a node (8)
#endif

#ifndef CY_BVH_MAX_ELEMENT_COUNT
#define CY_BVH_MAX_ELEMENT_COUNT	(1<<CY_BVH_ELEMENT_COUNT_BITS)	//!< Determines the maximum number of elements in a node (8)
#endif

#define _CY_BVH_NODE_DATA_BITS		(sizeof(unsigned int)*8)
#define _CY_BVH_ELEMENT_COUNT_MASK	((1<<CY_BVH_ELEMENT_COUNT_BITS)-1)
#define _CY_BVH_LEAF_BIT_MASK		((unsigned int)1<<(_CY_BVH_NODE_DATA_BITS-1))
#define _CY_BVH_CHILD_INDEX_BITS	(_CY_BVH_NODE_DATA_BITS-1)
#define _CY_BVH_CHILD_INDEX_MASK	(_CY_BVH_LEAF_BIT_MASK-1)
#define _CY_BVH_ELEMENT_OFFSET_BITS	(_CY_BVH_NODE_DATA_BITS-1-CY_BVH_ELEMENT_COUNT_BITS)
#define _CY_BVH_ELEMENT_OFFSET_MASK	((1<<_CY_BVH_ELEMENT_OFFSET_BITS)-1)

#endif //_CY_BVH_MACRO_H_INCLUDED_
