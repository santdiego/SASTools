//Ejemplo de aplicacion de la Clase CshareMemory 
#include "ShareMemoryLibV2.0.1\CshareMemory.h"
#include <stdio.h>

void main(void){
	printf("Ejemplo de aplicacion CshareMemory \n\n\n\n");

	float ejemploFloat[3] = { 1.1, 2.2, 3.3 }; // 12 bytes
	int ejemploInt[4] = { 0, 1, 2, 3 }; //16 bytes
	char *ejemploChar = "Hola Memoria"; //13 bytes
	char datosAgrupados[12+16+13];
	
	/*Cada memoria compartida debe entenderse como un objeto. 
	Cada objeto de memoria compartida tiene una serie de atributos basicos:
	-Un Nombre, el cual utiliza el sistema operativo para reconocer el espaciode memoria entre los distintos procesos.
	-Un Tamaño, fijado por el primer programa que instancie la memoria. 
	-Una Identificacion  (no es obligatorio), sirve para manipular la memoria dentro de un proceso, de forma eficiente.
	-Un puntero al espacio de memoria (automatico) que indica donde inicia el espacio de memoria RAM mapeado FUERA DEL PROCESO
	*/

	//Procedimiento para crear y abrir la memoria compartida
	//#1 : Instanciar la memoria compartida:  
	//Hay 3 formas de instanciar la memoria. 

	//A) - Utilizando el contructor por omision. 
	//CshareMemory memoriaCompartida;
	//En este caso el nombre y el tamaño se deberan asignar por separado.
	//memoriaCompartida.setName("memoriaEjemplo");
	//memoriaCompartida.setSize(41);//en bytes

	//B) - Inicializando el Nombre de la memoria. 
	//CshareMemory memoriaCompartida("memoriaEjemplo");
	//memoriaCompartida.setSize(41);//en bytes

	//C) - Inicializando el Nombre y el Tamaño de la memoria. 
	CshareMemory memoriaCompartida("memoriaEjemplo",41);

	//#2 : Abrir la memoria compartida: Cuando el nombre y el tamaño han sido fijados, se puede proceder a abrir la memoria compartida.
	//Internamente esta operacion le asigna una direccion (puntero) en la memoria RAM
	memoriaCompartida.openShareMemory();

	//Operacion de Escritura en memoria compartida: Pueden escribirse distintos tipos de datos simultaneamente en la memoria compartida,
	//para ello pueden agruparse todos los datos en una misma variable. Por ejemplo usando

	//memcpy(void *destino			,void *fuente	, size_t tamaño)
	memcpy(datosAgrupados			, ejemploFloat	, 12);	 //Copio los 3 floats 
	memcpy(datosAgrupados + 12		, ejemploInt	, 16); //Agrego los 4 ints 
	memcpy(datosAgrupados + 12 + 16	, ejemploChar	, 13); //Agrego el string

	//Escritura de los datos:
	memoriaCompartida.writeShareMemory(datosAgrupados);
	//Otra forma es especificar la cantidad de datos a guardar, por ejemplo si solo quisieramos copiar los flotantes 
	//memoriaCompartida.writeShareMemory(datosAgrupados,12);
	//ATENCION! : en este ejemplo datosAgrupados siempre debe tener igual o mayor tamaño que la memoria compartida,
	//de otro modo writeShareMemory intentara copiar por fuera de datosAgrupados
	
	//Desde otro proceso se utilizaria este codigo para leer la memoria: 
	char datosDeMemoria[12 + 16 + 13];
	float datosFloat[12];
	int datosInt[16];
	char cadenaCaracteres[13];

	//Se debe crear una memoria con el mismo nombre
	CshareMemory memoriaLectura("memoriaEjemplo", 41);
	//Se debe abrir la memoria 
	memoriaLectura.openShareMemory();

	//Lectura del contenido de la memoria
	memoriaLectura.readShareMemory(datosDeMemoria);//Lee los 41 bytes de la memoria
	//memoriaLectura.readShareMemory(datosDeMemoria, 12); //Lee solo los 12 primeros bytes de la memoria

	//Pueden darse formato a los datos de la siguiente forma

	memcpy(datosFloat		, datosDeMemoria			, 12);	 //Copio los 3 floats 
	memcpy(datosInt			, datosDeMemoria + 12		, 16); //Agrego los 4 ints 
	memcpy(cadenaCaracteres	, datosDeMemoria + 12 + 16	, 13); //Agrego el string
	
	printf("Nombre de la memoria compartida: '%s' \n", memoriaLectura.getName());//obtiene el nombre de la memoria
	printf("Tamaño de la memoria compartida: %i [bytes]\n\n", memoriaLectura.getSize());//obtiene el tamaño de la memoria
	printf("Flotantes en Memoria: {%f,%f,%f}  \n", datosFloat[0], datosFloat[1], datosFloat[2]);
	printf("Enteros en Memoria  : {%i,%i,%i,%i}\n", datosInt[0], datosInt[1], datosInt[2], datosInt[3]);
	printf("Cadena de Caracteres: '%s' \n", cadenaCaracteres);

	//Finalmente se cierran todas las memorias abiertas
	memoriaLectura.closeShareMemory();
	memoriaCompartida.closeShareMemory();

	


	while (1){};

}