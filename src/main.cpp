#include <Arduino.h>

#define BLYNK_PRINT Serial

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "settings.hpp"

#define PIN D7
#define NUM_OF_LED 250

int red, green, blue, first_led, last_led = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_LED, PIN, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte WheelPos) 
{
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbow(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i<strip.numPixels(); i++) 
    {
      strip.setPixelColor(i, Wheel((i*1+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

BLYNK_WRITE(V4) // set color for RGB_zebra
{
  int shift = param.asInt();
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i,Wheel(shift & 255));
    //strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + shift) & 255));
  }
  strip.show();
}

BLYNK_WRITE(V0) // ON yellow natural leds
{
   for(int i=0; i<NUM_OF_LED; i++)
  {
    strip.setPixelColor(i, 200, 100, 20); // Dioda "i" oraz skladowe R=255 G=0 B=0
     // Wysylamy dane do lancucha
  }
  strip.show(); 
}

BLYNK_WRITE(V3) // OFF leds
{
   for(int i=0; i<NUM_OF_LED; i++)
  {
    strip.setPixelColor(i, 0, 0, 0); // Dioda "i" oraz skladowe R=255 G=0 B=0
     // Wysylamy dane do lancucha
  }
  strip.show();
}

BLYNK_WRITE(V1) // ON rainbow
{
  rainbow(30);
  delay(10);
}

BLYNK_WRITE(V2)
{
  int shift = param.asInt();
  strip.setBrightness(shift);
  strip.show();          
}

void setup()
{
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass, IPAddress(192,168,55,169), 8080);

  strip.begin();
  strip.show();
}

void loop()
{
  Blynk.run();
}