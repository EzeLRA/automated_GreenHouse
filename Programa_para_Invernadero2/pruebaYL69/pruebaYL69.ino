#define pinS1 1
#define pinS2 2

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  int lectura1 = map(analogRead(pinS1),1023,0,0,100);
  int lectura2 = map(analogRead(pinS2),1023,0,0,100);

  Serial.println("humedadSuperficial1:" + String(lectura1));
  Serial.println("humedadSuperficial2:" + String(lectura2));
  
  delay(1000);
}
