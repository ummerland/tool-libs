/** @file MAX31855.h
 *
 * Copyright (c) 2019 IACE
 */
#ifndef MAX31855_H
#define MAX31855_H

#include "stm/spi.h"

class MAX31855 : public ChipSelect {
public:
    /**
     * Constructor
     * @param pin GPIO pin of chip select line
     * @param port GPIO port of chip select line
     */
    MAX31855(uint32_t pin, GPIO_TypeDef *port) : ChipSelect(pin, port) { }

    /**
     * return currently measured temperature
     * @return temperature in °C
     */
    double temp() {
        return sTemp;
    }

    /**
     * return currently measured internal temperature of chip
     * @return temperature in °C
     */
    double internal() {
        return iTemp;
    }

    /**
     * request measurement of sensor data
     */
    void sense() {
        SPIRequest r = {this, SPIRequest::MISO, nullptr, buffer, sizeof(buffer), 0};
        HardwareSPI::master()->request(r);
    }

    /**
     * callback when data is successfully measured
     *
     * copy data from buffer into struct.
     */
    void callback(uint8_t cbData) override {
        bitwisecopy((uint8_t *)&sensorData, 32, sizeof(sensorData), buffer, 1);
        sTemp = sensorData.TEMP * 0.25;
        iTemp =  sensorData.INTERNAL * 0.0625;
    }

private:
    struct __packed {
        uint8_t OC:1;                               ///< \b error: open circuit
        uint8_t SCG:1;                              ///< \b error: short to GND
        uint8_t SCV:1;                              ///< \b error: short to VCC
        //\cond false
        uint8_t :1;
        //\endcond
        uint16_t INTERNAL:12;                       ///< internal measured temperature in 0.0625°C steps
        uint8_t FAULT:1;                            ///< \b error: fault in thermocouple reading
        //\cond false
        uint8_t :1;
        //\endcond
        uint16_t TEMP:14;                           ///< thermocouple measured temperature in 0.25°C steps
    } sensorData = {};                              ///< sensor data struct. lsb to msb order.
    uint8_t buffer[4] = {};
    double sTemp = 0;
    double iTemp = 0;
};

#endif //MAX31855_H
