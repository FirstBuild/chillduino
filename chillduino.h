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

#ifndef CHILLDUINO_H
#define CHILLDUINO_H

#include <math.h>
#include "application.h"

#define CHILLDUINO_VERSION "0.4.1"

template <typename Api, void (Api::*setTemperature)(float)>
class Thermistor {
  public:
    static const unsigned long B = 3980;
    static const unsigned long R0 = 5000;
    static const unsigned long R1 = 15000;
    static const unsigned long R2 = 10000;
    static const unsigned long V = 5;
    static const unsigned long T0 = 25;
    static const unsigned long SAMPLE_FREQUENCY = 100;
    static const int SAMPLES_PER_AVERAGE = 10;

  private:
    Api *_api;
    int _samples[SAMPLES_PER_AVERAGE];
    int _sampleCount;

  public:
    Thermistor() :
      _api(0),
      _samples(),
      _sampleCount(0) { }
    
    Thermistor(Api& api) :
      _api(&api),
      _samples(),
      _sampleCount(0) { }

    void sample(void) {
      _samples[_sampleCount++] = _api->getFreshFoodThermistorReading();
      
      if (_sampleCount == SAMPLES_PER_AVERAGE) {
        float c = celsius();
        (_api->*setTemperature)(c);
        _sampleCount = 0;
      }

      Application::setTimeout<Thermistor,
        &Thermistor::sample>(SAMPLE_FREQUENCY, this);
    }

    int sample(void) const {
      long total = 0;
      
      for (int i = 0; i < _sampleCount; i++) {
        total += _samples[i];
      }
      
      return total / _sampleCount;
    }
    
    float voltage(void) const {
      return V * sample() / 1023.0;
    }

    float resistance(void) const {
      float v = voltage();
      return (V * R1 - v * (R1 + R2)) / v;
    }

    float kelvin(void) const {
      return 1 / (log(resistance() / R0) / B + (1 / (to_k(T0))));
    }

    float celsius(void) const {
      return to_c(kelvin());
    }

    static float to_k(float c) {
      return c + 273.15;
    }

    static float to_c(float k) {
      return k - 273.15;
    }
};

template <typename Api>
class Chillduino {
  private:
    Api& _api;
    Thermistor<Api, &Api::setFreshFoodTemperature> _freshFood;

  public:
    Chillduino(Api& api) :
      _api(api),
      _freshFood(api) { }
  
    void setup(void) {
      _freshFood.sample();
    }
};

#endif /* CHILLDUINO_H */
