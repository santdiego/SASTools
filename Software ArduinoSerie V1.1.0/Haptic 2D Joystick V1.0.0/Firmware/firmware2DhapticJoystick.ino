#include <TimerFour.h>

#include <TimerThree.h>

byte cantidad_sensores;
byte cantidad_actuadores;

//BUFFERS
unsigned int envio[20];
byte inData[20];
byte inData1[20];
float actuador[20];
float sensor[20];
float Ksensor[20];//mapeo
float Kactuador[20];//mapeo

float Zr;
float ZrB;
                                                            
int contador=0;
int encabezado=0,i=0,j=0,k=0;  

bool evento_tx=1,evento_control=0,evento_rx=0, evento_tarea_1=0;    //banderas de eventos-tareas

byte modo=0;



//------------------------------- ADLX335-------------------------------------------

#include <math.h>
 
int zx0=335;
int zy0=335;
int zz0=335;
int ax,ay,az;
float YY=0;
float vx;
float vy;
float vz; 
float gx,gy, gz,sensitivity,z,m,angleX,angleY,angleZ;
//------------------------------- galgas-------------------------------------------
int sensorValue = 0;        // value read from the pot
const int Puente_1 = A0;
const int Puente_2 = A1;
float val1, val2;
float peso1=0,peso2=0,peso21=0;
//--------------------------------Sensor de Corriente-------------------------------
float IA=0,Sensibilidad=0.100,Sensorfiltrado=0,SensorfiltradoB=0,SensorfiltradoB1=0,Sensorfiltrado1=0,IB=0;
float Iprom=0,Isuma=0,val11,sns,sns1;

//---------------------------------- motor -------------------------------------------
#define BRAKE 0
#define CW    1
#define CCW   2
#define CS_THRESHOLD 1000   // Definition of safety current (Check: "1.3 Monster Shield Example").

//MOTOR A
#define DIR_A 12
#define BRAKE_A 9
#define PWM_A 3
#define MOTOR_A 0
#define SNS_A A4
//MOTOR B
#define DIR_B 13
#define BRAKE_B 8
#define PWM_B 11
#define MOTOR_B 1
#define SNS_B A5


unsigned int usSpeed = 0;  //default motor speed
unsigned short usMotor_Status = BRAKE;

unsigned int usSpeedB = 0;  //default motor speed
unsigned short usMotor_StatusB = BRAKE;

//------------------------------controlador PD --------------------------------------------
#define TOLERANCIA 0.5
#define TENSION_MAX 30.0
#define Ts 0.001 //Periodo de muestreo de 1 ms

//***************** motor A ************************
//señal,referencia, acción de control y1(k)=y(k-1)
float Kp, Kd, Ki, Td, a,ap,ab,apB,integral=0,integral1=0; //Parametros PID + Td<1
float r,r1,y,y1,e,u,u1,e1,d1,d,df;//error, termino derivativo e1=e(k-1) d1(k)=d(k-1)

//**************** motor B *************************
float KpB, KdB,KiB, TdB, integralB=0,integral1B=0; //Parametros PID + Td<1
float rB,yB,yB1,eB,uB,u1B,e1B,d1B,dB,dfB,rB1;//error, termino derivativo e1=e(k-1) d1(k)=d(k-1)
           
//*************************************************************************************
//********** SERVICIO DE INTERRUPCION  ISR(SERIAL_RX)   *********************
//*************************************************************************************       
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


