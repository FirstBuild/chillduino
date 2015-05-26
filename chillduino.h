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

/**
 * The software version for the chillduino.
 *
 * Please follow the guidelines for semantic versioning.
 * This value should not be modified by hand, but is instead managed using
 * the bump-version script in the repository. That script will ensure that
 * the version in the code matches the version in the documentation.
 *
 */
#define CHILLDUINO_VERSION "1.1.0"

/**
 * The chillduino OFF mode.
 *
 * The modes can be cycled using the push button on the chillduino.
 * In this mode the software does not read inputs or write outputs.
 */
#define CHILLDUINO_MODE_OFF     0

/**
 * The chillduino COLD mode.
 *
 * The modes can be cycled using the push button on the chillduino.
 * In this mode the software keeps the refrigerator cold. This is
 * the highest temperature setting for the chillduino.
 */
#define CHILLDUINO_MODE_COLD    1

/**
 * The chillduino COLDER mode.
 *
 * The modes can be cycled using the push button on the chillduino.
 * In this mode the software keeps the refrigerator colder than the
 * COLD mode, but warmer than the COLDEST mode. This mode is considered
 * to be the mode setting for the typical user.
 */
#define CHILLDUINO_MODE_COLDER  2

/**
 * The chillduino COLDEST mode.
 *
 * The modes can be cycled using the push button on the chillduino.
 * In this mode the software keeps the refrigerator colder than all
 * other modes.
 */
#define CHILLDUINO_MODE_COLDEST 3

/**
 * The number of chillduino modes.
 *
 * This number is used in the software to allow the user to cycle through
 * the mode settings.
 */
#define CHILLDUINO_MODE_COUNT   4

class Chillduino {
  private:
    int _minimumFreshFoodThermistorReading;
    int _currentFreshFoodThermistorReading;
    int _maximumFreshFoodThermistorReading;
    int _previousDoorSwitchReading;
    int _currentDoorSwitchReading;
    int _previousModeSwitchReading;
    int _currentModeSwitchReading;
    int _mode;
    unsigned long _compressorTicksPerDefrost;
    unsigned long _remainingCompressorTicksUntilDefrost;
    unsigned long _defrostDurationInTicks;
    unsigned long _remainingTicksWhileDefrosting;
    unsigned long _remainingTicksForCompressorChange;
    unsigned long _minimumTicksForCompressorChange;
    unsigned long _remainingTicksForDoorClose;
    unsigned long _minimumTicksForDoorClose;
    bool _isCompressorRunning;
    bool _isDefrostRunning;
    bool _isDoorOpen;
    bool _isChanged;

  public:

    /**
     * Creates a new Chillduino and initializes each value to the default.
     *
     * @example
     *
     * ``` cpp
     * Chillduino chillduino;
     *
     * void setup(void) {
     *   chillduino
     *     .setMode(CHILLDUINO_MODE_COLDER)
     *     .setMinimumFreshFoodThermistorReading(500)
     *     .setMaximumFreshFoodThermistorReading(520)
     *     .setCompressorTicksPerDefrost(100 * TICKS_PER_HOUR)
     *     .setDefrostDurationInTicks(30 * TICKS_PER_MINUTE)
     *     .setMinimumTicksForCompressorChange(10 * TICKS_PER_MINUTE)
     *     .setMinimumTicksForDoorClose(100);
     * }
     *
     * void loop(void) {
     *   chillduino.setCurrentFreshFoodThermistorReading(analogRead(THERMISTOR));
     *   chillduino.setDoorSwitchReading(digitalRead(DOOR_SWITCH));
     *   chillduino.setModeSwitchReading(digitalRead(MODE_SWITCH));
     *   chillduino.loop();
     * }
     *
     * ```
     *
     */
    Chillduino(void) :
      _minimumFreshFoodThermistorReading(0),
      _currentFreshFoodThermistorReading(0),
      _maximumFreshFoodThermistorReading(0),
      _previousDoorSwitchReading(0),
      _currentDoorSwitchReading(0),
      _previousModeSwitchReading(0),
      _currentModeSwitchReading(0),
      _mode(CHILLDUINO_MODE_COLDER),
      _compressorTicksPerDefrost(0),
      _remainingCompressorTicksUntilDefrost(0),
      _defrostDurationInTicks(0),
      _remainingTicksWhileDefrosting(0),
      _remainingTicksForCompressorChange(0),
      _minimumTicksForCompressorChange(0),
      _remainingTicksForDoorClose(0),
      _minimumTicksForDoorClose(0),
      _isCompressorRunning(false),
      _isDefrostRunning(false),
      _isDoorOpen(false),
      _isChanged(false) { }

