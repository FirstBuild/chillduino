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

template <typename T, T expected>
class Expect {
  private:
    T _actual;

  public:
    Expect(void) :
      _actual() { }

    ~Expect(void) {
      assert(_actual == expected);
    }

    T operator()(void) const {
      return _actual;
    }

    void operator()(const T& value) {
      _actual = value;
    }
};

template <int X>
class WhenFreshFoodThermistorReads {
  public:
    static int getFreshFoodThermistorReading(void) {
      return X;
    }
};

template <int X>
class ShouldHaveFreshFoodTemperature {
  public:
    static Expect<int, X> setFreshFoodTemperature;
};

template <int X>
Expect<int, X> ShouldHaveFreshFoodTemperature<X>::setFreshFoodTemperature;

class ShouldSampleFreshFood :
  public WhenFreshFoodThermistorReads<512>,
  public ShouldHaveFreshFoodTemperature<25> {

  public:
    static const unsigned long FRESH_FOOD_THERMISTOR_B = 3980;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_RESISTANCE = 5000;
    static const unsigned long FRESH_FOOD_THERMISTOR_1_RESISTANCE = 15000;
    static const unsigned long FRESH_FOOD_THERMISTOR_2_RESISTANCE = 10000;
    static const unsigned long FRESH_FOOD_THERMISTOR_VOLTAGE = 5;
    static const unsigned long FRESH_FOOD_THERMISTOR_ROOM_TEMPERATURE = 25;

    static const int FRESH_FOOD_SAMPLE_FREQUENCY = 100;
    static const int FRESH_FOOD_SAMPLES_PER_AVERAGE = 10;
};

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
      while (Application::setTimeout<ShouldReturnNegativeId::dummy>(0) >= 0);
      Application::loop();
    }

    static void dummy(void) { }
};

int main(void) {
  ShouldReturnNegativeId::whenAllTimersAreUsed();
  Chillduino<ShouldSampleFreshFood>::setup();  
  Time::elapse(1000);

  return 0;
}
