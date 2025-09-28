#pragma once

#include "LabyrinthApi.h"

#include <cstdint>

#define INVALID_NODE_INDEX UINT32_MAX

class LABYRINTH_API Node
{
public:
    Node();

    uint32_t NeighborCount() const;

    void AddNeighborIndex(uint32_t pNeighborIndex);
    void RemoveNeighborIndex(uint32_t pNeighborIndex);

    void RemoveAllNeighbors();

    uint32_t GetNeighborIndex(uint32_t pLocalIndex) const;

private:
    uint32_t* mNeighborIndices = nullptr;
    size_t mNeighborCount = 0;
};
