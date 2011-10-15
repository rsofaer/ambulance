#ifndef _HPS_AMBULANCE_AMBULANCE_CORE_GTEST_H_
#define _HPS_AMBULANCE_AMBULANCE_CORE_GTEST_H_
#include "ambulance_core.h"
#include "data_file.h"
#include "gtest/gtest.h"

namespace _hps_ambulance_ambulance_core_gtest_h_
{
using namespace hps;

TEST(CoreStructs, ambulance_core)
{
  Victim victim;
  {
    victim.position.x = 56;
    victim.position.y = 23;
    victim.timeToLive = 120;
  }
  SimVictim simVictim(victim);
  Hospital hospital;
  {
    hospital.position.x = 129;
    hospital.position.y = 75;
    hospital.ambulances = 1;
  }
  SimAmbulance simAmbulance;
  {
    simAmbulance.position.x = simVictim.position.x;
    simAmbulance.position.y = simVictim.position.y;
    simAmbulance.simTime = 0;
    simAmbulance.pickedUp.push_back(&simVictim);
  }
  VictimList victimList;
  HospitalList hospitalList;
  SimVictimList simVictimList;
  SimAmbulanceList simAmbulanceList;
}

TEST(LoadDataFile, ambulance_core)
{
  // First victim in file.
  const Point expectPtFirst(15, 33);
  const int expectTtlFirst = 91;
  // Last victim in file.
  const Point expectPtLast(67, 22);
  const int expectTtlLast = 165;
  {
    VictimList victims;
    HospitalAmbulanceList hospitalAmbulances;
    ASSERT_TRUE(LoadDataFile("ambusamp2010", &victims, &hospitalAmbulances));
    ASSERT_EQ(300, victims.size());
    ASSERT_EQ(5, hospitalAmbulances.size());
    // Verify victims.
    {
      EXPECT_EQ(expectPtFirst, victims.front().position);
      EXPECT_EQ(expectTtlFirst, victims.front().timeToLive);
      EXPECT_EQ(expectPtLast, victims.back().position);
      EXPECT_EQ(expectTtlLast, victims.back().timeToLive);
    }
    // Verify hospitals.
    {
      EXPECT_EQ(5, hospitalAmbulances.front());
      EXPECT_EQ(10, hospitalAmbulances.back());
    }
  }
}

}

#endif //_HPS_AMBULANCE_AMBULANCE_CORE_GTEST_H_
