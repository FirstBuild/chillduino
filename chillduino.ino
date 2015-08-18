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

#include <EEPROM.h>
#include <chillhub.h>
#include "chillduino.h"

#define CHILLDUINO_UUID "e79f1dd1-48bb-4305-a306-6bb3ca53a5b7"
#define THERMISTOR_ID    0x91
#define COMPRESSOR_ID    0x92
#define DEFROST_ID       0x93
#define DOOR_ID          0x94
#define BIMETAL_ID       0x95

#define RX               0
#define TX               1
#define LED_MODE_COLDEST 4
#define LED_MODE_COLDER  5
#define LED_MODE_COLD    6
#define LED_MODE_OFF     7
#define COMPRESSOR       9
#define DEFROST          10
#define DEFROST_SWITCH   12
#define DOOR_LED         13
#define MODE_SWITCH      SCK
#define THERMISTOR       A0
#define RELAY_WATCHDOG   A2
#define DOOR_SWITCH      A4

#define THERMISTOR_MIN_COLD    294
#define THERMISTOR_MAX_COLD    392
#define THERMISTOR_MIN_COLDER  255
#define THERMISTOR_MAX_COLDER  357
#define THERMISTOR_MIN_COLDEST 231
#define THERMISTOR_MAX_COLDEST 335

#define TICKS_PER_SECOND   ((unsigned long) 1000)
#define TICKS_PER_MINUTE   (60 * TICKS_PER_SECOND)
#define TICKS_PER_HOUR     (60 * TICKS_PER_MINUTE)

#define EEPROM_UNSET 0xff
#define EEPROM_COMPRESSOR_RUNTIME 0
#define EEPROM_MODE (EEPROM_COMPRESSOR_RUNTIME + sizeof(unsigned long))
#define COMPRESSOR_RUNTIME (100 * TICKS_PER_HOUR)

#define BRIGHTNESS_STEP_IN_MILLISECONDS 5
#define MIN(a, b) ((a) < (b) ? (a) : (b))

Chillduino chillduino;
chInterface ChillHub;
int watchdog = 0;
unsigned long runtime = 0;
int mode = 0;

SIGNAL(TIMER0_COMPA_vect) {
  chillduino.tick();

  digitalWrite(RELAY_WATCHDOG, watchdog);
  watchdog ^= 1;
}

void setInterrupt(void) {
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void clearInterrupt(void) {
  TIMSK0 &= ~_BV(OCIE0A);
}

void chillduino_announce(void) {
  ChillHub.setup("chillduino", CHILLDUINO_UUID);
  ChillHub.subscribe(deviceIdRequestType, (chillhubCallbackFunction) chillduino_announce);
  ChillHub.subscribe(keepAliveType, (chillhubCallbackFunction) chillduino_keepalive);
  ChillHub.subscribe(setDeviceUUIDType, (chillhubCallbackFunction) chillduino_set_uuid);
  ChillHub.createCloudResourceU16("thermistor", THERMISTOR_ID, 0, 0);
  ChillHub.createCloudResourceU16("compressor", COMPRESSOR_ID, 0, 0);
  ChillHub.createCloudResourceU16("defrost", DEFROST_ID, 0, 0);
  ChillHub.createCloudResourceU16("door", DOOR_ID, 0, 0);
  ChillHub.createCloudResourceU16("bimetal", BIMETAL_ID, 0, 0);
}

void chillduino_keepalive(uint8_t unused) {
  (void) unused;
}

void chillduino_set_uuid(uint8_t *uuid) {
  (void) uuid;
}

void chillduino_push(void) {
  static unsigned long previous = millis();
  unsigned long current = millis();

  if ((current - previous) > 5000) {
    previous = current;
    ChillHub.updateCloudResourceU16(THERMISTOR_ID, analogRead(THERMISTOR));
  }
}

void adjust_brightness(void) {
  static int brightness = 0;
  static unsigned long previous = millis();
  unsigned long current = millis();

  if ((current - previous) > BRIGHTNESS_STEP_IN_MILLISECONDS) {
    previous = current;

    if (chillduino.isDoorOpen()) {
      brightness = MIN(brightness + 1, 255);
    }
    else {
      brightness = 0;
    }

    analogWrite(DOOR_LED, brightness);
  }
}

void setup(void) {
  pinMode(RX, INPUT);
  pinMode(TX, INPUT);
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);
  pinMode(LED_MODE_OFF, OUTPUT);
  pinMode(LED_MODE_COLD, OUTPUT);
  pinMode(LED_MODE_COLDER, OUTPUT);
  pinMode(LED_MODE_COLDEST, OUTPUT);
  pinMode(DOOR_SWITCH, INPUT);
  pinMode(DEFROST_SWITCH, INPUT);
  pinMode(COMPRESSOR, OUTPUT);
  pinMode(DEFROST, OUTPUT);
  pinMode(RELAY_WATCHDOG, OUTPUT);
  pinMode(DOOR_LED, OUTPUT);

  TCCR4B &= 0b11111001;

  EEPROM.get(EEPROM_COMPRESSOR_RUNTIME, runtime);

  if (runtime > COMPRESSOR_RUNTIME) {
    runtime = COMPRESSOR_RUNTIME;
  }

  mode = EEPROM.read(EEPROM_MODE);

  switch (mode) {
    case CHILLDUINO_MODE_OFF:
    case CHILLDUINO_MODE_COLD:
    case CHILLDUINO_MODE_COLDER:
    case CHILLDUINO_MODE_COLDEST:
      break;

    default:
      mode = CHILLDUINO_MODE_COLDER;
      break;
  }

  chillduino
    .setMode(mode)
    .setMinimumFreshFoodThermistorReading(THERMISTOR_MIN_COLDER)
    .setMaximumFreshFoodThermistorReading(THERMISTOR_MAX_COLDER)
    .setMinimumCompressorTicksPerDefrost(24 * TICKS_PER_HOUR)
    .setMaximumCompressorTicksPerDefrost(COMPRESSOR_RUNTIME)
    .setRemainingCompressorTicksUntilDefrost(runtime)
    .setDefrostDurationInTicks(30 * TICKS_PER_MINUTE)
    .setMinimumTicksForCompressorChange(10 * TICKS_PER_MINUTE)
    .setMinimumTicksForDoorClose(100)
    .setMinimumTicksForHeldModeSwitch(3 * TICKS_PER_SECOND)
    .setMinimumTicksForForceDefrost(5 * TICKS_PER_SECOND)
    .setMinimumOpensForForceDefrost(3)
    .setCompressorTicksPerDoorOpen(4 * TICKS_PER_HOUR)
    .setMinimumTicksForBimetalCutoff(100);

  Serial.begin(115200);

  setInterrupt();
  chillduino_announce();
}

