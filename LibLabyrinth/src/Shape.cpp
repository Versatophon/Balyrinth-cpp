#include "Shape.h"

#include "Topology.h"
#include "RoomNeighborhood.h"

#include <Vector2i.h>
#include <Vector3f.h>

#include <string>
#include <cstring>
#include <cmath>

void AddAARect(std::vector<float>& pContainer, const Vector3f& pMin, const Vector3f& pMax)
{
	pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
	pContainer.push_back(pMax.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
	pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
	pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
	pContainer.push_back(pMin.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
	pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
}

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

class RectangularSpaceSquareRoomNeighborhood:public RoomNeighborhood
{
	enum class Direction :uint32_t
	{
		Right = 0,
		Up = 1,
		Left = 2,
		Down = 3,
		Undefined = UINT32_MAX
	};

public:
	RectangularSpaceSquareRoomNeighborhood(int32_t pWidth, int32_t pHeight):
		mWidth(pWidth),
		mHeight(pHeight)
	{
	}

	uint32_t GetDirectionCount() const override
	{
		return 4;
	}

	uint32_t GetDirection(uint32_t pIndexFrom, uint32_t pIndexTo) const override
	{
		int32_t lDelta = int32_t(pIndexTo) - int32_t(pIndexFrom);

		if (lDelta == 1 && (pIndexTo % mWidth) != 0)
		{
			return uint32_t(Direction::Right);
		}

		if (lDelta == -1 && (pIndexFrom % mWidth) != 0)
		{
			return uint32_t(Direction::Left);
		}

		if (lDelta == mWidth)
		{
			return uint32_t(Direction::Up);
		}

		if (lDelta == -mWidth)
		{
			return uint32_t(Direction::Down);
		}

		return uint32_t(Direction::Undefined);
	}

	uint32_t GetOppositeDirection(uint32_t pDirection) const override
	{
		if (pDirection > uint32_t(Direction::Down))
		{
			return UINT32_MAX;
		}

		return (pDirection + 2) % 4;
	}

	uint32_t GetNextNode(uint32_t pIndexFrom, uint32_t pDirection) const override
	{
		if (pIndexFrom == UINT32_MAX)
		{
			return UINT32_MAX;
		}

		switch (Direction(pDirection))
		{
		case Direction::Right:
			if ((pIndexFrom + 1) % mWidth != 0)
			{
				return pIndexFrom + 1;
			}
			break;
		case Direction::Left:
			if (pIndexFrom % mWidth != 0)
			{
				return pIndexFrom - 1;
			}
			break;
		case Direction::Up:
			if (pIndexFrom + mWidth < mWidth * mHeight)
			{
				return pIndexFrom + mWidth;
			}
			break;
		case Direction::Down:
			if (pIndexFrom >= mWidth)
			{
				return pIndexFrom - mWidth;
			}
			break;
		}
		return UINT32_MAX;
	}

private:
	int32_t mWidth;
	int32_t mHeight;
};

class SquareRoomsOnRectangularSpace: public Shape
{
public:
	SquareRoomsOnRectangularSpace(size_t pWidth, size_t pHeight):
		Shape()
	{
		mSize = { int32_t(pWidth), int32_t(pHeight), 1};
		mRoomType = RoomType::Square;
		mTopology = new Topology(mSize.X * mSize.Y);
		mRoomNeighborhood = new RectangularSpaceSquareRoomNeighborhood(mSize.X, mSize.Y);

		for (size_t j = 0; j < mSize.Y; ++j)
		{
			for (size_t i = 0; i < mSize.X; ++i)
			{
				size_t lRoomIndex = j * mSize.X + i;
				if (i < mSize.X - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + 1);
				}

				if (j < mSize.Y - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + mSize.X);
				}
			}
		}
	}

	~SquareRoomsOnRectangularSpace()
	{
		delete mTopology;
		delete mRoomNeighborhood;
	}

	Vector3f GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vector3f(pIndex % mSize.X, pIndex / mSize.X, 0);
	}

	Vector3f GetSpaceSize() override
	{
		return Vector3f(mSize.X, mSize.Y, mSize.Z);
	}

	Vector3f GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		return { 0, 0, 0 };
	}

	GridShape GetGridShape() override
	{
		return GridShape::Square;
	}
};

