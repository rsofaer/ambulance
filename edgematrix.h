#ifndef _02_TSP_DISTANCE_MATRIX_H_
#define _02_TSP_DISTANCE_MATRIX_H_
#include "data_file.h"
#include <functional>
#include <cstdlib>

namespace hps
{
namespace tsp
{

/// <summary> Simple edge matrix struct. </summary>
template <typename WeightType>
struct EdgeMatrix
{
  typedef WeightType value_type;
  EdgeMatrix()
  : nodeCount(0UL),
    edgeArray(NULL)
  {};

  EdgeMatrix(const size_t nodeCount_)
  : nodeCount(nodeCount_),
    edgeArray(new WeightType[nodeCount * nodeCount])
  {};

  ~EdgeMatrix()
  {
    delete [] edgeArray;
  }

  inline void Reallocate(const size_t nodeCount_)
  {
    if (nodeCount_ != nodeCount)
    {
      delete [] edgeArray;
      nodeCount = nodeCount_;
      edgeArray = new WeightType[nodeCount * nodeCount];
    }
  }

  inline size_t Size() const
  {
    return nodeCount;
  }

  inline WeightType* Data()
  {
    return edgeArray;
  }

  inline const WeightType* Data() const
  {
    return edgeArray;
  }

  inline WeightType* GetRow(const size_t i)
  {
    return edgeArray + (i * nodeCount);
  }

  inline const WeightType* GetRow(const size_t i) const
  {
    return edgeArray + (i * nodeCount);
  }

  inline const WeightType& GetEdge(const size_t i, const size_t j) const
  {
    return *(GetRow(i) + j);
  }

  inline void SetEdge(const size_t i, const size_t j, const WeightType& value)
  {
    *(GetRow(i) + j) = value;
  }

  inline WeightType* operator[](const size_t i)
  {
    return GetRow(i);
  }

  inline const WeightType* operator[](const size_t i) const
  {
    return GetRow(i);
  }

  size_t nodeCount;
  WeightType* edgeArray;
};

template <typename NumericType>
inline NumericType AbsSubtract(const NumericType& lhs, const NumericType& rhs)
{
  if (lhs >= rhs)
  {
    return lhs - rhs;
  }
  else
  {
    return rhs - lhs;
  }
}

/// <summary> Compute the edge length between two nodes. </summary>
template <typename WeightType, typename SqrtType>
struct EdgeDistanceCalculator
  : public std::binary_function<DataFile::Location, DataFile::Location, WeightType>
{
  inline WeightType operator()(const DataFile::Location& from,
                               const DataFile::Location& to) const
  {
    const WeightType dx = static_cast<WeightType>(AbsSubtract(from.x, to.x));
    const WeightType dy = static_cast<WeightType>(AbsSubtract(from.y, to.y));
    const WeightType dz = static_cast<WeightType>(AbsSubtract(from.z, to.z));
    const WeightType distSq = (dx*dx) + (dy*dy) + (dz*dz);
    return static_cast<WeightType>(sqrt(static_cast<SqrtType>(distSq)));
  }
};

/// <summary> Compute the edge weights from the data file. </summary>
template <typename BinaryOperator, typename WeightType>
void EdgeMatrixFromDataFile(const DataFile& data,
                            BinaryOperator func,
                            EdgeMatrix<WeightType>* matrix)
{
  assert(matrix);
  matrix->Reallocate(data.nodes.size());
  // Only iterate lower triangle since upper triangle is a copy.
  const std::vector<DataFile::Location>& nodes = data.nodes;
  const size_t nodeCount = matrix->nodeCount;
  for (size_t row = 0; row < nodeCount; ++row)
  {
    for (size_t col = 0; col <= row; ++col)
    {
      const WeightType edgeCost = func(nodes[row], nodes[col]);
      // Set (i, j).
      matrix->SetEdge(row, col, edgeCost);
      // Set (j, i).
      matrix->SetEdge(col, row, edgeCost);
    }
  }
}

}
}

#endif //_02_TSP_DISTANCE_MATRIX_H_

