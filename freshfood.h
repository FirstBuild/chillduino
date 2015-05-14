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

#ifndef FRESHFOOD_H
#define FRESHFOOD_H

#include <math.h>
#include "application.h"

class Celsius {
  public:
    static float toKelvin(float c) {
      return c + 273.15;
    }
};

class Kelvin {
  public:
    static float toCelsius(float k) {
      return k - 273.15;
    }
};

template <typename X, typename C>
class FreshFood {
  private:
    static const unsigned long B  = X::FRESH_FOOD_THERMISTOR_B;
    static const unsigned long R0 = X::FRESH_FOOD_THERMISTOR_ROOM_RESISTANCE;
    static const unsigned long R1 = X::FRESH_FOOD_THERMISTOR_1_RESISTANCE;
    static const unsigned long R2 = X::FRESH_FOOD_THERMISTOR_2_RESISTANCE;
    static const unsigned long V  = X::FRESH_FOOD_THERMISTOR_VOLTAGE;
    static const unsigned long T0 = X::FRESH_FOOD_THERMISTOR_ROOM_TEMPERATURE;
    static const int SAMPLE_FREQUENCY    = X::FRESH_FOOD_SAMPLE_FREQUENCY;
    static const int SAMPLES_PER_AVERAGE = X::FRESH_FOOD_SAMPLES_PER_AVERAGE;

  private:
    static int total;
    static int count;

  private:
    static float to_v(float sample) {
      return V * sample / 1023;
    }

    static float to_r(float sample) {
      float v = to_v(sample);
      return (V * R1 - v * (R1 + R2)) / v;
    }

    static float to_k(float sample) {
      float r = to_r(sample);
      return 1 / (log(r / R0) / B + (1 / Celsius::toKelvin(T0)));
    }

    static float to_c(float sample) {
      return Kelvin::toCelsius(to_k(sample));
    }

  public:
    static void setup(void) {
      sample();
    }

    static void sample(void) {
      float celsius;

      total += X::getFreshFoodThermistorReading();

      if (++count == SAMPLES_PER_AVERAGE) {
        celsius = to_c(total / count);
        total = 0;
        count = 0;

        C::setFreshFoodTemperature(celsius);
      }

      Application::setTimeout<FreshFood<X, C>::sample>(SAMPLE_FREQUENCY);
    }
};

template <typename X, typename C>
int FreshFood<X, C>::total = 0;

template <typename X, typename C>
int FreshFood<X, C>::count = 0;

#endif /* FRESHFOOD_H */
