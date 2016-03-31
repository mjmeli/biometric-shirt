/**
 Battery Voltage
    Measures the current (battery) voltage powering the RFduino.
*/

#include <RFduinoBLE.h>

void setup(void)
{
  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("MCP4018 Test Started!");

  // Setup voltage referencing by using the VBG as analog reference
  analogReference(VBG);         // sets the reference to 1.2 V band gap (internal)
  analogSelection(VDD_1_3_PS);  // selects VDD with 1/3 prescaling as analog source
                                // this scales [0, 3.6] --> [0, 1.2] as [0, 1023]
}

void loop(void)
{
  // Read the analog reference value
  int sensorValue = analogRead(1);  // pin has no meaning...uses VDD pin
  float batteryVoltage = sensorValue * (3.6 / 1023.0);  // convert value to voltage
  // MATH: value is out of 1023. So value/1023 is fractional part of 1.2 V. However,
  // we used a prescaler on VDD (3.6/3 = 1.2), so this is also fractional part of 3.3 V.
  // Therefore: value / 1023 = voltage / 3.6

  // Print battery voltage
  Serial.println(batteryVoltage);

  RFduino_ULPDelay( SECONDS(1) );
}
