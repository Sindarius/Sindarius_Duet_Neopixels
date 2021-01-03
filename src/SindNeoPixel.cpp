
#include "SindNeoPixel.h"

SindNeoPixel::SindNeoPixel(uint16_t pixelCount, uint16_t pin)
{
  neoPixelPin = pin;
  strip = Adafruit_NeoPixel(pixelCount, neoPixelPin, NEO_GRB + NEO_KHZ800);
}

void SindNeoPixel::Setup(uint8_t brightness)
{
  strip.begin();
  SetBrightness(brightness);
  strip.show(); // Initialize all pixels to 'off'
}

void SindNeoPixel::SetBrightness(uint8_t brightness)
{
  strip.setBrightness(brightness);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t SindNeoPixel::Wheel(byte WheelPos)
{
#ifdef DEBUG_OUTPUT
  Serial.write("Wheel");
#endif
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void SindNeoPixel::colorWipe(uint32_t c, uint8_t wait)
{
#ifdef DEBUG_OUTPUT
  Serial.write("Wipe");
#endif
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

void SindNeoPixel::rainbow(uint8_t wait)
{
#ifdef DEBUG_OUTPUT
  Serial.write("rainbow");
#endif
  uint16_t i, j;
  for (j = 0; j < 256; j++)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void SindNeoPixel::rainbowCycle(uint8_t wait)
{
#ifdef DEBUG_OUTPUT
  Serial.write("rainbow cycle");
#endif
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void SindNeoPixel::theaterChase(uint32_t c, uint8_t wait)
{
#ifdef DEBUG_OUTPUT
  Serial.write("Theater Chase");
#endif
  for (int j = 0; j < 10; j++)
  { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, c); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0); //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void SindNeoPixel::theaterChaseRainbow(uint8_t wait)
{
#ifdef DEBUG_OUTPUT
  Serial.write("Theater Chase Rainbow");
#endif
  for (int j = 0; j < 256; j++)
  { // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, Wheel((i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0); //turn every third pixel off
      }
    }
  }
}

void SindNeoPixel::runningMan(uint8_t wait)
{
  uint16_t i, j;
  for (i = 0; i < strip.numPixels(); i++)
  {
    for (j = 0; j < strip.numPixels(); j++)
    {
      strip.setPixelColor(j, 0);
    }

    if (i == 0)
    {
      strip.setPixelColor(strip.numPixels() - 1, strip.Color(255, 255, 255));
    }
    else
    {
      strip.setPixelColor(i - 1, strip.Color(255, 255, 255));
    }
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(wait);
  }
}

#define PERCENTPULSECOUNT 2000

void SindNeoPixel::updateToPercent(uint32_t color, float percentComplete)
{
  if (percentComplete >= 1)
  {
    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, color);
    }
    return;
  }

  for (uint32_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, 0);
  }

  uint32_t pixelProgress = floor(strip.numPixels() * percentComplete);
  for (uint32_t i = 0; i < pixelProgress; i++)
  {
    strip.setPixelColor(i, color);
  }

  float currentPulseValue = currentTime % PERCENTPULSECOUNT;
  if (currentPulseValue < PERCENTPULSECOUNT / 2)
  {
    //Climb
    currentPulseValue = (255.0f / PERCENTPULSECOUNT * 2.0) * currentPulseValue;
  }
  else
  {
    //Fall
    currentPulseValue = (255.0f / PERCENTPULSECOUNT * 2.0) * (PERCENTPULSECOUNT - currentPulseValue);
  }
  uint32_t pixelColor = ceil(currentPulseValue);
  strip.setPixelColor(pixelProgress, strip.Color(pixelColor, pixelColor, pixelColor));
  strip.show();
}
