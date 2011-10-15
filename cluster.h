#ifndef _HPS_AMBULANCE_CLUSTER_H_
#define _HPS_AMBULANCE_CLUSTER_H_
#include <vector>
#include <algorithm>
#include <assert.h>

template <typename FeatureType>
struct Features
{
  int id;
  FeatureType value;// it could be a std::vector in svm light format, or just prices in dollars or heights and weights of people/animals. We don't need it but just in case.
};

template <typename CoordinateType>
struct Coordinates
{
  CoordinateType x;
  CoordinateType y;
  Coordinates()
  : x(0),
    y(0)
  {}
};

// a struct may end up improving the locality ref.
template <typename CoordinateType, typename DistanceType>
class Cluster
{
 private:
  typedef Coordinates<CoordinateType> Point;
  Point m_center;
 
  struct ClusterPointInfo
  {
    ClusterPointInfo()
    : index(0),
      distanceFromCenter(0)
    {}
  
    ClusterPointInfo(int index_, DistanceType distance_)
    : index(index_),
      distanceFromCenter(distance_)
    {}
    
    int index;
    DistanceType distanceFromCenter;
  };
  
  typedef std::vector<ClusterPointInfo> ClusterPointsInfo;
  ClusterPointsInfo m_clusterPoints ;
 
protected:
  void UpdateClusterCenter(const Point& newCenter)
  {
    m_center = newCenter;
  }
  
  void PushPair(int idx, DistanceType distance)
  {
    assert(idx >= 0 && idx < m_clusterPoints.size());
    ClusterPointInfo clusterPointInfo(idx,distance);
    m_clusterPoints.push_back(clusterPointInfo);
  }
 
 public:
  Cluster()
  : m_center(),
    m_clusterPoints()
  {}
  int ClusterSize() {return m_clusterPoints.size();}
  Point GetClusterMean();
};

#endif //_HPS_AMBULANCE_CLUSTER_H_
