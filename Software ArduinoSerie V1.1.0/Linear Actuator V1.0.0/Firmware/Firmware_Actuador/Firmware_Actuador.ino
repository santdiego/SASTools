//Librerias
#include <TimerThree.h>
#include <TimerOne.h>

//Estados Puente H
#define BRAKE 0
#define CW    1
#define CCW   2

//Sensores Corriente
#define pionner     1
#define act_lineal  2
#define brazo       3

#define Sen_Brazo A9
#define Sen_Actuador A12
#define Sen_Pionner A15

//MOTOR ACTUADOR
#define MOTOR_A1_PIN 7
#define MOTOR_B1_PIN 8
#define PWM_MOTOR_1 5
#define EN_PIN_1 A0
#define MOTOR_1 0

//ACTUADOR
#define Pos_Actuador A7

//PID
#define ts 0.001
#define tensionmax 12
#define alfa 0.01

float y, r, u, y1=0;
float integral, integral1;
float e, e1=0, d, d1=0, df, df1=0, yf, yf1;

float kp=1.8, ki=0.01, kd=0.5, td=0.9;

float usSpeed = 0;  
unsigned short usMotor_Status = BRAKE;

byte cantidad_sensores = 4;
byte cantidad_actuadores = 1;

//BUFFERS
unsigned int envio[20];
byte inData[20];
byte inData1[20];
float actuador[20];
float sensor[20];
float Ksensor = 1.0; //mapeo
float Kactuador = 1.0; //mapeo

int contador = 0;
int encabezado = 0, i = 0, j = 0, k = 0;
int ciclo = 0;
bool evento_tx = 0, evento_control = 0, evento_rx = 0, evento_tarea_1 = 0; //banderas de eventos-tareas
byte modo = 1, modo1 = 0;

//********************************************************************************
//********* XXX  INICIALIZACION Y SETUP DE PINES Y CONTROLADOR XXX  *************
//********************************************************************************
void setup() {

  Timer3.initialize(200);
  
  Serial.begin(400000);
  Timer1.initialize(1000);            //ventana de tiempo 1mS
  Timer1.attachInterrupt(funcion_t1);   //interrupcion timer 1
  pinMode(MOTOR_A1_PIN, OUTPUT);
  pinMode(MOTOR_B1_PIN, OUTPUT);
  pinMode(PWM_MOTOR_1, OUTPUT);
  pinMode(EN_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, HIGH);

  pinMode(A7, OUTPUT);
  pinMode(A10, OUTPUT);
  pinMode(A11, OUTPUT);
  pinMode(A13, OUTPUT);
  pinMode(A14, OUTPUT);
  pinMode(52, OUTPUT);
  pinMode(53, OUTPUT);

  digitalWrite(A8, HIGH);
  digitalWrite(A10, LOW);
  digitalWrite(A11, HIGH);
  digitalWrite(A13, LOW);
  digitalWrite(A14, HIGH);
  digitalWrite(52, LOW);
  digitalWrite(53, HIGH);

}//----------------   FIN DE SETUP -----------------------------

//*************************************************************************************
//***************************** XXX  MAIN  XXX  ***************************************
//*************************************************************************************
void loop() {

  if (evento_control == 1)       //CADA 1ms EJECUTA ESTE CODE - LAZO DE CONTROL
  {
    //Lecturas Referencia y Senosr
    Posicion();
    r=actuador[0];
    y=sensor[3];

    yf= yf1*(1-alfa)+y *alfa;
    
    //Calculo del Error
    e= r - yf;
          
    if ((modo == 1) || (modo == 3) || (modo == 5)) {
      modo1 = modo;
    }
    if (modo1 == 3) // MODO START
      if (abs(e)<2)
      {
        Stop();
        integral=0;
        u=0.0;
      }else
      {
        //Calculo parte Integral
        integral=integral1+ts*0.5*(e+e1);
        
        //Calculo parte Derivativa
        d=td*(d1-kd*(yf-yf1));
        
        df= df1*(1-alfa)+d *alfa;
      
        //Calculo Accion de Control
        u=kp*e+ki*integral+kd*df;

        //Limitacion Accion de Control
        if(u>tensionmax){ u=tensionmax;}
        if(u<-tensionmax){ u=-tensionmax;}

        //Sentido de Giro
        if (u>=0)
        {
          Forward();
        }else
        {
          Reverse();
          u=-u;
        }

        //Aplicacion de accion de control
        
        usSpeed=(u*1023)/12;
        motorGo(MOTOR_1, usMotor_Status, usSpeed);
        
        //Actualizacion de Variables
        e1=e;
        d1=d;
        integral1=integral;
        y1=y;
        df1= df;
        yf1= yf;
      }
    }

    //*************** ||  MODO = 0  STOP  ||  ***************************
    if (modo1 == 1) { // MODO STOP
      Stop();
      integral=0;
      u=0.0;
    }
    if (modo1 == 5) { //MODO PAUSA
      Stop();
      integral=0;
      u=0.0;
    }
  //*************************************************************************************
  //***************         EVENTO DE TAREAS        *************************************
  //*************************************************************************************
  if (evento_tarea_1 == 1)       //TAREA CADA 30 ms
  {
    //Mediciones de Corrientes
    if (ciclo == 0) 
    {
      Corriente(pionner);
    }
    if (ciclo == 1) 
    {
      Corriente(act_lineal);
    }
    if (ciclo == 2) 
    {
      Corriente(brazo);
      ciclo = -1;
    }
    
    ciclo++;
    evento_tarea_1 = 0;
  }

  //*************************************************************************************
  //***************    EVENTO DETECCION DE TRAMA    *************************************
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
  //*************** XXX  TRANSMISION DE DATOS  XXX  *************************************
  //*************************************************************************************
  if (evento_tx == 1)
  {
    Serial.write(255);    // envia cabecera FF FF
    Serial.write(255);    // envia cabecera FF FF
    Serial.write(modo);   //envia modo

    for (k = 0; k < cantidad_sensores; k++) {

      envio[k] =  (unsigned int)(sensor[k] * Ksensor) & 0x7FFF;

      if (sensor[k] < 0)
      {
        envio[k] = envio[k] ^ 0xFFFF;
        envio[k] = envio[k] + 1;
      }
      Serial.write(highByte(envio[k]));
      Serial.write(lowByte(envio[k]));
    }
    Serial.write(0);       //fin trama

    evento_tx = 0;
  }

}//----------------   FIN MAIN ---------------------------------

