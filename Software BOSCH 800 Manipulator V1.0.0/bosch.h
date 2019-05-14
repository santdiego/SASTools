/*
 * bosch.h
 *
 *  Created on: 22/08/2014
 *      Author: volney
 */

#ifndef BOSCH_H_
#define BOSCH_H_
//#include "/usr/include/i386-linux-gnu/sys/io.h"
#include "/usr/include/x86_64-linux-gnu/sys/io.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "posicion.h"
#include "control.h"

#define BASE_PCIPROTO			0X1000
#define MOTOR1_2 				BASE_PCIPROTO + 16
#define MOTOR3_4				BASE_PCIPROTO + 12
#define STROBE 					BASE_PCIPROTO + 20
#define RESET 					BASE_PCIPROTO + 28	// reset contador motor 0
#define RESET1					BASE_PCIPROTO + 4	// reset contador motor 1
#define READY 					BASE_PCIPROTO + 24
#define redM0					2.2476E-5
#define redM1 					2.87293030563E-5
#define redM2					4.345703125E-3
#define redM3					7.942942591E-5
#define radgrad					57.2956
unsigned char rot(unsigned int  val); // se usa para corregir problema de los D/A (el bits mas significativo es el menos significativo)


class bosch {

public:
	bosch();
	void Home();
	void stop();
	void start();
	void Reset_contadores(int n);
	bool Sensor(int articulacion);
	posicion leer_posicion();
	void PosCero();
	void Actua(double valor,int motor); //valores entre -1 y 1, motor (0 al 3)
	void Actua(control valor); 			//valores entre -1 y 1, motor (0 al 3)

	virtual ~bosch();
private:
	long int GposActM0 ;				// almacena posicion actual 4 motores
	long int GposActM1 ;
	long int GposActM2 ;
	long int GposActM3 ;
	long int GposAntM0 ;				// almacena posicion anterior 4 motores
	long int GposAntM1 ;
	long int GposAntM2 ;
	long int GposAntM3 ;
	long int offsetM0 ;					// almacena los offset 4 motores
	long int offsetM1 ;
	long int offsetM2 ;
	long int offsetM3 ;
	posicion RegPos;					// usado para registrar los valores de posicion absoluta del robot
	control ActMot;						// usado para registrar los valores de la ultima accion de control ( valor de los latch de los D/a)

};


#endif /* BOSCH_H_ */
