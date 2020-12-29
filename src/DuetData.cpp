//#define DEBUG
#include "DuetData.h"

//Default Constructor for setup
DuetData::DuetData() : doc(500)
{
    buffer = new byte[500];
    bufferIdx = 0;
    Status = "I";
    Progress = 0;
    DataUpdated = NULL;
    Reading = false;
}

DuetData::~DuetData()
{
    delete[] buffer;
    duet.end();
}

void DuetData::StartComms()
{
    duet.begin(57600);
}

void DuetData::RequestData()
{
    if (Reading) {
        return;
    }
    duet.write("M408 S0\n"); //Send M Code to request data
}

void DuetData::CheckBuffer()
{
    //If there are bytes available lets process them
    int bytesAvailable = duet.available();
    if (bytesAvailable > 0)
    {
        Reading = true;
        //Read all available bytes in the buffer
        for (int idx = 0; idx < bytesAvailable; idx++)
        {
            byte byteRead = duet.read();

#ifdef DEBUG
            Serial.write(byteRead);
#endif

            ///string terminated
            if (byteRead == 0 || byteRead == '\n')
            {
                Reading = false;
                if (bufferIdx > 10)
                {
                    ProcessBuffer();
                    bufferIdx = 0;
                    if (DataUpdated != NULL)
                    {
                        DataUpdated();
                    }
                }
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
    DeserializationError error = deserializeJson(doc, buffer);
    if (error)
    {
        Status = "Z";
        return; //There was an error deserializing the json
    }

    //Load up the fields we are interested in
    Status = doc["status"];
    Progress = doc["fraction_printed"];
}
