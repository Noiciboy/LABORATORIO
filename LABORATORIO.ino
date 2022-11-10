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

SMTPData datosSMTP;




//**************
//**** MQTT CONFIG *****
//**************
const char *mqtt_server = "node02.myqtthub.com";
const int mqtt_port = 1883;
const char *mqtt_user = "32";
const char *mqtt_pass = "32";
const char *root_topic_subscribe = "MODULO_PC/esp32";
const char *root_topic_publish = "Estado/public_esp32";
const char *root_topic_publish2 = "Estado/puerta";
const char *root_topic_publish3 = "Estado/humedad";
const char *root_topic_publish4 = "Estado/temperatura";


//**************
//**** WIFICONFIG ******
//**************
const char* ssid = "MARTES";
const char* password =  "1007747398N@t";git



//**************
//**** GLOBALES   ******
//**************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[75];
char npt[25];
char nht[25];
char ntt[25];

//long count=0;

//********
//* F U N C I O N E S **
//********
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

    pinMode(LED, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(VENT, OUTPUT);
    pinMode(LED_MAIL, OUTPUT);    
    pinMode(SENSOR, INPUT);     //Pulsador o sensor como entrada   
    digitalWrite(LED_MAIL, LOW);
    digitalWrite(LED, HIGH);
    dht.begin();             //inicio el modulo dht11
    int peticion ;
  
}

void loop() {
  
  if (!client.connected()) // si el cliente no se conecta entra
  {
    reconnect();
  }

  if (client.connected()){

         h = dht.readHumidity();
         t = dht.readTemperature();      
         tem = "Temperatura: "+ String (t) +"°C "+"Humedad: "+ h +"% PUERTA: "+PUERTA;
         pta = "PUERTA: "+ PUERTA;
         hum = "Humedad: "+ String (h) +"%";
         tmp = "Temperatura: "+ String (t) +"°C ";
         delay (500); 
             tem.toCharArray(msg,75);                 //convierta str en arreclo char con 57 carateres 
             client.publish(root_topic_publish,msg);
             pta.toCharArray(npt,25);                 //convierta str en arreclo char con 57 carateres 
             client.publish(root_topic_publish2,npt);
             hum.toCharArray(nht,25);                 //convierta str en arreclo char con 57 carateres 
             client.publish(root_topic_publish3,nht);
             tmp.toCharArray(ntt,25);                 //convierta str en arreclo char con 57 carateres 
             client.publish(root_topic_publish4,ntt);
             
                      if (t>=mnum) 
                     {
                      digitalWrite(VENT, HIGH);          //prendo el ventilaror(trabaja con ceros)
                      delay(1000); 
                     } 
                  else  digitalWrite(VENT, LOW); 
            
               
          h_ant=h;          //guarda el cmabio anterior
          t_ant=t;
          
          delay(1000);
          
    digitalWrite(LED_MAIL, LOW);        
          actuadores();
       }
       
Serial.println(msg);
Serial.println(npt);
delay(5000);
  client.loop();
  
    
}




//***********
//*    CONEXION WIFI      *
//***********
void setup_wifi(){
  delay(5000);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//***********
//*    CONEXION MQTT      *
//***********

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Broker...");
    // Creamos un cliente ID
    String clientId = "ESP_32_1";
    
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado al broker!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion a topic "+ String(root_topic_subscribe));
      }else{
        Serial.println("fallo Suscripciión a topic "+ String(root_topic_subscribe));
      }
    } else {
      Serial.print("falló conexión broker:( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//***********
//*       CALLBACK        *
//***********

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  mens=incoming;         //guardo en una variable para poder usarlo  como lectura
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);
  actuadores();   // llama la funcion y dependiendo la orden ejecuta
}

   void actuadores()                                                // funcion ""
      { 
          if (mens == "p0")
             {  Serial.println("peticion para abrir puerta" );
                mens = " ";
                digitalWrite(LED1, HIGH);
                correo();
                
             }
           if (mens == "p1")
             {  Serial.println("peticion para cerrar puerta" );
                mens = " ";
                digitalWrite(LED1, LOW);
                PUERTA= "CERRADA"; 
             } 
           if (mens == "b0")
             {
                digitalWrite(LED1, HIGH); 
                Serial.println("entro al b0"); 
             } 
           if (mens == "b1")
             {
                digitalWrite(LED1, LOW); 
                Serial.println("entro al b1"); 
             } 
           if (mens == "c0")
             {
                digitalWrite(VENT, LOW); 
                Serial.println("entro al b0"); 
             } 
           if (mens == "c1")
             {
                digitalWrite(VENT, HIGH); 
                Serial.println("entro al b1"); 
             }   
           if (mens[0] == 't' && mens[1] == 'e' && mens[2] == 'm' && mens[3] == ':')
             {
             mnum = ((mens[4]-48)*10) +mens[5]-48;                                 //guardo el numero para usarlo en la temperatura
                  if (t>=mnum) 
                     {
                      digitalWrite(VENT, HIGH);          //prendo el ventilaror(trabaja con ceros)
                      Serial.println(mnum);
                      delay(1000); 
                     } 
                  else  digitalWrite(VENT, LOW);  
                  
                if(mnum != mnuman)
                {
                  BANDERA = 1;       //si cambio la tempratura 
                }
                mnuman = mnum;                       //para que no entre si no cuando la temperatura cambie             
                                         
             }  
      }  

       void correo()
  { 
    Serial.println("Iniciando correo!!!");
    delay(1000);
    Serial.println("SE ACABA DE ABRIR LA PUERTA");
    PUERTA= "ABIERTA";
     if(BANDERA==1)
      digitalWrite(LED_MAIL, HIGH); 
//    datosSMTP.setLogin("smtp.gmail.com", 465, "mqttunisangil2022@gmail.com", "unisangil2022.");//Configuración del servidor de correo electrónico SMTP, host, puerto, cuenta y contraseña
//    datosSMTP.setSender("NOICIBOY_ESP32", "mqttunisangil2022@gmail.com");     //coloca el nombre del que envia el mensaje y el correo electrónico
//    datosSMTP.setPriority("High");// Establezca la prioridad o importancia del correo electrónico High, Normal, Low o 1 a 5 (1 es el más alto)
//    datosSMTP.setSubject("ALERTA DE SEGURIDAD");               // Establecer el asunto
//    datosSMTP.setMessage(EMAIL, false);// escribe el contenido del correo
//    datosSMTP.addRecipient("cristianavila@unisangil.edu.co");   // aqui se envia el correo al destinatario     
//    if (!MailClient.sendMail(datosSMTP))                       //si tiene datos comience a enviar correo electrónico.
//    Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
//    datosSMTP.empty();           //Borrar todos los datos del objeto datosSMTP para liberar memoria
      delay(1000);
     digitalWrite(LED_MAIL, LOW);
//     peticion=0;
     BANDERA==0;
  } 
