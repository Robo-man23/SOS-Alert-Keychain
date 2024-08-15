// By ~ Rhythm shah
// Sos alert button, women safety ----> HERO

#include <SoftwareSerial.h>

SoftwareSerial Serial1(10, 11); // RX, TX // For A9G Board

#define SOS 3
#define SLEEP_PIN 2 // Make this pin HIGH to make A9G board to go to sleep mode
#define bat 4

String SOS_NUM = "+919978814860"; // Add a number on which you want to receive call or SMS

int SOS_Time = 5; // Press the button 5 sec

// Necessary Variables
boolean stringComplete = false;
String inputString = "";
String fromGSM = "";
bool CALL_END = 1;
char* response = " ";
String res = "";
int c = 0;

int redPin= 7;
int greenPin = 8;
int bluePin = 9;
int analogInPin  = A0;    // Analog input pin
int sensorValue; 
float calibration = 0.36;

void setup()
{
  Serial.begin(115200);                      // For Serial Monitor
  Serial1.begin(115200); // For A9G Board


  // Making Radio OFF for power saving
  //WiFi.mode(WIFI_OFF);  // WiFi OFF
  //btStop();   // Bluetooth OFF

  pinMode(SOS, INPUT_PULLUP);
  pinMode(bat, INPUT_PULLUP);
  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Waiting for A9G to setup everything for 20 sec
  delay(20000);


  digitalWrite(SLEEP_PIN, LOW); // Sleep Mode OFF

  Serial1.println("AT");               // Just Checking
  delay(1000);

  Serial1.println("AT+GPS = 1");      // Turning ON GPS
  delay(1000);

  Serial1.println("AT+GPSLP = 2");      // GPS low power
  delay(1000);

  Serial1.println("AT+SLEEP = 1");    // Configuring Sleep Mode to 1
  delay(1000);

  Serial1.println("AT+CMGF = 1");
  delay(1000);

  Serial1.println("AT+CSMP  = 17,167,0,0 ");
  delay(1000);

  Serial1.println("AT+CPMS = \"SM\",\"ME\",\"SM\" ");
  delay(1000);
  
  digitalWrite(SLEEP_PIN, HIGH); // Sleep Mode ON
}

void loop()
{

  {
    //listen from GSM Module
    if (Serial1.available())
    {
      char inChar = Serial1.read();

      if (inChar == '\n') {

        //check the state
        if (fromGSM == "Location\r")
        {
          Get_gmap_link(0);  // Send Location without Call
          digitalWrite(SLEEP_PIN, HIGH);// Sleep Mode ON
          
        }

        else if (fromGSM == "cAll\r")
        {
          digitalWrite(SLEEP_PIN, LOW); // Sleep Mode OFF
          Serial.println("---------ITS RINGING-------");
          Serial1.println("ATA");
        }

        else if (fromGSM == "NO CARRIER\r")
        {
          Serial.println("---------CALL ENDS-------");
          CALL_END = 1;
          digitalWrite(SLEEP_PIN, HIGH);// Sleep Mode ON
        }

        //write the actual response
        Serial.println(fromGSM);
        //clear the buffer
        fromGSM = "";

      }
      else
      {
        fromGSM += inChar;
      }
      delay(20);
    }

    // read from port 0, send to port 1:
    if (Serial.available()) {
      int inByte = Serial.read();
      Serial1.write(inByte);
    }

    // When SOS button is pressed
    if (digitalRead(SOS) == LOW && CALL_END == 1)
    {
      Serial.print("Calling In.."); // Waiting for 5 sec
      for (c = 0; c < SOS_Time; c++)
      {
        Serial.println((SOS_Time - c));
        delay(1000);
        if (digitalRead(SOS) == HIGH)
          break;
      }

      if (c == 5)
      {
        Get_gmap_link(1);  // Send Location with Call
      }

      //only write a full message to the GSM module
      if (stringComplete)
      {
        Serial1.print(inputString);
        inputString = "";
        stringComplete = false;
      }

    }
    
    // When bat button is pressed
    if (digitalRead(bat) == LOW )
    {
      bat_level();
    }    
  }
}


//---------------------------------------------  Getting Location and making Google Maps link of it. Also making call if needed
void Get_gmap_link(bool makeCall)
{


  digitalWrite(SLEEP_PIN, LOW);
  delay(1000);
  Serial1.println("AT+LOCATION = 2");
  Serial.println("AT+LOCATION = 2");

  while (!Serial1.available());
  while (Serial1.available())
  {
    char add = Serial1.read();
    res = res + add;
    delay(1);
  }

  res = res.substring(17, 38);
  response = &res[0];

  Serial.print("Recevied Data - "); Serial.println(response); // printin the String in lower character form
  Serial.println("\n");

  if (strstr(response, "GPS NOT"))
  {
    Serial.println("No Location data");
    //------------------------------------- Sending SMS without any location
    Serial1.println("AT+CMGF=1");
    delay(1000);
    Serial1.println("AT+CMGS=\"" + SOS_NUM + "\"\r");
    delay(1000);

    Serial1.println ("Unable to fetch location. Please try again");
    delay(1000);
    Serial1.println((char)26);
    delay(1000);
  }
  else
  {

    int i = 0;
    while (response[i] != ',')
      i++;

    String location = (String)response;
    String lat = location.substring(2, i);
    String longi = location.substring(i + 1);
    Serial.println(lat);
    Serial.println(longi);

    String Gmaps_link = ( "http://maps.google.com/maps?q=" + lat + "+" + longi); //http://maps.google.com/maps?q=38.9419+-78.3020
    //------------------------------------- Sending SMS with Google Maps Link with our Location
    Serial1.println("AT+CMGF=1");
    delay(1000);
    Serial1.println("AT+CMGS=\"" + SOS_NUM + "\"\r");
    delay(1000);

    Serial1.println ("I'm here " + Gmaps_link);
    delay(1000);
    Serial1.println((char)26);
    delay(1000);

    Serial1.println("AT+CMGD=1,4"); // delete stored SMS to save memory
    delay(5000);
  }
  response = "";
  res = "";
  if (makeCall)
  {
    Serial.println("Calling Now");
    Serial1.println("ATD" + SOS_NUM);
    CALL_END = 0;
  }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void bat_level ()
{
  sensorValue = analogRead(analogInPin);
  float voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor
 
  int bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
 
  if ((bat_percentage >85)&&(bat_percentage >=100))
  {
    bat_percentage = 100;
    analogWrite(bluePin, 255);
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
     analogWrite(redPin, 170);
     analogWrite(bluePin, 255);
  }
  if ((bat_percentage >65)&&(bat_percentage =85))
  {
    bat_percentage = 100;
    analogWrite(greenPin, 255);
  }
 
  Serial.print("Analog Value = ");
  Serial.print(sensorValue);
  Serial.print("\t Output Voltage = ");
  Serial.print(voltage);
  Serial.print("\t Battery Percentage = ");
  Serial.println(bat_percentage);
  delay(1000);
}