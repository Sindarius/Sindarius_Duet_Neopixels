//#define DEBUG
#define  HAS_PANELDUE false

#include <Arduino.h>
#include "SindNeoPixel.h"
#include "DuetData.h"

#define NEOPIXELPIN 4
#define LEDPIN 13

unsigned long _tickTime;
unsigned long _lightTickTime;

SindNeoPixel sindNeoPixel(12, NEOPIXELPIN);
DuetData duetData;

void OnDataUpdated()
{
  sindNeoPixel.runningMan(10);
  Serial.println("Data Updated");
  Serial.println(duetData.Status);
}

void setup()
{
  pinMode(13, OUTPUT); // Use pin 13 for status LED (UNO)
  Serial.begin(57600); //Open hardware serial for USB
  _tickTime = millis();
  _lightTickTime = 0;
  sindNeoPixel.Setup(5);
  duetData.StartComms();
  duetData.DataUpdated = OnDataUpdated;
}

void loop()
{

//We only need to issue requests if there is no paneldue requesting info
  #if !HAS_PANLEDUE 
  if (millis() - _tickTime > 5000 && !duetData.Reading)
  {
    duetData.RequestData();
    _tickTime = millis();
  }
  #endif

  duetData.CheckBuffer();
  digitalWrite(LEDPIN, duetData.Reading ? HIGH : LOW);

  if (!duetData.Reading && millis() - _lightTickTime > 5 && duetData.Progress > 0)
  {
    _lightTickTime = millis();
    sindNeoPixel.updateToPercent(sindNeoPixel.strip.Color(0, 0, 255), duetData.Progress);
  }
}