/*********
  NADCOM Mateusz Glowinski
  +48 510 748 999 
*********/

#include <SoftwareSerial.h>
 

// Configure software serial port
SoftwareSerial SIM900(8, 9); 

int enableGPRSpin = 10;
int fireAlarmPin = 13;
int centralDamagePin = 12;
bool fireAlarmState = 1;
bool lastFireAlarmState = 1;
bool centralDamageState = 1;
bool lastCentralDamageState = 1;

String phoneNumbers[] = {"+48605108182", "+48502159172"};

void setup() 
{
  pinMode(enableGPRSpin, OUTPUT);
  pinMode(fireAlarmPin, INPUT_PULLUP);
  pinMode(centralDamagePin, INPUT_PULLUP);
  
  SIM900.begin(19200);
  /*
    Enable GPRS module
  */
  while(1)
  {
    String response = "";
    SIM900.println("AT");
    delay(100);
    while(SIM900.available())
    {
      response += (char) SIM900.read();
      delay(1);
    }
    
    if (response.indexOf("OK\r")>0)
    {
      break;  
    }
    
    digitalWrite(enableGPRSpin, HIGH);
    delay(1000);
    digitalWrite(enableGPRSpin, LOW);
    delay(5000);
  }   
  /*
    Waiting for establishing a signal
  */
  delay(20000);
}

void loop() 
{ 
  /*
   Checking fire alam status
  */
  fireAlarmState = digitalRead(fireAlarmPin);
  if (fireAlarmState != lastFireAlarmState) 
  {
    if (fireAlarmState == 0)
    { 
        sendSMS("Fire Alarm");
    }else
    {
      sendSMS("Fire Alarm Off");  
    }
    lastFireAlarmState = fireAlarmState;
  }

 /*
  Checking damage of central status 
 */
  centralDamageState = digitalRead(centralDamagePin);
  if (centralDamageState != lastCentralDamageState) 
  {
    if (centralDamageState == 0)
    { 
        sendSMS("Damage on Central");
    }else
    {
      sendSMS("Damage on Central Fixed");  
    }
    lastCentralDamageState = centralDamageState;
  }

}

void sendSMS(String text) 
{
  for( unsigned int a = 0; a < sizeof(phoneNumbers)/sizeof(phoneNumbers[0]); a++)
  {
    // AT command to set SIM900 to SMS mode
    SIM900.println("AT+CMGF=1"); 
    delay(100);
   
    SIM900.println("AT + CMGS = \"" + phoneNumbers[a] + "\""); 
    delay(100);
  
    SIM900.println(text); 
    delay(100);

    // End AT command with a ^Z, ASCII code 26
    SIM900.println((char)26); 
    delay(100);
    SIM900.println();
    // Give module time to send SMS
    delay(5000); 
  }
}
