/*
 * posicion.h
 *
 *  Created on: 26/08/2014
 *      Author: volney
 */

#ifndef POSICION_H_
#define POSICION_H_
#include <iostream>
using namespace std;

struct posicion{
	long int posM0;// valores absolutos de posicion motor 0
	long int posM1;// valores absolutos de posicion motor 1
	long int posM2;// valores absolutos de posicion motor 2
	long int posM3;// valores absolutos de posicion motor 3
	double posM0_rad;// valores en radianes de posicion motor 0
	double posM1_rad;// valores en radianes de posicion motor 1
	double posM2_rad;// valores en radianes de posicion motor 2
	double posM3_rad;// valores en radianes de posicion motor 3
	posicion operator+(const posicion &pos2);
	posicion operator-(const posicion &pos2);
	posicion operator*(const posicion &pos2);
	posicion operator*(const double &k);
	friend ostream& operator << (ostream &o,posicion &p);

};




#endif /* POSICION_H_ */
