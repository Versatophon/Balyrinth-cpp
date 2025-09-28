#include "Node.h"

#include <cstring>

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
    uint32_t* lTempContainer = new uint32_t[++mNeighborCount];

    memcpy(lTempContainer, mNeighborIndices, (mNeighborCount - 1) * sizeof(uint32_t));

    delete[] mNeighborIndices;

    mNeighborIndices = lTempContainer;

    mNeighborIndices[mNeighborCount - 1] = pNeighborIndex;
}

void Node::RemoveNeighborIndex(uint32_t pNeighborIndex)
{
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
}

void Node::RemoveAllNeighbors()
{
    mNeighborCount = 0;
    delete[] mNeighborIndices;
    mNeighborIndices = nullptr;
}

uint32_t Node::GetNeighborIndex(uint32_t pLocalIndex) const
{
    return mNeighborIndices[pLocalIndex];
}
