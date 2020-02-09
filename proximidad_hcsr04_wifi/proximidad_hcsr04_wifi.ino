// Aquí va el código de la conexión a wifi
#include <Wire.h> 
#include <SoftwareSerial.h>

SoftwareSerial wifi =  SoftwareSerial(10,11);      // (RX,TX)
// RX es el 10 con color rosado; TX es el 11 con color verde
#define dist A0                


//declarar datos de conexión Wi-Fi
String apiKey = "M4YAO2PJX3LT0Z7N";     // API KEY WRITE de ThingSpeak
String ssid="AndresFPV";   
String password ="andresporras";

#define FIREBASE_HOST "pruebasw-861ba.firebaseio.com"      //https://pruebasw-861ba.firebaseio.com/
#define FIREBASE_AUTH "qedEj6WYmZ8o017B4zQtCWkkd0Q8xGwC3gfyv0tG"  //contraseña de PruebaSw2


// Tiempo de Respuesta
void tiempoRespuesta(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (wifi.available()){
        c=wifi.read();
        Serial.print(c);
      }
    }                   
}



boolean thingSpeakWrite(float value1){  
      String cmd = "AT+CIPSTART=\"TCP\",\"";                  // Conexión TCP
      cmd += "184.106.153.149";                               // Esta es la IP de Thingspeak   
      cmd += "\",80";                                         // Puerto
      wifi.println(cmd); //Mandar datos seriales al módulo ESP8266
      Serial.println(cmd);
      if(wifi.find("Error"))
      {
        Serial.println("AT+CIPSTART error");
        return false;
      }  
      
      String getStr = "GET /update?api_key=";   // Prepara el String GET a mandar
      getStr += apiKey;  
      getStr +="&field1=";       
      getStr += String(value1);//convertir float a String porque es un valor numérico, pero se escribirá como String 
     
     
      getStr += "\r\n\r\n";    //no borrar
      
      // Enviar la longitud de la cadena
      cmd = "AT+CIPSEND=";
      cmd += String(getStr.length());
      wifi.println(cmd);
      Serial.println(cmd);  
      delay(100);
      
      if(wifi.find(">"))//si Ok envíe la cadena de GET
      {
        wifi.print(getStr);
        Serial.print(getStr);
      }
      else{
        wifi.println("AT+CIPCLOSE");
        // Para que el usuario sepa que no se están enviando datos
        Serial.println("AT+CIPCLOSE");
        return false;
      }
      return true;
}


// This uses Serial Monitor to display Range Finder distance readings
 
// Include NewPing Library
#include "NewPing.h"
 
// Hook up HC-SR04 with Trig to Arduino Pin 10, Echo to Arduino pin 13
// Maximum Distance is 400 cm
 
#define TRIGGER_PIN  3  //verde
#define ECHO_PIN     5  //azul
#define MAX_DISTANCE 400
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
 
float duration, distance;
 
int iterations = 5;

int counter = 0;
 
void setup() {
  Serial.begin (9600);
  
  wifi.begin(9600);       // Habilite la comunicación serial del módulo, mi módulo trabaja con este baudRate
   
  wifi.println("AT+CWMODE=1");   // Esto configura el módulo wifi como cliente
  tiempoRespuesta(1000);
  wifi.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // Se configura el SSID y password para conexión WIFI
  tiempoRespuesta(5000);
  Serial.println("Setup completed");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH)
}
 
void loop() {

  int dist = analogRead(A0); //revisar mi codigo de proximidad
   
  duration = sonar.ping_median(iterations);
  
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  
  distance = (duration / 2) * 0.0343;
  // Send results to Serial Monitor
  //Serial.print("Distance = "+String(distance)+"cm");
  
  if (distance >= 40 || distance <= 2) {
    Serial.println("Funcionando.");
    Serial.print("Contador= ");
    Serial.print(counter);
    Serial.println(" veces.");
    
  }
  else {
    
    counter = counter + 1;
    Serial.print("Distancia= ");
    Serial.print(distance);
    Serial.println(" cm.");
    Serial.print("Contador ahora es= ");
    Serial.print(counter);
    Serial.println(" veces.");
    delay(8000);
  }

  thingSpeakWrite(counter);
  delay(3000);
}
