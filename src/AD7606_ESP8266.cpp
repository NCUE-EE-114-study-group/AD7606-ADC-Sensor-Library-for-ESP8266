#include "AD7606_ESP8266.h"

// Include SPI so we can use SPI.beginTransaction(), SPI.transfer(), etc.
#include <SPI.h>

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
AD7606_ESP8266::AD7606_ESP8266() {
    // By default, we do not create a SPISettings instance here.
    // The user can call setSPISettings() to override defaults,
    // or the library will initialize a default one during begin().
}

//----------------------------------------------------------------------------
// Set pin assignments
//----------------------------------------------------------------------------
void AD7606_ESP8266::setPins(uint8_t busyPin, uint8_t resetPin, uint8_t startConvPin,
                             uint8_t csPin, uint8_t misoPin, uint8_t sckPin)
{
    _busyPin  = busyPin;
    _resetPin = resetPin;
    _startPin = startConvPin;
    _csPin    = csPin;
    _misoPin  = misoPin;
    _sckPin   = sckPin;
}

//----------------------------------------------------------------------------
// Set SPI parameters
//----------------------------------------------------------------------------
void AD7606_ESP8266::setSPISettings(uint32_t clockFreq, uint8_t bitOrder, uint8_t dataMode) {
    // If there's already a SPISettings instance, delete it first
    if (_spiSettingsPtr) {
        delete _spiSettingsPtr;
        _spiSettingsPtr = nullptr;
    }
    _spiSettingsPtr = new SPISettings(clockFreq, bitOrder, dataMode);
}

//----------------------------------------------------------------------------
// Set measurement constants
//----------------------------------------------------------------------------
void AD7606_ESP8266::setConstants(float scaleFactor, float errorConstant) {
    _scaleFactor = scaleFactor;
    _errorConst  = errorConstant;
}

//----------------------------------------------------------------------------
// Initialize AD7606 (GPIO, SPI.begin(), and hardware reset)
//----------------------------------------------------------------------------
void AD7606_ESP8266::begin() {
    // If no SPISettings has been set by the user, initialize one with defaults
    if (!_spiSettingsPtr) {
        _spiSettingsPtr = new SPISettings(1000000, MSBFIRST, SPI_MODE0);
    }

    // Configure pin modes
    pinMode(_busyPin,  INPUT);
    pinMode(_resetPin, OUTPUT);
    pinMode(_startPin, OUTPUT);
    pinMode(_csPin,    OUTPUT);

    // For ESP8266 hardware SPI:
    // - MISO is GPIO12
    // - MOSI is GPIO13
    // - SCK  is GPIO14
    // - SS   is GPIO15
    // Make sure your wiring matches these pins if using hardware SPI
    pinMode(_misoPin, INPUT);
    pinMode(_sckPin,  OUTPUT);

    // Default output states
    digitalWrite(_startPin, HIGH);
    digitalWrite(_csPin,    HIGH);

    // Perform a hardware reset
    _reset();

    // Begin hardware SPI
    SPI.begin();
}

//----------------------------------------------------------------------------
// Hardware reset of AD7606
//----------------------------------------------------------------------------
void AD7606_ESP8266::_reset() {
    // Generate a short low pulse
    digitalWrite(_resetPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(_resetPin, LOW);
    delayMicroseconds(1);
}

//----------------------------------------------------------------------------
// Trigger a single conversion
//----------------------------------------------------------------------------
void AD7606_ESP8266::_startConversion() {
    // Generate a short low pulse
    digitalWrite(_startPin, LOW);
    delayMicroseconds(1);
    digitalWrite(_startPin, HIGH);
}

//----------------------------------------------------------------------------
// Read 16 bytes from AD7606
//----------------------------------------------------------------------------
void AD7606_ESP8266::readData() {
    // Trigger a conversion first
    _startConversion();

    // Wait for BUSY pin to go LOW, indicating conversion is complete
    while (digitalRead(_busyPin) == HIGH) {
        delayMicroseconds(10);
    }

    // Use the SPISettings
    SPI.beginTransaction(*_spiSettingsPtr);
    digitalWrite(_csPin, LOW);

    // Read 16 bytes in a row
    for (int i = 0; i < _rawBytesCount; i++) {
        _raw[i] = SPI.transfer(0x00);
    }

    digitalWrite(_csPin, HIGH);
    SPI.endTransaction();
}

//----------------------------------------------------------------------------
// Parse raw bytes into eight 16-bit values
//----------------------------------------------------------------------------
void AD7606_ESP8266::parseRawBytes() {
    // Each pair of bytes forms one 16-bit value
    for (int i = 0; i < 8; i++) {
        _parsed[i] = ((uint16_t)_raw[i * 2] << 8) | (_raw[i * 2 + 1]);
    }
}

//----------------------------------------------------------------------------
// Get the raw 16-bit value from the specified channel
//----------------------------------------------------------------------------
uint16_t AD7606_ESP8266::getRawValue(uint8_t channel) {
    if (channel < 8) {
        return _parsed[channel];
    }
    return 0; // Out of range
}

//----------------------------------------------------------------------------
// Get the scaled voltage from the specified channel
//----------------------------------------------------------------------------
float AD7606_ESP8266::getVoltage(uint8_t channel) {
    if (channel >= 8) {
        return 0.0;
    }
    float scaledVoltage = _parsed[channel] * _scaleFactor;
    // If it's below the error constant, consider it as 0
    if (scaledVoltage <= _errorConst) {
        return 0.0;
    }
    return scaledVoltage;
}

//----------------------------------------------------------------------------
// Print all 8 channel voltages (comma-separated)
//----------------------------------------------------------------------------
void AD7606_ESP8266::printAllVoltages(Stream &out) {
    for (int i = 0; i < 8; i++) {
        float voltage = getVoltage(i);
        out.print(voltage, 5); // Print with 5 decimal places
        if (i < 7) out.print(",");
    }
    out.println();
}
