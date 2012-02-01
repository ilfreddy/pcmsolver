/*! \file PWLSolver.h 
\brief PCM solver
*/


#ifndef PWLSOLVER_H_
#define PWLSOLVER_H_

#include <string>
#include <vector>
#include <iostream>
#include <complex>

extern "C"{
#include "vector3.h"
#include "sparse2.h"
#include "intvector_pwl.h"
#include "basis_pwl.h"
}

using namespace std;

template<class T>
class PWLSolver : public WEMSolver<T> {
 public:
    PWLSolver(GreensFunction<T> &gfi, GreensFunction<T> &gfo);
    PWLSolver(GreensFunction<T> *gfi, GreensFunction<T> *gfo);
    PWLSolver(Section solver);
    ~PWLSolver();
    virtual void constructSystemMatrix();
    virtual void initInterpolation();
    virtual void compCharge(const VectorXd & potential, VectorXd & charge);
 protected:
    element *elementTree; //*E_; Hierarchical element list
    wavelet *waveletList; //*W_; List of wavelets
};
#endif