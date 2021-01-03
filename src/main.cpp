//#define DEBUG
#define HAS_PANELDUE false

#include <Arduino.h>
#include "SindNeoPixel.h"
#include "DuetData.h"

#define NEOPIXELPINA 5
#define NEOPIXELPINB 4
#define NEOPIXELPINC 3
#define NEOPIXELCOUNT 3
#define LEDPIN 13

unsigned long _tickTime;
unsigned long _lightTickTime;

SindNeoPixel sindNeoPixel[] = {SindNeoPixel(12, NEOPIXELPINA), SindNeoPixel(12, NEOPIXELPINB), SindNeoPixel(12, NEOPIXELPINC)};

DuetData duetData;

void OnDataUpdated()
{

  //We only want to show a reading data pulse if the printer is idle.
  if (duetData.Status == DUET_STATUS::IDLE)
  {
    sindNeoPixel[0].runningMan(10);
  }
#ifdef DEBUG
  Serial.println("Data Updated");
  Serial.println(duetData.Status);
#endif
}

void setup()
{
  pinMode(13, OUTPUT); // Use pin 13 for status LED (UNO)
  Serial.begin(57600); //Open hardware serial for USB

  //Setup Light
  _tickTime = millis();
  _lightTickTime = millis();
  for (int idx = 0; idx < NEOPIXELCOUNT; idx++)
  {
    sindNeoPixel[idx].Setup(5);
  }
  sindNeoPixel[1].debug = true;

  //Start up duet serial data
  duetData.StartComms();
  duetData.DataUpdated = OnDataUpdated;
}

void loop()
{
  //If there is not a paneldue attached it is necessary to send the requests to get data.
#if !HAS_PANLEDUE
  if (millis() - _tickTime > 2000 && !duetData.Reading)
  {
#ifdef DEBUG
    Serial.println("Requesting Data...");
#endif
    duetData.RequestData();
    _tickTime = millis();
  }
#endif

  duetData.CheckBuffer();
  digitalWrite(LEDPIN, duetData.Reading ? HIGH : LOW);

  if (millis() - _lightTickTime > 10 && !duetData.Reading)
  {
    _lightTickTime = millis();

    for(int idx = 0; idx < NEOPIXELCOUNT; idx++){
      sindNeoPixel[idx].currentTime = _lightTickTime;  
    }

    //Main NeoPixel
    switch (duetData.Status)
    {
    case DUET_STATUS::PRINTING:
    case DUET_STATUS::SIMULATING:
      sindNeoPixel[0].updateToPercent(sindNeoPixel[0].strip.Color(0, 0, 255), duetData.Progress);
      break;
    case DUET_STATUS::ERROR:
      for (int idx = 0; idx < NEOPIXELCOUNT; idx++)
      {
        sindNeoPixel[idx].colorWipe(sindNeoPixel[idx].strip.Color(255, 0, 0), 5);
      }
      asm volatile("jmp 0x0000"); //Force Reboot
      break;
    case DUET_STATUS::BUSY:
      sindNeoPixel[0].colorWipe(sindNeoPixel[0].strip.Color(255, 0, 255), 5);
      break;
    default:
      break;
    }

    //Update Temp
    sindNeoPixel[1].updateToPercent(sindNeoPixel[1].strip.Color(255, 0, 0), 0.25);
    sindNeoPixel[2].updateToPercent(sindNeoPixel[2].strip.Color(0, 0, 255), 0.5);
  }
}