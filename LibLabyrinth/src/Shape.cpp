#include "Shape.h"

#include "Topology.h"

#include "Vec3.h"

#include <string>

Shape::Shape()
{
}

Shape::~Shape()
{
}

class SquareRoomsOnRectangularSpace: public Shape
{
public:
	SquareRoomsOnRectangularSpace(size_t pWidth, size_t pHeight):
		Shape(),
		mWidth(pWidth),
		mHeight(pHeight)
	{
		mRoomType = RoomType::Square;
		mTopology = new Topology(pWidth * pHeight);

		for (size_t j = 0; j < pHeight; ++j)
		{
			for (size_t i = 0; i < pWidth; ++i)
			{
				size_t lRoomIndex = j * pWidth + i;
				if (i < pWidth - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + 1);
				}

				if (j < pHeight - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + pWidth);
				}
			}
		}
	}

	~SquareRoomsOnRectangularSpace()
	{
		delete mTopology;
	}

	Topology* GetTopology() override
	{
		return mTopology;
	}
	
	Vec3i GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vec3i(pIndex % mWidth, pIndex / mWidth, 0);
	}

	Vec3i GetSpaceSize() override
	{
		return Vec3i(mWidth, mHeight, 1);
	}

	Vec3i GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		return { 0, 0, 0 };
	}

private:
	uint32_t mWidth = 10;
	uint32_t mHeight = 10;
};

class SquareRoomsOnToreSpace: public Shape
{
public:
	SquareRoomsOnToreSpace(size_t pWidth, size_t pHeight) :
		Shape(),
		mWidth(pWidth),
		mHeight(pHeight)
	{
		mUnitSpaceDividerX = (mWidth == 1 ? INT32_MIN : int32_t(1 - mWidth));
		mUnitSpaceDividerY = (mHeight == 1 ? INT32_MIN : int32_t(1 - mHeight));

		mRoomType = RoomType::Square;
		mTopology = new Topology(pWidth * pHeight);

		for (size_t j = 0; j < pHeight; ++j)
		{
			for (size_t i = 0; i < pWidth; ++i)
			{
				size_t lRoomIndex = j * pWidth + i;
				if (i < pWidth - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + 1);
				}
				else
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex - pWidth + 1);
				}

				if (j < pHeight - 1)
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex + pWidth);
				}
				else
				{
					mTopology->ConnectNodes(lRoomIndex, lRoomIndex % pWidth);
				}
			}
		}
	}

	~SquareRoomsOnToreSpace()
	{
		delete mTopology;
	}
	
	Topology* GetTopology() override
	{
		return mTopology;
	}
	
	Vec3i GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vec3i(pIndex % mWidth, pIndex / mWidth, 0);
	}

	Vec3i GetSpaceSize() override
	{
		return Vec3i(mWidth, mHeight, 1);
	}

	Vec3i GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		Vec3i lPos0 = GetNodeNormalizedPosition(pIndex0);
		Vec3i lPos1 = GetNodeNormalizedPosition(pIndex1);

		return {(lPos1.X - lPos0.X) / mUnitSpaceDividerX, (lPos1.Y - lPos0.Y) / mUnitSpaceDividerY, 0 };
	}

private:
	uint32_t mWidth = 10;
	uint32_t mHeight = 10;

	int32_t mUnitSpaceDividerX = 1;
	int32_t mUnitSpaceDividerY = 1;
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
