#ifndef _HPS_AMBULANCE_AMBULANCE_CORE_H_
#define _HPS_AMBULANCE_AMBULANCE_CORE_H_
#include <vector>

namespace hps
{
namespace ambulance
{

/// <summary> A victim in the initial environment. </summary>
struct Victim
{
  int x;
  int y;
  int timeToLive;
};

/// <summary> A victim during simulation. </summary>
struct SimVictim : public Victim
{
  SimVictim(const Victim& rhs)
    : Victim(rhs),
      simStatus(Status_Bleeding)
  {}
  enum Status
  {
    Status_Bleeding,
    Status_PickedUp,
    Status_Rescued,
    Status_Expired,
  };
  Status simStatus;
};

/// <summary> A hospital in the initial environment. </summary>
struct Hospital
{
  int x;
  int y;
  int ambulances;
};

/// <summary> An ambulance during simulation. </summary>
struct SimAmbulance
{
  int x;
  int y;
  std::vector<Victim*> pickedUp;
  int simTime;
};

typedef std::vector<Victim> VictimList;
typedef std::vector<Hospital> HospitalList;
typedef std::vector<SimVictim> SimVictimList;
typedef std::vector<SimAmbulance> SimAmbulanceList;

}
using namespace ambulance;
}

#endif //_HPS_AMBULANCE_AMBULANCE_CORE_H_

