/*
 * control.h
 *
 *  Created on: 26/08/2014
 *      Author: volney
 */

#ifndef CONTROL_H_
#define CONTROL_H_
#include "posicion.h"
struct control{
	double M0;								//valores decimales de accion de los motores con un rango entre -1 y +1
	double M1;
	double M2;
	double M3;
	unsigned int Motor0;					//valores absolutos de accion de los motores con un rango entre (ffff y 8000) negativos , (7fff y 0) positivos
	unsigned int Motor1;
	unsigned int Motor2;
	unsigned int Motor3;
	control& operator=(const posicion &pos2);
	friend ostream& operator << (ostream &o,control &A);
};




#endif /* CONTROL_H_ */
