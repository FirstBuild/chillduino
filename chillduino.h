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

class Chillduino {
  private:
    int _minimumFreshFoodThermistorReading;
    int _currentFreshFoodThermistorReading;
    int _maximumFreshFoodThermistorReading;
    unsigned long _compressorTicksPerDefrost;
    unsigned long _remainingCompressorTicksUntilDefrost;
    unsigned long _defrostDurationInTicks;
    unsigned long _remainingTicksWhileDefrosting;
    unsigned long _remainingTicksForCompressorChange;
    unsigned long _minimumTicksForCompressorChange;
    bool _isCompressorRunning;
    bool _isDefrostRunning;
    bool _isChanged;

  public:
    Chillduino(void) :
      _minimumFreshFoodThermistorReading(0),
      _currentFreshFoodThermistorReading(0),
      _maximumFreshFoodThermistorReading(0),
      _compressorTicksPerDefrost(0),
      _remainingCompressorTicksUntilDefrost(0),
      _defrostDurationInTicks(0),
      _remainingTicksWhileDefrosting(0),
      _remainingTicksForCompressorChange(0),
      _minimumTicksForCompressorChange(0),
      _isCompressorRunning(false),
      _isDefrostRunning(false),
      _isChanged(false) { }

    Chillduino& setMinimumFreshFoodThermistorReading(int reading) {
      _minimumFreshFoodThermistorReading = reading;
      return *this;
    }

    Chillduino& setCurrentFreshFoodThermistorReading(int reading) {
      _currentFreshFoodThermistorReading = reading;
      return *this;
    }

    Chillduino& setMaximumFreshFoodThermistorReading(int reading) {
      _maximumFreshFoodThermistorReading = reading;
      return *this;
    }

    Chillduino& setCompressorTicksPerDefrost(unsigned long ticks) {
      _compressorTicksPerDefrost = ticks;
      _remainingCompressorTicksUntilDefrost = ticks;
      return *this;
    }

    Chillduino& setDefrostDurationInTicks(unsigned long ticks) {
      _defrostDurationInTicks = ticks;
      return *this;
    }

    Chillduino& setMinimumTicksForCompressorChange(unsigned long ticks) {
      _minimumTicksForCompressorChange = ticks;
      return *this;
    }

    bool isCompressorRunning(void) const {
      return _isCompressorRunning;
    }

    bool isDefrostRunning(void) const {
      return _isDefrostRunning;
    }

    bool isChanged(void) const {
      return _isChanged;
    }

    void tick(void) {
      if (_remainingTicksForCompressorChange > 0) {
        _remainingTicksForCompressorChange--;
      }

      if (_isCompressorRunning) {
        if (_remainingCompressorTicksUntilDefrost > 0) {
          _remainingCompressorTicksUntilDefrost--;
        }
      }

      if (_remainingTicksWhileDefrosting > 0) {
        _remainingTicksWhileDefrosting--;
      }
    }

    void loop(void) {
      _isChanged = false;

      if (isCompressorReadyForChange()) {
        if (isDefrostRunning()) {
          if (isDefrostComplete()) {
            stopRunningDefrost();
          }
        }
        else if (isCompressorRunning() && isReadyForDefrost()) {
          stopRunningCompressor();
          startRunningDefrost();
        }
        else if (isFreshFoodWarm() && !isCompressorRunning()) {
          startRunningCompressor();
        }
        else if (isFreshFoodCold() && isCompressorRunning()) {
          stopRunningCompressor();
        }
      }
    }

    void elapse(unsigned long ticks) {
      while (ticks--) {
        tick();
        loop();
      }
    }

  private:
    bool isFreshFoodWarm(void) const {
      return _currentFreshFoodThermistorReading
        > _maximumFreshFoodThermistorReading;
    }

    bool isFreshFoodCold(void) const {
      return _currentFreshFoodThermistorReading
        < _minimumFreshFoodThermistorReading;
    }

    bool isCompressorReadyForChange(void) const {
      return _remainingTicksForCompressorChange == 0;
    }

    bool isDefrostComplete(void) const {
      return _remainingTicksWhileDefrosting == 0;
    }

    bool isReadyForDefrost(void) const {
      return _remainingCompressorTicksUntilDefrost == 0;
    }

    void startRunningCompressor(void) {
      _isChanged = true;
      _isCompressorRunning = true;
      _remainingTicksForCompressorChange = _minimumTicksForCompressorChange;
    }

    void stopRunningCompressor(void) {
      _isChanged = true;
      _isCompressorRunning = false;
      _remainingTicksForCompressorChange = _minimumTicksForCompressorChange;
    }

    void startRunningDefrost(void) {
      _isChanged = true;
      _isDefrostRunning = true;
      _remainingTicksWhileDefrosting = _defrostDurationInTicks;
    }

    void stopRunningDefrost(void) {
      _isChanged = true;
      _isDefrostRunning = false;
      _remainingCompressorTicksUntilDefrost = _compressorTicksPerDefrost;
    }
};

#endif /* CHILLDUINO_H */
