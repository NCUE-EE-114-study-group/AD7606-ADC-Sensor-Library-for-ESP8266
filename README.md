# AD7606 ADC Sensor Library for ESP8266 (ESP12E)

English | [繁體中文](./README.zh-Hant.md)

**Description**  
This library provides functions to communicate with an AD7606 ADC using an ESP8266 board (such as NodeMCU). It utilizes hardware SPI to read 8 channels of data from the AD7606, allowing you to easily retrieve voltage measurements.

**Features**  
- Hardware SPI reading from AD7606  
- Customizable SPI frequency, bit order, and mode  
- Simple offset threshold (scaleFactor & errorConst)  
- Helper function to print all 8 channel voltages at once  

## Installation

1. Download or clone this repository.  
2. Place the folder `AD7606-ADC-Sensor-Library-for-ESP8266` into the `libraries` folder of your Arduino installation.  
3. Restart the Arduino IDE (or click **Sketch > Include Library > Add .ZIP Library** if you have a ZIP file).

## Usage

1. In your sketch, add `#include <AD7606_ESP8266.h>`.  
2. Create an `AD7606_ESP8266` object.  
3. Call `setPins()` if needed to specify custom pins (otherwise default pins are used).  
4. Call `setSPISettings()` if you need different SPI parameters (default is 1MHz, MSBFIRST, SPI_MODE0).  
5. Call `begin()` to initialize the device.  
6. Use `readData()` and `parseRawBytes()` to read data, then retrieve voltage with `getVoltage(channel)`, or use `printAllVoltages()` to print all channels.  

## ESP8266 Setup
![optional description](assets/ESP8266-AD7606_wiring.png)

## Example

See the example in `examples/BasicRead/BasicRead.ino` for a quick start.

## License

This project is released under the MIT License - see the [LICENSE](./LICENSE) file for details.
