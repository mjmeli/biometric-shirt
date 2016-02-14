#ifndef AD5933_h
#define AD5933_h

/**
 * AD5933 Register Map
 *  Datasheet p23
 */
// Device address and address pointer
#define AD5933_ADDR     (0x0D)
#define ADDR_PTR        (0xB0)
// Control Register
#define CTRL_REG1       (0x80)
#define CTRL_REG2       (0x81)
// Start Frequency Register
#define START_FREQ_1    (0x82)
#define START_FREQ_2    (0x83)
#define START_FREQ_3    (0x84)
// Frequency increment register
#define FREQ_INC_1      (0x85)
#define FREQ_INC_2      (0x86)
#define FREQ_INC_3      (0x87)
// Number of increments register
#define NUM_INC_1       (0x88)
#define NUM_INC_2       (0x89)
// Number of settling time cycles register
#define NUM_SCYCLES_1   (0x8A)
#define NUM_SCYCLES_2   (0x8B)
// Status register
#define STATUS_REG      (0x8F)
// Temperature data register
#define TEMP_DATA_1     (0x92)
#define TEMP_DATA_2     (0x93)
// Real data register
#define REAL_DATA_1     (0x94)
#define REAL_DATA_2     (0x95)
// Imaginary data register
#define IMAG_DATA_1     (0x96)
#define IMAG_DATA_2     (0x97)

/**
 * Constants
 *  Constants for use with the AD5933 library class.
 */
// Temperature measuring
#define TEMP_MEASURE    (true)
#define TEMP_NO_MEASURE (false)
// Clock sources
#define CLOCK_EXTERNAL  (0x00)
#define CLOCK_INTERNAL  (0x01)
// PGA gain options
#define PGA_GAIN_X1     (0x00)
#define PGA_GAIN_X5     (0x01)
// Power modes
#define POWER_STANDBY   (0x00)
#define POWER_DOWN      (0x01)
#define POWER_ON        (0x02)

/**
 * AD5933 Library class
 *  Contains mainly functions for interfacing with the AD5933.
 */
class AD5933 {
    public:
        // Public data
        // None yet?

        // Reset the board
        bool reset();

        // Temperature measuring
        bool setTemperature(bool);
        double getTemperature();

        // Clock
        bool setClockSource(int);
        bool setSettlingCycles(int);

        // Frequency sweep configuration
        bool setStartFrequency(long);
        bool setIncrementFrequency(long);
        bool setNumberIncrements(int);

        // Gain configuration
        bool setPGAGain(int);

        // Status register
        int readStatusReg();

        // Impedance data
        bool getComplexData(&double, &double);

        // Power mode
        bool setPowerMode(int);

    private:
        // Private data
        // None yet?

        // Sending/Receiving byte method, for easy re-use
        int getByte(int);
        bool sendByte(int, int);
};

#endif
