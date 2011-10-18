#ifndef _HPS_AMBULANCE_GREEDY_GTEST_H_
#define _HPS_AMBULANCE_GREEDY_GTEST_H_
#include "greedy.h"
#include "k-means.h"
#include "rand_bound.h"
#include "validate_gtest.h"
#include "gtest/gtest.h"

namespace _hps_ambulance_greedy_gtest_h_
{
using namespace hps;

TEST(RandomHospitals, Greedy)
{
  enum { MaxHospitalCoord = 200, };

  VictimList victims;
  HospitalAmbulanceList hospitalAmbulances;
  const std::string filename("ambusamp2010");
  LoadDataFile(filename, &victims, &hospitalAmbulances);
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
//  std::cout << ActionSequenceListFormatter(victims, hospitals, actionSequences)
//            << std::endl;
  int numRescued;
  ASSERT_TRUE(ValidateAmbulance(victims, hospitals, actionSequences,
                                &numRescued));
  std::cout << "Rescued " << numRescued << " victims for input file "
            << filename << "." << std::endl;
}

void KMeansGreedyTest(const std::string& filename, const int iterations,
                      int* numRescued)
{
  assert(iterations > 0);

  VictimList victims;
  HospitalAmbulanceList hospitalAmbulances;
  LoadDataFile(filename, &victims, &hospitalAmbulances);
  // Get points and k.
  KMeans<Point>::PointList points;
  points.reserve(victims.size());
  for (VictimList::const_iterator victim = victims.begin();
       victim != victims.end();
       ++victim)
  {
    points.push_back(victim->position);
  }
  const int k = static_cast<int>(hospitalAmbulances.size());
  // Run k-means.
  KMeans<Point>::PointList means;
  KMeans<Point>::ClusterList clusters;
  KMeans<Point>::Run(k, iterations, 1, points,
                     std::ptr_fun(ManhattanDistance),
                     &means, &clusters);
  // Sort clusters and hospitals based on size.
  std::vector<std::pair<size_t, int> > clusterSortList(k);
  std::vector<std::pair<int, int> > hospitalSortList(k);
  for (int clusterIdx = 0; clusterIdx < k; ++clusterIdx )
  {
    clusterSortList[clusterIdx] = std::make_pair(clusters[clusterIdx].size(),
                                                 clusterIdx);
    hospitalSortList[clusterIdx] = std::make_pair(hospitalAmbulances[clusterIdx],
                                                  clusterIdx);
  }
  std::sort(clusterSortList.begin(), clusterSortList.end());
  std::sort(hospitalSortList.begin(), hospitalSortList.end());
  // reissb -- 20111018 -- Does not seem to affect solution if hospitals are
  //   assigned in decreasing order.
  //std::sort(hospitalSortList.begin(), hospitalSortList.end(), std::greater<std::pair<int, int> >());
  // Make k-means hospitals giving the most abulances to the largest clusters.
  HospitalList hospitals(k);
  for (int clusterIdx = 0; clusterIdx < k; ++clusterIdx )
  {
    const std::pair<size_t, int>& clusterRecord = clusterSortList[clusterIdx];
    const std::pair<int, int>& hospitalRecord = hospitalSortList[clusterIdx];
    const int hospitalIdx = hospitalRecord.second;
    hospitals[hospitalIdx].id = hospitalIdx + 1;
    hospitals[hospitalIdx].position = means[clusterRecord.second];
    hospitals[hospitalIdx].ambulances = hospitalRecord.first;
  }
  // Rescue people and print output format.
  ActionSequenceList actionSequences;
  GreedyRescue::Run(victims, hospitals, &actionSequences);
//  std::cout << ActionSequenceListFormatter(victims, hospitals, actionSequences)
//            << std::endl;
  ASSERT_TRUE(ValidateAmbulance(victims, hospitals, actionSequences,
                                numRescued));
  std::cout << "Rescued " << *numRescued << " victims for input file "
            << filename << "." << std::endl;
}

struct GreedyRunStats
{
  GreedyRunStats()
    : minSaved(std::numeric_limits<int>::max()),
      maxSaved(std::numeric_limits<int>::min()),
      totalSaved(0)
  {}
  int minSaved;
  int maxSaved;
  int totalSaved;
};

TEST(KMeansHospitals, Greedy)
{
  enum { KMeansTestIterations = 1000, };
  enum { KMeansRepeatFile = 20, };
  {
    GreedyRunStats runStats;
    const std::string filename("ambusamp2010");
    for (int repeat = 0; repeat < KMeansRepeatFile; ++repeat)
    {
      int numRescued;
      KMeansGreedyTest(filename, KMeansTestIterations, &numRescued);
      runStats.totalSaved += numRescued;
      runStats.minSaved = std::min(runStats.minSaved, numRescued);
      runStats.maxSaved = std::max(runStats.maxSaved, numRescued);
    }
    std::cout << "After " << KMeansRepeatFile << " repeats on input file "
              << filename
              << "rescued {min, max, avg} = {"
              << runStats.minSaved << ", "
              << runStats.maxSaved << ", "
              << runStats.totalSaved / KMeansRepeatFile << "}"
              << std::endl;
  }
  {
    GreedyRunStats runStats;
    const std::string filename("ambusamp2009");
    for (int repeat = 0; repeat < KMeansRepeatFile; ++repeat)
    {
      int numRescued;
      KMeansGreedyTest(filename, KMeansTestIterations, &numRescued);
      runStats.totalSaved += numRescued;
      runStats.minSaved = std::min(runStats.minSaved, numRescued);
      runStats.maxSaved = std::max(runStats.maxSaved, numRescued);
    }
    std::cout << "After " << KMeansRepeatFile << " repeats on input file "
              << filename
              << "rescued {min, max, avg} = {"
              << runStats.minSaved << ", "
              << runStats.maxSaved << ", "
              << runStats.totalSaved / KMeansRepeatFile << "}"
              << std::endl;
  }
}

}

#endif //_HPS_AMBULANCE_GREEDY_GTEST_H_
