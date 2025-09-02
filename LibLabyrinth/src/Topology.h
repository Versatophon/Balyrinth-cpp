#pragma once

#include "LabyrinthApi.h"

#include "Node.h"

#include <cstddef>//size_t
#include <vector>

class LABYRINTH_API Topology
{
public:
	Topology() = delete;
	Topology(size_t pNodeCount);
	~Topology();

	void Clear();

	size_t GetSize() const;

	void ConnectNodes(size_t pIndex0, size_t pIndex1);

	/**
	* Get number of nodes connected
	* 
	* \param[in] pIndex Index of node
	* \return Number of nodes connected, SIZE_MAX if the index doesn't exist in this topology
	*/
	size_t GetNodeConnectionCount(size_t pIndex) const;

	const Node* GetNode(size_t pIndex) const;

	void ComputeLongestPath();
	uint32_t GetLongestPathLength() const;
	const uint32_t* GetLongestPathIndices() const;

private:
	std::vector<Node> mNodes;
	std::vector<uint32_t> mLongestPath;

	void ProcessDepth(uint32_t pInitialNodeIndex, std::vector<uint32_t>& pNodeWheights);

};

