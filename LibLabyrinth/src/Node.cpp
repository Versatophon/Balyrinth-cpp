#include "Node.h"

#include <cstring>

//Node::Node(const Node& pOtherNode):
//    mIndex(pOtherNode.mIndex)//,
//    //mNeighborIndices(pOtherNode.mNeighborIndices.begin(), pOtherNode.mNeighborIndices.end())
//{
//}

//void Node::SetIndex(uint32_t pIndex)
//{
//    mIndex = pIndex;
//}
//
//uint32_t Node::Index() const
//{
//    return mIndex;
//}

Node::Node()
{
}

uint32_t Node::NeighborCount() const
{
#if USE_MAP
    return mNeighborIndices.size();
#else
    return mNeighborCount;
#endif
}

void Node::AddNeighborIndex(uint32_t pNeighborIndex)
{
#if USE_MAP
    mNeighborIndices[pDirection] = pNeighborIndex;
#else
    uint32_t* lTempContainer = new uint32_t[++mNeighborCount];

    memcpy(lTempContainer, mNeighborIndices, (mNeighborCount - 1) * sizeof(uint32_t));

    delete[] mNeighborIndices;

    mNeighborIndices = lTempContainer;

    mNeighborIndices[mNeighborCount - 1] = pNeighborIndex;

    //if (mNeighborIndices[pDirection] == UINT32_MAX)
    //{
    //    mNeighborIndices[pDirection] = pNeighborIndex;
    //}
#endif
}

void Node::RemoveNeighborIndex(uint32_t pNeighborIndex)
{
#if USE_MAP
    mNeighborIndices.erase(pDirection);
#else
    size_t lFoundIndex = 0;
    for (; lFoundIndex < mNeighborCount; ++lFoundIndex)
    {
        if (mNeighborIndices[lFoundIndex] == pNeighborIndex)
        {//Ok, we found the index
            for (; lFoundIndex < mNeighborCount - 1; ++lFoundIndex)
            {//let swap every element to push last element to the end
                uint32_t lTempIndex = mNeighborIndices[lFoundIndex];
                mNeighborIndices[lFoundIndex] = mNeighborIndices[lFoundIndex + 1];
                mNeighborIndices[lFoundIndex + 1] = lTempIndex;
            }

            //reduce container size (can be nulled if size equals zero)
            uint32_t* lNewContainer = (--mNeighborCount) > 0 ? new uint32_t[mNeighborCount] : nullptr;
            memcpy(lNewContainer, mNeighborIndices, mNeighborCount * sizeof(uint32_t));
            mNeighborIndices = lNewContainer;
        }
    }

    //if (mNeighborIndices[pDirection] != UINT32_MAX)
    //{
    //    mNeighborIndices[pDirection] = UINT32_MAX;
    //    --mNeighborCount;
    //}
#endif
}

void Node::RemoveAllNeighbors()
{
#if USE_MAP
    mNeighborIndices = {};
#else
    mNeighborCount = 0;
    delete[] mNeighborIndices;
    mNeighborIndices = nullptr;

    //for (uint32_t i = 0; i < MAX_NEIGHBOR_COUNT; ++i)
    //{
    //    mNeighborIndices[i] = UINT32_MAX;
    //}
#endif
}

uint32_t Node::GetNeighborIndex(uint32_t pLocalIndex) const
{
#if USE_MAP
    std::unordered_map<size_t, size_t>::const_iterator lIt = mNeighborIndices.find(pDirection);

    if (lIt != mNeighborIndices.end())
    {
        return lIt->second;
    }

    return UINT32_MAX;
#else
    return mNeighborIndices[pLocalIndex];
#endif
}
