#ifndef _HPS_AMBULANCE_AMBULANCE_CORE_GTEST_H_
#define _HPS_AMBULANCE_AMBULANCE_CORE_GTEST_H_
#include "ambulance_core.h"
#include "gtest/gtest.h"

namespace _hps_ambulance_ambulance_core_gtest_h_
{
using namespace hps;

TEST(CoreStructs, ambulance_core)
{
  Victim victim;
  {
    victim.x = 56;
    victim.y = 23;
    victim.timeToLive = 120;
  }
  SimVictim simVictim(victim);
  Hospital hospital;
  {
    hospital.x = 129;
    hospital.y = 75;
    hospital.ambulances = 1;
  }
  SimAmbulance simAmbulance;
  {
    simAmbulance.x = simVictim.x;
    simAmbulance.y = simVictim.y;
    simAmbulance.simTime = 0;
    simAmbulance.pickedUp.push_back(&simVictim);
  }
  VictimList victimList;
  HospitalList hospitalList;
  SimVictimList simVictimList;
  SimAmbulanceList simAmbulanceList;
}

}

#endif //_HPS_AMBULANCE_AMBULANCE_CORE_GTEST_H_
