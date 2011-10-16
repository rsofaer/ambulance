#ifndef _HPS_AMBULANCE_KMEANS_GTEST_H_
#define _HPS_AMBULANCE_KMEANS_GTEST_H_
#include "k-means.h"
#include "gtest/gtest.h"

namespace _hps_ambulance_kmeans_gtest_h_
{
using namespace hps;

TEST(K_Means, k_means)
{
  enum { KMeansTestIterations = 1000, };
  {
    VictimList victims;
    HospitalAmbulanceList hospitalAmbulances;
    LoadDataFile("ambusamp2010", &victims, &hospitalAmbulances);
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
    KMeans<Point>::Run(k, KMeansTestIterations, 1, points,
                       std::ptr_fun(ManhattanDistance),
                       &means, &clusters);
  }
}

}

#endif //_HPS_AMBULANCE_KMEANS_GTEST_H_
