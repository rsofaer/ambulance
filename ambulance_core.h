#ifndef _HPS_AMBULANCE_AMBULANCE_CORE_H_
#define _HPS_AMBULANCE_AMBULANCE_CORE_H_
#include "graph.h"
#include <vector>
#include <math.h>
#include <iosfwd>
#include <cstdlib>

namespace hps
{
namespace ambulance
{

/// <summary> A position on the grid. </summary>
struct Point
{
  Point() : x(0), y(0) {}
  Point(const int x_, const int y_) : x(x_), y(y_) {}
  int x;
  int y;
};

/// <summary> Equality operator for a point. </summary>
inline bool operator==(const Point& lhs, const Point& rhs)
{
  return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

/// <summary> Addition operator for a point. </summary>
inline const Point operator+(const Point& lhs, const Point& rhs)
{
  return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

/// <summary> A victim in the initial environment. </summary>
struct Victim
{
  Point position;
  int timeToLive;
};

/// <summary> A victim during simulation. </summary>
struct SimVictim : public Victim
{
  SimVictim(const Victim& rhs)
    : Victim(rhs),
      id(-1),
      simStatus(Status_Bleeding)
  {}
  enum Status
  {
    Status_Bleeding,
    Status_PickedUp,
    Status_Rescued,
    Status_Expired,
  };
  int id;
  Status simStatus;
};

/// <summary> A hospital in the initial environment. </summary>
struct Hospital
{
  int id;
  Point position;
  int ambulances;
};

/// <summary> An ambulance during simulation. </summary>
struct SimAmbulance
{
  Point position;
  std::vector<Victim*> pickedUp;
  int simTime;
};

typedef std::vector<Victim> VictimList;
typedef std::vector<int> HospitalAmbulanceList;
typedef std::vector<Hospital> HospitalList;
typedef std::vector<SimVictim> SimVictimList;
typedef std::vector<SimAmbulance> SimAmbulanceList;

/// <summary> Compute Manhattan distance between points. </summary>
inline int ManhattanDistance(const Point& a, const Point& b)
{
  return abs(a.x - b.x) + abs(a.y - b.y);
}

// reissb -- 20111015 -- The graph functions are not needed after all.
//   I will leave them in here in case someone else needs them.
/// <summary> Graph structure to hold simulation victims. </summary>
typedef Graph<SimVictim*> SimVictimGraph;
/// <summary> Build a SimVictimGraph from a SimVictimList. </summary>
void BuildSimVictimGraph(SimVictimList* simVictims, SimVictimGraph* graph);

enum { VictimLoadTime = 1, };
enum { VictimUnloadTime = 1, };
enum { DriveOneBlockTime = 1, };

/// <summary> Ambulance action step. </summary>
struct ActionNode
{
  enum StopType { StopType_Undef, StopType_Victim, StopType_Hospital, };
  ActionNode() : id(0), stopType(StopType_Undef) {}
  ActionNode(const int id_, const StopType stopType_)
    : id(id_),
      stopType(stopType_)
  {}
  int id;
  StopType stopType;
};

typedef std::vector<ActionNode> ActionSequence;
typedef std::vector<ActionSequence> ActionSequenceList;

/// <summary> Output action sequence list. </summary>
void FormatActionSequenceList(const VictimList& victims,
                              const HospitalList& hospitals,
                              const ActionSequenceList& actionSequences,
                              std::ostream& stream);

/// <summary> Output action sequence list. </summary>
struct ActionSequenceListFormatter
{
  ActionSequenceListFormatter(const VictimList& victims_,
                              const HospitalList& hospitals_,
                              const ActionSequenceList& actionSequences_)
    : victims(&victims_),
      hospitals(&hospitals_),
      actionSequences(&actionSequences_)
  {}

  const VictimList* victims;
  const HospitalList* hospitals;
  const ActionSequenceList* actionSequences;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const ActionSequenceListFormatter& formatter)
{
  assert(formatter.victims && formatter.hospitals && formatter.actionSequences);
  FormatActionSequenceList(*formatter.victims, *formatter.hospitals,
                           *formatter.actionSequences, stream);
  return stream;
}


}
using namespace ambulance;
}

#endif //_HPS_AMBULANCE_AMBULANCE_CORE_H_
