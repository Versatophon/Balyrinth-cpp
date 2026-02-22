#pragma once

#include <cstdint>

class RoomNeighborhood
{
public:
	virtual uint32_t GetDirectionCount() const = 0;
	//can be undetermined for Triangle rooms, return 
	virtual uint32_t GetDirection(uint32_t pIndexFrom, uint32_t pIndexTo) const = 0;
	virtual uint32_t GetOppositeDirection(uint32_t pDirection) const = 0;

	virtual uint32_t GetNextNode(uint32_t pIndexFrom, uint32_t pDirection) const = 0;
};
