
#include <TimerOne.h>
#include <avr/wdt.h>

#define BUFFER_SIZE 20

bool event_tx = 0, event_ctr = 0, event_rx = 0, event_task1 = 0; //banderas de eventos-tareas

SASDevice::SASDevice(const byte& sensor_number, const byte& actuator_number, const long& baudrate = 500000, const float& sensor_map_const = 1, const float& actuator_map_const = 1 ) {
  n_sensors = sensor_number;
  n_actuators = actuator_number;
  warpPointer = this;
  new_data = false;
  float ksens = sensor_map_const; //mapeo
  float kact = actuator_map_const; //mapeo
  Serial.begin(baudrate);
}

void SASDevice::readSerial() {
  tempBuffer[i] = Serial.read();  //LEE UN BYTE
  //******************** || SEPARANDO LOS DADOS RECIBIDOS || *************************
  if (header_ctr < 2)
  {
    if (tempBuffer[i] == 255)//enganche de cabecera
    {
      header_ctr++;
    } else {//si no engancha cabecera sigue intentando
      i = 0;
      header_ctr = 0;
    }
  } else {//si recien engancho cabecera encabezado=2,i=1
    i = i + 1;
    if (i <= 3) {
      if (i == 1) {
        mode = tempBuffer[0];
      }
      if (i == 2) {
        n_actuators = tempBuffer[1];
      }
      if (i == 3) {
        n_sensors = tempBuffer[2];
      }
    } else {
      if (i >= (4 + 2 * n_actuators))
      {
        if (tempBuffer[3 + 2 * n_actuators] == 0)
        {
          for (k = 0; k < (2 * n_actuators + 3); k++)
          {
            finalBuffer[k] = tempBuffer[k];
          }
          new_data = true; //indico nuevo dato

        }
        i = 0; //reinicion enganche de trama
        header_ctr = 0;
      }
    }
  }
}

bool SASDevice::getCommands(float* cmd_buff, int& size)const {
  if (!new_data)
    return false;

  if (new_data) {
    mode = finalBuffer[0];                // RECIBE MODO
    n_actuators = finalBuffer[1]; // RECIBE CANTIDAD DE AcTUADORES
    n_sensors = finalBuffer[2];   // RECIBE CANTIDAD DE SENSORES
    for (k = 0; k < n_actuators; k++) {
      commandsBuffer[k] = ((float)256.00 * (0x7F & finalBuffer[2 * k + 4]) + finalBuffer[2 * k + 3]); //arma dato de 15 bits de magnitud
      if ((finalBuffer[2 * k + 4] & 0x80) > 0) { // signo en bit 16
        commandsBuffer[k] = k_act * commandsBuffer[k] * (-1.0);
      }
    }
    event_rx = 0; // RESET DE BANDERA RX TRAMA
    event_tx = 1; // ACTIVA BANDERA DE TX
  }
  return true;
}
bool SASDevice::sendSensors()
  if (event_tx == 1)
  {
    Serial.write(255);    // envia cabecera FF FF
    Serial.write(255);    // envia cabecera FF FF
    Serial.write(mode);   //envia mode

    for (k = 0; k < n_sensors; k++) {
      sendBuffer[k] =  (unsigned int)(fabs(sensorBuffer[k]) * k_sens) & 0x7FFF;
      if (sensorBuffer[k] < 0) {
        sendBuffer[0] = sendBuffer[0] | 0x8000;
      }
      Serial.write(highByte(sendBuffer[k]));
      Serial.write(lowByte(sendBuffer[k]));
    }
    Serial.write(0);       //fin trama
    event_tx = 0;
  }
}

class SASDevice {
  private:
    byte n_sensors;
    byte n_actuators;
    bool new_data;
    //BUFFERS
    unsigned int sendBuffer[BUFFER_SIZE];
    byte tempBuffer[BUFFER_SIZE]; //buffer temporal de lectura. No usar
    byte finalBuffer[BUFFER_SIZE]; //buffer final de lectura.Datos crudos.
    float commandsBuffer[BUFFER_SIZE]; //buffer final de datos de referencias. Datos en formato 15 bit.
    float sensorBuffer[BUFFER_SIZE]; //buffer final de escritura
    float k_sens = 1.0; //mapeo
    float k_act = 1.0; //mapeo

    int counter = 0;
    int header_ctr = 0, i = 0, j = 0, k = 0;
    byte mode = 0;

    static SASDevice* warpPointer;
    void readSerial()
    void static serialEvent() {
      warpPointer->readSerial();
    }
  public:
    bool getCommands(float* data, int& size)const

}

//*************************************************************************************
//********* XXX  INITIALIZATION AND SETUP DE LOS PINOS Y CONTROLADOR XXX  *************
//*************************************************************************************

SASDevice *sasDev;

void setup() {
  sasDev = new SASDevice(2, 2, 500000);

  Timer1.initialize(1000);            //ventana de tiempo 1mS
  Timer1.attachInterrupt(funcion_t1);   //interrupcion timer 1

}//----------------   FIM DO SETUP Y PARAMETROS -----------------------------

//*************************************************************************************
//***************************** XXX  MAIN  XXX  ***************************************
//*************************************************************************************
void loop() {
  if (event_ctr == 1)       //CADA 1ms EJECUTA ESTE CODE - LAZO DE CONTROL
  {
    if (mode == 1) // MODO = 1 START
    {
      for (int i = 0; i < 2; i++) {

      }
      sasDev->setSensors()
      sensorBuffer[0] = commandsBuffer[0]; //ejemplo: espejo en dato[0]

    }
    if (mode == 0) { //STOP
      //Incluir Aqui codigo de parada

    }
  }
  if (event_task1 == 1) {      //TAREA CADA xxx ms
    event_task1 = 0;
  }
  //*************************************************************************************
  //*************** EVENTO DETECCION DE TRAMA   *************************************
  //*************************************************************************************


  //*************************************************************************************
  //*************** XXX  TRANSMIION DE DATOS  XXX  *************************************
  //*************************************************************************************

}//----------------   FIM MAIN ---------------------------------


//*************************************************************************************
//****************** XXX  INTERRUPCION TIMER DE 1ms  XXX  *****************************
//*************************************************************************************
void funcion_t1()
{
  event_ctr = 1;

  counter++;
  if (counter >= 1000) {

    event_task1 = 1; //tarea cada 10 ms
    counter = 0;


  }
}
