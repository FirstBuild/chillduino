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
#include <assert.h>

#define TICKS_PER_SECOND   ((unsigned long) 1000)
#define TICKS_PER_MINUTE   (60 * TICKS_PER_SECOND)
#define TICKS_PER_HOUR     (60 * TICKS_PER_MINUTE)

Chillduino createChillduino(void) {
  return Chillduino()
    .setMode(CHILLDUINO_MODE_COLDER)
    .setMinimumFreshFoodThermistorReading(370)
    .setMaximumFreshFoodThermistorReading(392)
    .setMinimumCompressorTicksPerDefrost(TICKS_PER_HOUR)
    .setMaximumCompressorTicksPerDefrost(2 * TICKS_PER_HOUR)
    .setRemainingCompressorTicksUntilDefrost(2 * TICKS_PER_HOUR)
    .setDefrostDurationInTicks(30 * TICKS_PER_MINUTE)
    .setMinimumTicksForCompressorChange(10 * TICKS_PER_MINUTE)
    .setMinimumTicksForDoorClose(100)
    .setMinimumTicksForHeldModeSwitch(3 * TICKS_PER_SECOND)
    .setMinimumTicksForForceDefrost(5 * TICKS_PER_SECOND)
    .setMinimumOpensForForceDefrost(3)
    .setCompressorTicksPerDoorOpen(15 * TICKS_PER_MINUTE)
    .setMinimumTicksForBimetalCutoff(100);
}

void shouldStartWithCompressorAndDefrostNotRunning(void) {
  Chillduino chillduino = createChillduino();

  assert(!chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());
}

void shouldStartCompressorWhenFreshFoodIsWarm(void) {
  Chillduino chillduino = createChillduino()
    .setCurrentFreshFoodThermistorReading(400);

  chillduino.elapse(TICKS_PER_SECOND);
  assert(chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());
}

void shouldLeaveCompressorRunningUntilCooled(void) {
  Chillduino chillduino = createChillduino();

  chillduino.setCurrentFreshFoodThermistorReading(400)
    .elapse(TICKS_PER_HOUR);

  assert(chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());

  chillduino.setCurrentFreshFoodThermistorReading(360)
    .elapse(TICKS_PER_SECOND);

  assert(!chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());
}

void shouldMaintainCompressorOnceStarted(void) {
  Chillduino chillduino = createChillduino();

  chillduino.setCurrentFreshFoodThermistorReading(400)
    .elapse(TICKS_PER_SECOND);

  assert(chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());

  chillduino.setCurrentFreshFoodThermistorReading(360)
    .elapse(TICKS_PER_SECOND);

  assert(chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());
}

void shouldMaintainCompressorOnceStopped(void) {
  Chillduino chillduino = createChillduino();

  chillduino.setCurrentFreshFoodThermistorReading(400)
    .elapse(TICKS_PER_HOUR);

  assert(chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());

  chillduino.setCurrentFreshFoodThermistorReading(360)
    .elapse(TICKS_PER_SECOND);

  assert(!chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());

  chillduino.setCurrentFreshFoodThermistorReading(400)
    .elapse(TICKS_PER_SECOND);

  assert(!chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());
}

void shouldDefrostAfterAccumulatingCompressorRuntime(void) {
  Chillduino chillduino = createChillduino();

  chillduino.setCurrentFreshFoodThermistorReading(400)
    .elapse(2 * TICKS_PER_HOUR + TICKS_PER_MINUTE);

  assert(chillduino.isDefrostRunning());
}

void shouldStopDefrostingWhenComplete(void) {
  Chillduino chillduino = createChillduino();

  chillduino.setCurrentFreshFoodThermistorReading(400)
    .elapse(2 * TICKS_PER_HOUR);

  assert(!chillduino.isDefrostRunning());
  assert(chillduino.isCompressorRunning());
}

