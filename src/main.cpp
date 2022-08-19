#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "settings.hpp"

#define BLYNK_PRINT Serial
#define PIN_LED_DATA D7

Adafruit_BME280 bme;
uint8_t i2c_bme_address = 0x76;

uint16_t number_of_LEDs  = 250;
uint8_t red, green, blue, first_led, last_led = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(number_of_LEDs, PIN_LED_DATA, NEO_GRB + NEO_KHZ800);

uint32_t wheel(byte wheelPos) {
  if (wheelPos < 85) {
    return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
  } else if (wheelPos < 170) {
    wheelPos -= 85;
    return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  } else {
    wheelPos -= 170;
    return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++){
    for(i=0; i<strip.numPixels(); i++)     
      strip.setPixelColor(i, wheel((i*1+j) & 255));    
    strip.show();
    delay(wait);
  }
}
void setSlidersLevel(){
  Blynk.virtualWrite(V5,red);
  Blynk.virtualWrite(V6,green);
  Blynk.virtualWrite(V7,blue);
}

void changeColor(){
  for(uint8_t i =0; i < strip.numPixels(); i++)
    strip.setPixelColor(i,red,green,blue);
  strip.show();
  setSlidersLevel();
}

BLYNK_WRITE(V4) {  // set color for RGB_zebra
  red = param[0].asInt();
  green = param[1].asInt();
  blue = param[2].asInt();
  changeColor();
}

BLYNK_WRITE(V0) { // turn ON yellow natural leds
  red = 255; green = 150; blue = 40;
  changeColor();
}

BLYNK_WRITE(V3) { // turn OFF leds
  red = green = blue = 0;
  changeColor();
}

BLYNK_WRITE(V1) { // ON rainbow
  rainbow(30);
  delay(10);
}

BLYNK_WRITE(V5){ // red color slider
  red = param.asInt();
  changeColor();
}

BLYNK_WRITE(V6){ // green color slider
  green = param.asInt();
  changeColor();
}

BLYNK_WRITE(V7){ // blue color slider
  blue = param.asInt(); 
  changeColor();
}

BLYNK_READ(V10){ // temperature 
  float temperature = bme.readTemperature();
  Blynk.virtualWrite(V10,temperature);
}

BLYNK_READ(V11){ // pressure
  float pressure = bme.readPressure() / 100.0F;
  Blynk.virtualWrite(V11,pressure);
}

BLYNK_READ(V12){ // humidity
  float humidity = bme.readHumidity();
  Blynk.virtualWrite(V12,humidity);
}

void setup(){
  Serial.begin(9600);

  if(!bme.begin(i2c_bme_address)) 
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");

  Blynk.begin(auth, ssid, pass, IPAddress(ip_address[0],ip_address[1],ip_address[2],ip_address[3]), 8080);

  strip.begin();
  strip.show();
}

void loop(){
  Blynk.run();
}