//*************************************************************************************
//********* XXX  INITIALIZATION AND SETUP DE LOS PINOS Y CONTROLADOR XXX  *************
//************************************************************************************* 
void setup() {
      Serial.begin(500000);
      Timer4.initialize(1000);            //ventana de tiempo 1mS
      Timer4.attachInterrupt(funcion_t4);   //interrupcion timer 4 para control y comunucaion
   
  //------------------------------- galgas-------------------------------------------
 analogReference(EXTERNAL);
  //------------------------------controlador PD -------------------------------------
 //***********************controlador PD Motor A *************************************
Stop();
Td=0.0515;
Kp=0.9;
Kd=0.01;
a=0.01;
e1=0.0;
d1=0.0;
Zr=85;
Ki=0.0;
ap=0.01;
//***********************controlador PD Motor B *************************************
StopB();
TdB=0.0515;
KpB=0.85;
KdB=0.050;
ab=0.01;
apB=0.01;
e1B=0.0;
d1B=0.0;
ZrB=75;
KiB=0.0;
//---------------------------------- motor -------------------------------------------

  pinMode(DIR_A, OUTPUT);
  pinMode(BRAKE_A, OUTPUT);

 pinMode(DIR_B, OUTPUT);
  pinMode(BRAKE_B, OUTPUT);

  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);

  pinMode(SNS_A, INPUT);
  pinMode(SNS_B, INPUT);
  
  Timer3.initialize(50);  //configura la frec del pwm para el pin 3
  Timer3.setPeriod(50);  
  Ksensor[0]=32768.0/52;//angulo y
  Ksensor[1]=32768.0/52;// referencia A
  Ksensor[2]=32768.0/10;//peso 2
  Ksensor[3]=32768.0/30;//Accion de control motor A
  Ksensor[4]=32768.0/3;//corriente IA
  Ksensor[5]=32768.0/47;// angulo z
  Ksensor[6]=32768.0/47;//referencia B
  Ksensor[7]=32768.0/3;// peso 1
  Ksensor[8]=32768.0/30; //Accion de control motor B
  Ksensor[9]=32768.0/3;//Corriente B
  Kactuador[0]=1.0;
 

}//----------------   FIM DO SETUP Y PARAMETROS ----------------------------- 

