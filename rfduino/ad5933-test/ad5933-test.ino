/*
ad5933-test
    Reads impedance values from the AD5933 over I2C and prints them serially.

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
#include "AD5933.h"

#define START_FREQ  (80000)
#define FREQ_INCR   (1000)
#define NUM_INCR    (40)
#define SWEEP_DELAY (1)

#define TIME_PIN    (3)

AD5933 ad5933;

void setup(void)
{
  // Set up pin GPIO1 as a way to measure active time
  pinMode(TIME_PIN, OUTPUT);
  digitalWrite(TIME_PIN, HIGH);
  
  // Begin I2C
  Wire.begin();
  
  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");

  // Perform initial configuration. Fail if any one of these fail.
  if (!(ad5933.reset() &&
        ad5933.setInternalClock(true) &&
        ad5933.setStartFrequency(START_FREQ) &&
        ad5933.setIncrementFrequency(FREQ_INCR) &&
        ad5933.setNumberIncrements(NUM_INCR) &&
        ad5933.setPGAGain(PGA_GAIN_X1)))
        {
            Serial.println("FAILED in initialization!");
            RFduino_ULPDelay(INFINITE);
        }

  // Disable active timer
  digitalWrite(TIME_PIN, LOW);
}

void loop(void)
{
  // Test - get temperature
  double temp = ad5933.getTemperature();
  Serial.println(temp);

  // Enable active timer
  digitalWrite(TIME_PIN, HIGH);

  // Perform frequency sweep
  int cfreq = START_FREQ/1000;
  ad5933.setPowerMode(POWER_STANDBY); // standby
  ad5933.setControlMode(CTRL_INIT_START_FREQ);
  RFduino_ULPDelay(SWEEP_DELAY);
  ad5933.setControlMode(CTRL_START_FREQ_SWEEP);
  RFduino_ULPDelay(SWEEP_DELAY);
  while (true) {
    int real, imag;
    if (ad5933.getComplexData(&real, &imag)) {
      Serial.print(cfreq);
      Serial.print(":R=");
      Serial.print(real);
      Serial.print("/I=");
      Serial.println(imag);
      cfreq += FREQ_INCR/1000;
    } else {
      Serial.println("Failed to get data");
      break;
    }
    if ((ad5933.readStatusRegister() & STATUS_FREQ_SWEEP_DONE) == STATUS_FREQ_SWEEP_DONE) {
      Serial.println("Frequency sweep completed");
      break;
    } else {
      ad5933.setControlMode(CTRL_INCREMENT_FREQ);
      RFduino_ULPDelay(SWEEP_DELAY);
    }
  }

  // Disable active timer
  digitalWrite(TIME_PIN, LOW);

  // Delay
  RFduino_ULPDelay( SECONDS(5) );
}