    /**
     * Sets the minimum fresh food thermistor reading allowed.
     *
     * If the current thermistor reading falls below this value then
     * the software will stop running the compressor.
     *
     */
    Chillduino& setMinimumFreshFoodThermistorReading(int reading) {
      _minimumFreshFoodThermistorReading = reading;
      return *this;
    }

    /**
     * Sets the current fresh food thermistor reading.
     *
     * This value should be updated as frequently as possible.
     * The reading is compared to minimum and maximum readings
     * to determine the state of the compressor.
     *
     */
    Chillduino& setCurrentFreshFoodThermistorReading(int reading) {
      _currentFreshFoodThermistorReading = reading;
      return *this;
    }

    /**
     * Sets the maximum fresh food thermistor reading allowed.
     *
     * If the current thermistor reading rises above this value then
     * the software will start running the compressor.
     *
     */
    Chillduino& setMaximumFreshFoodThermistorReading(int reading) {
      _maximumFreshFoodThermistorReading = reading;
      return *this;
    }

    /**
     * Sets the amount of time (in ticks) that the compressor must
     * run before running the defrost.
     *
     * This value is considered to be a maximum. The defrost is also
     * affected by the amount of time the fresh food doors have been
     * open. Assuming the doors remain closed, the defrost will start
     * after the specified number of compressor ticks.
     *
     */
    Chillduino& setCompressorTicksPerDefrost(unsigned long ticks) {
      _compressorTicksPerDefrost = ticks;
      _remainingCompressorTicksUntilDefrost = ticks;
      return *this;
    }

    /**
     * Sets the amount of time (in ticks) that the defrost will
     * be running.
     *
     * This value is considered to be a maximum. There is a bimetal
     * that alerts the chillduino to prematurely end a defrost cycle.
     * Assuming the bimetal does not change value, the defrost will
     * run for the specified number of ticks.
     *
     */
    Chillduino& setDefrostDurationInTicks(unsigned long ticks) {
      _defrostDurationInTicks = ticks;
      return *this;
    }

    /**
     * Sets the minimum time (in ticks) that the compressor must wait
     * after changing state before it is able to change state again.
     *
     * This limit is used to prevent excessive relay toggling. Not only
     * would this be inefficient in terms of energy, it can also shorten
     * the life of the relays.
     *
     */
    Chillduino& setMinimumTicksForCompressorChange(unsigned long ticks) {
      _minimumTicksForCompressorChange = ticks;
      return *this;
    }

    /**
     * Sets the minimum time (in ticks) that the door switch reading
     * must remain unchanged before triggering a door close event.
     *
     * The door switch will maintain LOW or HIGH while the door is closed.
     * While the door is open, the value will toggle around once every
     * 10 milliseconds.
     *
     */
    Chillduino& setMinimumTicksForDoorClose(unsigned long ticks) {
      _minimumTicksForDoorClose = ticks;
      return *this;
    }

    /**
     * Sets the current door switch reading.
     *
     * This value should be updated as frequently as possible.
     * The reading is compared to the minimum reading to determine
     * if the door is open or closed.
     *
     */
    Chillduino& setDoorSwitchReading(int reading) {
      _currentDoorSwitchReading = reading;
      return *this;
    }

