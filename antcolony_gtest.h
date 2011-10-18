#ifndef _ANT_COLONY_GTEST_H
#define _ANT_COLONY_GTEST_H

#include "data_file.h"
#include "antcolony.h"
#include "k-means.h"
#include "rand_bound.h"
#include "validate_gtest.h"
#include "gtest/gtest.h"

namespace ant_colony_gtest_h{
  using namespace hps;
  using namespace ambulance;
  TEST(AntColony, Initialization){
    enum { MaxHospitalCoord = 200, };
    VictimList victims;
    HospitalAmbulanceList hospitalAmbulances;
    const std::string filename("ambusamp2010");
    LoadDataFile(filename, &victims, &hospitalAmbulances);
    ActionSequenceList actionSequences;
    const int numHospitals = static_cast<int>(hospitalAmbulances.size());
    HospitalList hospitals(numHospitals);
    for (int hospitalIdx = 0; hospitalIdx < numHospitals; ++hospitalIdx)
    {
      hospitals[hospitalIdx].id = hospitalIdx + 1;
      hospitals[hospitalIdx].position.x = 1 + RandBound(MaxHospitalCoord);
      hospitals[hospitalIdx].position.y = 1 + RandBound(MaxHospitalCoord);
      hospitals[hospitalIdx].ambulances = hospitalAmbulances[hospitalIdx];
    }

    AntColony ants(victims, hospitals, &actionSequences);
    ASSERT_TRUE(ants.pheremoneMatrix.Size() == hospitalAmbulances.size() + victims.size());
  }

}
#endif //_ANT_COLONY_GTEST_H
