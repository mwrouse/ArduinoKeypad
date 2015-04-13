#include <Wire.h>
#include "MPL3115A2.h"
#include "SevSeg.h"
#include <EEPROM.h>
#include "MEMORY.h"


#define SERIAL_DEBUG


// Seven-Segment Display
SevSeg output;

// Altimeter 
MPL3115A2 altimeter;
float altitude;
float apogee = 0;
float maxAltitude = -99999;
float minAltitude = 99999;
bool launchDetected = false;
bool savedApogee = false;

// Time Variables
unsigned long timer;
const int timerBeat = 200;

// Misc Variables
char tempString[10];


// Setup Altimeter
void setup()
{
  Wire.begin();  // Join i2c bus
  
  altimeter.begin();
  
  // Pinout for the display
  output.Begin(COMMON_CATHODE, 4, 8, 5, 11, 13, 7, 6, 10, 3, 9, 4, 2, 12);
  output.SetBrightness(100);
  
  // Configure Sensor
  altimeter.setModeAltimeter();
  
  //altimeter.setModeBarometer();
  altimeter.setOversampleRate(7);
  altimeter.enableEventFlags();
  
  delay(1500);
  
  #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    
   #endif
   
   // Read the last known apogee and display it
   EEPROM_read(225, apogee);
   sprintf(tempString, "%04d", (int)apogee);
   Serial.println(apogee);
   printString(tempString, 500);
   
   printString("REDY", 200);
   
   pinMode(13, OUTPUT);
   digitalWrite(13, HIGH);
   
   timer = millis();
  
}

// Program Loop
void loop()
{  
  if (millis() > timer)
  {
    timer = millis() + timerBeat;
    
    // Read Altitude
    readAltitude();
    
    #ifdef SERIAL_DEBUG
      //Serial.print("Alt: ");
      //Serial.println(altitude);
      //Serial.println();
    #endif
   
  }
 
  // Show altitude on display
  printAlt();
}

// Read the altitude from the sensor
void readAltitude()
{
  // Get altitude in ft.
  //altitude = altimeter.readPressure();
  float altitudeOld = altitude;
  altitude = altimeter.readAltitudeFt();

  // Set max if needed
  if (altitude > (maxAltitude + 2))
  {
    maxAltitude = altitude;
  }
  
  // Set min if necessary
  if (altitude < (minAltitude - 5))
  {
    minAltitude = altitude;
  }
  
  // Detect the launch
  if (altitude > (minAltitude + 10) && !launchDetected)
  {
    launchDetected = true;
  }
  
  // Detect and save the apogee into the memory
  if (altitude < (maxAltitude - 10) && !savedApogee && launchDetected)
  {
    apogee = maxAltitude - minAltitude;
    EEPROM_write(225, apogee);
    savedApogee = true;
  }
  
}

// Display altitude on display
void printAlt()
{
  if (!savedApogee)
  {
    sprintf(tempString, "%04d", (int)(altitude - minAltitude));
  }else{
    sprintf(tempString, "%04d", (int)(apogee));
  }  
  printString(tempString, 10);
}

// Print string to display
void printString(char* stringy, int delayTime)
{
  for (int i = 0; i < delayTime; i++)
  {
     output.DisplayString(stringy, 0);
  }
}
