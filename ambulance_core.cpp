#include "ambulance_core.h"
#include <fstream>

namespace hps
{
namespace ambulance
{

void BuildSimVictimGraph(SimVictimList* simVictims, SimVictimGraph* graph)
{
  assert(simVictims);
  assert(graph);

  // Load all victims into a fully connected graph.
  const int numVictims = static_cast<int>(simVictims->size());
  graph->nodes.resize(numVictims);
  SimVictimList::iterator victim = simVictims->begin();
  SimVictimGraph::NodeList::iterator node = graph->nodes.begin();
  for (;victim != simVictims->end(); ++victim, ++node)
  {
    // Connect all other nodes to this node.
    node->data = &*victim;
    SimVictimGraph::Node::AdjacencyList& adjacencyList = node->adjacencyList;
    adjacencyList.clear();
    for (SimVictimGraph::NodeList::iterator adjNode = graph->nodes.begin();
         adjNode != graph->nodes.end();
         ++adjNode)
    {
      if (adjNode == node)
      {
        continue;
      }
      else
      {
        adjacencyList.push_back(&*adjNode);
      }
    }
  }
}

inline std::ostream& operator<<(std::ostream& stream, const Point& point)
{
  stream << "(" << point.x << "," << point.y << ")";
  return stream;
}

void FormatActionSequenceList(const VictimList& victims,
                              const HospitalList& hospitals,
                              const ActionSequenceList& actionSequences,
                              std::ostream& stream)
{
  // Print hospital header line.
  stream << "Hospital" << std::endl;
  // Print hospital locations.
  for (HospitalList::const_iterator hospital = hospitals.begin();
       hospital != hospitals.end();
       ++hospital)
  {
    stream << hospital->id << ": " << hospital->position << std::endl;
  }
  // Print ambulance action sequence header line.
  stream << std::endl << "Ambulances";
  // Print action sequences.
  int ambulanceIdx = 0;
  for (ActionSequenceList::const_iterator seq = actionSequences.begin();
       seq != actionSequences.end();
       ++seq)
  {
    // Did this ambulance do no pickups?
    if (seq->size() <= 1)
    {
      continue;
    }
    ++ambulanceIdx;
    stream << std::endl << ambulanceIdx << ":";
    for (ActionSequence::const_iterator node = seq->begin();
         node != seq->end();
         ++node)
    {
      switch (node->stopType)
      {
      case ActionNode::StopType_Hospital:
        {
          const Hospital& hospital = hospitals[node->id - 1];
          stream << " H" << hospital.id << hospital.position;
        }
        break;
      case ActionNode::StopType_Victim:
        {
          const Victim& victim = victims[node->id - 1];
          stream << " P" << node->id << "(" << victim.position.x << ","
                 << victim.position.x << "," << victim.timeToLive << ")";
        }
        break;
      default:
        assert(false);
        break;
      }
    }
  }
}

}
}

