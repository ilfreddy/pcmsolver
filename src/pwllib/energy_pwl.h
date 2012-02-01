#ifndef ENERGY
#define ENERGY
/**************
 *  Energy.h  *
 **************/


/*=====================================*
 * Berechnet die potentielle Energie.  *
 *=====================================*/


double energy_pwl(double *u, unsigned int **F, vector3 ****T, unsigned int p, unsigned int m);
#endif