#include "gtest/gtest.h"

#include "../LibLabyrinth/src/Node.h"
#include "../LibLabyrinth/src/Topology.h"
#include "../LibLabyrinth/src/TopologyGenerator.h"

//Node
#define NODE_COUNT 49

#define DUMMY_INDEX 42
#define NEIGHBOR_INDEX_0 23
#define NEIGHBOR_INDEX_1 7
#define NEIGHBOR_INDEX_2 (NODE_COUNT + 47)//be sure to be outside the container

TEST(TestNode, TestDefaultConstruction)
{
	Node lNode;
	EXPECT_EQ(lNode.Index(), SIZE_MAX);
}

TEST(TestNode, TestConstruction)
{
	Node lNode(DUMMY_INDEX);
	EXPECT_EQ(lNode.Index(), DUMMY_INDEX);
}

TEST(TestNode, TestSetIndex)
{
	Node lNode(DUMMY_INDEX);
	EXPECT_EQ(lNode.Index(), DUMMY_INDEX);

	lNode.SetIndex(NEIGHBOR_INDEX_0);
	EXPECT_EQ(lNode.Index(), NEIGHBOR_INDEX_0);

	lNode.SetIndex(NEIGHBOR_INDEX_1);
	EXPECT_EQ(lNode.Index(), NEIGHBOR_INDEX_1);
}

TEST(TestNode, TestCopyConstruction)
{
	Node lDefaultNode;
	Node lNode(DUMMY_INDEX);

	Node lDefaultCopy = lDefaultNode;
	Node lCopy = lNode;

	EXPECT_EQ(lDefaultCopy.Index(), SIZE_MAX);
	EXPECT_EQ(lCopy.Index(), DUMMY_INDEX);
}

TEST(TestNode, TestNeigborCount)
{
	Node lNode(DUMMY_INDEX);

	EXPECT_EQ(lNode.NeighborCount(), 0);
	lNode.AddNeighborIndex(NEIGHBOR_INDEX_0);
	EXPECT_EQ(lNode.NeighborCount(), 1);
	lNode.AddNeighborIndex(NEIGHBOR_INDEX_1);
	EXPECT_EQ(lNode.NeighborCount(), 2);
}

TEST(TestNode, TestNeigborhood)
{
	Node lNode(DUMMY_INDEX);

	EXPECT_FALSE(lNode.IsConnected(NEIGHBOR_INDEX_0), 0);
	EXPECT_FALSE(lNode.IsConnected(NEIGHBOR_INDEX_1), 0);


	lNode.AddNeighborIndex(NEIGHBOR_INDEX_0);

	EXPECT_TRUE(lNode.IsConnected(NEIGHBOR_INDEX_0), 0);
	EXPECT_FALSE(lNode.IsConnected(NEIGHBOR_INDEX_1), 0);


	lNode.AddNeighborIndex(NEIGHBOR_INDEX_1);

	EXPECT_TRUE(lNode.IsConnected(NEIGHBOR_INDEX_0), 0);
	EXPECT_TRUE(lNode.IsConnected(NEIGHBOR_INDEX_1), 0);


	lNode.RemoveNeighborIndex(NEIGHBOR_INDEX_0);

	EXPECT_FALSE(lNode.IsConnected(NEIGHBOR_INDEX_0), 0);
	EXPECT_TRUE(lNode.IsConnected(NEIGHBOR_INDEX_1), 0);


	lNode.RemoveNeighborIndex(NEIGHBOR_INDEX_1);

	EXPECT_FALSE(lNode.IsConnected(NEIGHBOR_INDEX_0), 0);
	EXPECT_FALSE(lNode.IsConnected(NEIGHBOR_INDEX_1), 0);
}

TEST(TestNode, TestGetIndices)
{
	Node lNode(DUMMY_INDEX);
	lNode.AddNeighborIndex(NEIGHBOR_INDEX_0);
	lNode.AddNeighborIndex(NEIGHBOR_INDEX_1);

	EXPECT_EQ(lNode.NeighborCount(), 2);
	EXPECT_EQ(lNode.NeighborIndices()[0], NEIGHBOR_INDEX_0);
	EXPECT_EQ(lNode.NeighborIndices()[1], NEIGHBOR_INDEX_1);
}


TEST(TestTopology, TestConstructor)
{
	Topology lTopology(NODE_COUNT);

	EXPECT_EQ(lTopology.GetSize(), NODE_COUNT);
}

TEST(TestTopology, TestNodeConnection)
{
	Topology lTopology(NODE_COUNT);

	Topology lTopo2();

	lTopology.ConnectNodes(NEIGHBOR_INDEX_0, NEIGHBOR_INDEX_1);
	lTopology.ConnectNodes(NEIGHBOR_INDEX_0, NEIGHBOR_INDEX_2);

	EXPECT_TRUE(lTopology.AreConnected(NEIGHBOR_INDEX_0, NEIGHBOR_INDEX_1));
	EXPECT_TRUE(lTopology.AreConnected(NEIGHBOR_INDEX_1, NEIGHBOR_INDEX_0));
	EXPECT_FALSE(lTopology.AreConnected(NEIGHBOR_INDEX_0, NEIGHBOR_INDEX_2));
}

TEST(TestTopologyGenerator, TestSquareShapeGenerator)
{
	TopologyGenerator lTopologyGenerator;


	//  0-1-2
	//  | | |
	//  3-4-5
	//  | | |
	//  6-7-8

	const Topology* lGeneratedTopology = lTopologyGenerator.Generate(Shape::SquareRoomsOnRectangularSpace, 3, 3);

	EXPECT_TRUE(lGeneratedTopology->AreConnected(0, 1));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(1, 2));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(3, 4));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(4, 5));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(6, 7));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(7, 8));

	EXPECT_TRUE(lGeneratedTopology->AreConnected(0, 3));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(1, 4));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(2, 5));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(3, 6));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(4, 7));
	EXPECT_TRUE(lGeneratedTopology->AreConnected(5, 8));

	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(0), 2);
	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(1), 3);
	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(2), 2);

	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(3), 3);
	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(4), 4);
	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(5), 3);

	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(6), 2);
	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(7), 3);
	EXPECT_EQ(lGeneratedTopology->GetNodeConnectionCount(8), 2);

	lTopologyGenerator.Release(lGeneratedTopology);
}
