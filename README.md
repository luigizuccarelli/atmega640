# atmega640 iot-controller

## Overview 

The software used in this project cover
- xl335 accelerometer
- ds18b20 digital temperature sensor

It ouptuts data to the serial usb /dev/ttyUSB0 (which can be configured)
The main loop waits for a char from the serial port and then takes a mesurement of both sensors

## Interface
As mentioned all connectovity is from and to the serial usb device
This gives a good layer of abstraction (not dependant on specific pinouts)
The controller is an omega onion + written in golang
Data transferred for the temperature probe and accelerometer are int values, converiosn to float takes place at the golang omega contoller

## AVR fuses
Internal clock divide by 8 startup time set to max , spi and jtag enable, bootsize set to 00

```bash
-U lfuse:w:0x62:m -U hfuse:w:0x99:m -U efuse:w:0xff:m
```
