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

#include <chillduino.h>
#include <iostream>
#include <assert.h>

class Time {
  public:
    static void elapse(unsigned long ticks) {
      while (ticks--) {
        Application::tick();
        Application::loop();
      }
    }
};

class ShouldReturnNegativeId {
  public:
    static void whenAllTimersAreUsed(void) {
      while (Application::setTimeout<ShouldReturnNegativeId::empty>(0) >= 0);
      Application::loop();
    }

    static void empty(void) {
      
    }
};

template <int T, int C>
class ThermistorUnitTest {
  public:
    static const unsigned long FRESH_FOOD_THERMISTOR_B = 3980;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_RESISTANCE = 5000;
    static const unsigned long FRESH_FOOD_THERMISTOR_1_RESISTANCE = 15000;
    static const unsigned long FRESH_FOOD_THERMISTOR_2_RESISTANCE = 10000;
    static const unsigned long FRESH_FOOD_THERMISTOR_VOLTAGE = 5;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_TEMPERATURE = 25;

    static const int FRESH_FOOD_SAMPLE_FREQUENCY = 100;
    static const int FRESH_FOOD_SAMPLES_PER_AVERAGE = 10;
    static const int FRESH_FOOD_MAXIMUM_TEMPERATURE = 3;
    static const int FRESH_FOOD_MINIMUM_TEMPERATURE = 1;

    static const unsigned long COMPRESSOR_RATE_LIMIT = 600000;
    static const unsigned long COMPRESSOR_SAMPLE_FREQUENCY = 500;

  public:
    static int getFreshFoodThermistorReading(void) {
      return T;
    }

    static void setFreshFoodTemperature(float celsius) {
      assert(C == (int) celsius);
    }

    static void setCompressorIsRunning(bool running) {
      (void) running;
    }

    static void setup(void) {
      Time::elapse(FRESH_FOOD_SAMPLE_FREQUENCY * FRESH_FOOD_SAMPLES_PER_AVERAGE);
    }
};

class CompressorShouldBeRateLimited {
  private:
    static int reading;
    static bool compressor;

  public:
    static const unsigned long FRESH_FOOD_THERMISTOR_B = 3980;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_RESISTANCE = 5000;
    static const unsigned long FRESH_FOOD_THERMISTOR_1_RESISTANCE = 15000;
    static const unsigned long FRESH_FOOD_THERMISTOR_2_RESISTANCE = 10000;
    static const unsigned long FRESH_FOOD_THERMISTOR_VOLTAGE = 5;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_TEMPERATURE = 25;

    static const int FRESH_FOOD_SAMPLE_FREQUENCY = 100;
    static const int FRESH_FOOD_SAMPLES_PER_AVERAGE = 10;
    static const int FRESH_FOOD_MAXIMUM_TEMPERATURE = 3;
    static const int FRESH_FOOD_MINIMUM_TEMPERATURE = 1;

    static const unsigned long COMPRESSOR_RATE_LIMIT = 600000;
    static const unsigned long COMPRESSOR_SAMPLE_FREQUENCY = 500;

  public:
    static int getFreshFoodThermistorReading(void) {
      return reading;
    }

    static void setFreshFoodTemperature(float celsius) {
      (void) celsius;
    }

    static void setCompressorIsRunning(bool running) {
      compressor = running;
    }

    static void setup(void) {
      reading = 500;
      Time::elapse(FRESH_FOOD_SAMPLE_FREQUENCY * FRESH_FOOD_SAMPLES_PER_AVERAGE);
      assert(compressor);

      reading = 0;
      Time::elapse(FRESH_FOOD_SAMPLE_FREQUENCY * FRESH_FOOD_SAMPLES_PER_AVERAGE);
      assert(compressor);

      Time::elapse(COMPRESSOR_RATE_LIMIT);
      assert(!compressor);
    }
};

int CompressorShouldBeRateLimited::reading = 0;
bool CompressorShouldBeRateLimited::compressor = false;

int main(void) {
  ShouldReturnNegativeId::whenAllTimersAreUsed();
  Chillduino<ThermistorUnitTest<0, -273> >::setup();
  Chillduino<ThermistorUnitTest<512, 25> >::setup();
  Chillduino<ThermistorUnitTest<613, 205> >::setup();
  Chillduino<CompressorShouldBeRateLimited>::setup();

  return 0;
}
