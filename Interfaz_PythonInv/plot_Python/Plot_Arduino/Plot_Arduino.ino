const int analogPin1 = A0; //Define el Pin A0
int sensorValue = 0;
float voltageValue = 0;
unsigned long lastTime = 0, sampleTime = 200; //Variables de Tempo

void setup(){
  
  Serial.begin(9600); // Inicializamos puerto Serie

  }

void loop(){
  
  if (millis()-lastTime > sampleTime){ // Estableser Tiempo de Muestreo
    lastTime = millis();
    
    sensorValue = analogRead(analogPin1); // leemos el pin analogico

    voltageValue = scaling(sensorValue, 0, 1023, 0, 5); // Escalamos la lectura a Voltios

    Serial.println(voltageValue); // Imprime el resultado
    
    }
  }

// Funcion para convertir el valor del pot a voltios

float scaling(float x, float in_min, float in_max, float out_min, float out_max){
  
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  
  }
