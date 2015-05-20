/**
 * Copyright (c) 2015 FirstBuild
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <Adafruit_NeoPixel.h>
#include "chillduino.h"

#define COMPRESSOR     9
#define DEFROST        10
#define DOOR_SWITCH    13
#define DEFROST_SWITCH 12
#define THERMISTOR     A0
#define LEDS           A4
#define LED_COUNT      10
#define LED_OFF_R      0
#define LED_OFF_G      0
#define LED_OFF_B      0
#define LED_ON_R       255
#define LED_ON_G       255
#define LED_ON_B       255
#define OFF
#define COLD           45
#define COLDER         37
#define COLDEST        31

#define TICKS_PER_SECOND   ((unsigned long) 1000)
#define TICKS_PER_MINUTE   (60 * TICKS_PER_SECOND)
#define TICKS_PER_HOUR     (60 * TICKS_PER_MINUTE)

Chillduino chillduino;
Adafruit_NeoPixel pixels(LED_COUNT, LEDS, NEO_GRB + NEO_KHZ800);

SIGNAL(TIMER0_COMPA_vect) {
  chillduino.tick();
}

void setInterrupt(void) {
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void clearInterrupt(void) {
  TIMSK0 &= ~_BV(OCIE0A);
}

void setLedColor(int r, int g, int b) {
  for (int i = 0; i < LED_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  
  pixels.show();
}

void setup(void) {
  pinMode(DOOR_SWITCH, INPUT);
  pinMode(DEFROST_SWITCH, INPUT);
  pinMode(COMPRESSOR, OUTPUT);
  pinMode(DEFROST, OUTPUT);
  
  chillduino
    .setMinimumFreshFoodThermistorReading(243) //decrewsed set point
    .setMaximumFreshFoodThermistorReading(350) //increased set point
    .setCompressorTicksPerDefrost(100 * TICKS_PER_HOUR)
    .setDefrostDurationInTicks(30 * TICKS_PER_MINUTE)
    .setMinimumTicksForCompressorChange(10 * TICKS_PER_MINUTE)
    .setMinimumTicksForDoorClose(100);
  
  pixels.begin();
  pixels.show();
  
  Serial.begin(9600);
  setInterrupt();
}

void loop(void) {
  chillduino.setCurrentFreshFoodThermistorReading(analogRead(THERMISTOR));
  chillduino.setDoorSwitchReading(digitalRead(DOOR_SWITCH));
  chillduino.loop();

  if (chillduino.isChanged()) {
    Serial.println(chillduino.isDoorOpen()
      ? "Door is open" : "Door is closed");

    Serial.println(chillduino.isCompressorRunning()
      ? "Compressor is running" : "Compressor is not running");

    Serial.println(chillduino.isDefrostRunning()
      ? "Defrost is running" : "Defrost is not running");
    
    if (chillduino.isDoorOpen()) {
      setLedColor(LED_ON_R, LED_ON_G, LED_ON_B);
    }
    else {
      setLedColor(LED_OFF_R, LED_OFF_G, LED_OFF_B);
    }
    
    digitalWrite(COMPRESSOR, chillduino.isCompressorRunning());
    digitalWrite(DEFROST, chillduino.isDefrostRunning());
  }
}
