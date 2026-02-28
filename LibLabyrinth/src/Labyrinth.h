#pragma once

#include "LabyrinthApi.h"

#include <cstdint>

#if 0
enum class Algorithm
{
	WallBreakerStackBacktrack,
	WallBreakerQueueBacktrack,
	WallBreakerRandomBacktrack,
	WallBreakerBloom,
};
#endif

enum class RoomSelect//Maybe one to force choose a node with only one connection to generate corridor maze: the deepest maze ever
{
	Last,
	Fill
};

enum class Backtrack
{
	Stack,
	Queue,
	Random,
};

enum class ComputeDirection
{
	Any,
	ForceChange,
	//Always,//Default mode, for each node connected, get a random direction
	//ForceAlways,//Force change direction if this direction is not the only available
	//OnLock,//When it's not possible to dig anymore, creates long corridors
	//OnFixedLength,//When dig fixed count on a specific direction, force change
	//ForceOnFixedLength,
	//OnRandomLength,//Compute a new length when get a wall or reach the length 
	//ForceOnRandomLength,
};

struct GenerationParameters
{
	RoomSelect RoomSelectMode;
	Backtrack BacktrackMode;
	ComputeDirection ComputeDirectionMode;
	int32_t CorridorMinLength;
	int32_t CorridorMaxLength;
};

class Topology;
class RoomNeighborhood;
struct LabyrinthStepperId;
struct Seed;

class TopologyUpdaterListener
{
public:
	virtual void AddFirstNode(uint32_t pNodeIndex) = 0;
	virtual void AddEdge(uint32_t pNodeIndex0, uint32_t pNodeIndex1) = 0;

	virtual void UpdaterProcessCompleted(uint32_t pPathLength, const uint32_t* pPathIndices) = 0;
};

class LABYRINTH_API LabyrinthStepper
{
public:
	LabyrinthStepper(GenerationParameters pGenerationParameters);
	~LabyrinthStepper();

	void SetUpdateListener(TopologyUpdaterListener* pListener);

	void UpdateTopology(const Topology* pTopology, const RoomNeighborhood* pRoomNeighborhood);

	void UpdateAlgorithm(GenerationParameters pGenerationParameters);

	void InitiateGeneration(const Seed* pSeed = nullptr);

	void ProcessStep(uint32_t pConnectionCount, float pMaxTime);

	void ForceRedraw();

	const Topology* GetTopology() const;

	const Topology* GetBaseTopology() const;

private:
	LabyrinthStepperId* mId = nullptr;
};
