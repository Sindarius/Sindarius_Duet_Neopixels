//#define DEBUG
#include "DuetData.h"
#define BUFFERSIZE 600
//Default Constructor for setup
DuetData::DuetData() : doc(BUFFERSIZE)
{
    buffer = new byte[BUFFERSIZE];
    bufferIdx = 0;
    Status = DUET_STATUS::IDLE;
    Progress = 0;
    DataUpdated = NULL;
    Reading = false;
}

DuetData::~DuetData()
{
    delete[] buffer;
#if SOFTWARESERIAL
    duetSerial.end();
#endif
}

void DuetData::StartComms()
{
#if SOFTWARESERIAL
    duetSerial.begin(57600);
    duetSerial.setTimeout(1000);
#endif
}

void DuetData::RequestData()
{
    if (Reading)
    {
        return;
    }

#ifdef DEBUG
    Serial.println("M117 DuetData : Requesting Data...");
#endif
#if SOFTWARESERIAL
    duetSerial.flushOutput();
    duetSerial.write("M408 S0\n"); //Send M Code to request data
#else
    Serial.flushOutput();
    Serial.write("M408 S0\n"); //Send M Code to request data
#endif
}

void DuetData::CheckBuffer()
{

//If there are bytes available lets process them
#if SOFTWARESERIAL
    int bytesAvailable = duetSerial.available();
#else
    int bytesAvailable = Serial.available();
#endif

    if (bytesAvailable > 0)
    {
        Reading = true;
        //Read all available bytes in the buffer
        for (int idx = 0; idx < bytesAvailable; idx++)
        {
#if SOFTWARESERIAL
            byte byteRead = duetSerial.read();
#else
            byte byteRead = Serial.read();
#endif

            ///string terminated
            if (byteRead == 0 || byteRead == '\n')
            {

#ifdef DEBUG
                Serial.println("EOL");
                for (int i = 0; i < bufferIdx; i++)
                {
                    Serial.write(buffer[i]);
                }
                Serial.println();
                Serial.println(bufferIdx);

#endif

                if (bufferIdx > 50)
                {
#ifdef DEBUG
                    Serial.println("Processing Buffer");
#endif

                    buffer[bufferIdx] = byteRead;
                    ProcessBuffer();
                    if (DataUpdated != NULL)
                    {
                        DataUpdated();
                    }
                }
#ifdef DEBUG
                Serial.println("Buffer Read Complete.");
#endif
                Reading = false;
                bufferIdx = 0;
                return;
            }
            if (bufferIdx == BUFFERSIZE)
            {
#ifdef DEBUG
                Serial.println("Buffer about to overflow");
#endif
                bufferIdx = 0;
                Reading = false;
                return;
            }
            else
            {
                buffer[bufferIdx] = byteRead;
                bufferIdx++;
            }
        }
    }
}

void DuetData::ProcessBuffer()
{
    //Process the json data
    Status = DUET_STATUS::UNKNOWN;

    //Scan for invalid characters
    for (int i = 0; i < bufferIdx -1 ; i++)
    {
        if (buffer[i] < 32 || buffer[i] > 126 )
        {
            #ifdef DEBUG
            Serial.println("Throwing out buffer due to bad characters");
            #endif
            return;
        }
    }

    DeserializationError error = deserializeJson(doc, buffer);
    if (error.code() != error.Ok)
    {
        Status = DUET_STATUS::ERROR;
        Serial.println("Error");
        Serial.print(error.c_str());
        Serial.println(error.f_str());
        return; //There was an error deserializing the json
    }

    const char *status = doc["status"];
    switch (status[0])
    {
    case 'I':
        Status = DUET_STATUS::IDLE;
        break;
    case 'P':
        Status = DUET_STATUS::PRINTING;
        break;
    case 'M':
        Status = DUET_STATUS::SIMULATING;
        break;
    case 'B':
        Status = DUET_STATUS::BUSY;
        break;
    default:
        Status = DUET_STATUS::ERROR;
        break;
    }
    Progress = doc["fraction_printed"];
}
