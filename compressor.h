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

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "application.h"

template <typename X, typename C>
class Compressor {
  private:
    static const int RATE_LIMIT = X::COMPRESSOR_RATE_LIMIT;
    static const unsigned long SAMPLE_FREQUENCY = X::COMPRESSOR_SAMPLE_FREQUENCY;

  private:
    static bool _desired;

  public:
    static void setup(void) {
      waitForChange();
    }

    static void turnOn(void) {
      _desired = true;
    }

    static void turnOff(void) {
      _desired = false;
    }

    static void waitForChange(void) {
      static bool _actual = false;

      if (_actual == _desired) {
        Application::setTimeout<Compressor<X, C>::waitForChange>
          (SAMPLE_FREQUENCY);
      }
      else {
        _actual = _desired;
        X::setCompressorIsRunning(_actual);
        Application::setTimeout<Compressor<X, C>::waitForChange>(RATE_LIMIT);
      }
    }
};

template <typename X, typename C>
bool Compressor<X, C>::_desired = false;

#endif /* COMPRESSOR_H */
