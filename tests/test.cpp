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
#include <assert.h>

class ShouldBeAbleToSetTimer {
  private:
    unsigned int _value;

  public:
    ShouldBeAbleToSetTimer(void) :
      _value(0) { }

    unsigned int value(void) const {
      return _value;
    }

    void withMemberFunction(void) {
      _value++;
    }

    static void withStaticFunction(void *instance) {
      ShouldBeAbleToSetTimer *i = (ShouldBeAbleToSetTimer *) instance;
      i->_value++;
    }

    static void withTypedFunction(ShouldBeAbleToSetTimer *instance) {
      instance->_value++;
    }
};

void shouldBeAbleToSetTimeoutWithStaticFunction(void) {
  ShouldBeAbleToSetTimer s;
  Application::setTimeout<ShouldBeAbleToSetTimer::withStaticFunction>(0, &s);

  assert(s.value() == 0);
  Application::loop();
  assert(s.value() == 1);
  Application::loop();
  assert(s.value() == 1);
}

void shouldBeAbleToSetTimeoutWithTypedFunction(void) {
  ShouldBeAbleToSetTimer s;

  Application::setTimeout<ShouldBeAbleToSetTimer,
    ShouldBeAbleToSetTimer::withTypedFunction>(1, &s);

  assert(s.value() == 0);
  Application::loop();
  assert(s.value() == 0);
  Application::loop();
  assert(s.value() == 0);
  Application::tick();
  assert(s.value() == 0);
  Application::loop();
  assert(s.value() == 1);
  Application::loop();
  assert(s.value() == 1);
}

void shouldBeAbleToSetTimeoutWithMemberFunction(void) {
  ShouldBeAbleToSetTimer s;

  Application::setTimeout<ShouldBeAbleToSetTimer,
    &ShouldBeAbleToSetTimer::withMemberFunction>(2, &s);

  assert(s.value() == 0);
  Application::loop();
  assert(s.value() == 0);
  Application::loop();
  assert(s.value() == 0);
  Application::tick(5);
  assert(s.value() == 0);
  Application::loop();
  assert(s.value() == 1);
  Application::loop();
  assert(s.value() == 1);
}

class ShouldReturnNegativeId {
  public:
    static void whenTimersAreExhausted(void *) { }
};

void shouldReturnNegativeIdWhenTimersAreExhausted(void) {
  int i;
  ShouldReturnNegativeId s;

  do {
    i = Application::setTimeout<
      ShouldReturnNegativeId::whenTimersAreExhausted>(0, &s);
  }
  while (i >= 0);

  Application::loop();
}

#include <iostream>

class ShouldSampleTemperature {
  private:
    int _value;

  public:
    ShouldSampleTemperature(void) :
      _value(0) { }

    ~ShouldSampleTemperature(void) {
      assert(_value == 25);
    }

    int getFreshFoodThermistorReading(void) {
      return 512;
    }

    void setFreshFoodTemperature(float c) {
      _value = (int) c;
    }

    bool complete(void) const {
      return _value != 0;
    }
};

void shouldSampleFreshFoodTemperature(void) {
  ShouldSampleTemperature s;
  Chillduino<ShouldSampleTemperature> chillduino(s);
  chillduino.setup();

  while (!s.complete()) {
    Application::tick();
    Application::loop();
  }
}

int main(void) {
  shouldBeAbleToSetTimeoutWithStaticFunction();
  shouldBeAbleToSetTimeoutWithTypedFunction();
  shouldBeAbleToSetTimeoutWithMemberFunction();
  shouldReturnNegativeIdWhenTimersAreExhausted();
  shouldSampleFreshFoodTemperature();

  return 0;
}