//*************************************************************************************
//********** SERVICIO DE INTERRUPCION  ISR(SERIAL_RX)   *******************************
//*************************************************************************************
void serialEvent() {
  inData[i] = Serial.read();  //LEE UN BYTE

  //******************** || SEPARANDO LOS DADOS RECIBIDOS || *************************
  if (encabezado < 2)
  { if (inData[i] == 255)
    { encabezado = encabezado + 1;
    } else
    {
      i = 0;
      encabezado = 0;
    }
  } else
  {
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
      { if (inData[3 + 2 * cantidad_actuadores] == 0)
        { for (k = 0; k < (2 * cantidad_actuadores + 3); k++)
          {
            inData1[k] = inData[k];
          }
          evento_rx = 1;
        }
        i = 0;
        encabezado = 0;
      }
    }
  }
}//---------------- -------------------------

//*************************************************************************************
//****************** XXX  INTERRUPCION TIMER DE 30ms  XXX  ****************************
//*************************************************************************************
void funcion_t1()
{
  evento_control = 1;
  contador++;
  if (contador >= 30) {
    evento_tarea_1 = 1; //tarea cada 10 ms
    contador = 0;
}
}
//*************************************************************************************
//***************************** XXX  MOTOR  XXX  **************************************
//*************************************************************************************
void Stop()
{
  //Funcion utilizada para detener actuador.
  usMotor_Status = BRAKE;
  motorGo(MOTOR_1, usMotor_Status, 0);
}

void Forward()
{
  //Funcion para setear hacia arriba el sentido del actuador. 
  usMotor_Status = CW;
}

void Reverse()
{
  //Funcion para setear hacia abajo el sentido del actuador. 
  usMotor_Status = CCW;
}

void motorGo(uint8_t motor, uint8_t direct, uint16_t pwm)         //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
  //Funcion mediante la cual se le aplica al actuador el sentido y la velocidad mediante pwm.
  if (motor == MOTOR_1)
  {
    if (direct == CW)
    {
      digitalWrite(MOTOR_A1_PIN, LOW);
      digitalWrite(MOTOR_B1_PIN, HIGH);
    }
    else if (direct == CCW)
    {
      digitalWrite(MOTOR_A1_PIN, HIGH);
      digitalWrite(MOTOR_B1_PIN, LOW);
    }
    else
    {
      digitalWrite(MOTOR_A1_PIN, LOW);
      digitalWrite(MOTOR_B1_PIN, LOW);
    }
    Timer3.pwm(PWM_MOTOR_1, pwm);
  }
}
//*************************************************************************************
//***************************** XXX  SENSORES  XXX  ***********************************
//*************************************************************************************
void Corriente(int seccion)
{
  //Funcion mediante la cual se toma la medicion de la corriente de las distintas partes del sistema. 
  float voltajeSensor = 0.0;
  int cantidad = 100;
  float I = 0;
  //2.4885//4.9
  if (seccion == 1)
  {
    for (k = 0; k < cantidad; k++)
    {
      voltajeSensor = voltajeSensor + analogRead(Sen_Pionner) * (5.0 / 1023.0); //lectura del sensor
    }
    float Sensibilidad = 0.100;
    I = (2.335 - (voltajeSensor / cantidad)) / Sensibilidad; //Ecuación  para obtener la corriente
    sensor[0] = I * 1000;
  }
  if (seccion == 2)
  {
    for (k = 0; k < cantidad; k++)
    {
      voltajeSensor = voltajeSensor + analogRead(Sen_Actuador) * (5.0 / 1023.0); //lectura del sensor
    }
    float Sensibilidad = 0.100;
    I = (2.4717 - (voltajeSensor / cantidad)) / Sensibilidad; //Ecuación  para obtener la corriente
    sensor[1] = I * 1000;
  }
  if (seccion == 3)
  {
    for (k = 0; k < cantidad; k++)
    {
      voltajeSensor = voltajeSensor + analogRead(Sen_Brazo) * (5.0 / 1023.0); //lectura del sensor
    }
    float Sensibilidad = 0.100;
    I = (2.4685 - (voltajeSensor / cantidad)) / Sensibilidad; //Ecuación  para obtener la corriente
    sensor[2]=I*1000;
  }
}

void Posicion()
{
  //Funcion mediante la cual se sensa la posicion real del actuador.
  float R_0 = analogRead(Pos_Actuador);
  float R_1=(R_0*200)/1023;
  sensor[3] = R_1;
}

