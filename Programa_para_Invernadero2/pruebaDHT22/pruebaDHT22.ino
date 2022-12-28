#include <DHT.h>
#include <DHT_U.h>

int pin=4;
float temp;
float hum;

DHT dht(pin,DHT22);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  
  if (isnan(hum) || isnan(temp)) {
    Serial.println("EstadoDHT22:" + String("Desconectado"));
    Serial.println("Temperatura:0");
    Serial.println("Humedad:0");
    delay(1000);
    return;
  }else{
  //La comunicacion con Python se basa con cadenas
  Serial.println("EstadoDHT22:" + String("Conectado"));
  Serial.println("Temperatura:" + String(temp));
  Serial.println("Humedad:" + String(hum));
  delay(1000);
  }
  
}