void shouldSignalWhenAChangeOccurs(void) {
  Chillduino chillduino = createChillduino();

  assert(!chillduino.isChanged());

  chillduino.setCurrentFreshFoodThermistorReading(400);
  chillduino.tick();
  chillduino.loop();

  assert(chillduino.isChanged());
}

void shouldSampleTheDoorSwitch(void) {
  Chillduino chillduino = createChillduino();
  assert(!chillduino.isDoorOpen());

  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  assert(chillduino.isDoorOpen());

  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);
  assert(!chillduino.isDoorOpen());
}

void shouldSwitchModeWhenButtonIsPressed(void) {
  Chillduino chillduino = createChillduino();
  assert(chillduino.getMode() == CHILLDUINO_MODE_COLDER);

  chillduino.setModeSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);
  chillduino.setModeSwitchReading(0);
  chillduino.elapse(TICKS_PER_SECOND);
  assert(chillduino.getMode() == CHILLDUINO_MODE_COLDEST);

  chillduino.setModeSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);
  chillduino.setModeSwitchReading(0);
  chillduino.elapse(TICKS_PER_SECOND);
  assert(chillduino.getMode() == CHILLDUINO_MODE_OFF);

  chillduino.setModeSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);
  chillduino.setModeSwitchReading(0);
  chillduino.elapse(TICKS_PER_SECOND);
  assert(chillduino.getMode() == CHILLDUINO_MODE_COLD);

  chillduino.setModeSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);
  chillduino.setModeSwitchReading(0);
  chillduino.elapse(TICKS_PER_SECOND);
  assert(chillduino.getMode() == CHILLDUINO_MODE_COLDER);
}

void shouldToggleWiFiWhenButtonIsHeld(void) {
  Chillduino chillduino = createChillduino();

  assert(chillduino.getMode() == CHILLDUINO_MODE_COLDER);
  assert(!chillduino.isWiFiToggled());

  chillduino.setModeSwitchReading(1);
  chillduino.elapse(5 * TICKS_PER_SECOND);
  chillduino.setModeSwitchReading(0);
  chillduino.elapse(TICKS_PER_SECOND);

  assert(chillduino.getMode() == CHILLDUINO_MODE_COLDER);
  assert(chillduino.isWiFiToggled());

  chillduino.setModeSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);
  chillduino.setModeSwitchReading(0);
  chillduino.elapse(TICKS_PER_SECOND);

  assert(chillduino.getMode() == CHILLDUINO_MODE_COLDEST);
  assert(!chillduino.isWiFiToggled());
}

void shouldBeCapableOfForcingADefrostForTesting(void) {
  Chillduino chillduino = createChillduino();

  assert(!chillduino.isDefrostRunning());

  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);

  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);

  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_SECOND);

  assert(chillduino.isDefrostRunning());
  chillduino.elapse(30 * TICKS_PER_MINUTE + TICKS_PER_SECOND);
  assert(!chillduino.isDefrostRunning());
}

void shouldDefrostSoonerWhenDoorIsOpened(void) {
  Chillduino chillduino = createChillduino()
    .setCurrentFreshFoodThermistorReading(400);

  assert(!chillduino.isDefrostRunning());
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_HOUR + 45 * TICKS_PER_MINUTE);
  assert(chillduino.isDefrostRunning());
}

void shouldDefrostNoSoonerThanMinimum(void) {
  Chillduino chillduino = createChillduino()
    .setCurrentFreshFoodThermistorReading(400);

  assert(!chillduino.isDefrostRunning());
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_MINUTE);

  assert(!chillduino.isDefrostRunning());
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_MINUTE);

  assert(!chillduino.isDefrostRunning());
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_MINUTE);

  assert(!chillduino.isDefrostRunning());
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(TICKS_PER_MINUTE);

  assert(!chillduino.isDefrostRunning());
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDoorSwitchReading(1);
  chillduino.elapse(45 * TICKS_PER_MINUTE);

  assert(!chillduino.isDefrostRunning());
}

