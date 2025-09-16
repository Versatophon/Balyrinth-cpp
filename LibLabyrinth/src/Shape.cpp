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
	
	Vec3 GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vec3(pIndex % mWidth, pIndex / mWidth, 0);
	}

	Vec3 GetSpaceSize() override
	{
		return Vec3(mWidth, mHeight, 1);
	}

	Vec3 GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
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
		//mUnitSpaceDividerX = (mWidth == 1 ? INT32_MIN : int32_t( - mWidth));
		//mUnitSpaceDividerY = (mHeight == 1 ? INT32_MIN : int32_t( - mHeight));

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
	
	Vec3 GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vec3(pIndex % mWidth, pIndex / mWidth, 0);
	}

	Vec3 GetSpaceSize() override
	{
		return Vec3(mWidth, mHeight, 1);
	}

	Vec3 GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		Vec3 lPos0 = GetNodeNormalizedPosition(pIndex0);
		Vec3 lPos1 = GetNodeNormalizedPosition(pIndex1);

		//return {(lPos1.X - lPos0.X) / mUnitSpaceDividerX, (lPos1.Y - lPos0.Y) / mUnitSpaceDividerY, 0 };

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

private:
	uint32_t mWidth = 10;
	uint32_t mHeight = 10;

	//int32_t mUnitSpaceDividerX = 1;
	//int32_t mUnitSpaceDividerY = 1;
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
