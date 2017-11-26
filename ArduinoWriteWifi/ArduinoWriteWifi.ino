#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial ArduinoSerial(3, 2); // RX, TX
int led = 4;
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards



void setup() 
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  ArduinoSerial.begin(4800);
}
void loop() 
{
  while (ArduinoSerial.available() > 0) 
  {
    int val = ArduinoSerial.parseFloat();
    if (ArduinoSerial.read() == '\n') 
    {
      if (val==1)
      {
        digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
        Serial.println((String)"led est ON" );
      }
      else if (val==0){
        digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW}
        Serial.println((String)"led est OFF" );
      }
      else if (val == 0xA0) 
      {
        int A0 = analogRead(A0);
        ArduinoSerial.print(A0);
        ArduinoSerial.print("\n");
      }
      else
      {
        myservo.write(val);              // tell servo to go to position in variable 'pos'
        delay(15);
        Serial.println((String)"angle actuel :" +val );
      }
    }
  }
  delay(100)
}
