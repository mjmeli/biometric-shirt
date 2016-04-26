/**
  pcb-iteration-2
    All functionality to make sure Iteration 1 of our PCB is functional.
    - Reads temperature values from DS18B20 thermometer via OneWire.
    - Reads impedance values from the AD5933.
    - Sets digital potentiometers for calibration/reference
    - Manipulates digital switch to switch between resistor/electrode on AD5933
    - Measures battery voltage

    * Does NOT contain accelerometer code because the accelerometer had to be
      removed from the PCB.

    Programming: Connect GND, VCC, RST, GPIO0, GPIO1
 */

#include <RFduinoBLE.h>
#include <Wire.h>
#include <OneWire.h>
#include "FloatToString.h"
#include "AD5933.h"
#include "DS18B20.h"
#include "MCP4018.h"
#include "BiometricShirt.h"

// Create instance for OneWire
OneWire ds(TEMP_PIN);

// AD5933 On-board Calibration - not to be included in final design
double gain[NUM_INCR+1];
int phase[NUM_INCR+1];

// AD5933 Calibration Resistor Values
int realCalib[NUM_INCR+1];
int imagCalib[NUM_INCR+1];

// Timer step to track what we should do each iteration
unsigned int timer = 0;

// Value of the calibration resistor (predicted)
float calibrationResistorValue = CALIB_RESIST;

// Track whether bluetooth is connected
bool bluetoothConnected = false;
bool sendBluetooth = false;     // to prevent connections mid-sweep to send data
bool sentCalibrationValues = false; // for tracking sending calibration values

// Variable to denote when the app requests various commands
volatile int appCommands = 0;

void setup(void)
{
    // Begin bluetooth
    RFduinoBLE.advertisementData = "pcb-iteration-1";
    RFduinoBLE.begin();

    // Begin I2C
    Wire.begin();

    // Begin serial at 9600 baud for output
    Serial.begin(9600);
    Serial.println("PCB Iteration 1 Started!");

    // Set up pin GPIO3 for the digital switch and start measuring the
    // calibration resistor
    pinMode(IMP_MEASURE_SELECT_PIN, OUTPUT);
    switchImpedanceMeasurement(IMP_MEASURE_CALIBRATE);

    // Set up GPIO4 as LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Set the potentiometers to as close to 1k as possible. Get the predicted
    // resistance as well.
    int valueCode = MCP4018::getValueForResistance(1000);
    calibrationResistorValue = MCP4018::getResistanceForValue(valueCode);
    if (MCP4018::setValue(valueCode)) {
        Serial.println("Potentiometers set!");
    } else {
        Serial.println("FAILED in setting the potentiometers!");
    }

    Serial.println(calibrationResistorValue);

    // Set temperature resolution (default is 12 bit)
    if (DS18B20::setResolution(ds, RES_12BIT)) {
        Serial.println("Temperature resolution set!");
    } else {
        Serial.println("FAILED in setting temperature resolution!");
    }

    // Perform initial AD5933 configuration. Try again if any one of these fail.
    if (AD5933::reset() &&
        AD5933::setInternalClock(true) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1))
    {
        Serial.println("AD5933 initialized!");
    } else {
        Serial.println("FAILED in initialization!");
        RFduino_ULPDelay(1);
    }

    // Perform calibration sweep to populate calibration data arrays
    if (AD5933::calibrate(gain, phase, realCalib, imagCalib,
                          calibrationResistorValue, NUM_INCR+1))
    {
        Serial.println("Calibrated!");
    } else {
        Serial.println("FAILED in calibration!");
    }

    // Begin measuring the electrode
    switchImpedanceMeasurement(IMP_MEASURE_ELECTRODE);
}

void loop(void)
{
    // Toggle LED
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  
    // When a device first connects to the RFduino, the first impedance dataset
    // that should be sent is the calibration resistor values.
    if (bluetoothConnected && !sentCalibrationValues) {
        // Delay briefly. After initial connection, it may take a moment for
        // the actual data connection to establish.
        RFduino_ULPDelay(SECONDS(1));

        // Wait for the app to request the calibration resistor values
        while (appCommands != APP_CMD_CALIBRATION) {}

        // Send the calibration values
        sendCalibrationValues();
        sentCalibrationValues = true;
        appCommands = 0;
        Serial.println("Sent calibration resistor values");

        // Reset the timer so we instantly do a sweep
        timer = 0;
    }

    // Check if we have any commands to handle sent from the app
    if (appCommands > 0) {
        // If the app needs hydration information, do a sweep
        if (appCommands == APP_CMD_HYDRATION)
            measureImpedance();

        // Clear the command
        appCommands = 0;
    }

    // Check if the bluetooth connected and we need to start sending bluetooth
    // We check at the beginning of each loop to avoid sending data mid-sweep.
    sendBluetooth = bluetoothConnected;

    // Every 1 second, measure temperature
    if (timer % 1 == 0) {
        measureTemperature();
    }

    // Every 5 seconds, measure impedance values and battery voltage
    if (timer % 5 == 0) {
        measureImpedance();
        measureBatteryVoltage();
    }

    // Increment timer, reset every so often
    if (++timer >= 600) timer = 0;

    // Delay
    RFduino_ULPDelay( SECONDS(1) );
}

// Perform a temperature measurement and send the data
void measureTemperature() {
    // Get average temperature...add 1 to get body temperature
    float temp = DS18B20::getTemperature(ds);
    if (temp != 0.0) {
        temp += 1.0;    // only add 1 if a temperature was received
    }

    // Format temperature into string to send over Bluetooth
    char str[65];
    str[0] = 'T';
    str[1] = '$';
    fmtFloat(temp, 2, str+2, 63); // 2 decimals; sprintf sucks with arduino
    Serial.println(str);

    // Convert float to a string and send it over Bluetooth, if connected
    if (sendBluetooth) {
        RFduinoBLE.send(str, strlen(str));
    }
}

