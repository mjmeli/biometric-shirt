/*
   alpha-demo
    Reads temperature values from DS18B20 thermometer via OneWire.
    Reads impedance values from the AD5933.

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
      !!
      IMPORTANT: Also connect a 4.7k resistor between RED and GREEN as a
      pull-up resistor, plus resistors between RED and WHITE and RED and
      YELLOW as pull-ups on the I2C.
      !!
 */

#include <RFduinoBLE.h>
#include <Wire.h>
#include <OneWire.h>
#include "AD5933.h"
#include "DS18B20.h"
#include "FloatToString.h"

// Frequency sweep settings
#define START_FREQ  (80000)
#define FREQ_INCR   (1000)
#define NUM_INCR    (40)
#define REF_RESIST  (10000)

// Temperature sensor settings
#define TEMP_PIN    (2)

// Time measurement pin settings
#define TIME_PIN    (3)

// Create instance for OneWire
OneWire ds(TEMP_PIN);

// AD5933 Calibration
double gain[NUM_INCR+1];
int phase[NUM_INCR+1];

// Timer step to track what we should do each iteration
unsigned int timer = 0;

// Track whether bluetooth is connected
bool bluetoothConnected = false;

void setup(void)
{
    // Begin bluetooth
    RFduinoBLE.advertisementData = "alpha";
    RFduinoBLE.begin();

    // Set up pin GPIO1 as a way to measure active time
    pinMode(TIME_PIN, OUTPUT);
    digitalWrite(TIME_PIN, LOW);

    // Begin I2C
    Wire.begin();

    // Begin serial at 9600 baud for output
    Serial.begin(9600);
    Serial.println("AD5933 Test Started!");

    // Set temperature resolution (default is 12 bit)
    DS18B20::setResolution(ds, RES_12BIT);

    // Perform initial configuration. Fail if any one of these fail.
    if (!(AD5933::reset() &&
          AD5933::setInternalClock(true) &&
          AD5933::setStartFrequency(START_FREQ) &&
          AD5933::setIncrementFrequency(FREQ_INCR) &&
          AD5933::setNumberIncrements(NUM_INCR) &&
          AD5933::setPGAGain(PGA_GAIN_X1)))
    {
        Serial.println("FAILED in initialization!");
        RFduino_ULPDelay(INFINITE);
    }

    // Perform calibration sweep
    if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR+1))
        Serial.println("Calibrated!");
    else
        Serial.println("Calibration failed...");
}

void loop(void)
{
    digitalWrite(TIME_PIN, HIGH);
    
    // Every 1 second, read temperature
    if (timer % 1 == 0) {
        // Get average temperature...add 1 to get body temperature
        float temp = DS18B20::getTemperature(ds);
        if (temp != 0.0) {
            temp += 1.0;    // only add 1 if a temperature was received
        }
        Serial.println(temp);

        // Convert float to a string and send it over Bluetooth, if connected
        if (bluetoothConnected) {
            char str[65];
            fmtFloat(temp, 2, str, 65); // 2 decimal places
            RFduinoBLE.send(str, strlen(str));
        }
    }

    // Every 5 seconds, read impedance values
    if (timer % 5 == 0) {
        // Create variables to hold the impedance data and track frequency
        int real, imag, i = 0, cfreq = START_FREQ/1000;

        // Character array to hold data to print
        char str[65];

        // Initialize the frequency sweep
        if (!(AD5933::setPowerMode(POWER_STANDBY) &&          // place in standby
              AD5933::setControlMode(CTRL_INIT_START_FREQ) && // init start freq
              AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) // begin frequency sweep
             {
                 Serial.println("Could not initialize frequency sweep...");
             }

        // Perform the actual sweep
        while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE) {
            // Get the frequency data for this frequency point
            if (!AD5933::getComplexData(&real, &imag)) {
                Serial.println("Could not get raw frequency data...");
            }

            // Print out the frequency data
            sprintf(str, "%d: R=%d/I=%d", cfreq, real, imag);
            Serial.print(str);
            //RFduinoBLE.send(str, strlen(str));

            // Compute impedance
            double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
            double impedance = 1/(magnitude*gain[i]);
            Serial.print("  |Z|=");
            Serial.println(impedance);

            // Increment the frequency
            i++;
            cfreq += FREQ_INCR/1000;
            AD5933::setControlMode(CTRL_INCREMENT_FREQ);
        }

        // Set AD5933 power mode to standby when finished
        if (!AD5933::setPowerMode(POWER_STANDBY))
            Serial.println("Could not set to standby...");
    }

    digitalWrite(TIME_PIN, LOW);

    // Increment timer
    timer++;

    // Delay
    RFduino_ULPDelay( SECONDS(1) );
}

// Callback for when we connect to a device
void RFduinoBLE_onConnect(){
    bluetoothConnected = true;
    Serial.println("Bluetooth connection established!");
}

// Callback for when we disconnect from a device
void RFduinoBLE_onDisconnect(){
    bluetoothConnected = false;
    Serial.println("Bluetooth connection lost...");
}