class ToreSpaceSquareRoomNeighborhood :public RoomNeighborhood
{
	enum class Direction :uint32_t
	{
		Right = 0,
		Up = 1,
		Left = 2,
		Down = 3,
		Undefined = UINT32_MAX
	};

public:
	ToreSpaceSquareRoomNeighborhood(int32_t pWidth, int32_t pHeight) :
		mWidth(pWidth),
		mHeight(pHeight)
	{
	}

	uint32_t GetDirectionCount() const override
	{
		return 4;
	}

	//can be undetermined for Triangle rooms, return 
	uint32_t GetDirection(uint32_t pIndexFrom, uint32_t pIndexTo) const override
	{
		int32_t lDelta = int32_t(pIndexTo) - int32_t(pIndexFrom);

		if ((lDelta == 1 && (pIndexTo % mWidth) != 0) || (lDelta == (1 - mWidth) && (pIndexTo % mWidth) == 0))
		{
			return uint32_t(Direction::Right);
		}

		if ((lDelta == -1 && (pIndexFrom % mWidth) != 0) || (lDelta == (mWidth - 1) && (pIndexFrom % mWidth) == 0))
		{
			return uint32_t(Direction::Left);
		}

		if (lDelta == mWidth || (pIndexTo < mWidth && lDelta == (mWidth * (1 - mHeight))))
		{
			return uint32_t(Direction::Up);
		}

		if (lDelta == -mWidth || (pIndexFrom < mWidth && lDelta == (mWidth * (mHeight - 1))))
		{
			return uint32_t(Direction::Down);
		}

		return uint32_t(Direction::Undefined);
	}

	uint32_t GetOppositeDirection(uint32_t pDirection) const override
	{
		if (pDirection > uint32_t(Direction::Down))
		{
			return UINT32_MAX;
		}

		return (pDirection + 2) % 4;
	}

	uint32_t GetNextNode(uint32_t pIndexFrom, uint32_t pDirection) const override
	{
		switch (Direction(pDirection))
		{
		case Direction::Right:
			if ((pIndexFrom + 1) % mWidth != 0)
			{
				return pIndexFrom + 1;
			}
			else
			{
				return pIndexFrom + 1 - mWidth;
			}
			break;
		case Direction::Left:
			if (pIndexFrom % mWidth != 0)
			{
				return pIndexFrom - 1;
			}
			else
			{
				return pIndexFrom + mWidth - 1;
			}
			break;
		case Direction::Up:
			if (pIndexFrom + mWidth < mWidth * mHeight)
			{
				return pIndexFrom + mWidth;
			}
			else
			{
				return pIndexFrom - (mHeight - 1) * mWidth;
			}
			break;
		case Direction::Down:
			if (pIndexFrom >= mWidth)
			{
				return pIndexFrom - mWidth;
			}
			else
			{
				return pIndexFrom + (mHeight - 1) * mWidth;
			}
			break;
		}
		return UINT32_MAX;
	}

private:
	int32_t mWidth;
	int32_t mHeight;
};

class SquareRoomsOnToreSpace: public Shape
{
public:
	enum class Direction :uint32_t
	{
		Right = 0,
		Up = 1,
		Left = 2,
		Down = 3,
		Undefined = UINT32_MAX
	};

	SquareRoomsOnToreSpace(size_t pWidth, size_t pHeight) :
		Shape()
	{
		mRoomType = RoomType::Square;
		mSize = { int32_t(pWidth), int32_t(pHeight), 1 };
		mTopology = new Topology(mSize.X * mSize.Y);
		mRoomNeighborhood = new ToreSpaceSquareRoomNeighborhood(mSize.X, mSize.Y);

		for (size_t j = 0; j < mSize.Y; ++j)
		{
			for (size_t i = 0; i < mSize.X; ++i)
			{
				size_t lRoomIndex = j * mSize.X + i;
				if (i < mSize.X - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + 1);
				}
				else
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex - mSize.X + 1);
				}

