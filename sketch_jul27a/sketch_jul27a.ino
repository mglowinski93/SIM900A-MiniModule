/*********
  NADCOM Mateusz Glowinski
  +48 510 748 999 
*********/

#include <SoftwareSerial.h>
#include <QueueList.h>
#include <String.h>

// Configure software serial port
SoftwareSerial SIM900(8, 9); 

int enableGPRSpin = 10;
int fireAlarmPin = 13;
int centralDamagePin = 12;
bool fireAlarmState = 1;
bool lastFireAlarmState = 1;
bool centralDamageState = 1;
bool lastCentralDamageState = 1;
const short unsigned int sendingSMSRetries = 10;

//String phoneNumbers[] = {"+48605108182", "+48502159172"};
String phoneNumbers[] = {"+48605108182"};

QueueList <String> queue;

String readResponseSIM900(bool printResponseInLogs=true);
void logEvent(String text);
void sendSMS(String text) ;
bool responseSuccessful();

void setup() 
{
  Serial.println("Setting up SIM900 module");
  pinMode(enableGPRSpin, OUTPUT);
  pinMode(fireAlarmPin, INPUT_PULLUP);
  pinMode(centralDamagePin, INPUT_PULLUP);
  
  SIM900.begin(19200);
  Serial.begin(9600);
  /*
    Enable GPRS module
  */
  /*
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
  */
  /*
    Waiting for establishing a signal
  */
  //delay(20000);
    logEvent("Setting SIM900 module into TEXT mode");
    int initCounter = 0;
    String SMSModeMessage = "SIM900 set into TEXT mode";
    while(!responseSuccessful())
    {
        SIM900.println("AT+CMGF=1");
        initCounter++;
        if(initCounter > 100)
        {
          SMSModeMessage = "Failed to setup SIM900 into TEXT mode";
          break;  
        }
        delay(1000);
    }
    logEvent(SMSModeMessage);
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
        queue.push("Fire Alarm");
    }else
    {
        queue.push("Fire Alarm Off");  
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
        queue.push("Damage on Central");
    }else
    {
        queue.push("Damage on Central Fixed");  
    }
    lastCentralDamageState = centralDamageState;
  }

  if(!queue.isEmpty())
  { 
     sendSMS(queue.pop()); 
  }
}


void sendSMS(String text) 
{
  for( unsigned int a = 0; a < sizeof(phoneNumbers)/sizeof(phoneNumbers[0]); a++)
  {
    String message_log = "\n\n=====================\nSending " + text + " to " + phoneNumbers[a];
    logEvent(message_log);
    for(int i=0; i<sendingSMSRetries; i++)
    {
      SIM900.println("AT + CMGS = \"" + phoneNumbers[a] + "\""); 
      delay(100);
      SIM900.println(text); 
      delay(100);
      readResponseSIM900(false);
      // End AT command with a ^Z, ASCII code 26
      SIM900.println((char)26);
      delay(5000);
      if(responseSuccessful())
      {
        message_log = text + " was sent to " + phoneNumbers[a];
        logEvent(message_log);
        break;  
      }
      message_log = "Sending" + text + " to " + phoneNumbers[a] + " failed. It was " + i + "/" + sendingSMSRetries + "tries";
      logEvent(message_log);
    }
  }
}


void logEvent(String text)
{
  Serial.println(text);
}


String readResponseSIM900(bool printResponseInLogs=true)
{
    String response = "";
    
    while(SIM900.available())
    {
      response += (char) SIM900.read();
      delay(1);
    }
    
    if(printResponseInLogs)
    {
      String logMessage = "Response is: " + response;
      logEvent(logMessage);
    }
      
    return response;
}


bool responseSuccessful()
{
  String response = readResponseSIM900();
  return response.indexOf("OK\r")>0;
}
