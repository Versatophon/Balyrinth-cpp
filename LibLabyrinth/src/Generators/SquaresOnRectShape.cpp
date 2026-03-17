#include "../Shape.h"

#include "../Topology.h"

#include "../RoomNeighborhood.h"

#include <Vector3f.h>

class RectangularSpaceSquareRoomNeighborhood :public RoomNeighborhood
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
	RectangularSpaceSquareRoomNeighborhood(int32_t pWidth, int32_t pHeight) :
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


class SquareRoomsOnRectangularSpace : public Shape
{
public:
	SquareRoomsOnRectangularSpace(size_t pWidth, size_t pHeight) :
		Shape()
	{
		mSize = { int32_t(pWidth), int32_t(pHeight), 1 };
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
