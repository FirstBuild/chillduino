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
    static const unsigned long FRESH_FOOD_THERMISTOR_B = 3980;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_RESISTANCE = 5000;
    static const unsigned long FRESH_FOOD_THERMISTOR_1_RESISTANCE = 15000;
    static const unsigned long FRESH_FOOD_THERMISTOR_2_RESISTANCE = 10000;
    static const unsigned long FRESH_FOOD_THERMISTOR_VOLTAGE = 5;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_TEMPERATURE = 25;

    static const int FRESH_FOOD_SAMPLE_FREQUENCY = 100;
    static const int FRESH_FOOD_SAMPLES_PER_AVERAGE = 10;
    
    static int getFreshFoodThermistorReading(void) {
      return analogRead(FRESH_FOOD_THERMISTOR);
    }
    
    static void setFreshFoodTemperature(float celsius) {
      Serial.print("fresh food temperature: ");
      Serial.print(celsius);
      Serial.println(" *C");
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

void setup(void) {
  Serial.begin(9600);
  setInterrupt();

  Chillduino<Configuration>::setup();
}

void loop(void) {
  Application::loop();
}
