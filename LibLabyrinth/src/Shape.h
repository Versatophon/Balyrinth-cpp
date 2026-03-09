#pragma once

#include "LabyrinthApi.h"

#include <cstdint>//uint32_t
#include <cstddef>//size_t

#include <vector>

#include <Vector3i.h>

#include "GridShape.h"

class Topology;

struct Vector3f;

struct Parameter
{
	char Name[8] = { 0 };
	union
	{
		void* ValueAsPointer = nullptr;
		bool ValueAsBoolean;
		char ValueAsChar;
		int32_t ValueAsInteger32;
		int64_t ValueAsInteger64;
		float ValueAsFloat;
		double ValueAsDouble;
	};
};

struct Parameters
{
	size_t Count = 0;
	Parameter* Params = nullptr;
};

enum class RoomType
{
	Undefined,
	Triangular,//Has 6 connections 2 per edge, to get a kind of direction 
	Square,
	Hexagonal,
	Octogonal,
	Cubic,
};

class RoomNeighborhood;

//TODO: Constify
class LABYRINTH_API Shape
{
public:
	Shape();
	virtual ~Shape();
	Topology* GetTopology();
	RoomNeighborhood* GetRoomNeighborhood();
	Vector3i GetSize();

	virtual Vector3f GetNodeNormalizedPosition(uint32_t pIndex) = 0;
	virtual Vector3f GetSpaceSize() = 0;
	virtual Vector3f GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) = 0;

	virtual GridShape GetGridShape() = 0;

protected:
	Topology* mTopology = nullptr;
	RoomNeighborhood* mRoomNeighborhood = nullptr;
	RoomType mRoomType = RoomType::Undefined;
	Vector3i mSize;
};

typedef Shape* (ShapeGenerator)(Parameters& pParameters);

LABYRINTH_API Shape* GenerateSquaresOnRectShape(Parameters& pParameters);
LABYRINTH_API Shape* GenerateSquaresOnToreShape(Parameters& pParameters);
LABYRINTH_API Shape* GenerateHexagonsOnRectShape(Parameters& pParameters);
