#include <cstdio>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>

#include "ambulance_core.h"
#include "greedy.h"
#include "k-means.h"
#include "rand_bound.h"
#include "data_file.h"
using namespace hps;

void PrintUsage()
{
  std::cout << "Usage: ./ambulance <filename>" << std::endl;
}

void SaveVictims(const std::string& filename, const int iterations)
{
  enum { KMeansIterations = 1000, };
  assert(iterations > 0);

  VictimList victims;
  HospitalAmbulanceList hospitalAmbulances;
  LoadDataFile(filename, &victims, &hospitalAmbulances);
  ActionSequenceList actionSequences;
  HospitalList hospitals;
  int mostRescued = -1;
  for (int iteration = 0; iteration < iterations; ++iteration)
  {
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
    KMeans<Point>::Run(k, KMeansIterations, 1, points,
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
    
    // Make k-means hospitals giving the most abulances to the largest clusters.
    hospitals.resize(k);
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
    int rescued;
    ActionSequenceList trialActionSeq;
    GreedyRescue::Run(victims, hospitals, &trialActionSeq, &rescued);
    if (rescued > mostRescued)
    {
      mostRescued = rescued;
      actionSequences = trialActionSeq;
    }
  }
  std::cout << ActionSequenceListFormatter(victims, hospitals, actionSequences)
            << std::endl;

}

int main(int argc, char* argv[])
{
  srand(static_cast<unsigned int>(time(NULL)));
  if(argc != 2)
  {
    PrintUsage();
  }
  else
  {
    enum { GreedyIterations = 500, };
    std::string filename(argv[1]);
    SaveVictims(filename, GreedyIterations);
  }
  return 0;
}
