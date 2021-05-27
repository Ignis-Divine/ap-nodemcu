#include <ESP8266WiFi.h>  
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#define TRIGGER_PIN  5
#define ECHO_PIN     4
//#include "RestClient.h"
#include "DHT.h"
#define DHTPIN 14   //pin d5= sensor temperatura/humedad
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE,15);
#define ARDUINOJSON_USE_DOUBLE 0
int ledPIN = D1; //= Pin LED y Buzzer mismo pin

//para conexion a red
const char* const ssid = "FIBRATVSJR13324";
const char* const password = "wlan8cf6a3";

//variables de programa
const int btn =16; //D0
float h=0;
float t=0;
float s=0;
byte mac[6];
char buf[3];
char buff[100];
String txt="";
void setup(){
  //modo de pin

  Serial.begin(115200);
  //establecer parametro wifi
  pinMode(ledPIN , OUTPUT);
  WiFi.begin(ssid, password);
  //iniciar conexion
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Espere conectando...");
  }//Condicion Para Vincular Conexion
  Serial.println("** Conexion Establecida **");
  dht.begin();
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
}//Setup Parametros Consola y conexion.

//Post's Para Bases de Datos.

void post_temp(double h, double t, double s){
    //creacion de cliente http
     WiFiClient client;
    HTTPClient http;     
    //ruta "de api
    http.begin(client, "http://104.248.128.237:5050/nodemcu");
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(macAdd());
    //creacion de json
    StaticJsonBuffer<200> jsonBuffer;
    char json[256];
    JsonObject& root = jsonBuffer.createObject();
    root["humedad"] = h;
    root["temperatura"] = t;
    root["sonico"] = s;
    root.printTo(json, sizeof(json));
    Serial.println(json);
    //envio de solicitud
    int httpCode = http.POST(json);
    http.end(); //Close connection
}//Post Temperatura

char * macAdd(){
  WiFi.macAddress(mac);
  txt="";
  for (byte i = 0; i < 6; ++i){
    sprintf(buf, "%2X", mac[i]);
    txt += buf;
    if (i < 5) txt += ':';
  }//Formula para conversion de Direccion MAC Address
  snprintf( buff, sizeof(buff)-1, "%s", txt.c_str() );
  buff[sizeof(buff)] = '\0';
  //Serial.println(buff);
  return buff;
}//Conversion de MAC address de NodemCU

void loop(){
  //enviar datos mientras este conectado
  if (WiFi.status() == WL_CONNECTED){
    h = dht.readHumidity();
    t = dht.readTemperature(true);
    t=(t-32);
    t=t/(1.800);
    long duration,s;
  digitalWrite(TRIGGER_PIN, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  s = (duration/2) / 29.1;
  delay(7000);
    post_temp(h,t,s);
  

  }
}
