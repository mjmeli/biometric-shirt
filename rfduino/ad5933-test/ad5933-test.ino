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
  // Begin I2C
  Wire.begin();
  
  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");

  // Set up
  ad5933.reset();
  ad5933.setInternalClock(true);
  ad5933.setStartFrequency(30000);
  ad5933.setIncrementFrequency(1000);
  ad5933.setNumberIncrements(10);
  ad5933.setPGAGain(PGA_GAIN_X1);
}

void loop(void)
{ 
  // Test - get temperature
  double temp = ad5933.getTemperature();
  Serial.println(temp);

  // Perform frequency sweep
  ad5933.setPowerMode(POWER_STANDBY); // standby 
  ad5933.setControlMode(CTRL_INIT_START_FREQ);
  RFduino_ULPDelay(50);
  ad5933.setControlMode(CTRL_START_FREQ_SWEEP);
  RFduino_ULPDelay(50);
  while (true) {
    int real, imag;
    if (ad5933.getComplexData(&real, &imag)) {
      Serial.print("REAL=");
      Serial.print(real);
      Serial.print(" IMAG=");
      Serial.println(imag);
    } else {
      Serial.println("Failed to get data");
      break;
    }
    if ((ad5933.readStatusRegister() & STATUS_FREQ_SWEEP_DONE) == STATUS_FREQ_SWEEP_DONE) {
      Serial.println("Frequency sweep completed");
      break;
    } else {
      ad5933.setControlMode(CTRL_INCREMENT_FREQ);
      RFduino_ULPDelay(50);
    }
  }
  
  // Delay
  RFduino_ULPDelay( SECONDS(5) );
}
