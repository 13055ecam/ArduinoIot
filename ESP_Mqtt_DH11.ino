/*
  Projet d'apprentissage d'un objet connecté (IoT)  pour réaliser une sonde de température
  ESP8266 + DHT22 + LED + MQTT + Home-Assistant
  Projets DIY (https://www.projetsdiy.fr) - Mai 2016
  Licence : MIT
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "DHT.h"          // Librairie des capteurs DHT

#define ssid "ESP8266 WIFIKEY"
#define password ""
IPAddress apIP(192, 168, 5, 1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);



//const char WiFiAPPSK[] = "sparkfun";


#define mqtt_server "192.168.5.101"
#define mqtt_user "USERNAME"  //s'il a été configuré sur Mosquitto
#define mqtt_password "PASSWORD" //idem

#define temperature_topic "sensor/temperature"  //Topic température
#define humidity_topic "sensor/humidity"        //Topic humidité

//Buffer qui permet de décoder les messages MQTT reçus
char message_buff[100];

long lastMsg = 0;   //Horodatage du dernier message publié sur MQTT
long lastRecu = 0;
bool debug = false;  //Affiche sur la console si True

#define DHTPIN D4    // Pin sur lequel est branché le DHT

// Dé-commentez la ligne qui correspond à votre capteur 
#define DHTTYPE DHT11       // DHT 11 
//#define DHTTYPE DHT22         // DHT 22  (AM2302)

//Création des objets
DHT dht(DHTPIN, DHTTYPE);     
WiFiClient espClient;
PubSubClient client(espClient);

int led = 12;

void setup() {
  Serial.begin(9600);     //Facultatif pour le debug
  pinMode(led,OUTPUT);     //Pin 2 
  setup_wifi();           //On se connecte au réseau wifi
  //setupWiFi();
  client.setServer(mqtt_server, 1883);    //Configuration de la connexion au serveur MQTT
  client.setCallback(callback);  //La fonction de callback qui est executée à chaque réception de message   
  dht.begin();
}

//Connexion au réseau WiFi

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion a ");
  //Serial.println(wifi_ssid);
 WiFi.mode(WIFI_AP_STA);   //This line hides the viewing of ESP as wifi network
    WiFi.softAPConfig(apIP,gateway, subnet);   // subnet FF FF FF 00  
  WiFi.softAP(ssid,password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connexion WiFi etablie ");
 IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

}

//Reconnexion
void reconnect() {
  //Boucle jusqu'à obtenur une reconnexion
  while (!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, erreur : ");
      Serial.print(client.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  //Envoi d'un message par minute
  if (now - lastMsg > 1000 * 1) {
    lastMsg = now;
    //Lecture de l'humidité ambiante
    float h = dht.readHumidity();
    Serial.println(h);
    // Lecture de la température en Celcius
    float t = dht.readTemperature();
    Serial.println(t);

    //Inutile d'aller plus loin si le capteur ne renvoi rien
    if ( isnan(t) || isnan(h)) {
      Serial.println("Echec de lecture ! Verifiez votre capteur DHT");
      return;
    }
  
    if ( debug ) {
      Serial.print("Temperature : ");
      Serial.print(t);
      Serial.print(" | Humidite : ");
      Serial.println(h);
    }  
    client.publish(temperature_topic, String(t).c_str(), true);   //Publie la température sur le topic temperature_topic
    client.publish(humidity_topic, String(h).c_str(), true);      //Et l'humidité
  }
  if (now - lastRecu > 100 ) {
    lastRecu = now;
    client.subscribe("homeassistant/switch1");
  }
}

// Déclenche les actions à la réception d'un message
// D'après http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("payload: " + msgString);
  }
  
  if ( msgString == "ON" ) {
    digitalWrite(led,HIGH);  
  } else {
    digitalWrite(led,LOW);  
  }
}


