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
		Shape(),
		mWidth(pWidth),
		mHeight(pHeight)
	{
		mRoomType = RoomType::Square;
		mTopology = new Topology(pWidth * pHeight);
		mRoomNeighborhood = new RectangularSpaceSquareRoomNeighborhood(mWidth, mHeight);

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
		delete mRoomNeighborhood;
	}

	Vector3f GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vector3f(pIndex % mWidth, pIndex / mWidth, 0);
	}

	Vector3f GetSpaceSize() override
	{
		return Vector3f(mWidth, mHeight, 1);
	}

	Vector3f GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		return { 0, 0, 0 };
	}

	void DrawEdge(std::vector<Vector3f>& pNodePositions, std::vector<float>& pContainer, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth, float pLineWidth, bool pContiguousDraw) override
	{
		float lLineOffset = pLineWidth / 2.f;
		//mShape = mGeometryContainer.GetShape();//this
		Vector3f lSpaceSize = GetSpaceSize();
		float lHalfWidth = (1.f / 2.f) + lLineOffset;

		if (pContiguousDraw)
		{
			const Vector3f& lNode0Position = pNodePositions[pNodeIndex0];

			Vector3f lSpaceDelta = GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

			Vector3f lNodeNormalizedPos0 = GetNodeNormalizedPosition(pNodeIndex0);
			Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);

			Vector3f lInitialNode0Position = { lNodeNormalizedPos0.X, lNodeNormalizedPos0.Y, 0 };

			//Since the space delta is from node 0 to node 1, we need to compensate it with opposited delta
			Vector3f lInitialNode1Position = { (lNodeNormalizedPos1.X - lSpaceDelta.X * lSpaceSize.X),
											   (lNodeNormalizedPos1.Y - lSpaceDelta.Y * lSpaceSize.Y), 0 };

			Vector3f lNode1Position = lNode0Position + (lInitialNode1Position - lInitialNode0Position);

			pNodePositions[pNodeIndex1] = lNode1Position;

			Vector3f lMin{ std::min(lNode0Position.X, lNode1Position.X) - lLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - lLineOffset, pDepth };
			Vector3f lMax{ std::max(lNode0Position.X, lNode1Position.X) + lLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + lLineOffset, pDepth };
			AddAARect(pContainer, lMin, lMax);
		}
		else
		{
			const Vector3f& lNode0Position = pNodePositions[pNodeIndex0];
			const Vector3f& lNode1Position = pNodePositions[pNodeIndex1];

			Vector3f lSpaceDelta = GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

			if ((abs(lSpaceDelta.X) + abs(lSpaceDelta.Y) + abs(lSpaceDelta.Z)) == 0)
			{
				Vector3f lMin{ std::min(lNode0Position.X, lNode1Position.X) - lLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - lLineOffset, pDepth };
				Vector3f lMax{ std::max(lNode0Position.X, lNode1Position.X) + lLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + lLineOffset, pDepth };
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.X < 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualalNode0Position = { (-lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X),
													 (-lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y), 0 };


				Vector3f lMin{ lNode0Position.X - lLineOffset, lNode0Position.Y - lLineOffset, pDepth };
				Vector3f lMax{ lMin.X + lHalfWidth, lVirtualalNode0Position.Y + lLineOffset, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.X -= mWidth - lHalfWidth;
				lMax.X -= mWidth - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.X > 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualNode0Position = { -lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X,
											   -lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y, 0 };

				Vector3f lMax{ lNode0Position.X + lLineOffset, lNode0Position.Y + lLineOffset, pDepth };
				Vector3f lMin{ (lMax.X + lVirtualNode0Position.X - lLineOffset) / 2, lVirtualNode0Position.Y - lLineOffset, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.X += mWidth - lHalfWidth;
				lMax.X += mWidth - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.Y < 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualalNode0Position = { -lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X,
													 -lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y, 0 };

				Vector3f lMin{ lNode0Position.X - lLineOffset, lNode0Position.Y - lLineOffset, pDepth };
				Vector3f lMax{ lVirtualalNode0Position.X + lLineOffset, (lMin.Y + lVirtualalNode0Position.Y + lLineOffset) / 2, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.Y -= mHeight - lHalfWidth;
				lMax.Y -= mHeight - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.Y > 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualNode0Position = { -lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X,
												   -lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y, 0 };

				Vector3f lMax{ lNode0Position.X + lLineOffset, lNode0Position.Y + lLineOffset, pDepth };
				Vector3f lMin{ lVirtualNode0Position.X - lLineOffset, (lMax.Y + lVirtualNode0Position.Y - lLineOffset) / 2, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.Y += mHeight - lHalfWidth;
				lMax.Y += mHeight - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
		}
	}

private:
	int32_t mWidth = 10;
	int32_t mHeight = 10;
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
		Shape(),
		mWidth(pWidth),
		mHeight(pHeight)
	{
		mRoomType = RoomType::Square;
		mTopology = new Topology(pWidth * pHeight);
		mRoomNeighborhood = new ToreSpaceSquareRoomNeighborhood(mWidth, mHeight);

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
		delete mRoomNeighborhood;
	}
	
	Vector3f GetNodeNormalizedPosition(uint32_t pIndex) override
	{
		return Vector3f(pIndex % mWidth, pIndex / mWidth, 0);
	}

	Vector3f GetSpaceSize() override
	{
		return Vector3f(mWidth, mHeight, 1);
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

	void DrawEdge(std::vector<Vector3f>& pNodePositions, std::vector<float>& pContainer, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth, float pLineWidth, bool pContiguousDraw) override
	{
		float lLineOffset = pLineWidth / 2.f;
		//mShape = mGeometryContainer.GetShape();//this
		Vector3f lSpaceSize = GetSpaceSize();
		float lHalfWidth = (1.f / 2.f) + lLineOffset;

		if (pContiguousDraw)
		{
			const Vector3f& lNode0Position = pNodePositions[pNodeIndex0];

			Vector3f lSpaceDelta = GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

			Vector3f lNodeNormalizedPos0 = GetNodeNormalizedPosition(pNodeIndex0);
			Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);

			Vector3f lInitialNode0Position = { lNodeNormalizedPos0.X, lNodeNormalizedPos0.Y, 0 };

			//Since the space delta is from node 0 to node 1, we need to compensate it with opposited delta
			Vector3f lInitialNode1Position = { (lNodeNormalizedPos1.X - lSpaceDelta.X * lSpaceSize.X),
											   (lNodeNormalizedPos1.Y - lSpaceDelta.Y * lSpaceSize.Y), 0 };

			Vector3f lNode1Position = lNode0Position + (lInitialNode1Position - lInitialNode0Position);

			pNodePositions[pNodeIndex1] = lNode1Position;

			Vector3f lMin{ std::min(lNode0Position.X, lNode1Position.X) - lLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - lLineOffset, pDepth };
			Vector3f lMax{ std::max(lNode0Position.X, lNode1Position.X) + lLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + lLineOffset, pDepth };
			AddAARect(pContainer, lMin, lMax);
		}
		else
		{
			const Vector3f& lNode0Position = pNodePositions[pNodeIndex0];
			const Vector3f& lNode1Position = pNodePositions[pNodeIndex1];

			Vector3f lSpaceDelta = GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

			if ((abs(lSpaceDelta.X) + abs(lSpaceDelta.Y) + abs(lSpaceDelta.Z)) == 0)
			{
				Vector3f lMin{ std::min(lNode0Position.X, lNode1Position.X) - lLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - lLineOffset, pDepth };
				Vector3f lMax{ std::max(lNode0Position.X, lNode1Position.X) + lLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + lLineOffset, pDepth };
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.X < 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualalNode0Position = { (-lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X),
													 (-lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y), 0 };


				Vector3f lMin{ lNode0Position.X - lLineOffset, lNode0Position.Y - lLineOffset, pDepth };
				Vector3f lMax{ lMin.X + lHalfWidth, lVirtualalNode0Position.Y + lLineOffset, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.X -= mWidth - lHalfWidth;
				lMax.X -= mWidth - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.X > 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualNode0Position = { -lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X,
											   -lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y, 0 };

				Vector3f lMax{ lNode0Position.X + lLineOffset, lNode0Position.Y + lLineOffset, pDepth };
				Vector3f lMin{ (lMax.X + lVirtualNode0Position.X - lLineOffset) / 2, lVirtualNode0Position.Y - lLineOffset, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.X += mWidth - lHalfWidth;
				lMax.X += mWidth - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.Y < 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualalNode0Position = { -lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X,
													 -lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y, 0 };

				Vector3f lMin{ lNode0Position.X - lLineOffset, lNode0Position.Y - lLineOffset, pDepth };
				Vector3f lMax{ lVirtualalNode0Position.X + lLineOffset, (lMin.Y + lVirtualalNode0Position.Y + lLineOffset) / 2, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.Y -= mHeight - lHalfWidth;
				lMax.Y -= mHeight - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
			else if (lSpaceDelta.Y > 0 && true)
			{
				Vector3f lNodeNormalizedPos1 = GetNodeNormalizedPosition(pNodeIndex1);
				Vector3f lVirtualNode0Position = { -lSpaceDelta.X * lSpaceSize.X + lNodeNormalizedPos1.X,
												   -lSpaceDelta.Y * lSpaceSize.Y + lNodeNormalizedPos1.Y, 0 };

				Vector3f lMax{ lNode0Position.X + lLineOffset, lNode0Position.Y + lLineOffset, pDepth };
				Vector3f lMin{ lVirtualNode0Position.X - lLineOffset, (lMax.Y + lVirtualNode0Position.Y - lLineOffset) / 2, pDepth };
				AddAARect(pContainer, lMin, lMax);

				lMin.Y += mHeight - lHalfWidth;
				lMax.Y += mHeight - lHalfWidth;
				AddAARect(pContainer, lMin, lMax);
			}
		}
	}

private:
	int32_t mWidth = 10;
	int32_t mHeight = 10;
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
// 0,0 -> 2,0 -> 4,0 -> 6,0 -> 8,0 -> 10,0 -> 12, 

#define SQRT3BY2 .86602540378443864676372317075294f
#define TWOBYSQRT3 1.1547005383792515290182975610039f
#define SQRT3 1.7320508075688772935274463415059f

#if 1
static Vector3f P0{ SQRT3BY2, 0.5, 0.f };
static Vector3f P1{ 0.f, 1.f, 0.f };
static Vector3f P2{ -SQRT3BY2, .5f, 0.f };
static Vector3f P3{ -SQRT3BY2, -.5f, 0.f };
static Vector3f P4{ 0.f, -1.f, 0.f };
static Vector3f P5{ SQRT3BY2, -.5f, 0.f };
#else
static Vector3f P0{ SQRT3, 1.f, 0.f };
static Vector3f P1{ 0.f, 2.f, 0.f };
static Vector3f P2{ -SQRT3, 1.f, 0.f };
static Vector3f P3{ -SQRT3, -1.f, 0.f };
static Vector3f P4{ 0.f, -2.f, 0.f };
static Vector3f P5{ SQRT3, -1.f, 0.f };
#endif
static Vector3f D0{ 1.f, 0.f, 0.f };
static Vector3f D1{ .5f, SQRT3BY2, 0.f };
static Vector3f D2{ -.5f, SQRT3BY2, 0.f };
static Vector3f D3{ -1.f, 0.f, 0.f };
static Vector3f D4{ -.5f, -SQRT3BY2, 0.f };
static Vector3f D5{ .5f, -SQRT3BY2, 0.f };

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

	//int32_t mWidth;
	//int32_t mHeight;
};

class HexagonRoomsOnRectangularSpace : public Shape
{
public:
	HexagonRoomsOnRectangularSpace(size_t pWidth, size_t pHeight) :
		Shape(),
		mWidth(pWidth),
		mHeight(pHeight)
	{
		//TODO:
		mRoomType = RoomType::Hexagonal;
		mTopology = new Topology(pWidth * pHeight);

		///std::cout << "Generate " << pWidth << " * " << pHeight << std::endl;

		mRoomNeighborhood = new RectangularSpaceHexagonalRoomNeighborhood(Vector2i{ int32_t(mWidth), int32_t(mHeight) });// new RectangularSpaceSquareRoomNeighborhood(mWidth, mHeight);

		Vector2i lSize{ pWidth, pHeight };

		for (size_t j = 0; j < pHeight; ++j)
		{
			for (size_t i = 0; i < pWidth; ++i)
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
		Vector2i lHexagonalCoordinates = ToHexagonalCoordinates(IndexToArrayCoordinates(Vector2i{ int32_t(mWidth), int32_t(mHeight) }, pIndex));
		return Vector3f(lHexagonalCoordinates.X * SQRT3BY2, lHexagonalCoordinates.Y * 1.5f, 0);
	}

	Vector3f GetSpaceSize() override
	{
		return Vector3f(mWidth * SQRT3BY2 * 2, mHeight * 1.5f, 1);
	}

	Vector3f GetUnitSpaceDelta(uint32_t pIndex0, uint32_t pIndex1) override
	{
		return { 0, 0, 0 };
	}

	void DrawEdge(std::vector<Vector3f>& pNodePositions, std::vector<float>& pContainer, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth, float pLineWidth, bool pContiguousDraw) override
	{
		float lLineOffset = pLineWidth / 2.f;
		//mShape = mGeometryContainer.GetShape();//this
		Vector3f lSpaceSize = GetSpaceSize();
		float lHalfWidth = (1.f / 2.f) + lLineOffset;

		uint32_t lDirection = mRoomNeighborhood->GetDirection(pNodeIndex0, pNodeIndex1);

		//if (pContiguousDraw)
		{
			const Vector3f& lNode0Position = pNodePositions[pNodeIndex0];
			const Vector3f& lNode1Position = pNodePositions[pNodeIndex1];

			Vector3f lP0;
			Vector3f lP1;
			Vector3f lP2;
			Vector3f lP3;
			
			Vector3f lP0_Delta;
			Vector3f lP1_Delta;
			Vector3f lP2_Delta;
			Vector3f lP3_Delta;

			float lMultiplier = pLineWidth/SQRT3;

			switch (lDirection)
			{
			case 0:
				lP0_Delta = pLineWidth * P3 + lMultiplier * D0;
				lP1_Delta = pLineWidth * P5 + lMultiplier * D3;
				lP2_Delta = pLineWidth * P0 + lMultiplier * D3;
				lP3_Delta = pLineWidth * P2 + lMultiplier * D0;
				break;
			case 1:
				lP0_Delta = pLineWidth * P4 + lMultiplier * D1;
				lP1_Delta = pLineWidth * P0 + lMultiplier * D4;
				lP2_Delta = pLineWidth * P1 + lMultiplier * D4;
				lP3_Delta = pLineWidth * P3 + lMultiplier * D1;
				break;
			case 2:
				lP0_Delta = pLineWidth * P5 + lMultiplier * D2;
				lP1_Delta = pLineWidth * P1 + lMultiplier * D5;
				lP2_Delta = pLineWidth * P2 + lMultiplier * D5;
				lP3_Delta = pLineWidth * P4 + lMultiplier * D2;
				break;
			case 3:
				lP0_Delta = pLineWidth * P0 + lMultiplier * D3;
				lP1_Delta = pLineWidth * P2 + lMultiplier * D0;
				lP2_Delta = pLineWidth * P3 + lMultiplier * D0;
				lP3_Delta = pLineWidth * P5 + lMultiplier * D3;
				break;
			case 4:
				lP0_Delta = pLineWidth * P1 + lMultiplier * D4;
				lP1_Delta = pLineWidth * P3 + lMultiplier * D1;
				lP2_Delta = pLineWidth * P4 + lMultiplier * D1;
				lP3_Delta = pLineWidth * P0 + lMultiplier * D4;
				break;
			case 5:
				lP0_Delta = pLineWidth * P2 + lMultiplier * D5;
				lP1_Delta = pLineWidth * P4 + lMultiplier * D2;
				lP2_Delta = pLineWidth * P5 + lMultiplier * D2;
				lP3_Delta = pLineWidth * P1 + lMultiplier * D5;
				break;
			}

			lP0 = lNode0Position + lP0_Delta;
			lP1 = lNode1Position + lP1_Delta;
			lP2 = lNode1Position + lP2_Delta;
			lP3 = lNode0Position + lP3_Delta;

			{
				pContainer.push_back(lP0.X); pContainer.push_back(lP0.Y); pContainer.push_back(pDepth);
				pContainer.push_back(lP1.X); pContainer.push_back(lP1.Y); pContainer.push_back(pDepth);
				pContainer.push_back(lP2.X); pContainer.push_back(lP2.Y); pContainer.push_back(pDepth);
				pContainer.push_back(lP2.X); pContainer.push_back(lP2.Y); pContainer.push_back(pDepth);
				pContainer.push_back(lP3.X); pContainer.push_back(lP3.Y); pContainer.push_back(pDepth);
				pContainer.push_back(lP0.X); pContainer.push_back(lP0.Y); pContainer.push_back(pDepth);
			}
		}

	}

private:
	uint32_t mWidth = 10;
	uint32_t mHeight = 10;
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
