#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

#ifndef FILE_H_FILE
	#define FILE_H_FILE

	void readFile(char *A, int *muestreo, char *B, int *com,char *C, int *baudrate,char *D, int *dispositivo,char *E, int *cantidadSensores, char *F, int *cantidaActuadores);
	void writeFile(char *A, int *muestreo, char *B, int *com,char *C, int *baudrate,char *D, int *dispositivo,char *E, int *cantidadSensores, char *F, int *cantidadActuadores);

#endif