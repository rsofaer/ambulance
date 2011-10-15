#ifndef _02_TSP_GRAPH_H_
#define _02_TSP_GRAPH_H_
#include <vector>
#include <list>
#include <assert.h>

namespace hps
{
namespace tsp
{

/// <summary> An unweighted, directed graph.
template <typename NodeData>
struct Graph
{
public:
  /// <summary> A graph node. </summary>
  struct Node
  {
    Node() : data(), adjacencyList() {}
    Node(const NodeData& data_) : data(data_), adjacencyList() {}
    typedef std::vector<Node*> AdjacencyList;
    NodeData data;
    AdjacencyList adjacencyList;
  };

  Graph() : nodes() {}

  typedef std::vector<Node> NodeList;
  NodeList nodes;

private:
  // Disable copy and assign.
  Graph(const Graph&);
  Graph& operator=(const Graph&);
};

}
using namespace tsp;
}

#endif //_02_TSP_GRAPH_H_
