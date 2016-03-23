#ifndef MCP4018_h
#define MCP4018_h

/**
 * Includes
 */
#include <Arduino.h>
#include <Wire.h>

/**
 * MCP4018 Register Map
 *  Datasheet p23
 */
// Device address
#define MCP4018_ADDR    (0x2F)

/**
 * Constants
 *  Constants for use with the MCP4018 library class.
 */
// Minimum and maximum positions
#define POT_MIN     (0x00)
#define POT_MAX     (0x7F)
// I2C result success/fail
#define I2C_RESULT_SUCCESS       (0)
#define I2C_RESULT_DATA_TOO_LONG (1)
#define I2C_RESULT_ADDR_NAK      (2)
#define I2C_RESULT_DATA_NAK      (3)
#define I2C_RESULT_OTHER_FAIL    (4)

/**
 * MCP4018 Library class
 *  Contains mainly functions for interfacing with the MCP4018.
 */
class MCP4018 {
    public:
        // Set value between 0 and 128
        static bool setValue(byte);
};

#endif