				if (j < mSize.Y - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + mSize.X);
				}
				else
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex % mSize.X);
				}
			}
		}
	}

	~SquareRoomsOnToreSpace()
	{
		delete mTopology;
		delete mRoomNeighborhood;
	}
	
	Vector3f GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vector3f(pIndex % mSize.X, pIndex / mSize.X, 0);
	}

	Vector3f GetSpaceSize() override
	{
		return Vector3f(mSize.X, mSize.Y, mSize.Z);
	}

	Vector3f GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		Vector3f lPos0 = GetNodeNormalizedPosition(pIndex0);
		Vector3f lPos1 = GetNodeNormalizedPosition(pIndex1);

		float lDeltaX = lPos1.X - lPos0.X;
		float lDeltaY = lPos1.Y - lPos0.Y;

		float lSignX = lDeltaX < 0.f ? -1.f : 1.f;
		float lSignY = lDeltaY < 0.f ? -1.f : 1.f;

		return
		{
			fabs(lDeltaX) > 1.01f ? lSignX : 0.f,
			fabs(lDeltaY) > 1.01f ? lSignY : 0.f,
			0
		};
	}

	GridShape GetGridShape() override
	{
		return GridShape::Square;
	}
};

Shape* GenerateSquaresOnRectShape(Parameters& pParameters)
{
	if (pParameters.Count == 0)
	{
		pParameters.Count = 2;
		pParameters.Params = new Parameter[2];
		strcpy(pParameters.Params[0].Name, "Width");
		pParameters.Params[0].ValueAsInteger32 = 10;
		strcpy(pParameters.Params[1].Name, "Height");
		pParameters.Params[1].ValueAsInteger32 = 10;
		return nullptr;
	}

	uint32_t lWitdh = 10;
	uint32_t lHeight = 10;

	for (size_t i = 0; i < pParameters.Count; ++i)
	{
		if (std::string("Width") == pParameters.Params[i].Name) lWitdh = pParameters.Params[i].ValueAsInteger32;
		if (std::string("Height") == pParameters.Params[i].Name) lHeight = pParameters.Params[i].ValueAsInteger32;
	}

	return new SquareRoomsOnRectangularSpace(lWitdh, lHeight);
}

Shape* GenerateSquaresOnToreShape(Parameters& pParameters)
{
	if (pParameters.Count == 0)
	{
		pParameters.Count = 2;
		pParameters.Params = new Parameter[2];
		strcpy(pParameters.Params[0].Name, "Width");
		pParameters.Params[0].ValueAsInteger32 = 10;
		strcpy(pParameters.Params[1].Name, "Height");
		pParameters.Params[1].ValueAsInteger32 = 10;
		return nullptr;
	}

	uint32_t lWitdh = 10;
	uint32_t lHeight = 10;

	for (size_t i = 0; i < pParameters.Count; ++i)
	{
		if (std::string("Width") == pParameters.Params[i].Name) lWitdh = pParameters.Params[i].ValueAsInteger32;
		if (std::string("Height") == pParameters.Params[i].Name) lHeight = pParameters.Params[i].ValueAsInteger32;
	}

	return new SquareRoomsOnToreSpace(lWitdh, lHeight);
}

#if 1
//    1,3 -> 3,3 -> 5,3 -> 7,3 -> 9,3 -> 11,3 -> 13,3
// 0,2 -> 2,2 -> 4,2 -> 6,2 -> 8,2 -> 10,2 -> 12, 2
//    1,1 -> 3,1 -> 5,1 -> 7,1 -> 9,1 -> 11,1 -> 13,1
// 0,0 -> 2,0 -> 4,0 -> 6,0 -> 8,0 -> 10,0 -> 12, 0

#define SQRT3BY2 .86602540378443864676372317075294f

Vector2i ToHexagonalCoordinates(const Vector2i pInArrayRectangular)
{
	return Vector2i{(pInArrayRectangular.X * 2) + (pInArrayRectangular.Y % 2), pInArrayRectangular.Y };
}

Vector2i HexagonalToArrayCoordinates(const Vector2i pHexagonal)
{
	return Vector2i{ pHexagonal.X / 2, pHexagonal.Y };
}

int32_t ToRoomIndex(const Vector2i pRectangularSize, const Vector2i pInArrayRectangular)
{
	return pInArrayRectangular.Y * pRectangularSize.Width + pInArrayRectangular.X;
}