void shouldStopDefrostingWhenBimetalCutsOffPowerToTheDefrost(void) {
  Chillduino chillduino = createChillduino();

  chillduino.setCurrentFreshFoodThermistorReading(400)
    .elapse(2 * TICKS_PER_HOUR + TICKS_PER_MINUTE);

  chillduino.elapse(20 * TICKS_PER_MINUTE);
  assert(!chillduino.isBimetalCutoff());
  assert(chillduino.isDefrostRunning());

  chillduino.setDefrostSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDefrostSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDefrostSwitchReading(0);
  chillduino.elapse(10);
  chillduino.setDefrostSwitchReading(1);
  chillduino.elapse(10);
  chillduino.setDefrostSwitchReading(0);

  assert(chillduino.isBimetalCutoff());
  assert(!chillduino.isDefrostRunning());

  chillduino.elapse(TICKS_PER_MINUTE);
  assert(!chillduino.isBimetalCutoff());
  assert(!chillduino.isDefrostRunning());
}

void shouldTurnOffCompressorInOffMode(void) {
  Chillduino chillduino = createChillduino()
    .setCurrentFreshFoodThermistorReading(400);

  chillduino.elapse(TICKS_PER_MINUTE);
  assert(chillduino.isCompressorRunning());

  chillduino.setMode(CHILLDUINO_MODE_OFF);
  chillduino.elapse(TICKS_PER_MINUTE);
  assert(!chillduino.isCompressorRunning());
}

void shouldTurnOffDefrostInOffMode(void) {
  Chillduino chillduino = createChillduino()
    .setCurrentFreshFoodThermistorReading(400);

  chillduino.elapse(2 * TICKS_PER_HOUR + TICKS_PER_MINUTE);
  assert(chillduino.isDefrostRunning());

  chillduino.setMode(CHILLDUINO_MODE_OFF);
  chillduino.elapse(TICKS_PER_MINUTE);
  assert(!chillduino.isDefrostRunning());
}

void shouldLeaveOffCompressorAndDefrostInOffMode(void) {
  Chillduino chillduino = createChillduino()
    .setMode(CHILLDUINO_MODE_OFF)
    .setCurrentFreshFoodThermistorReading(400);

  chillduino.elapse(TICKS_PER_HOUR);
  assert(!chillduino.isCompressorRunning());
  assert(!chillduino.isDefrostRunning());
}

#include <iostream>

void shouldPersistCompressorRuntime(void) {
  Chillduino chillduino = createChillduino()
    .setCurrentFreshFoodThermistorReading(400);

  chillduino.elapse(2 * TICKS_PER_HOUR + 1);
  std::cout << chillduino.getRemainingCompressorTicksUntilDefrost() << std::endl;
  assert(chillduino.getRemainingCompressorTicksUntilDefrost() == 0);

  chillduino.elapse(30 * TICKS_PER_MINUTE + 1);
  assert(chillduino.getRemainingCompressorTicksUntilDefrost() ==
    2 * TICKS_PER_HOUR);
}

int main(void) {
  shouldStartWithCompressorAndDefrostNotRunning();
  shouldStartCompressorWhenFreshFoodIsWarm();
  shouldLeaveCompressorRunningUntilCooled();
  shouldMaintainCompressorOnceStarted();
  shouldMaintainCompressorOnceStopped();
  shouldDefrostAfterAccumulatingCompressorRuntime();
  shouldStopDefrostingWhenComplete();
  shouldSignalWhenAChangeOccurs();
  shouldSampleTheDoorSwitch();
  shouldSwitchModeWhenButtonIsPressed();
  shouldToggleWiFiWhenButtonIsHeld();
  shouldBeCapableOfForcingADefrostForTesting();
  shouldDefrostSoonerWhenDoorIsOpened();
  shouldDefrostNoSoonerThanMinimum();
  shouldStopDefrostingWhenBimetalCutsOffPowerToTheDefrost();
  shouldTurnOffCompressorInOffMode();
  shouldTurnOffDefrostInOffMode();
  shouldLeaveOffCompressorAndDefrostInOffMode();
  shouldPersistCompressorRuntime();

  return 0;
}
