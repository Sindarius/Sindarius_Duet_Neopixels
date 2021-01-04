#ifndef SINDNEOPIXEL_H
#define SINDNEOPXIEL_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

class SindNeoPixel
{
public:
Adafruit_NeoPixel strip;

    SindNeoPixel(uint16_t pixelCount, uint16_t neoPixelPin);
    void Setup(uint8_t brightness = 10);
    void SetBrightness(uint8_t brightness = 10);
    uint32_t Wheel(byte WheelPos);
    void colorWipe(uint32_t c, uint8_t wait);
    void rainbow(uint8_t wait);
    void rainbowCycle(uint8_t wait);
    void theaterChase(uint32_t c, uint8_t wait);
    void theaterChaseRainbow(uint8_t wait);
    void runningMan(uint8_t wait);
    void updateToPercent(uint32_t color, double percentComplete);

    bool debug = false;
    unsigned long currentTime = 0;
private:
    uint16_t neoPixelPin;
   
};

#endif