Vector2i IndexToArrayCoordinates(const Vector2i pRectangularSize, int32_t pIndex)
{
	return Vector2i{ pIndex % pRectangularSize.Width, pIndex / pRectangularSize.Width };
}

class RectangularSpaceHexagonalRoomNeighborhood :public RoomNeighborhood
{
	enum class Direction :uint32_t
	{
		Right = 0,
		RightUp = 1,
		LeftUp = 2,
		Left = 3,
		LeftDown = 4,
		RightDown = 5,
		Undefined = UINT32_MAX
	};

public:
	RectangularSpaceHexagonalRoomNeighborhood(Vector2i pSize) :
		mSize(pSize)
	{
	}

	uint32_t GetDirectionCount() const override
	{
		return 6;
	}

	//can be undetermined for Triangle rooms, could return several values
	uint32_t GetDirection(uint32_t pIndexFrom, uint32_t pIndexTo) const override
	{
		Vector2i lHexaFrom = ToHexagonalCoordinates(IndexToArrayCoordinates(mSize, pIndexFrom));
		Vector2i lHexaTo = ToHexagonalCoordinates(IndexToArrayCoordinates(mSize, pIndexTo));

		//int32_t lDelta = int32_t(pIndexTo) - int32_t(pIndexFrom);

		if ((lHexaTo.X) == (lHexaFrom.X+2) && (lHexaTo.Y) == (lHexaFrom.Y))
		{
			return uint32_t(Direction::Right);
		}

		if ((lHexaTo.X) == (lHexaFrom.X - 2) && (lHexaTo.Y) == (lHexaFrom.Y))
		{
			return uint32_t(Direction::Left);
		}

		if ((lHexaTo.X) == (lHexaFrom.X + 1) && (lHexaTo.Y) == (lHexaFrom.Y + 1))
		{
			return uint32_t(Direction::RightUp);
		}

		if ((lHexaTo.X) == (lHexaFrom.X - 1) && (lHexaTo.Y) == (lHexaFrom.Y + 1))
		{
			return uint32_t(Direction::LeftUp);
		}

		if ((lHexaTo.X) == (lHexaFrom.X + 1) && (lHexaTo.Y) == (lHexaFrom.Y - 1))
		{
			return uint32_t(Direction::RightDown);
		}

		if ((lHexaTo.X) == (lHexaFrom.X - 1) && (lHexaTo.Y) == (lHexaFrom.Y - 1))
		{
			return uint32_t(Direction::LeftDown);
		}

		return uint32_t(Direction::Undefined);
	}

	uint32_t GetOppositeDirection(uint32_t pDirection) const override
	{
		if (pDirection > uint32_t(Direction::RightDown))
		{
			return UINT32_MAX;
		}

		return (pDirection + 3) % GetDirectionCount();
	}

	uint32_t GetNextNode(uint32_t pIndexFrom, uint32_t pDirection) const override
	{
		Vector2i lHexaFrom = ToHexagonalCoordinates(IndexToArrayCoordinates(mSize, pIndexFrom));

		if (pDirection <= uint32_t(Direction::RightDown))
		{//direction is valid
			Vector2i lHexaTo = lHexaFrom + mDeltaFromDirection[pDirection];

			Vector2i lArrayCoords = HexagonalToArrayCoordinates(lHexaTo);

			if (lArrayCoords.X >= 0 && lArrayCoords.X < mSize.Width && lArrayCoords.Y >= 0 && lArrayCoords.Y < mSize.Height)
			{//Room is inside grid
				return ToRoomIndex(mSize, lArrayCoords);
			}
		}

		return UINT32_MAX;
	}

private:
	Vector2i mSize;

	std::vector<Vector2i> mDeltaFromDirection =
	{
		{2,0},
		{1,1},
		{-1,1},
		{-2,0},
		{-1,-1},
		{1,-1},
	};
};