//*************************************************************************************
//***************************** XXX  MAIN  XXX  ***************************************
//*************************************************************************************          
void loop() {
  
  //---------------------------------------------------------------------------------------------------- 
 
   
 
  
  if(evento_control==1)          //CADA 1ms EJECUTA ESTE CODE - LAZO DE CONTROL
  {
 

       
      if (modo==1)   // MODO = 1 START
      {  
 //--------------------ADLX335-------------------------------------------------------------------------
 ax=analogRead(12);
 ay=analogRead(13);
 az=analogRead(14);
 vx = ax*3.3/1023;
 vy = ay*3.3/1023;
 vz = az*3.3/1023; 
 z= 1.5*3.3/3; 
 vx-=z;
 vy-=z;
 vz-=z; 
 sensitivity = 0.330; //sensitivity=300
 gx = vx*sensitivity;
 gy = vy*sensitivity;
 gz = vz*sensitivity;
 
 m=sqrt(gx*gx+gy*gy+gz*gz);
 angleX=acos(gx/m);
 angleY=acos(gy/m);
angleZ=acos(gz/m);

angleX=(angleX*180/3.14)-95;
angleY=(angleY*180/3.14)-95;
if (angleY <-50)angleY=-50.0;
if (angleY >50)angleY=50.0;
 angleZ=(angleZ*180/3.14)-95;  
 if (angleZ <-45)angleZ=-45.0;
if (angleZ >45)angleZ=45.0;
 y=angleY; 
 z=angleZ*(-1);     
  
//--------------------------------Sensor de Corriente-------------------------------
 Sensorfiltrado=filtroI(analogRead(SNS_A));
 Sensorfiltrado=Sensorfiltrado1*(1-0.1)+0.1*Sensorfiltrado;
 Sensorfiltrado1=Sensorfiltrado;
 IA=((Sensorfiltrado-24)*0.0044)-0.0745; //Ecuación  para obtener la corriente
 
 SensorfiltradoB=filtroIB(analogRead(SNS_B));
 SensorfiltradoB=SensorfiltradoB1*(1-0.1)+0.1*SensorfiltradoB;
 SensorfiltradoB1=SensorfiltradoB;
 IB=-0.0072+(SensorfiltradoB*0.0016); //Ecuación  para obtener la corriente

 //------------------------------- galgas-------------------------------------------
   val1 =analogRead(Puente_1);// Read strain 1
   val1=val11*(1-0.1)+0.1*val1;
   val11=val1;
   peso1=((0.0062)*(val1-514))-0.0051;
   peso1=filtrop(peso1);
   peso1=filtrop(peso1)*0.55;
   if (peso1>0.55)peso1=0.55;
   if (peso1<-0.55)peso1=-0.55;

   val2 =analogRead(Puente_2);// Read strain 2
   peso2=((-0.0061)*(val2-512))-0.0113;
   peso2=filtrop2(peso2);
   peso2=-1*peso2*0.55;
  
   if (peso2>.55)peso2=0.55;
   if (peso2<-0.55)peso2=-0.55;
   

------------------------------------------------------------------------



//conversion a grados de referencia
r = Zr*(peso2);
r=r1*(1-ap)+ap*r;
y=filtroy(y);
y=y1*(1-a)+a*y;
e=r-y;
integral=integral1+Ts*0.5*(e+e1);
d=Td*(d1-Kd*(y-y1));
df = filtrod(d);
u=Kp*e+Ki*integral-Kd*df;
//signo positivo o negativo (giro horario o anti-antihorario) o alta impedancia del puente H
if(abs(e)<TOLERANCIA){
}else{
if(u>=0){
Forward();
}else{
 u=u-8;
Reverse();
}
if(u<0.0){
u=u*(-1.0);
}
if(u>TENSION_MAX){
u=TENSION_MAX;
}
}
//actualizacion de variables
   e1=e;
   r1=r;
   y1 = y;
   u1 = u;
   d1=d;
 integral1=integral;
//mapeo de tension [volt] a PWM de 10 bits
usSpeed=(unsigned int)((u+7)*(1000/TENSION_MAX));

//********************************************************************************/

//conversion a grados de referencia
rB = ZrB*peso1;
rB=filtrorB(rB);
rB=rB1*(1-apB)+apB*rB;
yB=filtroyB(z);
yB=yB1*(1-ab)+ab*yB;  //0.2=a
yB=yB;
//Calculo de error
eB=rB-yB;
//Calculo de integral
integralB=integral1B+Ts*0.5*(eB+e1B);
//Cálculo de término derivativo
dB=Td*(d1B-KdB*(yB-yB1));
dfB = filtrodB(dB);
//Controlador PD
uB=KpB*eB+KiB*integralB-KdB*dfB;

if(abs(eB)<TOLERANCIA){

}else{
if(uB>=0){
  uB=uB;
ReverseB();
}else{
  uB=uB-0.25;
ForwardB();
}

if(uB<0.0){
  uB=uB*(-1.0);
}

if(uB>TENSION_MAX){
uB=TENSION_MAX;

}
}
//actualizacion de variables
  e1B=eB;
  yB1=yB;
  u1B=uB;
  d1B=dB;
  rB1=rB;
  integral1B=integralB;
//mapeo de tension [volt] a PWM de 10 bits
usSpeedB=(unsigned int)((uB+1)*(255/TENSION_MAX));





   
 //---------------------------------------Envio Valores-------------------------------------

         sensor[0]=y;
         sensor[1]=r;
         sensor[2]=peso2;
         sensor[3]=u;
         sensor[4]=IA;
         sensor[5]=yB;  
         sensor[6]=rB; 
         sensor[7]=peso1;
         sensor[8]=uB;
         sensor[9]=IB;
         
        
        
     
      }
    
           
    //*************** ||  MODO = 0  STOP  ||  ***************************      
      if(modo==0){ //STOP  
     
    Stop();
    StopB();
      }
       evento_control=0; 
   }

     if(evento_tarea_1==1)          //TAREA CADA 10 ms
  {
  // Iprom=Isuma/10;
  // Isuma=0; 
   evento_tarea_1=0;
  }
   

 
  //*************************************************************************************
  //*************** EVENTO DETECCION DE TRAMA   *************************************
  //************************************************************************************* 
      if(evento_rx==1){
          modo=inData1[0];                  // RECIBE MODO      
          cantidad_actuadores=inData1[1];   // RECIBE CANTIDAD DE AcTUADORES
          cantidad_sensores=inData1[2];     // RECIBE CANTIDAD DE SENSORES

           for(k=0; k<cantidad_actuadores;k++){     
                   actuador[k]=((float)256.00*(0x7F & inData1[2*k+4])+inData1[2*k+3]);  //arma dato de 15 bits de magnitud
                   if((inData1[2*k+4]& 0x80)>0){ // signo en bit 16
                      actuador[k]=Kactuador[k]*actuador[k]*(-1.0);
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
            
            
            
            envio[k]=  (unsigned int)(sensor[k]*Ksensor[k]) ;  
            if(sensor[k]<0){ 
              envio[k]=-1.0*envio[k];
              envio[k]=(envio[k] & 0x7FFF )|  0x8000;
             } else{
              envio[k]=envio[k] & 0x7FFF;
             }
            
            Serial.write(highByte(envio[k]));
            Serial.write(lowByte(envio[k]));
          }          
          Serial.write(0);       //fin trama
       
         
      evento_tx=0;   
          
   }
    
}//----------------   FIM MAIN --------------------------------- 

//******************************* motor A ***************************
void Stop()
{

  Timer3.pwm(PWM_A,0);

 motorGo(MOTOR_B, usMotor_Status, 0);
}

void Forward()
{

  usMotor_Status = CW;
 motorGo(MOTOR_A, usMotor_Status, usSpeed);

}

void Reverse()
{
  usMotor_Status = CCW;

  motorGo(MOTOR_A,usMotor_Status, usSpeed);
  
}

//******************************* motor B ***************************
void StopB()
{

 analogWrite (PWM_B,0);

}

void ForwardB()
{

  usMotor_Status = CW;
 motorGo(MOTOR_B, usMotor_Status, usSpeedB);

}

void ReverseB()
{

  usMotor_Status = CCW;

  motorGo(MOTOR_B,usMotor_Status, usSpeedB);
  
}


void motorGo(uint8_t motor, uint8_t direct, unsigned int pwm)         //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
  
  if(motor == MOTOR_A)
  {
    if(direct == CW)
    {
       digitalWrite(DIR_A, LOW);
       digitalWrite(BRAKE_A, LOW);
    }
    if(direct == CCW)
    {
      digitalWrite(DIR_A, HIGH);
       digitalWrite(BRAKE_A, LOW);     
    }
    if(direct == BRAKE)
    {
      digitalWrite(DIR_A, HIGH);
      digitalWrite(BRAKE_A, HIGH); 
   
               
    }
   Timer3.pwm(PWM_A,usSpeed);
  }
 //**************************************motor B****************************
   if(motor == MOTOR_B)
  {
    if(direct == CW)
    {
      
       digitalWrite(DIR_B, LOW);
       digitalWrite(BRAKE_B, LOW);
    }
    if(direct == CCW)
    {
      
      digitalWrite(DIR_B, HIGH);
       digitalWrite(BRAKE_B, LOW);     
    }
    if(direct == BRAKE)
    {
     
      digitalWrite(DIR_B, HIGH);
      digitalWrite(BRAKE_B, HIGH);          
    }
   
  
analogWrite (PWM_B,usSpeedB) ;

  }
}



