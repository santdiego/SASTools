#define TWI_FREQ 400000L


//--------------------------DIRECIONES EEPROM---------------------------------
// Parametros PID

#define Q1_ADDR       20 
#define Q2_ADDR       40 
#define Q3_ADDR       60 
#define Q4_ADDR       80 
#define Q5_ADDR       100 
#define Q6_ADDR       120 
#define Q7_ADDR       140 
#define Q0_ADDR       160 
 

//Parametros ConfigRobot
#define FORMATO_ADDR  200
#define MODO_ADDR     205
#define AWIND_ADDR    210
#define PIDRST_ADDR   215
//---------------------------------------------------------------------------
#include <TimerFour.h>
//#include <TimerThree.h> //DEBUG
//#include <TimerOne.h> //DEBUG
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <EEPROM.h>

#include <AsciiCommand.h>
#include <Qrobot.h>
#include <ConfigRobot.h>
#include <avr/wdt.h>


#define STOP  0
#define PAUSE 2
#define PLAY  1
#define OFF   3

#define MOD0  200
#define MOD1  201

#define A2DC 0
#define ADAF1 1
#define ADAF2 2

byte cantidad_sensores=2;
byte cantidad_actuadores=2;

//BUFFERS
unsigned int envio[20];
byte inData[20];
byte inData1[20];
unsigned int actuador[20];
unsigned int sensor[20];
bool signo[20];

                                                            
int contador=0;
int encabezado=0,i=0,j=0,k=0;  

bool evento_tx=0,evento_control=0,evento_rx=0, evento_tarea_1=0;    //banderas de eventos-tareas

byte modo=0;


//*******Variables del programa**************
bool _ts = 0;
bool _tx = 0;
int cont=0;


//Banderas
bool _fd = 0;
bool _rx = 0;
bool _rc = 0;
//-------------------------------------------


//*******Variables del robot*******
//-----------------------------q0 (motor B)
const int q0_posMax       = 904;
const int q0_posMin       = 101;
const int q0_PIN_pwm      = 11;
const int q0_PIN_pote     = 8;
const int q0_PIN_dir      = 13;
const int q0_PIN_brk      = 8;
uint16_t  u0;


//------------------------------q1_A
const int q1_A_posMax     = 879;
const int q1_A_posMin     = 77;
const int q1_A_PIN_pote   = 10;
uint16_t u1;
//------------------------------q1_B
const int q1_B_posMax     = 879;
const int q1_B_posMin     = 77;
const int q1_B_PIN_pote   = 9;

//------------------------------q2 (motor A)
const int q2_posMax       = 938;
const int q2_posMin       = 143;
const int q2_PIN_pwm      = 3;
const int q2_PIN_pote     = 14;
const int q2_PIN_dir      = 12;
const int q2_PIN_brk      = 9;
uint16_t u2;
//------------------------------q3
const int q3_posMax       = 829;
const int q3_posMin       = 71;
const int q3_PIN_pote     = 15;
uint16_t u3;
//------------------------------q4
const int q4_posMax       = 905;
const int q4_posMin       = 84;
const int q4_PIN_pote     = 9;
uint16_t u4;

//------------------------------q5
const int q5_posMax       = 879;
const int q5_posMin       = 85;
const int q5_PIN_pote     = 11;
uint16_t u5;

//------------------------------q6
const int q6_posMax       = 929;
const int q6_posMin       = 88;
const int q6_PIN_pote     = 13;
uint16_t u6;
//------------------------------q7
const int q7_posMax       = 959;
const int q7_posMin       = 386;
const int q7_PIN_pote     = 12;
uint16_t u7;


//****************************************************Objetos Motor

Adafruit_MotorShield MotorDC1 = Adafruit_MotorShield();
Adafruit_DCMotor *mq7 = MotorDC1.getMotor(1);
Adafruit_DCMotor *mq6 = MotorDC1.getMotor(2);
Adafruit_DCMotor *mq1_A =  MotorDC1.getMotor(3);
Adafruit_DCMotor *mq1_B =  MotorDC1.getMotor(4);

