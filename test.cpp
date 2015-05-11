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

#include <assert.h>
#include "chillduino.h"

class SerialShouldHaveBaud {
private:
  unsigned long _expected;
  unsigned long _actual;

public:
  SerialShouldHaveBaud() : _expected(0), _actual(0) { }
  SerialShouldHaveBaud(unsigned long baud) : _expected(baud), _actual(0) { }

  void begin(unsigned long baud) {
    _actual = baud;
  }

  ~SerialShouldHaveBaud(void) {
    assert(_actual == _expected);
  }
};

void shouldStartSerialCommunication(void) {
  SerialShouldHaveBaud serial(9600);
  Chillduino<SerialShouldHaveBaud>::setup(&serial);
}

int main(void) {
  shouldStartSerialCommunication();
  return 0;
}
