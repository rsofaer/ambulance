#ifndef _HPS_ABULANCE_GREEDY_H_
#define _HPS_ABULANCE_GREEDY_H_
#include "ambulance_core.h"
#include "graph.h"
#include <vector>
#include <algorithm>
#include <limits>

namespace hps
{
namespace ambulance
{

struct GreedyRescue
{
  static void Run(const VictimList& victims, const HospitalList& hospitals,
                  ActionSequenceList* actionSequences);
};

}
using namespace ambulance;
}

#endif //_HPS_ABULANCE_GREEDY_H_

