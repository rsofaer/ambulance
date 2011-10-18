#ifndef _MATH_RAND_BOUND_GENERATOR_H_
#define _MATH_RAND_BOUND_GENERATOR_H_
#include <math.h>
#include <cstdlib>

namespace hps
{
namespace math
{

/// <summary> Partition consecutive intervals of size bound mapped to the
///   numbers [0, bound - 1].
/// </summary>
int RandBound(const int bound)
{
  assert(bound <= RAND_MAX);
  int factor = ((RAND_MAX - bound) / bound) + 1;
  int limit = factor * bound;
  int r;
  do
  {
    r = rand();
  } while (r >= limit);
  return r / factor;
}

/// <summary> Partition consecutive intervals of size bound mapped to the
///   numbers [0, bound - 1].
/// </summary>
struct RandBoundedGenerator
{
  RandBoundedGenerator(const int bound_)
  : bound(bound_),
    factor(((RAND_MAX - bound) / bound) + 1),
    limit(factor * bound)
  {
    assert(bound <= RAND_MAX);
  }

  inline int operator()() const
  {
    int r;
    do
    {
      r = rand();
    } while (r >= limit);
    return r / factor;
  }

  int bound;
  int factor;
  int limit;
};

inline double RandUniform()
{
  static RandBoundedGenerator s_rand(RAND_MAX);
  return static_cast<double>(s_rand()) / static_cast<double>(RAND_MAX - 1);
}

/// <summary> Generate values from a normal distribution using ratio of uniforms. </summary>
/// <remarks>
///   <para> Uses two uniform random variables to generate values from a normal
///     distribution. Code taken from:
///       William H. Press, Saul A. Teukolsky, William T. Vetterling, and Brian
///       P. Flannery. 2007. Numerical Recipes 3rd Edition: The Art of
///       Scientific Computing (3 ed.). Cambridge University Press, New York,
///       NY, USA.
///   </para>
/// </remarks>
double RatioOfUniforms(const double mu, const double sig)
{
  // Uses a squeeze on the cartesion plot of standard distribution region
  // to reject efficiently (u,v) not in the allowed region. Since (u,v) is
  // selected uniformly, the coordinates allowed model the normal distribution
  // with the desired mean and variance.
  double u, v, x, y, q;
  do
  {
    u = RandUniform();
    v = 1.7156 * (RandUniform() - 0.5);
    x = u - 0.449871;
    y = fabs(v) + 0.386596;
    q = (x * x) + (y * ((0.19600 * y) - (0.25472 * x)));
  } while ((q > 0.27597) &&
           ((q > 0.27846) || ((v * v) > (-4.0 * log(u) * (u * u)))));
  return mu + (sig * (v / u));
}

}
using namespace math;
}

#endif //_MATH_RAND_BOUND_GENERATOR_H_
