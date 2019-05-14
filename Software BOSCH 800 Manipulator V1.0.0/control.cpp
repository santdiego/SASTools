/*
 * control.cpp
 *
 *  Created on: 26/08/2014
 *      Author: volney
 */

#include"control.h"

control& control::operator=(const posicion &pos2){

	this->M0 = pos2.posM0_rad;
	this->M1 = pos2.posM1_rad;
	this->M2= pos2.posM2_rad;
	this->M3= pos2.posM3_rad;

	return *this;
}

ostream& operator << (ostream &o,control &A){

	o << " " << A.M0 << " " << A.M1 <<" " << A.M2 <<" " << A.M3 << " " ;
	return o;
}



