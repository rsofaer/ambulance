#ifndef _HPS_AMBULANCE_GREEDY_GTEST_H_
#define _HPS_AMBULANCE_GREEDY_GTEST_H_
#include "greedy.h"
#include "rand_bound_generator.h"
#include "gtest/gtest.h"

namespace _hps_ambulance_greedy_gtest_h_
{
using namespace hps;

TEST(Run, Greedy)
{
  enum { MaxHospitalCoord = 200, };

  VictimList victims;
  HospitalAmbulanceList hospitalAmbulances;
  LoadDataFile("ambusamp2010", &victims, &hospitalAmbulances);
  // Make any old hospital.
  const int numHospitals = static_cast<int>(hospitalAmbulances.size());
  HospitalList hospitals(numHospitals);
  for (int hospitalIdx = 0; hospitalIdx < numHospitals; ++hospitalIdx)
  {
    hospitals[hospitalIdx].id = hospitalIdx + 1;
    hospitals[hospitalIdx].position.x = 1 + RandBound(MaxHospitalCoord);
    hospitals[hospitalIdx].position.y = 1 + RandBound(MaxHospitalCoord);
    hospitals[hospitalIdx].ambulances = hospitalAmbulances[hospitalIdx];
  }
  // Rescue people and print output format.
  ActionSequenceList actionSequences;
  GreedyRescue::Run(victims, hospitals, &actionSequences);
  std::cout << ActionSequenceListFormatter(victims, hospitals, actionSequences)
            << std::endl;
}

}

#endif //_HPS_AMBULANCE_GREEDY_GTEST_H_
