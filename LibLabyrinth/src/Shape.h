#pragma once

#include "LabyrinthApi.h"

#include <cstdint>//uint32_t
#include <cstddef>//size_t

#include <variant>

#include "Vec3.h"

class Topology;

//enum class ShapeEnum
//{
//	TriangularRoomsOnRectangularSpace,
//	SquareRoomsOnRectangularSpace,
//	HexagonalRoomsOnRectangularSpace,
//	HexagonalRoomsOnHexagonalSpace,//Uses only width to define size
//	OctogonalRoomsOnRectangularSpace,
//	SquareRoomsOnToreSpace,
//};

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

class LABYRINTH_API Shape
{
public:
	Shape();
	virtual ~Shape();

	virtual Topology* GetTopology() = 0;
	virtual Vec3 GetNodeNormalizedPosition(uint32_t pIndex) = 0;
	virtual Vec3 GetSpaceSize() = 0;
	virtual Vec3 GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) = 0;

protected:
	Topology* mTopology = nullptr;
	RoomType mRoomType = RoomType::Undefined;
};

typedef Shape* (ShapeGenerator)(Parameters& pParameters);

LABYRINTH_API Shape* GenerateSquaresOnRectShape(Parameters& pParameters);
LABYRINTH_API Shape* GenerateSquaresOnToreShape(Parameters& pParameters);
