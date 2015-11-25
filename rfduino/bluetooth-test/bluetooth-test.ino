/*
bluetooth-test
    This sketch demonstrates how to accept a BLE connection with an RFduino, and
    then receive data over the Bluetooth connection once every 5 seconds.
*/

#include <RFduinoBLE.h>

const char * data = ";)";

void setup() {
  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  RFduinoBLE.advertisementData = "test";

  // start the BLE stack
  RFduinoBLE.begin();
}

void loop() {
  // send once every 5 seconds
  RFduino_ULPDelay( SECONDS(5) );

  // send a test string
  RFduinoBLE.send(data, strlen(data));
}
