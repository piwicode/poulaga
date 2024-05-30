## The real time clock module

I use a `DNIGEF-DS3231 AT24C32 IIC Precision RTC Real Time Clock Memory Tech`
device that combines [AT24C32](datasheets/AT24C32.pdf) EEPROM and a
[DS3231](datasheets/DS3231.pdf) real time clock on an
[I2C bus](https://learn.adafruit.com/working-with-i2c-devices/overview).

The realtime clock device uses a power supply when available and fallsback
on a battery. The time keeping feature uses 1 to 3.5 µA (IBATT).
A typical CR2032 battery is 235 mAh (to 2.0 volts) and will last 12 years.

The module has power pins VCC and GND, and also I2C line (SDA and SCL)
which can be found on [Arduino UNO pinout](datasheets/A000066-full-pinout.pdf).


## Display

TM1637.h