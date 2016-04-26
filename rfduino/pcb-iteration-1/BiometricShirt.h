/**
 * Header file for biometric shirt.
 * Mainly contains function definitions and constants.
 */

#ifndef biometric_shirt_h
#define biometric_shirt_h

// Function definitions
void measureTemperature(void);
void measureImpedance(void);
void measureBatteryVoltage(void);
bool switchImpedanceMeasurement(int);
void sendCalibrationValues(void);

// Frequency sweep settings
#define START_FREQ      (80000)
#define FREQ_INCR       (1000)
#define NUM_INCR        (40)
#define CALIB_RESIST    (1000)

// Minimum operating voltage for the LDO
#define LDO_MIN_VOLTAGE (2.1)
#define BAT_MAX_VOLTAGE (3.3)

// Pin for temperature sensor
#define TEMP_PIN    (2)

// Pins for digital switch
#define CALIBRATE_PIN (3)
#define ELECTRODE_PIN (4)

// App commands
#define APP_CMD_CALIBRATION (0x01)
#define APP_CMD_HYDRATION   (0x02)

#endif