class HexagonRoomsOnRectangularSpace : public Shape
{
public:
	HexagonRoomsOnRectangularSpace(size_t pWidth, size_t pHeight) :
		Shape()
	{
		//TODO:
		mRoomType = RoomType::Hexagonal;
		mSize = { int32_t(pWidth), int32_t(pHeight), 1 };
		mTopology = new Topology(mSize.X * mSize.Y);

		///std::cout << "Generate " << pWidth << " * " << pHeight << std::endl;

		mRoomNeighborhood = new RectangularSpaceHexagonalRoomNeighborhood(Vector2i{ mSize.X, mSize.Y });// new RectangularSpaceSquareRoomNeighborhood(mWidth, mHeight);

		Vector2i lSize{ mSize.X, mSize.Y };

		for (size_t j = 0; j < mSize.Y; ++j)
		{
			for (size_t i = 0; i < mSize.X; ++i)
			{
				Vector2i lInArrayCoordinates{ i,j };
				Vector2i lHexagonalCoordinates = ToHexagonalCoordinates(lInArrayCoordinates);

				Vector2i lRoom0 = HexagonalToArrayCoordinates({ lHexagonalCoordinates.X+2, lHexagonalCoordinates.Y });
				Vector2i lRoom1 = HexagonalToArrayCoordinates({ lHexagonalCoordinates.X+1, lHexagonalCoordinates.Y+1 });
				Vector2i lRoom2 = HexagonalToArrayCoordinates({ lHexagonalCoordinates.X-1, lHexagonalCoordinates.Y+1 });

				int32_t lRoomIndexBase = ToRoomIndex(lSize, lInArrayCoordinates);

				if (lRoom0.X < lSize.Width && lRoom0.X >= 0 && lRoom0.Y < lSize.Height && lRoom0.Y >= 0)
				{
					mTopology->ConnectNodes(lRoomIndexBase, ToRoomIndex(lSize, lRoom0));
				}

				if (lRoom1.X < lSize.Width && lRoom1.X >= 0 && lRoom1.Y < lSize.Height && lRoom1.Y >= 0)
				{
					mTopology->ConnectNodes(lRoomIndexBase, ToRoomIndex(lSize, lRoom1));
				}

				if (lRoom2.X < lSize.Width && lRoom2.X >= 0 && lRoom2.Y < lSize.Height && lRoom2.Y >= 0)
				{
					mTopology->ConnectNodes(lRoomIndexBase, ToRoomIndex(lSize, lRoom2));
				}
			}
		}
	}

	~HexagonRoomsOnRectangularSpace()
	{
		delete mTopology;
		delete mRoomNeighborhood;
	}

	Vector3f GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		Vector2i lHexagonalCoordinates = ToHexagonalCoordinates(IndexToArrayCoordinates(Vector2i{ mSize.X, mSize.Y }, pIndex));
		return Vector3f(lHexagonalCoordinates.X * SQRT3BY2, lHexagonalCoordinates.Y * 1.5f, 0);
	}

	Vector3f GetSpaceSize() override
	{
		return Vector3f(mSize.X * SQRT3BY2 * 2, mSize.Y * 1.5f, 1);
	}

	Vector3f GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		return { 0, 0, 0 };
	}

	GridShape GetGridShape() override
	{
		return GridShape::Hexagon;
	}
};

Shape* GenerateHexagonsOnRectShape(Parameters& pParameters)
{
	if (pParameters.Count == 0)
	{
		pParameters.Count = 2;
		pParameters.Params = new Parameter[2];
		strcpy(pParameters.Params[0].Name, "Width");
		pParameters.Params[0].ValueAsInteger32 = 10;
		strcpy(pParameters.Params[1].Name, "Height");
		pParameters.Params[1].ValueAsInteger32 = 10;
		return nullptr;
	}

	uint32_t lWitdh = 10;
	uint32_t lHeight = 10;

	for (size_t i = 0; i < pParameters.Count; ++i)
	{
		if (std::string("Width") == pParameters.Params[i].Name) lWitdh = pParameters.Params[i].ValueAsInteger32;
		if (std::string("Height") == pParameters.Params[i].Name) lHeight = pParameters.Params[i].ValueAsInteger32;
	}

	if (lHeight % 2 != 0)
	{
		lHeight += 1;

		if (pParameters.Count >= 2)
		{
			pParameters.Params[1].ValueAsInteger32 = lHeight;
		}
	}

	return new HexagonRoomsOnRectangularSpace(lWitdh, lHeight);
}
#endif
