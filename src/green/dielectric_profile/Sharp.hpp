/* pcmsolver_copyright_start */
/*
 *     PCMSolver, an API for the Polarizable Continuum Model
 *     Copyright (C) 2013 Roberto Di Remigio, Luca Frediani and contributors
 *
 *     This file is part of PCMSolver.
 *
 *     PCMSolver is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     PCMSolver is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with PCMSolver.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     For information on the complete list of contributors to the
 *     PCMSolver API, see: <https://repo.ctcc.no/projects/pcmsolver>
 */
/* pcmsolver_copyright_end */

#ifndef SHARP_HPP
#define SHARP_HPP

#include <iosfwd>

#include "Config.hpp"

/*! \file Sharp.hpp
 *  \class Sharp
 *  \brief A sharp dielectric separation
 *  \author Roberto Di Remigio
 *  \date 2015
 */

class Sharp
{
private:
    double epsilonLeft_;
    double epsilonRight_;
    double center_;
    double value(double point) const {
        if (point  >= center_) {
           return epsilonRight_;
        } else {
           return epsilonLeft_;
        }
    }
public:
    Sharp(double eL, double eR, double c) :
        epsilonLeft_(eL), epsilonRight_(eR), center_(c) {}
    /*! The permittivity profile of the transition layer
     *  \param[out]  e the value of the dielectric constant at point r
     *  \param[in]   r evaluation point
     */
    void operator()(double & e, const double r) const
    {
        e = value(r);
    }
};

#endif // SHARP_HPP