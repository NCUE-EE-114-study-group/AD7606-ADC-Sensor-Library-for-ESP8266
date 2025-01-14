#include <Arduino.h>
#include <AD7606_ESP8266.h>

// Create an AD7606_ESP8266 object
AD7606_ESP8266 ad7606;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // Some boards need extra time for the Serial to initialize
  }

  // Configure pins if necessary; this example shows all pins explicitly
  // Make sure to match your actual wiring
  ad7606.setPins(
    5,   // BUSY
    4,   // RESET
    16,  // START_CONVERSION
    15,  // CHIP_SELECT
    12,  // MISO
    14   // SCK
  );

  // Configure SPI transmission settings
  // Default: 1MHz, MSBFIRST, SPI_MODE0
  ad7606.setSPISettings(1000000, MSBFIRST, SPI_MODE0);

  // Measurement constants: scaleFactor & errorConst
  // If (rawValue * scaleFactor) <= errorConst, it is treated as 0
  ad7606.setConstants(0.000152587890625, 1.95);

  // Initialize AD7606: includes GPIO setup, SPI begin, and reset
  ad7606.begin();
}

void loop() {
  // Read raw data from AD7606
  ad7606.readData();

  // Parse the 16 bytes of raw data into 8 x 16-bit values
  ad7606.parseRawBytes();

  // Print out 8 channel voltages separated by commas
  ad7606.printAllVoltages(Serial);

  delay(100); // Read every 0.1 seconds
}
