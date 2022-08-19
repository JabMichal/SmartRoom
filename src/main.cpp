#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//#include <Adafruit_BMP280.h>

#define SEA_LEVEL_PRESSURE_HPA (1013.25) // TO ADJUST

Adafruit_BME280 bme;

int i2c_bme_address = 0x76;

#define BLYNK_PRINT Serial

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "settings.hpp"

#define PIN D7
#define NUM_OF_LED 250

int red, green, blue, first_led, last_led = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_LED, PIN, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte WheelPos) {
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

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++){
    for(i=0; i<strip.numPixels(); i++)     
      strip.setPixelColor(i, Wheel((i*1+j) & 255));    
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
  for(int i =0; i < strip.numPixels(); i++)
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

  unsigned status = bme.begin(i2c_bme_address);

  Blynk.begin(auth, ssid, pass, IPAddress(ip_address[0],ip_address[1],ip_address[2],ip_address[3]), 8080);

  Serial.println(status);

  strip.begin();
  strip.show();

   if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        //while (1) delay(10);
    }
}

void loop(){
  Blynk.run();
}