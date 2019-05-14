// Compilar con "g++ -o shmBosch shmBosch.cpp bosch.cpp control.cpp posicion.cpp CshareMemory.cpp -lrt"

#include "bosch.h"
#include "CshareMemory.h"
#include <stdio.h>
#include <stdlib.h>

#define SHARE_MEMORY_BASE_NAME "boschShareMemory"
typedef struct tag_VIRTUAL_DEVICE_INPUT {
	double jointVelocity[4];
} VIRTUAL_DEVICE_INPUT;

typedef struct tag_VIRTUAL_DEVICE_OUTPUT {
	double jointPosition[4];
} VIRTUAL_DEVICE_OUTPUT;

int main(void)
{
	printf("\n Cargue el programa servidor antes de continuar. Luego presione ENTER\n");
	//while(!getchar());
	//Confeccion del nombre de la memoria compartida
	char stringOut[40];
	char stringIn[40];
	sprintf(stringIn,"%s_entrada_1",SHARE_MEMORY_BASE_NAME);
    	sprintf(stringOut,"%s_salida_1",SHARE_MEMORY_BASE_NAME);

	//Inicializacion y apertura de la memoria compartida
	
    	CshareMemory *InShareMemory =new CshareMemory(stringIn ,sizeof(VIRTUAL_DEVICE_INPUT));
	CshareMemory *OutShareMemory=new CshareMemory(stringOut,sizeof(VIRTUAL_DEVICE_OUTPUT));

	if(InShareMemory->openShareMemory()==false){
		printf("Error Abriendo Memoria de Entrada");
		return 0;
	}
	if(OutShareMemory->openShareMemory()==false){
		printf("Error Abriendo Memoria de Salida");
		return 0;
	}

	//Variables e inicializacion del robot
	bosch robot;
	posicion qi;
	control  velocidad_ref;
	printf("\n Accione Boton de Inicio Bosch\n ");
	robot.start();
    	robot.Home();
	double  q[4]={0.0,0.0,0.0,0.0};
	VIRTUAL_DEVICE_OUTPUT output;
	VIRTUAL_DEVICE_INPUT  input;
	
	printf("\n---------Inicio Lazo Principal---------\n");
	printf("Para detener el robot presione ENTER");
	//Lazo principal
	for(unsigned i=0;i<25000;i++){
		//Recibo la velocidad del Robot
		 qi=robot.leer_posicion();
		 output.jointPosition[0]=qi.posM0_rad;
		 output.jointPosition[1]=qi.posM1_rad;
		 output.jointPosition[2]=qi.posM2_rad;
		 output.jointPosition[3]=qi.posM3_rad;

		//Guardo la velocidad en la memoria de salida
		 if(OutShareMemory->writeShareMemory(&output,sizeof(VIRTUAL_DEVICE_OUTPUT))!=true)
			printf("ERROR writeShareMemory");
		//Leo la memoria compartida de entrada al robot
            	if(InShareMemory->readShareMemory(&input)!=true)
			printf("ERROR readShareMemory");
		if((input.jointVelocity[0]!=0)||(input.jointVelocity[1]!=0)){

		 //Envio la Velocidad Al robot
		 velocidad_ref.M0=input.jointVelocity[0];    //[rad/s]
		 velocidad_ref.M1=input.jointVelocity[1];    //[rad/s]
		 velocidad_ref.M2=input.jointVelocity[2];    //[rad/s]
		 velocidad_ref.M3=input.jointVelocity[3];    //[rad/s]

		 //printf("Referencias: %f / %f /%f / %f \n",input.jointVelocity[0],input.jointVelocity[1],input.jointVelocity[2],input.jointVelocity[3]);

		 //Envio comandos al robot
		 robot.Actua(velocidad_ref);   
		}
		 usleep(1000);
	}
	InShareMemory->closeShareMemory();
	OutShareMemory->closeShareMemory();
	robot.stop(); //Detiene el Robot
	printf("\n------------Robot Detenido------------\n");
	return 0;
}
