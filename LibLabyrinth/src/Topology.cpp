#include "Topology.h"

#include <queue>

Topology::Topology(size_t pNodeCount):
	mNodes(std::vector<Node>(pNodeCount))
{
}

void Topology::Clear()
{
	for (size_t i = 0; i < mNodes.size(); ++i)
	{
		mNodes[i].RemoveAllNeighbors();
	}
}

Topology::~Topology()
{
}

size_t Topology::GetSize() const
{
	return mNodes.size();
}

void Topology::ConnectNodes(size_t pIndex0, size_t pIndex1)
{
	size_t lNodeCount = mNodes.size();

	if (pIndex0 < lNodeCount && pIndex1 < lNodeCount)
	{
		mNodes[pIndex0].AddNeighborIndex(pIndex1);
		mNodes[pIndex1].AddNeighborIndex(pIndex0);
	}
}

size_t Topology::GetNodeConnectionCount(size_t pIndex) const
{
	if (pIndex < mNodes.size())
	{
		return mNodes[pIndex].NeighborCount();
	}

	return SIZE_MAX;
}

const Node* Topology::GetNode(size_t pIndex) const
{
	if (pIndex < mNodes.size())
	{
		return &mNodes[pIndex];
	}

	return nullptr;
}

void Topology::ComputeLongestPath()
{
	std::vector<uint32_t> lNodeWeights;

	uint32_t lFirstNodeIndex = 0;
	uint32_t lLastNodeIndex = 0;

	//all node weights to 0
	lNodeWeights.resize(mNodes.size());
	memset(lNodeWeights.data(), 0, lNodeWeights.size() * sizeof(uint32_t));

	lNodeWeights[0] = 1;

	//Let's find first extremity
	ProcessDepth(0, lNodeWeights);

	uint32_t lLargestWeight = 0;
	uint32_t lLargestIndex = 0;
	for (uint32_t i = 0; i < lNodeWeights.size(); ++i)
	{
		if (lLargestWeight < lNodeWeights[i])
		{
			lLargestWeight = lNodeWeights[i];
			lLargestIndex = i;
		}
	}

	memset(lNodeWeights.data(), 0, lNodeWeights.size() * sizeof(uint32_t));

	lFirstNodeIndex = lLargestIndex;
	lNodeWeights[lFirstNodeIndex] = 1;

	//Let's find second extremity
	ProcessDepth(lFirstNodeIndex, lNodeWeights);


	lLargestWeight = 0;
	lLargestIndex = 0;
	for (uint32_t i = 0; i < lNodeWeights.size(); ++i)
	{
		if (lLargestWeight < lNodeWeights[i])
		{
			lLargestWeight = lNodeWeights[i];
			lLargestIndex = i;
		}
	}

	lLastNodeIndex = lLargestIndex;


	uint32_t lNodeWeight = lLargestWeight;
	uint32_t lCurrentNodeIndex = lLastNodeIndex;
	
	mLongestPath.clear();
	mLongestPath.push_back(lCurrentNodeIndex);

	while (lNodeWeight > 1)
	{
		const Node& lNode = mNodes[lCurrentNodeIndex];

		for (size_t i = 0; i < lNode.NeighborCount(); ++i)
		{
			uint32_t lNeighborIndex = lNode.GetNeighborIndex(i);

			if (lNodeWeight > lNodeWeights[lNeighborIndex])
			{
				lNodeWeight = lNodeWeights[lNeighborIndex];
				lCurrentNodeIndex = lNeighborIndex;
				mLongestPath.push_back(lCurrentNodeIndex);
				break;
			}
		}
	}
}

uint32_t Topology::GetLongestPathLength() const
{
	return mLongestPath.size();
}

const uint32_t* Topology::GetLongestPathIndices() const
{
	return mLongestPath.data();
}

void Topology::ProcessDepth(uint32_t pInitialNodeIndex, std::vector<uint32_t>& pNodeWeights)
{
	std::queue<uint32_t> lNodeIndicesToProcess;
	lNodeIndicesToProcess.push(pInitialNodeIndex);

	while (!lNodeIndicesToProcess.empty())
	{
		uint32_t lNodeIndex = lNodeIndicesToProcess.front(); lNodeIndicesToProcess.pop();

		const Node& lNode = mNodes[lNodeIndex];
		uint32_t lNodeWeight = pNodeWeights[lNodeIndex];

		for (size_t i = 0; i < lNode.NeighborCount(); ++i)
		{
			uint32_t lNeighborIndex = lNode.GetNeighborIndex(i);

			if (pNodeWeights[lNeighborIndex] == 0)
			{
				pNodeWeights[lNeighborIndex] = lNodeWeight + 1;
				lNodeIndicesToProcess.push(lNeighborIndex);
				//ProcessDepth(mNodes[lNeigborIndex], pNodeWeights[lNeigborIndex], pNodeWeights);
			}
		}
	}
}
