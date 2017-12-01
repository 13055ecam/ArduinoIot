#include <StringSplitter.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <string.h>


//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read
WiFiServer server(80);
SoftwareSerial NodeSerial(D2,D3);


void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
   pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  NodeSerial.begin(4800);

}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
   String s = "\r\n";
   //s += "Content-Type: text/html\r\n\r\n";
   s += "<!DOCTYPE HTML>\r\n<html><body>\r\n";
   s+="<form action='' method='get'>\r\n";
   s+="Angle : <input type='range' name =''max='180' min='0' step='1'>\r\n";
    s+="<input type='submit' value='Angle'>\r\n";
    s+="<br></br>\r\n";
    s+="<button name='' type='submit' value='1'>Led on</button>\r\n";
    s+="<button name='' type='submit' value='0'>Led offn</button>\r\n";
    String dest = req.substring(7,13);
    String dest2 = dest.substring(4,5);
    String dest3 = dest.substring(0,3);
    
     if (dest2=="0")
     {
      int val = 0;
      Serial.println(val);
      NodeSerial.print(val);
      NodeSerial.print("\n");
     }
     else if (dest2=="1")
     {
      int val = 1;
      Serial.println(val);
      NodeSerial.print(val);
      NodeSerial.print("\n");
     }
     else
     {
      int balo = atoi( &dest3[ 0 ] );
      Serial.println(balo);      
      NodeSerial.print(balo);
      NodeSerial.print("\n");
     }
   client.flush();
  s += "</form></body></html>\n";
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}
