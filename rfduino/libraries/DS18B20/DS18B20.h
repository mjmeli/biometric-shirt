#ifndef DS18B20_h
#define DS18B20_h

/**
 * Includes
 */
#include <OneWire.h>

/**
 * DS18B20 Function Commands
 *  p11-12 Datasheet
 */
#define CMD_CONVERT_TEMP    (0x44)
#define CMD_WRITE_SPAD      (0x4E)
#define CMD_READ_SPAD       (0xBE)
#define CMD_COPY_SPAD       (0x48)
#define CMD_RECALL_E2       (0xB8)
#define CMD_READ_PWR        (0xB4)

/**
 * Constants
 *  Constants for use with the DS18B20 library class.
 */
// Device ROM Code
#define DS18B20_CODE        (0x28)
// Thermometer resolutions
#define RES_9BIT            (0b00011111)
#define RES_10BIT           (0b00111111)
#define RES_11BIT           (0b01011111)
#define RES_12BIT           (0b01111111)
// Alarm codes
#define ALARM_DISABLED      (0x00)

/**
 * DS18B20 Library class
 *  Contains mainly functions for interfacing with the DS18B20 thermometers.
 */
class DS18B20 {
    public:
         static float getTemperature(OneWire);
         static bool setResolution(OneWire, byte);
};

#endif
