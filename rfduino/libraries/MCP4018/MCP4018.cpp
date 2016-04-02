/**
 * @file MCP4018.cpp
 * @brief Library code for MCP4018
 *
 * Library code for MCP4018.
 *
 * @author Michael Meli
 */

#include "MCP4018.h"

/**
 * Sets the value of the potentiometer wiper.
 *
 * @param val Value between 0 and 127 (inclusive) to set the resistance
 * @return Success or failure
 */
bool MCP4018::setValue(byte val) {
    // Ensure the value is within the range
    if (val < 0)
        return false;

    // Transmit the value to the MCP4018
    Wire.beginTransmission(MCP4018_ADDR);
    Wire.write(val);

    // Check that transmission completed successfully
    return Wire.endTransmission() == I2C_RESULT_SUCCESS;
}
