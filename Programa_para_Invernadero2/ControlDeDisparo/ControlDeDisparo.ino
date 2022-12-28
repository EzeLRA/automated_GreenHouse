#include <TimerOne.h>
volatile int i=0;
volatile boolean cruce_cero=0;
int dim;
int T_int=100;

#define triac 3

void setup(){
  pinMode(triac,OUTPUT);
  attachInterrupt(0,deteccion_cruceCero,RISING);
  Timer1.initialize(T_int);
  Timer1.attachInterrupt(Dimer);
}

void deteccion_cruceCero(){
  cruce_cero=true;
  i=0;
  digitalWrite(triac,LOW);
}

void Dimer(){
  if(cruce_cero==true){
    if(i>=dim){
    digitalWrite(triac,HIGH);
    }else{
    i++;
    }
  }
}

void loop(){
  dim=map(analogRead(A0),0,1023,0,83);
}
