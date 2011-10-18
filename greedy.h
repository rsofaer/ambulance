#ifndef _HPS_ABULANCE_GREEDY_H_
#define _HPS_ABULANCE_GREEDY_H_
#include "greedy_base.h"
#include "ambulance_core.h"
#include <vector>
#include <algorithm>
#include <limits>

namespace hps
{
namespace ambulance
{

struct GreedyRescue
{
  struct ManhattanDistInverseTTLScore
  {
    typedef float result_type;
    inline float operator()(const Point& a, const Victim& b)
    {
//      const float dist = static_cast<float>(ManhattanDistance(a, b.position));
//      const float timeMult = 1.0f / static_cast<float>(b.timeToLive);
      const float dist = static_cast<float>(ManhattanDistance(a, b.position));
      const float timeMult = static_cast<float>(b.timeToLive);
      return (dist * dist) * timeMult;
    }
  };
  inline static void Run(const VictimList& victims,
                         const HospitalList& hospitals,
                         ActionSequenceList* actionSequences)
  {
    //ManhattanDistanceScore scoreFunc;
    ManhattanDistInverseTTLScore scoreFunc;
    detail::GreedyBase::Run(victims, hospitals, &scoreFunc, actionSequences);
  }
};

/// <summary> Ant colony optimization using greedy backend. </summary>
struct AntColonyRescue
{
  /// <summary> Score using implicit interface of ScoreFunc. </summary>
  struct AntColonyScore
  {
    typedef float result_type;
    inline float operator()(const Point& a, const Victim& b)
    {
      return 1.0f;
    }
    // reissb -- 20111018 -- Place pointers to edge matrices, etc in here.
    //   These may be input params to Run() if you think that the client
    //   would like to inspect them.
  };

  static void Run(const VictimList& victims,
                  const HospitalList& hospitals,
                  ActionSequenceList* actionSequences)
  {
    // reissb -- 20111018 -- Place special logic wrapping greedy here.
    //   This should call greedy to run the ants using the proper
    //   scoring function.
    //
    //   This should go into greedy.cpp.
    //
    //   1) Initialize weights.
    //   2) Run greedy (ants) for N ants.
    //   3) Find best ant and update scoring.
    //   4) While there are still iterations, repeat 2-3.
  }
};

}
using namespace ambulance;
}

#endif //_HPS_ABULANCE_GREEDY_H_