//*************************************************************************************
//****************** XXX  INTERRUPCION TIMER DE 1ms  XXX  ********************************
//*************************************************************************************
void funcion_t4()
{     

   
      evento_control=1;
      contador++;
      if(contador>=10){
        evento_tarea_1=1;//tarea cada 10 ms
        contador=0;
      }
 

} 


float filtroI(float i){
static float p[30];
float y;
int j;

p[0]=i;
y=0.0;

y=0.033*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]+p[10]+p[11]+p[12]+p[13]+p[14]+p[15]+p[16]+p[17]+p[18]+p[19]+p[20]+p[21]+p[22]+p[23]+p[24]+p[25]+p[26]+p[27]+p[28]+p[29]);
p[29]=p[28];
p[28]=p[27];
p[27]=p[26];
p[26]=p[25];
p[25]=p[24];
p[24]=p[23];
p[23]=p[22];
p[22]=p[21];
p[21]=p[20];
p[20]=p[19];
p[19]=p[18];
p[18]=p[17];
p[17]=p[16];
p[16]=p[15];
p[15]=p[14];
p[14]=p[13];
p[13]=p[12];
p[12]=p[11];
p[11]=p[10];
p[10]=p[9];
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;

}
float filtroIB(float i){

static float p[30];
float y;
int j;

p[0]=i;
y=0.0;

y=0.033*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]+p[10]+p[11]+p[12]+p[13]+p[14]+p[15]+p[16]+p[17]+p[18]+p[19]+p[20]+p[21]+p[22]+p[23]+p[24]+p[25]+p[26]+p[27]+p[28]+p[29]);
p[29]=p[28];
p[28]=p[27];
p[27]=p[26];
p[26]=p[25];
p[25]=p[24];
p[24]=p[23];
p[23]=p[22];
p[22]=p[21];
p[21]=p[20];
p[20]=p[19];
p[19]=p[18];
p[18]=p[17];
p[17]=p[16];
p[16]=p[15];
p[15]=p[14];
p[14]=p[13];
p[13]=p[12];
p[12]=p[11];
p[11]=p[10];
p[10]=p[9];
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;
}
float filtroy(float i){

static float p[20];
float y;
int j;

p[0]=i;
y=0.0;

y=0.05*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]+p[10]+p[11]+p[12]+p[13]+p[14]+p[15]+p[16]+p[17]+p[18]+p[19]);

