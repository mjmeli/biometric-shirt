/*
temperature-test
    This sketch demonstrates reading a temperature over the temperature sensor.
    After properly connecting the temperature sensor (see below), this sketch
    will allow you to see temperature with the Serial Monitor.

    Connections:
        |-------------------|
        | Sensor  | RFduino |
        |-------------------|
        | RED     | +3V     |
        | WHITE   | GPIO 2  |
        | BLACK   | GND     |
        |-------------------|
        !!
        IMPORTANT: Also connect a 4.7k resistor between RED and WHITE as a
        pull-up resistor
        !!
*/

#include <OneWire.h>
#include "DS18B20.h"

// Data wire is plugged into pin 2 on the RFduino
#define TEMP_PIN 2

// Prepare a OneWire instance using our pin
OneWire ds(TEMP_PIN);

void setup(void)
{
    // Start serial communications
    Serial.begin(9600);
    Serial.println("Temperature Sensor Test");

    // Set proper resolution (default is 12 bit)
    DS18B20::setResolution(ds, RES_12BIT);
}

void loop(void)
{
    // Get temperature
    float temp = DS18B20::getTemperature(ds);

    // Print temperature to Serial Monitor if no error
    Serial.println(temp);

    // Delay. We can only get a reading once every 750ms at 12 bit resolution so no need to constantly read the same data.
    RFduino_ULPDelay( SECONDS(1) );
}

// Returns the temperature from the temperature sensor in degrees Fahrenheit.
// Returns -1000 if error.
float getTemp(void)
{
    byte data[12];  // data buffer
    byte addr[8];   // address buffer

    // Search for a new device on the bus. If 1, a new device is available. If 0,
    // the bus may be in error, nothing may be connected, or all devices have been
    // read. This should allow for multiple sensors on the same bus (must remove else block).
    // The address of the next device will be stored in addr.
    if (!ds.search(addr)) {
        ds.reset_search();  // start over
        return -1000;
    }
    else {
        ds.reset_search();
    }

    // Verify the CRC of the address
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return -1000;
    }

    // Verify the device is recognized by looking at the device ID in the first byte address (=0x28).
    if (addr[0] != 0x28) {
        Serial.println("Device not recognized!");
        return -1000;
    }

    /*
    * The code below changes the resolution of the temperature sensor. While this speeds
    * up conversion, it also reduces accuracy. I'm commenting this out until a time
    * restriction appears that makes using highest resolution an issue.
    *
    // If this is the first time seeing an address, set the conversion resolution to 9 bits
    if (!set) {
    ds.reset();
    ds.select(addr);
    ds.write(0x4E); // 0x4E = write scratchpad
    ds.write(0x00); // alarm low setting = 0 (not used)
    ds.write(0x00); // alarm high setting = 0 (not used)
    ds.write(0x00); // resolution = 00 (9 bit)
    }*/

    // Begin a temperature conversion
    ds.reset();
    ds.select(addr);
    ds.write(0x44);   // 0x44 = start conversion

    // Hard delay to get the most up-to-date reading. This gives the most time accurate reading
    // but locks the processor up from doing anything else. The existing method where we read
    // the *previous* reading every 1 second is suitable and increases utilization.
    //delay(1000);

    // Request to read the temperature sensor's scratchpad for the converted temperature
    ds.reset();
    ds.select(addr);
    ds.write(0xBE);   // 0xBE = read scratchpad

    // Read data (9 bytes total in register, only the first two bytes contain the temperature)
    for (int i = 0; i < 9; i++) {
        data[i] = ds.read();
    }

    // Convert read data (16 bit signed integer) to a float
    int16_t temp_raw = (data[1] << 8) | data[0];
    float temp_celsius = (float)temp_raw/16.0;

    // Convert temperature to Fahrenheit and return
    float temp_fahrenheit = temp_celsius * 1.8 + 32.0;
    return temp_fahrenheit;
}
