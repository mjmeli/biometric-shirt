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
#define IMP_MEASURE_SELECT_PIN (3)

// Pin for LED
#define LED_PIN     (4)

// Digital switch options
#define IMP_MEASURE_CALIBRATE   (0)
#define IMP_MEASURE_ELECTRODE   (1)

// App commands
#define APP_CMD_CALIBRATION (0x01)
#define APP_CMD_HYDRATION   (0x02)

#endif

