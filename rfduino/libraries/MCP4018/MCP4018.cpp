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

/**
 * Sets the value of the potentiometer wiper as close to a resistance
 * as possible. NOTE: This sets the resistance between pins A and W.
 *
 * @param val desired resistance between 0 and 100k (max resistance)
 * @return Success or failure
 */
bool MCP4018::setResistance(float val) {
    // Ensure the value is within the range
    if (val < 0 || val > MAX_RESISTANCE)
        return false;

    // Get value for the resistance
    byte value = getValueForResistance(val);
    if (value == -1)
        return false;

    // Set the value
    return setValue(value);
}

/**
 * Gets the value that will set the potentiometer as close to a resistance as
 * possible. This depends on the average measured wiper resistance. The wiper
 * resistance varies with voltage and temperature, so not perfectly accurrate.
 * NOTE: This gets the value for the resistance between pins A and W.
 *
 * @param val desired resistance between 0 and 100k (max resistance)
 * @return the value that can be used to set the potentiometer
 */
byte MCP4018::getValueForResistance(float val) {
    // Ensure the value is within the range
    if (val < 0 || val > MAX_RESISTANCE)
        return -1;

    // Determine how many steps are required to reach the resistance
    int numSteps;
    float resistance = WIPER_RESISTANCE;
    for (numSteps = 127; numSteps >= 0; numSteps--) {
        // Store the previous resistance for comparison later
        float prevResistance = resistance;

        // Add the step resistance
        resistance += STEP_RESISTANCE;

        // If the new resistance is over the desired, then compare the new one
        // and the old one to see which is closer.
        if (resistance >= val) {
            float upperError = resistance - val;
            float lowerError = val - resistance;
            if (upperError > lowerError) {
                // If upper is better, we take the current value of numSteps
                return numSteps;
            } else {
                // If lower is better, we take the previous value of numSteps
                return numSteps + 1;
            }
        }
    }
}

/**
 * Given a value that can be used to set the potentiometer, returns the expected
 * resistance. This depends on the average measured wiper resistance. The wiper
 * resistance varies with voltage and temperature, so not perfectly accurate.
 * NOTE: This gets the resistance for a value used to set between pins A and W.
 *
 * @param val value between 0 and 127 (inclusive) to set the resistance
 * @return the predicted resistance
 */
float MCP4018::getResistanceForValue(byte val) {
    // Ensure the value is within the range
    if (val < 0)
        return -1;

    // Determine how many steps are required to reach the resistance
    float resistance = WIPER_RESISTANCE;
    for (int i = 0; i < (127 - val); i++) {
        resistance += STEP_RESISTANCE;
    }
    return resistance;
}