Adafruit_MotorShield MotorDC2 = Adafruit_MotorShield(0x61);
Adafruit_DCMotor *mq5 = MotorDC2.getMotor(1);
Adafruit_DCMotor *mq4 = MotorDC2.getMotor(2);
Adafruit_DCMotor *mq3 = MotorDC2.getMotor(4);


ConfigRobot    robot = ConfigRobot ();
AsciiCommand command = AsciiCommand();
//Qwrite adaf1 = Qwrite(0x60);
//Qwrite adaf2 = Qwrite(0x61);
//Wire.setclock(400000L); //por defecto I2C esta en 100K pero se puede utilizart en 400K


Qrobot q0 =   Qrobot(q0_posMin, q0_posMax, q0_PIN_pwm, q0_PIN_pote, q0_PIN_dir, q0_PIN_brk, Q0_ADDR);
Qrobot q2 =   Qrobot(q2_posMin, q2_posMax, q2_PIN_pwm, q2_PIN_pote, q2_PIN_dir, q2_PIN_brk, Q2_ADDR);

Qrobot q1_A = Qrobot(q1_A_posMin, q1_A_posMax, q1_A_PIN_pote, Q1_ADDR);
Qrobot q1_B = Qrobot(q1_B_posMin, q1_B_posMax, q1_B_PIN_pote, Q1_ADDR);
Qrobot q3 =   Qrobot(q3_posMin,   q3_posMax,   q3_PIN_pote,   Q3_ADDR);
Qrobot q4 =   Qrobot(q4_posMin,   q4_posMax,   q4_PIN_pote,   Q4_ADDR);

Qrobot q5 =   Qrobot(q5_posMin,   q5_posMax,   q5_PIN_pote,   Q5_ADDR);
Qrobot q6 =   Qrobot(q6_posMin,   q6_posMax,   q6_PIN_pote,   Q6_ADDR);
Qrobot q7 =   Qrobot(q7_posMin,   q7_posMax,   q7_PIN_pote,   Q7_ADDR);


// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//-----------------------------------------------------------------------------------------------------------

void setup()
{
  /*byte value;
  value=TCCR1B;
  TCCR1B=(value&0xF8)|0x01;

  value=TCCR3B;
  TCCR3B=(value&0xF8)|0x01;*/
  
  Serial.begin(500000);
  MotorDC1.begin(1500);
  MotorDC2.begin(1500);


  pinMode(q0_PIN_dir, OUTPUT);
  pinMode(q2_PIN_dir, OUTPUT);
  pinMode(q0_PIN_brk, OUTPUT);
  pinMode(q2_PIN_brk, OUTPUT);

  Timer4.initialize     (robot.ts * 1000); //Timer de 1ms para tiempo muestreo DEBUG
  Timer4.attachInterrupt( timerTs ); //DEBUG
  //Timer3.initialize     (667);             // 667 us = 1499 Hz DEBUG

  
//  set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  ADCSRA |= PS_64;    // set our own prescaler to 64 

  robot.run=1;  //Para arrancar en modo RUN
 
}

void timerTs()
{
  if (robot.run)
    _ts = 1;
}