// Perform a battery voltage measurement and send the data
void measureBatteryVoltage() {
    // Configure ADC to read voltage level
    analogReference(VBG);     // sets the reference to 1.2 V band gap (internal)
    analogSelection(VDD_1_3_PS);  // set source to VDD with 1/3 prescaling

    RFduino_ULPDelay(100);    // needed to avoid artifacts

    // Read battery voltage value
    // MATH: value is out of 1023. So value/1023 is fractional part of 1.2 V.
    // However, we used a prescaler on VDD (3.6/3 = 1.2), so this is also
    // fractional part of 3.3 V. Therefore: value / 1023 = voltage / 3.6
    int sensorValue = analogRead(1);    // pin has no meaning...uses VDD
    float batteryVoltage = sensorValue * (3.6 / 1023.0);

    // Reset the analog reference and selection
    analogReference(DEFAULT);
    analogSelection(AIN_1_3_PS);    // analog inputs with 1/3 prescaling

    RFduino_ULPDelay(100);    // needed to avoid artifacts

    // Convert the battery voltage to a percent based on the LDO minimum
    if (batteryVoltage > 3.3) batteryVoltage = 3.3;
    int batteryPercentage = (int)(((float)batteryVoltage - LDO_MIN_VOLTAGE)/
                              ((float)BAT_MAX_VOLTAGE - LDO_MIN_VOLTAGE)*100.0);
    
    // Print to serial and perhaps send to app
    char str[65];
    sprintf(str, "B$%d", batteryPercentage);
    Serial.println(str);
    if (sendBluetooth) {
        RFduinoBLE.send(str, strlen(str));
    }
}

// Perform an impedance measurement and send the data
void measureImpedance() {
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

    // Send START command to app
    sprintf(str, "I$START$%d", NUM_INCR+1);
    Serial.println(str);
    if (sendBluetooth) {
        RFduinoBLE.send(str, strlen(str));
    }

    // Perform the actual sweep
    while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE) {
        // Get the frequency data for this frequency point
        if (!AD5933::getComplexData(&real, &imag)) {
            Serial.println("Could not get raw frequency data...");
        }

        // Print out the frequency data
        sprintf(str, "I$%d$%d$%d", cfreq, real, imag);
        Serial.print(str);
        if (sendBluetooth) {
            RFduinoBLE.send(str, strlen(str));
        }

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

    // Send HALT command
    sprintf(str, "I$HALT");
    Serial.println(str);
    if (sendBluetooth) {
        RFduinoBLE.send(str, strlen(str));
    }

    // Set AD5933 power mode to standby when finished
    if (!AD5933::setPowerMode(POWER_STANDBY))
        Serial.println("Could not set to standby...");
}

// Switch between measuring the calibration resistor or the electrode
bool switchImpedanceMeasurement(int option) {
    // Make the appropriate change
    switch (option) {
        case IMP_MEASURE_CALIBRATE:
            digitalWrite(IMP_MEASURE_SELECT_PIN, LOW);
            return true;
        case IMP_MEASURE_ELECTRODE:
            digitalWrite(IMP_MEASURE_SELECT_PIN, HIGH);
            return true;
        default:
            return false;
    }
}

// Send calibration values, generally for after a device first connects.
// Does NOT print to Serial, only send to Bluetooth, if connected.
void sendCalibrationValues() {
    // Make sure Bluetooth is connected
    if (!bluetoothConnected) return;

    // Character array to hold data to print
    char str[65];

    // Send START command
    sprintf(str, "I$START$%d", NUM_INCR+1);
    RFduinoBLE.send(str, strlen(str));

    // Iterate over the stored arrays of calibration resistor data.
    int cfreq = START_FREQ/1000;
    for (int i = 0; i < NUM_INCR+1; i++) {
        // Pull data from calibration resistor arrays and send
        sprintf(str, "I$%d$%d$%d", cfreq, realCalib[i], imagCalib[i]);
        RFduinoBLE.send(str, strlen(str));

        // Increment current frequency
        cfreq += FREQ_INCR/1000;

        // Arduino has a limit on the data transmission rate. To avoid dropping
        // data, throttle the transmission slightly with a delay.
        RFduino_ULPDelay(50);
    }

    // Send HALT command
    sprintf(str, "I$HALT");
    RFduinoBLE.send(str, strlen(str));
}

// Callback for when we connect to a device
void RFduinoBLE_onConnect(){
    bluetoothConnected = true;
    sentCalibrationValues = false;
    sendBluetooth = false;
    Serial.println("Bluetooth connection established!");
}

// Callback for when we disconnect from a device
void RFduinoBLE_onDisconnect(){
    bluetoothConnected = false;
    sendBluetooth = false;
    sentCalibrationValues = false;
    Serial.println("Bluetooth connection lost...");
}

// Callback for when we receive data.
void RFduinoBLE_onReceive(char *data, int len) {
    // Serial print the command received
    Serial.println("Bluetooth data received");
    for (int i = 0; i < len; i++) {
      Serial.print(data[i]);
    }
    Serial.println();

    // Preliminarily the command
    if (strncmp(data, "calibration", len) == 0) {
        appCommands = APP_CMD_CALIBRATION;
        sentCalibrationValues = false;
    } else if (strncmp(data, "hydration", len) == 0) {
        appCommands = APP_CMD_HYDRATION;
    } else {
      Serial.println("Invalid command!");
    }
}
