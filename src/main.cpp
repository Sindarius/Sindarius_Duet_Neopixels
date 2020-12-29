//#define DEBUG
#define HAS_PANELDUE false

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

  //We only want to show a reading data pulse if the printer is idle.
  if (duetData.Status == DUET_STATUS::IDLE)
  {
    sindNeoPixel.runningMan(10);
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
  sindNeoPixel.Setup(5);
  //Start up duet serial data
  duetData.StartComms();
  duetData.DataUpdated = OnDataUpdated;
}

void loop()
{

  //If there is not a paneldue attached it is necessary to send the requests to get data.
#if !HAS_PANLEDUE
  if (millis() - _tickTime > 1000 && !duetData.Reading)
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
    //Main NeoPixel
    switch (duetData.Status)
    {
    case DUET_STATUS::PRINTING:
    case DUET_STATUS::SIMULATING:
      sindNeoPixel.updateToPercent(sindNeoPixel.strip.Color(0, 0, 255), duetData.Progress);
      break;
    case DUET_STATUS::ERROR:
      sindNeoPixel.colorWipe(sindNeoPixel.strip.Color(255, 0, 0), 5);
      asm volatile ("jmp 0x0000"); //Force Reboot
      break;
    case DUET_STATUS::BUSY:
      sindNeoPixel.colorWipe(sindNeoPixel.strip.Color(255, 0, 255), 5);
      break;
    default:
      break;
    }
  }
}