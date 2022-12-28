#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>

//Sensores YL-69
#define pinS1 34
#define pinS2 35

//Sensores DHT
#define pinDHT1 4
#define pinDHT2 15                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              

float temp1,temp2;
float hum1,hum2;


DHT dht1(pinDHT1,DHT22);
DHT dht2(pinDHT2,DHT22);

void setup() {
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
}

void loop() {
  int lectura1 = map(analogRead(pinS1),4095,778,0,100);
  int lectura2 = map(analogRead(pinS2),4095,778,0,100);

  //int lectura1 = analogRead(pinS1);
  //int lectura2 = analogRead(pinS2);
  
  //int lectura1 = random(0,100);
  //int lectura2 = random(0,100);
  
  
  temp1 = dht1.readTemperature();
  hum1 = dht1.readHumidity();
  temp2 = dht2.readTemperature();
  hum2 = dht2.readHumidity();
  
  if (isnan(hum1) || isnan(temp1)) {
    Serial.println("EstadoDHT22(1):" + String("Desconectado"));
    Serial.println("Temperatura(1):0");
    Serial.println("Humedad(1):0");
    
  }else{
  //La comunicacion con Python se basa con cadenas
  Serial.println("EstadoDHT22(1):" + String("Conectado"));
  Serial.println("Temperatura(1):" + String(temp1));
  Serial.println("Humedad(1):" + String(hum1));
  
  }

  
  if (isnan(hum2) || isnan(temp2)) {
    Serial.println("EstadoDHT22(2):" + String("Desconectado"));
    Serial.println("Temperatura(2):0");
    Serial.println("Humedad(2):0");
    
  }else{
  //La comunicacion con Python se basa con cadenas
  Serial.println("EstadoDHT22(2):" + String("Conectado"));
  Serial.println("Temperatura(2):" + String(temp2));
  Serial.println("Humedad(2):" + String(hum2));
  
  }

  Serial.println("humedadSuperficial1:" + String(lectura1));
  Serial.println("humedadSuperficial2:" + String(lectura2));
  
  
  delay(1000);
  
}
