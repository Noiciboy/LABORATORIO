#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ESP32_MailClient.h>

#define DHTPIN 15
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// defino variables 

long duration;
int D_ant,Distance,bandera,mnum,BANDERA;
int LED = 13; 
int LED1 = 12;
int VENT = 14;  
int LED_MAIL = 18;     //para prender el led indicador que se envio el mensaje
int BOTON = 0;     //
int mnuman = 0;
int SENSOR = 4; // entrada del boton 

String stringdistance,str,pta,hum,tmp,tem,mens,PUERTA,EMAIL;
float  t,h,h_ant,t_ant;
