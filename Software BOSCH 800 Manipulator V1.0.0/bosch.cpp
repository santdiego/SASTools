/*
 * bosch.cpp
 *
 *  Created on: 22/08/2014
 *      Author: volney
 */

#include "bosch.h"
#include <math.h>

bosch::bosch() {
	// TODO Auto-generated constructor stub
	iopl(3);
	GposActM0 = GposActM1 = GposActM2 = GposActM3 = GposAntM0 = GposAntM1 = GposAntM2 = 0 ;
	GposAntM3 =  offsetM2 = offsetM3 = 0 ;
	offsetM0 = -1050;//-120;
	offsetM1 = -1281;//-120;
	PosCero();						// inicializo a cero los registros de posicion
	ActMot.M0 = 0;					// inicializo a cero los registros de acciones de control
	ActMot.M1 = 0;
	ActMot.M2 = 0;
	ActMot.M3 = 0;
	ActMot.Motor0=0x8000;
	ActMot.Motor1=0x8000;
	ActMot.Motor2=0x8000;
	ActMot.Motor3=0x8000;
}

void bosch::Reset_contadores(int n) {
	// TODO Auto-generated destructor stub
	switch (n){
	case  0:
		outb(0x00,RESET); 		//reseteo de contadores
		break;
	case 1:
		outb(0x00,RESET1); 		//reseteo de contadores
		break;
	}
}
bool bosch::Sensor(int articulacion){

	return (( inb(STROBE) >> articulacion ) & 1);
	return 1;

}

void bosch::Home() {
	// TODO Auto-generated constructor stub
	bool estado = bosch::Sensor(0);
	bosch::PosCero();
	while(bosch::Sensor(0) == estado)
	{
		usleep(10000);

		if(bosch::Sensor(0))
				bosch::Actua(-0.2,0);
			else
				bosch::Actua(0.2,0);
	}
	bosch::Reset_contadores(0);
	bosch::Actua(-0.05,0);
	estado = bosch::Sensor(1);
	while(bosch::Sensor(1) == estado)
		{
		usleep(10000);
			if(bosch::Sensor(1))
					bosch::Actua(-0.2,1);
				else
					bosch::Actua(0.2,1);
		}
	bosch::Reset_contadores(1);
	bosch::Actua(0,1);

	offsetM0 = -80;
	offsetM1 = -80;

}


void bosch::stop() {
	// TODO Auto-generated constructor stub
	outb(0x00,READY);
}

void bosch::start() {
	// TODO Auto-generated constructor stub
	outb(0x01,READY);
}


posicion bosch::leer_posicion() {
	// TODO Auto-generated constructor stub
long int temporal;
temporal = inl(MOTOR1_2);
temporal = inl(MOTOR1_2);		//leo dos veces para refrescar los latchs y actualizar la lectura de los contadores
GposActM0 = ((temporal & 0xffff0000)>>16);
GposActM1 = (temporal & 0x0000ffff);
if(((GposActM0 - GposAntM0) < 0x7fff) && ((GposAntM0 - GposActM0) < 0x7fff))
	RegPos.posM0 +=(GposActM0 - GposAntM0);
			else
				if(GposActM0 > GposAntM0)
					RegPos.posM0 +=((GposActM0 - GposAntM0)-0x10000);
				else
					RegPos.posM0 +=((GposActM0 - GposAntM0)+0x10000);

if(((GposActM1 - GposAntM1) < 0x7fff)&&((GposAntM1 - GposActM1) < 0x7fff))
	RegPos.posM1 +=(GposActM1 - GposAntM1);
			else
				if(GposActM1 > GposAntM1)
					RegPos.posM1 +=((GposActM1 - GposAntM1)-0x10000);
				else
					RegPos.posM1 +=((GposActM1 - GposAntM1)+0x10000);
GposAntM0 = GposActM0;
GposAntM1 = GposActM1;		// actualizo valores anteriores de posicion
RegPos.posM0_rad = RegPos.posM0 * redM0;
RegPos.posM1_rad = RegPos.posM1 * redM1;
RegPos.posM2_rad = RegPos.posM2 * redM2;
RegPos.posM3_rad = RegPos.posM3 * redM3;
return RegPos;
}
void bosch::PosCero(){
	RegPos=(RegPos-RegPos);			// pone a cero el registro de posicion
}

void bosch::Actua(double valor,int motor) {
	// TODO Auto-generated constructor stub
	unsigned int temp1,temp2;

	switch(motor)
			{
	case 0:
		if(fabs(valor)>0.95)
			 			ActMot.M0=(valor/fabs(valor));
						else
						ActMot.M0 = valor;

		ActMot.Motor0 = (ActMot.M0 * 0x8000) + 0x8000 - offsetM0;
		break;
	case 1:
		if(fabs(valor)>0.95)
								ActMot.M1=(valor/fabs(valor))*0.95;
							else
								ActMot.M1 = valor;

		ActMot.Motor1 = (ActMot.M1 * 0x8000) + 0x8000 - offsetM1;
		break;
		/*
	case 2:
		ActMot.M2 = valor;
		ActMot.Motor2 = (ActMot.M2 * 0x8000) + 0x8000 + offsetM2;
		break;
	case 3:
		ActMot.M3 = valor;
		ActMot.Motor3 = (ActMot.M3 * 0x8000) + 0x8000 + offsetM3;
		break;
		*/
	}

	temp1=rot(ActMot.Motor0);
	temp2=rot(ActMot.Motor1);
	outl(((temp1<<16)|temp2),MOTOR3_4);

}
void bosch::Actua(control valor){
	unsigned int temp1,temp2;
	if(fabs(valor.M0)>0.95)
				ActMot.M0=(valor.M0/fabs(valor.M0))*0.95;		// se normaliza a 0.95 para no desbordar por el offset
			else
				ActMot.M0 = valor.M0;

	if(fabs(valor.M1)>0.95)
				ActMot.M1=(valor.M1/fabs(valor.M1))*0.95;
			else
				ActMot.M1 = valor.M1;

	if(fabs(valor.M2)>0.95)
				ActMot.M2=(valor.M2/fabs(valor.M2))*0.95;
			else
				ActMot.M2 = valor.M2;

	if(fabs(valor.M3)>0.95)
				ActMot.M3=(valor.M3/fabs(valor.M3))*0.95;
			else
				ActMot.M3 = valor.M3;

	ActMot.Motor0 = (ActMot.M0 * 0x8000) + 0x8000 + offsetM0;

	ActMot.Motor1 = (ActMot.M1 * 0x8000) + 0x8000 + offsetM1;

	ActMot.Motor2 = (ActMot.M2 * 0x8000) + 0x8000 + offsetM2;

	ActMot.Motor3 = (ActMot.M3 * 0x8000) + 0x8000 + offsetM3;

	temp1=rot(ActMot.Motor0)& 0xFFFF;
	temp2=rot(ActMot.Motor1)& 0xFFFF;
	outl(((temp1<<16)|temp2),MOTOR3_4);


}

unsigned char rot(unsigned int  val)
{
	unsigned int j,tmp,res=0;
	for(j=0;j<16;j++){
			tmp=(val&0xffff);
			tmp=(val>>j)&1;
			tmp=tmp<<(15-j);
			res=res|tmp;
	}

	return res;
}

bosch::~bosch() {
	// TODO Auto-generated destructor stub
}

