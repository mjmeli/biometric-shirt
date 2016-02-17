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
#define REF_RESIST  (10000)

#define TIME_PIN    (3)

AD5933 ad5933;

double gain[NUM_INCR+1];
int phase[NUM_INCR+1];

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

  // Perform calibration sweep
  if (ad5933.calibrate(gain, phase, REF_RESIST, NUM_INCR+1))
    Serial.println("Calibrated!");
  else
    Serial.println("Calibration failed...");

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

  // Create arrays to hold the data
  int real[NUM_INCR+1], imag[NUM_INCR+1];

  // Perform the frequency sweep
  // NOTE: Do this and send data in real-time to reduce active time
  if (ad5933.frequencySweep(real, imag, NUM_INCR+1)) {
    // Print the frequency data
    int cfreq = START_FREQ/1000;
    for (int i = 0; i < NUM_INCR+1; i++, cfreq += FREQ_INCR/1000) {
      // Print raw frequency data
      Serial.print(cfreq);
      Serial.print(": R=");
      Serial.print(real[i]);
      Serial.print("/I=");
      Serial.print(imag[i]);

      // Compute impedance
      double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
      double impedance = 1/(magnitude*gain[i]);
      Serial.print("  |Z|=");
      Serial.println(impedance);
    }
    Serial.println("Frequency sweep complete!");
  } else {
    Serial.println("Frequency sweep failed...");
  }

  // Disable active timer
  digitalWrite(TIME_PIN, LOW);

  // Delay
  RFduino_ULPDelay( SECONDS(5) );
}
