/*

Wavelet Cavity c++ interface and wrapper methods

*/
#include <iostream>
#include <fstream> 
#include <string>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

extern "C"{
	//#include "WEM.h"
	//#include "read_points.h"
#include "vector2.h"
#include "vector3.h"
#include "interpolate.h"
//#include "topology.h"
//#include "kern.h"
//#include "compression.h"
//#include "postproc.h"
//#include "WEMRHS.h"
//#include "WEMPCG.h"
//#include "WEMPGMRES.h"
//#include "dwt.h"
#include "cubature.h"
#include "gauss_square.h"
#include "constants.h"
}

#include "Constants.h"
#include "Getkw.h"
#include "Cavity.h"
#include "WaveletCavity.h"

WaveletCavity::WaveletCavity(const Getkw & Input, const string path){
	Section cavity = Input.getSect(path);
	vector<double> spheresInput = cavity.getDblVec("Spheres");
	patchLevel = cavity.getInt("PatchLevel");
	probeRadius = cavity.getDbl("ProbeRadius");
	coarsity = cavity.getDbl("Coarsity");
	nSpheres = spheresInput.size()/4; // the correctness of the size has ben checked at input parsing
    sphereCenter.resize(NoChange, nSpheres);
    sphereRadius.resize(nSpheres);
	int j = 0;
	for (int i = 0; i < nSpheres; i++) {
		sphereCenter(0,i) = spheresInput[j];
		sphereCenter(1,i) = spheresInput[j+1];
		sphereCenter(2,i) = spheresInput[j+2];
		sphereRadius(i)   = spheresInput[j+3];
		j += 4;
	}
	uploadedDyadic = false;
}

WaveletCavity::WaveletCavity(const Section & cavity){
	vector<double> spheresInput = cavity.getDblVec("Spheres");
	patchLevel = cavity.getInt("PatchLevel");
	probeRadius = cavity.getDbl("ProbeRadius");
	coarsity = cavity.getDbl("Coarsity");
	nSpheres = spheresInput.size()/4; // the correctness of the size has ben checked at input parsing
    sphereCenter.resize(NoChange, nSpheres);
    sphereRadius.resize(nSpheres);
	int j = 0;
	for (int i = 0; i < nSpheres; i++) {
		sphereCenter(0,i) = spheresInput[j];
		sphereCenter(1,i) = spheresInput[j+1];
		sphereCenter(2,i) = spheresInput[j+2];
		sphereRadius(i)   = spheresInput[j+3];
		j += 4;
	}
	uploadedDyadic = false;
}

void WaveletCavity::writeInput(string &fileName){

    ofstream output;
    output.open(fileName.c_str(), fstream::out);

    output << nSpheres << endl;
	output.setf(ios_base::showpoint);
	output.precision(12);
    for(int i=0; i < nSpheres; i++) {
		output << sphereCenter(0,i) << " ";
		output << sphereCenter(1,i) << " ";
		output << sphereCenter(2,i) << " ";
		output << sphereRadius(i) << endl;
    }
    output.close();
	uploadedDyadic = false;
}

extern "C" {
	int waveletCavityDrv_(double probeRadius, double coarsity, 
						  int patchLevel);
}


void WaveletCavity::makeCavity() {
	int dummy = 0, check = 0;
	string fileName = "cavity.inp";
	writeInput(fileName);
	check = waveletCavityDrv_(probeRadius, coarsity, patchLevel);
}


void WaveletCavity::readCavity(const string & filename) {

		int i, j, k;
		double x, y, z;

		ifstream file;
		file.open(filename.c_str());
		file >> nLevels >> nPatches;

		int nNodes = (1 << nLevels) + 1;

		nPoints = nPatches * nNodes * nNodes;
		
		for (int k = 0; k < nPoints; k++) {
			file >> i >> j >> k >> x >> y >> z;
			Vector3i index(i, j, k);
			nodeIndex.push_back(index);
			Vector3d point(x, y, z);
			nodePoint.push_back(point);
		}

		file.close();
		uploadedDyadic = true;

}

void WaveletCavity::uploadPoints(int quadLevel, vector3 **** T_) {
	if (not uploadedDyadic) {
		cout << "Error: upload dyadic file first" << endl;
		exit(-1);
	}
	vector2 s, t;
	vector3 point;
	vector3 norm;
	int n = 1 << nLevels;
	double h = 1.0 / n;
	cubature *Q;
	init_Gauss_Square(&Q, quadLevel + 1);

	nTess = nPatches * n * n * Q[quadLevel].nop;

	tessCenter.resize(NoChange, nTess);
	tessNormal.resize(NoChange, nTess);
	tessArea.resize(nTess);

	nuclearPotential.resize(nTess);
	nuclearCharge.resize(nTess);
	electronicPotential.resize(nTess);
	electronicCharge.resize(nTess);

	//   	nuclearPotential.setConstant(1.0);

	int j = 0;
	for (int i1 = 0; i1 < nPatches; i1++){
		for (int i2 = 0; i2 < n; i2++){
			s.y = h * i2;
			for (int i3=0; i3 < n; i3++){
				s.x = h * i3;
				for (int k = 0; k < Q[quadLevel].nop; k++){
					t = vector2_add(s,vector2_Smul(h,Q[quadLevel].xi[k]));
					point = Chi(t,T_[i1], nLevels);
					norm = n_Chi(t,T_[i1], nLevels);
					Vector3d center(point.x, point.y, point.z);	 
					Vector3d normal(norm.x,  norm.y,  norm.z);	 
					normal.normalize();
					center = center / ToAngstrom;
					double area = h * h * Q[quadLevel].w[k] * vector3_norm(n_Chi(t, T_[i1], nLevels));
					tessCenter.col(j) = center.transpose();
					tessNormal.col(j) = normal.transpose();
					tessArea(j) = area;
					j++;
				}
				s.x += h;
			}
			s.y += h;
		}
	}
	free_Gauss_Square(&Q,quadLevel+1);  
	isBuilt = true;
}

ostream & operator<<(ostream &os, const WaveletCavity &cavity) {
	os << "Molecular cavity" << endl;
	os << "Probe Radius:   " << cavity.probeRadius << endl;
	os << "Coarsity:       " << cavity.coarsity << endl;
	os << "Patch Level:    " << cavity.patchLevel << endl;
	os << "Nr. of spheres: " << cavity.nSpheres;
    for(int i = 0; i < cavity.nSpheres; i++) {
		os << endl;
		os << i+1 << " ";
		os << cavity.sphereCenter(0,i) << " ";
		os << cavity.sphereCenter(1,i) << " ";
		os << cavity.sphereCenter(2,i) << " ";
		os << cavity.sphereRadius(i) << " ";
    }
	if (cavity.uploadedDyadic) {
		for(int i = 0; i < cavity.nPoints; i++) {
			os << endl;
			os << i+1 << " ";
			os << cavity.nodeIndex[i].transpose() << " " << cavity.nodePoint[i].transpose() << " ";
		}
	}
	return os;
}
