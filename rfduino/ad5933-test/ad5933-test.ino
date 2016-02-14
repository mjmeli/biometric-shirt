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
  int tTemp[2];
  long tTempVal;
  double cTemp;
  tTemp[0] = getByte(TEMP_DATA_1);
  tTemp[1] = getByte(TEMP_DATA_2);
  tTempVal = (tTemp[0] << 8 | tTemp[1]) & 0x1FFF;
  if ((tTemp[0] & (1<<5)) == 0) {
    cTemp = tTempVal/32.0;
  } else {
    cTemp = (tTempVal-16384)/32.0;
  }
  Serial.print(tTemp[0]);
  Serial.print(" ");
  Serial.print(tTemp[1]);
  Serial.print(" ");
  Serial.println(cTemp);
  
  // Delay for a minute
  RFduino_ULPDelay( SECONDS(5) );
}

int getByte(int address) {
  int rxByte;
  if (!setByte(ADDR_PTR, address))
    return false;

   Wire.requestFrom(AD5933_ADDR, 1);

   if (1 <= Wire.available()) {
    rxByte = Wire.read();
   }
   else {
    rxByte = -1;
   }
   return rxByte;
}

bool setByte(int address, int value) {
  Wire.beginTransmission(AD5933_ADDR);
  Wire.write(address);
  Wire.write(value);
  int res = Wire.endTransmission();

  if (res) {
    return false;
  } else {
    return true;
  }
}

