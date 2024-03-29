#ifndef _HPS_AMBULANCE_KMEANS_H_
#define _HPS_AMBULANCE_KMEANS_H_
#include "rand_bound.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include <limits>
#include <assert.h>
#include <omp.h>

namespace hps
{
namespace clustering
{

template <typename PointType>
struct KMeans
{
  typedef std::vector<PointType> PointList;
  typedef std::vector<PointList> ClusterList;

  /// <summary> Compute the mean for a set of points. </summary>
  struct ComputeMean
  {
    inline const PointType operator()(const PointList& cluster) const
    {
      // reissb -- 20111016 -- This line requires operator+ for the PointType.
      //   If this is not possible, then one could extend this template to
      //   take an addition function as well as a distance function.
      const PointType sumPoint = std::accumulate(cluster.begin(), cluster.end(),
                                                 PointType(0, 0));
      const int clusterSize = static_cast<int>(cluster.size());
      return PointType(sumPoint.x / clusterSize, sumPoint.y / clusterSize);
    }
  };

  /// <summary> Run k-means clustering with given distance function. </summary>
  template <typename DistanceFunc>
  static void Run(const int k, const int iterations,
                  const typename DistanceFunc::result_type deltaDistStable,
                  const PointList& points, const DistanceFunc& distanceFunc,
                  PointList* means, ClusterList* clusters);

};

template <typename PointType>
template <typename DistanceFunc>
void KMeans<PointType>:: Run(const int k, const int iterations,
                             const typename DistanceFunc::result_type deltaDistStable,
                             const PointList& points,
                             const DistanceFunc& distanceFunc,
                             PointList* means, ClusterList* clusters)
{
  assert(means && clusters);
  assert(k > 0);
  assert(k <= static_cast<int>(points.size()));
  // reissb -- 20111016 -- K-means algorithm
  //   1) Select k random initial cluster centers using input points.
  //   2) For N iterations:
  //     i)   For each point, identify the closest cluster center.
  //     ii)  For each collection of closest points, update center k_i with
  //          the new center (either a point or the mean of the cluster).

  // Select k random clusters.
  clusters->clear();
  clusters->resize(k);
  RandBoundedGenerator randClusterGenerator(k);
  for (typename PointList::const_iterator point = points.begin();
       point != points.end();
       ++point)
  {
    const int clusterIdx = randClusterGenerator();
    clusters->at(clusterIdx).push_back(*point);
  }
  // Allocate memory for iterations.
  typedef std::pair<typename DistanceFunc::result_type, int> DistanceMeanPair;
  typedef std::vector<DistanceMeanPair> DistanceMeanPairList;
  const int numPoints = static_cast<int>(points.size());
  DistanceMeanPairList closestMeans(numPoints);
  PointList prevMeans(k, PointType(0, 0));
  std::vector<typename DistanceFunc::result_type> meanDeltas(k);
  means->resize(k);
  // Iterate clusters.
  for(int iteration = 0; iteration < iterations; ++iteration)
  {
    // Make sure that there are no empty clusters.
    for (typename ClusterList::iterator cluster = clusters->begin();
         cluster != clusters->end();
         ++cluster)
    {
      if (cluster->empty())
      {
        // Steal a point from another cluster.
        for (;;)
        {
          const int stealClusterIdx = RandBound(k - 1);
          typename ClusterList::iterator takePt = clusters->begin() + stealClusterIdx;
          takePt += (takePt == cluster);
          if (takePt->size() > 1)
          {
            cluster->push_back(takePt->back());
            takePt->pop_back();
            break;
          }
        }
      }
    }
    // reissb -- 20111016 -- Mean updating may be parallelized, but it
    //   should be verified that it gives a true speedup since it is not
    //   computationally expensive unless N is very large.
    // Update means.
    std::transform(clusters->begin(), clusters->end(),
                   means->begin(), ComputeMean());
    // See if we have reached a stable iteration.
    {
      std::transform(means->begin(), means->end(), prevMeans.begin(),
                     meanDeltas.begin(), distanceFunc);
      const typename DistanceFunc::result_type deltaDist =
        std::accumulate(meanDeltas.begin(), meanDeltas.end(), 0);
      if (deltaDist <= deltaDistStable)
      {
        return;
      }
      prevMeans = *means;
    }
    // Clear the clusters.
    std::for_each(clusters->begin(), clusters->end(),
                  std::mem_fun_ref(&PointList::clear));
    // For each point, find the closest mean and add to the cluster.
#pragma omp parallel for schedule(static, 100)
    for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
      // Find closest mean.
      DistanceMeanPair& closestMean = closestMeans.at(pointIdx);
      closestMean = std::make_pair(std::numeric_limits<typename DistanceMeanPair::first_type>::max(),
                                   std::numeric_limits<typename DistanceMeanPair::second_type>::max());
      const PointType& point = points.at(pointIdx);
      int meanIdx = 0;
      for (typename PointList::const_iterator mean = means->begin();
           mean != means->end();
           ++mean, ++meanIdx)
      {
        const typename DistanceFunc::result_type distance = distanceFunc(point, *mean);
        if (distance < closestMean.first)
        {
          closestMean.first = distance;
          closestMean.second = meanIdx;
        }
      }
    }
    // Insert points into clusters.
    {
      typename PointList::const_iterator point = points.begin();
      for (typename DistanceMeanPairList::const_iterator closestMean = closestMeans.begin();
           closestMean != closestMeans.end();
           ++closestMean, ++point)
      {
        clusters->at(closestMean->second).push_back(*point);
      }
    }
  }
  // Compute final means.
  std::transform(clusters->begin(), clusters->end(),
                 means->begin(), ComputeMean());
}

}
using namespace clustering;
}

#endif //_HPS_AMBULANCE_KMEANS_H_
