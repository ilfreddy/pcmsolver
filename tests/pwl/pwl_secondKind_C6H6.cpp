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
 *     PCMSolver API, see: <http://pcmsolver.github.io/pcmsolver-doc>
 */
/* pcmsolver_copyright_end */

#define BOOST_TEST_MODULE PWLSolverSecondKindC6H6

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <iostream>
#include <iomanip>
#include <vector>

#include "Config.hpp"

#include "DerivativeTypes.hpp"
#include "PWLSolver.hpp"
#include "UniformDielectric.hpp"
#include "Vacuum.hpp"
#include "WaveletCavity.hpp"
#include "LoggerInterface.hpp"
#include "PhysicalConstants.hpp"

/*! \class PWLSolver
 *  \test \b  C6H6 tests PWLSolver with the second kind equation using ammonia and a wavelet cavity
 */
BOOST_AUTO_TEST_CASE(C6H6)
{
    LOG_INIT();
    double f = 1.2; /// constant from Tomasi Persico 1994
    // Set up cavity
    Eigen::Vector3d C1( 5.274,  1.999, -8.568);
    Eigen::Vector3d C2( 6.627,  2.018, -8.209);
    Eigen::Vector3d C3( 7.366,  0.829, -8.202);
    Eigen::Vector3d C4( 6.752, -0.379, -8.554);
    Eigen::Vector3d C5( 5.399, -0.398, -8.912);
    Eigen::Vector3d C6( 4.660,  0.791, -8.919);
    Eigen::Vector3d H1( 4.704,  2.916, -8.573);
    Eigen::Vector3d H2( 7.101,  2.950, -7.938);
    Eigen::Vector3d H3( 8.410,  0.844, -7.926);
    Eigen::Vector3d H4( 7.322, -1.296, -8.548);
    Eigen::Vector3d H5( 4.925, -1.330, -9.183);
    Eigen::Vector3d H6( 3.616,  0.776, -9.196);

    std::vector<Sphere> spheres;
    Sphere sph1(C1, 1.53*f); 
    Sphere sph2(C2, 1.53*f);
    Sphere sph3(C3, 1.53*f);
    Sphere sph4(C4, 1.53*f);
    Sphere sph5(C5, 1.53*f);
    Sphere sph6(C6, 1.53*f);
    
    Sphere sph7(H1, 1.06*f); 
    Sphere sph8(H2, 1.06*f);
    Sphere sph9(H3, 1.06*f);
    Sphere sph10(H4, 1.06*f);
    Sphere sph11(H5, 1.06*f);
    Sphere sph12(H6, 1.06*f);

    spheres.push_back(sph1);
    spheres.push_back(sph2);
    spheres.push_back(sph3);
    spheres.push_back(sph4);
    spheres.push_back(sph5);
    spheres.push_back(sph6);
    spheres.push_back(sph7);
    spheres.push_back(sph8);
    spheres.push_back(sph9);
    spheres.push_back(sph10);
    spheres.push_back(sph11);
    spheres.push_back(sph12);
    
    double probeRadius = 1.385*f; // Probe Radius for water
    int patchLevel = 2;
    double coarsity = 0.5;
    WaveletCavity cavity(spheres, probeRadius, patchLevel, coarsity);
    cavity.readCavity("molec_dyadic.dat");
    cavity.scaleCavity(1./convertBohrToAngstrom);

    C1 /=convertBohrToAngstrom;
    C2 /=convertBohrToAngstrom;
    C3 /=convertBohrToAngstrom;
    C4 /=convertBohrToAngstrom;
    C5 /=convertBohrToAngstrom;
    C6 /=convertBohrToAngstrom;

    H1 /=convertBohrToAngstrom;
    H2 /=convertBohrToAngstrom;
    H3 /=convertBohrToAngstrom;
    H4 /=convertBohrToAngstrom;
    H5 /=convertBohrToAngstrom;
    H6 /=convertBohrToAngstrom;

    double permittivity = 78.39;
    Vacuum<AD_directional> * gfInside = new Vacuum<AD_directional>();
    UniformDielectric<AD_directional> * gfOutside = new
    UniformDielectric<AD_directional>(permittivity);
    int firstKind = 1;
#ifdef DEBUG
    FILE* debugFile = fopen("debug.out","w");
    fclose(debugFile);
#endif
    Compression comp(2.5, 2.5, 0.001);
    PWLSolver solver(gfInside, gfOutside, comp, firstKind);
    solver.buildSystemMatrix(cavity);
    cavity.uploadPoints(solver.getQuadratureLevel(), solver.getT_());

    double Hcharge = 1.0;
    double Ccharge = 6.0;
    int size = cavity.size();
    Eigen::VectorXd fake_En = Eigen::VectorXd::Zero(size);
    for (int i = 0; i < size; ++i) {
        Eigen::Vector3d center = cavity.elementCenter(i);
        Eigen::Vector3d normal = cavity.elementNormal(i);

        double dist = (center - C1).norm();
        double C1En = -Ccharge*(center - C1).dot(normal)/(dist*dist*dist);
        dist = (center - C2).norm();
        double C2En = -Ccharge*(center - C2).dot(normal)/(dist*dist*dist);
        dist = (center - C3).norm();
        double C3En = -Ccharge*(center - C3).dot(normal)/(dist*dist*dist);
        dist = (center - C4).norm();
        double C4En = -Ccharge*(center - C4).dot(normal)/(dist*dist*dist);
        dist = (center - C5).norm();
        double C5En = -Ccharge*(center - C5).dot(normal)/(dist*dist*dist);
        dist = (center - C6).norm();
        double C6En = -Ccharge*(center - C6).dot(normal)/(dist*dist*dist);

        dist = (center - H1).norm();
        double H1En = -Hcharge*(center - H1).dot(normal)/(dist*dist*dist);
        dist = (center - H2).norm();
        double H2En = -Hcharge*(center - H2).dot(normal)/(dist*dist*dist);
        dist = (center - H3).norm();
        double H3En = -Hcharge*(center - H3).dot(normal)/(dist*dist*dist);
        dist = (center - H4).norm();
        double H4En = -Hcharge*(center - H4).dot(normal)/(dist*dist*dist);
        dist = (center - H5).norm();
        double H5En = -Hcharge*(center - H5).dot(normal)/(dist*dist*dist);
        dist = (center - H6).norm();
        double H6En = -Hcharge*(center - H6).dot(normal)/(dist*dist*dist);
        fake_En(i) = C1En + C2En + C3En + C4En + C5En + C6En +
          H1En + H2En + H3En + H4En + H5En + H6En;
    }
    // The total ASC for a dielectric is -Q*[(epsilon-1)/epsilon]
    Eigen::VectorXd fake_asc = Eigen::VectorXd::Zero(size);
    solver.computeCharge(fake_En, fake_asc);

    double totalASC = - (6 * Ccharge + 6 * Hcharge) * ( permittivity - 1) / permittivity; 
    double totalFakeASC = fake_asc.sum();
    std::cout << "totalASC - totalFakeASC = " << totalASC - totalFakeASC << std::endl;
    BOOST_REQUIRE_CLOSE(totalASC, totalFakeASC, 3e-2);
}
