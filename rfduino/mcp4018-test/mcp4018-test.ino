/*
mcp4018-test
    Sends various values to the digital potentialmeter MCP4018 to test it. Can send the value
    over serial connection as an ASCII string representing the value.

    Connections:
      |-------------------|-------------|
      | Wire    | RFduino | Description |
      |-------------------|-------------|
      | RED     | +3V     | Power       |
      | GREEN   | GPIO2   | Temp Sensor |
      | BLUE    | GPIO3   | Time Measure|
      | WHITE   | GPIO5   | I2C SCL     |
      | YELLOW  | GPIO6   | I2C SDC     |
      | BLACK   | GND     | Ground      |
      |-------------------|-------------|
*/

#include <RFduinoBLE.h>
#include <Wire.h>
#include "MCP4018.h"

void setup(void)
{
  // Begin I2C
  Wire.begin();

  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("MCP4018 Test Started!");
}

void loop(void)
{
    // Repeatedly send values of 0 to 127
    for (byte potVal = 0; potVal <= 127; potVal++) {
        if (MCP4018::setValue(potVal)) {
          Serial.print("Sent ");
          Serial.println(potVal);
      } else {
          Serial.println("Sending failed");
      }
        RFduino_ULPDelay( SECONDS(1) );
    }
    
    // Read a value from serial and handle it
    /*
    String str;

    // Read each ASCII character sent
    while (Serial.available() > 0) {
        char c = Serial.read();
        str += c;
        RFduino_ULPDelay(2);
    }

    // Convert ASCII to integer
    int val = -1;
    if (str.length() > 0) {
        val = str.toInt();

        // Send value if it is valid
        if (val >= 0 && val <= 127) {
            if (MCP4018::setValue(val)) {
                Serial.print("Sent ");
                Serial.println(val);
            } else {
                Serial.println("Sending failed");
            }
        } else {
            Serial.println("Invalid!");
        }
    } */
}
