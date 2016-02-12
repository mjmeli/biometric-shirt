/*
ad5933-test
    Reads impedance values from the AD5933 over I2C and prints them serially.

    Connections:
      |-------------------|-------------|
      | Sensor  | RFduino | Description |
      |-------------------|-------------|
      | RED     | +3V     | Power       |
      | WHITE   | GPIO2   | Temp Sensor |
      | BLACK   | GND     | Ground      |
      |-------------------|-------------|
*/

#include <RFduinoBLE.h>

void setup(void)
{
  // Begin bluetooth
  //RFduinoBLE.advertisementData = "demo";
  //RFduinoBLE.begin();

  // Begin serial at 9600 baud
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");
}

void loop(void)
{ 
  // Delay for a minute
  RFduino_ULPDelay( SECONDS(60) );
}

