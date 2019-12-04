
#include <TimerOne.h>
#include <avr/wdt.h>


byte cantidad_sensores = 2;
byte cantidad_actuadores = 2;

//BUFFERS
unsigned int envio[20];
byte inData[20]; //buffer temporal de lectura. No usar
byte inData1[20]; //buffer final de lectura.Datos crudos.
float actuador[20]; //buffer final de datos de actuadores. Datos en formato 15 bit. 
float sensor[20]; //buffer final de escritura
float Ksensor = 1.0; //mapeo
float Kactuador = 1.0; //mapeo

int contador = 0;
int encabezado = 0, i = 0, j = 0, k = 0;

bool evento_tx = 0, evento_control = 0, evento_rx = 0, evento_tarea_1 = 0; //banderas de eventos-tareas

byte modo = 0;





//*************************************************************************************
//********** SERVICIO DE INTERRUPCION  ISR(SERIAL_RX)   *********************
//*************************************************************************************
void serialEvent() {
  inData[i] = Serial.read();  //LEE UN BYTE
  //******************** || SEPARANDO LOS DADOS RECIBIDOS || *************************
  if (encabezado < 2)
  {
    if (inData[i] == 255)//enganche de cabecera
    {
      encabezado = encabezado + 1;
    } else {//si no engancha cabecera sigue intentando 
      i = 0;
      encabezado = 0;
    }
  } else {//si recien engancho cabecera encabezado=2,i=1 
    i = i + 1; 
    if (i <= 3) {
      if (i == 1) {
        modo = inData[0];
      }
      if (i == 2) {
        cantidad_actuadores = inData[1];
      }
      if (i == 3) {
        cantidad_sensores = inData[2];
      }
    } else {
      if (i >= (4 + 2 * cantidad_actuadores))
      {
        if (inData[3 + 2 * cantidad_actuadores] == 0)
        {
          for (k = 0; k < (2 * cantidad_actuadores + 3); k++)
          {
            inData1[k] = inData[k];
          }
          evento_rx = 1; //indico nuevo dato  
     
        }
        i = 0; //reinicion enganche de trama
        encabezado = 0;
      }
    }
  }
}//--END serialEvent()-------------- -------------------------


//*************************************************************************************
//********* XXX  INITIALIZATION AND SETUP DE LOS PINOS Y CONTROLADOR XXX  *************
//*************************************************************************************
void setup() {
  Serial.begin(500000);
  Timer1.initialize(1000);            //ventana de tiempo 1mS
  Timer1.attachInterrupt(funcion_t1);   //interrupcion timer 1

}//----------------   FIM DO SETUP Y PARAMETROS -----------------------------

//*************************************************************************************
//***************************** XXX  MAIN  XXX  ***************************************
//*************************************************************************************
void loop() {
  if (evento_control == 1)       //CADA 1ms EJECUTA ESTE CODE - LAZO DE CONTROL
  {
    if (modo == 1) // MODO = 1 START
    {
      sensor[0] = actuador[0]; //ejemplo: espejo en dato[0]
    
    }
    if (modo == 0) { //STOP
      //Incluir Aqui codigo de parada
     
    }
  }
  if (evento_tarea_1 == 1) {      //TAREA CADA xxx ms
    evento_tarea_1 = 0;
  }
  //*************************************************************************************
  //*************** EVENTO DETECCION DE TRAMA   *************************************
  //*************************************************************************************
  if (evento_rx == 1) {
    modo = inData1[0];                // RECIBE MODO
    cantidad_actuadores = inData1[1]; // RECIBE CANTIDAD DE AcTUADORES
    cantidad_sensores = inData1[2];   // RECIBE CANTIDAD DE SENSORES
    for (k = 0; k < cantidad_actuadores; k++) {
      actuador[k] = ((float)256.00 * (0x7F & inData1[2 * k + 4]) + inData1[2 * k + 3]); //arma dato de 15 bits de magnitud
      if ((inData1[2 * k + 4] & 0x80) > 0) { // signo en bit 16
        actuador[k] = Kactuador * actuador[k] * (-1.0);
      }
    }
    evento_rx = 0; // RESET DE BANDERA RX TRAMA
    evento_tx = 1; // ACTIVA BANDERA DE TX
  }

  //*************************************************************************************
  //*************** XXX  TRANSMIION DE DATOS  XXX  *************************************
  //*************************************************************************************
  if (evento_tx == 1)
  {
    Serial.write(255);    // envia cabecera FF FF
    Serial.write(255);    // envia cabecera FF FF
    Serial.write(modo);   //envia modo
  
    for (k = 0; k < cantidad_sensores; k++) {
      envio[k] =  (unsigned int)(fabs(sensor[k]) * Ksensor) & 0x7FFF;
      if (sensor[k] < 0) {
        envio[0] = envio[0] | 0x8000;
      }
      Serial.write(highByte(envio[k]));
      Serial.write(lowByte(envio[k]));
    }
    Serial.write(0);       //fin trama
    evento_tx = 0;
  }
}//----------------   FIM MAIN ---------------------------------


//*************************************************************************************
//****************** XXX  INTERRUPCION TIMER DE 1ms  XXX  *****************************
//*************************************************************************************
void funcion_t1()
{
  evento_control = 1;

  contador++;
  if (contador >= 1000) {

    evento_tarea_1 = 1; //tarea cada 10 ms
    contador = 0;

    
  }
}



