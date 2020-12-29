#define SOFTWARESERIAL

//Developed by Juan Rosario

#ifndef DUETDATA_H
#define DUETDATA_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AltSoftSerial.h>
#include "DuetStatus.h"

//Pins 8 and 9 on the Arduino Uno are reserved for AltSoftSerial


class DuetData
{
public:
    DuetData();
    ~DuetData();

    //Send a request for data from the Duet
    void RequestData();

    //Check if there is any data in the serial buffer from the Duet
    void CheckBuffer();

    //Data Updated Callback
    void (*DataUpdated)(void);

    //Current Set Status
    DUET_STATUS Status;

    //Current Progress
    float Progress;

    //Buffer Reading
    bool Reading;

    //Open Comms
    void StartComms();

private:
    void ProcessBuffer();
    int bufferIdx;
    byte* buffer;
    DynamicJsonDocument doc;
    #ifdef SOFTWARESERIAL
    AltSoftSerial duetSerial; //Software serial on pins 8/9 on Arduino Uno.
    #endif
};

#endif
