#ifndef _HPS_AMBULANCE_KMEANS_H_
#define _HPS_AMBULANCE_KMEANS_H_
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <assert.h>
#include <cmath>

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
};

// a struct may end up improving the locality ref.
template <typename CoordinateType>
struct Cluster
{
  typedef Coordinates<CoordinateType> Point;
  Point center;
  std::vector<Point> samplePoints;
};

template <typename CoordinateType>
class ClusterCenters
{
 private:
  typedef Coordinates<CoordinateType> Point;
  std::vector<Point> centers;
  typedef Point& reference_type;
  typedef Point value_type;
 public:
  ClusterCenters(int num_centers);
  ClusterCenters();
  ~ClusterCenters();

  //calculates the nearest center to the point and updates the center coordinates.
  void FindNearestCenter(const Point& point, Point* center);
  
  //calculates Grid Distance between point and center.
  int GetGridDistance(const Point& point, const Point& center);
  
  //accessors.
  //Get coordinates of the center at an index.
  inline const reference_type GetCenterAt(int idx) const 
  {
    assert(idx < centers.size());
    return centers[idx];
  }

  //mutators.
  inline void SetCenterAt(int idx, const Point point)
  {
    assert(idx < centers.size());
    centers[idx] = point;
  }
};
#endif //_HPS_AMBULANCE_KMEANS_H_
