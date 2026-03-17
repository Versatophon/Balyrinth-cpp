#include "../Shape.h"

#include "../Topology.h"

#include "../RoomNeighborhood.h"

#include <Vector3f.h>
#include <Vector2i.h>

//    1,3 -> 3,3 -> 5,3 -> 7,3 -> 9,3 -> 11,3 -> 13,3
// 0,2 -> 2,2 -> 4,2 -> 6,2 -> 8,2 -> 10,2 -> 12, 2
//    1,1 -> 3,1 -> 5,1 -> 7,1 -> 9,1 -> 11,1 -> 13,1
// 0,0 -> 2,0 -> 4,0 -> 6,0 -> 8,0 -> 10,0 -> 12, 0

#define SQRT3BY2 .86602540378443864676372317075294f
#define SQRT3 1.7320508075688772935274463415059f

inline Vector2i ToHexagonalCoordinates(const Vector2i pInArrayRectangular)
{
	return Vector2i{ (pInArrayRectangular.X * 2) + (pInArrayRectangular.Y % 2), pInArrayRectangular.Y };
}

inline Vector2i HexagonalToArrayCoordinates(const Vector2i pHexagonal)
{
	return Vector2i{ pHexagonal.X / 2, pHexagonal.Y };
}

inline int32_t ToRoomIndex(const Vector2i pRectangularSize, const Vector2i pInArrayRectangular)
{
	return pInArrayRectangular.Y * pRectangularSize.Width + pInArrayRectangular.X;
}

inline Vector2i IndexToArrayCoordinates(const Vector2i pRectangularSize, int32_t pIndex)
{
	return Vector2i{ pIndex % pRectangularSize.Width, pIndex / pRectangularSize.Width };
}

class ToreSpaceHexagonalRoomNeighborhood :public RoomNeighborhood
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
	ToreSpaceHexagonalRoomNeighborhood(Vector2i pSize) :
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

		int32_t lDeltaX = (lHexaTo.X - lHexaFrom.X);
		int32_t lDeltaY = lHexaTo.Y - lHexaFrom.Y;

		if (lDeltaY == 0)//Same line
		{
			if ((lDeltaX == 2) || (lDeltaX == ((1 - mSize.Width) * 2)))
			{
				return uint32_t(Direction::Right);
			}

			if ((lDeltaX == -2) || (lDeltaX == ((mSize.Width - 1) * 2)))
			{
				return uint32_t(Direction::Left);
			}
		}
		else if ((lDeltaY == 1) || (lDeltaY == (1 - mSize.Height)))//Up Direction
		{
			if ((lDeltaX == 1) || (lDeltaX == (1 - 2 * mSize.Width)))
			{
				return uint32_t(Direction::RightUp);
			}

			if ((lDeltaX == -1) || (lDeltaX == (2 * mSize.Width - 1)))
			{
				return uint32_t(Direction::LeftUp);
			}
		}
		else if ((lDeltaY == -1) || (lDeltaY == (mSize.Height - 1)))//Down Direction
		{
			if ((lDeltaX == 1) || (lDeltaX == (1 - 2 * mSize.Width)))
			{
				return uint32_t(Direction::RightDown);
			}

			if ((lDeltaX == -1) || (lDeltaX == (2 * mSize.Width - 1)))
			{
				return uint32_t(Direction::LeftDown);
			}
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

			if (lHexaTo.X >= (mSize.Width * 2))
			{
				lHexaTo.X -= (mSize.Width * 2);
			}
			else if (lHexaTo.X < 0)
			{
				lHexaTo.X += (mSize.Width * 2);
			}

			if (lHexaTo.Y >= (mSize.Height))
			{
				lHexaTo.Y -= mSize.Height;
			}
			else if (lHexaTo.Y < 0)
			{
				lHexaTo.Y += mSize.Height;
			}

			Vector2i lArrayCoords = HexagonalToArrayCoordinates(lHexaTo);

			return ToRoomIndex(mSize, lArrayCoords);
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

class HexagonRoomsOnToreSpace : public Shape
{
public:
	HexagonRoomsOnToreSpace(size_t pWidth, size_t pHeight) :
		Shape()
	{
		//TODO:
		mRoomType = RoomType::Hexagonal;
		mSize = { int32_t(pWidth), int32_t(pHeight), 1 };
		mTopology = new Topology(mSize.X * mSize.Y);

		///std::cout << "Generate " << pWidth << " * " << pHeight << std::endl;

		mRoomNeighborhood = new ToreSpaceHexagonalRoomNeighborhood(Vector2i{ mSize.X, mSize.Y });// new RectangularSpaceSquareRoomNeighborhood(mWidth, mHeight);

		Vector2i lSize{ mSize.X, mSize.Y };

		for (size_t j = 0; j < mSize.Y; ++j)
		{
			for (size_t i = 0; i < mSize.X; ++i)
			{
				Vector2i lInArrayCoordinates{ i,j };
				Vector2i lHexagonalCoordinates = ToHexagonalCoordinates(lInArrayCoordinates);

				Vector2i lRoom0 = HexagonalToArrayCoordinates({ (lHexagonalCoordinates.X + 2) % (lSize.Width * 2), lHexagonalCoordinates.Y });
				Vector2i lRoom1 = HexagonalToArrayCoordinates({ (lHexagonalCoordinates.X + 1) % (lSize.Width * 2), (lHexagonalCoordinates.Y + 1) % lSize.Height });
				Vector2i lRoom2 = HexagonalToArrayCoordinates({ (lHexagonalCoordinates.X + (lSize.Width * 2) - 1) % (lSize.Width * 2), (lHexagonalCoordinates.Y + 1) % lSize.Height });

				int32_t lRoomIndexBase = ToRoomIndex(lSize, lInArrayCoordinates);
				mTopology->ConnectNodes(lRoomIndexBase, ToRoomIndex(lSize, lRoom0));
				mTopology->ConnectNodes(lRoomIndexBase, ToRoomIndex(lSize, lRoom1));
				mTopology->ConnectNodes(lRoomIndexBase, ToRoomIndex(lSize, lRoom2));
			}
		}
	}

	~HexagonRoomsOnToreSpace()
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
		Vector3f lPos0 = GetNodeNormalizedPosition(pIndex0);
		Vector3f lPos1 = GetNodeNormalizedPosition(pIndex1);

		float lDeltaX = lPos1.X - lPos0.X;
		float lDeltaY = lPos1.Y - lPos0.Y;

		float lSignX = lDeltaX < 0.f ? -1.f : 1.f;
		float lSignY = lDeltaY < 0.f ? -1.f : 1.f;

		return
		{
			fabs(lDeltaX) > (SQRT3 + .01f) ? lSignX : 0.f,
			fabs(lDeltaY) > 1.51f ? lSignY : 0.f,
			0
		};
	}

	GridShape GetGridShape() override
	{
		return GridShape::Hexagon;
	}
};

Shape* GenerateHexagonsOnToreShape(Parameters& pParameters)
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

	return new HexagonRoomsOnToreSpace(lWitdh, lHeight);
}
