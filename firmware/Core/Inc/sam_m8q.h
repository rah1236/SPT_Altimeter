/*
 * sam_m8q.h
 *
 *  Created on: Jan 20, 2025
 *      Author: raheel
 */

#ifndef INC_SAM_M8Q_H_
#define INC_SAM_M8Q_H_



#endif /* INC_SAM_M8Q_H_ */
#ifndef SAM_M8Q_H
#define SAM_M8Q_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

// GPS data structure
typedef struct {
    // Time information
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    // Date information
    uint8_t day;
    uint8_t month;
    uint16_t year;

    // Position information
    float latitude;
    char lat_direction;     // N or S
    float longitude;
    char lon_direction;     // E or W

    // Additional data
    float altitude;         // Meters above sea level
    float speed;           // Speed in knots
    int satellites;        // Number of satellites in use
    bool fix_valid;        // Whether we have a valid GPS fix
    float hdop;            // Horizontal dilution of precision
} sam_m8q_data_t;

// Initialize GPS module via UART
bool sam_m8q_init_uart(UART_HandleTypeDef *huart);

// Initialize GPS module via I2C
bool sam_m8q_init_i2c(I2C_HandleTypeDef *hi2c);

// Get latest GPS data
bool sam_m8q_get_data(sam_m8q_data_t *data, bool force_copy);
// Put GPS in power save mode
bool sam_m8q_sleep(void);

// Wake GPS from power save mode
bool sam_m8q_wake(void);

// Check if new data is available
bool sam_m8q_data_ready(void);

// Process incoming UART data (call from UART IRQ)
void sam_m8q_uart_rx_callback(uint8_t data);

void sam_m8q_set_debug(bool enable);

#endif // SAM_M8Q_H