void loop()
{
  
    //************************************************************************************* 
      if(evento_rx==1){
          modo=inData1[0];                  // RECIBE MODO      
          cantidad_actuadores=inData1[1];   // RECIBE CANTIDAD DE AcTUADORES
          cantidad_sensores=inData1[2];     // RECIBE CANTIDAD DE SENSORES

           for(k=0; k<cantidad_actuadores;k++){     
                   actuador[k]=(unsigned int)(256*inData1[2*k+4]+inData1[2*k+3]);  //arma dato de 15 bits de magnitud
                   if((inData1[2*k+4]& 0x80)>0){ // signo en bit 16
                      signo[k]=1;
                   }else{
                      signo[k]=0;
                   }
                   
           }
       

      
      evento_rx=0; // RESET DE BANDERA RX TRAMA
      evento_tx=1; // ACTIVA BANDERA DE TX
              
  }
  
  //*************************************************************************************
  //*************** XXX  TRANSMIION DE DATOS  XXX  *************************************
  //*************************************************************************************  
      if(evento_tx==1)
      {   
       
          Serial.write(255);    // envia cabecera FF FF
          Serial.write(255);    // envia cabecera FF FF  
          Serial.write(modo);   //envia modo 
          
        
          for(k=0; k<cantidad_sensores;k++){
              
            envio[k]=  (unsigned int)(sensor[k]& 0x7FFF);  
            //if(sensor[k]<0){ envio[0]=envio[0] | 0x8000;}  
            Serial.write(highByte(envio[k]));
            Serial.write(lowByte(envio[k]));
          }          
          Serial.write(0);       //fin trama
         
      evento_tx=0;       
   }


  if (robot.run==0){
     allQwrite(0, 0, 0, 0, 0, 0, 0, 0);
  }
  if (_ts && robot.run)
  {


    sensor[0] = q0.read();
    sensor[1] = q1_A.read();
    sensor[2] = q2.read();
    sensor[3]= q3  .read();
    sensor[4]= q4  .read();
    sensor[5] = q5  .read();
    sensor[6] = q6  .read();
    sensor[7] = q7  .read();
  
    byte ss = modo;
  
      //int tpo = micros();     
      switch((int)ss)
      {
      case PLAY:
        allQwrite(actuador[0], actuador[1], actuador[2], actuador[3], actuador[4], actuador[5], actuador[6], actuador[7]);
   
        break;
        
      case STOP:
        allQwrite(0, 0, 0, 0, 0, 0, 0, 0);
        break;
        
      case OFF: 

        allQwrite(0, 0, 0, 0, 0, 0, 0, 0);
        q0.release();
        q1_A.release(mq1_A);
        q1_B.release(mq1_B);
        q2.release();
        q3.release(mq3);
        q4.release(mq4);
        q5.release(mq5);
        q6.release(mq6);
        q7.release(mq7);

        robotOff();       
        break;
              

      default:
        allQwrite(0, 0, 0, 0, 0, 0, 0, 0);
        break;        
      }
        _ts = 0;
 
               
 
    }
        
 

}//Fin de loop

void software_Reboot()
{
  wdt_enable(WDTO_15MS);
  while(1)
  {
  }
}




void robotOff()
{
  allQwrite(0, 0, 0, 0, 0, 0, 0, 0);
  //robot.run=0;
  delay(1000);
  software_Reboot();     
}



void allQwrite(uint16_t u0, uint16_t u1, uint16_t u2, uint16_t u3, uint16_t u4, uint16_t u5, uint16_t u6, uint16_t u7)
{

  q0.  write(u0);
  q1_A.write(u1,  mq1_A);
  q1_B.write(u1,  mq1_B);
  q2.  write(u2);
  q3.  write(u3 , mq3);
  q4.  write(u4 , mq4);
  q5.  write(u5 , mq5);
  q6.  write(u6 , mq6);
  q7.  write(u7 , mq7);
   
}

//------------------------------------------------------------------------------

void serialEvent(){ 
      inData[i] = Serial.read();  //LEE UN BYTE
      
   //******************** || SEPARANDO LOS DADOS RECIBIDOS || *************************          
          if(encabezado<2)
          {     if(inData[i]==255)
                { encabezado=encabezado+1;
                }else
                { i=0;encabezado=0;}
          }else
          { 
                i=i+1;
                if(i<=3){
                   if(i==1){
                    modo=inData[0];
                  }
                  if(i==2){
                    cantidad_actuadores=inData[1];
                  }
                   if(i==3){
                    cantidad_sensores=inData[2];
                  }
                }else{
                      if(i>=(4+2*cantidad_actuadores))
                      {  if(inData[3+2*cantidad_actuadores]==0)
                         {  for(k=0; k<(2*cantidad_actuadores+3);k++)
                            {inData1[k]=inData[k];}
                            evento_rx=1;
                         } 
                        i=0;
                        encabezado=0;
                      }
                }
          } 
}
//---------------- ------------------------- 






