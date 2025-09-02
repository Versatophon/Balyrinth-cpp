#pragma once

#include "LabyrinthApi.h"

#include <cstdint>
//#include <unordered_map>

//#define MAX_NEIGHBOR_COUNT 4

//#define USE_MAP 0//380MB vs 336MB

#define INVALID_NODE_INDEX UINT32_MAX

class LABYRINTH_API Node
{
public:
    Node();
    //Node(const Node& pOtherNode);

    //void SetIndex(uint32_t pIndex);

    //uint32_t Index() const;

    uint32_t NeighborCount() const;

    void AddNeighborIndex(uint32_t pNeighborIndex);
    void RemoveNeighborIndex(uint32_t pNeighborIndex);

    void RemoveAllNeighbors();

    uint32_t GetNeighborIndex(uint32_t pLocalIndex) const;

private:
    //uint32_t mIndex = UINT32_MAX;//Index in global container

#if USE_MAP
    std::unordered_map<size_t, size_t> mNeighborIndices;
#else
    uint32_t* mNeighborIndices = nullptr;// [MAX_NEIGHBOR_COUNT] = { UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX};
    size_t mNeighborCount = 0;
        //UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX 
#endif
};
