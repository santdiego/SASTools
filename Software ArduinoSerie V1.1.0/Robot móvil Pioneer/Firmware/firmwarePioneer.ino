          #include <TimerThree.h>
          #include <TimerOne.h>
          //#include <SoftwareSerial.h>
          #include <TimerFour.h>
          #include <avr/wdt.h>//debug
          
          #define BRAKEVCC 0
          #define CW   1                   //sentido horario
          #define CCW  2                   //sentido antihorario
          #define BRAKEGND 3
          #define CS_THRESHOLD 100
          #define Ts 0.001                 // periodo de muestreo 1mS
          #define TENSION_MAX 11.9         //izquierdo
          #define TENSION_MAXN -11.9        //derecho
          #define dz_der 0.3
          #define dz_izq 0.2

          
          byte cantidad_sensores=8;
          byte cantidad_actuadores=2;

          //unsigned int cantidad_act=0,cantidad_sens=0;
          
          unsigned int envio[10];
          int inApin[2] = {7, 4};                                                               // INA: Entrada sentido horario
          int inBpin[2] = {8, 9};                                                               // INB:  Entrada sentido antihorario
          int pwmpin[2] = {5, 6};                                                               // PWM   
          int cspin[2] = {2, 3};                                                                // CS: sensor de corriente
          int enpin[2] = {0, 1};                                                                // EN: estado de pines de salida (Analog pin)
          int cont1=0,cont2=0,cont_v1=0,cont_v2=0,contador=0;
          int encabezado=0,i=0,j=0,k=0,id1=0,id2=0,cuenta1=0,cuenta2;  
          int canal_b1=20;                                                                                  // Canal B encoder 1
          int canal_b2=19;                                                                                  // Canal B encoder 2
          int led=38;
          
          bool evento_tx=0,evento_control=0,evento_rx=0,signo[2];    //banderas de control y de transmision
          
          char buffer[200];
          
          float angulo=0,angulo2=0,veloc_angular1=0,veloc_angular2=0,i1=0,i2=0,dif1[10];
          float promedio=0,diferencia=0,veloc1=0,veloc2=0,veloc1f=0,veloc2f=0, veloc1f_1=0, veloc2f_1=0;
          
          //Variables PID Lineal
          float y, rv, u,u2,uv,y1,u1,u22;//señal,referencia, acción de control  y1(k)=y(k-1)
          float Kp, Ki, Kd,deriva, Td; //Parametros PID + Td<1
          float integral, integral1;
          float e, e1,ef,ef1,d1,d;//error, termino derivativo  e1=e(k-1)  d1(k)=d(k-1)

           //Variables PID Angular
          float yw, rw, uw=0,u2w,y1w;//señal,referencia, acción de control  y1(k)=y(k-1)
          float Kpw, Kiw, Kdw, Tdw; //Parametros PID + Td<1
          float integralw, integral1w;
          float ew, e1w,d1w,dw,efw,efw1;//error, termino derivativo  e1=e(k-1)  d1(k)=d(k-1)

          unsigned int pwm1,pwm2,modo=0;
          byte inData[10];
          byte inData1[10];

          float prom_envio=0,dif_envio=0,uv_envio=0,uw_envio=0,vel1_envio=0,vel2_envio=0;
          float Sensibilidad=0, ImotorIzq, ImotorDer, VmotorIzq, VmotorDer;//EMANUEL
           
           //ISR(SERIAL_RX)
       
       
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


                   //Subrutinas de interrupcion y funciones
          void canal_a1(){
          
            if(digitalRead(canal_b1)==HIGH){
            cont_v1--;
            }
            else {
            cont_v1++;
            }
          
        
          }
          
          void canal_a2(){
       
            if(digitalRead(canal_b2)==HIGH){
          
            cont_v2--;
            }
            else { 
          
            cont_v2++;
            }
          
        
          }
          
          void reset_t1(){
           
            cuenta1=cont_v1;
            cuenta2=cont_v2;
            cont_v1=0;
            cont_v2=0;

            evento_control=1;
            contador++;

            if(contador>=10){
              contador=0;
   
            evento_tx=1;   //DEBUG_SERIE         
            }
          }
  
        
                  
          void setup() {
            Serial.begin(500000);//debug
            attachInterrupt(2,canal_a1,RISING); //interrupcion canal A E1
            attachInterrupt(5,canal_a2,RISING); //interrupcion canal A E2v
            Timer1.initialize(1000);            //ventana de tiempo 1mS
            Timer1.attachInterrupt(reset_t1);   //interrupcion timer 1
            pinMode(canal_b1,INPUT);
            pinMode(canal_b2,INPUT);
            pinMode(led,OUTPUT);
            Timer3.initialize(111);            //periodo de pwm 111us ---- 9 KHz
            Timer4.initialize(111);
          
           for (int j=0; j<2; j++)
            {
              pinMode(inApin[j], OUTPUT);
              pinMode(inBpin[j], OUTPUT);
              pinMode(pwmpin[j], OUTPUT);
              pinMode(cspin[j], INPUT);
           
              
            }
           Sensibilidad = 10.0;//EMANUEL

          // Parametros PID
            Td=1;        
            Kp=0.1;//0.4
            Ki=0.3;
            Kd=2;
            integral=0;
            e1=0;
            d1=0;

           // Parametros PID angular
          
            Tdw=0.6;      
            Kpw=0.35;
            Kiw=0.6;
            Kdw=0.9;
            integralw=0.0;
            e1w=0.0;
            d1w=0.0;
      }
          
          void loop() {
             
            if(evento_control==1){
                   
                  veloc1=cuenta1*5.124735;            //relacion 360/9850*1000ms=36.54 , 1°= 0.017rad , 8.25cm=radio de rueda
                  veloc2=cuenta2*5.124735;                  
                  veloc1f=veloc1f_1*0.99+veloc1*0.01;   //filtra velocidad  rueda derecha
                  veloc2f=veloc2f_1*0.99+veloc2*0.01; //rueda izquierda

                  veloc1f_1=veloc1f;
                  veloc2f_1=veloc2f;
       
                  promedio=(veloc1f+veloc2f)/2;             // velocidad promedio
                  diferencia=(57.3/32)*(veloc1f-veloc2f);   // velocidad angular en grad/seg; 57.3=360/2pi; L=32Cm
                 
                  y=promedio;
                  yw=diferencia;

                  //-------------------------------------------------------------EMANUEL

                  VmotorIzq = analogRead(A14) * (5.0 / 1023.0); //lectura del sensor

                  ImotorIzq = (VmotorIzq-2.5)*Sensibilidad; //Ecuación  para obtener la corriente

                  VmotorDer = analogRead(A15) * (5.0 / 1023.0); //lectura del sensor
    
                  
                   ImotorDer = (VmotorDer-2.5)*Sensibilidad; //Ecuación  para obtener la corriente

                  //------------------------------------------------------


                  
                 
                 if (modo==1){
                      
                   digitalWrite(led,HIGH);
                  
                  //Pid velocidad lineal
                   e=rv-y;          //calculo del error

                   ef=ef1*0.9+e*0.1; 
                   ef1=ef;

                  //Calculo de integral 
                   
                   if((e>30)||(e<-30)){
                      integral=0;
                   }
                   if((e<30)&&(e>-30)){
                    integral=integral1+((Ts*0.5)*(e+e1));
                   }
                
                 
                 //Cálculo de término derivativo
                   d=Td*((d1*0.9)-(Kd*0.1*(y-y1)));  
   
                  //Controlador PID
                   uv=Kp*ef+Ki*integral+Kd*d;

                  //actualizacion de variables
                  e1=e;
                  d1=d;
                  integral1=integral;
                  y1=y;

                   //Pid velocidad angular

                   
                 ew=rw-yw;          //calculo del error
                 efw=efw1*0.9+ew*0.1; 
                 efw1=efw;
                  //Calculo de integral 
                
                  integralw=integral1w+((Ts*0.5)*(ew+e1w));
                  
                
                 
                 //Cálculo de término derivativo
                   dw=Tdw*((0.9*d1w)-(0.1*Kdw*(yw-y1w)));
                      
                  //Controlador PID
                   uw=Kpw*efw+Kiw*integralw+Kdw*dw;
                   
                 
                  //actualizacion de variables
                  e1w=ew;
                  d1w=dw;
                  integral1w=integralw;
                  y1w=yw;
                     
                 uw=uw*0.08;

               
                 u=(uv+uw); // derecha
                 u2=(uv-uw); // izquierda
                 
                 //Compenzacion de zona muerta
                 
                if(u>0){
                 u=u+dz_der; // derecha
                  } 
                else{
                 u=u-dz_der; // derecha
                  } 

                  
                 if(u2>0){
                 u2=u2+dz_izq; // izquierda
                  } 
                 else{
                 u2=u2-dz_izq; // izquierda
                  } 
    
                    // Valor absoluto de u y u2
                    if(u<0){
                       u1=(-1)*u;
    
                    }else{
                      u1=u; 
                    }

                     if(u2<0){
                       u22=(-1)*u2;
                      
                    }else{
                      u22=u2; 
                    }
                   //mapeo de tension [volt] a PWM de 10 bits
                  
                   pwm1=(unsigned int)((float)u1*(1023.0/TENSION_MAX));       //u para derecho
                   pwm2=(unsigned int)((float)u22*(1023.0/TENSION_MAX));      //u2 para izquierdo


                    // PWM
                   if(modo>0){
                    if(u2>dz_izq || u2<-dz_izq){
                     Timer3.pwm(5,pwm2);          //izquierdo
                   } else {
                       Timer3.pwm(5,0);
                    }
                    if(u>dz_der || u<-dz_der){
                     Timer4.pwm(6,pwm1);          // derecho
                    } else {
                      Timer4.pwm(6,0);
                     }

                   // Sentido de giro de ruedas
                   

                   if(u2<0 ){
                     digitalWrite(inBpin[0], LOW); // horario
                     digitalWrite(inApin[0], HIGH);
                   }
                     if(u<0)
                     {
                     digitalWrite(inApin[1], LOW);  //antihorario
                     digitalWrite(inBpin[1], HIGH);
                     
                   }
                    if(u2>0){
                    digitalWrite(inBpin[0], HIGH); //antihorario
                    digitalWrite(inApin[0], LOW);
                   }
                    
                    if(u>0){
                 
                    digitalWrite(inApin[1], HIGH);// horario
                    digitalWrite(inBpin[1], LOW);   
                    
                                       
                    }    
                   
                   }//FIN MODO 1
                   
                   evento_control=0;

            }
    
                 if(modo==0){

                     digitalWrite(led,LOW);
                     digitalWrite(inApin[0], LOW);
                     digitalWrite(inBpin[0], LOW);
                     digitalWrite(inApin[1], LOW);
                     digitalWrite(inBpin[1], LOW);
                     pwm1=0;
                     pwm2=0;
                     integral=0;
                     integralw=0;
                     Timer3.pwm(5,0);
                     Timer4.pwm(6,0);
                     
                  }
              
              //Transmisión
              
            if(evento_tx==1){

                 
                Serial.write(255);    // envia cabecera FF FF
                Serial.write(255);
                
                Serial.write(modo);     //envia modo
                
                 //////////// velocidad promedio //////////////
                if(promedio<0){
                       prom_envio=(-1.00)*promedio;
    
                    }else{
                      prom_envio=promedio;
                    }

                envio[0]=(prom_envio*2*117.02857); //   //convierte float a uint 14 bits+1 signo REVISAR
       
                
                envio[0]=envio[0]& 0x7FFF; //DEBUG
    

                 if(promedio<0){

                   envio[0]=envio[0] | 0x8000;
                }
                
            ////////////// velocidad angular //////////////////
                if(yw<0){
                      dif_envio=(-1.00)*yw;
    
                    }else{
                      dif_envio=yw;
                    }

                               
                envio[1]=(dif_envio*2*81.92); //   //convierte float a uint 14 bits+1 signo   REVISAR
       
                envio[1]=envio[1]& 0x7FFF;
                
                if(yw<0){
 
                  envio[1]=envio[1] | 0x8000; 
                 }
                 
         ///////// velocidad rueda izquierda ///////////// 

               if(veloc2f<0){
                       vel2_envio=(-1.00)*veloc2f;
    
                    }else{
                      vel2_envio=veloc2f;
                    }
                    
                envio[2]=(vel2_envio*2*117.02857); //   //convierte float a uint 14 bits+1 signo  REVISAR
              
                envio[2]=envio[2]& 0x7FFF;
                
                 if(veloc2f<0){

                  envio[2]=envio[2] | 0x8000;
                  
                }
                
           ////////// velocidad rueda derecha////////////
               if(veloc1f<0){
                       vel1_envio=(-1.00)*veloc1f;
    
                    }else{
                      vel1_envio=veloc1f;
                    }
                    
                envio[3]=(vel1_envio*2*117.02857); //   //convierte float a uint 14 bits+1 signo REVISAR
       
                envio[3]=envio[3]& 0x7FFF;
                
                 if(veloc1f<0){

                  envio[3]=envio[3] | 0x8000;
                  
                }
           ////////////// accion de control lineal ///////////     
                if(uv<0){
                      uv_envio=(-1)*uv;
    
                    }else{
                      uv_envio=uv;
                    }
               envio[4]=(uv_envio*2*682.66); //   //convierte float a uint 14 bits+1 signo REVISAR
   
               envio[4]=envio[4]& 0x7FFF;

                 if(uv<0){
                  
                    envio[4]=envio[4] | 0x8000;
                   }
            ////////////// accion de control angular /////////////   

                if(uw<0){
                      uw_envio=(-1)*uw;
    
                    }else{
                      uw_envio=uw;
                    }
                envio[5]=(uw_envio*2*682.66); //   //convierte float a uint 14 bits+1 signo REVISAR
               
                envio[5]=envio[5]& 0x7FFF;

                if(uw<0){
                  
                  envio[5]=envio[5] | 0x8000;
                }

              ////////// corriente motor 1 y 2 EMANUEL///////////
              envio[6]=(unsigned int)(fabs(1000*ImotorIzq)) & 0x7FFF;
              envio[7]=(unsigned int)(fabs(1000*ImotorDer)) & 0x7FFF;
   
              if (ImotorIzq < 0) {
                envio[6] = envio[6] | 0x8000;
              }
              if (ImotorDer < 0) {
                envio[7] = envio[7] | 0x8000;
              }
          
             ////////////////////////////////////////////////
               
                   for(k=0; k<cantidad_sensores;k++){
            
                    Serial.write(highByte(envio[k]));
                    Serial.write(lowByte(envio[k]));
                     }

                Serial.write(0);       //fin trama

                  evento_tx=0;           
            }
                //Recepcion 
            if(evento_rx==1){
                         
     
                        modo=inData1[0];      // Recibe modo

                        cantidad_actuadores=inData1[1];   
                        cantidad_sensores=inData1[2];   
              
                      
                          rv=(float)256.00*(0x7F & inData1[4])+inData1[3]; 
                        
                          rw=(float)256.00*(0x7F & inData1[6])+inData1[5]; 
                       
                   
                        if((inData1[4]& 0x80)>0){ 
                            signo[0]=1;
                            rv=rv*(-1.00);
                        }else{
                           signo[0]=0;
                        }
                        
                    
                        if((inData1[6]& 0x80)>0){
                            signo[1]=1;
                            rw=rw*(-1.0);
                        }else{
                           signo[1]=0;
                        }   
                             evento_rx=0;
                             //evento_tx=1; //DEBUG 
                          
                          if(rv>70){
                            rv=70;
                          }
                          if(rv<-70){
                            rv=-70;
                          }
                          if(rv<5 && rv>-5){
                                 rv=0;
                          }
                        
              
          }
          }
          }// Fin Main

 void software_Reboot()
{
  /*wdt_enable(WDTO_15MS);
  while(1)
  {
  }*/
}
