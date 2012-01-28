#ifndef WEM_PWL
#define WEM_PWL
/***********
 *  WEM.h  *
 ***********/


/*==================================================*
 *  Berechnet die komprimierte Steifigkeitsmatrix.  *
 *==================================================*/


void WEM_pwl(sparse2 *S, wavelet *W, vector3 *P, element *E, vector3 ****T, 
	 unsigned int p, unsigned int M, double SL(vector3,vector3),
	 double DL(vector3, vector3, vector3), double I);
#endif