p[19]=p[18];
p[18]=p[17];
p[17]=p[16];
p[16]=p[15];
p[15]=p[14];
p[14]=p[13];
p[13]=p[12];
p[12]=p[11];
p[11]=p[10];
p[10]=p[9];
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;

}
float filtrop(float i){

static float p[10];
float y;
int j;

p[0]=i;
y=0.0;
y=0.1*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]);
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;

}
float filtrop2(float i){

static float p[10];
float y;
int j;

p[0]=i;
y=0.0;
y=0.1*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]);
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;

}
float filtroyB(float i){

static float p[20];
float y;
int j;

p[0]=i;
y=0.0;
y=0.05*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]+p[10]+p[11]+p[12]+p[13]+p[14]+p[15]+p[16]+p[17]+p[18]+p[19]);

p[19]=p[18];
p[18]=p[17];
p[17]=p[16];
p[16]=p[15];
p[15]=p[14];
p[14]=p[13];
p[13]=p[12];
p[12]=p[11];
p[11]=p[10];
p[10]=p[9];
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;

}
float filtrorB(float i){

static float p[20];
float y;
int j;

p[0]=i;
y=0.0;
y=0.05*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]+p[10]+p[11]+p[12]+p[13]+p[14]+p[15]+p[16]+p[17]+p[18]+p[19]);

p[19]=p[18];
p[18]=p[17];
p[17]=p[16];
p[16]=p[15];
p[15]=p[14];
p[14]=p[13];
p[13]=p[12];
p[12]=p[11];
p[11]=p[10];
p[10]=p[9];
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];


 return y;

}
   float filtrod(float i){

static float p[10];
float y;
int j;

p[0]=i;
y=0.0;
y=0.1*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]);
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;

}

 float filtrodB(float i){

static float p[10];
float y;
int j;

p[0]=i;
y=0.0;
y=0.1*(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]+p[8]+p[9]);
p[9]=p[8];
p[8]=p[7];
p[7]=p[6];
p[6]=p[5];
p[5]=p[4];
p[4]=p[3];
p[3]=p[2];
p[2]=p[1];
p[1]=p[0];

 return y;

}  
 
