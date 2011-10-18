#ifndef _HPS_AMBULANCE_KMEANS_GTEST_H_
#define _HPS_AMBULANCE_KMEANS_GTEST_H_
#include "k-means.h"
#include "ambulance_core.h"
#include "rand_bound.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <numeric>
#include <limits>

namespace _hps_ambulance_kmeans_gtest_h_
{
using namespace hps;

TEST(KMeansFixedData, k_means)
{
  enum { KMeansIterations = 1000, };
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
    KMeans<Point>::Run(k, KMeansIterations, 1, points,
                       std::ptr_fun(ManhattanDistance),
                       &means, &clusters);
  }
}

struct KMeansGaussianDataHelpers
{
  /// <summary> Generate Gaussian distributed cluster points. </summary>
  struct ClusterGenerator
  {
    /// <summary> Generate a Gaussian distributed point around center with
    ///   independent x,y variances.
    /// </summary>
    Point operator()() const
    {
      return Point(static_cast<int>(RatioOfUniforms(center.x, variances.x)),
                   static_cast<int>(RatioOfUniforms(center.y, variances.y)));
    }
    Point center;
    Point variances;
  };

  inline static int GetX(const Point& point)
  {
    return point.x;
  }

  inline static int GetY(const Point& point)
  {
    return point.y;
  }

  template <typename GetCoordFunc>
  static bool CheckClusterSeparation(const Point* testVariances,
                                     const ClusterGenerator* clusters,
                                     const int clusterIdx,
                                     const int clusterDistSig,
                                     const GetCoordFunc getCoordFunc)
  {
    const int testCoord = getCoordFunc(clusters[clusterIdx].center);
    for (int sepChkIdx = 0; sepChkIdx < clusterIdx; ++sepChkIdx)
    {
      // Make sure that this cluster is far enough away.
      const int minDist = clusterDistSig * getCoordFunc(testVariances[sepChkIdx]);
      if (minDist > abs(testCoord - getCoordFunc(clusters[sepChkIdx].center)))
      {
        return false;
      }
    }
    return true;
  }
};

TEST(KMeansGaussianData, k_means)
{
  // reissb -- 20111017 -- Generate Gaussian distributed clusters in
  //   rectangular regions of the plane. Try to recover the centers
  //   used to generate the clusters with K-Means.
  //
  //   This test is not giving reliable results. Perhaps this is the
  //   fundamental shortcoming of K-Means. Perhaps it is a bug. I think
  //   that I may have to wait for K-Means++ to find out.
  enum { KMeansGaussianDataIterations = 10, };
  enum { KMeansIterations = 1000, };
  const Point TestVariances[] = { Point(4, 4), Point(4, 4),
                                  Point(4, 4), Point(4, 4) };
  enum { K = sizeof(TestVariances) / sizeof(TestVariances[0]), };
  enum { ClusterDistSig = 20, };
  enum { MaxCoord = 1000, };
  enum { MinClusterSize = 20, };
  enum { MaxClusterSize = 30, };
  enum { MaxDistError = 10, };
  KMeansGaussianDataHelpers::ClusterGenerator clusterGenerators[K];
  RandBoundedGenerator randPtGen(MaxCoord);
  for (int iteration = 0; iteration < KMeansGaussianDataIterations; ++iteration)
  {
    // Select first cluster center.
    clusterGenerators[0].center = Point(1 + randPtGen(),
                                        1 + randPtGen());
    clusterGenerators[0].variances =  TestVariances[0];
    // Generate K-1 cluster center points by making sure that clusters are spaced
    // beyond ClusterDistSig sigmas apart.
    for (int clusterIdx = 1; clusterIdx < K; ++clusterIdx)
    {
      Point& center = clusterGenerators[clusterIdx].center;
      clusterGenerators[clusterIdx].variances = TestVariances[clusterIdx];
      do
      {
        center = Point(1 + randPtGen(), 1 + randPtGen());
               // x is good
      } while (!KMeansGaussianDataHelpers::
                  CheckClusterSeparation(TestVariances,
                                         clusterGenerators,
                                         clusterIdx,
                                         ClusterDistSig,
                                         std::ptr_fun(&KMeansGaussianDataHelpers::GetX)) &&
               // or y is good
               !KMeansGaussianDataHelpers::
                  CheckClusterSeparation(TestVariances,
                                         clusterGenerators,
                                         clusterIdx,
                                         ClusterDistSig,
                                         std::ptr_fun(&KMeansGaussianDataHelpers::GetY)));
    }
    // Generate random points for random cluster sizes.
    KMeans<Point>::PointList points;
    for (int clusterIdx = 0; clusterIdx < K; ++clusterIdx)
    {
      const int clusterSize = MinClusterSize + RandBound(MaxClusterSize - MinClusterSize + 1);
      const int startIdx = static_cast<int>(points.size());
      points.resize(startIdx + clusterSize);
      std::generate(points.begin() + startIdx, points.end(),
                    clusterGenerators[clusterIdx]);
    }
    std::random_shuffle(points.begin(), points.end());
    // Run k-means.
    KMeans<Point>::PointList means;
    KMeans<Point>::ClusterList clusters;
    KMeans<Point>::Run(K, KMeansIterations, 0, points,
                       std::ptr_fun(ManhattanDistance),
                       &means, &clusters);
    // Make sure that clusters were recovered.
    std::vector<int> meanRecovered(K, 1);
    std::vector<std::pair<Point, Point> > centerMeanPairs;
    for (int clusterIdx = 0; clusterIdx < K; ++clusterIdx)
    {
      int minDist = ManhattanDistance(clusterGenerators[clusterIdx].center,
                                      means[0]);
      int minDistMeanIdx = 0;
      for (int meanIdx = 1; meanIdx < K; ++meanIdx)
      {
        // Find closest mean.
        const int dist = ManhattanDistance(clusterGenerators[clusterIdx].center,
                                           means[meanIdx]);
        if (dist < minDist)
        {
          minDist = dist;
          minDistMeanIdx = meanIdx;
        }
      }
      // Mark off mean.
      centerMeanPairs.push_back(std::make_pair(clusterGenerators[clusterIdx].center,
                                               means[minDistMeanIdx]));
      EXPECT_LT(minDist, MaxDistError);
      if (minDist < MaxDistError)
      {
        --meanRecovered[minDistMeanIdx];
      }
    }
    const int numRecovered = std::count_if(meanRecovered.begin(),
                                           meanRecovered.end(),
                                           std::bind2nd(std::less_equal<int>(), 0));
//    if (numRecovered < K)
//    {
//      bool chk = true;
//    }
    EXPECT_EQ(static_cast<int>(K), numRecovered);
  }
}

}

#endif //_HPS_AMBULANCE_KMEANS_GTEST_H_
