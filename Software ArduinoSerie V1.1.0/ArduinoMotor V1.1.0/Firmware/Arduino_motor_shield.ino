
#include <AFMotor.h>
#include <TimerOne.h>
#define SATURACION 254.0
#define BUFFER_SIZE 50
#define N 4


byte pwm_motor[4];
unsigned int sensor[N];
unsigned int u[N];
unsigned int envio;
bool evento_control=0;
bool evento_tx=0;
bool evento_rx=0;
bool signo[N];
byte modo=1;//1->normal, 2->escribir, 3->leer
byte inData[BUFFER_SIZE];
byte inData1[BUFFER_SIZE];
byte i=0;
byte j=0;
byte k=0;
byte encabezado=0;
byte cantidad_actuadores=0;
byte cantidad_sensores=0;


AF_DCMotor motor1(1, MOTOR12_8KHZ);
AF_DCMotor motor2(2, MOTOR12_8KHZ);
AF_DCMotor motor3(3, MOTOR34_8KHZ);
AF_DCMotor motor4(4, MOTOR34_8KHZ);

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
}//---------------- ------------------------- 

void setup() 
{

  motor1.setSpeed(0);
  motor1.run(RELEASE);
  motor2.setSpeed(0);
  motor2.run(RELEASE);
  motor3.setSpeed(0);
  motor3.run(RELEASE);
  motor4.setSpeed(0);
  motor4.run(RELEASE);


  Serial.begin(256000);

  
  Timer1.initialize(1000); //timer de 1 ms
  Timer1.attachInterrupt( timerIsr ); 

  modo=0;

}


void loop()
{


  if(evento_control==1)
  {
    sensor[0]=analogRead(A0); //[0-1023]

    sensor[1]=analogRead(A1);
    
    sensor[2]=analogRead(A2);
    
    sensor[3]=analogRead(A3);
 
 
    for(j=0;j<4;j++){

        if(u[j]>SATURACION){
            u[j]= SATURACION;
        }
        
    }

    
  
   
  
  if(modo>0){
    
    //----------MOTOR 1----------------------------
      if(u[0]==0.0){
        motor1.run(RELEASE);
      }else{
        if(signo[0]==1){
          motor1.run(FORWARD);
        }else{
          motor1.run(BACKWARD);
        }
        
      }
      pwm_motor[0]=(byte)(u[0]);
      motor1.setSpeed(pwm_motor[0]);

  //-------------------------------------------------

  //----------MOTOR 2-----------------------------------
      
      if(u[1]==0.0){
        motor2.run(RELEASE);
      }else{
        if(signo[1]==1){
          motor2.run(FORWARD);
        }else{
          motor2.run(BACKWARD);
        }    
      }
    
      pwm_motor[1]=(byte)(u[1]);
      motor2.setSpeed(pwm_motor[1]);

   //------------------------------------------------------

   
  //----------MOTOR 3-----------------------------------
      if(u[2]==0.0){
        motor3.run(RELEASE);
      }else{
        if(signo[2]==1){
          motor3.run(FORWARD);
        }else{
          motor3.run(BACKWARD);
        }
        
      }

      pwm_motor[2]=(byte)(u[2]);
      motor3.setSpeed(pwm_motor[2]);

      //-------------------------------------------------------

         
  //----------MOTOR 4-----------------------------------
      if(u[3]==0.0){
        motor4.run(RELEASE);
      }else{
        if(signo[3]==1){
          motor4.run(FORWARD);
        }else{
          motor4.run(BACKWARD);
        }
        
      }

      pwm_motor[3]=(byte)(u[3]);
      motor4.setSpeed(pwm_motor[3]);

      //-------------------------------------------------------
  }else{
    
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);

    motor1.setSpeed(0);
    motor2.setSpeed(0);
    motor3.setSpeed(0);
    motor4.setSpeed(0);
    
  }

     evento_control=0;
  
  }



  if(evento_rx==1){



     for(k=0; k<cantidad_actuadores;k++){     
             u[k]=(unsigned int)(256*(0x7F & inData1[2*k+4])+inData1[2*k+3]);  //arma dato de 15 bits de magnitud
             if((inData1[2*k+4]& 0x80)>0){ // signo en bit 16
                      signo[k]=1;
             }else{
                      signo[k]=0;
             }
                   
     }


    evento_rx=0;
    evento_tx=1;
   
  }
  if(evento_tx==1){

   
    Serial.write(255);
    Serial.write(255);
    
    Serial.write(modo);
    
    for(k=0; k<cantidad_sensores;k++){
      
            Serial.write(highByte(sensor[k]));
            Serial.write(lowByte(sensor[k]));
    } 
    
    Serial.write(0);

    evento_tx=0;
    
  }
    
     
 
  
}


 
/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
    evento_control=1;

    
}


