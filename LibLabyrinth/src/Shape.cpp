#include "Shape.h"

#include "Topology.h"
#include "RoomNeighborhood.h"

#include <Vector2i.h>
#include <Vector3f.h>

#include <string>
#include <cstring>
#include <cmath>

Shape::Shape()
{
}

Shape::~Shape()
{
}

Topology* Shape::GetTopology()
{
	return mTopology;
}

RoomNeighborhood* Shape::GetRoomNeighborhood()
{
	return mRoomNeighborhood;
}

Vector3i Shape::GetSize()
{
	return mSize;
}
