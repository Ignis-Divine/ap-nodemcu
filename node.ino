#include <ESP8266WiFi.h>  
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#define ARDUINOJSON_USE_DOUBLE 0

//para conexion a red
const char* const ssid = "FIBRATVSJR13324";
const char* const password = "wlan8cf6a3";

//variables de programa
const int btn  = 16; //D0
const int btn2 = 5; //D0
String a="";
String m="";
int est1=0;
int est2=0;
int val1=0;
int val2=0;
int ant1=0;
int ant2=0;
int led =02;
byte mac[6];
char buf[3];
char buff[100];
String txt="";
void setup(){
  //modo de pin

  Serial.begin(115200);
  //establecer parametro wifi
  WiFi.begin(ssid, password);
  //iniciar conexion
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Espere conectando...");
  }//Condicion Para Vincular Conexion
  Serial.println("** Conexion Establecida **");
}//Setup Parametros Consola y conexion.

//Post's Para Bases de Datos.
void post_msj(String a, String m){
    //creacion de cliente http
    HTTPClient http; 
    //ruta "de api
    http.begin("http://157.230.94.200:5050/nodemcu");
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(macAdd());
    //Creacion de json
    StaticJsonBuffer<200> jsonBuffer;
    char json[256];
    JsonObject& root = jsonBuffer.createObject();
    root["B"] = a;
    root["est"] = m;
    root.printTo(json, sizeof(json));
    Serial.println(json);
    //envio de solicitud
    int httpCode = http.POST(json);
    http.end(); //Close connection
}//Post 
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

void boton_1(){
  if (est1==1){
    a="1";
    m="B1 Activado";
      post_msj(a,m);     
    } //Datos Subir a Base de Datos     
}
void boton_2(){
  if(est2==1){
      a="2";
      m="B2 Activado";
      post_msj(a,m);
    }
}
void loop(){
  //enviar datos mientras este conectado
  if (WiFi.status() == WL_CONNECTED){
    val1= digitalRead(btn);
    val2= digitalRead(btn2);
    digitalWrite(led,HIGH);
    digitalWrite(led,LOW);  
//boton1
    if ((val1 == HIGH && ant1== LOW)){
      est1=1;
    }
    else{
      est1=0;
    } 
    ant1 = val1;
   if(est1 == 1){
    boton_1();
   }
//boton2
   if ((val2 == HIGH && ant2== LOW)){
      est2=1;
    }
    else{
      est2=0;
    } 
    ant2 = val2;
   if(est2 == 1){
    boton_2();
   }
  }
}
