/*
 * posicion.cpp
 *
 *  Created on: 26/08/2014
 *      Author: volney
 */
#include "posicion.h"
using namespace std;

posicion posicion::operator + (const posicion &pos2){

	posicion resultado;
	resultado.posM0=this->posM0+pos2.posM0;
	resultado.posM1=this->posM1+pos2.posM1;
	resultado.posM2=this->posM2+pos2.posM2;
	resultado.posM3=this->posM3+pos2.posM3;
	resultado.posM0_rad=this->posM0_rad+pos2.posM0_rad;
	resultado.posM1_rad=this->posM1_rad+pos2.posM1_rad;
	resultado.posM2_rad=this->posM2_rad+pos2.posM2_rad;
	resultado.posM3_rad=this->posM3_rad+pos2.posM3_rad;

	return resultado;
}

posicion posicion::operator - (const posicion &pos2){

	posicion resultado;
	resultado.posM0=this->posM0-pos2.posM0;
	resultado.posM1=this->posM1-pos2.posM1;
	resultado.posM2=this->posM2-pos2.posM2;
	resultado.posM3=this->posM3-pos2.posM3;
	resultado.posM0_rad=this->posM0_rad-pos2.posM0_rad;
	resultado.posM1_rad=this->posM1_rad-pos2.posM1_rad;
	resultado.posM2_rad=this->posM2_rad-pos2.posM2_rad;
	resultado.posM3_rad=this->posM3_rad-pos2.posM3_rad;

	return resultado;
}
posicion posicion::operator * (const posicion &pos2){

	posicion resultado;
	resultado.posM0=this->posM0*pos2.posM0;
	resultado.posM1=this->posM1*pos2.posM1;
	resultado.posM2=this->posM2*pos2.posM2;
	resultado.posM3=this->posM3*pos2.posM3;
	resultado.posM0_rad=this->posM0_rad*pos2.posM0_rad;
	resultado.posM1_rad=this->posM1_rad*pos2.posM1_rad;
	resultado.posM2_rad=this->posM2_rad*pos2.posM2_rad;
	resultado.posM3_rad=this->posM3_rad*pos2.posM3_rad;

	return resultado;
}
posicion posicion::operator * (const double &k){

	posicion resultado;
	resultado.posM0=this->posM0*k;
	resultado.posM1=this->posM1*k;
	resultado.posM2=this->posM2*k;
	resultado.posM3=this->posM3*k;
	resultado.posM0_rad=this->posM0_rad*k;
	resultado.posM1_rad=this->posM1_rad*k;
	resultado.posM2_rad=this->posM2_rad*k;
	resultado.posM3_rad=this->posM3_rad*k;

	return resultado;
}

 ostream& operator << (ostream &o,posicion &p){

	o << " " << p.posM0_rad << "  " << p.posM1_rad <<"  " << p.posM2_rad <<"  " << p.posM3_rad << " " ;
	return o;
}




