#ifndef AD7606_ESP8266_H
#define AD7606_ESP8266_H

#include <Arduino.h>
#include <SPI.h>

class AD7606_ESP8266 {
public:
    // Constructor
    AD7606_ESP8266();

    // Set pin assignments (override defaults if needed)
    void setPins(uint8_t busyPin, uint8_t resetPin, uint8_t startConvPin,
                 uint8_t csPin, uint8_t misoPin, uint8_t sckPin);

    // Set SPI parameters (clock frequency, bit order, SPI mode)
    void setSPISettings(uint32_t clockFreq, uint8_t bitOrder = MSBFIRST, uint8_t dataMode = SPI_MODE0);

    // Set measurement constants: scaleFactor and errorConstant
    void setConstants(float scaleFactor, float errorConstant);

    // Initialize AD7606 (GPIO, SPI, hardware reset)
    void begin();

    // Trigger a conversion and read 16 bytes of data
    void readData();

    // Parse the 16 raw bytes into eight 16-bit values
    void parseRawBytes();

    // Get the raw 16-bit value for a given channel
    uint16_t getRawValue(uint8_t channel);

    // Get the actual voltage for a given channel (apply scaleFactor and errorConstant)
    float getVoltage(uint8_t channel);

    // Print all 8 channel voltages as comma-separated values
    void printAllVoltages(Stream &out = Serial);

private:
    // Default pin assignments (can be overridden by setPins())
    uint8_t _busyPin  = 5;
    uint8_t _resetPin = 4;
    uint8_t _startPin = 16;
    uint8_t _csPin    = 15;
    uint8_t _misoPin  = 12;
    uint8_t _sckPin   = 14;

    // Pointer to an SPISettings instance; can be updated by setSPISettings()
    SPISettings* _spiSettingsPtr = nullptr;

    // Measurement constants (overridden by setConstants())
    float _scaleFactor = 0.000152587890625; // Default reference
    float _errorConst  = 1.95;              // If voltage <= this, treat as 0

    // AD7606 buffer
    static const uint8_t _rawBytesCount = 16;
    byte _raw[_rawBytesCount];
    uint16_t _parsed[8];

    // Internal helper functions
    void _reset();
    void _startConversion();
};

#endif // AD7606_ESP8266_H
