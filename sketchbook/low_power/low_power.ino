// Test Atmega328P on a breadboard (8 MHz internal clock) low power.
//
// The MCU is flashed and wired with the minimal circuit and documented at
// https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoToBreadboard/
//
// The current draws is 1.4μA at 5V when using `LowPower.powerDown` and `powerDown`
// with the fuse extended=0xFF, fuse low=0xE2, fuse high=0xDA
//
// Surprisingly, calling `power_all_disable()` from <avr/power.h> has a negative effect
// and results in a 312.0μA draw at 5V.


#include <avr/boot.h>
#include <avr/sleep.h>
#include <avr/power.h>

// Install "Low-Power by Rocket Screan Electronics
// https://github.com/rocketscream/Low-Power
#include "LowPower.h"

// Fetches and display the fuses values.
void showFuseValues() {
  cli();
  unsigned char fuse_bits_low = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
  byte fuse_bits_extended = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
  byte fuse_bits_high = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
  byte lock_bits = boot_lock_fuse_bits_get(GET_LOCK_BITS);
  sei();

  Serial.print("fuse_bits_extended: 0x");
  Serial.print(fuse_bits_extended, HEX);
  Serial.print("  0b");
  Serial.println(fuse_bits_extended, BIN);

  Serial.print("fuse_bits_low     : 0x");
  Serial.print(fuse_bits_low, HEX);
  Serial.print("  0b");
  Serial.println(fuse_bits_low, BIN);

  Serial.print("fuse_bits_high    : 0x");
  Serial.print(fuse_bits_high, HEX);
  Serial.print("  0b");
  Serial.println(fuse_bits_high, BIN);

  Serial.println();
}

void setup() {
  delay(100);
  Serial.begin(115200);
  Serial.println("\n----- Setup -----");
  showFuseValues();
}

void powerDown(period_t period, adc_t adc, bod_t bod) {
  // Taken from `LowPower.powerDown`.
  ADCSRA &= ~(1 << ADEN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_enable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  sleep_disable();
  sei();
  ADCSRA |= (1 << ADEN);
}

void loop() {
  Serial.println("Wait 2 secs, now");
  delay(2000);
  Serial.println("Sleep !");
  delay(500);

  switch (0) {
    case 0:
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
      break;
    case 1:
      powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
      break;
  }
}
