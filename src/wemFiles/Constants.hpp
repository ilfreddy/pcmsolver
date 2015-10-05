#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#ifndef pi
  #define pi 3.1415926535897932385
#endif

// Constants regarding solvent properties
const double epsilon = 78.39;   ///< dielectric constant of the solvent
const double kappaIS = 0.0;       ///< ion screening

const unsigned int delta = 10; ///< constant for memory allocation

const double eps = 1e-8; ///< accuracy for point equality

const double		op = -1; ///< order of the operator

// quadrature
const double		scalingFactor = 0.7071;///< size of relative outer radius
const unsigned int minQuadratureLevel =2; ///< minimal quadrature level
const int g_max = 10; ///< the maximum number of Cubatures calculated

const double q = 1; ///<

#endif