    /**
     * Sets the mode switch reading.
     *
     * This value should be updated as frequently as possible.
     * The reading is compared to the previous reading to determine
     * if the mode switch was pressed. Once pressed, the mode is
     * cycled in the following order: OFF -> COLD -> COLDER -> COLDEST.
     *
     */
    Chillduino& setModeSwitchReading(int reading) {
      _currentModeSwitchReading = reading;
      return *this;
    }

    /**
     * Sets the current mode of the chillduino.
     *
     * This should only be used after reboot as a way to continue
     * using the previous mode selection. This will force the mode
     * so that the mode switch will continue from this setting.
     *
     */
    Chillduino& setMode(int mode) {
      _mode = mode;
      return *this;
    }

    /**
     * Gets the current mode of the chillduino.
     *
     * The mode will change when the user has pressed the mode switch.
     *
     */
    int getMode(void) const {
      return _mode;
    }

    /**
     * Returns true if the compressor is running.
     *
     */
    bool isCompressorRunning(void) const {
      return _isCompressorRunning;
    }

    /**
     * Returns true if the defrost is running.
     *
     */
    bool isDefrostRunning(void) const {
      return _isDefrostRunning;
    }

    /**
     * Returns true if the fresh food door is open.
     *
     */
    bool isDoorOpen(void) const {
      return _isDoorOpen;
    }

    /**
     * Returns true if any of the output values have changed due to a
     * change in an input value.
     *
     * This function is used to determine whether the chillduino should
     * write new output values.
     *
     */
    bool isChanged(void) const {
      return _isChanged;
    }

    /**
     * Advances the chillduino timers by a single tick.
     *
     * The tick frequency should be consistent and chosen in a way that
     * makes sense for your system. It is preferable to have a 1 millisecond
     * as this is easily acheived on most systems by timer interrupts.
     * Ideally this function would be called from an interrupt service routine.
     *
     */
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

      if (_remainingTicksForDoorClose > 0) {
        _remainingTicksForDoorClose--;
      }
    }

    /**
     * Checks the input values for changes and modifies outputs accordingly.
     *
     * This function should be called as frequently as possible.
     * It is preferable (although not required) to have the update loop
     * called at the same rate as the tick. This function must not be
     * called from an interrupt as its time is not guaranteed.
     *
     */
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

      if (isDoorSwitchChanged()) {
        updatePreviousDoorSwitchReading();
        resetDoorSwitchTicks();
      }
      else {
        checkForDoorClose();
      }

      if (isModeSwitchChanged()) {
        if (isModeSwitchPressed()) {
          cycleToNextMode();
        }

        updatePreviousModeSwitchReading();
      }
    }

    /**
     * Causes the amount of time (in ticks) to elapse.
     *
     * This is a helper function that should only be used for testing.
     * This function should not be used in production.
     *
     */
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

    bool isDoorSwitchChanged(void) const {
      return _previousDoorSwitchReading != _currentDoorSwitchReading;
    }

    void updatePreviousDoorSwitchReading(void) {
      _previousDoorSwitchReading = _currentDoorSwitchReading;
    }

    void resetDoorSwitchTicks(void) {
      if (!_isDoorOpen) {
        _isDoorOpen = true;
        _isChanged = true;
      }

      _remainingTicksForDoorClose = _minimumTicksForDoorClose;
    }

    void checkForDoorClose(void) {
      if (_isDoorOpen && _remainingTicksForDoorClose == 0) {
        _isDoorOpen = false;
        _isChanged = true;
      }
    }

    bool isModeSwitchChanged(void) const {
      return _previousModeSwitchReading != _currentModeSwitchReading;
    }

    bool isModeSwitchPressed(void) const {
      return (_previousModeSwitchReading == 0)
        && (_currentModeSwitchReading == 1);
    }

    void updatePreviousModeSwitchReading(void) {
      _previousModeSwitchReading = _currentModeSwitchReading;
    }

    void cycleToNextMode(void) {
      _mode = (_mode + 1) % CHILLDUINO_MODE_COUNT;
      _isChanged = true;
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
