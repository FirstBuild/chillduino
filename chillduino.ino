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

#include "chillduino.h"

#define FRESH_FOOD_THERMISTOR A0

class Configuration {
  public:
    Configuration(void) { }

    int getFreshFoodThermistorReading(void) {
      int value = analogRead(FRESH_FOOD_THERMISTOR);
      
      Serial.print("fresh food reading: ");
      Serial.println(value);
      
      return value;
    }
    
    void setFreshFoodTemperature(float celsius) {
      Serial.print("fresh food temperature: ");
      Serial.println(celsius);
    }
};

SIGNAL(TIMER0_COMPA_vect) {
  Application::tick();
}

void setInterrupt(void) {
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void clearInterrupt(void) {
  TIMSK0 &= ~_BV(OCIE0A);
}

Configuration configuration;
Chillduino<Configuration> chillduino(configuration);

void setup(void) {
  Serial.begin(9600);
  setInterrupt();

  chillduino.setup();
}

void loop(void) {
  Application::loop();
}
