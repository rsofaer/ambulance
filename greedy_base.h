#ifndef _HPS_AMBULANCE_GREEDY_BASE_H_
#define _HPS_AMBULANCE_GREEDY_BASE_H_

namespace hps
{
namespace ambulance
{

/// </summary> A simple Manhattan distance score. </summary>
struct ManhattanDistanceScore
{
  typedef int result_type;
  template <typename HasPositionType>
  inline int operator()(const Point& a, const HasPositionType& b) const
  {
    return ManhattanDistance(a, b.position);
  }
};

namespace detail
{

/// <summary> The core logic for greedy algorithms. </summary>
struct GreedyBase
{
  template <typename ScoreFunc>
  static void Run(const VictimList& victims,
                  const HospitalList& hospitals,
                  ScoreFunc* scoreFunc,
                  ActionSequenceList* actionSequences);
};

/// <summary> Records for sorting ambulances by simulation time. </summary>
struct AmbulanceMinHeapRecord
{
  AmbulanceMinHeapRecord() : ambulance(NULL), actionSequence(NULL) {}
  AmbulanceMinHeapRecord(SimAmbulance* ambulance_,
                         ActionSequence* actionSequence_)
   : ambulance(ambulance_),
     actionSequence(actionSequence_)
  {}
  SimAmbulance* ambulance;
  ActionSequence* actionSequence;
};

/// <summary> Sort order for ambulance min heap. </summary>
struct AmbulanceMinHeapOrder
{
  inline bool operator()(const AmbulanceMinHeapRecord& lhs,
                         const AmbulanceMinHeapRecord& rhs) const
  {
    return lhs.ambulance->simTime > rhs.ambulance->simTime;
  }
};

/// <summary> Remove edges to non-bleeding victims. </summary>
struct RemoveEdgeIfNotBleeding
{
  inline bool operator()(const SimVictimGraph::Node* simVictimEdge) const
  {
    if (simVictimEdge->data->simStatus != SimVictim::Status_Bleeding)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
};

/// <summary> Functor to get pointer to an object. </summary>
template <typename AnyType>
struct MakePointer
{
  inline AnyType* operator()(AnyType& obj) const { return &obj; }
};

/// <summary> Check if victim is still bleeding at the given time. </summary>
struct UpdateNotBleedingAtSimTime
{
  UpdateNotBleedingAtSimTime(const int simTime_) : simTime(simTime_) {}
  inline bool operator()(SimVictim* simVictim) const
  {
    switch (simVictim->simStatus)
    {
    case SimVictim::Status_Bleeding:
      {
        if (simVictim->timeToLive <= simTime)
        {
          simVictim->simStatus = SimVictim::Status_Expired;
          return true;
        }
        else
        {
          return false;
        }
      }
    case SimVictim::Status_Rescued:
    case SimVictim::Status_Expired:
      return true;
    default:
      assert(false && "Case not covered.");
      return true;
    }
  }
  int simTime;
};

/// <summary> Find object with best score near a given point. </summary>
template <typename HasPositionType, typename ScoreFunc>
struct ForEachFindBestScore
{
  typedef typename ScoreFunc::result_type ScoreType;
  ForEachFindBestScore(const Point& point_, ScoreFunc* scoreFunc_)
    : point(point_),
      scoreFunc(scoreFunc_),
      bestScore(std::numeric_limits<ScoreType>::max()),
      bestScored(NULL)
  {}
  ForEachFindBestScore(const ForEachFindBestScore& rhs)
    : point(rhs.point),
      scoreFunc(rhs.scoreFunc),
      bestScore(rhs.bestScore),
      bestScored(rhs.bestScored)
  {}
  inline void operator()(HasPositionType* hasPositionType)
  {
    const ScoreType score = (*scoreFunc)(point, *hasPositionType);
    if (score < bestScore)
    {
      bestScore = score;
      bestScored = hasPositionType;
    }
  }
  inline void operator()(HasPositionType& hasPositionType)
  {
    const int score = (*scoreFunc)(point, hasPositionType);
    if (score < bestScore)
    {
      bestScore = score;
      bestScored = &hasPositionType;
    }
  }
  Point point;
  ScoreFunc* scoreFunc;
  ScoreType bestScore;
  HasPositionType* bestScored;
};

/// <summary> Find bleeding victim near point with best score. </summary>
template <typename ScoreFunc>
struct ForEachFindBestScoreBleeding
  : public ForEachFindBestScore<SimVictim, ScoreFunc>
{
  ForEachFindBestScoreBleeding(const Point& point, ScoreFunc* scoreFunc)
    : ForEachFindBestScore<SimVictim, ScoreFunc>(point, scoreFunc)
  {}
  inline void operator()(SimVictim* simVictim)
  {
    // Filter based on bleeding only.
    if (SimVictim::Status_Bleeding == simVictim->simStatus)
    {
      ForEachFindBestScore<SimVictim, ScoreFunc>::operator()(simVictim);
    }
  }
private:
  ForEachFindBestScoreBleeding& operator=(const ForEachFindBestScoreBleeding&);
};

template <typename ScoreFunc>
void GreedyBase::Run(const VictimList& victims,
                     const HospitalList& hospitals,
                     ScoreFunc* scoreFunc,
                     ActionSequenceList* actionSequences)
{
  assert(actionSequences);

  typedef std::vector<detail::AmbulanceMinHeapRecord> AmbulanceHeap;
  typedef
    ForEachFindBestScoreBleeding<ScoreFunc>
    BestBleedingVictimFinder;
  typedef
    ForEachFindBestScore<const Hospital, ManhattanDistanceScore>
    BestHospitalFinder;

  // Need someone to rescue and something to pick them up.
  if (victims.empty() || hospitals.empty())
  {
    return;
  }

  // Create simulation victims and graph.
  SimVictimList simVictims;
  simVictims.reserve(victims.size());
  {
    int victimId = 1;
    for (VictimList::const_iterator victim = victims.begin();
         victim != victims.end();
         ++victim, ++victimId)
    {
      simVictims.push_back(SimVictim(*victim));
      simVictims.back().id = victimId;
    }
  }
  std::vector<SimVictim*> bleedingVictims(simVictims.size());
  std::transform(simVictims.begin(), simVictims.end(), bleedingVictims.begin(),
                 detail::MakePointer<SimVictim>());
  // Create all ambulances for all hospitals.
  SimAmbulanceList simAmbulances;
  actionSequences->clear();
  {
    int hospitalIdx = 1;
    for (HospitalList::const_iterator hospital = hospitals.begin();
         hospital != hospitals.end();
         ++hospital, ++hospitalIdx)
    {
      assert(hospital->id > 0);

      // Create all ambulances for this hospital.
      ActionNode actionNode(hospitalIdx, ActionNode::StopType_Hospital);
      for (int ambulanceIdx = 0;
        ambulanceIdx < hospital->ambulances;
        ++ambulanceIdx)
      {
        simAmbulances.push_back(SimAmbulance());
        SimAmbulance& ambulance = simAmbulances.back();
        {
          ambulance.position = hospital->position;
          ambulance.simTime = 0;
        }
        actionSequences->push_back(ActionSequence());
        actionSequences->back().push_back(actionNode);
      }
    }
  }
  // Create min heap for ambulance simulation time.
  const int numAmbulances = static_cast<int>(simAmbulances.size());
  AmbulanceHeap ambulanceHeap;
  ambulanceHeap.reserve(numAmbulances);
  {
    SimAmbulanceList::iterator ambulance = simAmbulances.begin();
    ActionSequenceList::iterator actionSequence = actionSequences->begin();
    for (; ambulance != simAmbulances.end(); ++ambulance, ++actionSequence)
    {
      assert(actionSequences->end() != actionSequence);
      ambulanceHeap.push_back(detail::AmbulanceMinHeapRecord(&*ambulance,
                                                             &*actionSequence));
    }
  }
  // reissb -- 20111018 -- The order to the heap may be randomized initially.
  //   However, this does not show a definitive improvement.
  //std::random_shuffle(ambulanceHeap.begin(), ambulanceHeap.end());
  // Greedy search.
  int rescued = 0;
  do
  {
    // Get the ambulance with the smallest time.
    detail::AmbulanceMinHeapRecord ambulanceRecord = ambulanceHeap.front();
    SimAmbulance* ambulance = ambulanceRecord.ambulance;
    ActionSequence* actionSequence = ambulanceRecord.actionSequence;
    assert(ambulance->pickedUp.empty());
    std::pop_heap(ambulanceHeap.begin(), ambulanceHeap.end(),
                  detail::AmbulanceMinHeapOrder());
    ambulanceHeap.pop_back();
    // Try to pickup 4 victims.
    int pickupTime = ambulance->simTime;
    int returnTime = 0;
    const Hospital* returnHospital = NULL;
    int mostCritialVictimTime = std::numeric_limits<int>::max();
    int victimsPickedUp = 0;
    for (; victimsPickedUp < 4; ++victimsPickedUp)
    {
      // Find best scored bleeding victim to this ambulance.
      BestBleedingVictimFinder bestVictim =
        std::for_each(bleedingVictims.begin(), bleedingVictims.end(),
                      BestBleedingVictimFinder(ambulance->position, scoreFunc));
      assert(NULL != bestVictim.bestScored);
      SimVictim* const pickupVictim = bestVictim.bestScored;
      // See if this person may be picked up without death.
      static ManhattanDistanceScore s_manhattanScore;
      BestHospitalFinder bestHospital =
        std::for_each(hospitals.begin(), hospitals.end(),
                      BestHospitalFinder(pickupVictim->position,
                                         &s_manhattanScore));
      // Estimated time to pickup.
      const int victimDist = ManhattanDistance(ambulance->position,
                                               pickupVictim->position);
      const int pickupThisVictimTime = VictimLoadTime +
                                       (victimDist * DriveOneBlockTime);
      const int hospitalDist = bestHospital.bestScore;
      const int returnFromVictimtime = VictimUnloadTime +
                                       (hospitalDist * DriveOneBlockTime);
      // See if this victim will make it.
      const int newRouteTime = pickupTime +
                               pickupThisVictimTime + returnFromVictimtime;
      // Can we pick this fella up?
      const int victimTimeToLive = pickupVictim->timeToLive;
      if ((newRouteTime <= victimTimeToLive) &&
          (newRouteTime <= mostCritialVictimTime))
      {
        // Add this victim pickup and set new return time.
        pickupTime += pickupThisVictimTime;
        returnTime = returnFromVictimtime;
        mostCritialVictimTime = std::min(mostCritialVictimTime,
                                         victimTimeToLive);
        // Pickup victim and update ambulance positon.
        pickupVictim->simStatus = SimVictim::Status_Rescued;
        ++rescued;
        ambulance->pickedUp.push_back(pickupVictim);
        ambulance->position = pickupVictim->position;
        actionSequence->push_back(ActionNode(pickupVictim->id,
                                             ActionNode::StopType_Victim));
        // Record hospital.
        returnHospital = bestHospital.bestScored;
      }
      // We can't pickup the closest so lets just assume that we can't get
      // anyone else even though there may be a pickup reachable by considering
      // the distance to the hospitals at the same time as the distance to the
      // victims.
      else
      {
        break;
      }
    }
    // If we picked someone up, then update state.
    if (victimsPickedUp > 0)
    {
      // Place ambulance at pickup hospital.
      ambulance->position = returnHospital->position;
      ambulance->pickedUp.clear();
      actionSequence->push_back(ActionNode(returnHospital->id,
                                           ActionNode::StopType_Hospital));
      // Update ambulance clock only.
      ambulance->simTime = pickupTime + returnTime;
      // Place this ambulance back into the simulation.
      ambulanceHeap.push_back(ambulanceRecord);
      std::push_heap(ambulanceHeap.begin(), ambulanceHeap.end(),
                     detail::AmbulanceMinHeapOrder());
    }
    // Update the global simulation time to the ambulance that is furthest
    // in the past.
    if(!ambulanceHeap.empty())
    {
      const int simTime = ambulanceHeap.front().ambulance->simTime;
      // Remove victims that are not bleeding at this time.
      bleedingVictims.erase(std::remove_if(bleedingVictims.begin(),
                                           bleedingVictims.end(),
                                           detail::UpdateNotBleedingAtSimTime(simTime)),
                            bleedingVictims.end());
    }
  } while (!bleedingVictims.empty() && !ambulanceHeap.empty());
//  // Kill remaining victims.
//  for (std::vector<SimVictim*>::iterator deadVictim = bleedingVictims.begin();
//       deadVictim != bleedingVictims.end();
//       ++deadVictim)
//  {
//    (*deadVictim)->simStatus = SimVictim::Status_Expired;
//  }
//  bleedingVictims.clear();
}

}
}
using namespace ambulance;
}

#endif //_HPS_AMBULANCE_GREEDY_BASE_H_
