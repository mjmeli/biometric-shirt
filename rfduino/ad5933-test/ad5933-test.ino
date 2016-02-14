/*
ad5933-test
    Reads impedance values from the AD5933 over I2C and prints them serially.

    Connections:
      |-------------------|-------------|
      | Wire    | RFduino | Description |
      |-------------------|-------------|
      | RED     | +3V     | Power       |
      | GREEN   | GPIO2   | Temp Sensor |
      | WHITE   | GPIO5   | I2C SCL     |
      | YELLOW  | GPIO6   | I2C SDC     |
      | BLACK   | GND     | Ground      |
      |-------------------|-------------|
*/

#include <RFduinoBLE.h>
#include <Wire.h>
#include "AD5933.h"

AD5933 ad5933;

void setup(void)
{
  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");

  // Begin I2C
  Wire.begin();
}

void loop(void)
{ 
  // Test - get temperature
  double temp = ad5933.getTemperature();
  Serial.println(temp);
  
  // Delay for a minute
  RFduino_ULPDelay( SECONDS(5) );
}
