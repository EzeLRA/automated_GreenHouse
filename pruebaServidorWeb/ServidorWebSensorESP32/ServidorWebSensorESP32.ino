#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>

//Sensores YL-69
#define pinS1 34
#define pinS2 35

//Sensores DHT
#define pinDHT1 15
#define pinDHT2 4

float temp1,temp2;
float hum1,hum2;


DHT dht1(pinDHT1,DHT22);
DHT dht2(pinDHT2,DHT22);


//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);

//---------------------Credenciales de WiFi-----------------------

const char* ssid     = "Invernadero";
//const char* password = ""; (Opcional)

//---------------------VARIABLES GLOBALES-------------------------
int contconexion = 0;

String header; // Variable para guardar el HTTP request

//------------------------CODIGO HTML------------------------------
String paginaInicio = "<!DOCTYPE html>"
"<html>"
"<style type='text/css' media='screen'>table,th,td{border: 1px solid black;}</style>"
"<head>"
"<meta charset='utf-8' />"
"<META HTTP-EQUIV='Refresh' CONTENT='1'>"
"<title>Servidor Web ESP32</title>"
"</head>"
"<body>"
"<h1><p ALIGN='LEFT'>Gestor de ESP32</p></h1><hr>&nbsp</hr>";

String paginaFin =
"</body>"
"</html>";


//---------------------------SETUP--------------------------------
void setup() {
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  
  Serial.println("");
  
  // Conexión WIFI
  //WiFi.begin(ssid, password);  (Opcional)
  WiFi.begin(ssid);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { 
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      //IPAddress ip(192,168,1,180); 
      //IPAddress gateway(192,168,1,1); 
      //IPAddress subnet(255,255,255,0); 
      //WiFi.config(ip, gateway, subnet); 
      
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      server.begin(); // iniciamos el servidor
  }
  else{ 
      Serial.println("");
      Serial.println("Error de conexion");
  }
}

//----------------------------LOOP----------------------------------

void loop(){
  int lectura1 = map(analogRead(pinS1),4095,1065,0,100);
  int lectura2 = map(analogRead(pinS2),4095,1065,0,100);

  temp1 = dht1.readTemperature();
  hum1 = dht1.readHumidity();
  temp2 = dht2.readTemperature();
  hum2 = dht2.readHumidity();
  
  WiFiClient client = server.available();   // Escucha a los clientes entrantes

  if (client) {                             // Si se conecta un nuevo cliente
    Serial.println("New Client.");          // 
    String currentLine = "";                //
    while (client.connected()) {            // loop mientras el cliente está conectado
      if (client.available()) {             // si hay bytes para leer desde el cliente
        char c = client.read();             // lee un byte
        Serial.write(c);                    // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del 
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                     
            // Muestra la página web
            client.println(paginaInicio); 


              //Inicio de la tabla de contenido
            client.println("<table>");
  
            client.println("<tr><th scope='col'><strong>Parametros</strong></th><td type='text/css' align='CENTER' colspan='4'><strong>Sensores</strong></td></tr>");
            client.println("<tr><th scope='col'><strong>&nbsp</strong></th>");
            client.println("<td><strong>&nbsp DHT22(1) &nbsp</strong></td>");
            client.println("<td><strong>&nbsp DHT22(2) &nbsp</strong></td>");
            client.println("<td><strong>&nbsp YL-69(1) &nbsp</strong></td>");
            client.println("<td><strong>&nbsp YL-69(2) &nbsp</strong></td></tr>");

              //Temperatura
            client.println("<tr><th scope='col'><strong>Temperatura(°C)</strong></th>");
            client.println("<td type='text/css' align='CENTER'>" + String(temp1) + "</td>");
            client.println("<td type='text/css' align='CENTER'>" + String(temp2) + "</td>");
            client.println("<td type='text/css' align='CENTER'>---</td>");
            client.println("<td type='text/css' align='CENTER'>---</td>");
            client.println("</tr>");
              
              //Humedad
            client.println("<tr><th scope='col'><strong>Humedad(%)</strong></th>");
            client.println("<td type='text/css' align='CENTER'>" + String(hum1) + "</td>");
            client.println("<td type='text/css' align='CENTER'>" + String(hum2) + "</td>");
            client.println("<td type='text/css' align='CENTER'>---</td>");
            client.println("<td type='text/css' align='CENTER'>---</td>");
            client.println("</tr>");

              //Humedad Superficial
            client.println("<tr><th scope='col'><strong>&nbsp Humedad Superficial(%) &nbsp</strong></th>");
            client.println("<td type='text/css' align='CENTER'>---</td>");
            client.println("<td type='text/css' align='CENTER'>---</td>");
            client.println("<td type='text/css' align='CENTER'>" + String(lectura1) + "</td>");
            client.println("<td type='text/css' align='CENTER'>" + String(lectura2) + "</td>");
            client.println("</tr>");
            
            client.println("</table"); 
            
            client.println(paginaFin);
            
            // la respuesta HTTP termina con una linea en blanco
            client.println();
            break;
          } else { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
          currentLine += c;      // lo agrega al final de currentLine
        }
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