void loop(void) {
  chillduino.setCurrentFreshFoodThermistorReading(analogRead(THERMISTOR));
  chillduino.setDoorSwitchReading(digitalRead(DOOR_SWITCH));
  chillduino.setModeSwitchReading(digitalRead(MODE_SWITCH));
  chillduino.setDefrostSwitchReading(digitalRead(DEFROST_SWITCH));
  chillduino.loop();

  int current = chillduino.getRemainingCompressorTicksUntilDefrost()
    / TICKS_PER_HOUR;

  int previous = runtime / TICKS_PER_HOUR;

  if (current != previous) {
    runtime = chillduino.getRemainingCompressorTicksUntilDefrost();
    EEPROM.put(EEPROM_COMPRESSOR_RUNTIME, runtime);

    // Serial.print("Compressor ticks until defrost: ");
    // Serial.println(runtime);
  }

  if (chillduino.isChanged()) {
    // Serial.println(chillduino.isDoorOpen()
    //  ? "Door is open" : "Door is closed");

    // Serial.println(chillduino.isCompressorRunning()
    //  ? "Compressor is running" : "Compressor is not running");

    // Serial.println(chillduino.isDefrostRunning()
    //  ? "Defrost is running" : "Defrost is not running");
    
    if (chillduino.isWiFiToggled()) {
      // Serial.println("WiFi is toggled");
      ChillHub.sendU8Msg(0x2e, 0);
    }

    if (mode != chillduino.getMode()) {
      mode = chillduino.getMode();
      EEPROM.write(EEPROM_MODE, mode);
    }

    switch (chillduino.getMode()) {
      case CHILLDUINO_MODE_OFF:
        digitalWrite(LED_MODE_OFF, 1);
        digitalWrite(LED_MODE_COLD, 0);
        digitalWrite(LED_MODE_COLDER, 0);
        digitalWrite(LED_MODE_COLDEST, 0);
        break;

      case CHILLDUINO_MODE_COLD:
        digitalWrite(LED_MODE_OFF, 0);
        digitalWrite(LED_MODE_COLD, 1);
        digitalWrite(LED_MODE_COLDER, 0);
        digitalWrite(LED_MODE_COLDEST, 0);
        chillduino.setMinimumFreshFoodThermistorReading(THERMISTOR_MIN_COLD);
        chillduino.setMaximumFreshFoodThermistorReading(THERMISTOR_MAX_COLD);
        break;

      case CHILLDUINO_MODE_COLDER:
        digitalWrite(LED_MODE_OFF, 0);
        digitalWrite(LED_MODE_COLD, 0);
        digitalWrite(LED_MODE_COLDER, 1);
        digitalWrite(LED_MODE_COLDEST, 0);
        chillduino.setMinimumFreshFoodThermistorReading(THERMISTOR_MIN_COLDER);
        chillduino.setMaximumFreshFoodThermistorReading(THERMISTOR_MAX_COLDER);
        break;

      case CHILLDUINO_MODE_COLDEST:
        digitalWrite(LED_MODE_OFF, 0);
        digitalWrite(LED_MODE_COLD, 0);
        digitalWrite(LED_MODE_COLDER, 0);
        digitalWrite(LED_MODE_COLDEST, 1);
        chillduino.setMinimumFreshFoodThermistorReading(THERMISTOR_MIN_COLDEST);
        chillduino.setMaximumFreshFoodThermistorReading(THERMISTOR_MAX_COLDEST);
        break;

      default:
        break;
    };
    
    int isCompressorRunning = chillduino.isCompressorRunning();
    int isDefrostRunning = chillduino.isDefrostRunning();
    int isDoorOpen = chillduino.isDoorOpen();
    int isBimetalCutoff = chillduino.isBimetalCutoff();

    digitalWrite(COMPRESSOR, isCompressorRunning);
    digitalWrite(DEFROST, isDefrostRunning);

    ChillHub.updateCloudResourceU16(COMPRESSOR_ID, isCompressorRunning);
    ChillHub.updateCloudResourceU16(DEFROST_ID, isDefrostRunning);
    ChillHub.updateCloudResourceU16(DOOR_ID, isDoorOpen);
    ChillHub.updateCloudResourceU16(BIMETAL_ID, isBimetalCutoff);
  }

  adjust_brightness();
  chillduino_push();
  ChillHub.loop();
}
